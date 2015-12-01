
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "app_socket.h"
#include "protocal.h"
#include "control_box.h"
#include "error_code.h"
#include "log.h"

#define MIN_PACKE_LEN 7
#define CRC_LEN 2
#define MIN_PACKE_CMD (9 + 5 + CRC_LEN) //命令帧 + 5个0 + 两个CRC
#define MIN_PACKE_DATA (14 + 0 + CRC_LEN)
#define MIN_PACKE_ACK (8 + 6 + CRC_LEN)
#define SOCKET_ADDR_LEN 3

/* Table of CRC values for high-order byte */
static const unsigned char table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const unsigned char table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

typedef enum {
    SOCKET_HEADR_INDEX = 0,
    SOCKET_TYPE_INDEX = 1,
	DATA_LEN_HIGH_INDEX = 10,
	DATA_LEN_LOW_INDEX = 11
} SOCKET_PROTOCAL;

unsigned short crc16(unsigned char *buffer, int buffer_length)
{
    unsigned char crc_hi = 0xFF; /* high CRC byte initialized */
    unsigned char crc_lo = 0xFF; /* low CRC byte initialized */
    unsigned int i; /* will index into CRC lookup */

    /* pass through message buffer */
    while (buffer_length--) {
        i = crc_hi ^ *buffer++; /* calculate the CRC  */
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}


static int recv_check(char *dest_data, int *dest_len, char *src_data, int src_len)
{
	int handle_len = 0; //处理过的长度
	unsigned short crc_value;
	unsigned short recv_crc;
	int recv_len, new_len, i;
	*dest_len = ERROR_CODE_PARAMETER;

    if (dest_data == NULL || src_data == NULL || MIN_PACKE_LEN > src_len) {
        //fprintf(stderr, "frame err: illegal input.\n");
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
        return 0;
    }

    if (src_data[SOCKET_HEADR_INDEX] != 0x5a) { //协议头
        //fprintf(stderr, "frame err: frame err.\n");
        log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PROTOCOL_HEADER));
		/* XXX: 直接把数据全不丢弃，可能会有问题 */
		*dest_len = ERROR_CODE_PROTOCOL_HEADER;
        return src_len;
    }

    switch (src_data[SOCKET_TYPE_INDEX]) {
        case COMMAND_FRAME_CODE: //命令帧解析
			if (src_len < MIN_PACKE_CMD)
				return 0;
			crc_value = crc16(src_data, MIN_PACKE_CMD - CRC_LEN);
			recv_crc = (unsigned short)src_data[MIN_PACKE_CMD - 2]; //crc high
			recv_crc = recv_crc << 8 | (unsigned short)src_data[MIN_PACKE_CMD - 1]; //low high
			if (crc_value == recv_crc) {//crc 正确
				memcpy(dest_data, src_data, MIN_PACKE_CMD - 5 - CRC_LEN);
				*dest_len = MIN_PACKE_CMD - CRC_LEN - 5;
				handle_len = MIN_PACKE_CMD;
			} else {
				/* XXX: 直接把数据全不丢弃，可能会有问题 */
				//fprintf(stderr, "crc command err.\n");
				log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_CRC));
				handle_len = src_len;
				*dest_len = ERROR_CODE_CRC;
			}
            break;
        case DATA_FRAME_CODE:   //数据帧
			if (src_len < MIN_PACKE_DATA)
				return 0;
			recv_len = (unsigned short)src_data[DATA_LEN_HIGH_INDEX];
			recv_len = recv_len << 8 | (unsigned short)src_data[DATA_LEN_LOW_INDEX];
			if (recv_len + 13 > src_len)//可宏定义
				return 0;
			if (recv_len + 13 > 2048) {
				/* XXX: 直接把数据全不丢弃，可能会有问题 */
				//fprintf(stderr, "crc data err.\n");
				log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_DATA_TOO_LONG));
				*dest_len = ERROR_CODE_DATA_TOO_LONG;
				return src_len;
			}

			i = (recv_len + 13 + CRC_LEN) % 8;
			if (i == 0)
				new_len = recv_len + 13 + CRC_LEN;
			else
				new_len = ((recv_len + 13 + CRC_LEN) / 8 + 1) * 8;

			crc_value = crc16(src_data, new_len - CRC_LEN);
			recv_crc = (unsigned short)src_data[new_len - 2]; //crc high
			recv_crc = recv_crc << 8 | (unsigned short)src_data[new_len - 1]; //low high
			if (crc_value == recv_crc) {//crc 正确
				memcpy(dest_data, src_data, recv_len + 13);
				*dest_len = recv_len + 13;
				handle_len = new_len;
			} else {
				/* XXX: 直接把数据全不丢弃，可能会有问题 */
				handle_len = src_len;
				*dest_len = ERROR_CODE_CRC;
				log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_CRC));
			}
            break;
        default:
            break;
    }
    return handle_len;
}

