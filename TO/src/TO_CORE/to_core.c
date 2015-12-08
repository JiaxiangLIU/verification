

/**
 * @file to_core.c
 * @brief 核心板业务进程入口
 * @author danny
 * @date 2015.2.4
 **/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "to_init.h"
#include "log.h"
#include "comm_check.h"
#include "core_init.h"
#include "to_status.h"
#include "led.h"
#include "socket_buf.h"
#include "ack_handle.h"



static int led_init(void)
{	
	static int num;		
	int i;
	
	num = led_count();	

	if (num > 0) {
		led_open();		
		for (i = 0; i < num; i++)
			led_off(i);
	}	
}

extern pthread_mutex_t socket_send_mutex;


int main(void)
{	
   led_init();
 //  led_on(NORMAL_LED);
   led_on(VOTE_LED);
   led_off(COMMU_LED);
	 led_off(OPT_LED);
	 init_ack_and_resend_table();
   to_init();
#if 1
   // jiaxiang: 向表决板发送注册信息
   if (comm_register() == -1)
   {
        log_error("communication check err.");
        exit(0);
   }
   printf("comm register success.\n"); 
#endif

    // jiaxiang: 启动心跳进程heartbeat_check，暂时不关心
	comm_check_init();

	// jiaxiang: HERE!!!
    int temp;
	SYSTEM_STATUS temp_status;
    do {
        temp = get_sys_status();
    } while (temp < SYS_STANDBY);    

	TRIP_STATUS temp_trip;
	do {
		temp_trip = get_trip_status();
	} while (temp_trip < TRIP_START || temp_trip == TRIP_MAX);
	
    auto_control();
    while (1) {
		pthread_mutex_lock(&socket_send_mutex);
		send_realtime_buf();
		pthread_mutex_unlock(&socket_send_mutex);
		temp_status = get_sys_status();
		if (temp_status == SYS_FAULT || temp_status == SYS_MAINTENANCE)
			led_off(NORMAL_LED);
		else
			led_on(NORMAL_LED);
	}
    return 0;
}


