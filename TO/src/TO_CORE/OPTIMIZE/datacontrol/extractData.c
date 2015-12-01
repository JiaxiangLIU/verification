/**
 *@file  extrData.c
 *@brief 基础数据提取实现
 *@date  2014.4.1
 *@author  danny
 *@note extract RawData from files into gloabal variables
 * 				folder name : RawData
 * 				variables	: curves
 * 							              gradients
 * 							              limits
 * 							              stations
 * 							              telesemes
 * 							              tunnels
 */
#include "extrData.h"
#include <errno.h>
#include "opt_global_var.h"
#include <regex.h>

CURVE* curves;
GRADIENT* gradients;
LIMIT* limits;
STATION* stations;
TELESEME* telesemes;
TUNNEL* tunnels;
OPTCONSTPARAM opt_const;
LOCOPARAMETER locoInfo;
INTERVALRUNTIMEUNIT* intervalRuntimeUnits;
RAWSTRFLAGS* rawStrategyMetrics;
TIMESTRFLAGS* timeStrategyMetrics;
LIMITSTRFLAGS* limitStrategyMetrics;
GEDATA* geDatas;
INITIALDATA* initialDatas;
ROADCATEGORY* rc;
CARSTRUCT *carStruct;
HANDAREA throughExpert;
PUSHERENGINE pusherEngines;
float dragForce[8][120];
float brakeForce[8][120];

int MAX_GRADIENT;
int MAX_CURVE;
int MAX_LIMIT;
int MAX_TELESEME;
int MAX_TUNNEL;
int MAX_STATION;
int MAX_INTERVALRUNTIME;
int MAX_RAWSTRATEGY;
int MAX_TIMESTRATEGY;
int MAX_LIMITSTRATEGY;
int MAX_ROADCATEGORY;
int MAX_GEDATA;
int MAX_INITIALDATA;
int CARNUMS;
/**
 * @brief  读取文件的行数
 * @param fp  文件指针
 * @param  文件的行数
 * @return  文件的读取结果
 */
OPT_EXTR_RESULT getLine(FILE* fp,int* line);
/**
 * 读取优化中需要的常量值
 * @return
 */
OPT_EXTR_RESULT  extrOptConst();
/**
 * 读取交路数据
 * @return
 */
OPT_EXTR_RESULT extrRouteData();
/**
 * 列车基本特性信息提取
 * @return
 */
OPT_EXTR_RESULT extrLocoInfo();

/**
 * @brief  读取线路数据，机车数据等基本数据
 * @param routeNum  行程号
 * @return  提取
 */
OPT_EXTR_RESULT  extrData(int routeNum)
{
	//	int i;
	//	int line;
	char desc[EXP_DESC_LENGTH]="#extrData.c";
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	//进行交路提取
	opt_log_info("读取交路数据开始");
	ret = extrRouteData();
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	opt_log_info("读取交路数据结束");

	//读取优化中的常量
	opt_log_info("读取优化常量开始");
	ret=extrOptConst();
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	opt_log_info("读取优化常量结束");
	//基本数据提取
	opt_log_info("读取机车基本数据开始");
	ret=extrLocoInfo();
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	opt_log_info("读取机车基本数据结束");

	//读取列车时刻表
	opt_log_info("读取列车时刻表开始");
	ret=extrRoadTimetable();
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	opt_log_info("读取列车时刻表结束");

	//读取区域配置信息
	opt_log_info("读取手动区域配置信息开始");
	ret=extrHandAreaInfo();
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	opt_log_info("读取手动区域配置信息结束");

	return ret;
}
/**
 * 列车基本特性信息提取
 * @return
 */
