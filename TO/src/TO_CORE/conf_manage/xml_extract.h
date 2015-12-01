#ifndef _CFG_INIT_H
#define _CFG_INIT_H
#include <string.h>
#include <stdio.h>
#include "mxml.h"
#define ARRAY_LEN 50

/*
 *该枚举对应各配置文件的路径
 */
enum CFG_GROUP{
	LOCOINFO = 0,
	OPTIMIZESTRATEGY,
	ROUTEINFO,
	ROADSTATIONTIME
};

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
 * 原始优化策略匹配参数结构
 * OptimizeStrategy.xml中“RawStrategy”对应的数据结构
 */
struct strategyFlags
{
   int maxMass;  /**< 最大车重*/
   int minMass; /**< 最小车重 */
   int minLen; /**< 最小坡长*/
   int maxLen; /**<最大坡长*/
   float maxAvg; /**<最大平均速度*/
   float minAvg; /**<最小平均速度*/
   int partGradFlag; /**< 是否有局部非坡段类型的坡度*/
   int minSteepUpPect; /**<最小抖上坡百分比*/
   int maxSteepUpPect; /**<最大抖上坡百分比*/
   int minSteepDownPect; /**< 最小抖下坡百分比*/
   int maxSteepDownPect; /**< 最大抖下坡百分比*/
   int gradientType; /**< 坡段类型*/ //对应strategy中的flag
   char  name[40];  /**<策略名称*/
   int lastGraType;/**<上一段坡段类型，若没有该元素，则该元素值为100*/
   int nextGraType;/**<下一段坡段类型,若没有该元素，则该元素值为100*/
};

/**
 * 限速调整策略匹配参数结构
 * OptimizeStrategy.xml中“RawStrategy”对应的数据结构
 */
struct limitStrategyFLags
{
	int type;  /**< 限速类型标号*/
    char  name[40];  /**<策略名称*/
    int maxMass; /**< 最大车重*/
    int minMass; /**< 最小车重*/
};

/**
 * 时间调整策略匹配参数结构
 * OptimizeStrategy.xml中“TimeStrategy”对应的数据结构
 */
struct timeStrategyFlags
{
   int direction; /**< 前(左)或后(右)调整速度*/
   int leftRType; /**< 前(左)坡度类型*/
   int rightRType;/**<后(右)坡度类型*/
   int singleR;/**< 是否处理单独的段*/
   int up; /**< 升速或降速*/
   char name[40]; /** 策略名称*/
};
typedef struct timeStrategyFlags TIMESTRFLAGS;

/*
 * 用于存放OptimizeStrategy.xml中"ParameterConfig"内的数据，
 * 也可以存放RouteInfo.xml和LocoInfo.xml中的数据
 */
struct PARAMCFG {//PARAMCG - parameterconfig
	char name[ARRAY_LEN];//存放OptimizeStrategy.xml中"ParameterConfig"里"name"属性值，如"G", "PRECISION"等
	char value[ARRAY_LEN] ; //存放OptimizeStrategy.xml中"ParameterConfig"里"name"的value值
};

/*
 * 用于存放从OptimizeStrategy.xml提取的数据
 */
struct EXTR_OPT_RTN {//EXTR_OPT_RTN - extract optimize return,
	struct strategyFlags *strategyFlags_p; //strategyFlags_p - strategyFlags pointer
	int strategyFlags_len; //strategyFlags_len - strategyFlags length, 结构体strategyFlags的元素个数
	struct limitStrategyFLags *limitStrategyFLags_p; //
	int limitStrategyFLags_len;
	char PrmtCfg_value[ARRAY_LEN];//PrmtCfg_value - ParameterConfig value, 	用于存放ParameterConfig中Property的value值
	struct timeStrategyFlags *timeStrategyFlags_p;
	int timeStrategyFlags_len;
};

/**
 * 用于存放从OPtimizeStrategy.xml中提取数据的范围
 * */
struct EXTR_OPT_DATARANGE {
	char* max;
	char* min;
};

/*
 * 用于存放从RouteInfo.xml或LocoInfo.xml中提取的数据,RouteInfo.xml返回结构有变，对其返回的结构体重新定义
 */
struct EXTR_ROUTEINFO {
	struct PARAMCFG *RouteInfo_p;
	int RouteInfo_len;
	char* max;  //用于存放数据值范围
	char* min;	//用于存放数据值范围
};

