
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "message_queue.h"

static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static int queue_lock()
{
    return pthread_mutex_lock(&queue_mutex);
}
static int queue_unlock()
{
    return pthread_mutex_unlock(&queue_mutex);
}

struct multilevel_queue_t *queue_create(uint8_t number)
{
    int i; // for loop
    struct multilevel_queue_t *multi_queue;
    if (number <= 0)
    {
        fprintf(stderr, "queue err: the number of queue cant be less than zero.\n");
        return NULL;
    }
    if((multi_queue = (struct multilevel_queue_t *)malloc(sizeof(struct multilevel_queue_t))) == NULL)
    {
            fprintf(stderr, "system err: malloc failure.\n");
            return NULL;
    }

    multi_queue->number = number;
    if((multi_queue->queues = (MESSAGE_QUEUE *)malloc(sizeof(MESSAGE_QUEUE) * number)) == NULL)
    {
        fprintf(stderr, "system err: malloc failure.\n");
        free(multi_queue);
        return NULL;
    }
    
    for(i = 0 ; i < number ; i++)
    {
        multi_queue->queues[i].q_front = NULL;
        multi_queue->queues[i].q_rear = NULL;
        multi_queue->queues[i].length = 0;
        multi_queue->queues[i].priority = i;
    }
    return multi_queue;   
}

QUEUE_MANAGE_RESULT queue_destroy(struct multilevel_queue_t *multi_queue)
{
    if(multi_queue == NULL)
    {
        fprintf(stderr, "queue is already not exist.\n");
        return QUEUE_MANAGE_SUCCESS;
    }
    else
    {
       queue_clear_all(multi_queue);
       free(multi_queue->queues);
       free(multi_queue);
    }
    return QUEUE_MANAGE_SUCCESS; 
}


QUEUE_MANAGE_RESULT queue_clear_all(struct multilevel_queue_t *multi_queue)
{
    MESSAGE_NODE *index, *next;
    int i, j; //for loop
    if(multi_queue == NULL)
    {
        fprintf(stderr, "queue is not exist.\n");
        return QUEUE_MANAGE_SUCCESS;
    }
    
    for(i = 0 ; i < multi_queue->number ; i++)
    {
       index = multi_queue->queues[i].q_front;
       for (j = 0 ; j < multi_queue->queues[i].length; j++)
       {
            next = index->next;
            free(index);
            index = next;
       }
       multi_queue->queues[i].length = 0;
    }
    return QUEUE_MANAGE_SUCCESS; 
}

QUEUE_MANAGE_RESULT enqueue(struct multilevel_queue_t *multi_queue, void *value, uint8_t priority)
{
    MESSAGE_NODE *node;
    if (multi_queue == NULL)
        return QUEUE_NOT_EXIST;
    if (priority >= multi_queue->number || value == NULL)
    {
        fprintf(stderr, "queue err: queue of this priority not exist.\n");
        return QUEUE_MANAGE_FAILURE;
    }
    if ((node = (MESSAGE_NODE *)malloc(sizeof(MESSAGE_NODE))) == NULL) 
    {
        fprintf(stderr, "system err: malloc err.\n");
        return QUEUE_MALLOC_FAILURE;
    }
    if(queue_lock() != 0)
    {
        free(node);
        return LOCK_FAILURE;
    }
    node->value = value;
    if(multi_queue->queues[priority].length == 0)   /*消息队列为空*/
    {
        multi_queue->queues[priority].q_front = node;
        multi_queue->queues[priority].q_rear = node;
        multi_queue->queues[priority].length = 1;
        node->pre = node;
        node->next = NULL;
    }
    else
    {
        multi_queue->queues[priority].q_rear->next = node;
        node->pre = multi_queue->queues[priority].q_rear;
        multi_queue->queues[priority].q_rear = node;
        node->next = NULL;
        multi_queue->queues[priority].length ++;
    }
    if(queue_unlock() != 0)
    {
        free(node);
        return LOCK_FAILURE;
    }
    return QUEUE_MANAGE_SUCCESS;
}

void *dequeue(struct multilevel_queue_t *multi_queue, int *ret)
{
    int i; // for loop 
    MESSAGE_NODE *index;
    void *value;
    if(multi_queue == NULL || multi_queue->queues == NULL)
    {
        *ret = QUEUE_NOT_EXIST;
        return NULL;
    }
    if(queue_lock() != 0)
    {
        *ret = LOCK_FAILURE;
        return NULL;
    }

    for (i = 0 ; i < multi_queue->number ; i++)
    {
        if(multi_queue->queues[i].length > 0)
            break;
    }
    if ( i == multi_queue->number )
    {
        *ret = QUEUE_EMPTY;
        queue_unlock();
        return NULL;
    }
    
    index = multi_queue->queues[i].q_front;
    if (multi_queue->queues[i].length == 1)  /* 队列里只有一个节点 */
    {
        multi_queue->queues[i].q_front = NULL;
        multi_queue->queues[i].q_rear = NULL;
        multi_queue->queues[i].length = 0;
    }
    else
    {
        multi_queue->queues[i].q_front = index->next;
        multi_queue->queues[i].q_front->pre = multi_queue->queues[i].q_front;
        multi_queue->queues[i].length --;
    }
    
    value = index->value;
    free(index);
    
     
    if(queue_unlock() != 0)
    {
        *ret = QUEUE_EMPTY;
        return NULL;
    }
    *ret = QUEUE_MANAGE_SUCCESS;
    return value;
}
