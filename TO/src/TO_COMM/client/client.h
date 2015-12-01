#ifndef _CLIENT_H
#define _CLIENT_H
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "route_data.h"

extern pthread_mutex_t mutex_s1;

enum FTYPE {
	REALTIME = 0,
	BASE,
	STATUS,
	DATA2K,
	OPENOUT
};


typedef struct _client {
	FILE *fp[5];//存放文件(如线路数据，车站文件，揭示信息等)标识符的数组
	int fswitch;//文件表示符
	int pkt_total;//包描述帧里的包总数
	int pkt_num;//统计接收到的包个数
	int data_exp;//包描述帧中的包长度
	int data_len;
	unsigned short pktcrc16_exp;
	unsigned short pktcrc16;
}CLIENT;

#endif
