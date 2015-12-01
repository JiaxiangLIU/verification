
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "schedule.h"
#include "../threadpool/threadpool.h"
#include "../handle.h"
#include "../global.h"
#include "log.h"
//#include "vote_event_handle.h"

//#include "test_pr.h"

SCHEDULE_RESULT add_command(struct event_t *event)
{
	enqueue(queues, (void *)event, event->priority);
	fprintf(stderr, "add_command complete.\n");
    return SCHEDULE_SUCCESS;    
}


static inline void event_handle(struct event_t *event)
{
/*
    switch(event->type)
    {
        case EVENT_ROUTE_RECEIVE_COMPLETE:
            printf("event route receive complete.\n");
            threadpool_add(pool, &route_receive_complete, event, 0);
            break;
        case EVENT_ROUTE_INIT_BREAK:
			printf("event route init break.\n");
			threadpool_add(pool, &route_init_break, event, 0);
			break;
        case EVENT_ANNOUNCE_INFO_GET_FAILURE:
			printf("announce information get failure.\n");
			threadpool_add(pool, &get_announce_info_failure, event, 0);
			break;
        case EVENT_ROUTE_INIT_COMPLETE:
			printf("route init complete.\n");
			threadpool_add(pool, &route_init_complete, event, 0);
			break;
        case EVENT_ROUTE_INIT_SUSPEND:
			printf("route init suspend.\n");
			threadpool_add(pool, &route_init_suspend, event, 0);
			break;
        case EVENT_ROUTE_INIT_START:
			printf("route init start.\n");
			threadpool_add(pool, &route_init_start, event, 0);
			break;
        case EVENT_FINISH:
            break;
    }
*/
}
void schedule(void *arg)
{
    struct event_t *loc_event, *next_event;
    struct multilevel_queue_t *multi_queue;
    int ret;

    multi_queue = (struct multilevel_queue_t *)arg;
    if (multi_queue == NULL) {
        fprintf(stderr, "system err: queue not exist.\n");
        /*********************************************/
        /****************TODO: ERR_HANDLE*************/
    }
	
    while (1) {
    	loc_event = (struct event_t *)dequeue(multi_queue, &ret);
		if (ret == QUEUE_MANAGE_SUCCESS) {
			event_handle(loc_event); //need free loc_event
			//event_delete(loc_event);
        }
        else if (ret == QUEUE_EMPTY) {
        }
        else if (ret == LOCK_FAILURE) {
        }
        else if (ret == QUEUE_NOT_EXIST) {
        }
		usleep(10000);
	}
}