OPT_EXTR_RESULT extrLocoInfo()
{
	int i,j ;
	FILE* fid ;
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	int flag;	//标识是否可以进行类型转换
	char* float_pattern = "^[-+]?[0-9]*\.?[0-9]+$";  //匹配浮点数的正则表达式
	char* int_pattern = "^[-+]?[0-9]+$";	//匹配整型的正则表达式

	struct EXTR_ROUTEINFO *locoData;
	struct ROUTEINFO *routeData;

	enum cfgExtractRslt err_no=CFG_EXTR_SUCCESS;
	//读取牵引力
	locoData = extr_LocoInfo("dragPath", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#dragPath");
		return ret;
	}
	char dragPath[ARRAY_LEN];
	strcpy(dragPath,locoData->RouteInfo_p[0].value);
	if((fid = fopen(dragPath,"r"))==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrLocoInfo#dragforce");
		return ret;
	}
	for(i = 0;i < 120 ;i++){
		for (j = 0 ; j < 8 ;j++){
			fscanf(fid,"%f",&dragForce[j][i]);
		}
	}
	fclose(fid);
	free(locoData);

	//读取制动力
	locoData = extr_LocoInfo("brakePath", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#brakePath");
		return ret;
	}
	char brakePath[ARRAY_LEN];
	strcpy(brakePath,locoData->RouteInfo_p[0].value);

	if((fid = fopen(brakePath,"r"))==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrLocoInfo#brakeForce");
		return ret;
	}
	for(i = 0;i < 120 ;i++){
		for (j = 0 ; j < 8 ;j++){
			fscanf(fid,"%f",&brakeForce[j][i]);
		}
	}	//free(fid);	close(fid);
	fclose(fid);
	free(locoData);

	//读取车头重量和车头长度
	float locoWeight,locoLength;

	/**
	 * 读取数据类型的范围数据LocoInfo.xml, RouteInfo.xml
	 * */
	//	locoData = extr_LocoInfo("carWeight", &err_no);
	//	printf(locoData->max);
	//	printf(locoData->min);

	locoData =extr_LocoInfo("calculatedweight", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#calculatedweight");
		return ret;
	}
	flag = checkType(locoData->RouteInfo_p[0].value, float_pattern);
	if(flag==0 ||flag ==-1) {	//判断数据类型是否可以转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#calculatedweight");
		return ret;
	} else if(!((atof(locoData->min)<atof(locoData->RouteInfo_p[0].value)||fabs(atof(locoData->min)-atof(locoData->RouteInfo_p[0].value))<opt_const.PRECISION)
			&& (atof(locoData->RouteInfo_p[0].value)<atof(locoData->max)||fabs(atof(locoData->RouteInfo_p[0].value)-atof(locoData->max))<opt_const.PRECISION))) {	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#calculatedweight");
		return ret;
	} else {
		locoWeight=atof(locoData->RouteInfo_p[0].value);
	}
	free(locoData);

	locoData = extr_LocoInfo("locoLength", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#locoLength");
		return ret;
	}
	flag = checkType(locoData->RouteInfo_p[0].value, float_pattern);
	if(flag==0 ||flag ==-1) {	//判断数据类型是否可以转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#locoLength");
		return ret;
	} else if(!((atof(locoData->min)<atof(locoData->RouteInfo_p[0].value)||fabs(atof(locoData->min)-atof(locoData->RouteInfo_p[0].value))<opt_const.PRECISION)
			&& (atof(locoData->RouteInfo_p[0].value)<atof(locoData->max)||fabs(atof(locoData->RouteInfo_p[0].value)-atof(locoData->max))<opt_const.PRECISION))) {	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#locoLength");
		return ret;
	} else {
		locoLength=atof(locoData->RouteInfo_p[0].value);
	}
	free(locoData);

	//读取车长度，车厢节数，车重量
	float trainWeight,trainLength,trainNum;
	trainWeight=locoMarshInfo.totalWeight;
	trainLength=locoMarshInfo.totalLength;
	trainNum=locoMarshInfo.totalNums;
//	routeData = extr_RouteInfo("carWeight", &err_no);
//	if(err_no!=CFG_EXTR_SUCCESS)
//	{
//		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#trainWeight");
//		return ret;
//	}
//	flag = checkType(routeData->value, float_pattern);
//	if(flag==0 ||flag ==-1) {	//判断数据类型是否可以转换
//		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#trainWeight");
//		return ret;
//	} else if(!((atof(routeData->value)>atof(routeData->min)||fabs(atof(routeData->value)-atof(routeData->min))<opt_const.PRECISION)
//			&& (atof(routeData->value)<atof(routeData->max)||fabs(atof(routeData->value)-atof(routeData->max))<opt_const.PRECISION))) {	//判断数据是否在指定范围内
//		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#trainWeight");
//		return ret;
//	} else {
//		trainWeight=atof(routeData->value);
//	}
//	free(routeData);
//
//	routeData = extr_RouteInfo("carLength", &err_no);
//	if(err_no!=CFG_EXTR_SUCCESS)
//	{
//		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#trainLength");
//		return ret;
//	}
//	flag = checkType(routeData->value, float_pattern);
//	if(flag==0 ||flag ==-1) {	//判断数据类型是否可以转换
//		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#trainLength");
//		return ret;
//	} else if(!((atof(routeData->value)>atof(routeData->min)||fabs(atof(routeData->value)-atof(routeData->min))<opt_const.PRECISION)
//			&& (atof(routeData->value)<atof(routeData->max)||fabs(atof(routeData->value)-atof(routeData->max))<opt_const.PRECISION))) {	//判断数据是否在指定范围内
//		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#trainLength");
//		return ret;
//	} else {
//		trainLength=atof(routeData->value);
//	}
//	free(routeData);
//
//	routeData = extr_RouteInfo("carNum", &err_no);
//	if(err_no!=CFG_EXTR_SUCCESS)
//	{
//		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#trainNum");
//		return ret;
//	}
//	flag = checkType(routeData->value, int_pattern);
//	if(flag==0 ||flag ==-1) {	//判断数据类型是否可以转换
//		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#trainNum");
//		return ret;
//	} else if(!(atoi(routeData->value)>=atoi(routeData->min) && atoi(routeData->value)<=atoi(routeData->max))) {	//判断数据是否在指定范围内
//		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#trainNum");
//		return ret;
//	} else {
//		trainNum=atoi(routeData->value);
//	}
//	free(routeData);

	//计算列车总重量和总长度等
	locoInfo.totalWeight = locoWeight+trainWeight;
	locoInfo.totalLength =(locoLength+trainLength)*11;
	locoInfo.count=trainNum;
	carStruct = (CARSTRUCT*) malloc(locoInfo.count*sizeof(CARSTRUCT));
	if(carStruct==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrLocoInfo#carStruct");
		return ret;
	}
	float avgW=locoInfo.totalWeight/locoInfo.count;
	float avgL=locoInfo.totalLength/locoInfo.count;
	CARNUMS=locoInfo.count;
	for(i=0;i<CARNUMS;i++)
	{
		carStruct[i].carLength=avgL;
		carStruct[i].carWeight=avgW;
		carStruct[i].loadWeight=0.0;
	}
	//			extrCarsInfo();

	//提取戴维斯系数
	char  davisStr[10]="abc";
	for(i=0;i<3;i++)
	{
		char   strHead[10]="davis";
		char tmp[2];
		tmp[0]=davisStr[i];
		tmp[1]='\0';
		strcat(strHead,tmp);
		locoData = extr_LocoInfo(strHead, &err_no);
		if(err_no!=CFG_EXTR_SUCCESS)
		{
			file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#davis");
			return ret;
		}
		flag = checkType(locoData->RouteInfo_p[0].value, float_pattern);
		if(flag==0 ||flag ==-1) {	//判断数据类型是否可以转换
			file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#davis");
			return ret;
		} else if(!((atof(locoData->min)<atof(locoData->RouteInfo_p[0].value)||fabs(atof(locoData->min)-atof(locoData->RouteInfo_p[0].value))<opt_const.PRECISION)
				&& (atof(locoData->RouteInfo_p[0].value)<atof(locoData->max)||fabs(atof(locoData->RouteInfo_p[0].value)-atof(locoData->max))<opt_const.PRECISION))) {	//判断数据是否在指定范围内
			file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#davis");
			return ret;
		} else {
			locoInfo.davis[i]=atof(locoData->RouteInfo_p[0].value);
		}
		free(locoData);
	}

	//提取制动档位油耗
	for(i=0;i<abs(opt_const.MINGEAR);i++)
	{
		char   strHead[10]="brake";
		char  str[2];
		sprintf(str,"%d",i+1);
		strcat(strHead,str);
		locoData = extr_LocoInfo(strHead, &err_no);
		if(err_no!=CFG_EXTR_SUCCESS)
		{
			file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#brakeOil");
			return ret;
		}
		flag = checkType(locoData->RouteInfo_p[0].value, float_pattern);
		if(flag==0 ||flag ==-1) { //判断数据类型是否可以转换
			file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#brakeOil");
			return ret;
		}else {
			locoInfo.brakeConsumption[i]=atof(locoData->RouteInfo_p[0].value);
		}
		free(locoData);
	}

	//提取牵引档位油耗
	for(i=0;i<abs(opt_const.MAXGEAR);i++)
	{
		char   strHead[10]="drag";
		char  str[2];
		sprintf(str,"%d",i+1);
		strcat(strHead,str);
		locoData = extr_LocoInfo(strHead, &err_no);
		if(err_no!=CFG_EXTR_SUCCESS)
		{
			file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#dragOil");
			return ret;
		}
		flag = checkType(locoData->RouteInfo_p[0].value, float_pattern);
		if(flag==0 ||flag ==-1) {	//判断数据类型是否可以转换
			file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#dragOil");
			return ret;
		}else {
			locoInfo.dragConsumption[i]=atof(locoData->RouteInfo_p[0].value);
		}
		free(locoData);
	}

	//提取惰行档位油耗
	locoData = extr_LocoInfo("OilConsume", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLocoInfo#idelOil");
		return ret;
	}
	flag = checkType(locoData->RouteInfo_p[0].value, float_pattern);
	if(flag==0 ||flag ==-1) {	//判断数据类型是否可以转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#idelOil");
		return ret;
	}  else if(!((atof(locoData->min)<atof(locoData->RouteInfo_p[0].value)||fabs(atof(locoData->min)-atof(locoData->RouteInfo_p[0].value))<opt_const.PRECISION)
			&& (atof(locoData->RouteInfo_p[0].value)<atof(locoData->max)||fabs(atof(locoData->RouteInfo_p[0].value)-atof(locoData->max))<opt_const.PRECISION))) {	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrLocoInfo#idelOil");
		return ret;
	} else {
		locoInfo.idleConsumption=atof(locoData->RouteInfo_p[0].value);
	}
	free(locoData);

	return ret;
}
/**
 * 交路数据提取
 * @return
 */
