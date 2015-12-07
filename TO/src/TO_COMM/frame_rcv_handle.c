/**
 * @file frame_rcv_handle.c
 * @brief ����frame���պ�Ĳ���
 * @author danny
 * @date 2015.02.03
 **/

#include <stdio.h>
#include <stdlib.h>

#include "log.h"

#include "comm_check.h"
#include "handle.h"
#include "global.h"
#include "to_event.h"
#include "schedule.h"
#include "to_status.h"
#include "protocal.h"
#include "new_added.h"
#include "led.h"
#include "led_packet.h"


void command_frame_rcv_handle(void *cmd)
{
   char *command = (char *)cmd;
   printf("frame command is %d %d %d\n",command[0],command[1],command[2]);

    
    struct event_t *new_event;
	char commd[2] = {0, 0};
	int commu_brd_num;
	switch(command[0]){
	case SOURCE_VOTE_0:
    	switch(command[1]){
    	/*
        case 0x10://extract route
            //new_event = event_create(EVENT_ROUTE_EXTRACT, SOURCE_CONTROL_0, command[0]);
            if (new_event == NULL){
                fprintf(stderr, "system err: failed to malloc.\n");
            }
            add_command(new_event);
            free(command);          
            break;
        case 0x01:
            set_sys_status((uint8_t)command[2]);
            //printf("current station is: %d\n", (uint8_t)command[2]);
            log_info("current station is: %d\n", (uint8_t)command[2]);
            break;
		*/	
		case EVENT_BROADCAST_COM_ADDR://release master commu_brd addr
			set_master_commu((EVENT_SOURCE) command[2]);
			commu_brd_num = check_commu_num();
			if(commu_brd_num == (int)command[2]){//current commu_brd is master commu_brd
				led_packet_on(GPIOX_43);
			}
			else{
				led_packet_off(GPIOX_43);
			}
			break;
		case EVENT_MASTER_VOTE_EXCHANGE://update system status
			log_debug("receive system status is %d\n", command[2]);
			printf("receive system status is %d\n", command[2]);
			set_sys_status((SYSTEM_STATUS) command[2]);	
			if(get_sys_status() == SYS_LAUNCHED){
				// jiaxiang: 启动新线程监听与LKJ的通信（通过can2net）
				if (client_init() != 0) {
        			log_error("client check err");
       				 exit(0);
    			}
			}

			
			/***TODO: send base_data has been received done***/
			char cmd[2] = {0, 0};
			cmd[0] = SOURCE_COM_0;
			cmd[1] = EVENT_TRIP_STATUS_NOTICE;
			char trip_data_complete = TRIP_DATA_COMPLETE;
			if(get_sys_status() == SYS_STANDBY && \
					get_train_status() == TRAIN_DUTY){
					frame_encap(SOCKET_TYPE, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, &trip_data_complete, sizeof(TRIP_STATUS));
			}
			break;

			
		case EVENT_VOTE_TRIP_INIT_START://start to trip_init
			route_data_access();
			/*TODO: send version of extract_route_data to core_brd*/
			commd[0] = SOURCE_COM_0;
			commd[1] = EVENT_ROUTE_VERSION_SEND;
			char version = 0x12;//temp value
			if(get_sys_status() == SYS_STANDBY && \
				get_train_status() == TRAIN_DUTY){
				frame_encap(SOCKET_TYPE, CORE_0_DES, DATA_FRAME_CODE, commd, &version, sizeof(char));
			}
			break;
		case EVENT_SYS_TRIP_STATUS_SEND://update trip status
			set_trip_status((TRIP_STATUS) command[2]);
			break;
		case EVENT_VOTE_TRIP_INIT_COMPLETE://not defined
			set_trip_status((TRIP_STATUS) command[2]);
			/*TODO:get trip_num*/
			break;
		case EVENT_BOARD_REGISTER_FAILUER://reset all boards
			/*TODO:reset all boards*/
			break;
		default:  
			break;
		}
		break;
 		
	case SOURCE_CORE_0:
	case SOURCE_CORE_1:
	case SOURCE_CORE_2:
	case SOURCE_CORE_3:
		switch(command[1]){
		case EVENT_ROUTE_VERSION_RESULT:
				log_debug("command[2] is: %d", command[2]);
				if(command[2] == 1){
					route_data_send();	
				} 
			break;
		case EVENT_ROUTE_DATA_SEND_ACK: /* ��·���ݰ汾��һ�´�����·��ȡ��ɲ����ͣ��¶�Ӧ�� */
			route_data_confirm[0] = command[2];
			break;
		case EVENT_ROUTE_DATA_CURVE_ACK: /* ��·���ݰ汾��һ�´������ͣ�����Ӧ��  */
			route_data_confirm[1] = command[2];
			break;
		case EVENT_ROUTE_DATA_LIMIT_ACK: /* ��·���ݰ汾��һ�´������ͣ�����Ӧ��  */
			route_data_confirm[2] = command[2];
			break;
		case EVENT_ROUTE_DATA_TELESEME_ACK: /* ��·���ݰ汾��һ�´������ͣ��źŻ�Ӧ��  */
			route_data_confirm[3] = command[2];
			break;
		case EVENT_ROUTE_DATA_TUNNEL_ACK: /* ��·���ݰ汾��һ�´������ͣ����Ӧ��  */
			route_data_confirm[4] = command[2];
			break;
		case EVENT_ROUTE_DATA_STATION_ACK: /* ��·���ݰ汾��һ�´������ͣ���վӦ��  */
			route_data_confirm[5] = command[2];
			break;		
		}	
		break;
   	} 
	free(command);
}


void data_frame_rcv_handle(char *data, uint8_t length, char *command, int amount, int count)
{
    static int comm_count = 0;
    switch(command[1]) {
       
    }
}
