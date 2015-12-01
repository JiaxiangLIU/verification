

#include "log.h"

int zlog_rc;
zlog_category_t *zlog_zc;
int log_init(const char *conf_name, const char *cat_name)
{
	
    zlog_rc = zlog_init(conf_name);
	if (zlog_rc) {
		printf("init failed\n");
		return -1;
	}

	zlog_zc = zlog_get_category(cat_name);
	if (!zlog_zc) {
		printf("get cat fail\n");
		zlog_fini();
		return -2;
	}

    printf("log init finish.\n");
    return 0;
}

void log_fini(void) {
    zlog_fini();
}
