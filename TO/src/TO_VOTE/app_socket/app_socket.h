
/*
 * @function app_socket.h
 * @brief 应用进程的socket通信
 * @author danny
 * @date 2015.01.26
 * @note socket的使用方法
 *       配合protocal模块一起使用。
 *       先使用socket模块进行初始化，即set_board_addr->socket_connect
 *       初始化成功后，创建一个新线程启动socket_rcv进行数据接收
 *       如果想要发送数据，调用protocal中的frame_encap进行打包发送
 **/        

#ifndef APP_SOCKET_H_
#define APP_SOCKET_H_

#include <stdint.h>

/*子板地址表*/
#define COM_0_ADDR  "192.168.1.34"
#define COM_1_ADDR  "192.168.1.35"
#define CORE_0_ADDR "192.168.1.30"
#define CORE_1_ADDR "192.168.1.31"
#define CORE_2_ADDR "192.168.1.32"
#define CORE_3_ADDR "192.168.1.33"
#define VOTE_0_ADDR "192.168.1.38"

#define BOARD_PORT 8010

/*socket类型表*/
#define SOCKET_TYPE_0 0x80  
#define SOCKET_TYPE_1 0x40
#define SOCKET_TYPE_2 0x20
#define SOCKET_TYPE_3 0x10
#define SOCKET_TYPE_4 0x08
#define SOCKET_TYPE_5 0x04
#define SOCKET_TYPE_CAN2NET 0x02

#define MAX_SOCKET_NUM 10

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
 * @brief 设置板地址属性
 * @param type : 0~6分别代表COMM_0到VOTE_0
 * @return 返回0代表成功
 **/
int set_board_addr(int board);


/**
 * @function socket_connect
 * @brief  进行socket连接操作
 * @param  type 标识不同socket通道:SOCKET_TYPE_0~SOCKET_TYPE_5优先级由高到低
 * @return 成功返回SOCKET_SUCCESS,
 *         失败返回: SOCKET_CONNECT_FAILURE socket连接错误
 *                   INITIAL_FAILURE        初始化数据错误
 *                   REGISTER_FAILURE       注册失败    
 * @note   同类型数据使用同一个通道(type)进行传输，不同数据之间的传输不会互相干扰
 **/ 
SOCKET_RESULT socket_connect(uint8_t type);

/**
 * @function socket_rcv
 * @brief 接收底层传递之数据
 * @param arg socket之类型
 * @return void
 * @note 出错将调用异常处理流程:socket_err_handle
 **/ 
void socket_rcv(void *arg);

int set_loc_socket_by_type(uint8_t type, int socket_fd);

/**
 * @function socket_snd
 * @brief 将数据通过socket发送出去
 * @param type socket类型
 * @param data 想要发送的数据
 * @param length 数据长度
 * @return 发送结果
 **/ 
SOCKET_RESULT socket_snd(uint8_t type, char *data, int length);
#endif
