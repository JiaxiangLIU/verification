

/**
 * @file to_comm.c
 * @brief 通信板业务进程入口
 * @author danny
 * @date 2015.1.26
 **/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "to_init.h"
#include "vote_init.h"
#include "log.h"
#include "to_status.h"
#include "vote_status.h"
#include "ctrbox_comm.h"
#include "vote_event_handle.h"
#include "pthread.h"
#include "control_box.h"
#include "throttle_handle.h"
#include "led.h"
#include "global.h"
#include "fsm.h"

#include "ack_handle.h"
RT_VOTE rt_vote;
RT_CORE rt_core;
RT_VOTE fsm_rt;
RT_VOTE fsm_temp;



pthread_mutex_t rt_mutex = PTHREAD_MUTEX_INITIALIZER;
TEMP_GEAR temp_gear = {PTHREAD_MUTEX_INITIALIZER, 0};
MANUAL_TO_AUTO_FLAG manual_to_auto_flag = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};
AUTO_TO_MANUAL_FLAG auto_to_manual_flag = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};
CTRL_STATUS_FLAG ctrl_status_flag = {PTHREAD_MUTEX_INITIALIZER, 0};
THROTTLE_GEAR throttle_gear1 = {PTHREAD_MUTEX_INITIALIZER, 0, 0};
int second_ack=0;
int auto_change=0;


int act_com=COM_0_DES;
int act_con=CON_0_DES;

// maintain state led 43


int main(void)
{
   ////////////////////////////////////////
	led_open();
	led_off(led_card_nomal_1);
	led_off(led_card_nomal_2);
	led_off(led_sys_nomal_1);
	led_off(led_sys_maintain);
	led_off(led_sys_wait);
	led_off(led_sys_incan);

	init_ack_and_resend_table();


    to_init();
    /* 创建与控制盒通信的线程 */
    int ret1;
	pthread_t tid1;
	pthread_attr_t attr1;

	if ((pthread_attr_init(&attr1)) < 0) {

		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PTHREAD_CREATE));
		return ERROR_CODE_PTHREAD_CREATE;
	}
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
	ret1 = pthread_create(&tid1, &attr1, control_box_thread, NULL);

	if (ret1 != 0 ) {
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PTHREAD_CREATE));
		return ERROR_CODE_PTHREAD_CREATE;
	}

	/* 创建与输入输出板通信的线程 */
	int ret4;
	pthread_t tid4,tid5;
	pthread_attr_t attr4;

	if ((pthread_attr_init(&attr4)) < 0) {

		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PTHREAD_CREATE));
		return ERROR_CODE_PTHREAD_CREATE;
	}
	pthread_attr_setdetachstate(&attr4, PTHREAD_CREATE_DETACHED);

	ret4 = pthread_create(&tid4, &attr4, receive_throttle, NULL);
	ret4 = pthread_create(&tid5, &attr4, sent_throttle, NULL);

	if (ret4 != 0 ) {
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PTHREAD_CREATE));
		return ERROR_CODE_PTHREAD_CREATE;
	}


	   int ret2;
		pthread_t tid2;
		pthread_attr_t attr2;

		if ((pthread_attr_init(&attr2)) < 0) {

			log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PTHREAD_CREATE));
			return ERROR_CODE_PTHREAD_CREATE;
		}
		pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);
		ret2 = pthread_create(&tid2, &attr2, fsm_run, NULL);

		if (ret2 != 0 ) {
			log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_PTHREAD_CREATE));
			return ERROR_CODE_PTHREAD_CREATE;
		}

	    int led_i;
	    for(led_i=1;led_i<13;led_i++)
	    	VoteLedCtrl(led_i , 0);



		rt_vote.rt_control_status=RT_MANNUAL;
	    fsm_rt=rt_vote;
		fsm_temp=rt_vote;
#if 1
    if (comm_check_init() == -1) {
        log_error("comm check init failed");
        exit(0);
    }
    log_info("comm check init success.");

#endif
    start_heartbeat(0x08,0x02);
    set_sys_status(SYS_LAUNCHED);
    broadcast_sys_status();

    //
//    MARSHALLING_INFO mar_temp={0,0,0,0,0};
//    set_mar_info(mar_temp);


    sleep(5);
	led_on(led_card_nomal_1);
	led_on(led_card_nomal_2);
	led_on(led_sys_nomal_1);
    if(act_com==COM_0_DES)
        broadcast_actboard(0x00);
    else
    	broadcast_actboard(0x01);


    while(get_ctrl_status()==CTRL_UNSET){
    	log_info("control is not set\n");
    	//broadcast_sys_status();
    	//sleep(2);
    }
    log_info("control is  set^^^^^^^^^^^^^^^^^^^^^\n");

    int tmp1,tmp2;
	write_sent_throttle_queue(0, 0);
	read_recv_throttle_queue(&tmp1,&tmp2);


    sleep(2);
    set_sys_status(SYS_STANDBY);
    broadcast_sys_status();
    led_on(led_sys_wait);

#if 0
    if (route_init() == -1) {
        log_error("route init err.");
    }
#endif

    int ret;
    while(1){
    	printf("receive throttle\n");
   	if(get_trip_status()==TRIP_START){
    	  pthread_mutex_lock(&throttle_gear1.throttle_gear_mutex);
    	  ret=read_recv_throttle_queue(&(throttle_gear1.gear), &(throttle_gear1.auto_or_manual));
    	  pthread_mutex_unlock(&throttle_gear1.throttle_gear_mutex);
    	  }
    	usleep(10000);
    };
    return 0;
}
