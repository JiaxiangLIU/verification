
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "protocal.h"
#include "app_socket.h"
#include "socket_include.h"
#include "to_status.h"
#include "ack_handle.h"

PROTOCAL_RESULT frame_parse(char *buf, int length)
{
    uint16_t len;
    uint16_t amount;
    uint16_t count;
    char *command;    /*命令字段为3字节，带参数*/

    if (buf == NULL || 0 >= length)
    {
        fprintf(stderr, "frame err: illegal input.\n");
        return PROTOCAL_FAILURE;
    }
    
    if (buf[0] != 0x5a) //协议头
    {
        fprintf(stderr, "frame err: frame err.\n");
        return FRAME_ERROR;
    } 
    ack_recv_handle(buf, length);
		slave_respond_handle(buf, length);
    switch((uint8_t)buf[1])
    {
        case COMMAND_FRAME_CODE:
            if ((command = (char *)malloc(sizeof(char) * 3)) == NULL)
            {
                fprintf(stderr, "system err: malloc err.\n");
                return PROTOCAL_MALLOC_ERROR;
            }
            
            memcpy(command, &buf[5], 3);
            /********生成一个新线程去处理命令****/
            if(threadpool_add(pool, &command_frame_rcv_handle, command, 0) != 0)
            {
                fprintf(stderr, "threadpool_add err.\n");
                return THREAD_ERROR;
            }
            break;  //命令帧解析
        case DATA_FRAME_CODE:            
            
			count = (uint16_t)buf[9];
			count = count | ((uint16_t)buf[8] & 0x000f) << 8;
			amount = (uint16_t)buf[7];
			amount = amount << 4 | (uint16_t)buf[8] >> 4;

			len = (unsigned short)buf[10];
			len = len << 8 | (unsigned short)buf[11];
			len = len - 1;
			data_frame_rcv_handle(&buf[13], len, &buf[5], amount, count);            
            break;  //数据帧
        default:
            break;
    } 
    return PROTOCAL_SUCCESS;
}


/**
 * @function command_frame_encap
 * @brief 对command进行打包
 * @param command 命令
 * @param data 此处代表命令中的参数
 * @param des 目的地址
 * @param frame_len 帧长度
 * @return 成功返回frame指针，失败返回NULL
 **/ 
static char *command_frame_encap(char command[2], char *data, uint16_t des, int *frame_len);


/**
 * 数据帧描述符
 **/ 
typedef struct data_frame_descriptor
{
    uint16_t f_total_num;   /*帧总数*/
    uint16_t f_cnt_num;     /*当前帧序号*/
}DATA_FD;

/**
 * @function data_frame_encap
 * @brief 对data帧进行打包
 * @param command 数据帧中携带的命令字段
 * @param data 目标数据
 * @param data_len 数据长度
 * @param des 目的地址
 * @param frame_len 封装的帧长度
 * @return 成功返回frame指针，失败返回NULL
 **/  
static char *data_frame_encap(char command[2], char *data, uint16_t des, DATA_FD fd, int data_len, int *frame_len);


PROTOCAL_RESULT frame_encap(uint8_t stype, uint16_t des, uint8_t ftype, char command[2], char *data, int length)
{
    char    *buf;
    char    *command_index;
    char    *data_index;
    int     buf_len;

    int     data_to_send_len;
    int     index_len;
    int     max_data_len = MAX_DATA_FRAME_SIZE - DATA_FRAME_HEAD_SIZE;
    DATA_FD data_fd;

    SOCKET_RESULT snd_ret;

	if (get_sys_status() == SYS_MAINTENANCE)
		return PROTOCAL_FAILURE;

    if (data == NULL || 0 >= length)
    {
        fprintf(stderr, "frame err: illegal input.\n");
        return PROTOCAL_FAILURE;
    }
    
    switch(ftype)
    {
        case COMMAND_FRAME_CODE:

            command_index = command;
            data_index = data;
            
            buf = command_frame_encap(command_index, data_index, des, &buf_len); 
            if (buf == NULL)
                return FRAME_ENCAP_FAILURE;

			ack_send_handle((int)stype, buf, buf_len);
            snd_ret = socket_snd(stype, buf, buf_len);
            free(buf);
            if(snd_ret == SOCKET_NOT_EXIST)
            {
                return SOCKET_TYPE_ERR;
            }
            else if(snd_ret == SND_FAILURE)
            {
                return SOCKET_SND_FAILURE; 
            }
            
        break;

        case DATA_FRAME_CODE:
            
            command_index = command;
            data_index = data;
            
            if (length <= max_data_len)
            {    
                data_to_send_len = length;
                index_len = 0;
            }
            else
            {
                data_to_send_len = max_data_len;
                index_len = length - max_data_len;
            }
            if (length % max_data_len == 0)
                data_fd.f_total_num = length / max_data_len;
            else
                data_fd.f_total_num = length / max_data_len + 1;
            data_fd.f_cnt_num = -1;
            while(data_to_send_len != 0)
            {
                data_fd.f_cnt_num ++;
                /*数据帧打包*/
                buf = data_frame_encap(command_index, data_index, des, data_fd, data_to_send_len, &buf_len);
#if 0
                int _i;
                printf("\ndata frame :");
                for (_i = 0 ; _i < buf_len; _i++)
                    printf("%x  ", buf[_i]);
                printf("\n");
#endif
                if(buf == NULL)
                    return FRAME_ENCAP_FAILURE;
                
                /*发送数据帧*/

				if (stype == DAT_VOTE_SOC) {
					write_realtime_buf(stype, buf, buf_len);
					free(buf);
				} else {
									ack_send_handle((int)stype, buf, buf_len);
	                snd_ret = socket_snd(stype, buf, buf_len);
	                free(buf);
	                if(snd_ret == SOCKET_NOT_EXIST)
	                {
	                    return SOCKET_TYPE_ERR;
	                }
	                else if(snd_ret == SND_FAILURE)
	                {
	                    return SOCKET_SND_FAILURE;
	                }
				}
                
                /*剩下未发送数据长度计算*/
                data_index = data_index + data_to_send_len;     /*data位置移动*/
                if (index_len <= max_data_len)
                {
                    data_to_send_len = index_len;
                    index_len = 0;
                }
                else
                {
                    data_to_send_len = max_data_len;
                    index_len = index_len - max_data_len;
                }    
            }
        break;

        default:
        break;
    }



    return PROTOCAL_SUCCESS;
}


