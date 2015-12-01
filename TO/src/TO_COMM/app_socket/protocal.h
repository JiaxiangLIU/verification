

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
#define MAX_DATA_FRAME_SIZE 1000   /*���������֡��С*/ 
#define DATA_FRAME_HEAD_SIZE 14    /*����֡ͷ��С, ����CRC*/

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
 * @brief ���ײ�Э�鴫������Э������֡���н���
 * @param buf ��Ҫ���н���������
 * @param length ���ݳ���
 * @return PROTOCAL_RESULT �������
 *         PROTOCAL_FAILURE �����������
 *         FRAME_ERROR      ֡����
 *         PROTOCAL_SUCCESS �����ɹ�
 **/ 
PROTOCAL_RESULT frame_parse(char *buf, int length);


/**
 * @function frame_encap
 * @brief ��֡����Э���װ���������ײ�Э�鷢��
 * @param stype socket 0x80 0x40 0x20(command frame) 0x10 0x08(realtime data passage) 0x04
 * @param des Ŀ���ַ 0x02 �ͻ��� 0x04 ˾���� 0x08 ����� 0x10 0x20 0x40 0x80 ���İ� 0x100 0x200ͨ�Ű�               �ɵ���
 * @param ftype ֡���� ����֡������֡: COMMAND_FRAME_CODE; DATA_FRAME_CODE
 * @param command ֡����
 * @param data ֡����, ��������֡��˵��Ϊ֡����
 * @param length ֡���ݳ���
 * @return �ɹ�����PROTOCAL_SUCEESS
 **/
PROTOCAL_RESULT frame_encap(uint8_t stype, uint16_t des, uint8_t ftype, char command[2], char *data, int length);


#endif