//added

/** 
 * 板子地址属性 ip地址和端口
 **/ 
struct board_addr_t
{
    uint16_t    port;
    const char *host_addr;
};

static struct board_addr_t board_addr;

struct socket_t
{
    int      socket_fd;
    uint8_t  socket_type;   
};

static struct socket_t loc_sockets[MAX_SOCKET_NUM]; //存放socket_fd及对应优先级

/**
 * @function socket_err_handle
 * @brief 处理socket error
 * @param err_code 错误码
 * @param arg 可能需要的参数
 * @return void
 **/ 
static void socket_err_handle(int err_code, void *arg);

int set_board_addr(int board)
{
    switch(board)
    {
        case 0:
            board_addr.host_addr = COM_0_ADDR;
            break;
        case 1:
            board_addr.host_addr = COM_1_ADDR;
            break;
        case 2:
            board_addr.host_addr = CORE_0_ADDR;
            break;
        case 3:
            board_addr.host_addr = CORE_1_ADDR;
            break;
        case 4:
            board_addr.host_addr = CORE_2_ADDR;
            break;
        case 5:
            board_addr.host_addr = CORE_3_ADDR;
            break;
        case 6:
            board_addr.host_addr = VOTE_0_ADDR;
            break;
        default:
			log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
            return ERROR_CODE_PARAMETER;
    }
    board_addr.host_addr = "127.0.0.1";
    board_addr.port = BOARD_PORT;
    return 0; 
}


/**
 * @function socket_register
 * @brief 注册socket优先级
 * @param socket_fd socket 文件描述符
 * @return 成功返回0，注册失败返回-2，socket通信失败返回-1
 **/ 
static int socket_register(int socket_fd, uint8_t type);

/**
 * @function set_loc_socket_by_type
 * @brief 通过type设置local_socket数组的值
 * @param type socket的类型
 * @return 成功返回0，未找到type返回-1
 **/ 
int set_loc_socket_by_type(uint8_t type, int socket_fd);

/**
 * @function get_loc_socket_by_type
 * @brief 通过type获取loc_socket
 * @param type socket类型
 * @return 成功返回socket的值，失败返回-1
 **/ 
static int get_loc_socket_by_type(uint8_t type);


SOCKET_RESULT socket_connect(uint8_t type)
{
    int s_fd;
    struct sockaddr_in sockaddr;
    int ret;
    log_info("socket connect.\n");
    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s_fd == -1)
    {
        //fprintf(stderr, "system err: failed to connect socket.\n");
        log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_FD_PARAMETER));
        return SOCKET_CONNECT_FAILURE;
    }
    
    if (board_addr.host_addr == NULL || board_addr.port == 0)
    {
        //fprintf(stderr, "board addr not initial.\n");
        log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PARAMETER));
        return INITIAL_FAILURE;
    }
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(board_addr.port);
    sockaddr.sin_addr.s_addr = inet_addr(board_addr.host_addr);
        
    ret = connect(s_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
    if (ret == -1)
    {
        //fprintf(stderr, "system err: failed to connect.\n");
        log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_SOCKET_CONNECT));
        return SOCKET_CONNECT_FAILURE;
    }
    
    ret = socket_register(s_fd, type);
    
    //注册失败 
    if (ret < 0)
    {
        //fprintf(stderr, "register err.\n");
        log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_SOCKET_REGISTER));
        close(s_fd);
        return REGISTER_FAILURE;
    }    
    log_info("log: socket connect success.s_fd: %d\n", s_fd);    
    ret = set_loc_socket_by_type(type, s_fd);
    
    log_info("register: soc type %d: %d\n", type, get_loc_socket_by_type(type)); 
    if(ret < 0)
    {
        //fprintf(stderr, "register err.\n");
        log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_OTHER));
        close(s_fd);
        return INITIAL_FAILURE;
    }
    
    return SOCKET_SUCCESS;        
}

static int socket_register(int socket_fd, uint8_t type)
{

    char snd_buf[3];
    char rcv_buf[3];
    int ret;

    snd_buf[0] = 0x5a;  //根据底层通信协议确定注册帧的格式和内容
    snd_buf[1] = 0x51;
    snd_buf[2] = type;
    ret = send(socket_fd, snd_buf, sizeof(snd_buf), 0);
    if (ret == -1) {
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_SOCKET_SEND));
        return ERROR_CODE_SOCKET_SEND;
    }
    ret = recv(socket_fd, rcv_buf, sizeof(rcv_buf), MSG_WAITALL);
    if (ret == -1) {
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_SOCKET_RECV));
        return ERROR_CODE_SOCKET_RECV;
    }
    if (rcv_buf[0] == 0x5a && rcv_buf[1] == 0x50 && rcv_buf[2] == (type + 3))   //正确返回值
        return 0;
    else  {  //注册失败
    	log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_SOCKET_REGISTER));
        return ERROR_CODE_SOCKET_REGISTER;
    }
}


