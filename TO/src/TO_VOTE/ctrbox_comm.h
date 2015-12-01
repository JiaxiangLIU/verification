
#ifndef _CTRBOX_COMM_H_
#define _CTRBOX_COMM_H_
#include "to_status.h"
//#include "ctrbox_comm.h"
int ctrbox_data(EVENT_TYPE par);
typedef enum ctrbox_cmd
{
		CTRBOX_CMD_EVENT_MANNAL_COMPLETE , /* 转手动驾驶完成 */
		CTRBOX_CMD_EVENT_AUTO_COMPLETE , /* 转自动驾驶完成 */
		CTRBOX_CMD_EVENT_DISP_AUTO , /* 转自动流程交互：30秒内扳至惰性档位 */
		CTRBOX_CMD_EVENT_DISP_MANNAL , /* 转手动流程交互：30秒内扳至运行档位 */
		CTRBOX_CMD_EVENT_TWO_GEAR_CONFIRM,/* 转手动高出2档位需控制盒确认 */
}CTRBOX_CMD;

int ctrbox_comm(CTRBOX_CMD* cmd);
//typedef enum ctrbox_data_cmd{
//	CTRBOX_CMD_EVENT_VOTE_UPDATE_MARSHALLING = 0xD1, // 表决板编组信息与揭示信息更新  rt_hanle.c */
//	CTRBOX_CMD_EVENT_SIG_LIGHT_CHANGE = 0xD3, /* 前方信号灯变化 */
//	CTRBOX_CMD_EVENT_DISP_REAL_TIME_INFO = 0xD6, /* 实时信息显示与自动控车档位 */
//	CTRBOX_CMD_EVENT_VOTE_TRIP_INIT_COMPLETE = 0xD7, /* 发布行程初始化完成状态，行程启动，发布行程编号*/
//	CTRBOX_CMD_EVENT_VOTE_BRANCH_TRANSFER_PASS = 0xD8, /* 前方支线转移 */
//	CTRBOX_CMD_EVENT_VOTE_ROUTE_TRANSFER_PASS = 0xD9, /* 前方交路转移 */
//	CTRBOX_CMD_EVENT_VOTE_SIDE_LINE_PASS_START = 0xDA, /* 前方侧线通过起点 */
//	CTRBOX_CMD_EVENT_VOTE_SIDE_LINE_PASS_END = 0xDB, /* 前方侧线通过终点 */
//} CTRBOX_DATA_CMD;

/*typedef struct ctrbox_data_par{
	CTRBOX_DATA_CMD cmd;
	void * data;
}CTRBOX_DATA_PAR;*/

//int  ctrbox_data (EVENT_TYPE  par);
/* send  command to control box */
int ctrbox_command(EVENT_TYPE type);
#endif
