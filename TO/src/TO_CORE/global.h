

/**
 * @function global.h
 * @brief 定义core当中使用的全局变量
 * @author danny
 * @date 2015.02.02
 **/


#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "global.h"
#include "./threadpool/threadpool.h"
#include "./message_queue/message_queue.h"


#define THREAD_NUM 10   //最大线程数，决定线程池创建多少个线程
#define MAX_TASK_NUM 20 //最大任务数，决定线程池保留多少个任务进行等待
extern threadpool_t *pool;

#define QUEUE_NUM 2
extern struct multilevel_queue_t *queues; 
#include "error_code.h"
#endif
