#include <string.h>
#include <time.h>
#include "app_socket.h"
#include "ack_handle.h"
#include "abnormal_code.h"

static resend_type resend_buf[RESEND_BUF_LEN];

static ack_type ack_table[] = { /* FIXME: commu board has two addresses */
//	{0, {EVENT_CORE_REGISTER, MY_SEND_EVENT_SOURCE}, {SOURCE_VOTE_0, TABLE_NONE}},
	{0, {EVENT_LKJ_VERSION_DIFFERENCE, MY_SEND_EVENT_SOURCE}, {SOURCE_VOTE_0, TABLE_NONE}},
	{1, {EVENT_TRIP_STATUS_NOTICE, MY_SEND_EVENT_SOURCE}, {SOURCE_VOTE_0, TABLE_NONE}},
	{2, {EVENT_DUTY_ASSISTING_CHANGE, MY_SEND_EVENT_SOURCE}, {SOURCE_VOTE_0, SOURCE_CORE_0, TABLE_NONE}},
	{3, {EVENT_MARSHALLING_RE_CHANGE, MY_SEND_EVENT_SOURCE}, {SOURCE_VOTE_0, TABLE_NONE}},
	{4, {EVENT_ROUTE_VERSION_SEND, MY_SEND_EVENT_SOURCE}, {SOURCE_CORE_0, TABLE_NONE}},
	{5, {EVENT_TRIP_CHANGE, MY_SEND_EVENT_SOURCE}, {SOURCE_VOTE_0, TABLE_NONE}},
	{6, {EVENT_MARSHALLING_CHANGE, MY_SEND_EVENT_SOURCE}, {SOURCE_VOTE_0, TABLE_NONE}},
	{7, {EVENT_CLOCK_SYNC, MY_SEND_EVENT_SOURCE}, {SOURCE_VOTE_0,SOURCE_CORE_0, TABLE_NONE}},
	{TABLE_NONE, {TABLE_NONE}, {TABLE_NONE}}
};


static send_type slave_ack_table[] = {
	{EVENT_ROUTE_VERSION_RESULT, SOURCE_CORE_0},/* TODO: add the event you need*/
	{EVENT_BROADCAST_COM_ADDR, SOURCE_VOTE_0},////
	{EVENT_VOTE_TRIP_INIT_START, SOURCE_VOTE_0},//
	{EVENT_SYS_TRIP_STATUS_SEND, SOURCE_VOTE_0},////
	{EVENT_MASTER_VOTE_EXCHANGE, SOURCE_VOTE_0},////
	{TABLE_NONE, TABLE_NONE}
};



static int get_ack_event_source_from_dest(unsigned short dest)
{
	int temp = -1;
	
	
	if (dest == CON_0_DES)
		temp = SOURCE_CONTROL_0;
	else if (dest == COM_0_DES)
		temp = SOURCE_COM_0;
	else if (dest == COM_1_DES)
		temp = SOURCE_COM_1;
	else if (dest == CORE_0_DES)
		temp = SOURCE_CORE_0;
	else if (dest == CORE_1_DES)
		temp = SOURCE_CORE_1;
	else if (dest == CORE_2_DES)
		temp = SOURCE_CORE_2;
	else if (dest == CORE_3_DES)
		temp = SOURCE_CORE_3;
	else if (dest == VOTE_0_DES)
		temp = SOURCE_VOTE_0;
	else
		temp = -1;
	
	return temp;
}

static int get_para_from_data(char *data, int len)
{
	int value = -1;
	
	if (data == NULL || len < 8)
		return -1;
	switch (data[1]) {
		case COMMAND_FRAME_CODE:
			value = (int)data[7];
			break;
		default:
			break;
	}
	
	return value;
}

static int get_cmd_from_data(char *data, int len)
{
	int value = -1;
	
	if (data == NULL || len < 8)
		return -1;
	switch (data[1]) {
		case COMMAND_FRAME_CODE:
		case DATA_FRAME_CODE:
			value = (int)data[6];
			break;
		default:
			break;
	}
	
	return value;
}

