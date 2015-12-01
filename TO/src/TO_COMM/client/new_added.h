#ifndef _NEW_ADDED_H
#define _NEW_ADDED_H

#include <stdint.h>
#include "client_interface.h"
#include "to_status.h"
#include "rt_definition.h"


#define COMMU0_ID 9
#define COMMU1_ID 8
#define FLAG 0x01//corrcespond to "biaoshiwei" in excel "event table"


/*
 *������ʵʱ��Ϣ�Ľṹ��
 */
typedef struct _realtime{
	int8_t num;//������[0]
	int16_t year;//��[1-2]
	int8_t month;//��[3]
	int8_t day;//��[4]
	int8_t hour;//ʱ[5]
	int8_t min;//��[6]
	int8_t sec;//��[7]
	int8_t percentsec;//�ٷ���[8]
	int16_t train_model;//�����ͺ�[9-10]
	int16_t loco_number;//ji che hao[11-12]
	int8_t train_situation;//��������[13]
	int32_t train_mark;//���ֱ�ʶ[14-17]
	int32_t trip_number;//che ci[18-20]
	int8_t trip_status;//ben bu/ke huo[21]
	int16_t cargo_number;//liang shu[22-23]
	int16_t total_weight;//zong zhong[24-25]
	int16_t equivalent_length;//ji chang[26-27]
	int16_t load;//�س�[28-29]
	int16_t no_load;//�ճ�[30-31]
	int16_t train_type;//�г�����[32-33]
	int16_t brake_type;//�ƶ�����[34-35]
	int16_t reserve0;//����[36-37]
	int8_t route_number;//shu ru jiao lu hao[38]
	int8_t reserve1;//����[39]
	int8_t data_line_No;//���ݽ�·��[40]
	int8_t reserve2;//����[41]
	int16_t start_station;//che zhan hao[42-43]
	int16_t reserve3;//����[44-45]
	int8_t partition_side_line;//����������[46]
	int8_t down_partition_side_line;//�·�������[47]
	int8_t partition_expand_line;//������֧��[48]
	int8_t down_partion_expand_line;//�·���֧��[49]
	int8_t singnal_equipment_type;//�źŻ�����[50]
	int16_t singnal_equipment_No;//�źŻ����[51-52]
	int16_t distance;//����[53-54]
	int16_t train_singnal;//�����ź�[55-56]
	int8_t monitor_situation;//���״̬[57]
	int8_t brake_output;//�ƶ����[58]
	int16_t speed;//ʱ��[59-60]
	int16_t speed_restriction;//����[61-62]
	int32_t kilmeter_coordinate;//�����[63-65]
	int16_t speed_limit_level;//che su deng ji xian zhi[66-67]
} REALTIME_DATA;

/********************************
 * @MARSHALLING_INFO 
 * @this struct used to sotred group_info and jieshi_info
 ********************************/

/***********************************************
typedef struct _marshalling_info {
	int32_t trip_number;//che ci[18-20]
	int16_t cargo_number;//liang shu[22-23]
	int16_t equivalent_length;//ji chang[26-27]
	int16_t total_weight;//zong zhong[24-25]
	int16_t loco_number;//ji che hao[11-12]
}MARSHALLING_INFO;
***********************************************/

/**************************************
 *@JIESHI_INFO
***************************************/
/*
typedef struct _jieshi_info {
	int16_t start_station;//che zhan hao[42-43]
	int16_t speed_limit_level;//che su deng ji xian zhi[66-67]
	int8_t route_number;//shu ru jiao lu hao[38]
}JIESHI_INFO;
*/
TRAIN_STATUS trip_status;//ben bu/ke huo[21]



//extern MARSHALLING_INFO marshalling_info;
/*******************************************************
typedef enum {
    SIG_GREEN_LIGHT = 0,
    SIG_GREEN_YELLOW_LIGHT = 1,
	SIG_YELLOW_LIGHT = 2,
	SIG_YELLOW_2_LIGHT = 3,
	SIG_DOUBLE_YELLOW_LIGHT = 4,
	SIG_RED_YELLOW_LIGHT = 5,
	SIG_RED_LIGHT = 6,
	SIG_WHITE_LIGHT = 7,
	SIG_MAX = 8,
	SIG_RESERVED = 15
} TRAIN_SIG;
********************************************************/

