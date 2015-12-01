#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include "zlog.h"


extern int zlog_rc;
extern zlog_category_t *zlog_zc; 

int  log_init(const char *conf_name, const char *cat_name);
void log_fini(void);
#if LOG_DEBUG
#define log_info(format, ...)\
    do {\
    \
        zlog_info(zlog_zc, format, ##__VA_ARGS__);\
    } while(0)

#define log_error(format, ...)\
    do{\
    \
        zlog_error(zlog_zc, format, ##__VA_ARGS__);\
    }while(0)

#define log_warning(format, ...)\
    do{\
    \
        zlog_warn(zlog_zc, format, ##__VA_ARGS__);\
    }while(0)

#define log_debug(format, ...)\
    do{\
    \
        zlog_debug(zlog_zc, format, ##__VA_ARGS__);\
    }while(0)
#endif
#if !LOG_DEBUG
#define opt_log_info(format, ...) 
#define opt_log_error(format, ...) 
#define opt_log_debug(format, ...) 
#define opt_log_warning(format, ...) 

//#define opt_log_info(format, ...) log_info(format, ...) 
//#define opt_log_error(format, ...) log_error(format, ...)
//#define opt_log_debug(format, ...) log_debug(format, ...)
//#define opt_log_warning(format, ...) log_warning(format, ...)
#endif
#endif
