

#include "log.h"

int rc;
zlog_category_t *zc;
int log_init(const char *conf_name, const char *cat_name)
{
	
    rc = zlog_init(conf_name);
	if (rc) {
		printf("init failed\n");
		return -1;
	}

	zc = zlog_get_category(cat_name);
	if (!zc) {
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
