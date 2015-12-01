/**
 * @file     preprocess.h
 * @brief   优化部分分段数据预处理入口函数接口
 * @date  2014.4.1
 * @author sinlly
 */
#ifndef PREPROCESS_H_
#define PREPROCESS_H_
#include "opt_global_var.h"
#include "preprocess.h"
#include "tools.h"
#include "exception.h"

/**
 * @brief 分段预处理主入口函数
 * @param startPost   起始公里标
 * @param endPost   终止公里标
 * @return  异常处理结果
 */
OPTIMIZE_RESULT  preprocess(int startPost,int endPost) ;

#endif /* PREPROCESS_H_ */
