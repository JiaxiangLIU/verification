#include <stdio.h>
#include <stdlib.h>
#include "error_code.h"
//
static ERROR_DATA app_error_code_names[] = {
    {ERROR_CODE_OTHER, "Other"},
	{ERROR_CODE_FD_PARAMETER, "File Description Error"},
	{ERROR_CODE_PARAMETER, "Parameter Out Of Range"},
	{ERROR_CODE_POINTER_NULL, "Illegal Pointer"},
    {ERROR_CODE_BUFFER_OVERFLOW, "Buffer Overflow"},
	{ERROR_CODE_BUFFER_EMPTY, "Buffer Empty"},
	{ERROR_CODE_CRC, "Protocol Data CRC Error"},
	{ERROR_CODE_DEST_ADDR, "Destination Address Error"},
	{ERROR_CODE_SRC_ADDR, "Source Address Error"},
	{ERROR_CODE_PROTOCOL_HEADER, "Protocol Header Error"},
	{ERROR_CODE_DATA_TOO_LONG, "Protocol Data Length Is Too Long"},
    {ERROR_CODE_UNRECOGNIZED_SERVICE, "Unrecognized Protocol Service"},
	{ERROR_CODE_SOCKET_CONNECT, "Socket Connect Failure"},
	{ERROR_CODE_SOCKET_REGISTER, "Socket Register Failure"},
	{ERROR_CODE_SOCKET_RECV, "Socket Receive Data Failure"},
	{ERROR_CODE_SOCKET_SEND, "Socket Send Data Failure"},
	{ERROR_CODE_OPEN_FILE, "Open File Failure"},
	{ERROR_CODE_WRITE_FILE, "Write File Failure"},
	{ERROR_CODE_READ_FILE, "Read File Failure"},
	{ERROR_CODE_CON_INIT, "Thread Condition Initialization Failure"},
	{ERROR_CODE_MUTEX_INIT, "Thread Mutex Initialization Failure"},
	{ERROR_CODE_MUTEX_LOCK, "Thread Mutex Lock Failure"},
	{ERROR_CODE_MUTEX_UNLOCK, "Thread Mutex Unlock Failure"},
	{ERROR_CODE_PTHREAD_CREATE, "Create Thread Failure"},
	{ERROR_CODE_MALLOC, "Malloc Memory Failure"},
	{ERROR_CODE_LOGICAL_CONDITION_1, "Logical Condition 1 Error"},
	{ERROR_CODE_LOGICAL_CONDITION_2, "Logical Condition 2 Error"},
	{ERROR_CODE_LOGICAL_CONDITION_3, "Logical Condition 3 Error"},
	{ERROR_CODE_LOGICAL_CONDITION_4, "Logical Condition 4 Error"},
	{ERROR_CODE_LOGICAL_CONDITION_5, "Logical Condition 5 Error"},
	{ERROR_CODE_LOGICAL_CONDITION_6, "Logical Condition 6 Error"},
	{ERROR_CODE_LOGICAL_CONDITION_7, "Logical Condition 7 Error"},
	{ERROR_CODE_LOGICAL_CONDITION_8, "Logical Condition 8 Error"},
	{ERROR_CODE_RECV_TIMEOUT, "Receive Data Timeout"},
	{ERROR_CODE_RETRANSMISSION, "Retransmission Failure"},
	{ERROR_CODE_COMMU_NO_REPLY, "Communication No Reply"},
	{ERROR_CODE_VOTE, "Vote Failure"},
	{ERROR_CODE_AUTO, "Auto Control Failure"},
	{ERROR_CODE_MANUAL, "Manual Control Failure"},
	{ERROR_CODE_OPTIMIZE, "Optimize Failure"},
	
	/* TODO: Add your error code number maching text. */
	
    {0, NULL}
};

static const char *reserved_string = "Reserved For Use By TO Group";

static const char *get_error_string_by_index(ERROR_DATA *data_list, ERROR_CODE index, const char *default_string)
{
    const char *string = NULL;

    if (data_list) {
        while (data_list->string) {
            if (data_list->index == index) {
                string = data_list->string;
                break;
            }
            data_list++;
        }
    }

    return string ? string : default_string;
}

const char *error_code_name(ERROR_CODE index)
{
    return get_error_string_by_index(app_error_code_names, index, reserved_string);
}

/*********************************************************************

app example: 
printf("app error: %s\n", error_code_name(ERROR_CODE_BUFFER_OVERFLOW)); 

*********************************************************************/