OPT_EXTR_RESULT extrRouteData()
{
	OPT_EXTR_RESULT ret;
	int  i;
	ret.file_result=OPT_EXTR_SUCCESS;
	//	CURVE
	struct curve_t *curve;
	int length;
	curve=(struct curve_t *)read_binary_file(0x02, &length);
	if(curve==NULL){
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrRouteData#curve");
		return ret;
	}
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#extrRouteData#curve";
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	MAX_CURVE = length;
	curves = (CURVE*) malloc(sizeof(CURVE)*MAX_CURVE);
	if(curves==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrRouteData#curve");
		return ret;
	}
	for(i=0;i<MAX_CURVE;i++)
	{
		curves[i].start=curve[i].km_post;
		curves[i].end=curve[i].km_post+curve[i].length;
		curves[i].dir=curve[i].dir;
		curves[i].radius=curve[i].value;
	}
	free(curve);

	//	GRADIENT
	struct gradient_t *gra;
	gra=(struct gradient_t*)read_binary_file(0x01, &length);
	if(gra==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrRouteData#gradient");
		return ret;
	}
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#extrRouteData#gradient";
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	MAX_GRADIENT = length;
	gradients = (GRADIENT*) malloc(sizeof(GRADIENT)*MAX_GRADIENT);
	if(gradients==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret, "#extrRouteData#gradient");
		return ret;
	}
	for(i=0;i<MAX_GRADIENT;i++)
	{
		gradients[i].start=gra[i].km_post;
		gradients[i].length=gra[i].length;
		gradients[i].value=gra[i].value;
	}
	free(gra);
	//	STATION
	struct station_t *stat;
	stat=(struct station_t*)read_binary_file(0x05, &length);
	if(stat==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrRouteData#station");
		return ret;
	}
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#extrRouteData#station";
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	MAX_STATION = length;
	stations = (STATION*) malloc(sizeof(STATION)*MAX_STATION);
	if(stations==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrRouteData#station");
		return ret;
	}
	for(i=0;i<MAX_STATION;i++)
	{
		stations[i].id=stat[i].s_num;
		stations[i].start=stat[i].km_post;
	}
       FILE* fid2 = fopen("././ret/stations", "wt");
		if (fid2 == NULL) {
			fprintf(stderr, "open stations err!\n");
		}
		for (i = 0; i < MAX_STATION; i++) {
			fprintf(fid2, "%d\t%d\n", stations[i].id,stations[i].start);
		}
		fclose(fid2);
	free(stat);

	//	LIMIT
	// 测试限速策略
	//	length=3;
	//	MAX_LIMIT = length;
	//	limits = (LIMIT*) malloc(sizeof(LIMIT)*MAX_LIMIT);
	//	limits[0].start = 79793;
	//	limits[0].end = 152500;
	//	limits[0].limit = 80;
	//
	//	limits[1].start = 152500;
	//	limits[1].end = 154000;
	//	limits[1].limit = 55;
	//
	//	limits[2].start = 154000;
	//	limits[2].end = 269188;
	//	limits[2].limit = 80;


	struct limit_t *lmt;
	lmt=(struct limit_t*)read_binary_file(0x03, &length);
	if(lmt==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret, "#extrRouteData#limit");
		return ret;
	}
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#extrRouteData#limit";
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}

	MAX_LIMIT = length;
	limits = (LIMIT*) malloc(sizeof(LIMIT)*MAX_LIMIT);

	if(limits==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrRouteData#limit");
		return ret;
	}
	for(i=0;i<MAX_LIMIT;i++)
	{
		limits[i].start=lmt[i].km_post;
		limits[i].end=lmt[i].km_post+lmt[i].length;
		limits[i].limit=lmt[i].value;
	}
	free(lmt);


	//	TELESEME
	struct tele_t *tele;
	tele=(struct tele_t*)read_binary_file(0x04, &length);
	if(tele==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrRouteData#teleseme");
		return ret;
	}
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#extrRouteData#teleseme";
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	MAX_TELESEME = length;

	telesemes = (TELESEME*) malloc(sizeof(TELESEME)*MAX_TELESEME);
	if(telesemes==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret, "#extrRouteData#teleseme");
		return ret;
	}
	for(i=0;i<MAX_TELESEME;i++)
	{
		telesemes[i].post=tele[i].cont_km_post;
		telesemes[i].tel_num=tele[i].t_num;
	}
	FILE* fid = fopen("././ret/telesemes", "wt");
	if (fid == NULL) {
		fprintf(stderr, "open optFinalResult err!\n");
	}
	for (i = 0; i < MAX_TELESEME; i++) {
		fprintf(fid, "%d\t%d\n", telesemes[i].tel_num,telesemes[i].post);
	}
	fclose(fid);
	free(tele);


	//	TUNNEL
	struct tunnel_t * tunnel;
	tunnel=(struct tele_t*)read_binary_file(0x06, &length);
	if(tunnel==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrRouteData#tunnel");
		return ret;
	}
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#extrRouteData#tunnel";
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		return ret;
	}
	MAX_TUNNEL = length;
	tunnels = (TUNNEL*) malloc(sizeof(TUNNEL)*MAX_TUNNEL);
	if(tunnels==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrRouteData#tunnel");
		return ret;
	}
	for(i=0;i<MAX_TUNNEL;i++)
	{
		tunnels[i].start=tunnel[i].km_post;
		tunnels[i].end=tunnel[i].km_post+tunnel[i].length;
	}
	free(tunnel);
	return ret;
}
/**
 *  @brief 读取用于与在线优化比较的司机运行数据
 */
OPT_EXTR_RESULT extrInitialData()
{
	FILE* fid ;
	int line,i;
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	if((fid = fopen("././RawData/initialData","r"))==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrInitialData#initialData");
		return ret;
	}
	ret = getLine(fid,&line);
	if(ret.file_result!=OPT_EXTR_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#extrInitialData#initialData";
		strcat(desc,ret.description);
		file_exception_decorator(ret.file_result,&ret,desc);
		fclose(fid);
		return ret;
	}

	MAX_INITIALDATA = line;
	initialDatas = (INITIALDATA*)malloc(sizeof(INITIALDATA)*line);
	if(initialDatas==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrInitialData#initialDatas");
		fclose(fid);
		return ret;
	}
	for(i=0;i<line;i++)
	{
		fscanf(fid,"%f%f%f%f%f%f%f%d",
				&initialDatas[i].startV,&initialDatas[i].endV,&initialDatas[i].consumption,&initialDatas[i].time,&initialDatas[i].avgV,
				&initialDatas[i].totalWeight,&initialDatas[i].carLengh,&initialDatas[i].carNum);
	}
	close(fid);
	return ret;
}
/**
 * @brief 读取列车时刻表相关信息
 */
OPT_EXTR_RESULT extrRoadTimetable()
{
	//	int line=0;
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	struct rt_extrRoadStationTime *roadStationTime_tp;
	enum cfgExtractRslt err_no=CFG_EXTR_SUCCESS;
	roadStationTime_tp = extrRoadStationTime(&err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrRoadTimetable#roadStationTime_tp");
		return ret;
	}
	int i=0;
	int length=roadStationTime_tp->ptr_size;//时刻表Item的数量
	intervalRuntimeUnits = (INTERVALRUNTIMEUNIT*) malloc(sizeof(INTERVALRUNTIMEUNIT)*(length-1));
	if(intervalRuntimeUnits==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrRoadTimetable#intervalRuntimeUnits");
		return ret;
	}
	for(;i<length-1;i++)
	{
		intervalRuntimeUnits[i].startCzh=roadStationTime_tp->ptr[i].station;
		intervalRuntimeUnits[i].endCzh=atoi(roadStationTime_tp->ptr[i].netxstation);
		strcpy(intervalRuntimeUnits[i].startTime,roadStationTime_tp->ptr[i].starttime);
		strcpy(intervalRuntimeUnits[i].arriveTime,roadStationTime_tp->ptr[i+1].arrivetime);
		intervalRuntimeUnits[i].averageV=0;
		intervalRuntimeUnits[i].runTime=0;
		intervalRuntimeUnits[i].timeDiff=0;
		intervalRuntimeUnits[i].totalRuntime=0;
	}
	MAX_INTERVALRUNTIME = length-1;
	return ret;
}
/**
 * @brief 读取列车编组信息
 */