/*******************************************************
typedef enum {
    BRAKE_UNLOAD = 0,
    BRAKE_AIR_PRESSURE = 1,
	BRAKE_AIR_SHUTOFF = 2,
	BRAKE_MAX = 3,
	BRAKE_EMERGENCY = 6,
	BRAKE_RESERVED = 7
} BRAKE_OUTPUT;
********************************************************/

/*���������¼��Ľṹ��*/
/********************************************************
typedef struct _event_flag {
	int8_t brake_output_flag;
	int8_t monitor_situation_flag;
} EVENT_FLAG;
*********************************************************/

/*��������ٶȺ͹����Ľṹ��*/
/*********************************************************
typedef struct _speed_and_location {
	int16_t speed;//ʱ��
	int32_t kilmeter_coordinate;//�����
	int32_t new_kilmeter_coordinate;//�����
	int16_t singnal_equipment_No;
	int16_t distance;
} SPEED_AND_LOCATION;
**********************************************************/

/*�������EVENT_FLAG��SPEED_AND_LOCATION�Ľṹ��*/
/*********************************************************
typedef struct _to_send {
	EVENT_FLAG event_flag;
	SPEED_AND_LOCATION speed_and_location;
	BRAKE_OUTPUT brake_out;
	TRAIN_SIG train_sig;
} TO_SEND;
**********************************************************/

/*
* ǰ����·ת��
*/
/********************************************************
typedef struct ahead_cross_route_info
{
	int ahead_cross_route_cont_post_start; //ǰ����·ת����ʼ�����������
	int data_route_number; //���ݽ�·��
	int input_route_number; //���뽻·��
} AHEAD_CROSS_ROUTE_INFO;
*********************************************************/

/*
* ǰ��֧��ת��
*/
/********************************************************
typedef struct ahead_branch_route_info
{
	int ahead_branch_route_cont_post_start; //ǰ��֧��ת����ʼ�����������
	int branch_route_number; //����֧�ߺ�
} AHEAD_BRANCH_ROUTE_INFO;
*********************************************************/

/*
* ǰ��������Ϣ
*/
/********************************************************
typedef struct ahead_side_route_info
{
	int ahead_side_route_number; //���߹ɵ���
	int ahead_side_route_cont_post_start; //������ʼ�����������
	int ahead_side_route_cont_post_end; //���߽��������������
	int ahead_side_route_speed_limit; //��������
} AHEAD_SIDE_ROUTE_INFO;
*********************************************************/

/*******************************************************************
 * @ put_rt_rs - put realtime into realtime struct
 * @ ����: ��ʵʱ��Ϣ���ַ�����ת�浽realtime�ṹ����
 * @ ����: bufָ��ʵʱ��Ϣ��ʼ�洢�����飻 rsָ��ʵʱ��Ϣת���Ľṹ��
 *******************************************************************/
int put_rt_rs(char *buf, REALTIME_DATA *rs);

/*********************************************************
 * @ ����: ����ÿ��������ʵʱ��Ϣ�����ѽ������д�����fdָ����ļ�
 * @ ����: fd - ��д�����ݵ��ļ��� rs - ָ��洢ʵʱ��Ϣ�Ľṹ
 *********************************************************/
int realtime_translation(REALTIME_DATA rs, FILE* fd);


/*****************************
 * @ ����:�ú���������ʵʱ��Ϣ����ȡ������Ϣ
 * @ marshalling_info - ָ�������Ϣ�ṹ���ָ��
 * @ realtime_data - ʵʱ��Ϣ�ṹ��
 *****************************/
void set_marshalling_info(MARSHALLING_INFO *marshalling_info, REALTIME_DATA realtime_data);

/*****************************
 * @ ������ӡ������Ϣ,ֻ�ڲ���ʱʹ�øú���
 * @ marshalling_infoΪָ�������Ϣ�ṹ���ָ��
 ****************************/
void marshalling_info_print(MARSHALLING_INFO marshalling_info);


/*get communication borar event source according to BORARID in makefile*/
char get_commu_event_source(int board_id);


/*
 *check commu_brd source number  accordding to BOARDID in makefile
 */
int check_commu_num(void);

void judge_trip_train_status(REALTIME_DATA realtime_data);



#endif
