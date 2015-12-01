/**
 * @file  globalvar.h
 * @brief  全局变量的声明
 * @date  2014.3.31
 * @author sinlly
 */
#ifndef GLOBALVAR_H_
#define GLOBALVAR_H_
#include "opt_definition.h"
#include "opt_global_var.h"
#include <stdio.h>
#include <stdlib.h>
#include "exception.h"
#include "xml_extract.h"
#include "log.h"

#define DELTAS 10 ///时间偏差调整中需要调整的位置距离段首/段末的距离
#define VTHSMALL 1 ///小速度阈值
#define VTHMEDIUM 3 ///中等速度阈值
#define VTHLARGE 5 ///大速度阈值
#define EXP_DESC_LENGTH 100 ///异常描述的长度

extern int MAX_GRADIENT; /**<坡度实际长度*/
extern int MAX_CURVE; /**<曲线实际长度*/
extern int MAX_LIMIT; /**<限速实际长度*/
extern int MAX_TELESEME; /**<信号机实际长度*/
extern int MAX_TUNNEL; /**<隧道实际长度*/
extern int MAX_MGRADIENT; /**<加算坡度实际长度*/
extern int MAX_STATION; /**<车站实际长度*/
extern int MAX_INTERVALRUNTIME; /**< 车站间时间单元长度*/
extern int optSchIndex;/**<optSch 可变的index  优化结果*/
extern int MAX_OPTSCHLENGTH;  /**<optSch 总长度  优化结果最终实际长度*/
extern int MAX_ROADCATEGORY; /**< 坡段实际长度*/
extern int MAX_RAWSTRATEGY; /**< 原始优化策略矩阵长度*/
extern int MAX_TIMESTRATEGY;/**<时间偏差调整策略矩阵长度*/
extern int MAX_LIMITSTRATEGY;/**<限速偏差调整策略矩阵长度*/
extern int MAX_GEDATA;/**<GE Data的行数最大值*/
extern int MAX_INITIALDATA; /**< 总共的车次数量,用于测试*/
extern int CARNUMS;/**<车厢节数*/
extern int MAX_FINAL_LENGTH;/**优化最终的速度曲线长度*/

extern GRADIENT*  gradients;  /**<坡度*/
extern CURVE*  curves; /**<曲线*/
extern LIMIT*  limits; /**<限速*/
extern TELESEME*  telesemes; /**<信号机*/
extern TUNNEL*  tunnels; /**<隧道*/
extern LOCOPARAMETER locoInfo; /**<机车基本信息*/
extern STATION* stations; /**<车站信息*/
extern MGRADIENT* mGradients; /**<计算坡度*/
extern INTERVALRUNTIMEUNIT* intervalRuntimeUnits; /**<车站间时间单元信息*/
extern ROADCATEGORY*  rc; /**<坡段信息*/
extern RAWSTRFLAGS* rawStrategyMetrics; /**<原始优化策略矩阵*/
extern TIMESTRFLAGS* timeStrategyMetrics;/**<时间偏差调整策略矩阵*/
extern OPTIMIZERESULT* optimizeResult;/**<优化最终结果*/
extern LIMITSTRFLAGS* limitStrategyMetrics; /**<限速偏差调整策略矩阵*/
extern GEDATA* geDatas;
extern INITIALDATA* initialDatas;
extern CARSTRUCT* carStruct;/**<保存每个车厢的基本信息*/
extern OPTSCHCURVE* opt_final_curves;/**<最终给控制的优化结果（公里标+越过距离）*/
extern  OPTSCH* new_final_curve;/**<最终生成的优化速度曲线*/
extern OPTCONSTPARAM opt_const;/**<优化中使用到的常量*/
extern HANDAREA throughExpert;/**<贯通实验*/
extern PUSHERENGINE pusherEngines;/**<手动区域非贯通实验段(补机)段*/
extern LOCOMARSHALLING locoMarshInfo;/**<外部传入的机车编组信息*/

extern float dragForce[8][120]; /**<牵引档位油耗*/
extern float brakeForce[8][120]; /**<制动档位油耗*/
extern int mOffset; /**<mGradients偏移值*/
extern int mDirection; /**<mGradients方向*/
extern float totalAvgV; /**<总平均速度*/
extern float timeLimit; /**<时间偏差*/

#endif /* GLOBALVAR_H_ */
