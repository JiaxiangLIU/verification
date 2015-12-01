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
	FILE *fp[5];//����ļ�(����·���ݣ���վ�ļ�����ʾ��Ϣ��)��ʶ��������
	int fswitch;//�ļ���ʾ��
	int pkt_total;//������֡��İ�����
	int pkt_num;//ͳ�ƽ��յ��İ�����
	int data_exp;//������֡�еİ�����
	int data_len;
	unsigned short pktcrc16_exp;
	unsigned short pktcrc16;
}CLIENT;

#endif
