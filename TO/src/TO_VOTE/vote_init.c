
/**
 * @file vote_init.c
 * @brief 表决板初始化
 * @author danny
 **/ 

#include "vote_init.h"
#include "global.h"
#include "protocal.h"
#include "threadpool.h"
#include "comm_check.h"
#include "log.h"

/*
int route_init(void) 
{    
    char command[2];
    command[0] = 0x20;
    command[1] = 0x10;
    char arg = 0x00; 
    PROTOCAL_RESULT res = frame_encap(0x20, 0x200, COMMAND_FRAME_CODE, command, &arg, 1);
    if (res != PROTOCAL_SUCCESS)
    {
        log_error("frame encape err.");        
    }
    log_info("frame encape success.");        
    return res;
}
*/

int comm_check_init(void) {
    /* 初始化通信校验 */
    comm_check();
    /* 启动心跳包检查线程 */ 
#if 0
    if (threadpool_add(pool, &heartbeat_check, NULL, 0) != 0) {
        log_error("thread add err.");
        return -1;
    }
#endif
    return 0;
}

