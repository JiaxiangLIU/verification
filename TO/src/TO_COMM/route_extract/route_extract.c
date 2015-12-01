#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "route_data.h"
#include "cfgFileRead.h"
#include "dataStruct.h"
#include "log.h"

#define MAX_ROUTE_NUM     32    //���·��    
uint32_t MAX_TELE=0;
CONVERT_MILE_NODE *cm_node=NULL;
static int create_cm_node(void *data,int length);
typedef struct station_addr_t
{
    uint8_t     dr_num;       /**<��·��*/
    uint8_t     mr_num;
    uint8_t     up_or_down;  /**<������*/
    uint16_t    s_num;       /**<��վ��*/
    uint32_t    file_addr;   /**<�ļ�λ��*/
}STATION_ADDR_T;

/**
 * @function basedata_preprocess
 * @brief ������ļ���ȡԤ����
 * @param bd_file Ŀ������ļ�
 * @param ss_addr Ԥ������ɵ���ʼ��վλ����Ϣ
 * @param es_addr Ԥ������ɵ���ֹ��վλ����Ϣ
 * @return ��ȡ���
 **/ 
static ROUTE_EXTR_RESULT basedata_preprocess(FILE *bd_file, 
        STATION_ADDR_T *ss_addr, STATION_ADDR_T *es_addr);

/**
 * @function basedata_parse
 * @brief �ӻ�����ļ�����ȡ��·���
 * @param bd_file Ŀ������ļ�
 * @param ss_addr ��ʼ��վ��Ϣ
 * @param es_addr ��ֹ��վ��Ϣ
 * @return ��·��ȡ�Ľ��
 **/ 
static ROUTE_EXTR_RESULT basedata_parse(FILE *bd_file, ROUTE_LIST *r_list, 
        STATION_ADDR_T *ss_addr, STATION_ADDR_T *es_addr);


ROUTE_EXTR_RESULT basedata_file_extr(struct route_id_t   r_id, ROUTE_LIST  *r_list)
{
   FILE    *bd_file;

   STATION_ADDR_T ss_addr, es_addr;

   ROUTE_EXTR_RESULT ret;
   
   bd_file = fopen("./route_data/basedata.dat", "r");
   if (bd_file == NULL)
   {
       fprintf(stderr, "system err: failed to open basedata.dat.\n");
       return FILE_ERROR;
   }

   if (r_list == NULL)
   {
       fprintf(stderr, "system err: route list not initial.\n");
       return UNKNOWN_ERR;
   }
    

#if 1 
   ss_addr.dr_num = r_id.s_dr_num;
   es_addr.dr_num = r_id.e_dr_num;
   ss_addr.mr_num = r_id.s_mr_num;
   es_addr.mr_num = r_id.e_mr_num;
   ss_addr.s_num = r_id.ss_num;
   es_addr.s_num = r_id.es_num;
   ss_addr.up_or_down = r_id.s_up_or_down;
   es_addr.up_or_down = r_id.e_up_or_down;
   ret = basedata_preprocess(bd_file, &ss_addr, &es_addr);
#endif


   if (ret == SS_NOT_FOUND || ret == ES_NOT_FOUND || ret == FILE_ERROR || ret == UNKNOWN_ERR)
        return ret;
   
   ret = basedata_parse(bd_file, r_list, &ss_addr, &es_addr); 
   
   return EXTR_SUCCESS;
}

#define UP_R_ENTER_ADDR   0X80  //���н�·��ڵ�ַ�����ļ���λ��
#define DOWN_R_ENTER_ADDR 0X100 //���н�·��ڵ�ַ�����ļ���λ��


/**
 * @function get_station_addr_by_snum
 * @brief ��ݳ�վ�Ż�ȡ��վ��ַ
 * @param bd_file 
 * @param s_addr ������ų�վ��ַ��Ϣ
 * @return 
 **/
static int get_station_addr_by_snum(FILE *bd_file, STATION_ADDR_T *s_addr);

/**
 * @function basedata_preprocess
 * @brief ͨ��վ���ҵ���ʼ���յ㳵վ���ļ��е�λ��
 * @param bd_file basedata�ļ�
 * @param ss_addr �������ʼ��վ�й���Ϣ���ļ�λ�������ڴ�
 * @param es_addr ������յ㳵վ�й���Ϣ���ļ�λ�������ڴ�
 * @return ��·��ȡ�Ľ��
 **/ 
static ROUTE_EXTR_RESULT basedata_preprocess(FILE *bd_file, 
        STATION_ADDR_T *ss_addr, STATION_ADDR_T *es_addr)
{

   int ret;
   if (bd_file == NULL)   /* RETURN */
   {
       fprintf(stderr, "system err: basedata file not exist.\n");
       return FILE_ERROR;
   }
   if (ss_addr == NULL || es_addr == NULL)  /* RETURN */
   {
       fprintf(stderr, "system err: STATION_ADDR_T not initial.\n");
       return UNKNOWN_ERR;
   }

   ret = get_station_addr_by_snum(bd_file, ss_addr);
   
   if (ret == -1)    
       return FILE_ERROR;
   else if(ret == -2)   /* RETURN */ 
       return SS_NOT_FOUND;

   
   ret = get_station_addr_by_snum(bd_file, es_addr);
   
   if (ret == -1)
       return FILE_ERROR;
   else if(ret == -2)   /* RETURN */
       return ES_NOT_FOUND;

   return EXTR_SUCCESS;   
}

/**
 * @function get_station_addr_by_snum
 * @brief ͨ��վ���ҵ���վ����λ��
 * @param bd_file basedata����ļ�
 * @param s_addr �ṩ������ݽ�·�ţ���վ��
 * @return -1 �ļ�����; -2 û�ҵ��ó�վ; 0 �ɹ�
 */ 
