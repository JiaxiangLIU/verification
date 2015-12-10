
#include "threadpool.h"
#include "global.h"
#include "realtime_data.h"
#include "core_init.h"
#include "rt_definition.h"
void rt_main();

// jiaxiang: 这两个变量是什么含义？
RT_ARR_FLAG rt_arr_flag = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0};
RT_COMM rt_comm;

int auto_control()
{
    if (threadpool_add(pool, &rt_main, NULL, 0) != 0){
        return -1;
    }
    return 0;    
}
