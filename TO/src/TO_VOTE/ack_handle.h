#ifndef RESEND_H
#define RESEND_H

#include <pthread.h>
#include "protocal.h"
#include "to_status.h"

#define MAX_RESEND_CNT 3
#define MAX_RESEND_TIMEOUT 3
#define RESEND_DATA_LEN 100
#define RESEND_BUF_LEN 20
#define TABLE_NONE 0xFFFF
#define BOARD_NUM 8

#define NO_NEED_ACK -1
#define NEED_ACK 1
#define NO_FOUND_ID 0
#define FOUND_ID 1

/*
#define CON_0_DES  0x02
#define COM_0_DES  0x200
#define COM_1_DES  0x100
#define CORE_0_DES 0x80
#define CORE_1_DES 0x40
#define CORE_2_DES 0x20
#define CORE_3_DES 0x10
#define VOTE_0_DES 0x08

#define COM_0_ID  9
#define COM_1_ID  8
#define CORE_0_ID 7
#define CORE_1_ID 6
#define CORE_2_ID 5
#define CORE_3_ID 4
#define VOTE_0_ID 3
#define CTRL_0_ID 2
#define CTRL_1_ID 1

typedef enum {
	SOURCE_COM_0 = 0x00,
	SOURCE_COM_1 = 0x01,
	SOURCE_CORE_0 = 0x02,
	SOURCE_CORE_1 = 0x03,
	SOURCE_CORE_2 = 0x04,
	SOURCE_CORE_3 = 0x05, 
	SOURCE_CONTROL_0 = 0x06,
	SOURCE_CONTROL_1 = 0x07,
	SOURCE_VOTE_0 = 0x08,
	SOURCE_INPUT_0 = 0x09,
	SOURCE_OUTPUT_0 = 0x0a,
	MAX_SOURCE
} EVENT_SOURCE;
*/

#define COM_0_ID  9
#define COM_1_ID  8
#define CORE_0_ID 7
#define CORE_1_ID 6
#define CORE_2_ID 5
#define CORE_3_ID 4
#define VOTE_0_ID 3
#define CTRL_0_ID 2
#define CTRL_1_ID 1


#if BOARDID == CORE_0_ID
#define MY_SEND_EVENT_SOURCE SOURCE_CORE_0
#elif BOARDID == CORE_1_ID
#define MY_SEND_EVENT_SOURCE SOURCE_CORE_1
#elif BOARDID == CORE_2_ID
#define MY_SEND_EVENT_SOURCE SOURCE_CORE_2
#elif BOARDID == CORE_3_ID
#define MY_SEND_EVENT_SOURCE SOURCE_CORE_3
#elif BOARDID == COM_0_ID
#define MY_SEND_EVENT_SOURCE SOURCE_COM_0
#elif BOARDID == COM_1_ID
#define MY_SEND_EVENT_SOURCE SOURCE_COM_1
#elif BOARDID == VOTE_0_ID
#define MY_SEND_EVENT_SOURCE SOURCE_VOTE_0
//#elif BOARDID == CTRL_0_ID
//#define MY_SEND_EVENT_SOURCE SOURCE_CONTROL_0
#else
#error no define board id
#endif

typedef struct {
	pthread_mutex_t mutex;
	int id;
	int dest;
	unsigned char data[RESEND_DATA_LEN];
	int data_len;
//	int ack_source[BOARD_NUM];
	int is_acked;
	int fd;
	int cnt;
	int start_send_time;
} resend_type;

typedef struct {
	int cmd;
	int source;
} send_type;

typedef struct {
	//int cmd;
	int source[BOARD_NUM];
} recv_type;

typedef struct {
	int id;
	send_type send;
	recv_type recv;
} ack_type;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/* master */
	void init_ack_and_resend_table(void);
	void ack_send_handle(int fd, char *data, int len);
	void ack_recv_handle(char *data, int len);
	void ack_time_handle(void);
	
	/* slave */
	void slave_respond_handle(char *data, int len);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif













