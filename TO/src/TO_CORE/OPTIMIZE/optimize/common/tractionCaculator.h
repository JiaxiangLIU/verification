/**
 * @file    tractionCaculator.h
 * @brief  提供牵引计算模型的基本方法接口
 * @date  2014.4.8
 * @author sinlly
 * @note 包括 作用力、油耗的计算,单位时间行驶距离的计算, 单位距离行驶时间的计算,多质点模型中等效坡度的计算
 */
#ifndef TRACTIONCACULATOR_H_
#define TRACTIONCACULATOR_H_
#include "tractionCaculator.h"
#include <math.h>
#include "opt_global_var.h"
#include "tools.h"
#include <string.h>

/**
 * @brief 根据机车的档位和速度获得机车的作用力
 * @param gear  驾驶档位
 * @param v   速度
 * @return  作用力
 */
TRAC_CAL_RESULT  getForce(int gear, float v,float *force);//机车的作用力,bdforce

/**
 * @brief  对单位时间行驶的距离，油耗，速度变化量计算
 * @param start  起始位置
 * @param v   当前速度
 * @param gear  当前档位
 * @param tStep   单位时间
 * @param count   roadCategory的index,只是为了快速索引，从0开始也可以
 * @param delta_s  单位时间所行驶的距离
 * @param delta_v  单位时间速度变化量
 * @param delta_e  单位时间消耗油耗
 * @return  成功标识为1,非成功标识0
 */
TRAC_CAL_RESULT  DoCaculateByTime(float start, float v, int gear, float tStep, int *count,float *delta_s, float *delta_v, float *delta_e);//单位时间行驶距离的计算
/**
 * @brief 单位距离行驶的时间，油耗，速度变化量计算
 * @param start   起始位置
 * @param v   当前速度
 * @param gear  当前档位
 * @param tStep   单位距离行驶的时间
 * @param count   roadCategory的index,只是为了快速索引，从0开始也可以
 * @param delta_s  单位距离
 * @param delta_v  单位时间速度变化量
 * @param delta_e  单位时间消耗油耗
 * @return  成功标识为1,非成功标识为0
 */
TRAC_CAL_RESULT DoCaculateByDistance(float start,float v,int gear,float *delta_t,int *count,float delta_s,float *delta_v,float *delta_e);//单位距离行驶时间的计算
/**
 * @brief 多指点模型计算坡度
 * @param start  当前公里标
 * @param count  roadCategory的index,只是为了快速索引，从0开始也可以
 * @return  多质点计算之后的坡度
 */
float multiParticalModel(float start,int *count);//多质点模型
/**
 * @brief 根据档位和时间获得油耗
 * @param gear  档位
 * @param tStep  时间
 * @return   油耗
 */
float getConsumption(int gear, float tStep);
/**
 * @brief 检验常用空气制动是否安全
 * @param start开始坐标
 * @param end 终止坐标
 * @param v_in 开始速度
 * @param v_out 终止速度
 * @param real_distance返回的实际距离
 * @param cal_common_distance 返回的常用制动距离
 * @param cal_ergence_distance返回的紧急制动距离
 * @return 0 不安全 1 安全
 */
TRAC_CAL_RESULT airBrakeDistance(float start,float end,float v_in,float v_out,float*real_distance,float* cal_common_distance,float *cal_ergence_distance,int* flag);
void airTest1(OPTSCH* optSch,int optSch_count,char*file);
#endif /* TRACTIONCACULATOR_H_ */
