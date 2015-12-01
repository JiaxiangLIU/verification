

/**
 * @function global.h
 * @brief 定义vote当中使用的全局变量
 * @author danny
 * @date 2015.01.26
 **/


#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <pthread.h>
#include "global.h"
#include "./threadpool/threadpool.h"
#include "./message_queue/message_queue.h"
#include "rt_definition.h"
#include "error_code.h"
# include "led.h"
#include "protocal.h"
/* 线程池设置 */
#define THREAD_NUM 10   //最大线程数，决定线程池创建多少个线程
#define MAX_TASK_NUM 20 //最大任务数，决定线程池保留多少个任务进行等待
extern threadpool_t *pool;

/* 消息队列设置 */
#define QUEUE_NUM 2
extern struct multilevel_queue_t *queues; 

/* 板ID */
#define COMM_0 9
#define COMM_1 8
#define CORE_0 7
#define CORE_1 6
#define CORE_2 5
#define CORE_3 4
#define VOTE   3

/* 判断手动转自动控车的最小速度 */
#define THRESHOLD_V 20
/* 手动转自动的30s内需要切换到的档位 */
#define SWITCH_GEAR 8



/* 手自动控车转换的等待时间 */
#define COUNT_TIME 30

/* 实时数据 */
extern RT_VOTE rt_vote;
extern RT_CORE rt_core;
extern RT_VOTE fsm_rt;
extern RT_VOTE fsm_temp;

/* 有实时信息处理锁 */
extern pthread_mutex_t rt_mutex;

extern int act_com;
extern int act_con;
typedef struct {
	pthread_mutex_t temp_gear_mutex;
	int gear;
} TEMP_GEAR;

typedef struct {
	pthread_mutex_t throttle_gear_mutex;
	int gear;
	int auto_or_manual;
} THROTTLE_GEAR;

/* 判断档位（手柄位）有没有变化存放档位 */
extern TEMP_GEAR temp_gear;

/* 控制手动转自动线程的flag */
typedef struct {
	pthread_mutex_t mta_mutex;
	pthread_cond_t mta_cond;
	uint8_t mta_flag;
} MANUAL_TO_AUTO_FLAG;

/* 控制自动转手动线程的flag */
typedef struct {
	pthread_mutex_t atm_mutex;
	pthread_cond_t atm_cond;
	uint8_t atm_flag;
}AUTO_TO_MANUAL_FLAG;

/* 机车控制状态flag */
typedef struct {
	pthread_mutex_t ctrl_mutex;
	uint8_t ctrl_flag;	//0：手动，1：自动
}CTRL_STATUS_FLAG;

extern MANUAL_TO_AUTO_FLAG manual_to_auto_flag;
extern AUTO_TO_MANUAL_FLAG auto_to_manual_flag;
extern CTRL_STATUS_FLAG ctrl_status_flag;
extern THROTTLE_GEAR throttle_gear1;
extern int change_gear;
extern int second_ack;
extern int fsm_start;
#endif
