/**
 * @file     base_opt_fuc.h
 * @brief  优化相关机车运行子操作函数接口
 * @date  2014.7.23
 * @author sinlly
 * @note  将优化策略分解成若干个操作步骤
 */
#ifndef BASE_OPT_FUC_H_
#define BASE_OPT_FUC_H_
#include "opt_global_var.h"
#include "optimizeTools.h"
#include "tractionCaculator.h"
#include "exception.h"
/**
 * @brief  以动态的gear牵引（保证曲线的速度一直递增的档位）直到速度为Vi或到达段末或到达最大限速值
 * @param  startGear  初始档位
 * @param  rcIndex   所在段的index
 * @param limitV  最大限速值
 * @param  localIndex  当前优化的初始index
 * @param   flag  表示是结束还是v已经到达vi
 * @param   localCount  用于查找当前位置属于某个段的起始roadIndex
 * @param   optSch  优化结果曲线
 * @return  优化异常结果
 */
OPTIMIZE_RESULT  dyn_gr_trac_til_vi_or_ed_or_lmt(int startGear, int rcIndex , float limitV,int* localIndex, int* flag,int* localCount,OPTSCH** optSch);
/**
 *@brief 以某个档位牵引直到速度大于limit或达到段末
 *@param  startGear   初始档位
 *@param   rcIndex  所在段的Index
 *@param   limitV  限速大小
 *@param   localIndex  当前优化的初始index
 *@param   flag  是否超出限速标识
 *@param   localCount  用于查找当前位置属于某个段的起始roadIndex
 *@param   optSch  优化结果曲线
 *@return 优化异常结果
 */
OPTIMIZE_RESULT gr_trac_til_ed_or_lmt(int startGear ,int rcIndex, float limitV, float minV,int* localIndex, int* flag,int* localCount, OPTSCH** optSch);
/**
 *@brief  处理空气制动
 *@param   rcIndex  所在段的index
 *@param   localIndex  当前优化的初始index
 *@param   optSch  优化结果曲线
 *@return 优化异常结果
 */
OPTIMIZE_RESULT air_brake_op( int rcIndex , int*localIndex, OPTSCH** optSch);
/**
 *@brief   牵引机车使其保持某个平均速度上下浮动，默认先往下浮动
 *@param   startGear  初始档位
 *@param   avgV  需要保持的平均速度
 *@param   v_internal  保持平均速度上下浮动的阈值
 *@param   rcIndex   所在段的index
 *@param   localIndex   当前优化的初始index
 *@param   localCount  用于查找当前位置属于某个段的起始roadIndex
 *@param   optSch   优化结果曲线
 *@return 优化异常结果
 */
OPTIMIZE_RESULT constv_trac_til_ed(int startGear,float avgV, float v_internal,int rcIndex, int* localIndex, int* localCount,OPTSCH** optSch);
/**
 *@brief  从段末进行反求，直到同原曲线相交为止
 *@param   startV  反求的初始速度
 *@param   gear  进行反求所采用的档位
 *@param   rcIndex  所在段的index
 *@param   categoryStartIndex   段首所在optSch中的index
 *@param   localIndex  当前优化的初始index
 *@param   flag  标志位（0 表示与曲线有交点且交点位置在段内；1 表示与曲线在当前段内没有交点
 *@param   localCount   用于查找当前位置属于某个段的起始roadIndex
 *@param   optSch  优化结果曲线
 *@param   backOpt 反求的优化曲线片段
 *@param   backLength 反求的优化曲线片段长度
 *@return 优化异常结果
 */
OPTIMIZE_RESULT  seg_ed_rvrs_cal(float startV, int gear, int rcIndex,int categoryStartIndex,int* localIndex, int* flag, int* localCount,OPTSCH** optSch, OPTSCH** backOpt, int* backLength);
/**
 * @brief 从起始位置通过某个档位牵引直到段末
 * @param startGear 起始的gear
 * @param   rcIndex  所在段的index
 * @param  localIndex 当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  optSch 优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  gr_trac_til_ed(int startGear, int rcIndex, int* localIndex,int* localCount, OPTSCH** optSch);
/**
 *  @brief 将两个曲线片段连接在一起（后段desc连接到前段src，在实际中是将反求的曲线片段连接到原始曲线中
 *  @param src  已经优化的曲线
 *  @param  srcIndex  已经优化的点的index
 *  @param  markIndex  之前反求与原曲线交汇处的index
 *  @param  desc  反求之后的曲线片段
 *  @param  descLength  反求之后的曲线片段长度
 *  @return 优化异常结果
 */
