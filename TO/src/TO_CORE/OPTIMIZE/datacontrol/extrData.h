/**
 *@file  extrData.h
 *@brief 基础数据提取接口
 *@date  2014.4.1
 *@author  sinlly
 *@note  基础数据包括（线路数据、机车数据、策略函数、列车时刻表、编组信息等）
 */

#ifndef EXTRDATA_H_
#define EXTRDATA_H_

#include <stdio.h>
#include <stdlib.h>
#include "extrData.h"
#include <mxml.h>
#include "opt_definition.h"
#include <string.h>
#include "exception.h"
#include "read_binary_for_route.h"
#include "xml_extract.h"
#include "optimizeTools.h"
/**
 * @brief  读取线路数据，机车数据等基本数据
 * @return  文件读取异常结果
 */
OPT_EXTR_RESULT extrData();
/**
 *  @brief 读取用于与在线优化比较的司机运行数据
 *  @return 文件读取异常结果
 */
OPT_EXTR_RESULT extrInitialData(void);
/**
 * @brief 读取原始优化策略矩阵
 * @return  文件读取异常结果
 */
OPT_EXTR_RESULT extrRawStrategyMetrics(void);
/**
 * @brief 读取时间偏差调整策略矩阵
 * @return  文件读取异常结果
 */
OPT_EXTR_RESULT  extrTimeStrategyMetrics(void);

/**
 * @brief 读取限速偏差调整策略矩阵
 * @return  文件读取异常结果
 */
OPT_EXTR_RESULT  extrLimitStrategyMetrics(void);
/**
 * @brief 读取列车时刻表相关信息
 * @return  文件读取异常结果
 */
OPT_EXTR_RESULT extrRoadTimetable();
/**
 * @brief 读取手动区域的配置信息（包括贯通实验和补机）
 * @return  文件读取异常结果
 */
OPT_EXTR_RESULT extrHandAreaInfo();
/**
 *@brief  打开xml文件
 *@param  fileName 文件名称
 *@param  permission  权限控制
 *@return  xml文件根节点
 */
mxml_node_t*  openXML(char* fileName,char* permission);
/**
 * @brief 关闭xml文件
 * @param  root xml文件根节点
 */
void closeXML(mxml_node_t* root);

#endif /* EXTRDATA_H_ */
