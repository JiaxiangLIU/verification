#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include "zlog.h"


extern int rc;
extern zlog_category_t *zc; 

int  log_init(const char *conf_name, const char *cat_name);
void log_fini(void);
#if LOG_DEBUG
#define log_info(format, ...)\
    do {\
    \
        zlog_info(zc, format, ##__VA_ARGS__);\
    } while(0)

#define log_error(format, ...)\
    do{\
    \
        zlog_error(zc, format, ##__VA_ARGS__);\
    }while(0)

#define log_warning(format, ...)\
    do{\
    \
        zlog_warn(zc, format, ##__VA_ARGS__);\
    }while(0)

#define log_debug(format, ...)\
    do{\
    \
        zlog_debug(zc, format, ##__VA_ARGS__);\
    }while(0)
#endif
#if !LOG_DEBUG
#define log_info(format, ...) do{}while(0)
#define log_error(format, ...) do{}while(0)
#define log_warning(format, ...) do{}while(0)
#define log_debug(format, ...) do{}while(0)
#endif

#endif
