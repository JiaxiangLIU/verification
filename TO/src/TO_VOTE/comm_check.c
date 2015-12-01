#include "global.h"
#include "app_socket.h"
#include "schedule.h"
#include "protocal.h"
#include "log.h"
#include "threadpool.h"

#include "abnormal_code.h"
# define register_count 3
#include <pthread.h>

int can_check_flag = 0;

/* 心跳回馈的位操作
 *  VOTE --0X100
 * CTRL_2--0X80
   CTRL_1--0x40
   CORE_3--0x20
   CORE_2--0X10
   CORE_1--0X08
   CORE_0--0X04
   COMM_1--0X02
   COMM_0--0X01
*/   
static uint16_t comm_check_flag = 0;
static pthread_mutex_t check_flag_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
 * 注册信息
 * VOTE  --8
 * CONTROL_2--7
 * CONTROL_1--6
   CORE_3--5
   CORE_2--4
   CORE_1--3
   CORE_0--2
   COMM_1--1
   COMM_0--0
*/
static unsigned char regID[8]={0,0,0,0,0,0,0,0};
//static unsigned char heartID[8]={0,0,0,0,0,0,0,0};
static pthread_mutex_t reg_mutex = PTHREAD_MUTEX_INITIALIZER;

int comm_register(char boardid)
{
    pthread_mutex_lock(&reg_mutex);
    log_info("regID index: %d\n", boardid);
    regID[(int)boardid] = 1;
    pthread_mutex_unlock(&reg_mutex);
    return 0;
}

int comm_check() {
	int comm_flag = 0;
	int core_flag = 0;
	int control_flag = 0;
	int led_i;
	int count=0;
	while (!(comm_flag & core_flag & control_flag)) {
		pthread_mutex_lock(&reg_mutex);
		if (regID[2] | regID[3] | regID[4] | regID[5]) {
			for(led_i=0;led_i<4;led_i++)
				VoteLedCtrl(get_core_led(led_i),regID[led_i+2]);
			printf("核心板已连接...\n");
			core_flag = 1;
		}else
			printf("core is not register\n");

		if (regID[0] | regID[1]) {
			for(led_i=0;led_i<2;led_i++)
				VoteLedCtrl(get_comm_led(led_i),regID[led_i]);
			printf("通信板已连接...\n");
			if (regID[0] != 1)
				act_com = COM_1_DES;
			comm_flag = 1;
		}else
			printf("comm is not register\n");

		if (regID[6] | regID[7]) {
			for(led_i=0;led_i<1;led_i++)
				VoteLedCtrl(get_control_led(led_i),regID[led_i+6]);
			printf("控制盒已连接………\n");
			control_flag = 1;
		}else
			printf("control is not register\n");

		pthread_mutex_unlock(&reg_mutex);
		count++;
		if(count-1==register_count){
			printf("register failure\n");
			set_abnormal_code(SYS_ERROR_REGISTER_FAILURE, CON_0_DES);
			count=0;
		}
		sleep(4);   //停顿4s后再次检查
	}
	printf("初始化通信校验通过...\n");
}
/**
 * @function heartbeat_check 
 * @brief 进行心跳包校验
 **/ 



void heartbeat_check() {
	int i;
	int flag = 1;
	int comm_flag = 1;
	int core_flag = 1;
	int control_flag = 1;
	static int count = 0;
	if (count >= 30) {

		for (i = 0; i < 2; i++) {
			if (regID[i]) {
				flag = (comm_check_flag & (0x01 << i)) >> i;
				if (!flag) {
					printf("通信板心跳异常...\n");
					VoteLedCtrl(get_comm_led(i),0);
					set_abnormal_code(SYS_ERROR_NO_HEARTBEAT_COMM_MAIN,
							CON_0_DES);
					comm_flag = 0;
				} else
					printf("通信板心跳正常...\n");
			}
		}

		for (i = 2; i < 6; i++) {
			if (regID[i]) {
				flag = (comm_check_flag & (0x01 << i)) >> i;
				if (!flag) {
					VoteLedCtrl(get_core_led(i-2),0);
					printf("核心板心跳异常...\n");
					set_abnormal_code(SYS_ERROR_NO_HEARTBEAT_CORE, CON_0_DES);
					core_flag = 0;
				} else
					printf("核心板心跳正常...\n");
			}
		}

		for (i = 6; i < 8; i++) {
			if (regID[i]) {
				flag = (comm_check_flag & (0x01 << i)) >> i;
				if (!flag) {
					VoteLedCtrl(get_control_led(i-6),0);
					printf("控制盒心跳异常...\n");
					set_abnormal_code(SYS_ERROR_NO_HEARTBEAT_CONTROLLER_MAIN,
							CON_0_DES);
					control_flag = 0;
				} else
					printf("控制盒心跳正常...\n");
			}
		}

		pthread_mutex_lock(&check_flag_mutex);
		comm_check_flag = 0x00;
		pthread_mutex_unlock(&check_flag_mutex);
		count = 0;
	} else
		count++;

}


int comm_check_feedback(char *data)
{
    pthread_mutex_lock(&check_flag_mutex);
    switch(data[0]) {
        case COM_0_SOURCE: comm_check_flag = comm_check_flag | 0x01;
                     printf("check feed_back: %x\n", comm_check_flag);break;
        case COM_1_SOURCE: comm_check_flag = comm_check_flag | 0x02;
                     printf("check feed_back: %x\n", comm_check_flag);break;
        case CORE_0_SOURCE: comm_check_flag = comm_check_flag | 0x04;
                     printf("check feed_back: %x\n", comm_check_flag);break;
        case CORE_1_SOURCE: comm_check_flag = comm_check_flag | 0x08;
                     printf("check feed_back: %x\n", comm_check_flag);break;
        case CORE_2_SOURCE: comm_check_flag = comm_check_flag | 0x10;
                     printf("check feed_back: %x\n", comm_check_flag);break;
        case CORE_3_SOURCE: comm_check_flag = comm_check_flag | 0x20;
                     printf("check feed_back: %x\n", comm_check_flag);break;
        case CONTROL_0_SOURCE: comm_check_flag = comm_check_flag | 0x40;
                     printf("check feed_back: %x\n", comm_check_flag);break;
        case CONTROL_1_SOURCE: comm_check_flag = comm_check_flag | 0x80;
                     printf("check feed_back: %x\n", comm_check_flag);break;
        case VOTE_0_SOURCE: comm_check_flag = comm_check_flag | 0x100;
                     printf("check feed_back: %x\n", comm_check_flag);break;

    }
    pthread_mutex_unlock(&check_flag_mutex);
    return 0;
}