static int get_station_addr_by_snum(FILE *bd_file, STATION_ADDR_T *s_addr)
{

   uint32_t index_offset, next_offset, station_offset;
   uint16_t index_num;
   uint32_t s_count;  // s_count: the amount of stations
   uint32_t i; //for loop

   if (bd_file == NULL)
       return -1;
   if (s_addr == NULL)
   {
      fprintf(stderr, "system err: STATION_ADDR_T not initial.\n");
      return -2;
   }

   if (s_addr->up_or_down == 1)
       index_offset = UP_R_ENTER_ADDR;
   else if(s_addr->up_or_down == 0)
       index_offset = DOWN_R_ENTER_ADDR;
   else
   {
      fprintf(stderr, "system err: up_or_down err.\n");
      return -2; 
   }

   if (s_addr->dr_num >= MAX_ROUTE_NUM)
   {
       fprintf(stderr, "system err: route number err.\n");
       return -2;
   }

   /* ��Ѱ��վ��ڵ�ַ */
   if( fseek(bd_file, index_offset + 4 * s_addr->dr_num, SEEK_SET) == -1)
   {
       fprintf(stderr, "file err: failed to fseek.\n");
       return -1;
   }

   if(fread(&index_offset, 4,  1, bd_file) == 0 
           || fread(&next_offset, 4, 1, bd_file) == 0)
   {
       fprintf(stderr, "file err: failed to fread.\n");
       return -1;
   }

   //��վ���ڽ�·�ĳ�վ����
   s_count = (next_offset - index_offset) / 2;

   if( fseek(bd_file, index_offset, SEEK_SET) == -1)
   {
       fprintf(stderr, "file err: failed to fseek.\n");
       return -1;
   }
   
   if (fread(&station_offset, 4,  1, bd_file) == 0)
   {
       fprintf(stderr, "file err: failed to fread.\n");
       return -1;
   }       

   
   if(station_offset == 0xFFFFFFFF)
   {
       fprintf(stderr, "system err: route not exist.\n");
       return -2;
   }

   //i��1��ʼ����Ϊi=0����stationû������
   for (i = 1; i < s_count; i++)   
   {
        if (fseek(bd_file, index_offset + i * 4, SEEK_SET) == -1)
        {
           fprintf(stderr, "file err: failed to fseek.\n");
           return -1;
        }
        
        if (fread(&station_offset, 4,  1, bd_file) == 0)
        {
           fprintf(stderr, "file err: failed to fread.\n");
           return -1;
        }       

        if (fseek(bd_file, station_offset + 4, SEEK_SET) == -1)
        {
           fprintf(stderr, "file err: failed to fseek.\n");
           return -1;
        }
        if (fread(&index_num, 2,  1, bd_file) == 0)
        {
           fprintf(stderr, "file err: failed to fread.\n");
           return -1;
        }
        
        if (s_addr->s_num == index_num)
        {
            s_addr->file_addr = station_offset;
            break;
        } 
   }
   if (i == s_count)
   {
       fprintf(stderr, "system err: station not exist.\n");
       return -2;
   }

   return 0;
}

/**
 * @function limit_code_table_extr
 * @brief ��ȡ���ٴ�ű�
 * @param bd_file Ŀ������ļ�
 * @return ����һ����ά���飬�ڶ�ά����Ϊ8
 **/ 
static uint16_t (*limit_code_table_extr(FILE *bd_file))[8];   

/**
 * @function station_data_parse
 * @brief basedata�г�վ�����ȡ
 * @param data Ŀ�����
 * @param dr_num ��ݽ�·��
 * @param tele Ϊ�˼��㳵վĿǰ�����
 * @return �ɹ�����һ��struct station_t��ָ�������ʧ�ܷ���NULL
 **/ 
static struct station_t *station_data_parse(char data[16], uint16_t dr_num, struct tele_t *tele);

/**
 * @function tele_data_parse
 * @brief basedata���źŻ������ȡ
 * @param data Ŀ�����
 * @param dr_num ��ݽ�·��
 * @param pre_tele ���źŻ�
 * @param distance ������źŻ����
 * @return �ɹ�����һ��struct tele��ָ�������ʧ�ܷ���NULL
 **/ 
static struct tele_t *tele_data_parse(char data[16], uint16_t dr_num, struct tele_t *pre_tele, uint32_t distance);

/**
 * @function branch_change_data_parse
 * @brief ��ȡ֧��ת����ݲ��ж��Ƿ����ת��
 * @param data Ŀ�����
 * @return �ɹ�����֧��ת�ƽṹ��ָ�������ʧ�ܷ���NULL
 **/ 
static struct branch_change_t *branch_change_data_parse(char data[16]);

/**
 * @function branch_confirm_from_ts
 * @brief ���ʱ�̱�(ts: train schedule)ȷ��Ĭ�ϵ�֧��ת��
 * @param branch_change ֧��ת�ƽṹ��
 * @param train_sch ʱ�̱�ṹ��
 * @param count ��ǰ��վλ��
 * @param bd_file ������ļ�
 * @param cnt_file_addr ��ǰ�ļ�λ��
 * @return �ɹ�����0
 **/
static int branch_confirm_from_ts(struct branch_change_t *branch_change, 
                                  struct rt_extrRoadStationTime *train_sch, int count,
                                    FILE *bd_file, uint32_t cnt_file_addr);

/**
 * @function route_change_data_parse
 * @brief ��ȡ��·ת����ݲ��ж��Ƿ�ת��
 * @param data Ŀ�����
 * @param cnt_mr_num ��ǰ��ؽ�·�ţ����н�·ת�ƹ����ж�
 * @return �ɹ�����struct route_change_t �ṹ��ָ�������ʧ�ܷ���NULL
 **/ 
static struct route_change_t  *route_change_data_parse(char data[16], uint8_t cnt_mr_num);

/**
 * @function gradient_data_parse
 * @brief �¶������ȡ
 * @param data Ŀ�����
 * @param start_post ��·�����ʼλ�ã���֤�¶���ݵ�������
 * @param tele ��ǰ�¶����źŻ�
 * @param gra ����¶���ݵĽṹ
 * @param count �¶���ݼ���
 * @return ���δ��󷵻�-1���ɹ�����0
 **/  
static int gradient_data_parse(char data[16], int *start_post, struct tele_t *tele, struct gradient_t *gra, int *count);

/**
 * @function curve_data_parse
 * @brief ���������ȡ
 * @param data Ŀ�����
 * @param tele ��ǰ�źŻ�
 * @param curve ���������ݵĽṹ
 * @param count ������ݼ���
 * @return ���δ��󷵻�-1���ɹ�����0
 **/ 
static int curve_data_parse(char data[16], struct tele_t *tele, struct curve_t *curve, int *count);


/**
 * @function tunnel_data_parse
 * @brief ��������ȡ
 * @param data Ŀ�����
 * @param tele ��ǰ�źŻ�
 * @param tunnel ��������ݽṹ
 * @param count �����ݼ���
 * @return ���δ��󷵻�-1; �ɹ�����0
 **/ 
static int tunnel_data_parse(char data[16], struct tele_t *tele, struct tunnel_t *tunnel, int *count);

/**
 * @function limit_data_parse
 * @brief ���������ȡ
 * @param data Ŀ�����
 * @param tele ��ǰ�źŻ�
 * @param limit ���������Ϣ�ṹ
 * @param count ������ݼ���
 * @return ���δ��󷵻�-1���ɹ�����0
 **/ 
static int limit_data_parse(char data[16], struct tele_t *tele, struct limit_t *limit, int *count); 

/**
 * @function sidetrack_data_parse
 * @brief ��ȡ�ɵ���Ϣ
 * @param data Ŀ�����
 * @return �ɹ�����sidetrack_t�ṹ�壬ʧ�ܷ���NULL
 **/
static struct sidetrack_t *sidetrack_data_parse(char data[16]);

/**
 * @function save_result
 * @brief �洢��·��ȡ���:�¶�,����,����,��·����
 * @param data Ŀ����ݽṹ
 * @param type �������  0x01:�¶� 0x02:���� 0x03:���� 0x04:��·����
 * @param length ��ݳ���
 * @param path ���·��
 * @param mode 1: �ı���� 0: �����ƴ��
 **/ 
static int save_result(void *data, uint8_t type, int length, const char *path, int mode);

