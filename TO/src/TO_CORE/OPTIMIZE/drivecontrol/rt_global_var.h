/**
 * @file     globalvar.h
 * @brief  全局变量的声明
 * @date  2014.3.31
 * @author sinlly
 */
#ifndef RTGLOBALVAR_H_
#define RTGLOBALVAR_H_
#include "opt_definition.h"
#include "opt_global_var.h"
#include "rt_definition.h"
#include "inter_rtopt_definition.h"
#include "rt_global_var.h"
#include <stdio.h>
#include <stdlib.h>
#include "exception.h"
#include "xml_extract.h"


/*
 * declaration of the linked list
 */
extern LL_NODE* ll_head;  /**<head pointer of the linked list, with NULL data, just a label for remarking the start pointer of the linked list*/
extern LL_NODE* ll_current;  /**<current pointer of the linked list, already processed, current->next node is always the one that you are going to process*/
extern LL_NODE* ll_last; /**the one just be processed*/
extern int ll_flag; //is there temp opt in linked list? yes = 1, no = 0.

//extern int rt_delta_ahead = 600; //how far we look forward

#define LL_GET_DISTANCE 50 //set allowable distance for get the temp opt result
#define RT_SLEEP_USEC 200 //set sleep time as 200 msec
//#define MAX_GEAR  8;
#define DELTA_AHEAD 600;
//#define BOOL int
#define TRUE 1
#define FALSE 0

#define TESTING 0	//1 for test; 0 for core_board using

#define ENABLE_LOG_INFO 0	//1 for recording; 0 for non-recording
#define ENABLE_LOG_DEBUG 0	//1 for recording; 0 for non-recording
#define ENABLE_LOG_WARNING 0	//1 for recording; 0 for non-recording
#define ENABLE_LOG_ERROR 0	 //1 for recording; 0 for non-recording
#define ENABLE_LOG_PRINTF 0	 //1 for recording; 0 for non-recording


#endif /* GLOBALVAR_H_ */
