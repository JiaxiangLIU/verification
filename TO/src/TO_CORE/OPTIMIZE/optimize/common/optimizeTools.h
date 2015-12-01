/**
 * @file    optimizeTools.h
 * @brief  优化部分会用到的公共方法接口
 * @date  2014.4.8
 * @author sinlly
 * @note  主要包括油耗评估等
 */

#ifndef OPTIMIZETOOLS_H_
#define OPTIMIZETOOLS_H_
#include <math.h>
#include "opt_global_var.h"
/**
 * @brief 评估油耗
 * @param E  整体油耗
 * @param T  整体时间
 * @param diffTime  时间偏差
 * @param optSch 优化曲线结果
 * @param srcTime  司机运行时间
 * @param srcConsumption 司机运行油耗
 */
void evaluate(float* E, float* T, float* diffTime, OPTSCH* optSch,int len,float srcTime,float srcConsumption) ;//评估油耗
/**
 * @brief 存取临时结果
 * @param fileName 文件名
 * @param optSch  优化结果
 */
void saveResult(OPTSCH* optSch,int len,char* fileName);//保存结果
/**
 * @brief 存取最终结果
 * @param optSch  优化结果
 */
void saveFinalResult(OPTSCHCURVE* final_curve,int len,char* fileName);
/**
 * @brief 计算一定范围内的时间
 * @param  indexS   初始index
 * @param indexE  终止index
 * @param  optSch  优化结果
 * @return 返回indexS-indexE之间的时间
 */
float computeTime(int indexS,int indexE,OPTSCH* optSch);
/**
 * @brief 获得所在坡度段的限速大小
 * @param start  坡段的起始公里标
 * @param ending  坡段的终止公里标
 * @return  返回该段的限速
 */
int getCategoryLimitV(int start,int ending);
/**
 * @brief 获得当前的start位置和ending位置所在优化结果optSch中的对应的速度和index
 * @param optSch  优化结果
 * @param  start  起始公里标
 * @param  ending  终止公里标
 * @param  v1   起始公里标在optSch中对应的速度
 * @param  index1  起始公里标在optSch中对应的index
 * @param  v2  终止公里标在optSch中对应的速度
 * @param  index2  终止公里标在optSch中对应的index
 *
 */
void getSchVelocity(OPTSCH* optSch,int len,float start,float ending,float* v1,int* index1,float* v2,int* index2);
/**
 *@brief 获得当前start位置和ending位置所在的roadCategory中对应的坡度类型以及index
 *@param start  起始公里标
 *@param ending  终止公里标
 *@param flagS  起始公里标所在的roadCategory中对应的坡度类型
 *@param indexS  起始公里标所在的roadCategory中对应的index
 *@param flagE  终止公里标记所在的roadCategory中对应的坡度类型
 *@param indexE  终止公里标所在的roadCategory中对应的index
 *
 */
void getCategoryFlag(int start,int ending,int* flagS,int* indexS,int* flagE,int* indexE);
/**
 * @brief 从优化结果OPTSCH中寻找start和end所在OPTSCH中的index,取最外层
 * @param start   起始公里标
 * @param end   终止公里标
 * @param startIndex    找到的起始公里标对应的OPTSCH中的index
 * @param endIndex   找到的终止公里标对应的OPTSCH中的index
 */
void get_segment_index(OPTSCH* optSch,float start,float end,int* startIndex,int* endIndex,int len);
/**
 * @brief 给定信号机及越过距离，获得连续的公里标
 * @param tel_num 信号机编号
 * @param cross_dist 越过距离
 * @return 连续的公里标
 */
float get_connect_kp(int tel_num,int cross_dist);
#endif /* OPTIMIZETOOLS_H_ */