/**
 * @function limit_merge ������Ϣ�ϲ�
 * @brief ���źŻ�����,������ٺͽ�ʾ��Ϣ���ٺϲ�Ϊ�������������
 * @param bd_file ������ļ�,���л�ȡ���ٱ�,���ڽ������ٴ��������ֵ��ת��
 * @param r_list ��·��,������ȡ�źŻ����� 
 * @param limit �����������
 * @param count ������ݼ���,��ʼֵΪ������ٵĸ���
 * @param reve_info ��ʾ��Ϣ
 * @return �ɹ������µ������������飬ʧ�ܷ���NULL
 **/
static struct limit_t *limit_merge(FILE *bd_file, ROUTE_LIST *r_list, 
        struct limit_t *limit, int *count, void *reve_info);

/**
 * @function basedata_parse
 * @brief �ӻ��ļ�����ȡ:�¶ȣ����ߣ����ߣ���������ٴ�ź��źŻ�����
 * @param bd_file Ŀ���ļ�
 * @param r_list �źŻ����?��Ҫ���
 * @param ss_addr ��ʼ��վ��Ϣ
 * @param es_addr ��ֹ��վ��Ϣ
 * @return ��ȡ���
 **/ 
static ROUTE_EXTR_RESULT basedata_parse(FILE *bd_file, ROUTE_LIST *r_list, STATION_ADDR_T *ss_addr, STATION_ADDR_T *es_addr)
{

    uint32_t   cnt_tele_addr, next_tele_addr;
    uint32_t   cnt_file_addr;       /* ָ��ǰ��δ��ȡ��һ�� */
    uint8_t    cnt_dr_num, cnt_mr_num; /* ��ǰ��ݽ�·�ţ���ǰ��ؽ�·��*/
    char       data[16];               /* һ����� */
    
    ROUTE_NODE *index;  /* �źŻ�����ڵ� */
    
    struct station_t        *station_index;         /* ��?ǰ��վ */
    struct tele_t           *cnt_tele, *next_tele;  /* ��ǰ�źŻ���һ���źŻ� */
    struct route_change_t   *rc_index;              /* ��·ת�� */
    struct branch_change_t  *bc_index;              /* ֧��ת�� */
    uint32_t distance;                              /* �źŻ�֮��ľ��� */
    
    struct rt_extrRoadStationTime *train_sch;
    int sta_count;      /* ��ʱ�̱���ĳ�վ��λ */
    
    int count, i;  // for loop
    int ret;

    if (bd_file == NULL)
    {
        fprintf(stderr, "system err: basedata file not exist.\n");
        return FILE_ERROR;
    }

    if (ss_addr == NULL || es_addr == NULL)
    {
        fprintf(stderr, "system err: STATION_ADDR_T not initial.\n");
        return UNKNOWN_ERR;
    }

    if (r_list == NULL)
    {
        fprintf(stderr, "system err: route list not initial.\n");
        return UNKNOWN_ERR; 
    }

    /* ��ȡʱ�̱���Ϣ */
    train_sch = extrRoadStationTime(&ret);
    log_info("extract road station time table success.\n");

    sta_count = 0;
    while (train_sch->ptr[sta_count].station != ss_addr->s_num)
    {
        sta_count ++;
        if (sta_count >= train_sch->ptr_size)
        {
            fprintf(stderr, "train schedule error.\n");     /* ʱ�̱�У����� */
            return TRAIN_SCHEDULE_MISMATCH;
        }
    }

    /****************/
    /* ��ʼ��վ���� */    
    cnt_file_addr = ss_addr->file_addr; /* ��ʼ�ļ�λ�� */
    
    if( fseek(bd_file, cnt_file_addr, SEEK_SET) == -1)
    {
        fprintf(stderr, "file err: failed to fseek.\n");
        return FILE_ERROR;
    }   
    if (fread(data, sizeof(uint8_t),  16, bd_file) == 0)
    {
        fprintf(stderr, "file err: failed to fread.\n");
        return FILE_ERROR;
    }

    cnt_dr_num = ss_addr->dr_num;        /* ��ʼ��ݽ�·�� */
    cnt_mr_num = ss_addr->mr_num;

    station_index = station_data_parse(data, cnt_dr_num, NULL);
    
    if (station_index == NULL)
    {
        fprintf(stderr, "system err: failed to malloc.\n");
        return MALLOC_FAILURE;
    }    

    cnt_file_addr += 0x20;              /* �źŻ��λ���ڳ�վλ�ú����� */    
    cnt_tele_addr = cnt_file_addr;

    if ( (index = (ROUTE_NODE *)malloc( sizeof(ROUTE_NODE) )) == NULL)
    {
        fprintf(stderr, "system err: failed to malloc.\n");
        return MALLOC_FAILURE;
    }
    if (fseek(bd_file, cnt_tele_addr, SEEK_SET) == -1)
    {
        fprintf(stderr, "file err: failed to fseek.\n");
        return FILE_ERROR;
    }

    if (fread(data, sizeof(uint8_t), 16, bd_file) == 0)
    {
        fprintf(stderr, "file err: failed to fread.\n");
        return FILE_ERROR;
    }
    cnt_file_addr += 0x10;
    cnt_tele = tele_data_parse(data, cnt_dr_num, NULL, 0);  /* ��ʼ��վ�źŻ���� */
    station_index->tele_num = cnt_tele->t_num;      /* ��һ����վ�Ĺ�����Գ�վ�źŻ�Ĺ����Ϊ׼ */
    station_index->km_post = cnt_tele->cont_km_post;    
    
    index->node_id = 0;
    index->next = NULL;
    index->addr = cnt_tele_addr;
    index->value.station = station_index;
    index->value.sidetrack = NULL;
    index->value.route_change = NULL;
    index->value.branch_change = NULL;
    station_index = NULL;
    index->value.tele = cnt_tele;
    index->value.type = 0 | 0x03;           /* ����źŻ��״̬����վ�źŻ� */
    
    /* ��ʼ��route_listͷ */
    r_list->head = index;
    r_list->cnt = index;
    r_list->account ++; 
    
    count = 0;
    
    /* ��ʱ���� */
    uint16_t temp;

    /* �¶�,����,����,���߹ɵ���ݽṹ */
    struct gradient_t gradient[MAX_GRA_NUM];
    struct curve_t curve[MAX_CUR_NUM];
    struct tunnel_t tunnel[MAX_TUN_NUM];
    struct limit_t limit[MAX_LIM_NUM];
    struct limit_t *merge_limit;
    struct sidetrack_t *st_index, *st_head, *cnt_st_ptr;    /* ���߹ɵ� */
    int start_post = 0; /* ��ʼ����� */
    int gra_count = 0;  /* �¶ȼ��� */
    int cur_count = 0;  /* ���߼��� */
    int tun_count = 0;
    int lim_count = 0;
    int st_count = 0;

    log_info("begin to extract teleseme data.\n");
    while (1)
    {
        cnt_file_addr = cnt_tele_addr + 0x10;
        distance = 0;
        /* �ҳ������źŻ��λ�ã��ֱ�Ϊcnt_tele_addr��next_tele_addr */
        /* �����·ת�ƣ����cnt_tele_addr��λ����·ת�ƺ�ı��λ�� */
        /* distance��������źŻ�֮��ľ��� */
        do
        {
            if (fread(data, sizeof(uint8_t), 16, bd_file) == 0)
            {
                fprintf(stderr, "file err: failed to fread.\n");
                return FILE_ERROR;
            }
            cnt_file_addr += 0x10;
            
                        
            switch ((uint8_t)data[0])
            {
                
                
                case 0x53:case 0x54:            
                    for( i = 0 ; i < 6 ; i++)   //���������
                    {
                        temp = 0;
                        memcpy( &temp, &data[4+i*2], 2);
                        distance += (temp & 0x03ff) * 5;
                    }    
                break;

                
                case 0xA0:
                    log_info("branch change begin.\n");
                    bc_index = branch_change_data_parse(data);
                    if (bc_index == NULL)
                    {
                        fprintf(stderr, "branch change data parse err.\n");
                        return UNKNOWN_ERR;
                    }
                    ret = branch_confirm_from_ts(bc_index, train_sch, sta_count, bd_file, cnt_file_addr);
                    log_info("branch confirm finish. ret = %d\n", ret);
                    if (ret == -1)
                    {
                        fprintf(stderr, "train schedule err.\n");
                        return TRAIN_SCHEDULE_ERROR;
                    }
                    else if (ret == -2)
                    {
                        fprintf(stderr, "system err: file err.\n");
                        return FILE_ERROR;
                    }

                    /* ����֧��ת�� */
                    if (bc_index->is_change == 1)    
                    {
                        log_info("branch change occur.\n");
                        distance = 0;
                        cnt_file_addr = bc_index->file_addr;
                        if (fseek(bd_file, cnt_file_addr, SEEK_SET) == -1)
                        {
                            fprintf(stderr, "file err: failed to fseek.\n");
                            return FILE_ERROR;
                        }
                        cnt_dr_num = bc_index->des_dr_num;
                        if (bc_index->des_mr_num != 255)
                            cnt_mr_num = bc_index->des_mr_num;
                        cnt_tele_addr = cnt_file_addr;
                        log_info("branch change over.\n");
                    }
                    
                    /* ��ת�ƻ��߲�ȷ�����ص�ԭ��λ��ȥ */
                    else
                    {
                        //ԭ��λ��Ҫ��cnt_file_addr�Ļ���
                        if( fseek(bd_file, cnt_file_addr, SEEK_SET) == -1)
                        {
                            fprintf(stderr, "file err: failed to fseek.\n");
                            return FILE_ERROR;
                        }     
                    }
                    /* ��֧·ת������route_list�� */
                    
                    if (r_list->cnt->value.branch_change == NULL)
                        r_list->cnt->value.branch_change = bc_index;
                    else
                        r_list->cnt->value.branch_change->next = bc_index;
                  
                    r_list->cnt->value.type = r_list->cnt->value.type | 0x08;    
                    
                    log_info("branch change finish.\n");
                break;


                case 0xA1:
                    rc_index = route_change_data_parse(data, cnt_mr_num);
                    /* ����·ת�� */
                    if (rc_index->is_change == 1)    
                    {
                        distance = 0;
                        cnt_file_addr = rc_index->file_addr;
                        if (fseek(bd_file, cnt_file_addr, SEEK_SET) == -1)
                        {
                            fprintf(stderr, "file err: failed to fseek.\n");
                            return FILE_ERROR;
                        }
                        cnt_dr_num = rc_index->des_dr_num;
                        if (rc_index->des_mr_num != 255)
                            cnt_mr_num = rc_index->des_mr_num;
                        cnt_tele_addr = cnt_file_addr;
                    //    cnt_file_addr -= 0x10;                       
                    }
                    /* ����·ת������route_list�� */
                    if (r_list->cnt->value.route_change == NULL)
                        r_list->cnt->value.route_change = rc_index;
                    else
                        r_list->cnt->value.route_change->next = rc_index;
                   
                    r_list->cnt->value.type = r_list->cnt->value.type | 0x04;    
                break;


                default:
                break;
            }
        }while((uint8_t)data[0] != 0x50);    /* ȷ�������źŻ�֮�����·��������·��֧��ת�� */


        next_tele_addr = cnt_file_addr - 0x10;
        
       
   
        /* ���������źŻ�֮������ */
        for (cnt_file_addr = cnt_tele_addr; cnt_file_addr <= next_tele_addr; cnt_file_addr += 0x10)
        {
            if (cnt_file_addr == cnt_tele_addr)
            {
                start_post = cnt_tele->cont_km_post; /* ��ʼ����������źŻ����Ϊ׼ */     
                
                /* ��ת����ǰ�źŻ����һ��λ�� */
                if (fseek(bd_file, (cnt_file_addr+0x10), SEEK_SET) == -1)
                {
                    fprintf(stderr, "file err: failed to fseek.\n");
                    return FILE_ERROR;
                }
                continue;
            }


            if (fread(data, sizeof(uint8_t), 16, bd_file) == 0)
            {
                fprintf(stderr, "file err: failed to fread.\n");
                return FILE_ERROR;
            }


            switch((uint8_t)data[0])
            {
                /* ��ȡ��һ���źŻ� */
                case 0x50:
                    /* ����ǽ��վ�źŻ���ɵ���Ϣ������һ��վ */
                    if (cnt_tele->t_type == 0x10)
                    {
                        if (st_count != 0)
                        {
                            index->value.sidetrack = st_head;
                            st_count = 0;
                        }
                    }

                    next_tele = tele_data_parse(data, cnt_dr_num, cnt_tele, distance);
                    if (next_tele == NULL)
                    {
                        fprintf(stderr, "system err: failed to parse tele data.\n");
                        return MALLOC_FAILURE; 
                    }
                    if ((index = (ROUTE_NODE *)malloc(sizeof(ROUTE_NODE))) == NULL)
                    {
                        fprintf(stderr, "system err: malloc err.\n");
                        return MALLOC_FAILURE;
                    }
                    index->value.type = 0;
                    index->addr = cnt_file_addr;
                    index->node_id = r_list->account;
                    index->next = NULL;
                    index->value.tele = next_tele;
                    index->value.sidetrack = NULL;
                    index->value.route_change = NULL;
                    index->value.branch_change = NULL;
                    index->value.station = NULL;
                    index->value.type = index->value.type | 0x01;
                    
                    if(station_index != NULL)
                    {
                        station_index->tele_num = next_tele->t_num;
                        index->value.station = station_index;
                        station_index = NULL;
                        index->value.type = index->value.type | 0x02;
                        /* ���ɵ���Ϣ��ӽ�route list�� */
                        if (st_count != 0)
                        {
                            index->value.sidetrack = st_head;
                        
                            st_count = 0;
                        }
                    }

                    r_list->cnt->next = index;
                    r_list->cnt = index;
                    (r_list->account) ++;
                    
                    cnt_tele_addr = next_tele_addr;
                    cnt_tele = next_tele;
                break;

                case 0x51:
                    station_index = station_data_parse(data, cnt_dr_num, cnt_tele);
                    sta_count ++;
                    /* ʱ�̱�У��  */
                    if (station_index->s_num != train_sch->ptr[sta_count].station)
                    {
                        fprintf(stderr, "train schedule err.\n");
                        return TRAIN_SCHEDULE_MISMATCH;
                    }
                    if (station_index == NULL)
                    {
                        fprintf(stderr, "system err: malloc err.\n");
                        return MALLOC_FAILURE;
                    }
                break;
                
                case 0x52:
                    st_index = sidetrack_data_parse(data);
                    if (st_index == NULL)
                    {
                        fprintf(stderr, "system err: malloc err.\n");
                        return MALLOC_FAILURE;
                    }
                    if (st_count == 0)
                    {
                        st_head = st_index;
                        cnt_st_ptr = st_index;
                        st_count ++;
                    }
                    else
                    {
                        cnt_st_ptr->next = st_index;
                        cnt_st_ptr = st_index;
                        st_count ++;
                    } 
               
                break;
                
                case 0x53:case 0x54:
                     if(gradient_data_parse(data , &start_post, cnt_tele, gradient, &gra_count) == -1)
                     {
                        fprintf(stderr, "route_extract err: failed to parse gradient data.\n");
                        return EXTR_FAILURE;
                     }                   
                break;
                
                case 0x55:
                     if(limit_data_parse(data, cnt_tele, limit, &lim_count) == -1)
                     {
                         fprintf(stderr, "route_extract err: failed to parse limit data.\n");
                         return EXTR_FAILURE;
                     }
                break;

                case 0x56:
                     if(curve_data_parse(data, cnt_tele, curve, &cur_count) == -1)
                     {
                         fprintf(stderr, "route_extract err: failed to parse curve data.\n");
                         return EXTR_FAILURE;
                     }                    
                break;
                
                case 0x59:
                    if((tunnel_data_parse(data, cnt_tele, tunnel, &tun_count)) == -1)
                    {
                        fprintf(stderr, "route_extract err: failed to parse tunnel data.\n");
                        return EXTR_FAILURE;
                    }
                break;
                default:
                break;
            }           

        }
        if (cnt_file_addr >= es_addr->file_addr)
            break;
    }
    
    if ((merge_limit = limit_merge(bd_file, r_list, limit, &lim_count, NULL)) == NULL)
    {
        log_info("err.\n");
    }

    log_info("begin to save result.\n");
    MAX_TELE=r_list->account;
    create_cm_node(r_list,MAX_TELE);
    save_result(gradient, 0x01, gra_count, "./route_data/gradient", 1);
    save_result(curve, 0x02, cur_count, "./route_data/curve", 1);
    save_result(merge_limit, 0x03, lim_count, "./route_data/limit", 1);
    save_result(r_list, 0x04, r_list->account, "./route_data/route_list", 1);
    save_result(tunnel, 0x05, tun_count, "./route_data/tunnel", 1);
    
    save_result(gradient, 0x01, gra_count, "./route_data/gradient_b", 0);
    save_result(curve, 0x02, cur_count, "./route_data/curve_b", 0);
    save_result(merge_limit, 0x03, lim_count, "./route_data/limit_b", 0);
    save_result(r_list, 0x04, r_list->account, "./route_data/teleseme_b", 0);
    save_result(tunnel, 0x05, tun_count, "./route_data/tunnel_b", 0);
    save_result(r_list, 0x06, r_list->account, "./route_data/station_b", 0);
    
    log_info("data file extract finish.\n");
    return EXTR_SUCCESS;

}


