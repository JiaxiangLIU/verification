
/**
 * @file handle.h
 * @brief 事件处理函数定义
 * @author danny
 * @date 2015.01.29
 **/ 

#ifndef HANDLE_H_
#define HANDLE_H_

#include <stdint.h>
#include "handle.h"

/**
 * @function route_init
 * @brief 接收到行程初始化命令后执行的函数
 * @param arg 从命令中提取的参数信息
 * @note 动作:1. 对行程信息进行校验
 *            2. 判断是否需要进行交路提取
 *            3. 进行交路提取
 *            4. 将交路提取的结果发送给核心板
 *            5. 完成事件
 **/
void route_init(void *arg);

/**
 * @function command_frame_rcv_handle
 * @brief 进行命令帧接收后的处理
 * @param command 命令参数
 **/ 
void command_frame_rcv_handle(void *cmd);

/**
 * @function data_frame_rcv_handle
 * @brief
 **/
void data_frame_rcv_handle(char *data, uint8_t length, char *command, int amount, int count);
#endif