static int get_source_from_data(char *data, int len)
{
	int value = -1;
	
	if (data == NULL || len < 8)
		return -1;
	switch (data[1]) {
		case COMMAND_FRAME_CODE:
		case DATA_FRAME_CODE:
			value = (int)data[5];
			break;
		default:
			break;
	}
	
	return value;
}

static int get_dest_from_data(char *data, int len)
{
	int dest = -1;
	unsigned short recv_dest;
	
	if (data == NULL || len < 8)
		return -1;
	switch (data[1]) {
		case COMMAND_FRAME_CODE:
		case DATA_FRAME_CODE:
			recv_dest = (unsigned short)data[3];
			recv_dest = recv_dest << 8 | (unsigned short)data[4];
			dest = get_ack_event_source_from_dest(recv_dest);
			break;
		default:
			break;
	}
	
	return dest;
}

static void init_ack_table(ack_type *a_table)
{
	int i = 0;
	while (a_table->id != TABLE_NONE) {
		a_table->id = i;
		a_table++;
		i++;
	}
}

static void init_resend_table(resend_type *r_table)
{
	int i, j;
	
	for (i = 0; i < RESEND_BUF_LEN; i++) {
		r_table->id = TABLE_NONE;
		r_table->dest = -1;
		r_table->is_acked = 0;
	//	for (j = 0; j < BOARD_NUM; j++)
	//		r_table->ack_source[j] = TABLE_NONE;
		r_table->fd = -1;
		r_table->cnt = 0;
		pthread_mutex_init(&r_table->mutex, NULL);
		r_table++;
	}
}

void init_ack_and_resend_table(void)
{
	init_ack_table(ack_table);
	init_resend_table(resend_buf);
}

static int send_is_need_ack(int cmd, int source, ack_type *a_table, int *id)
{
	*id = -1;
	
	if (cmd < 0 || source < 0 || a_table == NULL)
		return NO_NEED_ACK;
			
	while (a_table->id != TABLE_NONE) {
		if (a_table->send.cmd == cmd && a_table->send.source == source) {
			*id = a_table->id; 
			return NEED_ACK;
		}
		a_table++;
	}
	
	return NO_NEED_ACK;
}

static int find_id_from_resend(int id, int dest, resend_type *r_table)
{
	int i;
	
	if (r_table == NULL || id < 0 || id == TABLE_NONE)
		return -1;
		
	for (i = 0; i < RESEND_BUF_LEN; i++) {
		pthread_mutex_lock(&r_table[i].mutex);
		if (r_table[i].id == id && r_table[i].dest == dest) {
			pthread_mutex_unlock(&r_table[i].mutex);
			return FOUND_ID;
		}
		pthread_mutex_unlock(&r_table[i].mutex);
	}
	
	return NO_FOUND_ID;
}


static int write_resend_to_table(int fd, char *data, int len, resend_type *r_table, int id, int dest)
{
	int i, j;
	
	if (fd < 0 || data == NULL || len > RESEND_DATA_LEN || len <= 0 || r_table == NULL || id < 0 || id == TABLE_NONE)
		return -1;
		
	for (i = 0; i < RESEND_BUF_LEN; i++) {
		pthread_mutex_lock(&r_table[i].mutex);
		if (r_table[i].id == TABLE_NONE) {
			r_table[i].id = id;
			r_table[i].dest = dest;
			r_table[i].fd = fd;
			r_table[i].is_acked = 0;
		//	for (j = 0; j < BOARD_NUM; j++)
		//		r_table[i].ack_source[j] = TABLE_NONE;
			r_table[i].cnt = 0;
			r_table[i].start_send_time = time(NULL);
			memcpy(&r_table[i].data, data, len);
			r_table[i].data_len = len;
			pthread_mutex_unlock(&r_table[i].mutex);
			return 1;
		}
		pthread_mutex_unlock(&r_table[i].mutex);
	}
	
	return -1;
}

