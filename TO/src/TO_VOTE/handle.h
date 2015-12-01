
/**
 * @file handle.h
 * @brief 事件处理函数定义
 * @author danny
 * @date 2015.01.29
 **/ 

#ifndef HANDLE_H_
#define HANDLE_H_

#include <stdint.h>

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

void rt_handle(char *data, int length);
#endif
