
/**
 * @file frame_rcv_handle.c
 * @brief 进行frame接收后的操作
 * @author danny
 * @date 2015.02.03
 **/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <to_event.h>
#include "handle.h"
#include "global.h"
#include "schedule.h"
#include "comm_check.h"
#include "log.h"
#include "to_status.h"
#include "handle.h"
#include "vote_event_handle.h"
#include "protocal.h"
#include "rt_definition.h"
#include "vote_status.h"
#include "comm_check.h"
#include "to_time.h"

void command_frame_rcv_handle(void *cmd) {
	char *command = (char *) cmd;
	struct event_t *new_event;
	// 打印command 的三个字段
	log_info("frame_rcv_handle.c:command frame handle begin: %x %x %x\n",
			command[0], command[1], command[2]);

	//
	if (command[1] == 0x00) {
		comm_check_feedback(command);
	} else {
		//betavote

		char command_vote[2];
		command_vote[0] = VOTE_0_SOURCE;
		char arg;
		int gear_current;
		// comm board
		if (command[0] == COM_0_SOURCE || command[0] == COM_1_SOURCE) {
			EVENT_TYPE com_event = command[1];
			switch (command[1]) {
			case EVENT_COM_REGISTER:
				log_info("com register\n");
				comm_register(command[0]);
				// comm addr
				break;
			case EVENT_COM_HEARTBEAT:
			case EVENT_LKJ_VERSION_DIFFERENCE:
				arg = command[2];
				log_info("communication version conflicts  %d", arg);
				command_vote[1] = EVENT_LOCK_SCREEN;
				if (frame_encap(COM_SOC, act_con, COMMAND_FRAME_CODE,
						command_vote, &arg, 1) != PROTOCAL_SUCCESS) {
					log_error("command to ctrlbox lkj error\n");
				}
				break;
			case EVENT_TRIP_STATUS_NOTICE:
				log_info("trip_status change\n");
				set_trip_status(command[2]);
//		  		  new_event = event_create(EVENT_TRIP_STATUS_NOTICE, COMM_EVENT, command[2],0);
//		  		  if(new_event == NULL) {
//		  				log_error("system err: failed to creat event.\n");
//		  			}
//		  			add_command(new_event);
				broadcast_trip_status();
				break;
			case EVENT_DUTY_ASSISTING_CHANGE:
				log_info("duty changed\n");
				set_train_status(command[2]);
				// real time send it.
				break;


			}
		}
		if (command[0] == CORE_0_SOURCE || command[0] == CORE_1_SOURCE
				|| command[0] == CORE_2_SOURCE || command[0] == CORE_3_SOURCE) {
			EVENT_TYPE core_event = command[1];
			switch (core_event) {
			case EVENT_CORE_REGISTER:
				log_info("core register\n");
				comm_register(command[0]);
				break;
			case EVENT_CORE_HEARTBEAT:
				//case EVENT_MASTER_CORE_EXCHANGE:
			case EVENT_TRIP_OPT_COMPLETE:
				log_info("optimize  over\n");
				set_trip_status(TRIP_START);
				broadcast_trip_status();
				// xing cheng bian hao
				led_on(44);
				break;
			}
		} else if (command[0] == CONTROL_0_SOURCE
				|| command[0] == CONTROL_1_SOURCE) {
			EVENT_TYPE control_event = command[1];
			signed char gear;
			switch (control_event) {
			case EVENT_CTRL_BOX_REGISTER:
				log_info("control++++++++++++++ register\n");
				comm_register(command[0]);
				break;
			case EVENT_CTRL_BOX_HEARTBEAT:
			case EVENT_CTRL_BOX_MASTER_SET:
				set_ctrl_status(CTRL_SET);
				char cmd[2];
				cmd[0] = command[0];
				cmd[1] = command[1];
				frame_encap(COM_SOC, CON_0_DES, COMMAND_FRAME_CODE, cmd,
						&command[2], 1);
				if (command[0] == CONTROL_1_SOURCE)     // initial con0
					act_con = CON_1_DES;
				break;
			case EVENT_CTRL_BOX_MASTER_RELIEVE:
			case EVENT_GEAR8_AUTO_CONFIRM_ACK:
				log_info("second gear ack\n");
				set_auto_ack_status(1);
				break;
			case EVENT_TWO_GEAR_TO_MANNUL:     //marshalling ack
				log_info("second gear ack\n");
				set_manual_ack_status(1);
				break;
			case EVENT_SYS_TO_MAINTENANCE:
				set_sys_status(SYS_MAINTENANCE);
				broadcast_sys_status();
				break;
			case EVENT_EXIT_MAINTENANCE:
				// exit maintaince
				set_sys_status(SYS_STANDBY);
				broadcast_sys_status();
				break;
			case EVENT_KEEP_AUTO:
				log_info("auto test\n");
				if (get_sys_status() != SYS_MAINTENANCE)
					log_warning("the status is not maintain state\n");
				else {
					log_info("auto test\n");
					auto_test(8);
				}
				break;
			case EVENT_SEND_AUTO_GEAR:
				if (get_sys_status() != SYS_MAINTENANCE)
					log_warning("the status is not maintain state\n");
				else {
					log_info("send gear\n");
					gear = command[2];
					auto_test((int) gear);
				}
				break;
			case EVENT_KEEP_MANNAL:
				if (get_sys_status() != SYS_MAINTENANCE)
					log_warning("the status is not maintain state\n");
				else {
					log_info("mannual test \n");
					mannual_test();
				}
				break;
			case EVENT_TRIP_INIT_START:/*行程初始化启动*/
				if (get_trip_status() == TRIP_DATA_COMPLETE
						&& get_mar_status() == MAR_ACK
						&& get_train_status() == TRAIN_DUTY) {
					new_event = event_create(EVENT_TRIP_INIT_START, SOURCE_CONTROL_0,
							&command[2], 0);
					if (new_event == NULL) {
						log_error("system err: failed to creat event.\n");
					}
					add_command(new_event);
				} else {
					log_info(
							"condition is not satisfied ,trip_status %d,mar_status %d,train_status %d",
							get_trip_status(), get_mar_status(),
							get_train_status());
				}
				break;
			case EVENT_MANNAL_TO_AUTO:/*手动转自动，不放到消息队列中处理*/

				log_info(
						"手动转自动：frame_rcv_handle.c:command frame handle begin: %x %x %x\n",
						command[0], command[1], command[2]);
				if (get_trip_status() == TRIP_START
						&& get_train_status() == TRAIN_DUTY) {
					gear_current = get_throttle();
					set_to_auto_gear(gear_current);
					set_to_auto_status(1);
//			  			pthread_mutex_lock(&(manual_to_auto_flag.mta_mutex));
//			  			manual_to_auto_flag.mta_flag = 1;
//			  			pthread_cond_signal(&(manual_to_auto_flag.mta_cond));
//			  			pthread_mutex_unlock(&(manual_to_auto_flag.mta_mutex));

				} else
					log_info(
							"condition is not satisfied ,trip_status %d,mar_status %d,train_status %d",
							get_trip_status(), get_mar_status(),
							get_train_status());

				break;
			case EVENT_AUTO_TO_MANNAL:/*自动转手动，不放到消息队列中处理*/
				log_info(
						"自动转手动：frame_rcv_handle.c:command frame handle begin: %x %x %x\n",
						command[0], command[1], command[2]);
				if (get_trip_status() == TRIP_START
						&& get_train_status() == TRAIN_DUTY) {
					set_to_manual_status(1);
					printf("get_to_manual_status %d\n", get_to_manual_status());
//			  			pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//			  			auto_to_manual_flag.atm_flag = 1;
//			  			pthread_cond_signal(&(auto_to_manual_flag.atm_cond));
//			  			pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));
				} else
					log_info(
							"condition is not satisfied ,trip_status %d,mar_status %d,train_status %d",
							get_trip_status(), get_mar_status(),
							get_train_status());
				break;

			}
		}
	}
	free(command);
}

