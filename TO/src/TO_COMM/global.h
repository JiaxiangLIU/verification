

/**
 * @function global.h
 * @brief ����comm����ʹ�õ�ȫ�ֱ���
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
 * �̳߳�����
 **/
#define THREAD_NUM 10   //����߳����������̳߳ش������ٸ��߳�
#define MAX_TASK_NUM 20 //����������������̳߳ر������ٸ�������еȴ�
extern threadpool_t *pool;

/**
 * ��Ϣ��������
 **/ 
#define QUEUE_NUM 2
extern struct multilevel_queue_t *queues; 

/**
 * �̼߳佻������
 **/ 
extern uint32_t MAX_TELE;
extern CONVERT_MILE_NODE *cm_node;
#endif
