/*
* 
* 
*
* File:     circ_buf.c
* Author:   MHW
* Create:   2015-08-28
*/

#include "circ_buf.h"
#include "error_code.h"
#include "log.h"

static struct circ_buf throttle_recv_queue = {PTHREAD_MUTEX_INITIALIZER, 0, 0};
static struct circ_buf throttle_sent_queue = {PTHREAD_MUTEX_INITIALIZER, 0, 0};


static int write_circ_buf(struct circ_buf *buf, int *throttle_num, int *auto_or_num, int len)
{
	int i, space_len, ret = -1;
	
	if (buf == NULL || throttle_num == NULL || auto_or_num == NULL || len == 0) {
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return ERROR_CODE_PARAMETER;
	}
	
	//printf("before write circ buf lock.\n");
	pthread_mutex_lock(&buf->mutex);
	//printf("after write circ buf lock.\n");
	space_len = CIRC_SPACE(buf->head, buf->tail, CIRC_LEN);
	//printf("space:%d\n",space_len);
	if (space_len >= len) {
		for (i = 0; i < len; i++) {
			log_info("write circ buf throttle: %d auto: %d\n", throttle_num[i], auto_or_num[i]);
			buf->data[buf->head].throttle_num = throttle_num[i];
			buf->data[buf->head].auto_or_num = auto_or_num[i];
			buf->head = (buf->head + 1) & (CIRC_LEN - 1);
		}
		ret = len;
	} else
		ret = ERROR_CODE_BUFFER_OVERFLOW;
	//printf("head : %d tail: %d",buf->head,buf->tail)	;
	pthread_mutex_unlock(&buf->mutex);
	
	return ret;
}



static int read_circ_buf(struct circ_buf *buf, int *throttle_num, int *auto_or_num, int len)
{
	int i, data_len, ret = -1;
	
	if (buf == NULL || throttle_num == NULL || auto_or_num == NULL || len == 0){
		log_warning("to_vote warning: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return ERROR_CODE_PARAMETER;
	}

	//printf("before read circ buf lock.\n");
		
	pthread_mutex_lock(&buf->mutex);
	//printf("after read circ buf lock.\n");

	data_len = CIRC_CNT(buf->head, buf->tail, CIRC_LEN);
	if (data_len > 0)
		log_info("read circ datalen is: %d\n", data_len);

	if (data_len >= len) {
		log_info("read circ buf throttle: %d auto: %d\n",buf->data[buf->tail].throttle_num,buf->data[buf->tail].auto_or_num);
		for (i = 0; i < len; i++) {
			throttle_num[i] = buf->data[buf->tail].throttle_num;
			auto_or_num[i] = buf->data[buf->tail].auto_or_num;
			buf->tail = (buf->tail + 1) & (CIRC_LEN - 1);
		}
		ret = len;
	} else
		ret = ERROR_CODE_BUFFER_EMPTY;

	
	pthread_mutex_unlock(&buf->mutex);
	//printf("read_cir ret is %x\n",ret);
	return ret;
}

int write_recv_throttle_queue(int throttle_num, int auto_or_num)
{
	return write_circ_buf(&throttle_recv_queue, &throttle_num, &auto_or_num, 1);
}

int read_recv_throttle_queue(int *throttle_num, int *auto_or_num)
{
	return read_circ_buf(&throttle_recv_queue, throttle_num, auto_or_num, 1);
}

int write_sent_throttle_queue(int throttle_num, int auto_or_num)
{
	return write_circ_buf(&throttle_sent_queue, &throttle_num, &auto_or_num, 1);
}

int read_sent_throttle_queue(int *throttle_num, int *auto_or_num)
{
	return read_circ_buf(&throttle_sent_queue, throttle_num, auto_or_num, 1);
}