static int find_all_souce_from_ack_table(int *source, ack_type *a_table, int id)
{
	int i;
	
	if (source < 0 || a_table == NULL || id < 0 || id == TABLE_NONE)
		return -1;
	
	while (a_table->id != TABLE_NONE) {
		if (a_table->id == id) {
			for (i = 0; i < BOARD_NUM; i++) {
				if (a_table->recv.source[i] != TABLE_NONE)
					*source = a_table->recv.source[i];
				else
					break;
			}
			return i;
		}
		a_table++;
	}
	
	return -2;
}

static int write_ack_source_to_table(int source, resend_type *r_table, int id)
{
	int i, j;
//	int temp_len, temp_source[BOARD_NUM];
	
	if (source < 0 || r_table == NULL || id < 0 || id == TABLE_NONE)
		return -1;
		
/*
	temp_len = find_all_souce_from_ack_table(temp_source, ack_table, id);
	if (temp_len <= 0 || temp_len > BOARD_NUM)
		return -1;

	for (j = 0; j < temp_len; j++) {
		if (temp_source[j] == source) {
			break;
		}
	}

	if (j >= temp_len)
		return -1;
*/
	for (i = 0; i < RESEND_BUF_LEN; i++) {
		pthread_mutex_lock(&r_table[i].mutex);
		if (r_table[i].id == id && r_table[i].dest == source) {
			r_table[i].is_acked = 1;
			r_table[i].dest = -1;
			r_table[i].id = TABLE_NONE;
			pthread_mutex_unlock(&r_table[i].mutex);
			return 1;
		}
		pthread_mutex_unlock(&r_table[i].mutex);
	}
	
	return -1;
}

void ack_send_handle(int fd, char *data, int len)
{
	int cmd, source, id, dest;
	
	if (fd < 0 || data == NULL || len <= 0)
		return;
	
	cmd = get_cmd_from_data(data, len);
	source = get_source_from_data(data, len);
	dest = get_dest_from_data(data, len);
	
	if (cmd >= 0 && source >= 0 && dest >= 0) {
		if (send_is_need_ack(cmd, source, ack_table, &id) == NEED_ACK) {
			if (find_id_from_resend(id, dest, resend_buf) == NO_FOUND_ID) {
				write_resend_to_table(fd, data, len, resend_buf, id, dest);
			}
		}
	}
}


static int recv_is_need_ack(int para, int source, ack_type *a_table, int *id)
{
	int i;
	
	*id = -1;
	
	if (para < 0 || source < 0 || a_table == NULL)
		return NO_NEED_ACK;
			
	while (a_table->id != TABLE_NONE) {
		if (a_table->send.cmd == para) {
			for (i = 0; i < BOARD_NUM; i++) {
				if (a_table->recv.source[i] == TABLE_NONE)
					break;
				else if (a_table->recv.source[i] == source) {
					*id = a_table->id; 
					return NEED_ACK;
				}
			}
		}
		a_table++;
	}
	
	return NO_NEED_ACK;
}

void ack_recv_handle(char *data, int len)
{
	int para, cmd, source, id, dest;
	
	if (data == NULL || len <= 0)
		return;
			
	cmd = get_cmd_from_data(data, len);
	switch (cmd) {
		case CORE_0_ACK:
		case CORE_1_ACK:
		case CORE_2_ACK:
		case CORE_3_ACK:
			break;
		case COM_0_ACK:
		case COM_1_ACK:
			break;
		case VOTE_ACK:
			break;
		case CONTROL_0_ACK:
	//	case CONTROL_1_ACK:
			break;
		default:
			return;
	}
	
	para = get_para_from_data(data, len);	
	source = get_source_from_data(data, len);
	//dest = get_dest_from_data(data, len);
	
	if (cmd >= 0 && source >= 0 /*&& dest >= 0*/) {
		if (recv_is_need_ack(para, source, ack_table, &id) == NEED_ACK) {
			if (find_id_from_resend(id, source, resend_buf) == FOUND_ID) {
				write_ack_source_to_table(source, resend_buf, id);
			}
		}
	}
}

