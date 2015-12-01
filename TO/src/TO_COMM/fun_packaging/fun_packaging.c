#include <string.h>
#include "led.h"
#include "led_packet.h"
#include "log.h"
#include "new_global.h"
#include "to_status.h"
#include "protocal.h"
#include "rt_definition.h"
#include "system_error_code.h"
#include "fun_packaging.h"
#include "to_time.h"
#include "new_added.h"

#define JIESHI_STATUS_OFFSET 1
#define JIESHI_STATUS_LEN 1
#define JIESHI_ID_OFFSET 2
#define JIESHI_ID_LEN 2
#define TIME_OUTPUT_LEN 30

pthread_mutex_t mutex_s2 = PTHREAD_MUTEX_INITIALIZER;
char data_request = 0;

/*
*bd - base data
*/
static int parse_basedata_status(char bd_status)
{
	char cmd[2];
	char flag;
	int retn = PARSE_STATUS_INFO_SUCCESS;
	
	static int send_once = 0;
	switch (bd_status) {
	case 0x0f: 
		//LKJ_status = 0x00;//"LKJ_status" is global bariable
		log_info("dis connection with LKJ\n");
		set_abnormal_code(SYS_ERROR_LKJ_COMM_FAILURE,VOTE_0_DES);//set sys_errro_code and destination the sys_error_code send to
		//send_abnormal_code();
		led_packet_off(GPIOX_41);
		/*TODO: send signal to vote board*/
		break;
				
	case 0x5a:
		log_info("match LKJ data\n");
		led_packet_on(GPIOX_41);
		//pthread_mutex_lock(&mutex_s2);
		//data_request = 0x01;
		//pthread_mutex_unlock(&mutex_s2);
		/*TODO: match base_data version*/
		cmd[0] = SOURCE_COM_0;
		cmd[1] = EVENT_LKJ_VERSION_DIFFERENCE;
		flag = 0x00;
		if(get_sys_status() == SYS_STANDBY && send_once == 0){
			log_info("before send platform data version matching");
			frame_encap(SOCKET_TYPE, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, &flag, sizeof(char));
			send_once = 1;
			log_info("hehind send platform data version matching");
		}
		
		break;
					
	case 0xa5:
		log_info("not match with LKJ data\n");
		/*TODO: LED GPIOX_41 blink*/
		cmd[0] = SOURCE_COM_0;
		cmd[1] = EVENT_LKJ_VERSION_DIFFERENCE;
		flag = 0x01;
		if(get_sys_status() == SYS_STANDBY){
			frame_encap(SOCKET_TYPE, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, &flag, sizeof(char));
			log_info("send platform data version not matching");
			
		}
		break;
					
	case 0x3c:
		log_info("updating\n");
		/*TODO: what?*/
		break;
					
	default:
		retn = PARSE_STATUS_INFO_ERR;
		log_info("not defined\n");
		break;
		/*TODO: what?*/
	}
	return retn;
}


/*
*judge jieshi_status in status_data received
*/
static int parse_jieshi_status(char *recv_buf)
{
	int retn;
	retn = PARSE_STATUS_INFO_SUCCESS;
	switch (recv_buf[JIESHI_STATUS_OFFSET]) {//Analyzing jieshi state
	case 0x00:
		log_info("don't know jieshi state\n");
		break;
	case 0x0a:
		log_info("jieshi is changing\n");
		break;
	case 0x05:
		log_info("jieshi is ready\n");
		break;
	default:
		retn = PARSE_STATUS_INFO_ERR;
		break;
	}
	return retn;
}


/*
*
*/
static void parse_base_data_version(char *recv_buf)
{
	int base_data_version = ((int)recv_buf[0] << 24 | (int)recv_buf[1] << 16
	| (int)recv_buf[2] << 8 | (int)recv_buf[3]);
	log_info("base_data_version is %x\n", base_data_version);
}