int set_loc_socket_by_type(uint8_t type, int socket_fd)
{
    switch(type)
    {
        case SOCKET_TYPE_0:
           loc_sockets[0].socket_type = SOCKET_TYPE_0;
           loc_sockets[0].socket_fd = socket_fd;
           break;   
        
        case SOCKET_TYPE_1:
           loc_sockets[1].socket_type = SOCKET_TYPE_1;
           loc_sockets[1].socket_fd = socket_fd;
           break;
     
        case SOCKET_TYPE_2:
           loc_sockets[2].socket_type = SOCKET_TYPE_2;
           loc_sockets[2].socket_fd = socket_fd;
           break;
           
        case SOCKET_TYPE_3:
           loc_sockets[3].socket_type = SOCKET_TYPE_3;
           loc_sockets[3].socket_fd = socket_fd;
           break;

        case SOCKET_TYPE_4:
           loc_sockets[4].socket_type = SOCKET_TYPE_4;
           loc_sockets[4].socket_fd = socket_fd;
           break;

        case SOCKET_TYPE_5:
           loc_sockets[5].socket_type = SOCKET_TYPE_5;
           loc_sockets[5].socket_fd = socket_fd;
           break;

        case SOCKET_TYPE_CAN2NET:
        	loc_sockets[6].socket_type = SOCKET_TYPE_CAN2NET;
		   loc_sockets[6].socket_fd = socket_fd;
		   break;

        default:
		   log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PARAMETER));
           return ERROR_CODE_PARAMETER;
    }
    return 0;

}

static int get_loc_socket_by_type(uint8_t type)
{
    int ret;
    switch(type)
    {
        case SOCKET_TYPE_0:
           ret = loc_sockets[0].socket_fd;
           break;   
        
        case SOCKET_TYPE_1:
           ret = loc_sockets[1].socket_fd;
           break;
     
        case SOCKET_TYPE_2:
           ret = loc_sockets[2].socket_fd;
           break;

        case SOCKET_TYPE_3:
           ret = loc_sockets[3].socket_fd;
           break;

        case SOCKET_TYPE_4:
           ret = loc_sockets[4].socket_fd;
           break;

        case SOCKET_TYPE_5:
           ret = loc_sockets[5].socket_fd;
           break;

        case SOCKET_TYPE_CAN2NET:
		   ret = loc_sockets[6].socket_fd;
		   break;

        default:
		   log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PARAMETER));
           ret =  ERROR_CODE_PARAMETER;
           break;
    }
    return ret;
}

#if 0
void socket_rcv(void *arg)
{
    int soc;
    int s_fd;
    int ret;
    uint8_t type;
    char    rcv_buf[MAX_RCV_BUF_SIZE];

    PROTOCAL_RESULT p_ret;  //协议解析结果

    type = *((uint16_t *)arg);
    soc = get_loc_socket_by_type(type);
    printf("socekt_rcv.c, type: %d socket fd: %d\n", type, soc);
    if (soc == -1)
    {
        socket_err_handle(SOCKET_NOT_EXIST, NULL);
        return;
    }
    s_fd = soc;

    while(1)
    {
        ret = read(s_fd, rcv_buf, sizeof(rcv_buf));
#if 0
        printf("recv buf: ");
        int _i;
        for(_i = 0; _i < ret; _i++) {
            printf("%x  ", rcv_buf[_i]);
        }
        printf("\n");
#endif
        if (-1 == ret)
        {
            socket_err_handle(RCV_FAILURE, &s_fd);
            break;
        }
        p_ret = frame_parse(rcv_buf, ret);
        if(p_ret == FRAME_ERROR)
        {
            fprintf(stderr, "there is an error exist in frame.\n");
        } 
    }
}
#endif

#define CAN_DATA_LEN 8
#define CAN_ID_LEN 4
#define CAN_LENGTH_LEN 4

