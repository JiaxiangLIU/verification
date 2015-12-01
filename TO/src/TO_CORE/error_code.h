#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__


typedef enum {
    ERROR_CODE_MIN = -41,
	
	/* TODO: Add your error code number, Don't forget to change ERROR_CODE_MIN value. */
	
	ERROR_CODE_OPTIMIZE = -40,
	ERROR_CODE_MANUAL = -39,
	ERROR_CODE_AUTO = -38,
	ERROR_CODE_VOTE = -37,
	ERROR_CODE_COMMU_NO_REPLY = -36,
	ERROR_CODE_RETRANSMISSION = -35,
	ERROR_CODE_RECV_TIMEOUT = -34,
	ERROR_CODE_LOGICAL_CONDITION_8 = -33,
	ERROR_CODE_LOGICAL_CONDITION_7 = -32,
	ERROR_CODE_LOGICAL_CONDITION_6 = -31,
	ERROR_CODE_LOGICAL_CONDITION_5 = -30,
	ERROR_CODE_LOGICAL_CONDITION_4 = -29,
	ERROR_CODE_LOGICAL_CONDITION_3 = -28,
	ERROR_CODE_LOGICAL_CONDITION_2 = -27,
	ERROR_CODE_LOGICAL_CONDITION_1 = -26,
	ERROR_CODE_MALLOC = -25,
	ERROR_CODE_PTHREAD_CREATE = -24,
	ERROR_CODE_MUTEX_UNLOCK = -23,
	ERROR_CODE_MUTEX_LOCK = -22,
	ERROR_CODE_MUTEX_INIT = -21,
	ERROR_CODE_CON_INIT = -20,
	ERROR_CODE_READ_FILE = -19,
	ERROR_CODE_WRITE_FILE = -18,
	ERROR_CODE_OPEN_FILE = -17,
	ERROR_CODE_SOCKET_SEND = -16,
	ERROR_CODE_SOCKET_RECV = -15,
	ERROR_CODE_SOCKET_REGISTER = -14,
	ERROR_CODE_SOCKET_CONNECT = -13,
	ERROR_CODE_UNRECOGNIZED_SERVICE = -12,
	ERROR_CODE_DATA_TOO_LONG = -11,
	ERROR_CODE_PROTOCOL_HEADER = -10,
	ERROR_CODE_SRC_ADDR = -9,
	ERROR_CODE_DEST_ADDR = -8,
	ERROR_CODE_CRC = -7,
	ERROR_CODE_BUFFER_EMPTY = -6,
	ERROR_CODE_BUFFER_OVERFLOW = -5,
	ERROR_CODE_POINTER_NULL = -4,
	ERROR_CODE_PARAMETER = -3,
	ERROR_CODE_FD_PARAMETER = -2,
	ERROR_CODE_OTHER = -1
} ERROR_CODE;

typedef struct {
    ERROR_CODE index;     /* index number that matches the text */
    const char *string;   /* use NULL to end the list */
} ERROR_DATA;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	const char *error_code_name(ERROR_CODE index);

	/*********************************************************************

	app example: 
	printf("app error: %s\n", error_code_name(ERROR_CODE_BUFFER_OVERFLOW)); 

	*********************************************************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
