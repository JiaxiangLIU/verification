#ifndef TO_TIEM_H
#define TO_TIEM_H

#include<stdio.h>
#include<stdlib.h>
typedef struct comm_time{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}COMM_TIME;


/*
 * ��������ʱ��ת����Ϊ��׼ʱ���ʽ "2014-11-02 11:11:11"ע���м�ո�
 * ע�⣺out ӦΪchar��30�������ַ
 * ʱ�����ݴ�СΪ30��
 */

int convert_to_time(COMM_TIME comm_time,char* out);

/*
 * inӦΪ�յ���ʱ�����ݣ�ֱ�ӵ���
 */
int set_to_time(char * date,int length);
#endif
