/*
 * exception.c
 * 优化模块异常处理相关的函数实现
 *  Created on: 2015年1月15日
 *      Author: sinllychen
 */
#include "exception.h"
/**
 * @brief 将文件的异常结果适配给最终的优化结果
 * @param result  文件结果
 * @return   优化模块的优化结果
 */
OPTIMIZE_RESULT  file_result_adapter( OPT_EXTR_RESULT result)
{
	OPTIMIZE_RESULT ret;
	switch(result.file_result)
	{
	case OPT_EXTR_SUCCESS:
		ret.opt_rest = OPTIMIZE_SUCCESS; break;
	case OPT_OPEN_ERROR:
	case XML_EXTR_ERROR :
		ret.opt_rest=FILE_OPEN_ERROR; break;
	case OPT_MALLOC_ERROR:
		ret.opt_rest = MALLOC_ERROR; break;
	case OPT_FILE_SEEK_ERROR:
		ret.opt_rest = FILE_SEEK_ERROR ; break;
	case OPT_TYPE_TRANSFORM_ERROR:
		ret.opt_rest = TYPE_TRANSFORM_ERROR; break;
	}
	strcpy(ret.description,result.description);
	return ret;
}
/**
 * @brief 将牵引计算的异常结果适配给最终的优化结果
 * @param result 牵引计算的结果
 * @return  优化模块的优化结果
 */
OPTIMIZE_RESULT  trac_result_adapter(TRAC_CAL_RESULT result)
{
	OPTIMIZE_RESULT ret;
	switch(result.trac_result)
	{
	case TRAC_CAL_SUCCESS:
		ret.opt_rest = OPTIMIZE_SUCCESS; break;
	case VELOCITY_OVER_MAX:
		ret.opt_rest=VELOCITY_EXCEED;break;
	case VELOCITY_UNDER_ZERO:
		ret.opt_rest=VELOCITY_BELOW_ZERO; break;
	case INVALID_GEAR:
		ret.opt_rest = GEAR_INVALID; break;
	}
	strcpy(ret.description,result.description);
	return ret;
}
/**
 * @brief 将预处理的异常结果适配给最终的优化结果
 * @param result  预处理的结果
 * @return 优化模块的优化结果
 */
OPTIMIZE_RESULT  pre_result_adapter(PRE_PROCESS_RESULT result)
{
	OPTIMIZE_RESULT ret;
	switch(result.preprocess_result)
	{
	case PRE_PROCESS_SUCCESS:
		ret.opt_rest = OPTIMIZE_SUCCESS; break;
	case PRE_MALLOC_ERROR:
		ret.opt_rest=MALLOC_ERROR;break;
	case PRE_FIND_INDEX_ERROR:
		ret.opt_rest=FIND_INDEX_ERROR; break;
	case PRE_NOT_FIND_LIMIT:
	case PRE_NOT_FIND_CZ:
		ret.opt_rest = ROUTE_DATA_ERROR; break;
	}
	strcpy(ret.description,result.description);
	return ret;
}
/**
 * @brief  将原始优化计算的异常结果适配给最终的优化结果
 * @param result  原始优化计算的结果
 * @return  优化模块的优化结果
 */
OPTIMIZE_RESULT  raw_result_adapter(RAW_OPTIMIZE_RESULT  result)
{
	OPTIMIZE_RESULT ret;
	switch(result.rawopt_result)
	{
	case RAW_OPTIMIZE_SUCCESS:
		ret.opt_rest = OPTIMIZE_SUCCESS; break;
	case RAW_MALLOC_ERROR:
		ret.opt_rest=MALLOC_ERROR;break;
	case RAW_REALLOC_ERROR:
		ret.opt_rest=REALLOC_ERROR; break;
	case RAW_NOT_FIND_STRATEGY:
		ret.opt_rest = NOT_FIND_STRATEGY; break;
	}
	strcpy(ret.description,result.description);
	return ret;
}
/**
 * @brief 将限速调整的异常结果适配给最终的优化结果
 * @param result  限速调整的结果
 * @return  优化模块的优化结果
 */
OPTIMIZE_RESULT  limit_result_adapter(LIMIT_OPTIMIZE_RESULT  result)
{
	OPTIMIZE_RESULT ret;
	switch(result.limitopt_result)
	{
	case LIMIT_OPTIMIZE_SUCCESS:
		ret.opt_rest = OPTIMIZE_SUCCESS; break;
	case LIMIT_MALLOC_ERROR:
		ret.opt_rest=MALLOC_ERROR;break;
	case LIMIT_REALLOC_ERROR:
		ret.opt_rest=REALLOC_ERROR; break;
	case LIMIT_NOT_FIND_STRATEGY:
		ret.opt_rest = NOT_FIND_STRATEGY; break;
	}
	strcpy(ret.description,result.description);
	return ret;
}