static struct station_t *station_data_parse(char data[16],  uint16_t dr_num, struct tele_t *tele)
{
    uint16_t s_num = 0;
    uint32_t tmis_num = 0;
    uint16_t l_num = 0;
    uint16_t cen_dist = 0;
    struct station_t *index;
    
    if ((index = (struct station_t *)malloc( sizeof(struct station_t)) ) == NULL)
        return NULL;

    memcpy(&cen_dist, &data[2], 2);
    cen_dist = (cen_dist & 0x3ff) * 10;
    memcpy(&s_num, &data[4], 2);
    memcpy(&tmis_num, &data[6], 3);
    memcpy(&l_num, &data[9], 2);
   
    index->s_num = s_num; 
    index->tmis_num = tmis_num;
    index->l_num = l_num;
    index->dr_num = dr_num;
    index->cen_dist = cen_dist;

    if (tele != NULL)   //�ǵ�һ����վ
    {
        index->km_post = tele->cont_km_post + cen_dist;
    }
    return index; 
}


static struct tele_t *tele_data_parse(char data[16], uint16_t dr_num, struct tele_t *pre_tele, uint32_t distance)
{
   uint32_t buf;
   uint16_t t_num;
   uint16_t speed_code;
   uint8_t  t_type;
   uint8_t  t_trend;
   int32_t  cont_km_post;
   int32_t  discont_km_post;
   
