#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "cssl.h"
#include "led.h"
#define SEND_LEN 5


static char led_cores[4]={led_core_0,led_core_1,led_core_2,led_core_3};
static char led_comms[2]={led_comm_0,led_comm_1};
static char led_controls[2]={led_control_0,0};
char get_core_led(int i){
	if(i<0||i>3){
		printf("core led par is wrong \n");
	}
	return led_cores[i];
}
char get_comm_led(int i){
	if(i<0||i>1){
		printf("comm led par is wrong \n");
	}
	return led_comms[i];
}
char get_control_led(int i){
	if(i<0||i>1){
		printf("control led par is wrong \n");
	}
	return led_controls[i];
}




/*
 * led_op 0 led off  1 led on
 *
 */
int VoteLedCtrl(char LED_NUM , char LED_OP)
{
		int len, i;
		char recvbuf[128], temp[8], sndbuf[128];
		static cssl_t *serin, *serout1, *serout2;
		static int open_flag = 0;
		cssl_start();

			serout1 = cssl_open("/dev/ttySP3", NULL, 0, 921600, 8, 0, 1);

		if (serout1 == NULL) {
			fprintf(stderr, "open serial error!\n");
			return -1;
		} else {
			open_flag = 1;
		}
    sndbuf[0] = 0x5a;
    sndbuf[1] = 0x4e;
    sndbuf[2] = LED_NUM;
    sndbuf[3] = LED_OP;
    sndbuf[4] = LED_NUM+LED_OP;
 		cssl_putdata(serout1, sndbuf, 5);
            cssl_close(serout1);
			sleep(1);
	return 0;

}