OPTIMIZE_RESULT conct_two_seg(OPTSCH** src, int* srcIndex,  int markIndex, OPTSCH**  desc, int descLength);
/**
 * @brief 以动态的gear（保证曲线的速度一直递增的档位）牵引直到速度到达了vo或者超过了某一个限速或者已经行驶到段末
 * @param  startGear  初始档位
 * @param  vo  段末的vo
 * @param  rcIndex  所在段的index
 * @param  localIndex  当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  limit 某一限速（可能为最大限速也可能为最小限速)
 * @param  flag 表示速度是到达了vo或者是到达了Limit或者是行驶到了段末
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT   dyn_gr_trac_til_vo_or_ed_or_lmt(int startGear, float vo,int rcIndex , int* localIndex, int* localCount, float limit, int* flag,OPTSCH** optSch);
/**
 * @brief 以动态的gear（保证曲线的速度一直递增的档位）牵引直到速度到达了vo或者超过了某一个限速或者已经行驶到段末
 * @param  startGear  初始档位
 * @param  rcIndex  所在段的index
 * @param  vo  段末的vo
 * @param  localIndex  当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  limit 某一限速（可能为最大限速也可能为最小限速)
 * @param  flag 表示速度是到达了vo或者是到达了Limit或者是行驶到了段末
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT db_dyn_gr_til_vo_or_ed_or_lmt(int startGear,float vo, int rcIndex , int* localIndex, int* localCount, float limit, int* flag,OPTSCH** optSch);
/**
 * @brief 以某个档位制动直到到达某个限速或者已经行驶到段末
 * @param  startGear  初始档位
 * @param  rcIndex  所在段的index
 * @param  localIndex  当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  limit  某一限速（可能为最大限速也可能为最小限速）
 * @param  flag  表示速度是到达了某个limit或者是行驶到了段末(0表示到段末，1表示到达某个limit)
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT   db_til_lmt_or_ed(int startGear, int rcIndex, int * localIndex,int* localCount, float limit, int* flag, OPTSCH** optSch);
/**
 * @brief 以某个档位制动直到行驶到段末
 * @param  startGear  初始档位
 * @param  rcIndex  所在段的index
 * @param  localIndex  当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  db_gr_til_ed(int startGear, int rcIndex, int* localIndex,int* localCount, OPTSCH** optSch);
/**
 * @brief 从段末反求同原曲线相交或者公里标已经到达某个mark的位置
 * @param  startV  反求的初始速度
 * @param  gear  反求的gear
 * @param  rcIndex  所在段的index
 * @param    localIndex  当前优化的初始index
 * @param    localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param    flag  标志位（0：表示与曲线在段内有交点，1：表示同曲线在段内没有交点）
 * @param    optSch  优化结果曲线
 * @param    backOpt  反求的优化曲线片段
 * @param    backLength  反求的优化曲线片段长度
 * @param    mark  某个mark位置（公里标）
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  seg_ed_rvrs_cal_til_mark(float startV,int gear, int rcIndex, int* localIndex, int* localCount, int* flag, OPTSCH** optSch, OPTSCH** backOpt, int* backLength, float mark);
/**
 * @brief 从某个位置开始反求同曲线有交点则退出（只需要有交点，无需其他位置）
 * @param gear  反求采用的档位
 * @param  localIndex  反求的起始index(optSch中的index)
 * @param localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param backLength  反求的曲线片段的长度
 * @param flag：是否追上原始曲线（同原始曲线有交点）的flag
 * @param optSch  优化速度曲线
 * @param backOpt 反求的曲线片段
 * @return 优化异常结果
 */
