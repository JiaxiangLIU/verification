#include <pthread.h>

#ifndef __SOCKET_BUF_H__
#define __SOCKET_BUF_H__

#define SOCKET_CNT(head, tail, size)    (((head) - (tail)) & ((size)-1))
#define SOCKET_SPACE(head, tail, size)  SOCKET_CNT((tail),((head)+1),(size))

#define SOCKET_REALTIME_LEN 150
#define REALTIME_BUF_LEN 16

typedef struct {
	unsigned char type;
	int len;
	char data[SOCKET_REALTIME_LEN];
} socket_data_t;

typedef struct {
	pthread_mutex_t mutex;
	int head;
	int tail;
	socket_data_t buf[REALTIME_BUF_LEN];
} socket_buf_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	int write_realtime_buf(unsigned char type, char *data, int length);
	
	int send_realtime_buf(void);
			
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
