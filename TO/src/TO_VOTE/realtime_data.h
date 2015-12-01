
#ifndef _REALTIME_DATA_H_
#define _REALTIME_DATA_H_

#include <stdint.h>

/*声明产生事件的结构体*/
typedef struct _realtime_event_flag {
	int8_t brake_output_flag;
	int8_t monitor_situation_flag;
} RT_EVENT_FLAG;

#endif
