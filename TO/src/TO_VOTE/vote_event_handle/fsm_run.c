#include "fsm.h"
#include "rt_definition.h"

int fsm_start=0;
// compare two struct
int rtvote_diff(RT_VOTE rt1,RT_VOTE rt2){
	if(rt1.rt_control_status!=rt2.rt_control_status)
		return 1;
	if(rt1.rt_core.rt_status.rt_enable_status!=rt2.rt_core.rt_status.rt_enable_status)
		return 1;
	if(rt1.rt_core.rt_opt.opt_gear!=rt2.rt_core.rt_opt.opt_gear)
		return 1;
	if(rt1.rt_core.rt_opt.rt_gear!=rt2.rt_core.rt_opt.rt_gear)
		return 1;
	if(rt1.rt_core.rt_comm.speed_and_location.kilmeter_coordinate!=rt2.rt_core.rt_comm.speed_and_location.kilmeter_coordinate)
		return 1;
	if(rt1.rt_core.rt_comm.brake_out!=rt2.rt_core.rt_comm.brake_out)
		return 1;
	if(rt1.rt_core.rt_comm.train_sig!=rt2.rt_core.rt_comm.train_sig)
		return 1;
	return 0;
}
int can_manual_to_auto(RT_VOTE rt_vote1) {
	if(get_branch_status()==BRANCH_CHANGE){
			if(get_branch_info().ahead_branch_route_cont_post_start
					-rt_vote1.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate<distance_safe)
				return 0;
	}
	if(get_side_status()==SIDE_START){
		if((get_side_info().ahead_side_route_cont_post_start-distance_safe<rt_vote1.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate)&&
				(rt_vote1.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate<get_side_info().ahead_side_route_cont_post_end))
			return 0;
	}
	if((rt_vote1.rt_core.rt_comm.train_sig==SIG_RED_YELLOW_LIGHT)||
					(rt_vote1.rt_core.rt_comm.train_sig==SIG_RED_LIGHT)||
					(rt_vote1.rt_core.rt_comm.train_sig==SIG_WHITE_LIGHT))
		return 0;
	if(
			rt_vote1.rt_core.rt_status.rt_enable_status == RT_AUTO_ENABLE
			&& rt_vote1.rt_core.rt_comm.speed_and_location.speed > THRESHOLD_V
			&& rt_vote1.rt_core.rt_comm.brake_out != BRAKE_AIR_PRESSURE
			&& rt_vote1.rt_core.rt_comm.brake_out != BRAKE_AIR_SHUTOFF
			&& get_train_status()==TRAIN_DUTY) {	// 进入自动控车的条件
		return 1;
	}
	else
	{
		//log_info("get_train_status() %d\n",get_train_status());
		return 0;
	}
}

void fsm_run() {
	log_info("fsm thread start\n");
	char cmd[2];
	cmd[0] = VOTE_0_SOURCE;
	cmd[1] = 0xD6;
	while (1) {
		while (fsm_start) {
			pthread_mutex_lock(&rt_mutex);
			fsm_rt = rt_vote;
			pthread_mutex_unlock(&rt_mutex);
			if (can_manual_to_auto(fsm_rt) == 1) {
				set_auto_status(AUTO_ENABLE);
			} else {
				set_auto_status(AUTO_DISABLE);
			}
			if (get_branch_status() == BRANCH_CHANGE) {
				if (get_branch_info().ahead_branch_route_cont_post_start
						- fsm_rt.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate
						< 0)
					set_branch_status(BRANCH_INIT);
			}
			if (get_side_status() == SIDE_START) {
				if (get_side_info().ahead_side_route_cont_post_end
						- fsm_rt.rt_core.rt_comm.speed_and_location.new_kilmeter_coordinate
						< 0) {
					int distance;
					distance = get_side_info().ahead_side_route_cont_post_end;
					set_side_status(SIDE_INIT);
					char side_end[2];
					side_end[0] = VOTE_0_SOURCE;
					side_end[1] = (char) EVENT_VOTE_SIDE_LINE_PASS_END;
					frame_encap(DAT_SOC, act_con, DATA_FRAME_CODE, side_end,
							&distance, sizeof(int));
				}
			}
			if (get_auto_status() == AUTO_ENABLE) {

				fsm_rt.rt_core.rt_status.rt_enable_status = RT_AUTO_ENABLE;
			} else {

				fsm_rt.rt_core.rt_status.rt_enable_status = RT_AUTO_UNABLE;
				set_to_manual_status(1);
			}
			int emerge_flag = 0;
			if (fsm_rt.rt_core.rt_comm.brake_out == BRAKE_EMERGENCY) {
				emerge_flag = 1;
			}
			fsm_control(&fsm_rt.rt_core.rt_opt.rt_gear,
					&fsm_rt.rt_control_status, emerge_flag);
			if (rtvote_diff(fsm_rt, fsm_temp)) {
				if ((int) fsm_rt.rt_control_status
						!= (int) fsm_temp.rt_control_status) {
					write_sent_throttle_queue(fsm_rt.rt_core.rt_opt.rt_gear,
							(int) fsm_rt.rt_control_status);
				} else if ((fsm_rt.rt_control_status == RT_AUTO)
						&& (fsm_rt.rt_core.rt_opt.rt_gear
								!= fsm_temp.rt_core.rt_opt.rt_gear)) {
					write_sent_throttle_queue(fsm_rt.rt_core.rt_opt.rt_gear,
							(int) fsm_rt.rt_control_status);
				}

				//向控制盒发送实时信息 保存

				if (frame_encap(DAT_SOC, act_con, DATA_FRAME_CODE, cmd, &fsm_rt,
						sizeof(RT_VOTE)) != PROTOCAL_SUCCESS) {
					log_error("failed to encap frame");
				}
				fsm_temp = fsm_rt;
			}
			usleep(20000);

		}
	}// while(1)
}
