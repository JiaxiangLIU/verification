/**
 * @file    rt_definition.h
 * @brief   real time process 全局变量中会使用到的一些结构体的定义
 * @date    2015.3.20
 * @author  jin
 * @note  	定义整个项目中需要用的数据结构以及预留最大空间,其他文件均需引用该头文件,同时定义其他需要的参数常量
 */

#include "rt_definition.h"

#ifndef INTERRTOPTDEFINITION_H_
#define INTERRTOPTDEFINITION_H_

//#include "realtime_data.h"
//#include "to_status.h"
//#include "opt_definition.h"



//typedef struct _marshalling_info {
//	int32_t trip_number;//che ci[18-20]
//	int16_t cargo_number;//liang shu[22-23]
//	int16_t equivalent_length;//ji chang[26-27]
//	int16_t total_weight;//zong zhong[24-25]
//	int16_t loco_number;//ji che hao[11-12]
////	TRAIN_STATUS trip_status;//ben bu/ke huo[21]
////	int16_t start_station;//che zhan hao[42-43]
////	int16_t speed_limit_level;//che su deng ji xian zhi[66-67]
////	int8_t route_number;//shu ru jiao lu hao[38]
//} MARSHALLING_INFO;

/**
 * linked list struct definition
 */
typedef struct ll_node
{
	OPTIMIZERESULT* data;
	int data_length;
	struct ll_node* next;
} LL_NODE;


///**
// * tele post info
// */
//typedef struct tele_post
//{
//	int tel_num;/**<信号机编号*/
//	float cross_dist;/**<越过距离*/
//	float tel_kp;/**<信号机实际公里标*/
//} TELE_POST;


///*
// * real time parameters
// */
//typedef struct rt_par
//{
//	int rt_gear;
//	int opt_gear;
//	float rt_v;
//	float opt_v;
//	float rt_cont_post;
//	int rt_post;
//	TRAIN_SIG rt_light;
////	float total_time;/**<行走的时间*/
//	TELE_POST tele_post;
//} RT_PAR;

///**
// * real time enable definition
// */
//typedef enum rt_enable_status
//{
//	RT_AUTO_ENABLE = 0, //enable for auto drive
//	RT_AUTO_UNABLE,
//	RT_AIR_BRAKING,
//	//	ADD_LOCO_AREA,
//	//	RUN_THROUGH_TEST,
//	//	AIR_DAMPING_AVG,
//	//	RT_VELOCITY_MANNUAL,
//} RT_ENABLE_STATUS;

