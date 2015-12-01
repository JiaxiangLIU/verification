
#include <stdlib.h>
#include "to_event.h"

struct event_t *event_create(EVENT_TYPE type, EVENT_SOURCE  source, uint8_t parameter, uint8_t priority)
{
    struct event_t *event;
    if((event = (struct event_t *)malloc(sizeof(struct event_t))) == NULL)
    {
        return NULL;
    }
    event->type = type;
    event->source = source;
    event->parameter = parameter;
    event->priority = priority;
    return event;
}

void event_delete(struct event_t *event)
{
    free(event);
}
