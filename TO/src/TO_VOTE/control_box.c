#include <errno.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <netinet/in.h>
#include <string.h>
#include "control_box.h"
#include "crc16.h"
#include "app_socket.h"
#include "global.h"
#include "log.h"


#define CIRC_CNT(head, tail, size)    (((head) - (tail)) & ((size)-1))
#define CIRC_SPACE(head, tail, size)  CIRC_CNT((tail),((head)+1),(size))
#define CIRC_LEN 4096
static cb_sent_buf_t temp_sent_buf = {PTHREAD_MUTEX_INITIALIZER};

struct circ_buf {
	pthread_mutex_t mutex;
	int head;
	int tail;
	char data[CIRC_LEN];
};

static struct circ_buf send_buf = {PTHREAD_MUTEX_INITIALIZER};

/*
 * jiaxiang: copy _len_ bytes from _value_ to _buf_
 */
static int write_circ_buf(struct circ_buf *buf, char *value, int len)
{
	int i, space_len, ret = -1;

	if (len < 0)
		return ret;

	pthread_mutex_lock(&buf->mutex);
	
	space_len = CIRC_SPACE(buf->head, buf->tail, CIRC_LEN);

	if (space_len >= len) {
		for (i = 0; i < len; i++) {
			buf->data[buf->head] = value[i];
			buf->head = (buf->head + 1) & (CIRC_LEN - 1);
		}
		ret = len;
	}

	pthread_mutex_unlock(&buf->mutex);
	return ret;
}

/*
 * jiaxiang: copy _len_ bytes from _buf_ to _value_
 */
static int read_circ_buf(struct circ_buf *buf, char *value, int len)
{
	int i, data_len, ret = -1;

	if (len < 0)
		return ret;

	pthread_mutex_lock(&buf->mutex);

	data_len = CIRC_CNT(buf->head, buf->tail, CIRC_LEN);

	if (data_len <= 0) {
		pthread_mutex_unlock(&buf->mutex);
		return ret;
	}

	if (data_len >= len) {
		for (i = 0; i < len; i++) {
			value[i] = buf->data[buf->tail];
			buf->tail = (buf->tail + 1) & (CIRC_LEN - 1);
		}
		ret = len;
	} else if (data_len < len) {
		for (i = 0; i < data_len; i++) {
			value[i] = buf->data[buf->tail];
			buf->tail = (buf->tail + 1) & (CIRC_LEN - 1);
		}
		ret = data_len;
	}

	pthread_mutex_unlock(&buf->mutex);
	
	return ret;
}


static cb_sent_buf_t cb_sent_buf = {PTHREAD_MUTEX_INITIALIZER};

static void control_box_recv_handler(int socket_fd, unsigned char *data, int data_len)
{
}

/*
 * jiaxiang: 将_buf_中的数据通过_socket_fd_发送出去
 */
static int control_box_sent_handler(int socket_fd, cb_sent_buf_t *buf)
{
	int bytes, i;
	
	bytes = 0;
	i = 0;
	
	if (buf == NULL || socket_fd <= 0) {
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return -ERROR_CODE_PARAMETER;
	}
	pthread_mutex_lock(&buf->mutex);
	
	if (buf->len > 0 && buf->len <= PACKET_MAX_LEN) {
		while (i < buf->len) {
			bytes = write(socket_fd, &buf->data[i], buf->len - i);
			log_info("send bytes: %d\n", bytes);
			if (bytes < 0) {
				pthread_mutex_unlock(&buf->mutex);
				log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_SOCKET_SEND));
				return ERROR_CODE_SOCKET_SEND;
			}
			i += bytes;
			//buf->len -= bytes;
		}

	}
	buf->len = 0;
	
	pthread_mutex_unlock(&buf->mutex);
	
	return 0;
}

static int tcp_receive(
	int socket_fd,
	unsigned char *data,
	unsigned short data_len,
	unsigned int timeout)
{
    int received_bytes = 0;/* return value */
    int max = 0;
    fd_set read_fds;
    struct timeval select_timeout;

    /* Make sure the socket is open */
    if (socket_fd < 0)
        return 0;
    /* we could just use a non-blocking socket, but that consumes all
       the CPU time.  We can use a timeout; it is only supported as
       a select. */
    if (timeout >= 1000) {
        select_timeout.tv_sec = timeout / 1000;
        select_timeout.tv_usec = 1000 * (timeout - select_timeout.tv_sec * 1000);
    } else {
        select_timeout.tv_sec = 0;
        select_timeout.tv_usec = 1000 * timeout;
    }
    FD_ZERO(&read_fds);
    FD_SET(socket_fd, &read_fds);
    max = socket_fd + 1;
    /* see if there is a packet for us */
    if (select(max, &read_fds, NULL, NULL, &select_timeout) > 0) {
    	if (data_len > PACKET_MAX_LEN)
    		data_len = PACKET_MAX_LEN;
    	if (FD_ISSET(socket_fd, &read_fds)) {
        	received_bytes = recv(socket_fd, (unsigned char *) &data[0], data_len, 0);
        	if (received_bytes < 0) {
				//perror("tcp rec -1 error");
				log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_RECV_TIMEOUT));
				if ((errno == ECONNRESET) || (errno == ETIMEDOUT)) {
//					tcp_set_socket(-1);
//					close(socket_fd);
					log_error("errno is %d\n", errno);
				}
//				tcp_set_socket(-1);
				close(socket_fd);
				received_bytes = 0;
			} else if (received_bytes == 0) {
				//perror("tcp rec 0 error");
				log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_SOCKET_RECV));
				if ((errno == ENOENT) || (errno == EBADF) || (errno == EAGAIN)) {
//					tcp_set_socket(-1);
//					close(socket_fd);
					log_error("errno is %d\n", errno);
				}
