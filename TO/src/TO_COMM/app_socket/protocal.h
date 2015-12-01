

#ifndef PROTOCAL_H_
#define PROTOCAL_H_

#include <stdint.h>
#include "protocal.h"


#define DATA_FRAME_CODE 0x52
#define COMMAND_FRAME_CODE 0X58

#define SOCKET_TYPE 0x80
#define COM_SOC 0x20
#define DAT_SOC 0x04

#define CON_0_DES  0x02//ke hu duan
#define COM_0_DES  0x200
#define COM_1_DES  0x100
#define CORE_0_DES 0x80
#define CORE_1_DES 0x40
#define CORE_2_DES 0x20
#define CORE_3_DES 0x10
#define VOTE_0_DES 0x08

#define COMMAND_FRAME_SIZE 9
#define MAX_DATA_FRAME_SIZE 1000   /*最大传输数据帧大小*/ 
#define DATA_FRAME_HEAD_SIZE 14    /*数据帧头大小, 包括CRC*/

typedef enum protocal_result
{
    PROTOCAL_SUCCESS = 0,
    PROTOCAL_FAILURE = -1,
    FRAME_ERROR = -2,
    FRAME_ENCAP_FAILURE = -3,
    SOCKET_TYPE_ERR = -10,
    SOCKET_SND_FAILURE = -11,
    THREAD_ERROR = -22,
    PROTOCAL_MALLOC_ERROR = -23
}PROTOCAL_RESULT;

#define ROUTE_DATA_NUM 6
extern int route_data_confirm[ROUTE_DATA_NUM];
/**
 * @function frame_parse
 * @brief 将底层协议传递来的协议数据帧进行解析
 * @param buf 需要进行解析的数据
 * @param length 数据长度
 * @return PROTOCAL_RESULT 解析结果
 *         PROTOCAL_FAILURE 输入参数错误
 *         FRAME_ERROR      帧错误
 *         PROTOCAL_SUCCESS 解析成功
 **/ 
PROTOCAL_RESULT frame_parse(char *buf, int length);


/**
 * @function frame_encap
 * @brief 对帧进行协议封装，并交给底层协议发送
 * @param stype socket 0x80 0x40 0x20(command frame) 0x10 0x08(realtime data passage) 0x04
 * @param des 目标地址 0x02 客户端 0x04 司控器 0x08 表决板 0x10 0x20 0x40 0x80 核心板 0x100 0x200通信板               可叠加
 * @param ftype 帧类型 数据帧或命令帧: COMMAND_FRAME_CODE; DATA_FRAME_CODE
 * @param command 帧命令
 * @param data 帧数据, 对于命令帧来说此为帧参数
 * @param length 帧数据长度
 * @return 成功返回PROTOCAL_SUCEESS
 **/
PROTOCAL_RESULT frame_encap(uint8_t stype, uint16_t des, uint8_t ftype, char command[2], char *data, int length);


#endif