OPTIMIZE_RESULT rvrs_cal_til_inctersect(int gear, int *localIndex, int*localCount, int* backLength, int* flag, int rcIndex,OPTSCH** optSch,OPTSCH** backOpt);
/**
 * @brief 通过某个档位牵引曲线至某个平均速度，然后保持该平均速度行驶到段末
 * @param gear  牵引到平均速度的档位
 * @param avgV  需要保持的平均速度
 * @param rcIndex  所在段的index
 * @param  localIndex 当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  flag   是否低于匀速
 * @param  gearup  低于匀速之后让其速度上升的档位
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  gr_trac_to_avgv_till_ed(int gear, float avgV, int rcIndex,float v_internal,  int * localIndex,int* localCount,  int* flag, OPTSCH** optSch);
/**
 * @brief 通过某个档位牵引曲线至某个平均速度，然后保持该平均速度行驶到段末, 整个过程中，不使用制动档位
 * @param gear  牵引到平均速度的档位
 * @param avgV  需要保持的平均速度
 * @param rcIndex  所在段的index
 * @param  localIndex 当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  flag   是否低于匀速
 * @param  gearup  低于匀速之后让其速度上升的档位
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  gr_trac_to_avgv_till_ed_no_db_gear(int gear, float avgV, int rcIndex,float  v_internal,  int * localIndex,int* localCount,  int* flag, OPTSCH** optSch);
/**
 * @brief 当速度低于平均速度-阈值时，先后交替up和down档位运行，直到速度高于平均速度+阈值
 * @param gearUp  向上牵引档位
 * @param gearDown  向下制动档位
 * @param localIndex  当前优化的初始index
 * @param localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  flag  标志位，表示需要牵引或者制动（1：表示制动，-1表示牵引）
 * @param  v_internal  匀速允许的上下起伏的阈值
 * @param  avgV 需要保持的平均速度
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT constv_gr_down_up_til_end(float start,float startV,  int gearup, int geardown,int rcIndex,int *localIndex,int* localCount,  int* flag,float  v_internal, float avgV, OPTSCH** optSch);
/**
 * @brief 当速度低于平均速度-阈值时，先后交替up和down档位运行，直到速度高于平均速度+阈值；整个过程中不使用制动档位
 * @param gearUp  向上牵引档位
 * @param gearDown  向下制动档位
 * @param localIndex  当前优化的初始index
 * @param localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  flag  标志位，表示需要牵引或者制动（1：表示制动，-1表示牵引）
 * @param  v_internal  匀速允许的上下起伏的阈值
 * @param  avgV 需要保持的平均速度
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT constv_gr_down_up_til_end_no_db_gear(float start,float startV,  int gearup, int geardown,int rcIndex,int *localIndex,int* localCount,  int* flag,float  v_internal, float avgV, OPTSCH** optSch);
/**
 *  @brief 根据速度与平均速度的差别获得向下制动（速度递减）和向上牵引（速度递增）的档位
 *  @param  start  初始位置/结束后为最终的位置
 *  @param  startV 初始速度/结束后为最终的速度
 *  @param  avgV  平均速度
 *  @param  localCount   用于查找当前位置属于某个段的起始roadIndex
 *  @param  flag  标志位，表示需要牵引或者制动（1：表示制动，-1表示牵引）
 *  @param  gearup  向上牵引的档位
 *  @param  geardown 向下制动的档位
 *  @return 优化异常结果
 */
OPTIMIZE_RESULT  get_avg_up_down_gear(float* start,float* startV, float avgV,int *localCount,int *flag,int* gearup,int* geardown);
/**
 *  @brief 根据速度与平均速度的差别获得向下制动（速度递减）和向上牵引（速度递增）的档位；整个过程中不使用制动档位
 *  @param  start  初始位置/结束后为最终的位置
 *  @param  startV 初始速度/结束后为最终的速度
 *  @param  avgV  平均速度
 *  @param  localCount   用于查找当前位置属于某个段的起始roadIndex
 *  @param  flag  标志位，表示需要牵引或者制动（1：表示制动，-1表示牵引）
 *  @param  gearup  向上牵引的档位
 *  @param  geardown 向下制动的档位
 *  @return 优化异常结果
 */
OPTIMIZE_RESULT get_avg_up_down_gear_no_db_gear(float* start,float* startV, float avgV,int *localCount,int *flag,int* gearup,int* geardown);
/**
 * @brief 从段末以某个速度和档位反求，直到和原曲线有在段内有交点或者到某个mark位置仍旧没有交点结束
 * @param startV  初始速度
 * @param gear  初始档位
 * @param rcIndex  所在段的index
 * @param localIndex  当前优化的初始index
 * @param localCount  用于查找当前位置属于某个段的起始roadIndex
 * @param optSch  优化结果曲线
 * @param  backOpt  反求的曲线片段
 * @param  backLength  反求的曲线片段长度
 * @param mark   某个标记位置
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  seg_ed_rvrs_cal_til_mark_or_inter(float startV,int gear, int rcIndex,int* localIndex, int* flag, int* localCount,OPTSCH** optSch, OPTSCH** backOpt, int* backLength,float mark);
/**
 * @brief  以动态的gear牵引（保证曲线的速度一直递增的档位）直到速度为Vi或到达指定距离
 * @param  startGear  初始档位
 * @param  rcIndex   所在段的index
 * @param  dis       指定距离
 * @param  localIndex  当前优化的初始index
 * @param   flag  111标志 到段末且速度未异常101标识 走到指定距离且速度正常 100标志 为走到指定距离距离且速度达到限制(101与100其实可以合并)
 * @param   localCount  用于查找当前位置属于某个段的起始roadIndex
 * @param   optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  dyn_gr_trac_til_vi_or_ed_or_dis(int startGear,int rcIndex,float dis,float limitV,int* localIndex, int* flag,int* localCount,OPTSCH** optSch);
/**
 * @brief 仅保存该大小的数组
 * @param src 源曲线
 * @param des 目标曲线
 * @param length  复制的长度大小
 * @return copy之后索引到的index
 */
int copy_optsch(OPTSCH* src,OPTSCH* des,int length);
/**
 * 仅将原来策略稍微做了一下修改，将到段末改为指定距离。
 */
OPTIMIZE_RESULT gr_trac_til_dis_or_lmt(int startGear ,int rcIndex,float dis, float limitV, float minV, int* localIndex, int* flag,int* localCount, OPTSCH** optSch);
#endif /* BASE_OPT_FUC_H_ */