   struct tele_t *index;

   if ( (index = (struct tele_t *)malloc(sizeof(struct tele_t))) == NULL)
       return NULL;

   memcpy(&t_type, &data[1], 1);
   memcpy(&speed_code, &data[2], 2);
   memcpy(&t_num, &data[4], 2);
   memcpy(&buf, &data[12], 4);
  
   t_trend = (buf>>23) & 0x01;
   if ((buf & 0x80000000) == 0)
       discont_km_post = buf & 0x7FFFFF;
   else 
       discont_km_post = 0 - (buf & 0x7FFFFF);
   
   if (pre_tele == NULL) 
   {
       cont_km_post = discont_km_post;
   }
   else
   {
       
       pre_tele->distance = distance;       //ǰ���źŻ�ľ���
       cont_km_post = pre_tele->cont_km_post + distance;
   }

   index->dr_num = dr_num;
   index->t_num = t_num;
   index->t_trend = t_trend;
   index->t_type = t_type;
   index->discont_km_post = discont_km_post;
   index->cont_km_post = cont_km_post;
   index->sp_limit_code = speed_code; 
   return index; 
}


static struct branch_change_t *branch_change_data_parse(char data[16])
{
   struct branch_change_t *index;
   if ((index = (struct branch_change_t *)malloc(sizeof(struct branch_change_t))) == NULL)
   {
       return NULL;
   }
   index->bc_num = data[1];
   index->sign_1 = data[2];
   index->sign_2 = data[3];

   memcpy(&(index->file_addr), &data[4], 4);
   index->des_dr_num = data[8];
   index->des_mr_num = data[9];
   index->start_dr_num = data[10];
   memcpy(&(index->l_num), &data[13],2);
   index->work_info = data[13];
   index->next = NULL;

   return index;
}

