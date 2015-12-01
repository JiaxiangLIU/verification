#include "heartbeat.h"
#include "led.h"
static char src_board;
static char des_board;

static send_heartbeat() {
	char cmd[2];
	char arg = 0x01;
	cmd[0] = src_board;
	cmd[1] = 0x00; // event
	int a=0;
	int led_count=0;
	char led_flag=0;
	while (1) {
		if(a==0){
			led_flag=0;
			if(led_count==1){
				led_flag=1;
			}
			VoteLedCtrl(led_outcan,led_flag);
			if ((frame_encap(SOCKET_HEART, des_board, COMMAND_FRAME_CODE, cmd, &arg, 1))
							!= PROTOCAL_SUCCESS) {
						log_error("failed to frame_encap\n");
					}
			led_count=(led_count+1)%2;

		}

		ack_time_handle();

		// heart beat check comm not need
		heartbeat_check();

		send_abnormal_code();
		sleep(1);
		a++;
		if(a>=3)
			a=0;
	}
}

void start_heartbeat(char src, char des){
	src_board=src;
	des_board=des;
	log_info("start heart beat thread\n");
	 if (threadpool_add(pool, &send_heartbeat, NULL, 0) != 0) {
	        log_error("thread add err");
	    }
}
