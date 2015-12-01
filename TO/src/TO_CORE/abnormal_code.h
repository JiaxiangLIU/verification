/*
*
*/
#ifndef _ABNORMAL_CODE_H
#define _ABNORMAL_CODE_H


#include <pthread.h>
#include "system_error_code.h"
#define CIRC_LEN 256

typedef struct {
	SYSTEM_ERROR_CODE abnormal_code;
//	int socket_type;////
	int des_brd;//des_brd--destination board
//	int frame_type;////assign DATA_FRAME_CODE or COMMAND_FRAME_CODE
//	char frame_cmd[2];//assign event_source and event
//	char *send_data;
//	int send_data_len;//send_data_len--send_data_length
}ABNORMAL_S;//ABNORMAL_S--ABNORMAL_STRUCT

typedef struct circ_buf {
	pthread_mutex_t mutex;
	int head;
	int tail;
	ABNORMAL_S data[CIRC_LEN];
} CIRC_BUF;


/*
*
*/
	int set_abnormal_code(SYSTEM_ERROR_CODE error, int dest);
	void send_abnormal_code(void);
#endif