static char *command_frame_encap(char *command, char *data, uint16_t des, int *frame_len)
{
    char *frame;
    int  len;
    frame = (char *)malloc(sizeof(char) * COMMAND_FRAME_SIZE);
    if(frame == NULL)
    {
        fprintf(stderr, "system err: malloc err.\n");
        return NULL;
    }

    len = COMMAND_FRAME_SIZE;
    frame[0] = 0x5a;
    frame[1] = 0x58;
    frame[2] = 0x00;    /*均为板外通信*/

    frame[3] = des >> 8;
    frame[4] = des & 0xff;

    memcpy(&frame[5], command, 2);

    memcpy(&frame[7], data, 1);

    frame[8] = 0;
    
    *frame_len = len;
    return frame;
}


static char *data_frame_encap(char command[2], char *data, uint16_t des, DATA_FD fd, int data_len, int *frame_len)
{
    char *frame;
    int len;
    uint16_t index_len;
    len = data_len + DATA_FRAME_HEAD_SIZE;
    frame = (char *)malloc(sizeof(char) * len);
    if(frame == NULL)
    {
        fprintf(stderr, "system err: malloc err.\n");
        return NULL;
    }

    frame[0] = 0x5a;
    frame[1] = 0x52;
    frame[2] = 0x00;    /*均为板外通信*/

    frame[3] = des >> 8;
    frame[4] = des & 0xff;

    memcpy(&frame[5], command, 2);
    
    frame[7] = fd.f_total_num >> 4;
    frame[8] = ((fd.f_total_num & 0x0f) << 4) + (fd.f_cnt_num >> 8);
    frame[9] = fd.f_cnt_num & 0xff;  
    
    index_len = data_len + 1;    //帧长度
    frame[11] = index_len & 0xff;
    frame[10] = (index_len>>8) & 0xff;

    frame[12] = command[0];

    memcpy(&frame[13], data, data_len);

    frame[len-1] = 0x0c;    /* crc */
    
    *frame_len = len;
    return frame;
}

static int master_commu_addr = COM_0_DES;
static pthread_mutex_t commu_addr_mutex = PTHREAD_MUTEX_INITIALIZER;


void set_commu_addr(int addr)
{
	pthread_mutex_lock(&commu_addr_mutex);
	if (addr == COM_0_DES || addr == COM_1_DES)
		master_commu_addr = addr;
	pthread_mutex_unlock(&commu_addr_mutex);
}

unsigned short get_commu_addr(void)
{
	unsigned short temp;
	
	pthread_mutex_lock(&commu_addr_mutex);
	if (master_commu_addr == COM_0_DES || master_commu_addr == COM_1_DES)
		temp = (unsigned short)master_commu_addr;
	else
		temp = (unsigned short)COM_0_DES;
	pthread_mutex_unlock(&commu_addr_mutex);

	return temp;

}
char get_core_event_source(int board_id)
{
	int temp;
	
	if (board_id == CORE_0_ID)
		temp = SOURCE_CORE_0;
	else if (board_id == CORE_1_ID)
		temp = SOURCE_CORE_1;
	else if (board_id == CORE_2_ID)
		temp = SOURCE_CORE_2;
	else if (board_id == CORE_3_ID)
		temp = SOURCE_CORE_3;
	else
		temp = SOURCE_CORE_0;
	
	return (char)temp;
}


