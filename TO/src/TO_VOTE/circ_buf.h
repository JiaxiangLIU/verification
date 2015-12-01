/*
* 
* 
*
* File:     circ_buf.h
* Author:   MHW
* Create:   2015-08-28
*/

#include <pthread.h>

#ifndef __CIRC_BUF_H__
#define __CIRC_BUF_H__

#define CIRC_CNT(head, tail, size)    (((head) - (tail)) & ((size)-1))
#define CIRC_SPACE(head, tail, size)  CIRC_CNT((tail),((head)+1),(size))

#define CIRC_LEN 1024

typedef struct throttle_struct {
	int throttle_num;
	int auto_or_num;
} throttle_t;


struct circ_buf {
	pthread_mutex_t mutex;
	int head;
	int tail;
	throttle_t data[CIRC_LEN];
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	int write_recv_throttle_queue(int throttle_num, int auto_or_num);
	int read_recv_throttle_queue(int *throttle_num, int *auto_or_num);
	int write_sent_throttle_queue(int throttle_num, int auto_or_num);
	int read_sent_throttle_queue(int *throttle_num, int *auto_or_num);
		
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
