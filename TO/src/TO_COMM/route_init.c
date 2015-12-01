
/**
 * @file route_init
 * @brief �����г̳�ʼ���¼�����
 * @author danny
 * @date 2015.01.29
 **/ 

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "handle.h"
#include "route_data.h"
#include "protocal.h"
#include "schedule.h"
#include "log.h"
#include "global.h"
#include "to_event.h"
/**
 * @function route_verify
 * @brief �Ϳ��ƺ�ȷ�Ͻ�·��Ϣ
 * @param 
 **/ 
static void route_verify();

/**
 * @function route_data_access
 * @brief ��ȡ��·����, �ȼ���Ƿ���ģ��,�����ֱ�ӷ���ģ��,������н�·��ȡ
 **/
void route_data_access(void);

/**
 * @function route_data_send
 * @brief ����İ巢�ͽ�·����
 **/
 void route_data_send(void);

void route_init(void *arg)
{
    uint8_t *type;
    type = (uint8_t *)arg;
    
    char command[2];
    char argument;
    uint8_t ret;
    switch(*type) 
    {
        case 0x10:
            /*ȷ�Ϲ���*/
            route_verify();
            break;
        case 0x20:
           // printf("receive command ,begin to route init ...\n");
            log_info("receive command ,begin to route init ...\n");
            route_data_access();
            command[0] = 0x20;
            command[1] = 0x10;
            argument = 0x9a;
            if ((ret = frame_encap(0x20, 0x0080, COMMAND_FRAME_CODE, command, &argument, 1)) == PROTOCAL_SUCCESS)
            {
                //printf("route_init.c: send command success.\n");
                log_info("route init send to core to ask whether ready sucess\n");
            }
            /*��·��ȡ*/
            break;
        case 0x30:
        	 // printf("route data send began\n");
        	  log_info("route data send began\n");
            route_data_send();
            /*���ͽ�·����*/
            break;
    }
/*    
    struct event_t *fin_event = event_create(EVENT_FINISH, FINISHED_EVENT, 0);    
    
    if (add_command(fin_event) == MUTEX_ERR)
    {
        log_error("route_init.c: event  mutex err.\n");
        exit(0);
    }
*/
    /*��ʵʱ��Ϣ�л�ȡ���Σ����أ���������ʼ��վ����ʼ���ݽ�·�ţ���ʼ��ؽ�·�ţ���ʼ�б�*/
    /*��ʱ�̱��ȡ�յ㳵վtmis�ţ��̶�����յ㳵վ�����ݽ�·�ţ��б𣬳�վ��*/
    /*����ƺн���ȷ�ϣ��ȴ����ƺеĻظ�*/
    /*��·��ȡ*/
    /*���ͽ�·��ȡ������*/
     
    /*����г̳�ʼ��*/
    
}

static void route_verify()
{
}

void route_data_access()
{
    /*����г�ģ��*/
    /*��·��ȡ*/
#if 1
    struct route_id_t new_id;
    ROUTE_EXTR_RESULT ret;
    ROUTE_LIST r_list;
    
    new_id.s_up_or_down = 1;
    new_id.s_dr_num = 2;
    new_id.s_mr_num = 2;
    new_id.ss_num = 213;
    new_id.e_up_or_down = 1;
    new_id.e_dr_num = 2;
    new_id.e_mr_num = 2;
    new_id.es_num = 148;
    r_list.account = 0;
#endif    
    ret = basedata_file_extr(new_id, &r_list);
    //printf("route_init.c: success.\n");
}

