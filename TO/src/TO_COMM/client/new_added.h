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
 *定义存放实时信息的结构体
 */
typedef struct _realtime{
	int8_t num;//生命号[0]
	int16_t year;//年[1-2]
	int8_t month;//月[3]
	int8_t day;//日[4]
	int8_t hour;//时[5]
	int8_t min;//分[6]
	int8_t sec;//秒[7]
	int8_t percentsec;//百分秒[8]
	int16_t train_model;//机车型号[9-10]
	int16_t loco_number;//ji che hao[11-12]
	int8_t train_situation;//机车工况[13]
	int32_t train_mark;//车种标识[14-17]
	int32_t trip_number;//che ci[18-20]
	int8_t trip_status;//ben bu/ke huo[21]
	int16_t cargo_number;//liang shu[22-23]
	int16_t total_weight;//zong zhong[24-25]
	int16_t equivalent_length;//ji chang[26-27]
	int16_t load;//重车[28-29]
	int16_t no_load;//空车[30-31]
	int16_t train_type;//列车类型[32-33]
	int16_t brake_type;//制动类型[34-35]
	int16_t reserve0;//备用[36-37]
	int8_t route_number;//shu ru jiao lu hao[38]
	int8_t reserve1;//备用[39]
	int8_t data_line_No;//数据交路号[40]
	int8_t reserve2;//备用[41]
	int16_t start_station;//che zhan hao[42-43]
	int16_t reserve3;//备用[44-45]
	int8_t partition_side_line;//本分区侧线[46]
	int8_t down_partition_side_line;//下分区侧线[47]
	int8_t partition_expand_line;//本分区支线[48]
	int8_t down_partion_expand_line;//下分区支线[49]
	int8_t singnal_equipment_type;//信号机种类[50]
	int16_t singnal_equipment_No;//信号机编号[51-52]
	int16_t distance;//距离[53-54]
	int16_t train_singnal;//机车信号[55-56]
	int8_t monitor_situation;//监控状态[57]
	int8_t brake_output;//制动输出[58]
	int16_t speed;//时速[59-60]
	int16_t speed_restriction;//限速[61-62]
	int32_t kilmeter_coordinate;//公里标[63-65]
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

/*声明产生事件的结构体*/
/********************************************************
typedef struct _event_flag {
	int8_t brake_output_flag;
	int8_t monitor_situation_flag;
} EVENT_FLAG;
*********************************************************/

/*声明存放速度和公里标的结构体*/
/*********************************************************
typedef struct _speed_and_location {
	int16_t speed;//时速
	int32_t kilmeter_coordinate;//公里标
	int32_t new_kilmeter_coordinate;//公里标
	int16_t singnal_equipment_No;
	int16_t distance;
} SPEED_AND_LOCATION;
**********************************************************/

/*声明存放EVENT_FLAG和SPEED_AND_LOCATION的结构体*/
/*********************************************************
typedef struct _to_send {
	EVENT_FLAG event_flag;
	SPEED_AND_LOCATION speed_and_location;
	BRAKE_OUTPUT brake_out;
	TRAIN_SIG train_sig;
} TO_SEND;
**********************************************************/

/*
* 前方交路转移
*/
/********************************************************
typedef struct ahead_cross_route_info
{
	int ahead_cross_route_cont_post_start; //前方交路转移起始点连续公里标
	int data_route_number; //数据交路号
	int input_route_number; //输入交路号
} AHEAD_CROSS_ROUTE_INFO;
*********************************************************/

/*
* 前方支线转移
*/
/********************************************************
typedef struct ahead_branch_route_info
{
	int ahead_branch_route_cont_post_start; //前方支线转移起始点连续公里标
	int branch_route_number; //输入支线号
} AHEAD_BRANCH_ROUTE_INFO;
*********************************************************/

/*
* 前方侧线信息
*/
/********************************************************
typedef struct ahead_side_route_info
{
	int ahead_side_route_number; //侧线股道号
	int ahead_side_route_cont_post_start; //侧线起始点连续公里标
	int ahead_side_route_cont_post_end; //侧线结束点连续公里标
	int ahead_side_route_speed_limit; //侧线限速
} AHEAD_SIDE_ROUTE_INFO;
*********************************************************/

/*******************************************************************
 * @ put_rt_rs - put realtime into realtime struct
 * @ 功能: 将实时信息从字符数组转存到realtime结构体中
 * @ 参数: buf指向实时信息开始存储的数组； rs指向实时信息转存后的结构体
 *******************************************************************/
int put_rt_rs(char *buf, REALTIME_DATA *rs);

/*********************************************************
 * @ 功能: 解析每个完整的实时信息，并把解析结果写入参数fd指向的文件
 * @ 参数: fd - 欲写入数据的文件； rs - 指向存储实时信息的结构
 *********************************************************/
int realtime_translation(REALTIME_DATA rs, FILE* fd);


/*****************************
 * @ 功能:该函数用来从实时信息中提取编组信息
 * @ marshalling_info - 指向编组信息结构体的指针
 * @ realtime_data - 实时信息结构体
 *****************************/
void set_marshalling_info(MARSHALLING_INFO *marshalling_info, REALTIME_DATA realtime_data);

/*****************************
 * @ 用来打印编组信息,只在测试时使用该函数
 * @ marshalling_info为指向编组信息结构体的指针
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
