
/**
 * @file route_init.c
 * @brief 行程初始化
 * @author danny
 * @date 2015.02.04
 **/ 

#include <stdint.h>
#include <stdio.h>
#include "handle.h"
#include "protocal.h"
#include "schedule.h"
#include "log.h"
void route_init(void *arg)
{
    uint8_t *type;
    type = (uint8_t *)arg;
    char command[2];
    char argument;
    
    switch(*type)
    {
        case 0x20:
            //printf("verify route data.\n");
        	log_info("core board is ready for data\n");
            /*确认*/
            remove("./route_data/gradient_b");
            remove("./route_data/curve_b");
            remove("./route_data/limit_b");
            remove("./route_data/teleseme_b");
            remove("./route_data/tunnel_b");
            remove("./route_data/station_b");
            command[0] = 0x30;
            command[1] = 0x10;
            argument = 0x00;
            if((frame_encap(COM_SOC, get_commu_addr(), COMMAND_FRAME_CODE, command, &argument, 1)) != PROTOCAL_SUCCESS )

            {
                log_error("route_init.c: send command error.\n");
            }
            break;
        case 0x30:
            printf("optimize begin.\n");
            log_info("route data receive over,begin to optimize\n");
            opt_init();
            command[0] = 0xff;
            command[1] = 0x10;
            argument = 0x00; 
            if(frame_encap(COM_SOC, VOTE_0_DES, COMMAND_FRAME_CODE, command, &argument, 1) != PROTOCAL_SUCCESS) {
                
                log_error("route_init.c: send command error.\n");
            }
//            command[0] = 0xee;
//		   command[1] = 0x10;
//		   char argument = 0x00;
//		   if(frame_encap(0x20, 0x008, COMMAND_FRAME_CODE, command, &argument, 1) != PROTOCAL_SUCCESS) {
//
//			   printf("route_init.c: send command error.\n");
//		   }
            break;
        default:
            break;
    }

   /* struct event_t *fin_event = event_create(EVENT_FINISH, FINISHED_EVENT, 0);  */  
}

