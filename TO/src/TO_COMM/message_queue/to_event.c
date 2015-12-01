
#include <stdlib.h>
#include "to_event.h"
#include "to_status.h"

struct event_t *event_create(EVENT_TYPE type, EVENT_SOURCE  source, uint8_t parameter)
{
    struct event_t *event;
    if((event = (struct event_t *)malloc(sizeof(struct event_t))) == NULL)
    {
        return NULL;
    }
    event->type = type;
    event->source = source;
    event->parameter = parameter;
    event->priority = 1;    /*没有事件表, 事件优先级暂定为1*/ 
    return event;
}

void event_delete(struct event_t *event)
{
    free(event);
}
