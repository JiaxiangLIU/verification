
/**
 * @file route_init
 * @brief 进行行程初始化事件处理
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
 * @brief 和控制盒确认交路信息
 * @param 
 **/ 
static void route_verify();

/**
 * @function route_data_access
 * @brief 获取交路数据, 先检查是否有模板,如果有直接返回模板,否则进行交路提取
 **/
void route_data_access(void);

/**
 * @function route_data_send
 * @brief 向核心板发送交路数据
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
            /*确认工作*/
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
            /*交路提取*/
            break;
        case 0x30:
        	 // printf("route data send began\n");
        	  log_info("route data send began\n");
            route_data_send();
            /*发送交路数据*/
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
    /*从实时信息中获取车次，车重，车长，起始车站，起始数据交路号，起始监控交路号，起始行别*/
    /*从时刻表获取终点车站tmis号，继而获得终点车站的数据交路号，行别，车站号*/
    /*与控制盒进行确认，等待控制盒的回复*/
    /*交路提取*/
    /*发送交路提取的数据*/
     
    /*完成行程初始化*/
    
}

static void route_verify()
{
}

void route_data_access()
{
    /*检查行程模板*/
    /*交路提取*/
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