/*
 * struct EXTRACT_FR_STT用于存放从roadStationTime.xml提取出的车站号，TMIS号，交路号,
 * EXTRACT_FR_STT -- extract from station time table
 */
struct EXTRACT_FR_STT{
	int station;
	int tmis;
	int route_num;
	char arrivetime[ARRAY_LEN];
	char starttime[ARRAY_LEN];
	char netxstation[ARRAY_LEN];
};

/*
 *  函数extrRoadStationTime()的返回指针所指向的结构体类型
 */
struct rt_extrRoadStationTime{
	struct EXTRACT_FR_STT *ptr;//ptr为指向存放roadStationTime.xml信息的结构体的指针
	int ptr_size;//用于存放roadStationTime.xml中的条目数，每组(车站号+TMIS号+交路号)为一个条目
};

/*
 * 用于存放从RouteInfo.xml提取的"pusherEngine"或"throughExpert"的信息
 */
struct IN_ROUTEINFO {
	int direction;
	int endTelesemes;
	int endPosOffset;
	int id;
	int linenum;
	int startTelesemes;
	int startPosOffset;
	int type;
	float endV;
};

/*
 * 用于存放从RouteInfo.xml提取的信息
 */
struct ROUTEINFO{
	struct IN_ROUTEINFO *in_routeinfo;
	int part_num;//"pusherEngine"中的“part num”的个数
	char value[ARRAY_LEN];
	char* min; //用于存放数据值范围
	char* max; //用于存放数据值范围
};


/*
 * function: to open xml files
 * argument: 该参数用以打开不同的xml文件，此参数在"xml_extract.h"中的枚举"enum CFG_GROUP"中定义
 * return: 返回打开的文件流
 */
FILE *cfg_open(int cfg_logo);

/*
 * function: 加载xml文件
 * argument: FILE *fd - fopen xml文件后返回的文件流
 * return: 返回成功加载xml文件后的节点指针
 */
mxml_node_t *cfgload(FILE *fd);

/*
 * function: 用于提取LocoInfo.xml的信息
 * argument: const char *attr_value - 输入"Item name"的值"locoType"，或\
 * 				”Property name“的值，如"brakecyLinderDiameter","brakeparas"等
 * return: 返回LocoInfo.xml中对应的value
 */
struct EXTR_ROUTEINFO *extr_LocoInfo(const char *attr_value, int *err_no);

/*
 * function: 提取OptimizeStrategy.xml文件
 * argument: const char *element - 取OptimizeStrategy.xml中的\
 * 				"RawStrategy","LimitStrategy"或"ParameterConfig"\
 * 				里“Property name”的值
 * return: 指向"struct EXTR_OPT_RTN"结构体的指针，该结构体在cfg.init.h中有定义
 */
struct EXTR_OPT_RTN *extr_OptimizeStrategy (const char *element, int *err_no);

/*
 *  function: 提取RouteInfo.xml文件
 *  argument:const char *element - 当"Item name"为"basicDataInfo","strokeIdentification","stationDictionary"时，\
 *  			element为"Property name"的值；当"Item name"为"pusherEngine","throughExpert"时，element为\
 *  			"pusherEngine"或"throughExpert"
 *  return: 指向"struct EXTR_ROUTEINFO"结构体的指针，该结构体在cfg.init.h中有定义
 */
struct ROUTEINFO *extr_RouteInfo (const char *element, int *err_no);

/*
 * function: 提取列车时刻表相关信息
 * argument: int *err_no - 指向错误码的指针
 * return: 结构体指针，该结构体的成员为存放列车时刻表信息的结构体的指针和列车时刻表信息的条目数
 */
struct rt_extrRoadStationTime *extrRoadStationTime(int *err_no);

/*
 * @brief: 用于释放调用extr_LocoInfo(), extr_OptimizeStrategy(), extr_RouteInfo(), extrRoadStationTime()函数后
 * 			分配的内存空间
 * @argument: void *return_type, 类型与调用函数返回的指针类型保持一致
 */
int delet(void *return_type);

/*
 * @brief: 调用完extr_LocoInfo()函数后，利用此函数从extr_LocoInfo()的返回值中检索参数的值，\
 * 			以roadStationTime.xml为例，输入"davisa"后，得到“davisa”的值"0.0001" ,此函数 \
 * 			也可用在extr_RouteInfo()和extr_OptimizeStrategy()之后
 * @argument:
 * @return:
 */
/*char *get_single_value(const char *parament, struct EXTR_ROUTEINFO *parament_table);*/

#endif
