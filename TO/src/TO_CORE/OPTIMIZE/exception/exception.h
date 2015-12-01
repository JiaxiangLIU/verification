/**
 * @file exception.h
 * @brief 优化模块异常处理结构定义，异常描述结果统一描述成文件名+相关描述的形式"
 * @date Created on: 2015年1月15日
 * @author: sinllychen
 */

#ifndef TO_0_5_OPTIMIZE_EXCEPTION_EXCEPTION_H_
#define TO_0_5_OPTIMIZE_EXCEPTION_EXCEPTION_H_

#define EXP_DESC_LENGTH 100
#include "exception.h"
#include "opt_global_var.h"
#include <string.h>
/**
 *文件读取异常标识
 */
typedef enum opt_extr_code
{
    OPT_EXTR_SUCCESS = 1,  //!< 文件读取成功
    OPT_OPEN_ERROR = -1,   //!< 文件打开失败
    OPT_MALLOC_ERROR=-2,//!<内存分配错误
    XML_EXTR_ERROR=-3,//!<XML文件打开失败
	OPT_FILE_SEEK_ERROR=-4,//!<文件读取定位失败
	OPT_TYPE_TRANSFORM_ERROR=-5//!<数据类型转换及数据范围错误
}OPT_EXTR_CODE;
/**
 * 文件读取结果
 */

typedef struct opt_extr_result
{
	OPT_EXTR_CODE  file_result; //!<异常代码
	char  description[EXP_DESC_LENGTH]; //!<异常描述
}OPT_EXTR_RESULT;

/**
 * 牵引计算异常标识
 */
typedef enum trac_cal_code
{
    TRAC_CAL_SUCCESS =1, //!< 牵引计算成功
	VELOCITY_OVER_MAX = 120,//!< 速度超出120
	VELOCITY_UNDER_ZERO = 0,//!< 速度小于0
	INVALID_GEAR = -1       //!< 无效档位低于-8或者高于8
}TRAC_CAL_CODE;
/**
 * 牵引计算结果
 */
typedef struct trac_cal_result
{
     TRAC_CAL_CODE  trac_result; //!<牵引计算异常代码
     char  description[EXP_DESC_LENGTH]; //!<异常描述
}TRAC_CAL_RESULT;

/**
 *  优化预处理计算异常代码
 */
typedef enum pre_process_code
{
     PRE_PROCESS_SUCCESS =1,//!< 预处理正常
	 PRE_MALLOC_ERROR=-1,         //!< 内存分配错误
	 PRE_FIND_INDEX_ERROR =-2,    //!< 索引数组无法找到
	 PRE_NOT_FIND_LIMIT=-3,       //!< 找不到限速
	 PRE_NOT_FIND_CZ=-7
}PRE_PROCESS_CODE;
/**
 * 优化预处理计算结果
 */
typedef struct pre_process_result
{
	 PRE_PROCESS_CODE  preprocess_result;//!<预处理异常代码
	 char  description[EXP_DESC_LENGTH];//!<异常描述
}PRE_PROCESS_RESULT;

/**
 * 原始优化计算异常代码
 */
typedef enum raw_optimize_code
{
	RAW_OPTIMIZE_SUCCESS =1,//!< 原始优化计算成功
	RAW_MALLOC_ERROR=-1,       //!< 内存分配失败
	RAW_REALLOC_ERROR=-2 ,     //!< 内存再分配失败
	RAW_NOT_FIND_STRATEGY = -3 //!< 未找到策略
}RAW_OPTIMIZE_CODE;
/**
 * 原始优化计算结果
 */
typedef struct raw_optimize_result
{
	RAW_OPTIMIZE_CODE  rawopt_result;//!<原始优化异常处理代码
	char  description[EXP_DESC_LENGTH];//!<异常描述
}RAW_OPTIMIZE_RESULT;

/**
 * 限速调整计算异常代码
 */
typedef enum limit_optimize_code
{
	LIMIT_OPTIMIZE_SUCCESS=1,//!< 限速调整成功
	LIMIT_MALLOC_ERROR=-1,        //!< 内存分配失败
	LIMIT_REALLOC_ERROR=-2,       //!< 内存再分配失败
	LIMIT_NOT_FIND_STRATEGY=-3    //!< 未找到策略
}LIMIT_OPTIMIZE_CODE;
/**
 * 限速调整计算结果
 */
