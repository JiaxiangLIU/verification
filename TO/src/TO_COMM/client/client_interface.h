#ifndef _CLIENT_INTERFACE_H
#define _CLIENT_INTERFACE_H

extern int LKJ_status_flag ;//LKJ_TAX_status准备好后，用LKJ_TAX_status_flag来通知其他进程
extern char LKJ_TAX_status ;//用来表征:1.安全平台和LKJ的通断状态, 正常时置位LKJ_TAX_status的第0位
					        //2.状态信息里的基础数据版本和基础数据里的版本信息是否一致,一致时置位LKJ_TAX_status的第1位
					        //3.优化器和安全平台的通断状态,正常时置位LKJ_TAX_status的第2位
							//4.可以申请揭示信息时，置位LKJ_TAX_status

typedef struct _status_flag{
	int LKJ_status_flag;//用来标识安全平台与LKJ数据匹配
	int jieshi_status_flag;//用来标识揭示信息已经准备好
} STATUS_FLAG;

extern STATUS_FLAG status_flag;

extern unsigned char cli_flag_g;    //选择相应的模式: 0x00--初始化 0x01--基础数据文件 0x02--车站数据 0x04--揭示信息 0xff--结束标志

void comm_client();
#endif
