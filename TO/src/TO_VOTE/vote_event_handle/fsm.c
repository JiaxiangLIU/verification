#include "fsm.h"

static FSM_STATE fsm_state=FSM_MANUAL;
static time_t startTime =0;
static time_t endTime=0;
/* GEAR 应当是优化器建议输出档位，不是的时候应当修改,ctrl应当是自动状态,
 * ermerge_flag紧急制动标志
 * */
void status_init(){
	set_to_auto_status(0);
	set_to_manual_status(0);
	set_auto_ack_status(0);
	set_manual_ack_status(0);
}
void fsm_control(int *gear, RT_CONTROL_STATUS * ctrl, int emerge_flag) {
	switch (fsm_state) {
	printf("fsm_state %d\n",fsm_state);
	case FSM_MANUAL:
		//initlog
		startTime = 0;
		endTime = 0;
		if (get_to_auto_status() == 1) {
			fsm_state = FSM_START_AUTO;
		}
		*gear = get_throttle();
		*ctrl = CONTROL_MANNUL;
		break;
	case FSM_START_AUTO:
		if (startTime == 0) {
			startTime = time(NULL);
			if (startTime == 0)
				startTime = 1;
			//  todo send daojishi
			ctrbox_command(EVENT_DISP_AUTO);
		} else {
			endTime = time(NULL);
			if (endTime - startTime > OVER_TIME) {
				fsm_state = FSM_OVER_TIME;
				ctrbox_command(EVENT_AUTO_TIME_ALARM);
			} else {
				if (get_to_auto_gear() == SWITCH_GEAR) {
					log_info("please two ack;\n");
					fsm_state = FSM_ACK_AUTO;
					ctrbox_command(EVENT_GEAR8_AUTO_CONFIRM);
				} else {
					log_info("请拨至8档\n");
					if (get_throttle() == SWITCH_GEAR) {
						fsm_state = FSM_AUTO;
						status_init();
						ctrbox_command(EVENT_AUTO_COMPLETE);
						// todo complete
					}
				}
			}
		}
		if (emerge_flag == 1) {
			fsm_state = FSM_ERGE_AUTO;
		}
		break;
	case FSM_ACK_AUTO:

		//todo request ACK MESSEGE
		endTime = time(NULL);
		if (endTime - startTime > OVER_TIME) {
			fsm_state = FSM_OVER_TIME;
			ctrbox_command(EVENT_AUTO_TIME_ALARM);
		} else {
			if (get_auto_ack_status() == 1) {
				fsm_state = FSM_AUTO;
				status_init();
				ctrbox_command(EVENT_AUTO_COMPLETE);
				// todo complete
			}
			log_info("wait for to auto ack\n");
		}
		if (emerge_flag == 1) {
			fsm_state = FSM_ERGE_AUTO;
		}
		break;
	case FSM_OVER_TIME:
		if (*gear > 0)
			*gear = 0;
		if (get_throttle() <=(*gear)){
			fsm_state = FSM_MANUAL;
		    status_init();
		    ctrbox_command(EVENT_MANNAL_COMPLETE);
		}
		// todo complete
		if (emerge_flag == 1) {
			fsm_state = FSM_ERGE_AUTO;
		}
		break;
	case FSM_AUTO:
		startTime = 0;
		endTime = 0;
		printf("get_to_manual_status %d\n",get_to_manual_status());
		if (get_throttle() != SWITCH_GEAR) {
			set_to_manual_status(1);
		}
		if (get_to_manual_status() == 1) {
			printf("get_to_manual_status %d\n",get_to_manual_status());
			fsm_state = FSM_START_MANUAL;
		}
		if (emerge_flag == 1) {
			fsm_state = FSM_ERGE_AUTO;
		}

		break;
	case FSM_START_MANUAL:
		if (startTime == 0) {
			startTime = time(NULL);
			if (startTime == 0)
				startTime = 1;
			ctrbox_command(EVENT_DISP_MANNAL);
			//  todo send daojishi
		} else {
			endTime = time(NULL);
			if (endTime - startTime > OVER_TIME) {
				fsm_state = FSM_OVER_TIME;
				ctrbox_command(EVENT_MANNUL_TIME_ALARM);
			} else {
				if (get_throttle() <= (*gear)) {
					fsm_state = FSM_MANUAL;
					status_init();
					ctrbox_command(EVENT_MANNAL_COMPLETE);
					// todo complete
				} else if (get_throttle() - (*gear) <= 2) {
					fsm_state = FSM_ACK_MANUAL;
					ctrbox_command(EVENT_TWO_GEAR_CONFIRM);
				} else
					log_info("over 2 ，throttle wrong \n");
			}
		}
		if (emerge_flag == 1) {
			fsm_state = FSM_ERGE_AUTO;
		}
		break;

	case FSM_ACK_MANUAL:
		//todo request ACK MESSEGE
		endTime = time(NULL);
		if (endTime - startTime > OVER_TIME) {
			fsm_state = FSM_OVER_TIME;
			ctrbox_command(EVENT_MANNUL_TIME_ALARM);
		} else {
			if (get_manual_ack_status() == 1) {
				fsm_state = FSM_MANUAL;
				status_init();
				ctrbox_command(EVENT_MANNAL_COMPLETE);
				// todo complete
			}
			log_info("wait for to manual ack\n");
		}
		if (emerge_flag == 1) {
			fsm_state = FSM_ERGE_AUTO;
		}
		break;
	case FSM_ERGE_AUTO:
		*gear = 0;
		if (get_throttle() < 0) {
			// todo complete
			ctrbox_command(EVENT_MANNAL_COMPLETE);
			status_init();
			fsm_state = FSM_MANUAL;
		}
		break;
	}

}

