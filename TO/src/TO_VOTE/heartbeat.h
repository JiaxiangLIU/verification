#ifndef HEART_H
#define HEART_H

#include "global.h"
#include "protocal.h"
#include "threadpool.h"
#include "log.h"
#include "comm_check.h"
#include "abnormal_code.h"

#define SOCKET_HEART 0x20

/*开始心跳函数，每隔3s由指定源发往指定板 心跳
 * 在注册以后，调用该函数就可以，
 * src 源，与protocal.h相同，比如 通信板0 写0x00
 *          COM_0_SOURCE  0x00
            COM_1_SOURCE  0x01
            CORE_0_SOURCE 0x02
            CORE_1_SOURCE 0x03
            CORE_2_SOURCE 0x04
            CORE_3_SOURCE 0x05
            VOTE_0_SOURCE 0x08
            CONTROL_0_SOURCE  0x06
            CONTROL_1_SOURCE  0x07
            INPUT_0_SOURCE    0x09
            OUTPUT_0_SOURCE   0x10
 *
 * 目的地址des，对应表如下
 * CON_0_DES  0x02
   //CON_1_DES  0x04
   COM_0_DES  0x200
   COM_1_DES  0x100
   CORE_0_DES 0x80
   CORE_1_DES 0x40
   CORE_2_DES 0x20
   CORE_3_DES 0x10
   VOTE_0_DES 0x08
 */
void start_heartbeat(char src,char des);


#endif