OPT_EXTR_RESULT extrCarsInfo()
{
	int line=0;
	mxml_node_t* root;
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	root=openXML("././RawData/carInformation.xml","r");
	if(root==NULL)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrCarsInfo#carInformation");
		return ret;
	}
	mxml_node_t *car;
	car = mxmlFindElement(root, root, "Car",NULL, NULL,MXML_DESCEND);
	locoInfo.totalLength=0.0;
	locoInfo.totalWeight=0.0;
	locoInfo.count=0;
	for(;car!=NULL;car=mxmlFindElement(car, root, "Car",NULL, NULL,MXML_DESCEND))
	{
		mxml_node_t *weight;
		mxml_node_t * load;
		mxml_node_t * length;
		int num=atoi(mxmlElementGetAttr(car,"num"));
		carStruct = (CARSTRUCT*) realloc(carStruct,(line+num)*sizeof(CARSTRUCT));
		if(carStruct==NULL)
		{
			file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrCarsInfo#carStruct");
			return ret;
		}
		weight=mxmlFindElement(car, root, "Weight",NULL, NULL,MXML_DESCEND);
		float w,ld,l;
		w=atof(weight->child->value.text.string);
		load=mxmlFindElement(car, root, "Load",NULL, NULL,MXML_DESCEND);
		ld=atof(load->child->value.text.string);
		length=mxmlFindElement(car, root, "Length",NULL, NULL,MXML_DESCEND);
		l= atof(length->child->value.text.string);

		printf("%d,%d,%d\n",w,ld,l);
		int i;
		for(i=line;i<line+num;i++)
		{
			carStruct[i].carLength=l;
			locoInfo.totalLength=locoInfo.totalLength+l;
			carStruct[i].carWeight= w;
			carStruct[i].loadWeight= ld;
			locoInfo.totalWeight=locoInfo.totalWeight+w+ld;
			printf("line=%d: %f,%f,%f\n",i,carStruct[i].carLength,carStruct[i].carWeight,carStruct[i].loadWeight);
		}
		line=line+num;
	}
	locoInfo.totalLength=locoInfo.totalLength*11.0;
	printf("%f \n",locoInfo.totalLength);
	printf("%f \n",locoInfo.totalWeight);
	CARNUMS = line;
	locoInfo.count=CARNUMS;
	printf("%d\n",locoInfo.count);
	closeXML(root);
	return ret;
}
/**
 * @brief 读取原始优化策略矩阵
 */
OPT_EXTR_RESULT  extrRawStrategyMetrics()
{
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	struct EXTR_OPT_RTN *opt_strategy;
	enum cfgExtractRslt err_no=CFG_EXTR_SUCCESS;
	opt_strategy = extr_OptimizeStrategy("RawStrategy", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrRawStrategy#opt_strategy");
		return ret;
	}
	int length=opt_strategy->strategyFlags_len;
	rawStrategyMetrics = (RAWSTRFLAGS*) malloc(length*sizeof(RAWSTRFLAGS));
	if(rawStrategyMetrics==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrRawStrategy#rawStrategyMetrics");
		return ret;
	}
	int i=0;
	for(;i<length;i++)
	{
		strcpy(rawStrategyMetrics[i].name,opt_strategy->strategyFlags_p[i].name);
		rawStrategyMetrics[i].maxMass=opt_strategy->strategyFlags_p[i].maxMass;
		rawStrategyMetrics[i].minMass=opt_strategy->strategyFlags_p[i].minMass;
		rawStrategyMetrics[i].gradientType=opt_strategy->strategyFlags_p[i].gradientType;
		rawStrategyMetrics[i].maxLen=opt_strategy->strategyFlags_p[i].maxLen;
		rawStrategyMetrics[i].minLen=opt_strategy->strategyFlags_p[i].minLen;
		rawStrategyMetrics[i].maxAvg=opt_strategy->strategyFlags_p[i].maxAvg;
		rawStrategyMetrics[i].minAvg=opt_strategy->strategyFlags_p[i].minAvg;
		rawStrategyMetrics[i].partGradFlag=opt_strategy->strategyFlags_p[i].partGradFlag;
		rawStrategyMetrics[i].maxSteepUpPect=opt_strategy->strategyFlags_p[i].maxSteepUpPect;
		rawStrategyMetrics[i].minSteepUpPect=opt_strategy->strategyFlags_p[i].minSteepUpPect;
		rawStrategyMetrics[i].maxSteepDownPect=opt_strategy->strategyFlags_p[i].maxSteepDownPect;
		rawStrategyMetrics[i].minSteepDownPect=opt_strategy->strategyFlags_p[i].minSteepDownPect;
		rawStrategyMetrics[i].lastGraType=opt_strategy->strategyFlags_p[i].lastGraType;
		rawStrategyMetrics[i].nextGraType=opt_strategy->strategyFlags_p[i].nextGraType;
	}
	MAX_RAWSTRATEGY = length;
	free(opt_strategy->strategyFlags_p);
	free(opt_strategy);
	return ret;
}
/**
 * @brief 读取时间偏差调整策略矩阵
 */
