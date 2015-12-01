/**
 * @file     rawOptimize.h
 * @brief    原始优化计算主入口函数接口
 * @date  2014.4.9
 * @author sinlly
 */
#ifndef RAWOPTIMIZE_H_
#define RAWOPTIMIZE_H_

#include "opt_definition.h"
#include "opt_global_var.h"
#include "rawStrategy.h"
#include "optimizeTools.h"
#include "extrData.h"
#include "exception.h"
/**
 * @brief  原始优化主流程入口函数
 * @param massType   车重量
 * @param startPost   起始公里标
 * @param endPost    终止公里标
 * @param vStart    起始速度
 * @param vEnd     终止速度
 * @param ret   异常处理
 * @return   优化速度曲线
 */
OPTSCH* rawOptimize(int massType,int startPost, int endPost,float vStart,float vEnd,OPTIMIZE_RESULT *ret);

#endif /* RAWOPTIMIZE_H_ */
