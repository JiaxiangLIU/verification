/**
 *  @file after_process.h
 *  @brief  优化档位后处里生成新的曲线主函数接口
 *  @date Created on: 2015年4月2日
 *   @author sinlly
 */
#ifndef OPTIMIZE_OPTIMIZE_AFTERPROCESS_AFTER_PROCESS_H_
#define OPTIMIZE_OPTIMIZE_AFTERPROCESS_AFTER_PROCESS_H_
#include "tmp_gear_trasfer.h"
#include "opt_global_var.h"
#include "tractionCaculator.h"
#define  MOREGEAR 100 //是否有多个档位的标识
/**
 * 后处理中保存每个档位的基本信息
 */
struct gearSegInfo{
	  int index; //所在的数组中的index(从0开始)
	  int gear;    //档位
	  float midV;   //该档位的中间位置的速度
	  int startIndex; //起始公里标
	  int endIndex; //终止公里标
}typedef GEARSEGINFO;
/**
 * @brief  优化后处理生成最终曲线主函数
 * @param optSch   优化曲线
 * @param len 优化曲线长度
 * @param new_final_curve 新生成的曲线
 * @param final_len 新生成的曲线长度
 * @param final_ret  异常结果
 * @param 后处理后的曲线
 */
OPTSCH*  after_process(OPTSCH* optSch,int len,OPTSCH* new_final_curve,int* final_len,OPTIMIZE_RESULT*final_ret);
/**
 * @brief  临时限速优化后处理生成最终曲线主函数
 * @param optSch   临时限速调整之后的优化曲线片段
 * @param len 临时限速调整之后的优化曲线片段长度
 * @param new_final_curve   将临时限速优化结果调整为连续档位且拉平的新生成的曲线
 * @param final_len  新生成的曲线长度
 * @param cap new_final_curve数组的容量
 * @param final_ret  是否有异常
 * @param tmpStart 临时限速的起始公里标
 * @param tmpEnd 临时限速的终止公里标
 * @param tmpValue 临时限速的限速值
 * @return 临时限速优化结果
 */
OPTSCH*  tmp_after_process(OPTSCH* optSch,int len,OPTSCH* new_final_curve,int* final_len,int* cap, OPTIMIZE_RESULT*final_ret,float tmpStart,float tmpEnd,float tmpValue);
/**
 * @brief 获得两个位置的中间速度（中间公里标位置的速度）
 * @param optSch  优化速度曲线
 * @param sIndex  优化中起始位置的index
 * @param eIndex   优化中终止位置的index
 * @return   中间速度
 */
float getMidV(OPTSCH* optSch,int sIndex,int eIndex);
/**
 * @brief 判断sIndex到eIndex中是否有点接近限速
 * @param optSch 优化速度曲线
 * @param sIndex 需要判断是否接近限速的起始位置index
 * @param eIndex 需要判断是否接近限速的终止位置index
 * @return 1 表示接近限速，0表示未接近限速
 */
int ifNearLimit(OPTSCH* optSch,int sIndex,int eIndex);
/**
 * @brief 判断临时限速中sIndex到eIndex中是否有点接近限速
 * @param optSch  临时限速的优化曲线片段
 * @param sIndex 需要判断是否接近临时限速的起始位置index
 * @param eIndex 需要判断是否接近临时限速的终止位置index
 * @param tmpStart 临时限速的起始公里标
 * @param tmpEnd 临时限速的终止公里标
 * @param tmpValue 临时限速值
 * @return 1 表示接近限速，0表示未接近限速
 */
int ifTmpNearLimit(OPTSCH* optSch,int sIndex,int eIndex,float tmpStart,float tmpEnd,float tmpValue);
/**
 * @brief 档位连续渐变处理函数
 * @param new_final_curve  指向最终的优化曲线的曲线
 * @param srcGear  变换前档位
 * @param destGear  变换后档位
 * @param cap  new_final_curve的容量
 * @param n   当前的最终的优化曲线点的个数-1（处理后需要返回）,相当于new_final_curve最后一个点的index
 * @param dist   当前优化曲线的公里标位置（处理后需要返回）,相当于当前需要gear_transfer的起始位置，调用函数之后，返回为处理后的终止位置
 * @param  velocity  当前优化曲线的速度（处理后需要返回），同dist类似
 * @return  异常结果
 */
OPTIMIZE_RESULT gear_transfer(OPTSCH** new_final_curve,int srcGear,int destGear,int *cap,int *n,float* dist,float* velocity);
/**
 * @brief 根据给定的速度和力，通过牵引力和速度对应的表查询到相对应的档位，求出的力对应在g1,和g2对应的力之间，g1小于g2
 * @param force  给定的牵引力或者制动力
 * @param velocity  当前的速度
 * @param g1  较低的档位
 * @param g2  较高的档位
 * @param delF1 较低的档位对应的力同force的差值（绝对值）
 * @param delF2 较高的档位对应的力同force的差值（绝对值）
 * @return  若对应的力不再两个档位之间，则返回单个档位，若在两个档位之间则返回MOREGEAR的标识
 */
int getGearByFV(float force,int velocity,int *g1,int *g2,float *delF1,float* delF2);
/**
 * @brief 根据某个档位进行牵引
 * @param new_final_curve  新计算的优化速度曲线
 * @param gear  牵引的档位（可能为正档位也可能为负档位）
 * @param endS 牵引的结束位置
 * @param cap  new_final_curve的容量,存在扩容操作
 * @param n    new_final_curve的实际的点的个数（处理后需要返回）
 * @param dist   当前优化曲线的公里标位置（处理后需要返回）
 * @param  velocity  当前优化曲线的速度（处理后需要返回）
 * @return 异常结果
 */
OPTIMIZE_RESULT dragByGear(OPTSCH** new_final_curve,int gear,float endS, int * cap, int* n,float* dist,float* velocity);
/**
 * @brief 从tmpSegGear结构中获得从startIndex位置到endIndex位置的距离
 * @param tmpSegGears 保存档位长度小于给定长度的档位结构
 * @param optSch  原始优化曲线,需要通过其获得公里标记
 * @param startIndex tmpSegGears中的startIndex
 * @param endIndex tmpSegGears中的endIndex
 * @return 两个index对应的档位之间的距离
 */
float getPostFromTmpSeg(GEARSEGINFO*  tmpSegGears,OPTSCH* optSch,int startIndex,int endIndex);
/**
 * @brief 对临时限速最后的点进行调整（包括档位渐变以及追上原始曲线）
 * @param new_tmp_curve  临时限速最终的速度曲线
 * @param tmp_final_len  临时限速最终速度曲线的长度
 * @param cap  new_tmp_curve的容量
 * @param optSch  原始经过临时限速处理之后的曲线
 * @param index  临时限速处理的最后一个点在原始速度曲线中的index
 * @param ret 优化异常结果
 * @return 调整之后的临时限速曲线
 */
OPTSCH* tmp_end_process(OPTSCH* new_tmp_curve ,int* tmp_final_len,int *cap, OPTSCH* optSch,int* inx,OPTIMIZE_RESULT* ret);
#endif /* OPTIMIZE_OPTIMIZE_AFTERPROCESS_AFTER_PROCESS_H_ */