OPT_EXTR_RESULT   extrTimeStrategyMetrics()
{
	//	int line=0;
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	struct EXTR_OPT_RTN *time_strategy;
	enum cfgExtractRslt err_no=CFG_EXTR_SUCCESS;
	time_strategy = extr_OptimizeStrategy("TimeStrategy", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrRawStrategy#time_strategy");
		return ret;
	}
	int length=time_strategy->timeStrategyFlags_len;
	timeStrategyMetrics = (TIMESTRFLAGS*) malloc(length*sizeof(TIMESTRFLAGS));
	if(timeStrategyMetrics==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrRawStrategy#timeStrategyMetrics");
		return ret;
	}
	int i=0;
	for(;i<length;i++)
	{
		timeStrategyMetrics[i].direction=time_strategy->timeStrategyFlags_p[i].direction;
		timeStrategyMetrics[i].leftRType=time_strategy->timeStrategyFlags_p[i].leftRType;
		timeStrategyMetrics[i].rightRType=time_strategy->timeStrategyFlags_p[i].rightRType;
		timeStrategyMetrics[i].singleR=time_strategy->timeStrategyFlags_p[i].singleR;
		timeStrategyMetrics[i].up=time_strategy->timeStrategyFlags_p[i].up;
		strcpy(timeStrategyMetrics[i].name,time_strategy->timeStrategyFlags_p[i].name);
		printf("%s,%d,%d,%d,%d,%d\n",timeStrategyMetrics[i].name,timeStrategyMetrics[i].direction,timeStrategyMetrics[i].leftRType,timeStrategyMetrics[i].rightRType,timeStrategyMetrics[i].singleR,timeStrategyMetrics[i].up);
	}
	MAX_TIMESTRATEGY = length;
	free(time_strategy->timeStrategyFlags_p);
	free(time_strategy);
	return ret;
	//	mxml_node_t* root;
	//	root=openXML("././RawData/TimeStrategy.xml","r");
	//
	//	if(root==NULL)
	//	{
	//		file_exception_decorator(FILE_OPEN_ERROR,&ret,"#extrTimeStrategyMetrics");
	//		return ret;
	//	}
	//	mxml_node_t *strategy;
	//	strategy = mxmlFindElement(root, root, "Strategy",NULL, NULL,MXML_DESCEND);
	//	line=0;
	//	for(;strategy!=NULL;strategy=mxmlFindElement(strategy, root, "Strategy",NULL, NULL,MXML_DESCEND))
	//	{
	//		int direction=atoi(mxmlElementGetAttr(strategy,"direction"));
	//		int leftType=atoi(mxmlElementGetAttr(strategy,"leftType"));
	//		int rightType=atoi(mxmlElementGetAttr(strategy,"rightType"));
	//		int singleType=atoi(mxmlElementGetAttr(strategy,"singleType"));
	//		int vUp=atoi(mxmlElementGetAttr(strategy,"vUp"));
	//		char* name=mxmlElementGetAttr(strategy,"name");
	//
	//		timeStrategyMetrics = (TIMESTRFLAGS*) realloc(timeStrategyMetrics,(line+1)*sizeof(TIMESTRFLAGS));
	//		if(timeStrategyMetrics==NULL)
	//		{
	//			file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrTimeStrategyMetrics");
	//			return ret;
	//		}
	//		timeStrategyMetrics[line].direction=direction;
	//		timeStrategyMetrics[line].leftRType=leftType;
	//		timeStrategyMetrics[line].rightRType=rightType;
	//		timeStrategyMetrics[line].singleR=singleType;
	//		timeStrategyMetrics[line].up=vUp;
	//		copy(timeStrategyMetrics[line].name,name);
	//		line=line+1;
	//	}
	//	MAX_TIMESTRATEGY = line;
	//	closeXML(root);

}

/**
 * @brief 读取限速偏差调整策略矩阵
 */
OPT_EXTR_RESULT  extrLimitStrategyMetrics()
{
	//	int line=0;
	//	mxml_node_t* root;
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	struct EXTR_OPT_RTN *lmt_strategy;
	enum cfgExtractRslt err_no=CFG_EXTR_SUCCESS;
	lmt_strategy = extr_OptimizeStrategy("LimitStrategy", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrLimitStrategy#lmt_strategy");
		return ret;
	}
	int length=lmt_strategy->limitStrategyFLags_len;
	limitStrategyMetrics = (LIMITSTRFLAGS*) malloc(length*sizeof(LIMITSTRFLAGS));
	if(limitStrategyMetrics==NULL)
	{
		file_exception_decorator(OPT_MALLOC_ERROR,&ret,"#extrLimitStrategy#limitStrategyMetrics");
		return ret;
	}
	int i=0;
	for(;i<length;i++)
	{
		limitStrategyMetrics[i].type=lmt_strategy->limitStrategyFLags_p[i].type;
		strcpy(limitStrategyMetrics[i].name,lmt_strategy->limitStrategyFLags_p[i].name);
		limitStrategyMetrics[i].maxMass=lmt_strategy->limitStrategyFLags_p[i].maxMass;
		limitStrategyMetrics[i].minMass=lmt_strategy->limitStrategyFLags_p[i].minMass;
	}
	MAX_LIMITSTRATEGY = length;
	free(lmt_strategy->limitStrategyFLags_p);
	free(lmt_strategy);
	return ret;
}
/**
 * @brief 读取优化中使用到的所有常量值
 */
