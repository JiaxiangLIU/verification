#ifndef _DATASTRUCT_H
#define _DATASTRUCT_H

/*
 * 该枚举用于定义配置文件提取函数extrRoadStationTime()内部调用出错时的错误码
 */
enum cfgExtractRslt{//cfgExtractRslt -- configuration_file Extract Result
	CFG_EXTR_SUCCESS = 0,
	MALLOC_FAILED,
	CFG_OPEN_ERR,
	CFG_LOAD_ERR,
	CFG_FIND_ELEMENT_ERR
};

/*
 * struct EXTRACT_FR_STT用于存放从roadStationTime.xml提取出的车站号，TMIS号，交路号,
 * EXTRACT_FR_STT -- extract from station time table
 */
struct EXTRACT_FR_STT{
	int station;
	int tmis;
	int route_num;
	int bifurcation_distance;
	int bifurcation_speed;
};

/*
 *  函数extrRoadStationTime()的返回指针所指向的结构体类型
 */
struct rt_extrRoadStationTime{
	struct EXTRACT_FR_STT *ptr;//ptr为指向存放roadStationTime.xml信息的结构体的指针
	int ptr_size;//用于存放roadStationTime.xml中的条目数，每组(车站号+TMIS号+交路号)为一个条目
};

#endif
