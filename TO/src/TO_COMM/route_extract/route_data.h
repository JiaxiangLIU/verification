/*
 * @file route_data.h
 * @brief �����˽�·��·�����г̱�ʶ������·��ȡ�����ṹ�����ݽṹ
 * @author danny
 * @date 2014/12/16
 */ 


#ifndef ROUTE_DATA_H_
#define ROUTE_DATA_H_

#include <stdint.h>
#include "route_data.h"

typedef enum route_extr_result
{
    EXTR_SUCCESS = 0,
    EXTR_FAILURE = -1,
    FILE_ERROR = 31,
    TRAIN_SCHEDULE_MISMATCH = 81,
    TRAIN_SCHEDULE_ERROR = 82,
    SS_NOT_FOUND = 61,      //��ʼ��վδ�ҵ�
    ES_NOT_FOUND = 62,      //��ֹ��վδ�ҵ�
    MALLOC_FAILURE = 71,    //�ڴ�������
    UNKNOWN_ERR = 100
}ROUTE_EXTR_RESULT;



/* ���г̱�ʶ�� */
struct route_id_t
{
    uint8_t     s_mr_num;       /**<monitor routing number*/
    uint8_t     s_dr_num;       /**<data routing number*/
    uint8_t     s_up_or_down;   /**<1 : up; 0 : down*/
    uint16_t    ss_num;         /**<start station number*/

    uint8_t     e_mr_num;
    uint8_t     e_dr_num;
    uint8_t     e_up_or_down;
    uint16_t    es_num;         /**<end station number*/
};

/* ���������
typedef enum km_post_trend
{
    TREND_INCREASE = 1,
    TREND_DECREASE = 0
}KM_POST_TREND;
*/
/* �źŻ�����
typedef enum tele_type       
{
    IN_AND_OUT = 0X10,
    OUT = 0X20,
    IN = 0X30,
    PASS = 0X40,
    IN_ADVANCE = 0X50,
    ADMIT = 0X60,
    CUT = 0X70,
    IN_ADVANCE_FIR = 0X90,
    IN_ADVANCE_SEC = 0XA0
}TELE_TYPE;
*/

/* �źŻ� */
struct tele_t
{
    uint8_t  t_type;            /* �źŻ����� */
    uint8_t  t_trend;           /* �źŻ����� */
    uint16_t t_num;             /* �źŻ���� */
    uint16_t dr_num;            /* ��·�� */
    uint16_t sp_limit_code;     /* speed limit code ���ٴ��� */
    uint32_t distance;          /* ������źŻ����� */
    int32_t  discont_km_post;   /* ԭʼ�źŻ������ */
    int32_t  cont_km_post;      /* �����źŻ������ */
    
};

/* �ɵ���Ϣ */ 
struct sidetrack_t
{
    uint8_t  st_num;            /* �ɵ��� */
    uint16_t in_dist;           /* ������� */
    uint16_t out_dist;          /* ������� */
    uint16_t in_limit_code;     /* �������ٴ��� */
    uint16_t out_limit_code;    /* �������ٴ��� */
    int16_t  adjust_dist;       /* �������� */
    struct sidetrack_t *next;
};



/* ��վ */
struct station_t
{
    uint16_t s_num;                 /* station number */
    uint16_t tele_num;              /* ��վ�źŻ���� */
    uint16_t dr_num;                /* data route number */
    uint32_t tmis_num;              /* tmis number */
    uint16_t l_num;                 /* line number */
    uint16_t cen_dist;              /* վ���ľ��� */
    uint32_t km_post;
};

/* ��·ת�� */
struct route_change_t
{
    uint32_t file_addr;
    uint32_t rc_num;                /* ��·ת�Ʊ�ţ��Զ��� */
    uint8_t  des_dr_num;     
    uint8_t  is_change;             /* �ж��Ƿ����ת�� 1: changed ; 0: not changed */
    uint8_t  des_mr_num;
    uint8_t  start_mr_num;
    uint16_t cot_train_num;         /* cot_train_num:�������� */
    uint16_t l_num;                 /* line number */
    struct route_change_t *next;    /* ͬһ�źŻ��ڵĶ��ת�� */
};