///**
// * loco working status definition
// */
//typedef enum rt_loco_working_status
//{
//	RT_READY = 0,
//	RT_RUNNING,
//	RT_WAITING,
//	RT_FINISH,
//	RT_EXCEPTION,
//} RT_LOCO_WORKING_STATUS;
//
//typedef enum rt_control_status
//{
//	RT_MANNUAL = 0,
//	RT_AUTO,
//} RT_CONTROL_STATUS;
//
//typedef enum rt_trip_status
//{
//	RT_DUTY = 0,
//	RT_ASSISTING,
//}RT_TRIP_STATUS;
//
///*
// * real time status
// */
//typedef struct rt_status
//{
//	RT_ENABLE_STATUS rt_enable_status;
//	RT_LOCO_WORKING_STATUS rt_loco_working_status;
//	BRAKE_OUTPUT rt_airbrake_status;
//	RT_CONTROL_STATUS rt_control_status;
//	RT_TRIP_STATUS rt_trip_status;
//} RT_STATUS;
//
typedef struct anti_stop_flags
{
//	int as_check_flag;
//	int as_enter_flag;
	int ma_check_flag;
	int ma_enter_flag;
} AHEAD_EST_FLAGS;
//
//
///**
// * real time anti-stop definition
// */
//typedef enum anti_stop_type
//{
//	RT_ANTI_STOP_NULL = 0, //enable for auto drive
//	RT_FORBID_STOP_ON_GRAD = 82,/**<禁止坡停区域标识*/     //!< FORBID_STOP_ON_GRAD
//	RT_FORBID_STOP_CROSS_GRAD = 83,
//} ANTI_STOP_TYPE;
//
///**
// * next anti-stop definition
// */
//typedef struct anti_stop_info
//{
//	ANTI_STOP_TYPE anti_stop_type;
//	float con_post_start;/**<连续的公里标*/
//	float con_post_end;/**<连续的公里标*/
//} ANTI_STOP_INFO;
//
///**
// * next station info in real time process
// */
//typedef struct next_station_info
//{
//	int ns_id;  /**< 车站号*/
//	int cont_ns_start; /**< 公里标*/
//	float t_to_ns; /*time to next station*/
//} NEXT_STATION_INFO;
//
///**
// * over speed estimation info in real time process
// */
//typedef struct over_speed_info
//{
//	int os_flag;
//	int cont_os_start;
//} OVER_SPEED_INFO;
//
///**
// * grade ability estimation info in real time process
// */
//typedef struct grade_ability_info
//{
//	int ga_flag;
//	int cont_ga_start;
//} GRADE_ABILITY_INFO;
//
//
//typedef struct mannual_area_info
//{
//	int ma_flag;
//	int cont_ma_start;
//} MANNUAL_AREA_INFO;
//
//
///**
// * real time output struct
// * add: enable info, rt_v, rt_post,
// */
//typedef struct rt_data
//{
//	RT_PAR rt_par; //parameters for real time control
//	RT_STATUS rt_status; //real time status
//	NEXT_STATION_INFO ns_info; //next station info
//	OVER_SPEED_INFO os_info; //over speed info
//	GRADE_ABILITY_INFO ga_info; //grade ability info
//	ANTI_STOP_INFO as_info; //anti stop info
//	MANNUAL_AREA_INFO ma_info;
//} RT_DATA;
//
//
///**
// * next enable info definition
// */
//typedef struct rt_next_enable_info
//{
//	RT_ENABLE_INFO rt_en_info;
//	int tel_num_start;/**<信号机编号*/
//	float cross_dist_start;/**<越过距离*/
//	float tel_kp_start;/**<信号机实际公里标*/
//	float con_tel_kp_start;/**<连续的公里标*/
//	int tel_num_end;/**<信号机编号*/
//	float cross_dist_end;/**<越过距离*/
//	float tel_kp_end;/**<信号机实际公里标*/
//	float con_tel_kp_end;/**<连续的公里标*/
//};
//typedef struct rt_next_enable_info RT_NXEN_INFO;


/**
 * real time input struct
 */
typedef struct rt_input
{
	int rt_con_start_post;
	int rt_con_end_post;
	float rt_con_tel_kp;
	float rt_velocity;
	SPEED_AND_LOCATION speed_location;
	TRAIN_SIG rt_light;
	BRAKE_OUTPUT rt_airbrake_status;
} RT_INPUT;

/**
 * real time in&out
 */
typedef struct rt_inout
{
	float rt_temp_start;
	float rt_temp_end;
} RT_INOUT;

/**
 * real time flags
 */
typedef struct rt_flag
{
	int rt_opt_flag;
	int rt_check_temp_flag;
	int rt_ctr_flag;
} RT_FLAG;

/**
 * real time flags
 */
typedef struct rt_index
{
	int rt_global_index;
	int rt_temp_index;
} RT_INDEX;


/**
 * real time running status definition
 */
typedef enum rt_process_result
{
	RT_SUCCESS = 0,
	RT_MEMORRY_FAILURE,
	RT_NOREACH_OPTRESULT,
	RT_PASS_OPTRESULT,
	RT_LOCATE_FAILURE,
	RT_TEMPOPT_FAILURE,
	RT_LL_ERROR,
	RT_T_EST_FAILURE,
	RT_OSPEED_EST_FAILURE,
	RT_LOCK_FAILURE,
	RT_UNLOCK_FAILURE,
	RT_OPR_ERROR,
} RT_RESULT;

typedef enum ll_manage_result
{
		LL_SUCCESS = 0,
		LL_MEMORRY_FAILURE,
		LL_INPUT_DATA_NULL,
		LL_LOCK_FAILURE,
		LL_UNLOCK_FAILURE,
} LL_RESULT;

#endif /* DATADEFINITION_H_ */
