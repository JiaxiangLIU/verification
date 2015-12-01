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
 * 将数据中时间转换成为标准时间格式 "2014-11-02 11:11:11"注意中间空格
 * 注意：out 应为char【30】数组地址
 * 时间数据大小为30！
 */

int convert_to_time(COMM_TIME comm_time,char* out);

/*
 * in应为收到的时间数据，直接调用
 */
int set_to_time(char * date,int length);
#endif