static int branch_confirm_from_ts(struct branch_change_t *branch_change, 
                                  struct rt_extrRoadStationTime *train_sch, int count,
                                    FILE *bd_file, uint32_t cnt_file_addr)
{
   char     data[16]; 
   uint16_t next_s_num;
   uint16_t s_num_1, s_num_2;   /*s_num_1 ָδ����֧��ת�Ƶ���һ����վ�� 
                                 *s_num_2 ָ����֧��ת�Ƶ���һ����վ��
                                 *next_s_num ָʱ�̱��ϵ���һ����վ
                                 */
   
   if (train_sch == NULL || count < 0)
      return -1;
 
   next_s_num = train_sch->ptr[count+1].station; 
   
   if( fseek(bd_file, cnt_file_addr, SEEK_SET) == -1)
   {
       fprintf(stderr, "file err: failed to fseek.\n");
       return -2;
   } 
  
   do
   {        
        if (fread(data, sizeof(uint8_t),  16, bd_file) == 0)
        {
           fprintf(stderr, "file err: failed to fread.\n");
           return -2;
        }
        
   }while((uint8_t)data[0] != 0x51);    /*ֱ��������һ����վ*/ 

   memcpy(&s_num_1, &data[4], 2);

   if( fseek(bd_file, branch_change->file_addr, SEEK_SET) == -1)
   {
       fprintf(stderr, "file err: failed to fseek.\n");
       return -2;
   } 
  
   do
   {    
        if (fread(data, sizeof(uint8_t),  16, bd_file) == 0)
        {
           fprintf(stderr, "file err: failed to fread.\n");
           return -2;
        }
   }while((uint8_t)data[0] != 0x51);    /*ֱ��������һ����վ*/ 

   memcpy(&s_num_2, &data[4], 2);

   if (s_num_1 == next_s_num)
   {
       branch_change->is_change = 0;
   }
   else if (s_num_2 == next_s_num)
   {
       branch_change->is_change = 1;
   }
   else
   {
       branch_change->is_change = -1;
   }
  
   return 0;

}

static struct route_change_t *route_change_data_parse(char data[16], uint8_t cnt_mr_num)
{
    uint8_t  start_mr_num;
    uint8_t  des_mr_num;
    uint8_t  des_dr_num;
    uint8_t  is_change;
    uint16_t cot_train_num; 
    uint16_t l_num;
    uint32_t file_addr;

    struct route_change_t *index;

    memcpy(&cot_train_num, &data[2], 2);
    memcpy(&file_addr, &data[4], 4);
    memcpy(&des_dr_num, &data[8], 1);
    memcpy(&des_mr_num, &data[9], 1);
    memcpy(&start_mr_num, &data[10], 1);
    memcpy(&l_num, &data[11], 2);

    if (start_mr_num == 255)
        is_change = 1;
    else if(cnt_mr_num == start_mr_num)
        is_change = 1;
    else
        is_change = 0;
    
    if ((index = (struct route_change_t *)malloc(sizeof(struct route_change_t))) == NULL ) 
    {
        return NULL;
    }
    index->start_mr_num = start_mr_num;
    index->des_mr_num = des_mr_num;
    index->des_dr_num = des_dr_num;
    index->file_addr = file_addr;
    index->cot_train_num = cot_train_num;
    index->l_num = l_num;
    index->is_change = is_change;
    index->next = NULL;

    return index;
}

static int gradient_data_parse(char data[16], int *start_post, struct tele_t *tele,struct gradient_t *gra, int *count)
{
    int i;
    uint8_t if_ascent[6];   /* 1:���� 0:���� */
    uint8_t decimal[6] = {};
    uint8_t inter[6];  
    uint16_t length[6];

    uint16_t buf_1 = 0;
    uint32_t buf_2 = 0;    
    
    if (gra == NULL || count == NULL || data == NULL || tele == NULL)
        return -1;
   
    memcpy(&buf_2, &data[1], 3);
    for (i = 0 ; i < 6 ; i++)
    {
        decimal[i] = ( buf_2>>(i * 4) ) & 0x0f;
        memcpy(&buf_1, &data[4+2*i], 2);
        if_ascent[i] = buf_1>>15;    
        inter[i] = (buf_1>>10) & 0x1f;
        length[i] = (buf_1 & 0x3ff) * 5;
       
        if (length[i] != 0)
        {
            if (i == 0)
            {
                gra[*count].km_post = *start_post;                       /* ������� */
                gra[*count].cross_dis = *start_post - tele->cont_km_post;/* Խ����� = ��ǰ�����-�źŻ��������  */
            }
            else
            {            
                gra[*count].km_post = gra[(*count)-1].km_post + length[i-1]; 
                gra[*count].cross_dis = gra[(*count)-1].cross_dis + length[i-1];
            }
            
            gra[*count].tele_num = tele->t_num;
            gra[*count].value = inter[i] + decimal[i] * 0.1;
            
            if (if_ascent[i] == 1)
                gra[*count].value = 0 - gra[*count].value;
            
            gra[*count].length = length[i];
            (*start_post) += gra[*count].length;
            (*count) ++;
            if ((*count) >= MAX_GRA_NUM)
            {
                return -2;
            }
        }
        else
           break; 
    }
    return 0;
    
}


static int curve_data_parse(char data[16], struct tele_t *tele, struct curve_t *curve, int *count)
{
    uint16_t crs_dis;
    uint32_t length;
    uint16_t radius;

    if (data == NULL || tele == NULL || curve == NULL || count == NULL)
        return -1;
    
    memcpy(&crs_dis, &data[1], 2);
    memcpy(&radius, &data[3], 2);
    memcpy(&length, &data[7], 4);
    
    curve[*count].tele_num = tele->t_num;
    curve[*count].cross_dis = crs_dis;
    curve[*count].length = length; 
    curve[*count].km_post = tele->cont_km_post + crs_dis;
    curve[*count].dir = ((radius>>15)==0) ? -1 : 1;
    curve[*count].value = radius & 0x7FFF;
    (*count)++;

    if ((*count) >= MAX_CUR_NUM)
    {
        return -2;
    }
    return 0;
}


static int tunnel_data_parse(char data[16], struct tele_t *tele, struct tunnel_t *tunnel, int *count)
{
    uint16_t cross_dist;
    uint32_t index;
    int32_t s_post;
    int32_t e_post;

    if (data == NULL || tele == NULL || tunnel == NULL || count == NULL)
        return -1;
    
    memcpy(&cross_dist, &data[1], 2);
    memcpy(&index, &data[3], 4);
    if ((index & 0x80000000) != 0)
        s_post = 0 - (index & 0x7fffff);
    else
        s_post = index & 0x7fffff;
    memcpy(&index, &data[7], 4);
    if ((index & 0x80000000) != 0)
        e_post = 0 - (index & 0x7fffff);
    else
        e_post = index & 0x7fffff;

    tunnel[*count].tele_num = tele->t_num;
    tunnel[*count].km_post = tele->cont_km_post + cross_dist;
    tunnel[*count].length = abs(e_post - s_post);
    tunnel[*count].cross_dist = cross_dist;
    (*count) ++;
    
    return 0;
}


static struct sidetrack_t *sidetrack_data_parse(char data[16])
{
    uint8_t     st_num;
    uint16_t    in_dist;
    uint16_t    out_dist;
    uint16_t    in_code;
    uint16_t    out_code;
    uint16_t    adjust_dist;

