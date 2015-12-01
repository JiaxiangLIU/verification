/**
 * @file     limitAdjust.h
 * @brief   限速调整主入口函数接口
 * @date  2014.7.23
 * @author sinlly
 * @note  包括普通限速和临时限速
 */
#ifndef LIMITADJUST_H_
#define LIMITADJUST_H_
#include "opt_global_var.h"
#include "optimizeTools.h"
#include "limitAdjust.h"
#include "tools.h"
#include "limitAdjustStrategy.h"
#include "extrData.h"
#include "exception.h"
/**
 *@brief  优化限速策略处理主入口函数
 *@param  optSch  原始优化曲线
 *@param  len 原始优化曲线的长度
 *@param  startPost 进行限速调整的起始位置
 *@param  endPost  进行限速调整的终止位置
 *@return 异常处理结果
 */
OPTIMIZE_RESULT  limitAdjust(OPTSCH* optsch,int len,int startPost,int endPost);
/**
 * @brief  临时限速处理
 * @param optSch 上一次的原始优化结果（若出现多次临时限速处理则为上一次原始结果，若为第一次，则为第一次的优化结果）
 * @param len 优化速度曲线长度
 * @param limitStart 临时限速的起始公里标
 * @param limitEnd 临时限速的终止公里标
 * @param value 临时限速值
 * @param limitIndexS  临时限速起始位置在optSch中的index
 * @param limitIndexE  临时限速终止位置在optSch中的index
 * @return 异常处理结果
 */
OPTIMIZE_RESULT  tempLimitAdjust(OPTSCH* optsch,int len, int limitStart,int limitEnd,int value,int *limitIndexS,int *limitIndexE);

#endif /* LIMITADJUST_H_ */
