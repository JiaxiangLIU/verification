
#ifndef _REALTIME_DATA_H_
#define _REALTIME_DATA_H_

#include <pthread.h>
#include <stdint.h>
#include "rt_definition.h"

/*声明产生事件的结构体*/
/*
typedef struct _realtime_event_flag {
	int8_t brake_output_flag;
	int8_t monitor_situation_flag;
} RT_EVENT_FLAG;
*/

typedef struct {
    pthread_mutex_t rt_mutex;
    pthread_cond_t rt_cond;
    uint8_t rt_flag;
}RT_ARR_FLAG;   //有实时信息到达时的flag

/*typedef enum {
    SIG_GREEN_LIGHT = 0,
    SIG_GREEN_YELLOW_LIGHT = 1,
	SIG_YELLOW_LIGHT = 2,
	SIG_YELLOW_2_LIGHT = 3,
	SIG_DOUBLE_YELLOW_LIGHT = 4,
	SIG_RED_YELLOW_LIGHT = 5,
	SIG_RED_LIGHT = 6,
	SIG_WHITE_LIGHT = 7,
	SIG_MAX = 8,
	SIG_RESERVED = 15
} TRAIN_SIG;

typedef enum {
    BRAKE_UNLOAD = 0,
    BRAKE_AIR_PRESSURE = 1,
	BRAKE_AIR_SHUTOFF = 2,
	BRAKE_MAX = 3,
	BRAKE_EMERGENCY = 6,
	BRAKE_RESERVED = 7
} BRAKE_OUTPUT;
*/
/*声明存放速度和公里标的结构体*/
/*
typedef struct _speed_and_location {
	int16_t speed;//时速
	int32_t kilmeter_coordinate;//公里标
	int32_t new_kilmeter_coordinate;//公里标
	int16_t singnal_equipment_No;
	int16_t distance;
} SPEED_AND_LOCATION;

typedef struct _realtime_data {
	RT_EVENT_FLAG event_flag;
	SPEED_AND_LOCATION speed_and_location;
	BRAKE_OUTPUT brake_out;
	TRAIN_SIG train_sig;
} RT_DATA_REV;


*/

extern RT_ARR_FLAG rt_arr_flag;
extern RT_COMM rt_comm;


#endif
