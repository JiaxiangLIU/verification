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
#include "circ_buf.h"
#include "throttle_handle.h"
#include "error_code.h"
#include "log.h"


static uint8_t throttle_table[THROTTLE_NUM_TOTAL] = {
	0x00,
	0x00, 0x08, 0x02, 0x0A,
	0x07, 0x0F, 0x06, 0x0E
};

#define THROTTLE_VALUE_INDEX 4

static uint8_t throttle_value_max = 0x9d;
static uint8_t throttle_value_min = 0x00;

static const char *can_in_out = "/dev/ttySP1";
static cssl_t *serout = NULL, *serin = NULL;



static uint8_t get_throttle_value_max()
{
	return throttle_value_max;
}

static uint8_t get_throttle_value_min()
{
	return throttle_value_min;
}

void set_throttle_value_max(uint8_t value)
{
	throttle_value_max = value;
}

void set_throttle_value_min(uint8_t value)
{
	throttle_value_min = value;
}

static int get_back_throttle(uint8_t value, int *throttle_num)
{
	uint32_t threshold_value;
	uint32_t max_value;
	uint32_t min_value;
	uint32_t current_value;
	uint8_t max;
	uint8_t min;
	int i;
	int flag;

	max = get_throttle_value_max();
	min = get_throttle_value_min();

	if (min >= max || value < min || value > max) {
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		//perror("throttle value error\n");
		return ERROR_CODE_PARAMETER;
	}

	max_value = max * 1000;
	min_value = min * 1000;
	current_value = value * 1000;

	threshold_value = (max_value - min_value) / (THROTTLE_NUM_TOTAL - 1);

	for (i = 0; i < THROTTLE_NUM_TOTAL; i++) {
		if (current_value <= min_value + i * threshold_value) {
			break;
		}
	}

	if (i < THROTTLE_NUM_TOTAL) {
		*throttle_num = i;
		flag = 1;
	} else
		flag = ERROR_CODE_OTHER;

	return flag;

}


static int set_back_throttle(uint8_t *value, int throttle_num)
{
	uint32_t threshold_value;
	uint32_t max_value;
	uint32_t min_value;
	uint32_t current_value;
	uint8_t max;
	uint8_t min;
	int temp_throttle;

	max = get_throttle_value_max();
	min = get_throttle_value_min();

	if (throttle_num < 0)
		temp_throttle = throttle_num * (-1);

	if (temp_throttle > MAX_THROTTLE_NUM || value == NULL) {
		//perror("set throttle value error\n");
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return ERROR_CODE_PARAMETER;
	}


	max_value = max * 1000;
	min_value = min * 1000;
	
	threshold_value = (max_value - min_value) / (THROTTLE_NUM_TOTAL - 1);
	
	current_value = min_value + temp_throttle * threshold_value;

	if (current_value >= threshold_value / 2)
		current_value -= threshold_value / 2;

	if (current_value > max_value)
		*value = max;
	else
		*value = current_value / 1000;

	if (temp_throttle == 7)
		*value = *value + 10;

	return 1;

}




static int set_throttle_to_data(uint8_t *data, int buf_size, int throttle_num, int auto_or_manual)
{
	if (buf_size != THROTTLE_DATA_LEN || data == NULL
		|| throttle_num < MIN_THROTTLE_NUM || throttle_num > MAX_THROTTLE_NUM) {
		//perror("set throttle data error\n");
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return ERROR_CODE_PARAMETER;
	}

	data[0] = THROTTLE_SEND_HEAD;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	data[7] = 0;
	data[8] = 0;


	if (auto_or_manual == MANUAL_DRIVE) {
		data[AUTO_MANUAL_INDEX] &= ~((unsigned)AUTO_DRIVE << AUTO_MANUAL_BIT);
	} else if (auto_or_manual == AUTO_DRIVE) {
		data[AUTO_MANUAL_INDEX] |= (unsigned)AUTO_DRIVE << AUTO_MANUAL_BIT;
	} else {
		//perror("set throttle data2 error\n");
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_LOGICAL_CONDITION_1));
		return ERROR_CODE_LOGICAL_CONDITION_1;
	}

	if (throttle_num > 0) {
		data[THROTTLE_TYPE_INDEX] = FRONT_STATE;
		data[THROTTLE_VALUE_INDEX] = 0;
		if (throttle_num > MAX_THROTTLE_NUM)
			return ERROR_CODE_LOGICAL_CONDITION_2;
		data[THROTTLE_NUM_INDEX] = throttle_table[throttle_num];
		data[THROTTLE_PRESET_INDEX] = 0x01;
	} else if (throttle_num == 0) {
		data[THROTTLE_TYPE_INDEX] = STOP_STATE;
		data[THROTTLE_VALUE_INDEX] = 0;
		data[THROTTLE_NUM_INDEX] = 0;
	} else {
		data[THROTTLE_TYPE_INDEX] = BACK_STATE;
		data[THROTTLE_NUM_INDEX] = 0;
		data[THROTTLE_PRESET_INDEX] = BACK_PRE_SET;
		data[8] = 0x08;
		if (set_back_throttle(&data[THROTTLE_VALUE_INDEX], throttle_num) < 0)
			return ERROR_CODE_LOGICAL_CONDITION_3;
	}

	return 1;
}


/*
 * jiaxiang: 从_data_中读取档位_throttle_num_以及手动自动标志_auto_or_manual_，
 * 如果是前进档则返回1，如果是后退档则返回0
 */
