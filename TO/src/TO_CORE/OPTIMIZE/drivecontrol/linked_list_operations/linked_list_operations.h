/*
 * linked_list_operations.h
 *
 *  Created on: Jan 11, 2015
 *      Author: Jin
 */

#ifndef LINKED_LIST_OPERATIONS_H_
#define LINKED_LIST_OPERATIONS_H_

#include <string.h>
#include <stdlib.h>

//#include "dataDefinition.h"
//#include "globalvar.h"

#endif /* LINKED_LIST_OPERATIONS_H_ */


/**
 * @brief create linked list
 * @param none
 * @return the running status
 */
LL_RESULT ll_create();

/**
 * @brief insert data to linked list
 * @brief sort data by the operation order
 * @param data	the optimized result for temp speed limit
 * @return result	the running status
 */
LL_RESULT ll_insert(OPTIMIZERESULT* data, int data_length);

/**
 * @brief delete the whole linked list, free memory
 * @param p_node the starting node of the linked list
 * @return the running status
 */
LL_RESULT ll_delete(LL_NODE* p_node);