/* ֧��ת�� */
struct branch_change_t
{
    uint8_t bc_num;                /* ֧��ת�Ʊ�ţ��Զ��� */
    uint32_t file_addr;
    uint8_t  is_change;            /* 1��ת�ƣ� 0����ת�� -1����ȷ��*/
    uint8_t  des_dr_num;
    uint8_t  des_mr_num;
    uint8_t  start_dr_num;
    uint8_t  default_branch_num;
    uint8_t  sign_1;
    uint8_t  sign_2;
    uint16_t l_num;
    uint8_t  work_info;
    struct branch_change_t *next;   /* ͬһ�źŻ��¿����ж��ת�� */
};


typedef struct node_value_t
{
   uint8_t                  type;   /*type ����ýڵ���е����ͣ�0001��tele 0010��station 
                                      0100��route_change 1000��branch_change,�����ͻ������tele��*/
   struct tele_t            *tele;
   struct station_t         *station;
   struct sidetrack_t       *sidetrack;
   struct route_change_t    *route_change;
   struct branch_change_t   *branch_change;
}node_value_t;


typedef struct route_node
{   
    uint32_t node_id;
    uint32_t addr;   /*<addr: ��ԭʼbasedata�ļ��е�λ��*/    
    node_value_t    value;      
    struct route_node   *next;
}ROUTE_NODE;

/* ��·��·�� */
typedef struct route_list
{
    uint32_t account;
    ROUTE_NODE *head;
    ROUTE_NODE *cnt;
}ROUTE_LIST;

/* ��basedata����ȡ��·���ݣ�һ����·�������ڵ��¶�,����,������������ */
#define MAX_GRA_NUM 2000
#define MAX_CUR_NUM 800
#define MAX_LIM_NUM 500
#define MAX_TUN_NUM 50
/* ��base_data�ļ�����ȡ���¶ȣ����ߣ�������Ϣ */
struct gradient_t
{
    uint16_t tele_num;  /* �����źŻ���� */
    uint16_t length;    /* ���� */
    uint32_t cross_dis; /* Խ������ */
    int32_t  km_post;   /* ��������� */
    float value;        /* �¶�ֵ +:����; -:���� */
};

struct curve_t
{
    uint16_t tele_num;
    uint32_t length;
    uint32_t cross_dis; /* Խ������ */
    int32_t  km_post;   /* ��ʼ����� */
    uint16_t value;     /* ����ֵ */
    int dir;            /* ���߷��� 1:�� -1:�� */
};

/* ֻ����base_data�е�������Ϣ��������:�źŻ�����,���ߵ�����,��ʾ��Ϣ���� */
struct limit_t
{
    uint16_t tele_num;
    uint16_t cross_dis;
    uint16_t subregion_dis;
    uint16_t code;
    uint16_t value;
    uint32_t length;
    int32_t  km_post;   /* ��ʼ����� */
    int32_t  end_post;  /* ��ֹ����� */
};

struct tunnel_t
{
    uint16_t tele_num;
    uint16_t cross_dist;
    uint32_t km_post;
    uint32_t length;
};
typedef struct convert_mile_node {
    int16_t t_num;             /* �źŻ���� */
    int32_t  discont_km_post;   /* ԭʼ�źŻ������ */
    int32_t  cont_km_post;      /* �����źŻ������ */
    int16_t distance;          /* ������źŻ����� */
}CONVERT_MILE_NODE;
int convert_seq_mile(int16_t telenum,int16_t distance);
/**
 * @function basedata_file_extr
 * @brief ��basedata�����ļ�����ȡ
 * @param r_id ���г̱�ʶ��
 * @param r_list �г�������ȡ�ɹ���������г�����
 * @return ������ȡ�Ľ�����ɹ�����EXTR_SUCCESS
 **/
ROUTE_EXTR_RESULT basedata_file_extr(struct route_id_t  r_id, ROUTE_LIST    *r_list);
#endif
