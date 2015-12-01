
/**
 * @file to_comm.c
 * @brief é€šä¿¡æ¿ä¸šåŠ¡è¿›ç¨‹å…¥å£
 * @author danny
 * @date 2015.1.26
 **/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "log.h"

#include "comm_check.h"
#include "to_init.h"
#include "global.h"
#include "client_interface.h"
#include "protocal.h"
#include "to_status.h"
#include "ack_handle.h"

int main(void) {
	
   led_packet_init();
   init_ack_and_resend_table();//³õÊ¼»¯Ó¦´ğº¯Êı
   to_init();
#if 1
	while(comm_register() != 0) {
         log_error("communication check err");
    }
    usleep(5000);
    log_debug("comm register success.\n");
#endif
    comm_check_init();
/***call client_init() after register success
    if (client_init() != 0) {
        log_error("client check err");
        exit(0);
    }
*************************************/    
   // while(!status_flag.LKJ_status_flag);//wait until security platform data match with LKJ data 
#if 0
    int loop = 0;   
    while(!LKJ_TAX_status_flag && loop < 10)
    {
        usleep(1000);
        loop++;
    }
    log_info("LKJ_TAX_status: %x", LKJ_TAX_status);
#endif
/*    
    char command[2];
    char arg ;

    if (!LKJ_TAX_status_flag) {
        command[0] = 0x02;
        command[1] = 0x01;
        arg =  0x01;
        frame_encap(0x20, 0x08, COMMAND_FRAME_CODE, command, &arg, 1);
        while(!LKJ_TAX_status_flag);
    }
    command[0] = 0x02;
    command[1] = 0x01;
    arg = 0x02;
    frame_encap(0x20, 0x08, COMMAND_FRAME_CODE, command, &arg, 1);
    
    //cli_flag_g = 0x01; 
*/   
    while(1){
    	usleep(1000);
    };
    return 0;
}
