
/**
 * @file comm_check.c
 * @brief 进行通信校验
 **/

#include "app_socket.h"
#include "protocal.h"
#include "threadpool.h"
#include "log.h"
#include "global.h"
#include "to_status.h"
#include "ack_handle.h"
#include "abnormal_code.h"
#include "led.h"


int comm_register(void)
{
    char cmd[2];
    char arg;

#ifdef BOARDID 
	cmd[0] = get_core_event_source(BOARDID);
	cmd[1] = EVENT_CORE_REGISTER;
    //arg = BOARDID;
    arg = 0x01;
#else
    cmd[0] = SOURCE_CORE_0;
    cmd[1] = EVENT_CORE_REGISTER;
    arg = 0x01;
#endif
	if (get_sys_status() == SYS_INIT) {
	    log_info("core register board id: %d\n", BOARDID);
	    if ((frame_encap(COM_SOC, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, &arg, 1)) != PROTOCAL_SUCCESS) {
	    	log_error("failed to encape frame.");
	    }
	}

	// jiaxiang: bug? 这个注册函数总是返回1，意味着注册不可能失败
	return 1;
}


void heartbeat_check()
{
		static int sec = 0;
    char cmd[2];
    char arg[2];
		static char led_flag = 0;

#ifdef BOARDID 
    cmd[0] = get_core_event_source(BOARDID);
    cmd[1] = EVENT_CORE_HEARTBEAT;

    //arg[0] = BOARDID;
    arg[0] = 0x01;
#else
	cmd[0] = SOURCE_CORE_0;
	cmd[1] = EVENT_CORE_HEARTBEAT;

	//arg[0] = BOARDID;
	arg[0] = 0x01;
#endif

	//	cmd[0] = SOURCE_CORE_0;
	//	cmd[1] = EVENT_ROUTE_VERSION_RESULT;
	//	frame_encap(COM_SOC, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, arg, 1);

    while(1) {
			sec++;
			ack_time_handle();
			send_abnormal_code();
			if (sec > 3) {
				sec = 0;
				comm_register();
				frame_encap(COM_SOC, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, arg, 1);

				if (led_flag == 0) {
					led_on(COMMU_LED);
					led_flag = 1;
				} else {
					led_off(COMMU_LED);
					led_flag = 0;
				}
			}
    	sleep(1);
    }
}

