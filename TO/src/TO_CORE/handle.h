
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
 * @brief 进行行程初始化相关流程操作
 * @param arg 行程初始化参数
 * @return void
 **/
void route_init(void *arg);

/**
 * @function opt_init
 * @brief 优化入口函数
 **/ 
int opt_init(void);
/**
 * @function command_frame_rcv_handle
 * @brief 进行命令帧接收后的处理
 * @param command 命令参数
 **/ 
void command_frame_rcv_handle(void *cmd);

/**
 * @function data_frame_rcv_handle
 * @brief 接收数据帧后的处理
 * @param data 数据
 * @param length 数据长度
 * @param command 命令
 * @param amount 帧总数
 * @param count 当前计数
 * @return 
 **/ 
void data_frame_rcv_handle(char *data, uint16_t length, char *command, int amount, int count);
#endif
