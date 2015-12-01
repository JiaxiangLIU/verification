/**
 * @file      rawStrategy.h
 * @brief  原始优化计算策略
 * @date  2014.4.9
 * @author sinlly
 */
#ifndef RAWSTRATEGY_H_
#define RAWSTRATEGY_H_
#include "opt_global_var.h"
#include "optimizeTools.h"
#include "tractionCaculator.h"
#include "extrData.h"
#include "exception.h"


/**
 * @brief 策略调用函数指针
 * @param vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param  optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* (*rawStrategy)(float vo,float v_interval,int count,int roadIndex,int index,OPTSCH* optSch,float avgV,OPTIMIZE_RESULT* ret);
/**
 * @brief  超陡下坡策略函数实现
  *@param vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* sup_steep_dwn_strgy(float vo,float v_interval,int count,int roadIndex,int index,OPTSCH* optSch,float avgV,OPTIMIZE_RESULT* ret);
/**
 * @brief  陡上坡策略函数实现
  *@param vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* steep_up_strgy(float vo,float v_interval,int count,int roadIndex,int index,OPTSCH* optSch,float avgV,OPTIMIZE_RESULT* ret);
/**
 * @brief  超陡下坡策略函数实现
  *@param  vo  段出速度
 * @param  v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param  optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* steep_dwn_strgy(float vo,float v_interval,int count,int roadIndex,int index,OPTSCH* optSch,float avgV,OPTIMIZE_RESULT *ret);
/**
 * @brief  缓坡策略函数实现
  *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* gentle_hill_strgy(float vo,float v_interval,int count,int roadIndex,int index,OPTSCH* optSch,float avgV,OPTIMIZE_RESULT *ret);
/**
 * @brief 空车 陡下坡策略函数实现
  *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* empty_steep_dwn_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret);
/**
 * @brief 当缓坡所在的车站之间的下坡的比例大于24%的时候，缓破策略采用-3档位进行牵引（此时缓坡的策略）
 * @param vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* steep_dwn_more_gentle_hill_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV, OPTIMIZE_RESULT *ret);

/**
 * @brief  空车的陡上坡策略
 * @param vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @note  空车的时候，无需将速度牵引到限速。而只需要将速度牵引到该段的平均速度，保持平均速度到段末
 * @param ret 异常处理结果
 * @return  优化曲线结果
 */
OPTSCH* empty_steep_up_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) ;

/**
 * @brief  缓坡策略函数实现,用于下一段为陡下坡或超陡下坡的缓坡策略
  *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* next_steep_dwn_gentle_hill_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret);
/**
 * @brief  缓坡策略函数实现(用于上一段为缓下坡，下一段也为缓下坡的缓坡策略)
  *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* last_dwn_next_dwn_gentle_hill_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) ;
/**
 * @brief  缓坡策略函数实现(用于一段为陡下坡的缓坡策略)
  *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* next_steep_up_gentle_hill_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) ;
#endif /* RAWSTRATEGY_H_ */