void exception_display(OPTIMIZE_RESULT result)
{
     switch(result.opt_rest)
     {
     case FILE_OPEN_ERROR:
    	 fprintf(stderr,"文件打开失败:%s\n",result.description);
    	 opt_log_error("文件打开失败:%s", result.description);
    	 exit(0);
    	 break;
     case FILE_SEEK_ERROR:
    	 fprintf(stderr,"文件读取索引失败:%s\n",result.description);
    	 opt_log_error("文件读取索引失败:%s", result.description);
    	 exit(0);
    	 break;
     case VELOCITY_EXCEED:
    	 fprintf(stderr,"速度超过最大速度:%s\n",result.description);
    	 opt_log_error("速度超过最大速度:%s", result.description);
    	 exit(0);
    	 break;
     case VELOCITY_BELOW_ZERO:
    	 fprintf(stderr,"速度小于0:%s\n",result.description);
    	 opt_log_error( "速度小于0:%s", result.description);
    	 exit(0);
    	 break;
     case GEAR_INVALID:
    	 fprintf(stderr,"档位无效(<最低档位或>最高档位):%s\n",result.description);
    	 opt_log_error("档位无效(<最低档位或>最高档位):%s", result.description);
    	 exit(0);
    	 break;
     case MALLOC_ERROR:
    	 fprintf(stderr,"内存分配失败:%s\n",result.description);
    	 opt_log_error("内存分配失败:%s", result.description);
    	 exit(0);
    	 break;
     case REALLOC_ERROR:
    	 fprintf(stderr,"内存再分配失败:%s\n",result.description);
    	 opt_log_error("内存再分配失败:%s", result.description);
    	 exit(0);
    	 break;
     case NOT_FIND_STRATEGY:
    	 fprintf(stderr,"未找到策略:%s\n",result.description);
    	 opt_log_error("未找到策略:%s", result.description);
    	 exit(0);
    	 break;
     case ROUTE_DATA_ERROR:
    	 fprintf(stderr,"交路数据出错:%s\n",result.description);
    	 opt_log_error("交路数据出错:%s", result.description);
    	 exit(0);
    	 break;
     case FIND_INDEX_ERROR:
    	 fprintf(stderr,"查找index错误:%s\n",result.description);
    	 opt_log_error("查找index错误:%s", result.description);
    	 exit(0);
    	 break;
     case TYPE_TRANSFORM_ERROR:
    	 fprintf(stderr,"数据类型转换或数据范围错误:%s\n",result.description);
    	 opt_log_error("数据类型转换或数据范围错误:%s", result.description);
    	 exit(0);
    	 break;
     default:
    	 opt_log_info("在线优化成功");
    	 printf("在线优化成功\n");
    	 break;
     }
}
/**
 * @brief 文件异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return    包装之后的异常
 */
void  file_exception_decorator(int code,OPT_EXTR_RESULT* ret, char desc[])
{
	   ret->file_result=code;
	   strcpy(ret->description,desc);
}
/**
 * @brief 牵引计算异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return     包装之后的异常
 */
void  trac_exception_decorator(int code,TRAC_CAL_RESULT* ret, char desc[])
{
	ret->trac_result=code;
	strcpy(ret->description,desc);
}
/**
 * @brief 预处理异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return  ret  包装之后的异常
 */
void  pre_exception_decorator(int  code,PRE_PROCESS_RESULT* ret, char desc[])
{
	ret->preprocess_result=code;
	strcpy(ret->description,desc);
}
/**
 * @brief 限速调整异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return  ret  包装之后的异常
 */
void   limit_opt_exception_decorator(int  code,LIMIT_OPTIMIZE_RESULT * ret, char desc[])
{
	ret->limitopt_result=code;
	strcpy(ret->description,desc);
}
/**
 * @brief 原始优化异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return  ret  包装之后的异常
 */
void  raw_opt_exception_decorator(int  code,RAW_OPTIMIZE_RESULT* ret, char desc[])
{
	ret->rawopt_result=code;
	strcpy(ret->description,desc);
}
/**
 * @brief 优化异常包装
 * @param code  异常代码
 * @param desc  描述
 * @return ret  包装之后的异常
 */
void  optimize_exception_decorator(int code,OPTIMIZE_RESULT* ret, char desc[])
{
	ret->opt_rest=code;
	strcpy(ret->description,desc);
}

