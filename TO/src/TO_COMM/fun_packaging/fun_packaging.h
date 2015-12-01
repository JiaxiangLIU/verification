#ifndef FUN_PACKAGING_H
#define FUN_PACKAGING_H
#include "rt_definition.h"
#include "new_added.h"
#define PARSE_STATUS_INFO_SUCCESS 0
#define PARSE_STATUS_INFO_ERR -1

int parse_status_info(int canid, char *recv_buf);

/*
*led flicker when received realtime_info
*/
void led_flicker(char *led_status, int gpiox_num);
void send_trip_info(TRIP_INFO *trip_info_buf, TRIP_INFO *trip_info_send);
void send_jieshi_info();

/*
**@brief:get time from realtime_info then send out
*/
void get_time_then_send(REALTIME_DATA real_time_struct);

#endif
