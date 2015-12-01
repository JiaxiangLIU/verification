

/**
 * @function global.h
 * @brief 定义comm当中使用的全局变量
 * @author danny
 * @date 2015.01.26
 **/


#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "global.h"
#include "./threadpool/threadpool.h"
#include "./message_queue/message_queue.h"
#include "route_data.h"
#include "error_code.h"
/**
 * 线程池设置
 **/
#define THREAD_NUM 10   //最大线程数，决定线程池创建多少个线程
#define MAX_TASK_NUM 20 //最大任务数，决定线程池保留多少个任务进行等待
extern threadpool_t *pool;

/**
 * 消息队列设置
 **/ 
#define QUEUE_NUM 2
extern struct multilevel_queue_t *queues; 

/**
 * 线程间交互设置
 **/ 
extern uint32_t MAX_TELE;
extern CONVERT_MILE_NODE *cm_node;
#endif
