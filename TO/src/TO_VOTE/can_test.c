

#include "threadpool.h"
#include "protocal.h"

void can_test(){
    char cmd[2];
    char arg[10];
    cmd[0] = 0x00;
    cmd[1] = 0x00;

    arg[0] = 0xff;
    arg[1] = 0xff;
    arg[2] = 0xff;
    arg[3] = 0xff;
    arg[4] = 0xff;
    arg[5] = 0xff;
    arg[6] = 0xff;
    arg[7] = 0xff;
    arg[8] = 0xff;
    arg[9] = 0x00;
    while(1){
        frame_encap(0x40, 0x001, DATA_FRAME_CODE, cmd, arg, 10);
        arg[9]++;
        if (arg[9] > 8)
            arg[9] = 0;
        sleep(1);
    }    
}