    struct sidetrack_t *sidetrack;

    if ((sidetrack = (struct sidetrack_t *)malloc(sizeof(struct sidetrack_t))) == NULL)
    {
        return NULL;
    }
    memcpy(&st_num, &data[1], 1);
    memcpy(&in_dist, &data[2], 2);
    memcpy(&out_dist, &data[4], 2);
    memcpy(&in_code, &data[6], 2);
    memcpy(&out_code, &data[8], 2);
    memcpy(&adjust_dist, &data[10], 2); 

    
    sidetrack->st_num = st_num;   
    sidetrack->in_dist= in_dist;   
    sidetrack->out_dist = out_dist;   
    sidetrack->in_limit_code = in_code;   
    sidetrack->out_limit_code = out_code;   
    sidetrack->adjust_dist =  adjust_dist - 0xffff; //����   
    sidetrack->next = NULL;
    return sidetrack;
}

static int limit_data_parse(char data[16], struct tele_t *tele, struct limit_t *limit, int *count)
{
    uint16_t crs_dist;
    uint16_t subregion_dist;
    uint16_t code;
    uint32_t length;

    if (data == NULL || tele == NULL || limit == NULL || count == NULL)
        return -1;

    memcpy(&crs_dist, &data[1], 2);
    memcpy(&subregion_dist, &data[3], 2);
    memcpy(&code, &data[5], 2);
    memcpy(&length, &data[7], 4);

    limit[*count].tele_num = tele->t_num;
    limit[*count].cross_dis = crs_dist;
    limit[*count].subregion_dis = subregion_dist;
    limit[*count].code = code;
    limit[*count].length = length;
    limit[*count].km_post = tele->cont_km_post + crs_dist;

    (*count)++;

    return 0;
}

static uint16_t (*limit_code_table_extr(FILE *bd_file))[8]
{
    uint16_t  (*index)[8];
    uint32_t start_post;
    uint32_t code_num;
    if (bd_file == NULL)
        return NULL;
    
    fseek(bd_file, 0x14, SEEK_SET);
    fread(&start_post, 4, 1, bd_file);
    fread(&code_num, 4, 1, bd_file);
    
    if ((index = (uint16_t (*)[8])malloc(sizeof(uint16_t) * 8 * code_num)) == NULL)
    {
        return NULL;
    }

    fseek(bd_file, start_post, SEEK_SET);
    
    fread(index, 2, code_num * 8, bd_file);
    return index; 
}

static struct limit_t *limit_merge(FILE *bd_file, ROUTE_LIST *r_list, struct limit_t *limit, int *count, void *reve_info)
{
    uint16_t (*limit_code_table)[8];    //���ٴ�ű� 
    struct limit_t *index;              //��������
    struct limit_t temp[MAX_LIM_NUM];   //����źŻ���������
    uint32_t cnt_node_num;
    ROUTE_NODE *cnt_node;

    uint16_t code;
     
    int temp_count;
    int i, j, k;  //for loop

    if (bd_file == NULL)
    {
        fprintf(stderr, "file err: base data file err.\n");
        return NULL;
    }
    else if (r_list == NULL) 
    {
        fprintf(stderr, "route list is NULL\n");
        return NULL;
    }
    else if (limit == NULL)
    {
        fprintf(stderr, "limit is NULL\n");
        return NULL;
    }
    else if (reve_info == NULL)
    {
        fprintf(stderr, "revesation info is NULL\n");
    }
    
    limit_code_table = limit_code_table_extr(bd_file);
    if (limit_code_table == NULL)
    {
        fprintf(stderr, "system err: malloc err.\n");
        return NULL;
    }
    
    if ((index = (struct limit_t *)malloc(sizeof(struct limit_t) * MAX_LIM_NUM)) == NULL)
    {
        fprintf(stderr, "system err: malloc err.\n");
        return NULL;
    }
    
    temp_count = 0;
    cnt_node = r_list->head;
    cnt_node_num = 0;

    /* �źŻ�������� */
    for (i = 0 ; i < (int32_t)r_list->account; i++)
    {
        temp[i].tele_num = cnt_node->value.tele->t_num;
        temp[i].km_post = cnt_node->value.tele->cont_km_post;
        temp[i].length = cnt_node->value.tele->distance;
        code = cnt_node->value.tele->sp_limit_code;
        temp[i].value = limit_code_table[code][0];
        temp[i].code = code;
        temp[i].end_post = temp[i].km_post + temp[i].length;
        //��һ���źŻ�
        cnt_node = cnt_node->next;
    }
    temp_count = r_list->account;

    /* ���������� */
    for (i = 0 ; i < *count; i++)
    {   
        code = limit[i].code;
        limit[i].value = limit_code_table[code][0];
        limit[i].end_post = limit[i].km_post + limit[i].length;
    }

    i = 0;  //�źŻ����ټ���
    j = 0;  //������ټ���
    k = 0;  //�ϲ�����

    index[0].km_post = temp[0].km_post;

    while (i < temp_count)
    {
        if (limit[j].tele_num == temp[i].tele_num)
        {
            while(limit[j].tele_num == temp[i].tele_num)
            {
                if( limit[j].subregion_dis > temp[i].length - limit[j].cross_dis)
                {
                    limit[j].subregion_dis = temp[i].length - limit[j].cross_dis;
                }

                if (limit[j].cross_dis == 0)
                {
                    index[k].km_post = temp[i].km_post;
                    index[k].length = limit[j].subregion_dis;
                    index[k].end_post = index[k].km_post + index[k].length;
                    index[k].value = (limit[j].value > temp[i].value)?temp[i].value:limit[j].value;
                    k++;
                    
                }
                else
                {
                    if (k == 0)                    
                        index[k].km_post = temp[i].km_post;
                    else
                        index[k].km_post = index[k-1].end_post;
                    index[k].length = temp[i].km_post + limit[j].cross_dis - index[k].km_post;
                    index[k].end_post = index[k].km_post + index[k].length;
                    index[k].value = temp[i].value;
                    k++;

                    index[k].km_post = index[k-1].end_post;
                    index[k].length = limit[j].subregion_dis;
                    index[k].end_post = index[k].km_post + index[k].length;
                    index[k].value = (limit[j].value > temp[i].value)?temp[i].value:limit[j].value;
                    k++;
                }

                j++;
            }
            if (index[k-1].end_post < temp[i].end_post)
            {
                index[k].km_post = index[k-1].end_post;
                index[k].end_post = temp[i].end_post;
                index[k].length = index[k].end_post - index[k].km_post;
                index[k].value = temp[i].value;
                k++;
            }
        }
        else
        {
            index[k].km_post = temp[i].km_post;
            index[k].length = temp[i].length;
            index[k].value = temp[i].value;
            index[k].end_post = temp[i].end_post;
            k++;
        }
        i++;
    }
    *count = k;

