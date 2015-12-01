
/**
 * @file to_init.c
 * @brief 初始化进程
 * @author danny
 * @date 2015.5.13
 **/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "app_socket.h"
#include "schedule.h"
#include "protocal.h"
#include "log.h"

threadpool_t *pool;                 //定义全局线程池
struct multilevel_queue_t *queues;  //定义全局消息队列

static inline void to_log_init(void);
/**
 * @fucntion thread_pool_init
 * @brief 线程池初始化
 **/ 
static inline void thread_pool_init(void);

/**
 * @function message_queue_init
 * @brief 消息队列初始化
 **/ 
static inline void message_queue_init(void);

/**
 * @function schedule_init
 * @brief 调度线程初始化
 **/ 
static inline void schedule_init(void);

/**
 * @function socket_comm_init
 * @brief 建立socket
 **/ 
static inline void socket_comm_init(void);

int to_init(void) {

    to_log_init(); // jiaxiang: log初始化，不关心

    /* 线程池初始化 */
    thread_pool_init(); // jiaxiang: 申请线程池（全局变量pool），并没有任何线程运行

    /* 消息队列初始化 */
    message_queue_init();
    /*
     * jiaxiang: 申请队列（全局变量queues）。根据schedule的状况，可知queues只包含
     * 单一类型事件。
     */

    /* 调度初始化 */
    schedule_init();
    /*
     * jiaxiang: 启了一个线程运行schedule函数，schedule函数轮询queues，
     * 从queues中取event，执行event_handle()处理该event。利用event_handle()
     * 处理事件时会启动一个单独线程。注意：目前这个schedule函数/线程形同虚设，它只
     * 处理一种事件：EVENT_TRIP_INIT_START（控制盒行程初始化启动命令），然后就
     * 无限循环。
     */

    /* socket通信初始化 */
    socket_comm_init();
    
    return 0;
}

static inline void to_log_init(void) 
{
    int ret = log_init("./to_log.conf", "my_cat");
    printf("to_log_init: %d\n", ret);
}

static inline void thread_pool_init(void)
{
    pool = threadpool_create(THREAD_NUM, MAX_TASK_NUM, THREADPOOL_FLAG_COMMON);
    if (pool == NULL)
    {
        log_error("failed to create thread pool.\n");
        exit(0);
    }
 
}

static inline void message_queue_init(void)
{
    queues = queue_create(QUEUE_NUM);
    if (queues == NULL)
    {
        fprintf(stderr, "system err: failed to create queues.\n");
        exit(0);
    }
}

static inline void schedule_init(void)
{
    if (threadpool_add(pool, schedule, queues, 0) != 0)
    {
        fprintf(stderr, "system err: failed to start schedule thread.\n");
        exit(0);
    }
}

static inline void socket_comm_init(void)
{
    SOCKET_RESULT  sock_ret;
    uint16_t type, type_0, type_1, type_2, type_3, type_4, type_5;
    int     ret;

    set_board_addr(0);  //0代表COMM_0 

    type = SOCKET_TYPE_0;
    sock_ret = socket_connect(type); 
    if (sock_ret == SOCKET_CONNECT_FAILURE)
    {
        fprintf(stderr, "failed to connect socket.\n");
        exit(0);
    }
    else if (sock_ret == REGISTER_FAILURE)
    {
        fprintf(stderr, "failed to register.\n");
        exit(0);
    }    
   
    type = SOCKET_TYPE_1;
    sock_ret = socket_connect(type); 
    if (sock_ret == SOCKET_CONNECT_FAILURE)
    {
        fprintf(stderr, "failed to connect socket.\n");
        exit(0);
    }
    else if (sock_ret == REGISTER_FAILURE)
    {
        fprintf(stderr, "failed to register.\n");
        exit(0);
    }    
   
    type = SOCKET_TYPE_2;
    sock_ret = socket_connect(type); 
    if (sock_ret == SOCKET_CONNECT_FAILURE)
    {
        fprintf(stderr, "failed to connect socket.\n");
        exit(0);
    }
    else if (sock_ret == REGISTER_FAILURE)
    {
        fprintf(stderr, "failed to register.\n");
        exit(0);
    }    
    
    type = SOCKET_TYPE_3;
    sock_ret = socket_connect(type); 
    if (sock_ret == SOCKET_CONNECT_FAILURE)
    {
        fprintf(stderr, "failed to connect socket.\n");
        exit(0);
    }
    else if (sock_ret == REGISTER_FAILURE)
    {
        fprintf(stderr, "failed to register.\n");
        exit(0);
    }    
    
    type = SOCKET_TYPE_4;
    sock_ret = socket_connect(type); 
    if (sock_ret == SOCKET_CONNECT_FAILURE)
    {
        fprintf(stderr, "failed to connect socket.\n");
        exit(0);
    }
    else if (sock_ret == REGISTER_FAILURE)
    {
        fprintf(stderr, "failed to register.\n");
        exit(0);
    }    
    

    type = SOCKET_TYPE_5;
    sock_ret = socket_connect(type); 
    if (sock_ret == SOCKET_CONNECT_FAILURE)
    {
        fprintf(stderr, "failed to connect socket.\n");
        exit(0);
    }
    else if (sock_ret == REGISTER_FAILURE)
    {
        fprintf(stderr, "failed to register.\n");
        exit(0);
    } 

    type_0 = 0x80;
    ret = threadpool_add(pool, socket_rcv, (void *)&type_0, 0);    
    if (ret != 0)
    {
        fprintf(stderr, "failed to create socket_rcv.\n");
        exit(0);
    }


    type_1 = 0x40;
    ret = threadpool_add(pool, socket_rcv, (void *)&type_1, 0);    
    if (ret != 0)
    {
        fprintf(stderr, "failed to create socket_rcv.\n");
        exit(0);
    }


    type_2 = 0x20;
    ret = threadpool_add(pool, socket_rcv, (void *)&type_2, 0);    
    if (ret != 0)
    {
        fprintf(stderr, "failed to create socket_rcv.\n");
        exit(0);
    }


    type_3 = 0x10;
    ret = threadpool_add(pool, socket_rcv, (void *)&type_3, 0);    
    if (ret != 0)
    {
        fprintf(stderr, "failed to create socket_rcv.\n");
        exit(0);
    }


    type_4 = 0x08;
    ret = threadpool_add(pool, socket_rcv, (void *)&type_4, 0);    
    if (ret != 0)
    {
        fprintf(stderr, "failed to create socket_rcv.\n");
        exit(0);
    }
 
    type_5 = 0x04;
    ret = threadpool_add(pool, socket_rcv, (void *)&type_5, 0);    
    if (ret != 0)
    {
        fprintf(stderr, "failed to create socket_rcv.\n");
        exit(0);
    }
    /*延迟1s, 等待所有接收程序启动*/
    sleep(1);
}