static int get_throttle_from_data(uint8_t *data, int len, int *throttle_num, int *auto_or_manual)
{
	int value;
	int i;
	int temp_num;

	if (data[0] != THROTTLE_RECV_HEAD || len != THROTTLE_DATA_LEN 
		|| data == NULL || throttle_num == NULL) {
		//perror("get throttle data error\n");
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return ERROR_CODE_PARAMETER;
	}

	switch (data[THROTTLE_TYPE_INDEX]) {
		case FRONT_STATE:
			if (data[7] == 1/* && data[8] == 0*/) {
				for (i = 1; i < THROTTLE_NUM_TOTAL; i++) {
					if (throttle_table[i] == data[THROTTLE_NUM_INDEX])
						break;
				}
				if (i < THROTTLE_NUM_TOTAL) {
					*throttle_num = i;
					value = 1;
				} else
					value = ERROR_CODE_LOGICAL_CONDITION_1;
			} else
				value = ERROR_CODE_LOGICAL_CONDITION_2;

			break;
		case STOP_STATE:
			if (data[7] == 0 || data[7] == 2/*&& data[8] == 0*/) {
				if (throttle_table[0] == data[THROTTLE_NUM_INDEX]) {
					*throttle_num = 0;
					value = 1;
				} else
					value = ERROR_CODE_LOGICAL_CONDITION_3;
			} else
				value = ERROR_CODE_LOGICAL_CONDITION_4;

			break;
		case BACK_STATE:

			if (get_back_throttle(data[THROTTLE_VALUE_INDEX], &temp_num) > 0) {
				*throttle_num = temp_num;
				value = 0;
			} else
				value = ERROR_CODE_LOGICAL_CONDITION_5;
			break;
		default:
			value = ERROR_CODE_LOGICAL_CONDITION_6;
			break;
	}

	*auto_or_manual = ((data[AUTO_MANUAL_INDEX] >> AUTO_MANUAL_BIT & 0x01) == 1) ? 1 : 0;

	return value;
}

void *receive_throttle(void *arg)
{    // a buff[]0-8  b buff 0-15
    int len, i;
    char recvbuf[9];
	int throttle_num;
	int auto_or_manual;
	int temp;
	int ret;


    cssl_start(); // jiaxiang: cssl是与输入输出板通信的方法
    // jiaxiang: 建立与CAN-IN-OUT的通信，与输入板输出板的通信不分端口
	serout = cssl_open(can_in_out, NULL, 0, 921600, 8, 0, 1);
    serin = serout;
	if (serout == NULL) {
		//fprintf(stderr, "open serial error!\n");
		log_error("to_vote error: %s\n", error_code_name(ERROR_CODE_OPEN_FILE));
		return 0;
	}

	while (1) {
		log_info(" begin to recv  \n");
		// jiaxiang: 接收数据放入recvbuf
		len = cssl_getdata(serin, (uint8_t *)recvbuf, sizeof(recvbuf));
		log_info(" begin to recv :Recv len = %d \n", len);
		for (i = 0 ; i < len; i++) {
			log_info("%02x", (unsigned char)recvbuf[i]);
		}
		log_info("\n");

		if (len > 0) {
			/*
			 * jiaxiang: 从_recvbuf_中读取档位信息到_throttle_num_，手动自动信息到
			 * _auto_or_manual_
			 */
			if ((temp = get_throttle_from_data(recvbuf, len, &throttle_num, &auto_or_manual)) == 1) {
				log_info("front throttle num is %d\n", throttle_num);
				// jiaxiang: 将档位和手/自动信息写入环形缓冲区throttle_recv_queue
				ret = write_recv_throttle_queue(throttle_num, auto_or_manual);
				if (ret <= 0)
					log_warning("enqueue err\n");
				else
					log_info("front enqueue is %d\n",throttle_num);
			} else if (temp == 0) { // jiaxiang: 负档位的case
				log_info("back throttle num is %d\n", throttle_num);
				// jiaxiang: 将档位的绝对值写入环形缓冲区throttle_recv_queue
				ret = write_recv_throttle_queue(throttle_num * (-1), auto_or_manual);
				if (ret <= 0)
					log_warning("back enqueue err\n");
				else
					log_info("back enqueue is %d\n", throttle_num * (-1));
			} else
				log_warning("get_throttle_from_data error%d\n", temp);
		}
		usleep(10000);
	}
}




void *sent_throttle(void *arg)
{ 
    int len;
    char send_buf[9];
	int throttle_num;
	int auto_or_manual;
	int ret;

	while (serout == NULL) {
		//fprintf(stderr, "open serial error!\n");
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_FD_PARAMETER));
		//return 0;
	}
	
	bzero(send_buf, sizeof(send_buf));
	
	while (1) {
		// jiaxiang: 从发送缓冲区throttle_sent_queue中读取档位信息和手/自动信息
		ret = read_sent_throttle_queue(&throttle_num, &auto_or_manual);
		if (ret > 0) {
			// jiaxiang: 将档位信息和手/自动信息发送到CAN-IN-OUT
			set_throttle_to_data(send_buf, sizeof(send_buf), throttle_num, auto_or_manual);
			cssl_putdata(serin, (uint8_t *)send_buf, sizeof(send_buf));
			log_info("sent enqueue is %d\n",throttle_num);
		} /*else
			printf("enqueue err\n");*/
		
		usleep(10000);
	}
}



