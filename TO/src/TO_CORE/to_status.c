#include <pthread.h>
#include "to_status.h"

/* 设置 获取 系统状态 */
static pthread_mutex_t sys_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static SYSTEM_STATUS sys_status = SYS_INIT;

void set_sys_status(SYSTEM_STATUS status)
{
    pthread_mutex_lock(&sys_status_mutex);
    sys_status = status;
    pthread_mutex_unlock(&sys_status_mutex);
}

SYSTEM_STATUS get_sys_status(void)
{
    pthread_mutex_lock(&sys_status_mutex);
    SYSTEM_STATUS temp = sys_status;
    pthread_mutex_unlock(&sys_status_mutex);
    return temp;
}

/* 设置 获取 本补状态 */
static pthread_mutex_t train_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static TRAIN_STATUS train_status = TRAIN_DUTY;

void set_train_status(TRAIN_STATUS status)
{
    pthread_mutex_lock(&train_status_mutex);
    train_status = status;
    pthread_mutex_unlock(&train_status_mutex);
}

TRAIN_STATUS get_train_status(void)
{
    pthread_mutex_lock(&train_status_mutex);
    TRAIN_STATUS temp = train_status;
    pthread_mutex_unlock(&train_status_mutex);
    return temp;
}

/* 设置 获取 行程状态 */
static pthread_mutex_t trip_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static TRIP_STATUS trip_status = TRIP_DATA_RECVING;

void set_trip_status(TRIP_STATUS status)
{
    pthread_mutex_lock(&trip_status_mutex);
    trip_status = status;
    pthread_mutex_unlock(&trip_status_mutex);
}

TRIP_STATUS get_trip_status(void)
{
    pthread_mutex_lock(&trip_status_mutex);
    TRIP_STATUS temp = trip_status;
    pthread_mutex_unlock(&trip_status_mutex);
    return temp;
}

/* 设置 获取 操控状态 */
static pthread_mutex_t control_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static CONTROL_STATUS control_status = CONTROL_MAX;

void set_control_status(CONTROL_STATUS status)
{
    pthread_mutex_lock(&control_status_mutex);
    control_status = status;
    pthread_mutex_unlock(&control_status_mutex);
}

CONTROL_STATUS get_control_status(void)
{
    pthread_mutex_lock(&control_status_mutex);
    CONTROL_STATUS temp = control_status;
    pthread_mutex_unlock(&control_status_mutex);
    return temp;
}

/* 设置 获取 自动使能状态 */
static pthread_mutex_t auto_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static AUTO_STATUS auto_status = AUTO_MAX;

void set_auto_status(AUTO_STATUS status)
{
    pthread_mutex_lock(&auto_status_mutex);
    auto_status = status;
    pthread_mutex_unlock(&auto_status_mutex);
}

AUTO_STATUS get_auto_status(void)
{
    pthread_mutex_lock(&auto_status_mutex);
    AUTO_STATUS temp = auto_status;
    pthread_mutex_unlock(&auto_status_mutex);
    return temp;
}
