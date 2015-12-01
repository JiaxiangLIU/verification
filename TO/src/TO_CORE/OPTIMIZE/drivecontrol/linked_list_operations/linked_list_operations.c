/*
 * linked_list_operations.c
 *
 *  Created on: Jan 11, 2015
 *      Author: Jin
 */

#include <pthread.h>
#include "rt_global_var.h"
#include "optimizeTools.h"
#include "linked_list_operations.h"
#include "error_code.h"
#include "log.h"

LL_NODE* ll_head;  /**<head pointer of the linked list, with NULL data, just a label for remarking the start pointer of the linked list*/
LL_NODE* ll_current;  /**<current pointer of the linked list, already processed, current->next node is always the one that you are going to process*/
LL_NODE* ll_last; /**the one just passed*/
int ll_flag;

static pthread_mutex_t ll_mutex = PTHREAD_MUTEX_INITIALIZER;
static int ll_lock()
{
    return pthread_mutex_lock(&ll_mutex);
}
static int ll_unlock()
{
    return pthread_mutex_unlock(&ll_mutex);
}


LL_RESULT ll_create()
{
	// assign memorry to head
	if((ll_head = (LL_NODE*)malloc(sizeof(LL_NODE))) == NULL)
	{
//		fprintf(stderr, "system err: malloc failure.\n");
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_MALLOC));
		return LL_MEMORRY_FAILURE;
	}
	//assign memorry to the ll_current
	if((ll_current = (LL_NODE*)malloc(sizeof(LL_NODE))) == NULL)
	{
//		fprintf(stderr, "system err: malloc failure.\n");
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_MALLOC));
		return LL_MEMORRY_FAILURE;
	}
	//assign memorry to the ll_last
	if((ll_last = (LL_NODE*)malloc(sizeof(LL_NODE))) == NULL)
	{
//		fprintf(stderr, "system err: malloc failure.\n");
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_MALLOC));
		return LL_MEMORRY_FAILURE;
	}

	// initialize ll_head
	ll_head->data = NULL;
	ll_head->data_length = 0;
	ll_head->next = NULL;
	ll_current = ll_head; // initial *current
	ll_last = NULL;

	// assign *heads
	ll_head->data = NULL;
	ll_head->data_length = 0;
	ll_head->next = NULL;

	// initial *current
	ll_current = ll_head;
	ll_last = NULL;

	// initial ll_flag
	ll_flag = 0;

	log_info("linked list initialization success.\n");
	return LL_SUCCESS;
}


LL_RESULT ll_insert(OPTIMIZERESULT* data, int data_length)
{
	LL_NODE* p_node; /*temp node*/
	LL_NODE* p_current; /*temp current node*/
	LL_NODE* p_next; /*temp next node*/
	OPTIMIZERESULT* data_current;
	OPTIMIZERESULT* data_after;
	int post_data, post_current, post_after; /*gong li biao*/

	if (NULL == data)
	{
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_PARAMETER));
		return LL_INPUT_DATA_NULL;
	}

	//ll_lock;
	if (ll_lock() != 0)
	{
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_MUTEX_LOCK));
		return LL_LOCK_FAILURE;
	}


	// assign data to p_node
	if((p_node = (LL_NODE*)malloc(sizeof(LL_NODE))) == NULL)
	{
//		fprintf(stderr, "system err: malloc failure.\n");
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_MALLOC));
		return LL_MEMORRY_FAILURE;
	}
	p_node->data = data;
	p_node->data_length = data_length;
	p_node->next = NULL;

	log_debug("linked list p_node create success\n");

	//if the first one, make it link to head
	if (ll_current == ll_head)
	{
		ll_head->next = p_node;
		log_debug("current node: ll_head, insert p_node after ll_head\n");
	}
	else
	{
		// check if the inserting data is priori to the current, insert it before ll_current
		data_current = ll_current->data;
		post_current = data_current->start;
		post_data = data->start;
		if (post_data < post_current)
		{
			ll_last->next = p_node;
			p_node->next = ll_current;
			ll_current = p_node;
			log_debug("inserting node priori to ll_current, insert p_node before ll_current\n");
		}
		else
		{
			// temp current copy
			p_current = ll_current;
			while(1)
			{
				// if the current is the last, add p_node to the last.
				if (p_current->next == NULL)
				{
					p_current->next = p_node;
					log_debug("current node: last one, insert p_node at the end\n");
					break;
				}
				else //if the current is not the last, insert p_node as in an sorted order by post(Gong Li Biao)
				{
					data_current = p_current->data; //get current opt data
					p_next = p_current->next; //set next linked node
					data_after = p_next->data; //get next opt data

					post_data = data->start; //the post of the data to be inserted
					post_current = data_current->start; // the temp current post
					post_after = data_after->start; // the temp next post

					// if we find the right place to insert the p_node, insert it and break;
					if (post_data >= post_current && post_data < post_after)
					{
						p_current->next = p_node;
						p_node->next = p_next;
						log_debug("insert p_node in (not at the end) the linked list in the order of post\n");
						break;
					}
					else
					{
						log_debug("trying to find the right post for inserting p_node\n");
						p_current = p_next;
					}
				}
			}
		}
	}

	// change ll_flag into 1 whenever insert action was called.
	ll_flag = 1;

	//ll_unlock;
	if (ll_unlock() != 0)
	{
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_MUTEX_UNLOCK));
		return LL_UNLOCK_FAILURE;
	}

	return LL_SUCCESS;
}

//LL_RESULT ll_check(LL_NODE* p_node)
//{
//	return LL_SUCCESS;
//}


//LL_RESULT ll_get()
//{
//	// if ll_current->next != NULL, get current data and set current to next
//	if (ll_current->next != NULL)
//	{
//		//ll_lock
//		if (ll_lock() != 0)
//		{
//			return LL_LOCK_FAILURE;
//		}
//
//		ll_last = ll_current;
//		ll_current = ll_current->next;
//
//		//set rt_check_temp_flag = 0 whenever a get done.
//		rt_check_temp_flag = 0;
//		ll_get_flag = 1;
//
//		//ll_unlock
//		if (ll_unlock() != 0)
//		{
//			return LL_UNLOCK_FAILURE;
//		}
//	}
//
//	return LL_SUCCESS;
//}


LL_RESULT ll_delete(LL_NODE* p_node)
{
	LL_NODE* p_next;

	//ll_lock
	if (ll_lock() != 0)
	{
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_MUTEX_LOCK));
		return LL_LOCK_FAILURE;
	}

	// free p_node till NULL
	while(p_node != NULL)
	{
		p_next = p_node->next;
		free(p_node);

		// p_node point to next
		p_node = p_next;
	}
	log_info("linked list memory free success.\n");

	//ll_unlock
	if (ll_unlock() != 0)
	{
		log_error("linked list error: %s\n", error_code_name(ERROR_CODE_MUTEX_UNLOCK));
		return LL_UNLOCK_FAILURE;
	}

	return LL_SUCCESS;
}






