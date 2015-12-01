
/**
 * @file ctrbox_comm.c
 * @brief 与控制盒通信模块
 **/ 

#include "protocal.h"
#include "global.h"
#include "log.h"
#include "ctrbox_comm.h"
#include "to_status.h"
#include "vote_status.h"
int ctrbox_comm(CTRBOX_CMD* cmd){
	char command[2];
	CTRBOX_CMD temp= *cmd;
	char arg=0x01 ;
	PROTOCAL_RESULT res;
	command[0]=VOTE_0_SOURCE;
	EVENT_TYPE event_code;
	switch(temp)	{
//		case CTRBOX_CMD_ROUTE_RECEIVE_COMPLETE:
//			command[0] = 0x00;
//			command[1] = 0x01;
//			res = frame_encap(0x20, 0x02, COMMAND_FRAME_CODE, command, &arg, 1);
//			break;
//		case CTRBOX_CMD_ROUTE_INIT_BREAK:
//			command[0] = 0x00;
//			command[1] = 0x02;
//			res = frame_encap(0x20, 0x02, COMMAND_FRAME_CODE, command, &arg, 1);
//			break;
//		case CTRBOX_CMD_GET_ANNOUNCE_INFO_FAILURE:
//			command[0] = 0x00;
//			command[1] = 0x04;
//			res = frame_encap(0x20, 0x02, COMMAND_FRAME_CODE, command, &arg, 1);
//			break;
//		case CTRBOX_CMD_ROUTE_INIT_COMPLETE:
//			command[0] = 0x01;
//			command[1] = 0x51;
//			res = frame_encap(0x20, 0x02, COMMAND_FRAME_CODE, command, &arg, 1);
//			break;
//		case CTRBOX_CMD_ROUTE_INIT_SUSPEND:
//			command[0] = 0x01;
//			command[1] = 0x52;
//			res = frame_encap(0x20, 0x02, COMMAND_FRAME_CODE, command, &arg, 1);
//			break;
//		case CTRBOX_CMD_ROUTE_INIT_START:
//			command[0] = 0x20;//VOTE_0_SOURCE;
//			command[1] = 0x10;//03;
//			res = frame_encap(COM_SOC, act_com, COMMAND_FRAME_CODE, command, &arg, 1);
//			break;

	   case CTRBOX_CMD_EVENT_DISP_AUTO:
		   event_code=EVENT_DISP_AUTO;
		   command[1]=(char)event_code;
		   break;
	   case CTRBOX_CMD_EVENT_DISP_MANNAL:
		   event_code=EVENT_DISP_MANNAL;
		   command[1]=(char)event_code;
		   break;
	   case CTRBOX_CMD_EVENT_MANNAL_COMPLETE:
		   event_code=EVENT_MANNAL_COMPLETE;
	       command[1]= (char)event_code;
	       break;
	   case CTRBOX_CMD_EVENT_AUTO_COMPLETE:
		   event_code=EVENT_AUTO_COMPLETE;
	       command[1]= (char)event_code;
		   break;
	   case CTRBOX_CMD_EVENT_TWO_GEAR_CONFIRM:
		   event_code=EVENT_TWO_GEAR_CONFIRM;
		   command[1]= (char)event_code;
		   break;
	}
	res=frame_encap(COM_SOC,act_con,COMMAND_FRAME_CODE,command,&arg,1);
	if(res != PROTOCAL_SUCCESS) {
		log_error("frame encape err.\n");
	} else {
		log_info("send to control_box success.%d\n",cmd);
	}
	free(cmd);
	return res;
}
int ctrbox_data(EVENT_TYPE par)
{
	   char command[2];
		PROTOCAL_RESULT res;
		command[0]=VOTE_0_SOURCE;
    	AHEAD_BRANCH_ROUTE_INFO temp_branch;
	    AHEAD_SIDE_ROUTE_INFO temp_side;
	    int temp_distance;
		command[1]=(char)par;
		switch(par){
		    /*case EVENT_SIG_LIGHT_CHANGE:
		    	//todo send controlbox
		    	break;
		    case EVENT_VOTE_BRANCH_TRANSFER_PASS:
		    	temp_branch=get_branch_info();
		    	temp_distance=temp_branch.ahead_branch_route_cont_post_start;
		    	frame_encap(DAT_SOC,act_con,DATA_FRAME_CODE,command,&temp_distance,4);
		    	break;
		    case EVENT_VOTE_ROUTE_TRANSFER_PASS:
		    	//todo send
		    	break;
		    	*/
		    case EVENT_FRONT_SIDE_LINE_PASS:
		    {
		    command[1]=(char)EVENT_VOTE_SIDE_LINE_PASS_START;
		    temp_side=get_side_info();
		    temp_distance=temp_side.ahead_side_route_cont_post_start;
		    frame_encap(DAT_SOC,act_con,DATA_FRAME_CODE,command,(char *)&temp_distance,4);

		    }
		}
	return 0;
}
int ctrbox_command(EVENT_TYPE type){

	log_info("send evnt type  %2x  \n",type);
    char command[2];
    command[0]=VOTE_0_SOURCE;
    char arg=0x01;
	command[1]=(char)type;
	if(frame_encap(COM_SOC,act_con,COMMAND_FRAME_CODE,command,&arg,1)!=PROTOCAL_SUCCESS){
		log_error("command to ctrlbox error\n");
		return 0;
	}
    return 1;

}
