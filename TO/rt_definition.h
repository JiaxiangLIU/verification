/**
 * @file    rt_definition.h
 * @brief   real time process 重定义
 * @brief   接受时将信息放在发送端结构体，然后将自己信息赋值后，封装成新结构体，发送给目的地
 * @date    2015.10.19
 * @author  lzk
 * @note  	定义整个项目中需要用的数据结构,其他文件均需引用该头文件,各板使用符合自己的结构体
 */
#ifndef RTDEFINITION_H_
#define RTDEFINITION_H_

/*
 *****************************************************************
* COMM板结构
* 实时信息
* 对应comm板 TO_SEND 结构体
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

// 以后可能修改
typedef struct _event_flag {
	int8_t monitor_situation_flag;
} EVENT_FLAG;

typedef struct _speed_and_location {
	int16_t speed;//
	int32_t kilmeter_coordinate;//
	int32_t new_kilmeter_coordinate;//
	int16_t singnal_equipment_No;
	int16_t distance;
} SPEED_AND_LOCATION;


typedef struct rt_comm {
	EVENT_FLAG event_flag;
	SPEED_AND_LOCATION speed_and_location;
	BRAKE_OUTPUT brake_out;
	TRAIN_SIG train_sig;
} RT_COMM;


/*
 * ***************************************************************
* CORE板结构
* 实时信息
* 对应CORE板 RT_DATA结构体
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
	RT_READY = 0,
	RT_RUNNING,
	RT_WAITING,
	RT_FINISH,
	RT_EXCEPTION,
} RT_LOCO_WORKING_STATUS;

typedef struct rt_status
{
	RT_ENABLE_STATUS rt_enable_status;
	RT_LOCO_WORKING_STATUS rt_loco_working_status;
} RT_STATUS;


typedef struct rt_opt
{
       int opt_gear;
       int rt_gear;
       int opt_v;
} RT_OPT;

typedef struct rt_core	//实时信息结构体
{
	RT_COMM rt_comm; //parameters for real time control
	RT_STATUS rt_status; //real time status
    RT_OPT  rt_opt;
} RT_CORE;


/*
 *****************************************************************
* vote板结构
* 实时信息
*  对应vote板 RT_DATA结构体
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
* 编组信息
*/
typedef struct marshalling_info
{
int32_t trip_number; //车次
int16_t cargo_number; //辆数
int16_t equivalent_length; //换长
int16_t total_weight; //总重
} MARSHALLING_INFO;


/*
* 前方交路转移
*/
typedef struct ahead_cross_route_info
{
	int ahead_cross_route_enable; //侧线有效标识
	int data_route_number; //数据交路号
	int input_route_number; //输入交路号
	int ahead_cross_route_cont_post_start; //前方交路转移起始点连续公里标
} AHEAD_CROSS_ROUTE_INFO;


/*
* 前方支线转移
*/
typedef struct ahead_branch_route_info
{
	int ahead_cross_route_enable; //侧线有效标识
	int data_route_number; //数据交路号
	int input_route_number; //输入交路号
	int ahead_branch_route_cont_post_start; //前方支路转移起始点连续公里标
} AHEAD_BRANCH_ROUTE_INFO;


/*
* 前方侧线信息
*/
typedef struct ahead_side_route_info
{
	int ahead_side_route_enable; //侧线有效标识
	int ahead_side_route_number; //侧线股道号
	int ahead_side_route_cont_post_start; //侧线起始点连续公里标
	int ahead_side_route_cont_post_end; //侧线结束点连续公里标
} AHEAD_SIDE_ROUTE_INFO;
#endif
