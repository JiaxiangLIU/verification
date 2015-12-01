/*
 * vote_event_handle.c
 *
 *  Created on: Aug 27, 2015
 *      Author: liuyan
 */

#include <to_event.h>
#include "vote_event_handle.h"
#include "ctrbox_comm.h"
#include "protocal.h"
#include "log.h"
#include "global.h"
#include "to_status.h"
#include "vote_status.h"


/* 设置 获取 本补状态 */
static pthread_mutex_t auto_test_mutex = PTHREAD_MUTEX_INITIALIZER;
static int auto_ack =1;


/**
 * 行程初始化启动，通知控制盒
 * */
void route_init_start(struct event_t *event) {
	log_info("行程初始化启动，通知控制盒\n");
	/* 控制盒发起行程初始化 */
    trip_ack_handle();
    mar_ack_handle();
    jieshi_ack_handle();

	char command[2];
	command[0] =VOTE_0_SOURCE;
	EVENT_TYPE event_code=EVENT_VOTE_TRIP_INIT_START;
	command[1] = (char)event_code;
	char arg = 0x00;
	PROTOCAL_RESULT res = frame_encap(COM_SOC, act_com, COMMAND_FRAME_CODE, command, &arg, 1);
	if (res != PROTOCAL_SUCCESS)
	{
		log_error("ROUTE INITIAL COMMAND frame encape err.");
	}
	set_trip_status(TRIP_INIT);
    broadcast_trip_status();
	//return res;
	/* 通知控制盒 */
	event_delete(event);
}

//0：手动，1：自动
void auto_test(int gear){
	int ret=1;
	int gear_store,mannual;
	while(ret>0){
		ret= read_recv_throttle_queue(&gear_store,&mannual);
	}
	write_sent_throttle_queue(gear, 1);
	read_recv_throttle_queue(&gear_store,&mannual);
	if(gear_store!=gear){
		log_error("gear input is wrong\n");
	}
    if(gear==9){
    	write_sent_throttle_queue(0, 0);
    	read_recv_throttle_queue(&gear_store,&mannual);
    	if(gear_store!=0){
    		log_error("gear input is wrong\n");
    	}
    }
}
void mannual_test(){
	log_info("mannual test\n");
	int ret=1;
	int gear_store,mannual;
    char cmd[2];
	cmd[0]=VOTE_0_SOURCE;
	cmd[1]=0x14;
	signed char arg;

	while(get_sys_status()==SYS_MAINTENANCE){
		ret= read_recv_throttle_queue(&gear_store,&mannual);
		if(ret>0){
			log_info("receive gear\n");
			arg=(signed char)gear_store;
			frame_encap(COM_SOC,CON_0_DES,COMMAND_FRAME_CODE,cmd,&arg,1);
		}
	}
}
