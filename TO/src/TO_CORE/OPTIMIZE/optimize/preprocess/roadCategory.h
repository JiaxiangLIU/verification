/**
 * @file    roadCategory.h
 * @brief  分段相关接口函数
 * @date  2014.4.8
 * @author danny
 * @note  主要包括临界速度的计算,分段等
 */
#ifndef ROADCATEGORY_H_
#define ROADCATEGORY_H_

#include "opt_global_var.h"
#include "roadCategory.h"
#include "tractionCaculator.h"
#include "exception.h"

#define STATIONCOUNT 10  ///临时中间车站的个数
/**
 * 临界速度
 */
struct critical_v
{
	float value;   /**<  临界速度值 */
	int graflg;  /**< 坡度类型*/
	float mGra; /**< 坡度值 */
};
typedef struct critical_v CRITICAL_V;
/**
 * 用来存储分段的起始点和终止点的结构
 * 用于针对手动区域和贯通实验进行再分段
 * flag=0标识非手动区域
 * flag=opt_const.THROUGHEXPERT 为贯通实验区域
 * flag=opt_const.PUSHERENGINE 为补机区域段
 */
struct catPoint
{
	int value;  /**<公里标*/
	int flag; /**<标记*/
};
typedef struct catPoint CATPOINT;

/**
 * 道路分段临时存储结构
 */
struct Category
{
	int start;  /**< 段起始公里标 */
	int end;  /**< 段终止公里标 */
	float gradient;  /**< 段的平均坡度值 */
	int flag; /**< 段类型*/
	float vi; /**< 段起始速度*/
	float vo; /**< 段终止速度*/
	float suggear; /**< 段推荐的档位*/
};
typedef struct Category CATEGORY;
/**
 * @brief  临界速度计算
 * @param new_gradients   多质点模型下的加算坡度
 * @param max_length   加算坡度的长度
 * @param cv   指向临界速度数组的指针
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  computeCriticalV(MGRADIENT* new_gradients,int max_length,CRITICAL_V* cv);
/**
 *  @brief 道路分段
 * @param new_gradients  多质点模型下的加算坡度
 * @param max_length  加算坡度的长度
 * @param cv   临界速度
 * @param startPost   起始公里标
 * @param endPost   终止公里标
 * @return  优化异常结果
 */
OPTIMIZE_RESULT  computeRoadCategory(MGRADIENT* new_gradients,int max_length,CRITICAL_V* cv,int startPost, int endPost);
/**
 * @brief 加入手动区域（补机和贯通实验）之后进行重新分段，将手动区域变为独立的段
 * @param cat  原始的分段信息
 * @param len_cat 原始的分段信息的长度
 * @param rcTmp  新的分段信息存储结构
 * @param rc_tmp_len  新的分段的长度
 * @param pre_ret  优化预处理异常结果
 * @return 新的分段信息
 */
ROADCATEGORY* reRoadCategory(CATEGORY* cat,int len_cat,ROADCATEGORY* rcTmp,int* rc_tmp_len,PRE_PROCESS_RESULT* pre_ret);
/**
 * @brief CATPOINT结构体快速排序
 * @param  src   待排序的整型数组
 * @param  i  数组起始index 一般为0
 * @param  j  数组的终止index  一般为length-1
 */
void quickSortByCat(CATPOINT src[],int i,int j);
/**
 * @brief CATPOINT快排序获得分割点
 * @param src  原始数组
 * @param  i   起始index
 * @param  j 终止index
 * @return  分割点的index
 */
int partitionByCat(CATPOINT src[],int i,int j);
/**
 * @brief 数据交换
 * @param src 需要交换的数组
 * @param i  交换的数的index
 * @param j  交换的数的index
 */
void swapByCat(CATPOINT src[],int i,int j);//交换
/**
 *@brief  去除数组中的重复顶点
 *@param  src 源数组
 *@param  srcLength 源数组长度
 *@param  dest 去除重复点后的数组
 *@param  destLength 去除重复点后的数组长度
 */
void uniqueByCat(CATPOINT src[],int srcLength,CATPOINT dest[],int* desLength);//去除重复的点
#endif /* ROADCATEGORY_H_ */
