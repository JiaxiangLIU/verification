
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

/**
 * @function to_log_init
 * @brief 日志模块初始化
 **/ 
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
    
    to_log_init(); // jiaxiang: 日志初始化，不关心

    /* 线程池初始化 */
    thread_pool_init(); // jiaxiang: 申请线程池，没有新线程

    /* 消息队列初始化 */
    message_queue_init(); // jiaxiang: 没有特别的

    /* 调度初始化 */
    // jiaxiang: 在线程池中申请新线程运行schedule，注意这是空线程，啥都不做！
    schedule_init();

    /* socket通信初始化 */
    /*
     * jiaxiang: 与内CAN建立socket连接，建立类型为0~5的6个socket，并存在socket
     * 数组loc_sockets里。从线程池启动6个线程分别对每个类型的socket进行监听并处理，根据接收
     * 帧的类型执行frame_rcv_handle处理，若为命令帧，需要另启新线程。
     */
    socket_comm_init();

    return 0;
}

static inline void to_log_init(void)
{
    int ret = log_init("to_log.conf", "my_cat");
    printf("to_log_init ret: %d\n", ret);
}

static inline void thread_pool_init(void)
{
    pool = threadpool_create(THREAD_NUM, MAX_TASK_NUM, THREADPOOL_FLAG_COMMON);
    if (pool == NULL)
    {
        fprintf(stderr, "failed to create thread pool.\n");
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

    // jiaxiang: 把地址设置成内CAN端口
    set_board_addr(0);  //0代表COMM_0 

    /*
     * jiaxiang: 注册类型为SOCKET_TYPE_0～5的socket，并将该socket加入全局的
     * socket数组loc_sockets里。
     */
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


    /*
     * jiaxiang: 启动单独线程监听类型为SOCKET_TYPE_0的socket，并对到来的
     * 信息进行解析和处理。分command和data，如果是command，则启动新线程运行
     * command_frame_rcv_handle函数处理；如果是data，则直接data_frame_rcv_handle
     * 函数处理。
     *
     * 接下来的操作是分别对各类型的socket进行单独监听和处理。
     */
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