void led_flicker(char *led_status, int gpiox_num)
{
	if(*led_status == 0){
		led_packet_on(gpiox_num);
	}else{
		led_packet_off(gpiox_num);
	}
	*led_status = ~(*led_status);
}


int parse_status_info(int canid, char *recv_buf)
{
	int parse_status_info_result = PARSE_STATUS_INFO_SUCCESS;
	//when "canid & 0x7ff" is "0x100", presenting received data is the first frame status data
	switch ((unsigned int) (((unsigned int) canid) & 0x07ff)) {
	case 0x100:
		if (parse_basedata_status(recv_buf[0]) == PARSE_STATUS_INFO_ERR) {
			parse_status_info_result = PARSE_STATUS_INFO_ERR;
		}
		if (parse_jieshi_status(recv_buf) == PARSE_STATUS_INFO_ERR) {
			parse_status_info_result = PARSE_STATUS_INFO_ERR;
		}
		break;
	case 0x101:
		parse_base_data_version(recv_buf);
		break;
	default:
		break;
	}
	return parse_status_info_result;
}

void send_trip_info(TRIP_INFO *trip_info_buf, TRIP_INFO *trip_info_send)
{
	int renew_trip_info_flag = memcmp(trip_info_send, trip_info_buf, sizeof(TRIP_INFO));
	if (renew_trip_info_flag != 0) { //when trip_info has changed, update marshalling_info_send
		log_debug("trip_info has changed.\n");
		memcpy(trip_info_send, trip_info_buf, sizeof(TRIP_INFO));
		log_debug("trip_info_send is ready.\n");
		/*TODO: send trip_info_send to vote board*/
		char frame_cmd[2];
		frame_cmd[0] = SOURCE_COM_0; //event source
		frame_cmd[1] = EVENT_TRIP_CHANGE; //event name, "0x08" present marshalling_info event
		log_debug("trip_info_send is ready to send to vote_brd.\n");
		frame_encap(0x40, VOTE_0_DES,DATA_FRAME_CODE, frame_cmd, (char *) trip_info_send,
					sizeof(TRIP_INFO));
		//log_debug("trip_info_send has been sent to bote_brd.\n");
	}
}

void send_jieshi_info()
{
	char frame_cmd[2];
	JIESHI_INFO jieshi_info;
	frame_cmd[0] = SOURCE_COM_0; //event source
	frame_cmd[1] = EVENT_MARSHALLING_CHANGE; 
	jieshi_info.start_kilmeter_coordinate = 0;
	jieshi_info.end_kilmeter_coordinate = 0;
	jieshi_info.limit_speed = 0;
	frame_encap(0x40, VOTE_0_DES,DATA_FRAME_CODE, frame_cmd, 
		(char *) &jieshi_info, sizeof(JIESHI_INFO));
}

void get_time_then_send(REALTIME_DATA real_time_struct)
{
	COMM_TIME comm_time;
	char time_output[TIME_OUTPUT_LEN];
	char command[2] = {SOURCE_COM_0, EVENT_CLOCK_SYNC};
	static int send_time_once = 0;
	memset(&comm_time, 0, sizeof(COMM_TIME));
	memset(time_output, 0, TIME_OUTPUT_LEN);
	comm_time.year = real_time_struct.year;
	comm_time.month = real_time_struct.month;
	comm_time.day = real_time_struct.day;
	comm_time.hour = real_time_struct.hour;
	comm_time.minute = real_time_struct.min;
	comm_time.second = real_time_struct.sec;
	convert_to_time(comm_time,time_output);
	if (send_time_once == 0) {
		set_to_time(time_output, TIME_OUTPUT_LEN);
		frame_encap(DAT_SOC, VOTE_0_DES, DATA_FRAME_CODE, command, time_output, TIME_OUTPUT_LEN);
		frame_encap(DAT_SOC, CORE_0_DES, DATA_FRAME_CODE, command, time_output, TIME_OUTPUT_LEN);
		send_time_once = 1;
	}
}
