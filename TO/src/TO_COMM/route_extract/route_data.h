/*
 * @file route_data.h
 * @brief 定义了交路链路表，新行程标识符，交路提取操作结构的数据结构
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
    SS_NOT_FOUND = 61,      //起始车站未找到
    ES_NOT_FOUND = 62,      //终止车站未找到
    MALLOC_FAILURE = 71,    //内存分配错误
    UNKNOWN_ERR = 100
}ROUTE_EXTR_RESULT;



/* 新行程标识符 */
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

/* 公里标趋势
typedef enum km_post_trend
{
    TREND_INCREASE = 1,
    TREND_DECREASE = 0
}KM_POST_TREND;
*/
/* 信号机类型
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

/* 信号机 */
struct tele_t
{
    uint8_t  t_type;            /* 信号机类型 */
    uint8_t  t_trend;           /* 信号机趋势 */
    uint16_t t_num;             /* 信号机编号 */
    uint16_t dr_num;            /* 交路号 */
    uint16_t sp_limit_code;     /* speed limit code 限速代号 */
    uint32_t distance;          /* 距离后方信号机距离 */
    int32_t  discont_km_post;   /* 原始信号机公里标 */
    int32_t  cont_km_post;      /* 连续信号机公里标 */
    
};

/* 股道信息 */ 
struct sidetrack_t
{
    uint8_t  st_num;            /* 股道号 */
    uint16_t in_dist;           /* 进岔距离 */
    uint16_t out_dist;          /* 出岔距离 */
    uint16_t in_limit_code;     /* 进岔限速代号 */
    uint16_t out_limit_code;    /* 出岔限速代号 */
    int16_t  adjust_dist;       /* 修正距离 */
    struct sidetrack_t *next;
};



/* 车站 */
struct station_t
{
    uint16_t s_num;                 /* station number */
    uint16_t tele_num;              /* 出站信号机编号 */
    uint16_t dr_num;                /* data route number */
    uint32_t tmis_num;              /* tmis number */
    uint16_t l_num;                 /* line number */
    uint16_t cen_dist;              /* 站中心距离 */
    uint32_t km_post;
};

/* 交路转移 */
struct route_change_t
{
    uint32_t file_addr;
    uint32_t rc_num;                /* 交路转移编号，自定义 */
    uint8_t  des_dr_num;     
    uint8_t  is_change;             /* 判断是否进行转移 1: changed ; 0: not changed */
    uint8_t  des_mr_num;
    uint8_t  start_mr_num;
    uint16_t cot_train_num;         /* cot_train_num:修正车次 */
    uint16_t l_num;                 /* line number */
    struct route_change_t *next;    /* 同一信号机内的多个转移 */
};

/* 支线转移 */
struct branch_change_t
{
    uint8_t bc_num;                /* 支线转移编号，自定义 */
    uint32_t file_addr;
    uint8_t  is_change;            /* 1：转移； 0：不转移 -1：不确定*/
    uint8_t  des_dr_num;
    uint8_t  des_mr_num;
    uint8_t  start_dr_num;
    uint8_t  default_branch_num;
    uint8_t  sign_1;
    uint8_t  sign_2;
    uint16_t l_num;
    uint8_t  work_info;
    struct branch_change_t *next;   /* 同一信号机下可能有多个转移 */
};


typedef struct node_value_t
{
   uint8_t                  type;   /*type 代表该节点具有的类型：0001：tele 0010：station 
                                      0100：route_change 1000：branch_change,公里标突变融入tele中*/
   struct tele_t            *tele;
   struct station_t         *station;
   struct sidetrack_t       *sidetrack;
   struct route_change_t    *route_change;
   struct branch_change_t   *branch_change;
}node_value_t;


typedef struct route_node
{   
    uint32_t node_id;
    uint32_t addr;   /*<addr: 在原始basedata文件中的位置*/    
    node_value_t    value;      
    struct route_node   *next;
}ROUTE_NODE;

/* 交路链路表 */
typedef struct route_list
{
    uint32_t account;
    ROUTE_NODE *head;
    ROUTE_NODE *cnt;
}ROUTE_LIST;

/* 从basedata中提取线路数据，一条线路上最多存在的坡度,曲线,限速数据数量 */
#define MAX_GRA_NUM 2000
#define MAX_CUR_NUM 800
#define MAX_LIM_NUM 500
#define MAX_TUN_NUM 50
/* 从base_data文件中提取的坡度，曲线，限速信息 */
struct gradient_t
{
    uint16_t tele_num;  /* 所属信号机编号 */
    uint16_t length;    /* 长度 */
    uint32_t cross_dis; /* 越过距离 */
    int32_t  km_post;   /* 连续公里标 */
    float value;        /* 坡度值 +:上坡; -:下坡 */
};

struct curve_t
{
    uint16_t tele_num;
    uint32_t length;
    uint32_t cross_dis; /* 越过距离 */
    int32_t  km_post;   /* 起始公里标 */
    uint16_t value;     /* 曲线值 */
    int dir;            /* 曲线方向 1:左 -1:右 */
};

/* 只包含base_data中的限速信息，不包括:信号机限速,侧线点限速,揭示信息限速 */
struct limit_t
{
    uint16_t tele_num;
    uint16_t cross_dis;
    uint16_t subregion_dis;
    uint16_t code;
    uint16_t value;
    uint32_t length;
    int32_t  km_post;   /* 起始公里标 */
    int32_t  end_post;  /* 终止公里标 */
};

struct tunnel_t
{
    uint16_t tele_num;
    uint16_t cross_dist;
    uint32_t km_post;
    uint32_t length;
};
typedef struct convert_mile_node {
    int16_t t_num;             /* 信号机编号 */
    int32_t  discont_km_post;   /* 原始信号机公里标 */
    int32_t  cont_km_post;      /* 连续信号机公里标 */
    int16_t distance;          /* 距离后方信号机距离 */
}CONVERT_MILE_NODE;
int convert_seq_mile(int16_t telenum,int16_t distance);
/**
 * @function basedata_file_extr
 * @brief 从basedata数据文件中提取
 * @param r_id 新行程标识符
 * @param r_list 行程链表，提取成功则会生成行程链表
 * @return 数据提取的结果，成功返回EXTR_SUCCESS
 **/
ROUTE_EXTR_RESULT basedata_file_extr(struct route_id_t  r_id, ROUTE_LIST    *r_list);
#endif
