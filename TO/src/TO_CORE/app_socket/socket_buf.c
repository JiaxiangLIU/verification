#include <string.h>
#include "socket_buf.h"
#include "error_code.h"
#include "log.h"
#include "app_socket.h"

socket_buf_t socket_realtime_buf = {PTHREAD_MUTEX_INITIALIZER, 0, 0};


int write_realtime_buf(unsigned char type, char *data, int length)
{
	int len = 0;
	int space_len;
	int head;
	
	if (length > SOCKET_REALTIME_LEN) {
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return ERROR_CODE_PARAMETER;
	} else
		len = length;
		
	pthread_mutex_lock(&socket_realtime_buf.mutex);
	
	space_len = SOCKET_SPACE(socket_realtime_buf.head, socket_realtime_buf.tail, REALTIME_BUF_LEN);
	
	if (space_len > 0) {
		socket_realtime_buf.buf[socket_realtime_buf.head].type = type;
		memcpy(socket_realtime_buf.buf[socket_realtime_buf.head].data, data, len);
		socket_realtime_buf.buf[socket_realtime_buf.head].len = len;
		socket_realtime_buf.head = (socket_realtime_buf.head + 1) & (REALTIME_BUF_LEN - 1);
	} else {
		head = (socket_realtime_buf.head - 1) & (REALTIME_BUF_LEN - 1) ;
		socket_realtime_buf.buf[head].type = type;
		memcpy(socket_realtime_buf.buf[head].data, data, len);
		socket_realtime_buf.buf[head].len = len;
	}
	
	pthread_mutex_unlock(&socket_realtime_buf.mutex);
	
	return 0;
}

int send_realtime_buf(void)
{
	int ret, cnt_len, fd, send_len = 0, i = 0;
	unsigned char send_type;
	char send_buf[SOCKET_REALTIME_LEN];
	
	pthread_mutex_lock(&socket_realtime_buf.mutex);

	cnt_len = SOCKET_CNT(socket_realtime_buf.head, socket_realtime_buf.tail, REALTIME_BUF_LEN);
	
	if (cnt_len > 0) {
		if (socket_realtime_buf.buf[socket_realtime_buf.tail].len > SOCKET_REALTIME_LEN) {
			socket_realtime_buf.tail = (socket_realtime_buf.tail + 1) & (REALTIME_BUF_LEN - 1);
			pthread_mutex_unlock(&socket_realtime_buf.mutex);
			log_warning("send_realtime_buf warning: %s\n", error_code_name(ERROR_CODE_DATA_TOO_LONG));
			return ERROR_CODE_DATA_TOO_LONG;
		}
		memcpy(send_buf, socket_realtime_buf.buf[socket_realtime_buf.tail].data, socket_realtime_buf.buf[socket_realtime_buf.tail].len);
		send_len = socket_realtime_buf.buf[socket_realtime_buf.tail].len;
		send_type = socket_realtime_buf.buf[socket_realtime_buf.tail].type;
		socket_realtime_buf.tail = (socket_realtime_buf.tail + 1) & (REALTIME_BUF_LEN - 1);
		pthread_mutex_unlock(&socket_realtime_buf.mutex);
	} else {
		pthread_mutex_unlock(&socket_realtime_buf.mutex);
		return 0;
	}

	if (send_len > 0 && send_len <= SOCKET_REALTIME_LEN) {
		fd = get_loc_socket_by_type(send_type);
		if (fd < 0) {
			log_warning("send_realtime_buf warning: %s\n", error_code_name(ERROR_CODE_FD_PARAMETER));
			return ERROR_CODE_FD_PARAMETER;
		}

		while (i < send_len) {
			ret = write(fd, &send_buf[i], send_len - i);
			if (ret < 0) {
				log_warning("send_realtime_buf warning: %s\n", error_code_name(ERROR_CODE_SOCKET_SEND));
				return ERROR_CODE_SOCKET_SEND;
			}
			i += ret;
		}
	}
	
	return 0;
}


