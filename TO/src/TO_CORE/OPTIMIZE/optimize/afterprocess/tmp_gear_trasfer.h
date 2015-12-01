/**
 * @file tmp_gear_trasfer.h
 * @brief 临时限速档位连续换档策略接口
 * @date Created on: 2015年3月10日
 *  @author Author: sinllychen
 */
#include "opt_global_var.h"
#include "tractionCaculator.h"
#ifndef OPTIMIZE_OPTIMIZE_AFTERPROCESS_GEARTRASFER_H_
#define OPTIMIZE_OPTIMIZE_AFTERPROCESS_GEARTRASFER_H_
#define MAXGEN 100 //最大的偏移位置

/**
 * @brief 档位连续变换策略，其中中间档位为奇数个档位。如从5档-3档，中间只有4档，奇数个档位
 * @param new_tmp_curve   临时限速调整片段曲线
 * @param tmp_final_len  临时限速调整片段曲线长度(当前最后一个点的index为tmp_fianl_len-1)
 * @param cap tmp_final_len的容量
 * @param optSch    进行临时限速调整之后的完整的曲线
 * @param srcInxT 临时限速与完整曲线衔接部分在完整曲线中的index
 * @param tStep  普通档位跳跃的时间长度
 * @param idelStep 惰行档位跳跃的时间长度
 * @param opt_ret 优化异常结果
 * @return  临时限速调整之后的经过档位连续变换后的完整曲线
 */
OPTSCH* odd_gear_trasfer(OPTSCH* new_tmp_curve,int * tmp_final_len,int* cap, OPTSCH* optSch,int* srcInxT,float tStep,float idelStep,OPTIMIZE_RESULT* opt_ret);
/**
 * @brief 档位连续变换策略，其中中间档位为奇数个档位。如从5档-2档，中间只有4,3档，偶数个档位
 * @param new_tmp_curve   临时限速调整片段曲线
 * @param tmp_final_len  临时限速调整片段曲线长度(当前最后一个点的index为tmp_fianl_len-1)
 * @param cap tmp_final_len的容量
 * @param optSch    进行临时限速调整之后的完整的曲线
 * @param srcInxT 临时限速与完整曲线衔接部分在完整曲线中的index
 * @param tStep  普通档位跳跃的时间长度
 * @param idelStep 惰行档位跳跃的时间长度
 * @param opt_ret 优化异常结果
 * @return  临时限速调整之后的经过档位连续变换后的完整曲线
 */
OPTSCH*  even_gear_trasfer(OPTSCH* new_tmp_curve,int * tmp_final_len,int*cap,OPTSCH* optSch,int* srcInxT,float tStep,float idelStep,OPTIMIZE_RESULT* opt_ret);
/**
 * @brief 档位连续变换策略，其中中间档位是从负档位跳跃到正档位或者从正档位跳跃到负档位
 * @param new_tmp_curve   临时限速调整片段曲线
 * @param tmp_final_len  临时限速调整片段曲线长度(当前最后一个点的index为tmp_fianl_len-1)
 * @param cap tmp_final_len的容量
 * @param optSch    进行临时限速调整之后的完整的曲线
 * @param srcInxT 临时限速与完整曲线衔接部分在完整曲线中的index
 * @param tStep  普通档位跳跃的时间长度
 * @param idelStep 惰行档位跳跃的时间长度
 * @param opt_ret 优化异常结果
 * @return  临时限速调整之后的经过档位连续变换后的完整曲线
 */
OPTSCH*  jump_odd_gear_trasfer(OPTSCH* new_tmp_curve,int * tmp_final_len,int*cap,OPTSCH* optSch,int *srcInxT,float tStep,float idelStep,OPTIMIZE_RESULT* opt_ret);
/**
 * @brief 档位连续变换策略，其中中间档位是从负档位跳跃到正档位或者从正档位跳跃到负档位
 * @param new_tmp_curve   临时限速调整片段曲线
 * @param tmp_final_len  临时限速调整片段曲线长度(当前最后一个点的index为tmp_fianl_len-1)
 * @param cap tmp_final_len的容量
 * @param optSch    进行临时限速调整之后的完整的曲线
 * @param srcInxT 临时限速与完整曲线衔接部分在完整曲线中的index
 * @param tStep  普通档位跳跃的时间长度
 * @param idelStep 惰行档位跳跃的时间长度
 * @param opt_ret 优化异常结果
 * @return  临时限速调整之后的经过档位连续变换后的完整曲线
 */
OPTSCH*  jump_even_gear_trasfer(OPTSCH* new_tmp_curve,int * tmp_final_len,int*cap,OPTSCH* optSch,int* srcInxT,float tStep,float idelStep, OPTIMIZE_RESULT* opt_ret);
/**
 *@brief  档位连续变换策略匹配函数
 * @param new_tmp_curve   临时限速调整片段曲线
 * @param tmp_final_len  临时限速调整片段曲线长度(当前最后一个点的index为tmp_fianl_len-1)
 * @param cap 临时限速调整片段结构的当前最大容量
 * @param sGear  临时限速需要渐变的档位
 * @param optSch    进行临时限速调整之后的完整的曲线
 * @param eGear  临时限速片段同完整曲线衔接部分需要连续变换的终止档位
 * @param srcInx 临时限速与完整曲线衔接部分在完整曲线中的index
 * @param opt_ret 异常结果
 * @return  临时限速调整之后的经过档位连续变换后的完整曲线
 */
OPTSCH*  gear_strategy_match(OPTSCH* new_tmp_curve,int * tmp_final_len,int * cap, int sGear,OPTSCH* optSch,  int eGear,int* srcInx, OPTIMIZE_RESULT* opt_ret);
///**
// * @brief 往前判断从index开始的档位是否是我们需要的档位，同时该档位的运行时间至少为最低限定时间
// * @param optSch  原始速度曲线
// * @param len   原始速度曲线的长度
// * @param srcGear   需要的档位
// * @param index   判断的起始index
// * @param time  限定的时间
// * @return
// */
//int verifyFwdGear(OPTSCH* optSch,int len, int srcGear,int index,float time);
/**
 *@brief 计算档位调整之前和档位调整之后的曲线的交点位置
 * @param optSch  原始曲线
 * @param srcIndex  原始需要调整的index
 * @param optBack   调整之后的部分曲线
 * @param newIndex  调整之后的点
 * @param midStart  交点的公里标
 * @param midV  交点的速度
 */
void get_mid_position(OPTSCH*optSch, int srcIndex,OPTSCH* optBack,int newIndex,float* midStart,float* midV);

#endif /* OPTIMIZE_OPTIMIZE_AFTERPROCESS_GEARTRASFER_H_ */