typedef struct limit_optimize_result
{
	LIMIT_OPTIMIZE_CODE   limitopt_result;//!<原始优化异常处理代码
	char  description[EXP_DESC_LENGTH];//!<异常描述
}LIMIT_OPTIMIZE_RESULT;
/**
 * 整个优化模块最外层的异常代	FILE_SEEK_ERROR = 40,    //!< 文件读取时索引错误
	VELOCITY_EXCEED = 45 ,   //!< 速度越界
	VELOCITY_BELOW_ZERO = 47,//!< 速度小于0
	GEAR_INVALID = 49,       //!< 无效档位
	MALLOC_ERROR = 60,       //!< 内存分配失败
	REALLOC_ERROR = 61,      //!< 内存再次分配失败
	NOT_FIND_STRATEGY =72,   //!< 未找到策略
	FIND_INDEX_ERROR=75,      //!< 未找到元素码
 */
typedef enum optimize_code
{
	OPTIMIZE_SUCCESS =1 ,    //!< 优化成功
	FILE_OPEN_ERROR = 20,    //!< 文件打开错误
	FILE_SEEK_ERROR = 40,    //!< 文件读取时索引错误
	VELOCITY_EXCEED = 45 ,   //!< 速度越界
	VELOCITY_BELOW_ZERO = 47,//!< 速度小于0
	GEAR_INVALID = 49,       //!< 无效档位
	MALLOC_ERROR = 60,       //!< 内存分配失败
	REALLOC_ERROR = 61,      //!< 内存再次分配失败
	NOT_FIND_STRATEGY =72,   //!< 未找到策略
	FIND_INDEX_ERROR=75,      //!< 未找到元素
	ROUTE_DATA_ERROR=32,//!<未找到限速
	TYPE_TRANSFORM_ERROR= 77 //!<数据类型转换及数据范围错误
}OPTIMIZE_CODE;
/**
 * 整个优化模块最外层返回结果
 */
typedef struct optimize_result
{
	 OPTIMIZE_CODE  opt_rest;//!<优化异常处理代码
	 char description[EXP_DESC_LENGTH];//!<异常描述
}OPTIMIZE_RESULT;
/**
 * @brief 将文件的异常结果适配给最终的优化结果
 * @param result  文件结果
 * @return   优化模块的优化结果
 */
OPTIMIZE_RESULT  file_result_adapter( OPT_EXTR_RESULT result);
/**
 * @brief 将牵引计算的异常结果适配给最终的优化结果
 * @param result 牵引计算的结果
 * @return  优化模块的优化结果
 */
OPTIMIZE_RESULT  trac_result_adapter(TRAC_CAL_RESULT result);
/**
 * @brief 将预处理的异常结果适配给最终的优化结果
 * @param result  预处理的结果
 * @return 优化模块的优化结果
 */
OPTIMIZE_RESULT  pre_result_adapter(PRE_PROCESS_RESULT result);
/**
 * @brief  将原始优化计算的异常结果适配给最终的优化结果
 * @param result  原始优化计算的结果
 * @return  优化模块的优化结果
 */
OPTIMIZE_RESULT  raw_result_adapter(RAW_OPTIMIZE_RESULT  result);
/**
 * @brief 将限速调整的异常结果适配给最终的优化结果
 * @param result  限速调整的结果
 * @return  优化模块的优化结果
 */
OPTIMIZE_RESULT  limit_result_adapter(LIMIT_OPTIMIZE_RESULT  result);
/**
 * @brief 优化异常结果输出
 * @param result  优化结果
 */
void exception_display(OPTIMIZE_RESULT result);
/**
 * @brief 文件异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return    包装之后的异常
 */
void  file_exception_decorator(int code,OPT_EXTR_RESULT* ret, char desc[]);
/**
 * @brief 牵引计算异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return     包装之后的异常
 */
void   trac_exception_decorator(int code,TRAC_CAL_RESULT *ret , char desc[]);
/**
 * @brief 预处理异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return  ret  包装之后的异常
 */
void  pre_exception_decorator(int code,PRE_PROCESS_RESULT *ret, char desc[]);
/**
 * @brief 限速调整异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return  ret  包装之后的异常
 */
void  limit_opt_exception_decorator(int code,LIMIT_OPTIMIZE_RESULT *ret,char desc[]);
/**
 * @brief 原始优化异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return  ret  包装之后的异常
 */
void  raw_opt_exception_decorator(int code,RAW_OPTIMIZE_RESULT *ret, char desc[]);
/**
 * @brief 优化异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return ret  包装之后的异常
 */
void  optimize_exception_decorator(int code,OPTIMIZE_RESULT *ret,char desc[]);

#endif /* TO_0_5_OPTIMIZE_EXCEPTION_EXCEPTION_H_ */
