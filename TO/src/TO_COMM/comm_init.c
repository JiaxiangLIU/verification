

#include "comm_init.h"
#include "global.h"
#include "threadpool.h"
#include "log.h"
#include "comm_check.h"

int comm_check_init(void) {
    if (threadpool_add(pool, &heartbeat_check, NULL, 0) != 0) {
        log_error("thread add err");
        return -1;
    }
    return 0;
}
