
#ifndef SCHEDULE_H_
#define SCHEDULE_H_

#include <to_event.h>
#include "schedule.h"
#include "message_queue.h"


typedef enum schedule_result
{
    SCHEDULE_SUCCESS = 0,
    SCHEDULE_FAILURE = -1,
    MUTEX_ERR = -2
}SCHEDULE_RESULT;

/**
 * @function add_task
 * @brief 唤醒调度线程(schedule)，将任务提交给调度线程
 * @param event 任务
 * @return SCHEDULE_RESULT
 * @note 事件处理结束时，会产生一个伪事件，该事件会通知调度线程到队列里取事件
 * @note 当有多个add_command时，会竞争schedule thread，竞争失败会互斥等待
 **/ 
SCHEDULE_RESULT add_command(struct event_t *event);

/**
 * @function schedule
 * @brief 调度线程
 * @param multi_queue 目标操作的多级队列
 * @return void 暂定为void
 **/
void schedule(void *arg);
#endif
