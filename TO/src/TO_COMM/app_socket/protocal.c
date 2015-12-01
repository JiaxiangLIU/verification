
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocal.h"
#include "app_socket.h"
#include "socket_include.h"
#include "ack_handle.h"
#include "system_error_code.h"

int route_data_confirm[ROUTE_DATA_NUM];

PROTOCAL_RESULT frame_parse(char *buf, int length)
{
    uint16_t len;
    uint16_t amount;
    uint16_t count;
    char *command;    /*include 3 bytes*/

    if (buf == NULL || 0 >= length)
    {
        fprintf(stderr, "frame err: illegal input.\n");
        return PROTOCAL_FAILURE;
    }
    
    if (buf[0] != 0x5a) //protocal header
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
            printf("begin to command receive\n");
            /******creat a new thread to process command******/
            if(threadpool_add(pool, &command_frame_rcv_handle, command, 0) != 0)
            {
                fprintf(stderr, "threadpool_add err.\n");
                return THREAD_ERROR;
            }
            break;  //����֡����
        case DATA_FRAME_CODE:            
            
            count = (uint8_t)buf[9];
            count = (count << 4) + ((uint8_t)buf[8] >> 4);
            amount = (uint8_t)buf[8];
            amount = ((amount & 0xff) << 4) + (uint8_t)buf[7];

            memcpy(&len, &buf[10], 2);
            len = len - 1;
            data_frame_rcv_handle(&buf[13], length - 14, &buf[5], amount, count);            
            break;  //���֡
       
        default:
            break;
    } 
    return PROTOCAL_SUCCESS;
}


/**
 * @function command_frame_encap
 * @brief ��command���д��
 * @param command ����
 * @param data �˴���������еĲ���
 * @param des Ŀ�ĵ�ַ
 * @param frame_len ֡����
 * @return �ɹ�����frameָ�룬ʧ�ܷ���NULL
 **/ 
static char *command_frame_encap(char command[2], char *data, uint16_t des, int *frame_len);


/**
 * ���֡������
 **/ 
typedef struct data_frame_descriptor
{
    uint16_t f_total_num;   /*֡����*/
    uint16_t f_cnt_num;     /*��ǰ֡���*/
}DATA_FD;

/**
 * @function data_frame_encap
 * @brief ��data֡���д��
 * @param command ���֡��Я��������ֶ�
 * @param data Ŀ�����
 * @param data_len ��ݳ���
 * @param des Ŀ�ĵ�ַ
 * @param frame_len ��װ��֡����
 * @return �ɹ�����frameָ�룬ʧ�ܷ���NULL
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
			ack_send_handle(stype, buf, buf_len);
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
                /*����֡���*/
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
                
                
				ack_send_handle(stype, buf, buf_len);
                snd_ret = socket_snd(stype, buf, buf_len);
			
				int route_data_confirm_count = 1;
				while(command_index[1] == EVENT_ROUTE_DATA_SEND) {
					usleep(900000);
					if(route_data_confirm[0] == data_fd.f_cnt_num) {
						break;
					}
					if(route_data_confirm_count++ == 5) break;
					socket_snd(stype, buf, buf_len);
				}
				
				while(command_index[1] == EVENT_ROUTE_DATA_CURVE) {
					usleep(900000);
					if(route_data_confirm[1] == data_fd.f_cnt_num) {
						break;
					}
					if(route_data_confirm_count++ == 5) break;
					socket_snd(stype, buf, buf_len);
				} 			
				
				while(command_index[1] == EVENT_ROUTE_DATA_LIMIT) {
					usleep(900000);
					if(route_data_confirm[2] == data_fd.f_cnt_num) {
						break;
					}
					if(route_data_confirm_count++ == 5) break;
					socket_snd(stype, buf, buf_len);
				} 			
				
				while(command_index[1] == EVENT_ROUTE_DATA_TELESEME) {
					usleep(900000);
					if(route_data_confirm[3] == data_fd.f_cnt_num) {
						break;
					}
					if(route_data_confirm_count++ == 5) break;
					socket_snd(stype, buf, buf_len);
				} 			
				while(command_index[1] == EVENT_ROUTE_DATA_TUNNEL) {
					usleep(900000);
					if(route_data_confirm[4] == data_fd.f_cnt_num) {
						break;
					}
					if(route_data_confirm_count++ == 5) break;
					socket_snd(stype, buf, buf_len);
				} 			
				while(command_index[1] == EVENT_ROUTE_DATA_STATION) {
					usleep(900000);
					if(route_data_confirm[5] == data_fd.f_cnt_num) {
						break;
					}
					if(route_data_confirm_count++ == 5) break;
					socket_snd(stype, buf, buf_len);
				} 			 				
				
				if(route_data_confirm_count == 5) {
					set_abnormal_code(SYS_ERROR_ROUTE_EXTRACT_FAILURE, 0X08);
				}
				
                free(buf);
                if(snd_ret == SOCKET_NOT_EXIST)
                {
                    return SOCKET_TYPE_ERR;
                }
                else if(snd_ret == SND_FAILURE)
                {
                    return SOCKET_SND_FAILURE;
                }
                
                
                data_index = data_index + data_to_send_len;     /*dataλ���ƶ�*/
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
    frame[2] = 0x00;    /*��Ϊ����ͨ��*/

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
    frame[2] = 0x00;    /*��Ϊ����ͨ��*/

    frame[3] = des >> 8;
    frame[4] = des & 0xff;

    memcpy(&frame[5], command, 2);
    
    frame[7] = fd.f_total_num >> 4;
    frame[8] = ((fd.f_total_num & 0x0f) << 4) + (fd.f_cnt_num >> 8);
    frame[9] = fd.f_cnt_num & 0xff;  
    
    index_len = data_len + 1;    //֡����
    frame[11] = index_len & 0xff;
    frame[10] = (index_len>>8) & 0xff;

    frame[12] = command[0];

    memcpy(&frame[13], data, data_len);

    frame[len-1] = 0x0c;    /* crc */
    
    *frame_len = len;
    return frame;
}
