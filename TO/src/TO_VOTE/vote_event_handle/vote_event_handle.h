
/**
 * @file vote_event_handle.h
 * @brief 表决版事件处理定义
 * @author liuyan
 * @date 2015.8.25
 **/

#ifndef VOTE_EVENT_HANDLE_VOTE_EVENT_HANDLE_H_
#define VOTE_EVENT_HANDLE_VOTE_EVENT_HANDLE_H_
#include "circ_buf.h"
#include <to_event.h>
#define HIGH_GEAR 2


void auto_to_manual();
void manual_to_auto();
void route_init_start();
void auto_test();
void mannual_test();
#endif /* VOTE_EVENT_HANDLE_VOTE_EVENT_HANDLE_H_ */
