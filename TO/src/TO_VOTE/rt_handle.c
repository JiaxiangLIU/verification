#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "handle.h"
#include "realtime_data.h"
#include "global.h"
#include "protocal.h"
#include "log.h"
#include "circ_buf.h"
#include "to_status.h"
#include "rt_definition.h"
#include "vote_status.h"
//#include "rt_definition.h"





static int wait_led_count=0;
static int led_on_flag=1;


void rt_handle(char *data, int length)
{
	wait_led_count++;
	if(wait_led_count==2){
		if(led_on_flag==1){
		   led_off(led_sys_wait);
		   led_on_flag=0;
		}
		else{
			led_on(led_sys_wait);
			led_on_flag=1;
		}
		wait_led_count=0;
	}
	log_info("+++++realtime data handle+++++\n");
	/**
	 * 进行逻辑判断，并向控制盒，输入输出板发送实时数据
	 * */
	pthread_mutex_lock(&(rt_mutex));
	memcpy(&rt_core, data, sizeof(RT_CORE));	//将接收到的数据赋值给rt_data全局变量
    rt_vote.rt_core=rt_core;
	rt_vote.rt_control_status=CONTROL_AUTO;
	pthread_mutex_unlock(&(rt_mutex));
	log_info("rt_handle 档位: %d 速度: %d优化速度 %d原始公里标 %d控制状态:%d结构体vote大小:%d \n",
	rt_vote.rt_core.rt_opt.rt_gear, rt_vote.rt_core.rt_comm.speed_and_location.speed,rt_vote.rt_core.rt_opt.opt_v, rt_vote.rt_core.rt_comm.speed_and_location.kilmeter_coordinate,
	rt_vote.rt_control_status,sizeof(RT_VOTE));


#if 0
	if(can_manual_to_auto()==1){
		set_auto_status(AUTO_ENABLE);
	}
	else{
		set_auto_status(AUTO_DISABLE);
	}
	if(get_branch_status()==BRANCH_CHANGE){
				if(branch_info.ahead_branch_route_cont_post_start
						-rt_vote.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate<0)
					set_branch_status(BRANCH_INIT);
	}
	if(get_side_status()==SIDE_START){
		if(side_info.ahead_side_route_cont_post_end
				-rt_vote.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate<0){
			int distance;
			distance=side_info.ahead_side_route_cont_post_end;
			set_side_status(SIDE_INIT);
			char side_end[2];
			side_end[0]=VOTE_0_SOURCE;
			side_end[1]=(char)EVENT_VOTE_SIDE_LINE_PASS_END;
			frame_encap(DAT_SOC,act_con,DATA_FRAME_CODE,side_end,&distance,sizeof(int));
		}
	}
	if(get_auto_status()==AUTO_ENABLE) {

				pthread_mutex_lock(&rt_mutex);
				rt_vote.rt_core.rt_status.rt_enable_status = RT_AUTO_ENABLE;
				pthread_mutex_unlock(&rt_mutex);
				set_to_manual_status(0);
	}
	else {
				pthread_mutex_lock(&rt_mutex);
				rt_vote.rt_core.rt_status.rt_enable_status = RT_AUTO_UNABLE;
				pthread_mutex_unlock(&rt_mutex);
				set_to_manual_status();
	}
    int emerge_flag=0;
	if(rt_vote.rt_core.rt_comm.brake_out== BRAKE_EMERGENCY){
		emerge_flag=1;
	}

	fsm_control(&rt_vote.rt_core.rt_opt.rt_gear, &rt_vote.rt_control_status,emerge_flag);

	log_info("rt_handle 档位: %d 速度: %d优化速度 %d原始公里标 %d控制状态:%d结构体vote大小:%d \n",
	    rt_vote.rt_core.rt_opt.rt_gear, rt_vote.rt_core.rt_comm.speed_and_location.speed,rt_vote.rt_core.rt_opt.opt_v, rt_vote.rt_core.rt_comm.speed_and_location.kilmeter_coordinate,
		rt_vote.rt_control_status,sizeof(RT_VOTE));

	char cmd[2];
	cmd[0] = VOTE_0_SOURCE;
	cmd[1] = (char)EVENT_DISP_REAL_TIME_INFO;
	if (frame_encap(DAT_SOC,act_con, DATA_FRAME_CODE, cmd, &rt_vote, sizeof(RT_VOTE))
			!= PROTOCAL_SUCCESS) {
		log_error("failed to encap frame");
		return;
	}
	write_sent_throttle_queue(rt_vote.rt_core.rt_opt.rt_gear, (int)rt_vote.rt_control_status);


//	char cmd[2];
//	int gear_temp;
////	pthread_mutex_lock(&throttle_gear1.throttle_gear_mutex);
//	gear_temp=throttle_gear1.gear;
////	pthread_mutex_unlock(&throttle_gear1.throttle_gear_mutex);
//	log_info("当前的控车状态：%d\n", ctrl_status_flag.ctrl_flag);
//
//	if(ctrl_status_flag.ctrl_flag == 0) {	//当前为手动控车
//
//		 rt_vote.rt_core.rt_opt.rt_gear=gear_temp;
//        rt_vote.rt_control_status=RT_MANNUAL;
//			cmd[0] = VOTE_0_SOURCE;
//			cmd[1] = 0xD6;
//			//向控制盒发送实时信息
//			if(frame_encap(0x80, 0x02, DATA_FRAME_CODE, cmd, &rt_vote, sizeof(RT_VOTE)) != PROTOCAL_SUCCESS) {
//				log_error("failed to encap frame");
//			}
//
//	} else {	//当前为自动控车
//		int auto_change1;
//		pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//		auto_change1=auto_change;
//		pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));
//		rt_vote.rt_control_status=RT_AUTO;
//		if(get_train_status()==TRAIN_ASSISTING&&auto_change1==0) {
//			//自动转手动
//			pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//			auto_to_manual_flag.atm_flag = 1;
//			pthread_cond_signal(&(auto_to_manual_flag.atm_cond));
//			pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));
//		} else if((rt_vote.rt_core.rt_comm.brake_out==BRAKE_AIR_PRESSURE)||
//				(rt_vote.rt_core.rt_comm.brake_out==BRAKE_AIR_SHUTOFF)||
//				( rt_vote.rt_core.rt_comm.brake_out==BRAKE_EMERGENCY)
//				)
//		{
//			if(auto_change1==0){
//			//自动转手动
//			pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//			auto_to_manual_flag.atm_flag = 1;
//			pthread_cond_signal(&(auto_to_manual_flag.atm_cond));
//			pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));}
//		} else if(rt_vote.rt_core.rt_comm.speed_and_location.speed < 10&&auto_change1==0) {
//			//自动转手动
//			pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//			auto_to_manual_flag.atm_flag = 1;
//			pthread_cond_signal(&(auto_to_manual_flag.atm_cond));
//			pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));
//		}
//		else if((rt_vote.rt_core.rt_comm.train_sig==SIG_RED_YELLOW_LIGHT)||
//				(rt_vote.rt_core.rt_comm.train_sig==SIG_RED_LIGHT)||
//				(rt_vote.rt_core.rt_comm.train_sig==SIG_WHITE_LIGHT)){ //  信号灯变化 红 红黄 白色
//          if(auto_change1==0){
//			pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//			auto_to_manual_flag.atm_flag = 1;
//			pthread_cond_signal(&(auto_to_manual_flag.atm_cond));
//			pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));
//          }
//		}
//
//		else if(gear_temp != SWITCH_GEAR&&auto_change1==0) { //标记手柄档位是否有变化
//			//自动转手动
//			pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//			auto_to_manual_flag.atm_flag = 1;
//			pthread_cond_signal(&(auto_to_manual_flag.atm_cond));
//			pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));
//		} else {
//
//		}
//		if(get_branch_status()==BRANCH_CHANGE&&auto_change1==0){
//			if(branch_info.ahead_branch_route_cont_post_start
//					-rt_vote.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate<distance_safe){
//				//自动转手动
//				pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//				auto_to_manual_flag.atm_flag = 1;
//				pthread_cond_signal(&(auto_to_manual_flag.atm_cond));
//				pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));
//			}
//		}
//
//
//		if(get_side_status()==SIDE_START&&auto_change1==0){
//			if(side_info.ahead_side_route_cont_post_start
//					-rt_vote.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate<distance_safe){
//				//自动转手动
//				pthread_mutex_lock(&(auto_to_manual_flag.atm_mutex));
//				auto_to_manual_flag.atm_flag = 1;
//				pthread_cond_signal(&(auto_to_manual_flag.atm_cond));
//				pthread_mutex_unlock(&(auto_to_manual_flag.atm_mutex));
//			}
//		}
//
//		//向输入输出板（司控器）发送实时信息
//		write_sent_throttle_queue(rt_vote.rt_core.rt_opt.rt_gear, (int)ctrl_status_flag.ctrl_flag);
//		//向控制盒发送实时信息
//		cmd[0] = VOTE_0_SOURCE;
//		cmd[1] = 0xD6;
//		log_info("+++++++++status：%d\n", rt_vote.rt_control_status);
//		//copy
//
//		if (frame_encap(0x80, 0x02, DATA_FRAME_CODE, cmd, &rt_vote,
//				sizeof(RT_VOTE)) != PROTOCAL_SUCCESS) {
//			log_error("failed to encap frame");
//		}
//	}
#endif
}