//				tcp_set_socket(-1);
				close(socket_fd);
			}
		}
    }
	return received_bytes;
}

static int control_box_connect(char *ipaddr, int port)
{
	int sock;
	struct sockaddr_in serverAddr;

	if (ipaddr == NULL) {
		//fprintf(stderr, "ipaddr == NULL\n");
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return ERROR_CODE_PARAMETER;
	}
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		//perror("socket");
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_SOCKET_CONNECT));
		return ERROR_CODE_SOCKET_CONNECT;
	}
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ipaddr);//ipadaddrΪ�������˵�ip��ַ

	if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
		//perror("connect");
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_SOCKET_CONNECT));
		close(sock);
		return ERROR_CODE_SOCKET_CONNECT;
	}

	return sock;
}


void *control_box_thread(void *arg)
{
	log_info("access control_box thread.\n");

	int socket_fd, port;//comm��cli��can2net������socket��ʶ��
	int data_len, i;
	unsigned char data[PACKET_MAX_LEN];
	char *ip;
	uint16_t type;
	cb_sent_buf_t *cb = &cb_sent_buf;
	port = PORT_CAN2NET; // jiaxiang: 端口指向本机负责与控制盒通信的can2net程序
	ip = "127.0.0.1";

	
	socket_fd = control_box_connect(ip, port);//��can2net����socket
	if (socket_fd < 0) {
		//fprintf(stderr, "Socket with can2net error!\n");
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_FD_PARAMETER));
		return 0;
	}
	
	/*
	 * jiaxiang: 将与CAN-CTRL连接的socket放入socket数组loc_sockets
	 */
	set_loc_socket_by_type(SOCKET_TYPE_CAN2NET, socket_fd);
	type = SOCKET_TYPE_CAN2NET;

	int ret;

	/*
	 * jiaxiang: 在线程池中新启动线程运行socket_rcv负责监听处理类型为
	 * SOCKET_TYPE_CAN2NET的事件
	 */
    ret = threadpool_add(pool, socket_rcv, (void *)&type, 0);    
    if (ret != 0)
    {
        //fprintf(stderr, "failed to create socket_rcv.\n");
        log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_OTHER));
        exit(0);
    }


	/*
	 * jiaxiang: 本线程开始负责把缓冲区里的数据发送到CAN2NET对应的socket中
	 */
	while (1) {
		/*
		 * jiaxiang: 将数据从缓冲区send_buf复制到temp_sent_buf，并将temp_sent_buf通过
		 * 函数control_box_sent_handle发送到socket_fd（CAN2NET）中。
		 */
		data_len = read_circ_buf(&send_buf, temp_sent_buf.data, PACKET_MAX_LEN);
		if (data_len > 0) {
			temp_sent_buf.len = data_len;
			control_box_sent_handler(socket_fd, &temp_sent_buf);
		}
		/*data_len = tcp_receive(socket_fd, data, PACKET_MAX_LEN, 10);
		if (data_len > 0) {
			control_box_recv_handler(socket_fd, data, data_len);
		}*/
		//control_box_sent_handler(socket_fd, cb);
	}
}

/*
 * jiaxiang: 将数据_data_发送到全局环形缓冲区send_buf中
 * （真正通过socket发送是在control_box_thread中利用control_box_sent_handler
 * 进行）
 */
int control_box_send(int canid, unsigned char *data, int len)
{
	int frame_total, send_len, zero_len;
	int i;
	unsigned short crc;
	
	if (data == NULL || len + 2 + 8 > PACKET_MAX_LEN) {
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return ERROR_CODE_PARAMETER;
	}
	canid = 0xaaaaaaaa;
	if ((len + 2) % 8 != 0)
		frame_total = (len + 2 + 8) / 8 + 1;
	else
		frame_total = (len + 2 + 8) / 8;
	send_len = frame_total * 8;

	zero_len = send_len - len - 8 - 2;
	
	pthread_mutex_lock(&cb_sent_buf.mutex);

	for (i = 0; i < 4; i++)
		cb_sent_buf.data[i] = (unsigned char)((send_len - 4) >> (8 * i) & 0x000000ff);

	for (i = 0; i < 4; i++)
		cb_sent_buf.data[i + 4] = (unsigned char)(canid >> (8 * i) & 0x000000ff);
	
	memcpy(&cb_sent_buf.data[8], data, len);

	for (i = 0; i < zero_len; i++)
		cb_sent_buf.data[len + 8 + i] = 0;

	crc = crc16(&cb_sent_buf.data[8], send_len - 2 - 8);

	cb_sent_buf.data[send_len - 1] = (unsigned char)(crc & 0x00ff); //crc low
	cb_sent_buf.data[send_len - 2] = (unsigned char)(crc >> 8); //crc high
	
	cb_sent_buf.len = send_len;

	memmove(&cb_sent_buf.data[8], cb_sent_buf.data, send_len);
	send_len += 8;

	for (i = 0; i < 4; i++)
		cb_sent_buf.data[i] = (unsigned char)((send_len - 4) >> (8 * i) & 0x000000ff);

	for (i = 0; i < 4; i++)
		cb_sent_buf.data[i + 4] = (unsigned char)(canid >> (8 * i) & 0x000000ff);

	cb_sent_buf.len = send_len;

	write_circ_buf(&send_buf, cb_sent_buf.data, cb_sent_buf.len);

	pthread_mutex_unlock(&cb_sent_buf.mutex);
#if 0
	char print_char;
	int count_i;
	printf("control box send data^^^^^^^^^^^^^^\n");
	for(count_i=0;count_i<send_len;count_i++)
		printf("%2x ",cb_sent_buf.data[count_i]);
	printf("\n");
#endif
	return 1;
}

