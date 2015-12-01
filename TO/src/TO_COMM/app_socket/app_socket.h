
/*
 * @function app_socket.h
 * @brief Ӧ�ý��̵�socketͨ��
 * @author danny
 * @date 2015.01.26
 * @note socket��ʹ�÷���
 *       ���protocalģ��һ��ʹ�á�
 *       ��ʹ��socketģ����г�ʼ������set_board_addr->socket_connect
 *       ��ʼ���ɹ��󣬴���һ�����߳�����socket_rcv�������ݽ���
 *       �����Ҫ�������ݣ�����protocal�е�frame_encap���д������
 **/        

#ifndef APP_SOCKET_H_
#define APP_SOCKET_H_

#include <stdint.h>

/*�Ӱ��ַ��*/
#define COM_0_ADDR  "192.168.1.34"
#define COM_1_ADDR  "192.168.1.35"
#define CORE_0_ADDR "192.168.1.30"
#define CORE_1_ADDR "192.168.1.31"
#define CORE_2_ADDR "192.168.1.32"
#define CORE_3_ADDR "192.168.1.33"
#define VOTE_0_ADDR "192.168.1.38"

#define BOARD_PORT 8010

/*socket���ͱ�*/
#define SOCKET_TYPE_0 0x80  
#define SOCKET_TYPE_1 0x40
#define SOCKET_TYPE_2 0x20
#define SOCKET_TYPE_3 0x10
#define SOCKET_TYPE_4 0x08
#define SOCKET_TYPE_5 0x04
#define SOCKET_TYPE_CAN2NET 0x02

#define MAX_SOCKET_NUM 6

#define MAX_RCV_BUF_SIZE 1000
#define MAX_SND_BUF_SIZE 1000

typedef enum socket_result
{
    SOCKET_SUCCESS = 0,
    SOCKET_FAILURE = -1,
    SOCKET_CONNECT_FAILURE = -2,
    INITIAL_FAILURE = -3,
    REGISTER_FAILURE = -4,
    SOCKET_NOT_EXIST = -5,
    RCV_FAILURE = -6,
    SND_FAILURE = -7
}SOCKET_RESULT;


/**
 * @function set_board_addr 
 * @brief ���ð��ַ����
 * @param type : 0~6�ֱ����COMM_0��VOTE_0
 * @return ����0����ɹ�
 **/
int set_board_addr(int board);


/**
 * @function socket_connect
 * @brief  ����socket���Ӳ���
 * @param  type ��ʶ��ͬsocketͨ��:SOCKET_TYPE_0~SOCKET_TYPE_5���ȼ��ɸߵ���
 * @return �ɹ�����SOCKET_SUCCESS,
 *         ʧ�ܷ���: SOCKET_CONNECT_FAILURE socket���Ӵ���
 *                   INITIAL_FAILURE        ��ʼ�����ݴ���
 *                   REGISTER_FAILURE       ע��ʧ��    
 * @note   ͬ��������ʹ��ͬһ��ͨ��(type)���д��䣬��ͬ����֮��Ĵ��䲻�ụ�����
 **/ 
SOCKET_RESULT socket_connect(uint8_t type);

/**
 * @function socket_rcv
 * @brief ���յײ㴫��֮����
 * @param arg socket֮����
 * @return void
 * @note ���������쳣��������:socket_err_handle
 **/ 
void socket_rcv(void *arg);

int set_loc_socket_by_type(uint8_t type, int socket_fd);

/**
 * @function socket_snd
 * @brief ������ͨ��socket���ͳ�ȥ
 * @param type socket����
 * @param data ��Ҫ���͵�����
 * @param length ���ݳ���
 * @return ���ͽ��
 **/ 
SOCKET_RESULT socket_snd(uint8_t type, char *data, int length);
#endif
