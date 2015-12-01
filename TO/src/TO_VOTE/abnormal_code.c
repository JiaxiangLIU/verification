#include "abnormal_code.h"
#include "ack_handle.h"
#define CIRC_CNT(head, tail, size)    (((head) - (tail)) & ((size)-1))
#define CIRC_SPACE(head, tail, size)  CIRC_CNT((tail),((head)+1),(size))
#define CIR_BUF_LEN 1//CIR_BUF_LEN--CIR_BUF_LENGTH
static CIRC_BUF send_buf = {PTHREAD_MUTEX_INITIALIZER};


static int write_abnormal_code(CIRC_BUF *buf, ABNORMAL_S *value, int len)
{
	//*buf = {PTHREAD_MUTEX_INITIALIZER};
	int i, space_len, ret = -1;

	if (len < 0)
		return ret;

	pthread_mutex_lock(&buf->mutex);
	
	space_len = CIRC_SPACE(buf->head, buf->tail, CIRC_LEN);

	if (space_len >= len) {
		for (i = 0; i < len; i++) {
			buf->data[buf->head].abnormal_code = value[i].abnormal_code;
		//	buf->data[buf->head].socket_type = value[i].socket_type;
			buf->data[buf->head].des_brd = value[i].des_brd;
		//	buf->data[buf->head].frame_type = value[i].frame_type;
		//	buf->data[buf->head].frame_cmd[0] = value[i].frame_cmd[0];
		//	buf->data[buf->head].frame_cmd[1] = value[i].frame_cmd[1];
		//	buf->data[buf->head].send_data = value[i].send_data;
		//	buf->data[buf->head].send_data_len = value[i].send_data_len;
			buf->head = (buf->head + 1) & (CIRC_LEN - 1);
		}
		ret = len;
	}

	pthread_mutex_unlock(&buf->mutex);
	return ret;
}


int set_abnormal_code(SYSTEM_ERROR_CODE error, int dest)
{
	ABNORMAL_S value;
	value.abnormal_code = error;
	value.des_brd = dest;
	write_abnormal_code(&send_buf, &value, CIR_BUF_LEN);
	
	return 1;
}
static int read_abnormal_code(CIRC_BUF *buf, ABNORMAL_S *value, int len)
{
	int i, data_len, ret = -1;

	if (len < 0)
		return ret;

	pthread_mutex_lock(&buf->mutex);

	data_len = CIRC_CNT(buf->head, buf->tail, CIRC_LEN);

	if (data_len <= 0) {
		pthread_mutex_unlock(&buf->mutex);
		return ret;
	}

	if (data_len >= len) {
		for (i = 0; i < len; i++) {
			value[i] = buf->data[buf->tail];
			value[i].abnormal_code = buf->data[buf->tail].abnormal_code;
	//		value[i].socket_type = buf->data[buf->tail].socket_type;
			value[i].des_brd = buf->data[buf->tail].des_brd;
	//		value[i].frame_type = buf->data[buf->tail].frame_type;
	//		value[i].frame_cmd[0] = buf->data[buf->tail].frame_cmd[0];
	//		value[i].frame_cmd[1] = buf->data[buf->tail].frame_cmd[1];
	//		value[i].send_data = buf->data[buf->tail].send_data;
	//		value[i].send_data_len = buf->data[buf->tail].send_data_len;
			buf->tail = (buf->tail + 1) & (CIRC_LEN - 1);
		}
		ret = len;
	} else if (data_len < len) {
		for (i = 0; i < data_len; i++) {
			value[i] = buf->data[buf->tail];
			value[i].abnormal_code = buf->data[buf->tail].abnormal_code;
	//		value[i].socket_type = buf->data[buf->tail].socket_type;
			value[i].des_brd = buf->data[buf->tail].des_brd;
	//		value[i].frame_type = buf->data[buf->tail].frame_type;
	//		value[i].frame_cmd[0] = buf->data[buf->tail].frame_cmd[0];
	//		value[i].frame_cmd[1] = buf->data[buf->tail].frame_cmd[1];
	//		value[i].send_data = buf->data[buf->tail].send_data;
	//		value[i].send_data_len = buf->data[buf->tail].send_data_len;
			buf->tail = (buf->tail + 1) & (CIRC_LEN - 1);
		}
		ret = data_len;
	}

	pthread_mutex_unlock(&buf->mutex);
	
	return ret;
}


void send_abnormal_code(void)
{
	ABNORMAL_S value;
	int len;
	char frame_cmd[2];
	char arg;

	frame_cmd[0] = MY_SEND_EVENT_SOURCE;
	frame_cmd[1] = EVENT_SEND_ERROR;

	
	len = CIR_BUF_LEN;
	if (read_abnormal_code(&send_buf, &value, len) > 0) {
		arg = (char)value.abnormal_code;
		frame_encap(COM_SOC, value.des_brd, COMMAND_FRAME_CODE, 
		frame_cmd, &arg, 1);
	}
}
