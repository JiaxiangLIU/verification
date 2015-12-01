#include "to_time.h"
int convert_to_time(COMM_TIME comm_time,char* out){
    char date[]="2012-11-11";
    sprintf(date,"%4d-%2d-%2d",comm_time.year,comm_time.month,comm_time.day);
	char time[]="12:12:12";
	sprintf(time,"%2d:%2d:%2d",comm_time.hour,comm_time.minute,comm_time.second);
	sprintf(out, "date -s \"%s %s\"", date, time);
	return 1;
}
int set_to_time(char * data,int length){
	if (length != 30) {
		printf("time data is error\n");
	}
	system(data);
	char command[]="clock -w";
	system(command);
}