static int ack_error_handle(int error_code)
{
	set_abnormal_code(error_code, VOTE_0_DES);
	//while (1)
		printf("ack_error_handle.\n");
	/* TODO: */
}

void ack_time_handle(void)
{
	int i, new_time, error_code;
	ABNORMAL_S value;
	
	new_time = time(NULL);
	
	for (i = 0; i < RESEND_BUF_LEN; i++) {
		pthread_mutex_lock(&resend_buf[i].mutex);
		if (resend_buf[i].id != TABLE_NONE && resend_buf[i].is_acked == 0) {
			if (new_time - resend_buf[i].start_send_time >= MAX_RESEND_TIMEOUT) {
				if (resend_buf[i].cnt < MAX_RESEND_CNT) {
					socket_snd(resend_buf[i].fd, resend_buf[i].data, resend_buf[i].data_len);
					resend_buf[i].cnt++;
					resend_buf[i].start_send_time = time( NULL);
				} else {
					/* TODO: */
					error_code = SYS_ERROR_CAN_FAILURE;
					ack_error_handle(error_code);
					//value.abnormal_code = SYS_ERROR_CAN_FAILURE;
					resend_buf[i].id = TABLE_NONE;
					resend_buf[i].dest = -1;
				}
			}
		}
		pthread_mutex_unlock(&resend_buf[i].mutex);
	}
}

static int find_cmd_source(send_type *s_table, int cmd, int source)
{
	if (s_table == NULL || cmd < 0 || source < 0)
		return -1;
	while(s_table->cmd != TABLE_NONE) {
		if (s_table->cmd == cmd && s_table->source == source)
			return 1;
		s_table++;
	}
	
	return 0;
}

static void respond_data(int cmd, int source)
{
	char command[2];
	char argument;
	unsigned short dest;	
	
	command[0] = MY_SEND_EVENT_SOURCE;
	
	switch (source) {
		case SOURCE_CORE_0:
			command[1] = CORE_0_ACK;
			dest = CORE_0_DES;
			break;
		case SOURCE_CORE_1:
			command[1] = CORE_1_ACK;
			dest = CORE_1_DES;
			break;
		case SOURCE_CORE_2:
			command[1] = CORE_2_ACK;
			dest = CORE_2_DES;
			break;
		case SOURCE_CORE_3:
			command[1] = CORE_3_ACK;
			dest = CORE_3_DES;
			break;
		case SOURCE_COM_0:
			command[1] = COM_0_ACK;
			dest = COM_0_DES;
			break;
		case SOURCE_COM_1:
			command[1] = COM_1_ACK;
			dest = COM_1_DES;
			break;
		case SOURCE_VOTE_0:
			command[1] = VOTE_ACK;
			dest = VOTE_0_DES;
			break;
		case SOURCE_CONTROL_0:
			command[1] = CONTROL_0_ACK;
			dest = CON_0_DES;
			break;
	//	case SOURCE_CONTROL_1:
	//		command[1] = CONTROL_1_ACK;
	//		break;
		default:
			return;
	}
	argument = (char)cmd; 
	frame_encap(COM_SOC, dest, COMMAND_FRAME_CODE, command, &argument, 1);
}

void slave_respond_handle(char *data, int len)
{
	int cmd, source;
	
	if (data == NULL || len <= 0)
		return;

	cmd = get_cmd_from_data(data, len);
	source = get_source_from_data(data, len);
	
	if (cmd >= 0 && source >= 0) {
		if (find_cmd_source(slave_ack_table, cmd, source) == 1)
			respond_data(cmd, source);
	}
}