OPT_EXTR_RESULT  extrOptConst()
{
	struct EXTR_OPT_RTN *opt_param;
	struct EXTR_OPT_DATARANGE *opt_datarange;
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	enum cfgExtractRslt err_no=CFG_EXTR_SUCCESS;
	char* float_pattern = "^[-+]?[0-9]*\.?[0-9]+$";	//匹配浮点型的正则表达式
	char* int_pattern = "^[-+]?[0-9]+$";	//匹配整型的正则表达式
	int flag;	//标识是否可以进行类型转换

	/**
	 * 提取范围数据OptimizeStrategy.xml
	 * */
	//	opt_datarange =extr_opt_datarange("G",&err_no);
	//	printf(opt_datarange->max);
	//	printf(opt_datarange->min);
	//
	//	opt_datarange = extr_opt_datarange("PRECISION",&err_no);
	//	printf(opt_datarange->max);
	//	printf(opt_datarange->min);

	opt_param = extr_OptimizeStrategy("PRECISION", &err_no);
	opt_datarange = extr_opt_datarange("PRECISION", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#PRECISION");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value,float_pattern);
	if(flag==0 || flag ==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#PRECISION");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#G");
		return ret;
	} else {
		opt_const.PRECISION=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("G", &err_no);
	opt_datarange = extr_opt_datarange("G", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#G");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value,float_pattern);
	if(flag==0 ||flag ==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#G");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#G");
		return ret;
	} else {
		opt_const.G=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("EMPTYINTERVAL", &err_no);
	opt_datarange = extr_opt_datarange("EMPTYINTERVAL", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#EMPTYINTERVAL");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value,float_pattern);
	if(flag==0 || flag ==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#EMPTYINTERVAL");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#EMPTYINTERVAL");
		return ret;
	} else {
		opt_const.EMPTYINTERVAL=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("LIMIT_INTERVAL", &err_no);
	opt_datarange = extr_opt_datarange("LIMIT_INTERVAL", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#LIMIT_INTERVAL");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value,int_pattern);
	if(flag==0 || flag ==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#LIMIT_INTERVAL");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#LIMIT_INTERVAL");
		return ret;
	} else {
		opt_const.LIMIT_INTERVAL=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("DELTA_S", &err_no);
	opt_datarange = extr_opt_datarange("DELTA_S", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#DELTA_S");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#DELTA_S");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#DELTA_S");
		return ret;
	} else {
		opt_const.DELTA_S=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("TSTEP", &err_no);
	opt_datarange = extr_opt_datarange("TSTEP", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#TSTEP");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#TSTEP");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#TSTEP");
		return ret;
	} else {
		opt_const.TSTEP=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("SSTEP", &err_no);
	opt_datarange = extr_opt_datarange("SSTEP", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#SSTEP");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#SSTEP");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#SSTEP");
		return ret;
	} else {
		opt_const.SSTEP=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("MAXV", &err_no);
	opt_datarange = extr_opt_datarange("MAXV", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#MAXV");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MAXV");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MAXV");
		return ret;
	} else {
		opt_const.MAXV=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("AIRLENGTH", &err_no);
	opt_datarange = extr_opt_datarange("AIRLENGTH", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#AIRLENGTH");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#AIRLENGTH");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#AIRLENGTH");
		return ret;
	} else {
		opt_const.AIRLENGTH=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("MAXLENGTH", &err_no);
	opt_datarange = extr_opt_datarange("MAXLENGTH", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#MAXLENGTH");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MAXLENGTH");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MAXLENGTH");
		return ret;
	} else {
		opt_const.MAXLENGTH=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("LIMITV", &err_no);
	opt_datarange = extr_opt_datarange("LIMITV", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#LIMITV");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#LIMITV");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#LIMITV");
		return ret;
	} else {
		opt_const.LIMITV=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("MINV", &err_no);
	opt_datarange = extr_opt_datarange("MINV", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#MINV");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MINV");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MINV");
		return ret;
	} else {
		opt_const.MINV=(float)atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("MAXGEAR", &err_no);
	opt_datarange = extr_opt_datarange("MAXGEAR", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#MAXGEAR");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MAXGEAR");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MAXGEAR");
		return ret;
	} else {
		opt_const.MAXGEAR=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("MINGEAR", &err_no);
	opt_datarange = extr_opt_datarange("MINGEAR", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#MINGEAR");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MINGEAR");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#MINGEAR");
		return ret;
	} else {
		opt_const.MINGEAR=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("AIRS", &err_no);
	opt_datarange = extr_opt_datarange("AIRS", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#AIRS");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#AIRS");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#AIRS");
		return ret;
	} else {
		opt_const.AIRS=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("AIRE", &err_no);
	opt_datarange = extr_opt_datarange("AIRE", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#AIRE");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#AIRE");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#AIRE");
		return ret;
	} else {
		opt_const.AIRE=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("DISTANCE", &err_no);
	opt_datarange = extr_opt_datarange("DISTANCE", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#DISTANCE");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#DISTANCE");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#DISTANCE");
		return ret;
	} else {
		opt_const.DISTANCE=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("FEATUREINVALID", &err_no);
	opt_datarange = extr_opt_datarange("FEATUREINVALID", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#FEATUREINVALID");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#FEATUREINVALID");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#FEATUREINVALID");
		return ret;
	} else {
		opt_const.FEATUREINVALID=atoi(opt_param->PrmtCfg_value);
	}


	opt_param = extr_OptimizeStrategy("LIMITTHRESHOLD", &err_no);
	opt_datarange = extr_opt_datarange("LIMITTHRESHOLD", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#LIMITTHRESHOLD");
		return ret;
	}
	flag =checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag == 0||flag==-1) {	//判断是否可以进行类型转换
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#LIMITTHRESHOLD");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#LIMITTHRESHOLD");
		return ret;
	} else {
		opt_const.LIMITTHRESHOLD=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("THROUGHEXPERT", &err_no);
	opt_datarange = extr_opt_datarange("THROUGHEXPERT", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#THROUGHEXPERT");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag==0 || flag==-1) {
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#THROUGHEXPERT");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#THROUGHEXPERT");
		return ret;
	} else {
		opt_const.THROUGHEXPERT=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("PUSHERENGINE", &err_no);
	opt_datarange = extr_opt_datarange("PUSHERENGINE", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#PUSHERENGINE");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag==0 || flag==-1) {
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#PUSHERENGINE");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#PUSHERENGINE");
		return ret;
	} else {
		opt_const.PUSHERENGINE=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("COMGEARSTEP", &err_no);
	opt_datarange = extr_opt_datarange("COMGEARSTEP", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#COMGEARSTEP");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag==0 || flag==-1) {
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#COMGEARSTEP");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#COMGEARSTEP");
		return ret;
	}  else {
		opt_const.COMGEARSTEP=atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("IDELGEARSTEP", &err_no);
	opt_datarange = extr_opt_datarange("IDELGEARSTEP", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#IDELGEARSTEP");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag==0 || flag==-1) {
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#IDELGEARSTEP");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#IDELGEARSTEP");
		return ret;
	} else {
		opt_const.IDELGEARSTEP=atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("NEARLIMIT", &err_no);
	opt_datarange = extr_opt_datarange("NEARLIMIT", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#NEARLIMIT");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag==0 || flag==-1) {
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#NEARLIMIT");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#NEARLIMIT");
		return ret;
	} else {
		opt_const.NEARLIMIT=atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("POSTINITLENGTH", &err_no);
	opt_datarange = extr_opt_datarange("POSTINITLENGTH", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#POSTINITLENGTH");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value, int_pattern);
	if(flag==0 || flag==-1) {
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#POSTINITLENGTH");
		return ret;
	} else if(!(atoi(opt_param->PrmtCfg_value)>=atoi(opt_datarange->min) && atoi(opt_param->PrmtCfg_value)<=atoi(opt_datarange->max))){	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#POSTINITLENGTH");
		return ret;
	}  else {
		opt_const.POSTINITLENGTH=atoi(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("GEARLENGTHLIMIT", &err_no);
	opt_datarange = extr_opt_datarange("GEARLENGTHLIMIT", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#GEARLENGTHLIMIT");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag==0 || flag==-1) {
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#GEARLENGTHLIMIT");
		return ret;
	} else if(!(((atof(opt_datarange->min)<atof(opt_param->PrmtCfg_value))||fabs((atof(opt_datarange->min)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||fabs(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value))<opt_const.PRECISION))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#GEARLENGTHLIMIT");
		return ret;
	} else {
		opt_const.GEARLENGTHLIMIT=atof(opt_param->PrmtCfg_value);
	}

	opt_param = extr_OptimizeStrategy("TMPSPEEDACCURACY", &err_no);
	opt_datarange = extr_opt_datarange("TMPSPEEDACCURACY", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrOptConst#TMPSPEEDACCURACY");
		return ret;
	}
	flag = checkType(opt_param->PrmtCfg_value, float_pattern);
	if(flag==0 || flag==-1) {
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#TMPSPEEDACCURACY");
		return ret;
	}else if(!(((atof(opt_datarange->min)<(float)atof(opt_param->PrmtCfg_value))||(atof(opt_datarange->min)-(float)atof(opt_param->PrmtCfg_value)<opt_const.PRECISION))
			&& ((atof(opt_param->PrmtCfg_value)<atof(opt_datarange->max))||(atof(opt_datarange->max)-atof(opt_param->PrmtCfg_value)<opt_const.PRECISION)))){ 	//判断数据是否在指定范围内
		file_exception_decorator(OPT_TYPE_TRANSFORM_ERROR,&ret,"#extrOptConst#TMPSPEEDACCURACY");
		return ret;
	} else {
		opt_const.TMPSPEEDACCURACY=atof(opt_param->PrmtCfg_value);
	}

	return ret;
}
/**
 * @brief 读取手动区域（贯通实验和补机区域）
 * @return 异常结果
 */
OPT_EXTR_RESULT extrHandAreaInfo()
{
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	struct ROUTEINFO *routeData;
	enum cfgExtractRslt err_no=CFG_EXTR_SUCCESS;

	int i,tel_num,dist;
	//读取补机段的配置文件
	routeData = extr_RouteInfo("pusherEngine", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrHandAreaInfo#pusherEngine");
		return ret;
	}
	pusherEngines.len=routeData->part_num;
	pusherEngines.handAreas=(HANDAREA*)malloc(sizeof(HANDAREA)*(routeData->part_num));
	if(pusherEngines.handAreas==NULL){
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#extrHandAreaInfo#pusherEngine");
		return ret;
	}
	for(i=0;i<routeData->part_num;i++)
	{
		tel_num=routeData->in_routeinfo[i].startTelesemes;
		dist=routeData->in_routeinfo[i].startPosOffset;
		pusherEngines.handAreas[i].start= get_connect_kp(tel_num,dist);
		tel_num=routeData->in_routeinfo[i].endTelesemes;
		dist=routeData->in_routeinfo[i].endPosOffset;
		pusherEngines.handAreas[i].ending=get_connect_kp(tel_num,dist);
	}
	free(routeData->in_routeinfo);
	free(routeData);
	//读取贯通实验段的配置文件
	routeData = extr_RouteInfo("throughExpert", &err_no);
	if(err_no!=CFG_EXTR_SUCCESS)
	{
		file_exception_decorator(XML_EXTR_ERROR,&ret,"#extrHandAreaInfo#throughExpert");
		return ret;
	}
	tel_num=routeData->in_routeinfo->startTelesemes;
	dist=routeData->in_routeinfo->startPosOffset;
	throughExpert.start= get_connect_kp(tel_num,dist);
	tel_num=routeData->in_routeinfo->endTelesemes;
	dist=routeData->in_routeinfo->endPosOffset;
	float endV=routeData->in_routeinfo->endV;
	throughExpert.ending= get_connect_kp(tel_num,dist);
	throughExpert.endV=endV;
	free(routeData->in_routeinfo);
	free(routeData);
	return ret;
}
/**
 * @brief  读取文件的行数
 * @param fp  文件指针
 * @param line  文件的行数
 * @return 文件的结果
 */
OPT_EXTR_RESULT getLine(FILE* fp,int* line)
{
	*line=0;
	int ch;
	OPT_EXTR_RESULT ret;
	ret.file_result=OPT_EXTR_SUCCESS;
	if(fp == NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&ret,"#getLine");
		return ret;
	}
	while((ch=fgetc(fp))!=EOF){
		if(ch=='\n')
		{
			*line=*line+1;
		}
	}
	if(fseek(fp,0,SEEK_SET)==-1){
		file_exception_decorator(OPT_FILE_SEEK_ERROR,&ret,"#getLine");
	}
	return ret;
}
/**
 *@brief  打开xml文件
 *@param  fileName 文件名称
 *@param  permission  权限控制
 *@return  xml文件根节点
 */
mxml_node_t*  openXML(char* fileName,char* permission)
{
	FILE *fp;
	mxml_node_t *root;
	fp = fopen(fileName, permission);
	if(fp==NULL)
		fprintf(stderr,"open xml err!\n");
	root = mxmlLoadFile(NULL, fp,MXML_TEXT_CALLBACK);
	fclose(fp);
	return root;
}
/**
 * @brief 关闭xml文件
 * @param  root xml文件根节点
 */
void closeXML(mxml_node_t* root)
{
	mxmlDelete(root);
}

/**
 * @brief 检查传入的字符串类型是否可以转成指定的类型
 * @param str 待转换的字符串
 * @param pattern 正则表达式匹配规则
 * @return flag -1正则表达式pattern不能通过编译 1 表示类型匹配能够转换成功 0 表示类型不匹配不能转换成功
 * */
int checkType(char* str, char* pattern) {
	regex_t reg;
	int status;
	const size_t nmatch =1;
	regmatch_t pmatch[1];
	int flag;
	if (regcomp(&reg, pattern, REG_EXTENDED) < 0) {
		printf("正则表达式编译不能通过");
		flag = -1;
	}
	status = regexec(&reg, str, nmatch, pmatch, 0);
	if (status == REG_NOMATCH) {
		//printf("not match\n");
		flag = 0;
	} else {
		//printf("match\n");
		flag = 1;
	}
	regfree(&reg);
	return flag;
}

