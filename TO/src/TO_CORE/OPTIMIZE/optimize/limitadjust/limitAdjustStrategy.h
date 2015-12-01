/**
 * @file    limitAdjustStrategy.h
 * @brief  限速调整策略接口函数
 * @date  2014.4.14
 * @author danny
 * @note
 */
#ifndef LIMITADJUSTSTRATEGY_H_
#define LIMITADJUSTSTRATEGY_H_

#include "opt_global_var.h"
#include "limitAdjustStrategy.h"
#include "optimizeTools.h"
#include "tractionCaculator.h"
#include "exception.h"

/**
 *  限速信息
 */
struct limitInfo
{
	float start;  /**< 限速起始公里标*/
	float end;  /**< 限速终止公里标*/
	float limit; /**< 限速值*/
	int indexS; /**< 限速起始公里标所在的段的index*/
	int index1; /**<  限速起始公里标所在的优化曲线的index*/
	int indexE; /**< 限速终止公里标所在的段的index*/
	int index2; /**< 限速终止公里标所在的优化曲线的index*/
};
typedef struct limitInfo LIMITINFO ;
/**
 * @brief 限速策略调整函数指针
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线的长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT (*limitStrategy)(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief 限速范围全在陡下坡/超抖下坡 或者跨陡下坡和超陡下坡
 * @param optSch  优化速度曲线
 * @param len  优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy01(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 *  @brief 限速范围都在陡上坡
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy02(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief 限速范围都在缓坡（缓上坡，缓下坡，平坡）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy03(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief  限速范围跨陡下坡和陡上坡（陡下坡先，陡上坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy04(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief 限速范围跨陡上坡和陡下坡（陡上坡先，陡下坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy05(OPTSCH* optsch,int len,LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief  限速范围跨陡下坡和缓坡（陡下坡先，缓坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy06(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief  限速范围跨缓坡和陡下坡（缓坡先，陡下坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy07(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief 限速范围跨陡上坡和缓坡（陡上坡先，缓坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy08(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief 限速范围跨缓坡和陡上坡（缓坡先，陡上坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy09(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);

/**
 * @brief 限速范围部分或者完全在手动区域段
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy10(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief 判断当前index所在的点是否是手动区域，若是手动区域则返回原始档位，若不是手动区域则返回newGear;
 * @param optSch  原始优化速度曲线
 * @param index 当前点所在的原始优化速度曲线中所在的index
 * @param srcGgear  新的档位
 * @return 返回的档位
 */
int identifyGear(OPTSCH* optSch, int index , int newGear);

#endif /* LIMITADJUSTSTRATEGY_H_ */
