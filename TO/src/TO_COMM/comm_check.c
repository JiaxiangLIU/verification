
/**
 * @file comm_check.c
 * @brief 进行通信校验
 **/

#include "app_socket.h"
#include "protocal.h"
#include "threadpool.h"
#include "client_interface.h"
#include "log.h"
#include "global.h"
#include "to_status.h"
#include "new_added.h"
#include "led.h"
#include "led_packet.h"
#include "ack_handle.h"



int comm_register(void)
{
    char cmd[2];
    char arg;
	arg = BOARDID;
	char temp;
	temp = get_commu_event_source(arg);
    cmd[0] = temp;
    cmd[1] = EVENT_COM_REGISTER;
	char flag[1] = {0x01};
	//master_commu = get_master_commu();

	/*when system init and communication board has not registered, commu_brd \
	start to register to vote_brd*/
	if(get_sys_status() == SYS_INIT && get_master_commu() != SOURCE_COM_0 && \
		get_master_commu() != SOURCE_COM_1){
		if ((frame_encap(SOCKET_TYPE, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, flag, 1)) != PROTOCAL_SUCCESS) {
       		log_error("failed to encape frame.");
			led_packet_off(GPIOX_50);
			return -1;
		}
		else{
			led_packet_on(GPIOX_50);
			return 0;
		}
	}
	return -1;
}


void heartbeat_check()
{
	static int sec = 0;
    char cmd[2];
    char arg[2];
    cmd[0] = MY_SEND_EVENT_SOURCE;
    cmd[1] = EVENT_COM_HEARTBEAT;
    arg[0] = 0x01;
	static char led_status_40 = 0;
    while(1) {
      /*if ((frame_encap(SOCKET_TYPE, VOTE_0_DES, DATA_FRAME_CODE, cmd, arg, 2)) != PROTOCAL_SUCCESS)
        {
            log_error("failed to encape frame.");
        }*/
		ack_time_handle();
	  	send_abnormal_code();
		sec++;
		if (sec > 3) {
			led_flicker(&led_status_40, GPIOX_40);
			frame_encap(SOCKET_TYPE, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, arg, 1);
			sec = 0;
		}
		sleep(1);   //停顿两秒
    }
}


int client_init(void)
{
    int ret = threadpool_add(pool, &comm_client, NULL, 0);
    if (ret != 0)
    {  
        log_error("failed to create a new thread");
        return ret;
    }
    else 
        return 0;
}