static long get_file_size(FILE *fp)
{
    long size;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    return size;
}


 void route_data_send(void)
{
    char command[2];

    char *index;
    FILE *fp;
    long f_size;

    command[1] = EVENT_ROUTE_DATA_SEND;
    command[0] = SOURCE_COM_0;  
    log_info("gradient_b send\n");
    fp = fopen("./route_data/gradient_b", "r");
    if (fp == NULL)
    {
        log_error("gradient_b %s\n",error_code_name(ERROR_CODE_WRITE_FILE));
        exit(0);
    }
    f_size = get_file_size(fp);
    index = (char *)malloc(f_size);
    fread(index, sizeof(char), f_size, fp);
    fclose(fp);
    if (frame_encap(0x04, CORE_0_DES, DATA_FRAME_CODE, command, index, f_size) != PROTOCAL_SUCCESS)
    {
        log_error("route_init.c: gradient frame err.\n");
    }
    free(index);    
    
    command[1] = EVENT_ROUTE_DATA_CURVE;
    log_info("curve began\n");
    fp = fopen("./route_data/curve_b", "r");
    if (fp == NULL)
    {
    	log_error("curve_b %s\n",error_code_name(ERROR_CODE_WRITE_FILE));
        exit(0);
    }
    f_size = get_file_size(fp);
    index = (char *)malloc(f_size);
    fread(index, sizeof(char), f_size, fp);
    fclose(fp);
    if (frame_encap(0x04, CORE_0_DES, DATA_FRAME_CODE, command, index, f_size) != PROTOCAL_SUCCESS)
    {
        log_error("route_init.c: curve frame err.\n");
    }
    free(index);    

    
    command[1] = EVENT_ROUTE_DATA_LIMIT;
    log_info("limit began/n");
    fp = fopen("./route_data/limit_b", "r");
    if (fp == NULL)
    {
    	log_error("limit_b %s\n",error_code_name(ERROR_CODE_WRITE_FILE));
        exit(0);
    }
    f_size = get_file_size(fp);
    index = (char *)malloc(f_size);
    fread(index, sizeof(char), f_size, fp);
    fclose(fp);
    if (frame_encap(0x04, CORE_0_DES, DATA_FRAME_CODE, command, index, f_size) != PROTOCAL_SUCCESS)
    {
        log_error("route_init.c: limit frame err.\n");
    }
    free(index);    
    
    command[1] = EVENT_ROUTE_DATA_TELESEME;
    log_info("teleseme began\n");
    fp = fopen("./route_data/teleseme_b", "r");
    if (fp == NULL)
    {
    	log_error("teleseme_b %s\n",error_code_name(ERROR_CODE_WRITE_FILE));
        exit(0);
    }
    f_size = get_file_size(fp);
    index = (char *)malloc(f_size);
    fread(index, sizeof(char), f_size, fp);
    fclose(fp);
    if (frame_encap(0x04, CORE_0_DES, DATA_FRAME_CODE, command, index, f_size) != PROTOCAL_SUCCESS)
    {
        log_info("route_init.c: teleseme frame err.\n");
    }
    free(index);    
    
    command[1] = EVENT_ROUTE_DATA_TUNNEL;
    log_info("tunnel began\n");
    fp = fopen("./route_data/tunnel_b", "r");
    if (fp == NULL)
    {
    	log_error("tunnel_b %s\n",error_code_name(ERROR_CODE_WRITE_FILE));
        exit(0);
    }
    f_size = get_file_size(fp);
    index = (char *)malloc(f_size);
    fread(index, sizeof(char), f_size, fp);
    fclose(fp);
    if (frame_encap(0x04, CORE_0_DES, DATA_FRAME_CODE, command, index, f_size) != PROTOCAL_SUCCESS)
    {
        log_error("route_init.c: tunnel frame err.\n");
    }
    free(index);
    
    command[1] = EVENT_ROUTE_DATA_STATION;
    log_info("station_b began\n");
    fp = fopen("./route_data/station_b", "r");
    if (fp == NULL)
    {
    	log_error("station_b %s\n",error_code_name(ERROR_CODE_WRITE_FILE));
        exit(0);
    }
    f_size = get_file_size(fp);
    index = (char *)malloc(f_size);
    fread(index, sizeof(char), f_size, fp);
    fclose(fp);
    if (frame_encap(0x04, CORE_0_DES, DATA_FRAME_CODE, command, index, f_size) != PROTOCAL_SUCCESS)
    {
        log_error("route_init.c: station_b frame err.\n");
    }
    free(index);    
 
    /*
    command[1] = 0x37;
    index = (char *)malloc(1);
    *index = 0xff;
    if (frame_encap(0x04, 0x0080, DATA_FRAME_CODE, command, index, 1) != PROTOCAL_SUCCESS) {
        log_error("route_init.c: finish frame err.\n");
    }
    free(index);
	*/
}
