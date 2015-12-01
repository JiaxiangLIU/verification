

#ifndef EVENT_H_
#define EVENT_H_

#include <stdint.h>

#include "to_event.h"
#include "to_status.h"


typedef enum event_manage_result
{
    EVENT_MANAGE_SUCCESS = 0,
    EVENT_MANAGE_FAILURE = -1
}EVENT_MANAGE_RESULT;

typedef enum event_status
{
    READY = 0,      /*�������*/
    WAITING = 1,    /*����Ϣ�����ڵȴ�*/
    RUNNING = 2,    /*��������*/
    FINISH = 3,     /*���*/
    EXCEPTION = 4   /*�쳣*/
}EVENT_STATUS;

/*
typedef enum event_type
{
    EVENT_ROUTE_EXTRACT = 0X10,
    EVENT_FINISH = 0XFF     //event_finish��һ��������¼���������¼�����
}EVENT_TYPE;
*/


/*has been defined in to_status.h
typedef enum event_source
{
    REALTIME_INFO = 0x01,
    CONTROL_BOX = 0x02,
    DRIVER_CONTROLLER = 0x03,
    FINISHED_EVENT = 0X04
}EVENT_SOURCE;
*/

struct event_t
{
    uint8_t id;
    EVENT_TYPE type;
    uint8_t priority;
    EVENT_SOURCE source;
    uint8_t parameter;  /*parameter, can choose to not fill*/
    EVENT_STATUS status;

    void *routine;
    void *data;
};

/**
 * @function event_create
 * @brief creat a new event
 * @param type event type, confirm other property of the event
 * @param source event source
 * @param parameter event patameter
 * @return return event_t struct when success, return NULL when failed
 **/ 
struct event_t *event_create(EVENT_TYPE type, EVENT_SOURCE source, uint8_t parameter);

/**
 * @function event_delete
 * @brief ɾ���¼�
 * @param event Ŀ�����
 * @return �޷���ֵ
 **/ 
void event_delete(struct event_t *event);
#endif
