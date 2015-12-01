

#ifndef EVENT_H_
#define EVENT_H_

#include <stdint.h>

#include "to_status.h"


typedef enum event_manage_result
{
    EVENT_MANAGE_SUCCESS = 0,
    EVENT_MANAGE_FAILURE = -1
}EVENT_MANAGE_RESULT;

typedef enum event_status
{
    READY = 0,      /*创建完成*/
    WAITING = 1,    /*在消息队列内等待*/
    RUNNING = 2,    /*正在运行*/
    FINISH = 3,     /*完成*/
    EXCEPTION = 4   /*异常*/
}EVENT_STATUS;
#if 0
typedef enum event_type
{
	/*通信版事件*/
	EVENT_ROUTE_RECEIVE_COMPLETE = 0x01,
	EVENT_ROUTE_INIT_BREAK = 0x02,
	//EVENT_AIR_BRAKE_START = 0x03, 空气制动启动
	EVENT_ANNOUNCE_INFO_GET_FAILURE = 0x04,
	//EVENT_SIGNAL_LIGHT_CHANGE = 0x05, 前方信号灯变化

	/*核心板事件*/
	EVENT_ROUTE_INIT_COMPLETE = 0x51,
	EVENT_ROUTE_INIT_SUSPEND = 0x52,

	/*控制盒事件*/
	EVENT_ROUTE_INIT_START = 0x91,
	//手自动转换另起新的线程来处理，不用消息队列schedule调度处理
	//EVENT_MANUAL_TO_AUTO = 0x92,
	//EVENT_AUTO_TO_MANUAL = 0x93,

	/*输入版事件*/
	EVENT_HANDLE_CHANGE = 0xC1,

    EVENT_FINISH = 0XFF     //event_finish是一种特殊的事件，代表旧事件结束
}EVENT_TYPE;
#endif
/*
typedef enum event_source
{
	COMM_EVENT = 0x00,
	CORE_EVENT = 0x01,
	CONTROL_BOX = 0x02,
	DRIVER_CONTROLLER = 0x03,
}EVENT_SOURCE;*/

struct event_t
{
    //uint8_t id; 考虑不加id
	//uint8_t id;

    EVENT_TYPE type;
    uint8_t priority;
    EVENT_SOURCE source;
    uint8_t parameter;  /*参数，可不填*/
    EVENT_STATUS status;

    void *routine;
    void *data;
};

/**
 * @function event_create
 * @brief 创建一个新事件
 * @param type 事件类型 确定事件的其他属性
 * @param source 事件源
 * @param parameter 事件携带的参数，可选填
 * @return 成功返回事件结构，失败返回NULL
 **/ 
struct event_t *event_create(EVENT_TYPE type, EVENT_SOURCE  source, uint8_t parameter, uint8_t priority);

/**
 * @function event_delete
 * @brief 删除事件
 * @param event 目标对象
 * @return 无返回值
 **/ 
void event_delete(struct event_t *event);
#endif