void data_frame_rcv_handle(char *data, uint8_t length, char *cmd, int amount, int count)
{
	AHEAD_BRANCH_ROUTE_INFO temp;
	AHEAD_SIDE_ROUTE_INFO side_temp;
    char *command = (char *)cmd;
    log_info("data frame rcv : %x %x\n", command[0], command[1]);
    if(command[0]==COM_0_SOURCE||command[0]==COM_1_SOURCE){
    	EVENT_TYPE com_data=command[1];
    	switch(com_data){
    	     case EVENT_MARSHALLING_RE_CHANGE:
    	    	 set_mar_status(MAR_ACK);
    	    	 marshall_handle(data,length);
    	    	 break;
    	     case EVENT_TRIP_CHANGE:
    	    	 trip_handle(data,length);
    	    	 break;
    	     case EVENT_MARSHALLING_CHANGE:
    	    	 jieshi_handle(data,length);
    	    	 break;
    	     case EVENT_FRONT_BRANCH_TRANSFER:
    	    	 set_branch_status(BRANCH_CHANGE);
    	    	 memcpy(&temp,data,sizeof(AHEAD_BRANCH_ROUTE_INFO));
    	    	 set_branch_info(temp);
    	    	 //todo sendto controbox
    	    	 ctrbox_data(com_data);
    	    	 break;
    	     case EVENT_FRONT_SIDE_LINE_PASS:
 		    	if(get_side_status()!=SIDE_INIT){
 		    		log_error("side info exit\n");
 		    		break;
 		    	}
    	    	 set_side_status(SIDE_START);
    	    	 memcpy(&side_temp,data,sizeof(AHEAD_SIDE_ROUTE_INFO));
    	    	 set_side_info(side_temp);
    	    	 ctrbox_data(com_data);
    	    	 break;
    	     case  EVENT_CLOCK_SYNC:
 				log_info("clock sync\n");
 				set_to_time(data,length);
    	}
    }
    if(command[0]==CORE_0_SOURCE||command[0]==CORE_1_SOURCE||command[0]==CORE_2_SOURCE||command[0]==CORE_3_SOURCE){
    	EVENT_TYPE core_data=command[1];
    	switch(core_data){
    	     case EVENT_REAL_TIME_CTRL:
//                 memcpy(&rt_data_temp,data,sizeof(RT_DATA));
//    	    		log_info("rt_handle 速度: %f 连续公里标: %f 信号机:%d  结构体大小:%d  长度： %d\n",
//    	    			rt_data_temp.rt_par.rt_v, rt_data_temp.rt_par.rt_cont_post,
//    	    			rt_data_temp.rt_par.tele_post.tel_num,  sizeof(RT_DATA),length);

    	    	 if(fsm_start==0){
    	    		 fsm_start=1;
    	    		 log_info("fsm_start began\n");
    	    	 }

    	    	rt_handle(data, length);
    	    	 break;
    	}
    }
    if(command[0]==CONTROL_0_SOURCE||command[0]==CONTROL_1_SOURCE){}

}
