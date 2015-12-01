#ifndef FSM_H
#define FSM_H
#define OVER_TIME 30
#include "global.h"
# include "vote_status.h"
#include "to_status.h"
#include "time.h"
#include "string.h"
#include "rt_definition.h"
#include "log.h"
#include "ctrbox_comm.h"

typedef enum fsm_state{
	FSM_MANUAL=0,
	FSM_START_AUTO,
	FSM_ACK_AUTO,
	FSM_OVER_TIME,
	FSM_AUTO,
	FSM_START_MANUAL,
	FSM_ACK_MANUAL,
	FSM_ERGE_AUTO
}FSM_STATE;
void status_init();
void fsm_control(int *gear,RT_CONTROL_STATUS * ctrl,int emerge_flag);
void fsm_run();
#endif
