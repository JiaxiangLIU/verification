
#ifndef SCHEDULE_H_
#define SCHEDULE_H_

//#include "schedule.h"
#include "to_event.h"
#include "message_queue.h"


typedef enum schedule_result
{
    SCHEDULE_SUCCESS = 0,
    SCHEDULE_FAILURE = -1,
    MUTEX_ERR = -2
}SCHEDULE_RESULT;

/**
 * @function add_task
 * @brief ���ѵ����߳�(schedule)���������ύ�������߳�
 * @param event ����
 * @return SCHEDULE_RESULT
 * @note �¼��������ʱ�������һ��α�¼������¼���֪ͨ�����̵߳�������ȡ�¼�
 * @note ���ж��add_commandʱ���Ὰ��schedule thread������ʧ�ܻụ��ȴ�
 **/ 
SCHEDULE_RESULT add_command(struct event_t *event);

/**
 * @function schedule
 * @brief �����߳�
 * @param multi_queue Ŀ������Ķ༶����
 * @return void �ݶ�Ϊvoid
 **/
void schedule(void *arg);
#endif