void trip_handle(char *data, int length){
	log_info("trip handle%d\n",sizeof(TRIP_INFO));
	if((sizeof(TRIP_INFO))!=length){
		log_error("receive trip_info data error\n");
		return ;
	}
	TRIP_INFO trip_temp;
	memcpy(&trip_temp,data,length);
	set_trip_info(trip_temp);
	char cmd[2];
	cmd[0]=VOTE_0_SOURCE;
	cmd[1]=(char)EVENT_TRIP_UPDATE;
	if(frame_encap(DAT_SOC,CON_0_DES,DATA_FRAME_CODE,cmd,&trip_temp,(int)sizeof(TRIP_INFO))!=PROTOCAL_SUCCESS){
		log_error("marshalling_info send to control error\n");
	}
	return;
}
void trip_ack_handle(){
	log_info("send TRIP info to core\n");
	char cmd[2];
	TRIP_INFO trip_temp;
	trip_temp=get_trip_info();
	/*if(get_mar_status()==MAR_INIT){
		set_mar_status(MAR_ACK);
	}
	else
	{
		if(get_sys_status()>=SYS_STANDBY){
			set_sys_status(SYS_LAUNCHED);
			broadcast_status();
		}
	}*/
	cmd[0]=VOTE_0_SOURCE;
	EVENT_TYPE event_code=EVENT_SEND_TRIP_TO_CORE;
	cmd[1]=(char)event_code;
	if(frame_encap(DAT_SOC,CORE_0_DES,DATA_FRAME_CODE,cmd,&trip_temp,(int)sizeof(TRIP_INFO))!=PROTOCAL_SUCCESS){
				log_error("trip ack send to core error\n");
				return;
			}
}
void marshall_handle(char *data, int length){
	log_info("marshalling handle%d\n",sizeof(MARSHALLING_INFO));
	if((sizeof(MARSHALLING_INFO))!=length){
		log_error("receive marshalling_info data error\n");
		return ;
	}
	MARSHALLING_INFO mar_temp;
	memcpy(&mar_temp,data,length);
	set_mar_info(mar_temp);
	char cmd[2];
	cmd[0]=VOTE_0_SOURCE;
	cmd[1]=(char)EVENT_VOTE_UPDATE_MARSHALLING;
	if(frame_encap(DAT_SOC,CON_0_DES,DATA_FRAME_CODE,cmd,&mar_temp,(int)sizeof(MARSHALLING_INFO))!=PROTOCAL_SUCCESS){
		log_error("marshalling_info send to control error\n");
	}
	return;
}
void mar_ack_handle(){
	log_info("send bianzu info to core\n");
	char cmd[2];
	MARSHALLING_INFO mar_temp;
	mar_temp=get_mar_info();
	/*if(get_mar_status()==MAR_INIT){
		set_mar_status(MAR_ACK);
	}
	else
	{
		if(get_sys_status()>=SYS_STANDBY){
			set_sys_status(SYS_LAUNCHED);
			broadcast_status();
		}
	}*/
	cmd[0]=VOTE_0_SOURCE;
	EVENT_TYPE event_code=EVENT_VOTE_SENT_MARSHALLING;
	cmd[1]=(char)event_code;
	if(frame_encap(DAT_SOC,CORE_0_DES,DATA_FRAME_CODE,cmd,&mar_temp,(int)sizeof(MARSHALLING_INFO))!=PROTOCAL_SUCCESS){
				log_error("marshalling ack send to core error\n");
				return;
			}
}
void jieshi_handle(char *data, int length){
	log_info("jieshi handle%d\n",sizeof(JIESHI_INFO));
	if((sizeof(JIESHI_INFO))!=length){
		log_error("receive jieshi_info data error\n");
		return ;
	}
	JIESHI_INFO jieshi_temp;
	memcpy(&jieshi_temp,data,length);
	set_jieshi_info(jieshi_temp);
	char cmd[2];
	cmd[0]=VOTE_0_SOURCE;
	cmd[1]=(char)EVENT_MAR_UPDATE;
	if(frame_encap(DAT_SOC,CON_0_DES,DATA_FRAME_CODE,cmd,&jieshi_temp,(int)sizeof(JIESHI_INFO))!=PROTOCAL_SUCCESS){
		log_error("jieshi_info send to control error\n");
	}
	return;
}
void jieshi_ack_handle(){
	log_info("send jieshi info to core\n");
	char cmd[2];
	JIESHI_INFO jieshi_temp;
	jieshi_temp=get_jieshi_info();
	/*if(get_mar_status()==MAR_INIT){
		set_mar_status(MAR_ACK);
	}
	else
	{
		if(get_sys_status()>=SYS_STANDBY){
			set_sys_status(SYS_LAUNCHED);
			broadcast_status();
		}
	}*/
	cmd[0]=VOTE_0_SOURCE;
	EVENT_TYPE event_code=EVENT_SEND_MAR_TO_CORE;
	cmd[1]=(char)event_code;
	if(frame_encap(DAT_SOC,CORE_0_DES,DATA_FRAME_CODE,cmd,&jieshi_temp,(int)sizeof(JIESHI_INFO))!=PROTOCAL_SUCCESS){
				log_error("jieshi ack send to core error\n");
				return;
			}
}