    for (i = 1; i < *count; i++)
    {
        index[i].km_post = index[i-1].end_post;
    }
#if 0
    int flag = 0;
    for (i = 0 ; i < *count; i++)
    {
        printf("��ʼ�����: %d ��ֹ�����: %d ����ֵ: %d\n", index[i].km_post, index[i].end_post, index[i].value);
        if (i > 0)
        {
            if (index[i].km_post != index[i-1].end_post)
            {
                printf("index i : %d\n", i);
                flag = -1;
            }
        }
    } 
    printf("flag : %d\n", flag);
#endif
    return index;
}
        

static int save_result(void *data, uint8_t type, int length, const char *path, int mode)
{
    struct gradient_t *gra;
    struct curve_t *cur;
    struct limit_t *lim;
    struct sidetrack_t *st;
    struct tunnel_t *tun;
    ROUTE_LIST *list;
    ROUTE_NODE *index;
    FILE *data_file;
    int i;  //for loop

    if (data == NULL || length == 0 || path == NULL)
        return -1;
    
    if ((data_file = fopen(path, "w")) == NULL)
        return -1;
    
    if (mode == 1)
    {
        switch (type)
        {
            case 0x01:
                gra = (struct gradient_t *)data;
                fprintf(data_file, "�źŻ�  Խ�����    �������  ����    �¶�ֵ\n");
                for(i = 0 ; i < length; i++)
                {
                    fprintf(data_file, "%-d         %-d         %-d         %-d         %-f\n",
                            gra[i].tele_num, gra[i].cross_dis, gra[i].km_post, gra[i].length, gra[i].value);
                }
                break;

            case 0x02:
                cur = (struct curve_t *)data;
                fprintf(data_file, "�źŻ�   Խ�����    �������  ����    ����ֵ  ���߷���\n");
                for(i = 0; i < length; i++)
                {
                    fprintf(data_file, "%-d     %-d     %-d     %-d     %-d     %-d\n",
                            cur[i].tele_num, cur[i].cross_dis, cur[i].km_post, cur[i].length, cur[i].value, cur[i].dir);
                }
            break;

            case 0x03:
                lim = (struct limit_t *)data;
                fprintf(data_file, "��ʼ�����    ��ֹ�����    ����ֵ\n");
                for(i = 0; i < length; i++)
                {
                    fprintf(data_file, "%d     %d     %d\n", lim[i].km_post, lim[i].end_post, lim[i].value);
                }
                break;

            case 0x04:
                list = (ROUTE_LIST *)data;
                index = list->head;
                fprintf(data_file, "�źŻ���  ��·��  ���ٴ��    ԭʼ�����  ������� ����\n");
                for(i = 0; i < length; i++)
                {
                    fprintf(data_file, "\n%d  %d  %d  %d  %d  %d\n", 
                            index->value.tele->t_num, index->value.tele->dr_num,
                            index->value.tele->sp_limit_code, index->value.tele->discont_km_post,
                            index->value.tele->cont_km_post, index->value.tele->distance);
                    if((index->value.type & 0x02))
                    {
                        fprintf(data_file, "��վ�źŻ�:%d ��վ��:%d �����:%d\n", index->value.station->tele_num, index->value.station->s_num, index->value.station->km_post); 
                        
                        st = index->value.sidetrack;
                      
                        while(st != NULL)
                        {
                            fprintf(data_file, "�ɵ���:%d �������:%d ��������:%d �������:%d\n", 
                                    st->st_num, st->in_limit_code, 
                                    st->out_limit_code, st->adjust_dist);
                            st = st->next;
                        }
                    }
                    index = index->next;
                }
                break;
            case 0x05:
                tun = (struct tunnel_t *)data;
                fprintf(data_file, "�źŻ�  Խ�����    ��ʼ�����  ����\n");
                for(i = 0; i < length; i++)
                {
                    fprintf(data_file, "%d  %d  %d  %d\n", tun[i].tele_num, tun[i].cross_dist, tun[i].km_post, tun[i].length);
                }                
                break;
            default:
                fclose(data_file);
                return -1;
        }
    }
    else
    {
        switch (type)
        {
            case 0x01:
                gra = (struct gradient_t *)data;
                fwrite(gra, sizeof(struct gradient_t) ,length, data_file);
                break;
            case 0x02:
                cur = (struct curve_t *)data;
                fwrite(cur, sizeof(struct curve_t), length, data_file);
                break;

            case 0x03:
                lim = (struct limit_t *)data;
                fwrite(lim, sizeof(struct limit_t), length, data_file);
                break;

            case 0x04:
                list = (ROUTE_LIST *)data;
                index = list->head;
                for (i = 0 ; i < length ; i++)
                {
                    fwrite(index->value.tele, sizeof(struct tele_t), 1, data_file);
                    index = index->next;
                }
                break;
            case 0x05:
                tun = (struct tunnel_t *)data;
                fwrite(tun, sizeof(struct tunnel_t), length, data_file);
                break;
            case 0x06:
                list = (ROUTE_LIST *)data;
                index = list->head;
                for (i = 0 ; i < length ; i++)
                {
                    if ((index->value.type & 0x02) != 0)
                    {
                        fwrite(index->value.station, sizeof(struct station_t), 1, data_file);
                    }
                    index = index->next;
                }
                break;
            default:
                fclose(data_file);
                return -1;
        }
    }

    fclose(data_file);
    return 0; 
        
}
static int create_cm_node(void *data,int length){
			ROUTE_LIST *list;
			ROUTE_NODE *index;
			FILE *data_file;
			data_file = fopen("./route_data/convert_mile", "w");
			if (data_file == NULL) {
				log_error("data_file creat error\n");
			}
			int i;
			list = (ROUTE_LIST *) data;
			index = list->head;
			cm_node=(CONVERT_MILE_NODE*)malloc(sizeof(CONVERT_MILE_NODE)*length);
			fprintf(data_file, "���� �źŻ���    ԭʼ�����  ������� ����\n");
			for (i = 0; i < length; i++) {
				fprintf(data_file, "%d  %d  %d  %d  %d\n",i,
						index->value.tele->t_num,
						index->value.tele->discont_km_post,
						index->value.tele->cont_km_post,
						index->value.tele->distance);
		       cm_node[i].t_num=index->value.tele->t_num;
		       cm_node[i].discont_km_post=index->value.tele->discont_km_post;
		       cm_node[i].cont_km_post=index->value.tele->cont_km_post;
		       cm_node[i].distance=index->value.tele->distance;
				index = index->next;
			}
			fclose(data_file);
		}
 int convert_seq_mile(int16_t telenum,int16_t distance){
	 int i;
	 int seq=0;
	 if(cm_node==NULL){
		 log_warning("tele find err max_tele :%d tele: %d \n",MAX_TELE,telenum);
		 return 0;
	 }
	 for(i=0;i<MAX_TELE;i++){
		 if(cm_node[i].t_num==telenum){
			 seq=cm_node[i].cont_km_post-distance;
		 }
	 }
	 if(seq==0){
		 log_warning("tele find err max_tele :%d tele: %d \n",MAX_TELE,telenum);
		 return 0;
	 }
	 return seq;
 }
