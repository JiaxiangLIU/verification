#ifndef CONTROL_BOX_H
#define CONTROL_BOX_H

#include <pthread.h>

#define PORT_CAN2NET 8000
#define PACKET_MAX_LEN 1024
#define CB_FRAME_LEN 64

typedef struct cb_sent_buf_struct {
	pthread_mutex_t mutex;
    unsigned char data[PACKET_MAX_LEN];
	int len;
} cb_sent_buf_t;

//extern cb_sent_buf_t cb_sent_buf;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	void *control_box_thread(void *arg);
	int control_box_send(int canid, unsigned char *data, int len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
