/**
 * @file    rt_definition.h
 * @brief   real time process �ض���
 * @brief   ����ʱ����Ϣ���ڷ��Ͷ˽ṹ�壬Ȼ���Լ���Ϣ��ֵ�󣬷�װ���½ṹ�壬���͸�Ŀ�ĵ�
 * @date    2015.10.19
 * @author  lzk
 * @note  	����������Ŀ����Ҫ�õ����ݽṹ,�����ļ��������ø�ͷ�ļ�,����ʹ�÷����Լ��Ľṹ��
 */

#ifndef RTDEFINITION_H_
#define RTDEFINITION_H_
#include <stdint.h>

/*
 *****************************************************************
* COMM��ṹ
* ʵʱ��Ϣ
* ��Ӧcomm�� TO_SEND �ṹ��
*/
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

typedef enum {
	BRAKE_UNLOAD = 0,
	BRAKE_AIR_PRESSURE = 1,
	BRAKE_AIR_SHUTOFF = 2,
	BRAKE_MAX = 3,
	BRAKE_EMERGENCY = 6,
	BRAKE_RESERVED = 7
} BRAKE_OUTPUT;

// �Ժ�����޸�
typedef struct _event_flag {
	int8_t monitor_situation_flag;
} EVENT_FLAG;

typedef struct _speed_and_location {
	int16_t speed;//
	int32_t kilmeter_coordinate;//
	int32_t new_kilmeter_coordinate;//
	int16_t singnal_equipment_No;
	int16_t distance;
	//int32_t speed_limit;
} SPEED_AND_LOCATION;


typedef struct rt_comm {
	EVENT_FLAG event_flag;
	SPEED_AND_LOCATION speed_and_location;
	BRAKE_OUTPUT brake_out;
	TRAIN_SIG train_sig;
	//int16_t speed_limit;
} RT_COMM;


/*
 * ***************************************************************
* CORE��ṹ
* ʵʱ��Ϣ
* ��ӦCORE�� RT_DATA�ṹ��
*/

// real time enable definition
typedef enum rt_enable_status
{
	RT_AUTO_ENABLE = 0, //enable for auto drive
	RT_AUTO_UNABLE,
	RT_AIRBRAKING,
} RT_ENABLE_STATUS;


//loco working status definition 0~5
typedef enum rt_loco_working_status
{
	BEFORE_OPR_AREA = 0,
	IN_OPR_AREA,
	AFTER_OPR_AREA = 0,
} RT_POST_STATUS;

typedef struct rt_status
{
	RT_ENABLE_STATUS rt_enable_status;
	RT_POST_STATUS rt_post_status;
} RT_STATUS;


typedef struct rt_opt
{
	int opt_gear;
	int rt_gear;
	int opt_v;
} RT_OPT;

typedef struct mannual_area_info
{
	int ma_flag;
	int cont_ma_start;
} MANUAL_AREA_INFO;

typedef struct rt_core	//ʵʱ��Ϣ�ṹ��
{
	RT_COMM rt_comm; //parameters for real time control
	RT_STATUS rt_status; //real time status
    RT_OPT  rt_opt;
//    MANUAL_AREA_INFO ma_info;
} RT_CORE;


/*
 *****************************************************************
* vote��ṹ
* ʵʱ��Ϣ
*  ��Ӧvote�� RT_DATA�ṹ��
*/
typedef enum rt_control_status
{
	RT_MANNUAL = 0,
	RT_AUTO,
} RT_CONTROL_STATUS;

typedef struct rt_vote
{
    RT_CORE rt_core;
    RT_CONTROL_STATUS rt_control_status;
}RT_VOTE;




/*
 ** ***************************************************************
* ������Ϣ
*/
typedef struct marshalling_info
{
	int32_t trip_number; //����
	int16_t cargo_number; //����
	int16_t equivalent_length; //����
	int16_t total_weight; //����
} MARSHALLING_INFO;


/*
* ǰ����·ת��
*/
typedef struct ahead_cross_route_info
{
	int ahead_cross_route_enable; //������Ч��ʶ
	int data_route_number; //���ݽ�·��
	int input_route_number; //���뽻·��
	int ahead_cross_route_cont_post_start; //ǰ����·ת����ʼ�����������
} AHEAD_CROSS_ROUTE_INFO;


/*
* ǰ��֧��ת��
*/
typedef struct ahead_branch_route_info
{
	int ahead_cross_route_enable; //������Ч��ʶ
	int data_route_number; //���ݽ�·��
	int input_route_number; //���뽻·��
	int ahead_branch_route_cont_post_start; //ǰ��֧·ת����ʼ�����������
} AHEAD_BRANCH_ROUTE_INFO;


/*
* ǰ��������Ϣ
*/
typedef struct ahead_side_route_info
{
	int ahead_side_route_enable; //������Ч��ʶ
	int ahead_side_route_number; //���߹ɵ���
	int ahead_side_route_cont_post_start; //������ʼ�����������
	int ahead_side_route_cont_post_end; //���߽��������������
} AHEAD_SIDE_ROUTE_INFO;

/*
* �г���Ϣ
*/
typedef struct trip_info
{
	int start_station;//che zhan hao
	int end_station;//
	int trip_number; //����
} TRIP_INFO;

/*
* ��ʾ��Ϣ
*/
typedef struct jieshi_info
{
	int start_kilmeter_coordinate;
	int end_kilmeter_coordinate;
	int limit_speed;
} JIESHI_INFO;

/*
*��ʱ����
*/
typedef struct temp_speed_lmt//lmt--limit
{
	int statrt_km_post;
	int end_km_post;
	int distance;
	int temp_speed;
} TEMP_SPEED_LMT;
#endif

