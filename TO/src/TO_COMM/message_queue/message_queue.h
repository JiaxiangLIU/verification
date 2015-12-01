
/**
 * @file message_queue
 * @brief 定义消息队列的结构
 * @author danny
 * @date 2015.1.28
 * @note 消息队列使用方法: queue_create创建消息队列
 *                         enqueue消息进队列
 *                         dequeue消息出队列
 **/  


#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_
    
#include <stdint.h>

#include "message_queue.h"


typedef enum queue_manage_result
{
    QUEUE_MANAGE_SUCCESS = 0,
    QUEUE_MANAGE_FAILURE = -1,
    QUEUE_CREATE_FAILURE = 1,
    QUEUE_NOT_EXIST = 22,
    QUEUE_EMPTY = 23,
    LOCK_FAILURE = 70,
    QUEUE_MALLOC_FAILURE = 71
}QUEUE_MANAGE_RESULT;


struct message_node
{
    struct message_node *pre;
    struct message_node *next;
    void *value;
};
typedef struct message_node MESSAGE_NODE;

struct message_queue
{
    MESSAGE_NODE *q_front;  
    MESSAGE_NODE *q_rear;
    uint8_t length;
    uint8_t priority;
};
typedef struct message_queue MESSAGE_QUEUE;

struct multilevel_queue_t
{
    MESSAGE_QUEUE *queues;  /* 队列数组，按优先级从高到低排列 */
    uint8_t number; /* 队列个数 */
};


/**
 * @function queue_create
 * @brief 创建一个多级消息队列结构
 * @param number 队列个数
 * @return 返回多级消息队列结构对象
 **/ 
struct multilevel_queue_t *queue_create(uint8_t number);


/**
 * @function queue_destroy
 * @brief 销毁一个多级队列结构
 * @param multi_queue 指定的多级队列结构
 * @return 操作结果，成功为0
 **/
QUEUE_MANAGE_RESULT queue_destroy(struct multilevel_queue_t *multi_queue);


/**
 * @function queue_clear_all
 * @brief 清空多级队列结构
 * @param multi_queue 清空对象
 * @return 成功返回0
 **/
QUEUE_MANAGE_RESULT queue_clear_all(struct multilevel_queue_t *multi_queue);



/**
 * @function enqueue
 * @brief 消息结点入队操作
 * @param multi_queue 多级队列结构
 * @param node 消息节点，一般来说事件
 * @param priority 节点的优先级属性，决定进哪个队列
 * @return 成功返回0，若队列不存在返回QUEUE_NOT_EXIST，若参数不符合要求返回QUEUE_MANAGE_FAILURE
 **/
QUEUE_MANAGE_RESULT enqueue(struct multilevel_queue_t *multi_queue, void *value, uint8_t priority);



/**
 * @function dequeue
 * @brief 消息队列出队操作，按照优先级的顺序进行选择
 * @param multi_queue 目标多级队列结构
 * @param ret 操作结果
 * @return 消息节点中的值value
 **/
void *dequeue(struct multilevel_queue_t *multi_queue, int *ret);
#endif
