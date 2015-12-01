/**
 * @file   optimize_curve.h
 * @brief   主函数，包含优化计算，线程调度，日志处理，通信等的所有的主方法
 * @date  2014.3.31
 * @author sinlly
 */
#ifndef OPTIMIZE_CURVE_H_
#define OPTIMIZE_CURVE_H_

#include "optimize_curve.h"
#include "rawOptimize.h"
#include "extrData.h"
#include "preprocess.h"

#define max(a,b)(((a)>(b))?(a):(b))
#define min(a,b)(((a)>(b))?(b):(a))
#define mean(a,b)(0.5*((a)+(b)))
#define BOOL int
#define TRUE 1
#define FALSE 0
/**
 * 优化外层主入口函数
 * @param routeNum 行程号
 * @return  优化速度曲线
 */
OPTSCHCURVE* optimize(int routeNum) ;
/**
 * @brief 临时限速处理主入口函数
 * @param startPost  临时限速起始公里标
 * @param endPost  临时限速终止公里标
 * @param value   临时限速值
 * @param optSch    优化速度曲线
 * @param len  原始优化速度曲线的长度
 * @param tmp_len 临时限速最终结果片段的长度
 * @return   处理之后的优化速度曲线
 */
OPTSCHCURVE* tempLimitOpimize(int startPost, int endPost,int value,OPTSCH* optSch,int len,int* tmp_len);

#endif /* MAIN_H_ */