void socket_rcv(void *arg)
{
    int soc;
    int s_fd;
    int ret;
    uint8_t type;
    char rcv_buf[MAX_RCV_BUF_SIZE];
	char dest_buf[MAX_RCV_BUF_SIZE];
	int dest_len;
	int handle_len;
	char temp_buf[CAN_DATA_LEN + CAN_ID_LEN + CAN_LENGTH_LEN];
	int temp_ret;
	int recv_ret;
	int temp_i;
	int temp_len;
	int temp_packet;

    PROTOCAL_RESULT p_ret;

    type = *((uint16_t *)arg);
    soc = get_loc_socket_by_type(type);
    log_info("socekt_rcv.c, type: %d socket fd: %d\n", type, soc);
    if (soc < 0)
    {
        socket_err_handle(SOCKET_NOT_EXIST, NULL);
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_OTHER));
        return;
    }
    s_fd = soc;

    while(1)
    {
    	if ((uint8_t)SOCKET_TYPE_CAN2NET != type)
			ret = read(s_fd, rcv_buf, sizeof(rcv_buf));
		else {
			ret = 0;
			temp_ret = 0;
			temp_i = 0;
			while (1) {
				recv_ret = read(s_fd, &temp_buf[temp_ret], sizeof(temp_buf) - temp_ret);
				if (recv_ret > 0) {
					temp_ret += recv_ret;
					log_info("control box recv len: %d\n", recv_ret);
					log_info("control box recv buf: ");
					int j;
					for (j = 0; j < recv_ret; j++)
						log_info(" %02x", temp_buf[j]);
					log_info("\n");
				}

				if (temp_ret == CAN_DATA_LEN + CAN_ID_LEN + CAN_LENGTH_LEN) {
					temp_i++;
					temp_ret = 0;
					if (temp_i == 1) {
						memcpy(&temp_len, &temp_buf[CAN_ID_LEN + CAN_LENGTH_LEN], sizeof(int));
						log_info("temp_len is %d\n",temp_len);
						temp_packet = (temp_len + CAN_DATA_LEN) / CAN_DATA_LEN;
						log_info("packe_len is %d\n",temp_packet);
						ret = 0;
					} else {
						memcpy(&rcv_buf[ret], &temp_buf[CAN_ID_LEN + CAN_LENGTH_LEN], CAN_DATA_LEN);
						ret += CAN_DATA_LEN;
						if (temp_i == temp_packet) {
							temp_i = 0;
							break;
						}
					}


				}
			}
			log_info("control box handle buf: ");
			int j;
			for (j = 0; j < ret; j++)
				log_info(" %02x", rcv_buf[j]);
			log_info("\n");
		}

        if (-1 == ret) {
            socket_err_handle(RCV_FAILURE, &s_fd);
            break;
        }
		if (ret > 0) {
			do {
				handle_len = recv_check(dest_buf, &dest_len, rcv_buf, ret);

				if (dest_len > 0) {
					p_ret = frame_parse(dest_buf, dest_len);

					int test_i;
					/*
					printf("dest_buf is:");
					for(test_i=0;test_i<dest_len;test_i++){
						printf("%02x ",dest_buf[test_i]);
					}
					printf("\n");
					*/
					if (p_ret == FRAME_ERROR) {
						log_warning("there is an error exist in frame.\n");
					}
				}

				if (handle_len > 0) {
					memcpy(rcv_buf, &rcv_buf[handle_len], ret - handle_len);
					ret -= handle_len;
				}
                               if (ret == 0)
					break;
			} while (handle_len != 0);
		}
    }
}

SOCKET_RESULT socket_snd(uint8_t type, char *data, int length)
{
	int dest;
	uint16_t control_addr = 0x02;
	int canid = 0x55ff;
    dest = (uint16_t)data[3] << 8 | (uint16_t)data[4];
    if(dest == control_addr) {	//往控制盒发送数据
    	control_box_send(canid,data,length);
    	return SOCKET_SUCCESS;
    } else {	//内CAN通道发送数据

		int s_fd;

		int nbytes_to_send;     /*ready for send*/
		int nbytes_sent;        /*data has been sent*/
		int nbytes_total;       /*total data*/
		int ret;

		if (data == NULL || 0 >= length)
		{
			//fprintf(stderr, "socket err: illegal input.\n");
			log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
			return SOCKET_FAILURE;
		}

		s_fd = get_loc_socket_by_type(type);
		if (s_fd < 0)
		{
			//fprintf(stderr, "socket not found.\n");
			log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_FD_PARAMETER));
			return SOCKET_NOT_EXIST;
		}
		log_info("s_fd: %d\n", s_fd);
		nbytes_to_send = length;
		nbytes_total = length;
		nbytes_sent = 0;

		while (nbytes_sent < nbytes_total)      /* loop to send data  */
		{
			log_info("to_send:%d sent:%d\n", nbytes_to_send, nbytes_sent);
			ret = write(s_fd, (data+nbytes_sent), nbytes_to_send);
			if(-1 == ret)
			{
				//fprintf(stderr, "system err: socket_snd failure.\n_");
				log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_SOCKET_SEND));
				return SND_FAILURE;
			}
			nbytes_sent += ret;
			nbytes_to_send = nbytes_total - nbytes_sent;
		}
		return SOCKET_SUCCESS;
    }
}

/**************************/
/**********TODO************/
static void socket_err_handle(int err_code, void *arg)
{
    switch(err_code)
    {
        case SOCKET_NOT_EXIST:
            break;
        case RCV_FAILURE:
            close(*((int *)arg));
            break;
    }
}
