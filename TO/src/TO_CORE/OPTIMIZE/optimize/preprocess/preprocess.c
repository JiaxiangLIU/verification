/**
 * @file    preprocess.c
 * @brief  优化预处理部分函数实现
 * @date  2014.3.31
 * @author sinlly
 * @note  包括加算坡度计算,时刻表附加时分计算等
 */
#include "preprocess.h"
#include "roadCategory.h"
#include <stdlib.h>
/**
 * 坡度临时结构
 */
struct gradientTemp {
	int start;  /**< 起始公里标*/
	int length;  /**< 坡长 */
	float value; /**< 坡度值 */
};
typedef struct gradientTemp GRADIENTTEMP;
/**
 * 曲线临时结构
 */
struct curveTemp {
	int start;  /**< 起始公里标*/
	int end; /**<终止公里标*/
	int length; /**<曲线长度*/
	int radius; /**< 曲线半径*/
	float value;/**< 曲线的加算坡度值*/
};
typedef struct curveTemp CURVETEMP;
/**
 *  隧道临时结构
 */
struct tunnelTemp {
	int start; /**< 起始公里标*/
	int end;  /**<终止公里标*/
	int length; /**<隧道长度*/
	float value;/** 隧道的加算坡度值*/
};
typedef struct tunnelTemp TUNNELTEMP;

/**
 * @brief 计算加算坡度
 * @param endPost  终止公里标
 * @return 异常处理结果
 */
PRE_PROCESS_RESULT  mergeGradient(int endPost) ;

/**
 *@brief 处理车站间的时间偏差的一些初始化
 *@return 异常处理结果
 */
PRE_PROCESS_RESULT  generateRunTime();

MGRADIENT* mGradients;
MGRADIENT *new_gradients;
int MAX_NEW_GRADIENTS;
int MAX_MGRADIENT;
int MAX_INTERVALRUNTIME;
float totalAvgV;
int mOffset;
int mDirection;
float timeLimit;

#define MAX_MGRADIETTEMP (MAX_GRADIENT*2)
/**
 * @brief 该函数实现计算新的单个加算坡度
 * @param  old_gradients   代表原来的加算坡度值
 * @param length  代表原来计算坡度数组的长度
 * @param current_x  代表当前的公里标
 * @param  mem_index表示当前下标
 * @param  new_mem_index表示新的下标
 * @return  单个加算坡度值
 */
float getPointGradient(MGRADIENT *old_gradients,int length,int loco_length,int current_x,int mem_index,int *new_mem_index)
{
	int start_post;
	int count;
	int end_post;
	float  result_gradient;
	start_post=current_x-loco_length;
	end_post=current_x;
	count = mem_index;
	while(count<=length-1)
	{
		if (old_gradients[count].start < start_post)
		{
			count = count + 1;
		}
		else
		{
			break;
		}
	}

	*new_mem_index = count;
	result_gradient = 0;
	while(count <= (length-1) &&  old_gradients[count].start < end_post)
	{
		if (old_gradients[count].end <= end_post)
		{
			result_gradient = result_gradient + (old_gradients[count].end - old_gradients[count].start) * old_gradients[count].value;
		}
		else
		{
			result_gradient = result_gradient + (end_post - old_gradients[count].start) * old_gradients[count].value;
		}
		count = count + 1;

	}
	result_gradient = result_gradient / loco_length;
	return result_gradient;

}

/**
 * @brief  该函数实现多质点模型来计算新的加算坡度
 * @param  old_gradients   原始加算坡度
 * @param  length  代表原来计算坡度数组的长度
 * @param  loco_length   代表列车的长度
 * @param  end_post: 行程终止公里标
 */
PRE_PROCESS_RESULT  newGradientGeneration(MGRADIENT *old_gradients,int length,int loco_length,int end_post)
{
	PRE_PROCESS_RESULT pre_ret;
	pre_ret.preprocess_result=PRE_PROCESS_SUCCESS;
	int start_post;
	int mem_index;
	int *new_mem_index;
	int j=0;
	new_mem_index=&j;
	int i;
	start_post=old_gradients[0].start + loco_length;
	//	end_post = old_gradients[MAX_MGRADIENT-1].start;
	mem_index = 0;
	new_gradients = (MGRADIENT*) malloc(sizeof(MGRADIENT)*((int)((end_post-start_post)/10)+1));
	if(new_gradients==NULL)
	{
		pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#newGradGen#new_gradients");
		return pre_ret;
	}
	MAX_NEW_GRADIENTS=(int)((end_post-start_post)/10)+1 ;
	for (i=start_post; i<=end_post;i=i+10)
	{
		new_gradients[(int)((i - start_post)/10)].start = i;
		new_gradients[(int)((i - start_post)/10)].end= i + 10;
		new_gradients[(int)((i - start_post)/10)].value=getPointGradient(old_gradients,length,loco_length,i,mem_index,new_mem_index);
		mem_index=*new_mem_index;
	}
	/**仅仅是保存文件*/
	opt_log_info("连续加算坡度结果写入开始");
	FILE* fid = fopen("././ret/new_continue_gradients", "w");
	if (fid == NULL) {
		fprintf(stderr, "open new_gradients err!\n");
	}

	for (i = 0; i < MAX_MGRADIENT; i++) {
		fprintf(fid, "%d\t%d\t%f\n", new_gradients[i].start,
				new_gradients[i].end, new_gradients[i].value);
	}
	fclose(fid);
	opt_log_info("连续加算坡度结果写入成功");
	return pre_ret;
}

/**
 * @brief 计算加算坡度
 * @param endPost  终止公里标
 * @return 异常处理结果
 */
PRE_PROCESS_RESULT  mergeGradient(int endPost) {
	//	printf("mergeGradient started!\n");
	//初始化
	PRE_PROCESS_RESULT pre_ret;
	pre_ret.preprocess_result=PRE_PROCESS_SUCCESS;
	GRADIENTTEMP* gradientT = (GRADIENTTEMP*) malloc(
			sizeof(GRADIENTTEMP) * MAX_GRADIENT);
	if(gradientT==NULL)
	{
		pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#mergeGradient#gradientT");
		free(gradientT);
		return pre_ret;
	}
	CURVETEMP* curveT = (CURVETEMP*) malloc(sizeof(CURVETEMP) * MAX_CURVE);
	if(curveT==NULL)
	{
		pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#mergeGradient#curveT");
		free(curveT);
		return pre_ret;
	}
	TUNNELTEMP* tunnelT = (TUNNELTEMP*) malloc(sizeof(TUNNELTEMP) * MAX_TUNNEL);
	if(tunnelT==NULL)
	{
		pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#mergeGradient#tunnelT");
		free(tunnelT);
		return pre_ret;
	}
	//	float lcars = 69* 11;
	float lcars=locoInfo.totalLength;
	int direction = 1;
	if (gradients[0].start - gradients[1].start > 0)
		direction = -1;
	int i = 0;
	for (; i < MAX_GRADIENT; i++) {
		gradientT[i].start = gradients[i].start;
		gradientT[i].length = gradients[i].length;
		gradientT[i].value = gradients[i].value;
	}
	i = 0;
	for (; i < MAX_CURVE; i++) {
		curveT[i].start = curves[i].start;
		curveT[i].end = curves[i].end;
		curveT[i].length = abs(curveT[i].end - curveT[i].start);
		curveT[i].radius = curves[i].radius;
	}
	i = 0;
	for (; i < MAX_TUNNEL; i++) {
		tunnelT[i].start = tunnels[i].start;
		tunnelT[i].end = tunnels[i].end;
		tunnelT[i].length = abs(tunnelT[i].end - tunnelT[i].start);
	}
	//开始计算加算坡度
	mOffset = 0;
	mDirection = direction;
	i = 0;
	for (; i < MAX_CURVE; i++) //计算曲线的加算坡度
	{
		curveT[i].value = curveT[i].length * 600.0 / (curveT[i].radius * lcars);
	}
	i = 0;
	for (; i < MAX_TUNNEL; i++) //计算隧道的加算坡度
	{
		tunnelT[i].value = 0.00013 * tunnelT[i].length;
	}
	int maxLength = MAX_GRADIENT * 2 + MAX_TUNNEL * 2 + MAX_CURVE * 2;
	int index = 0;
	int points[maxLength];
	i = 0;
	//将曲线，隧道，坡度点映射到point中
	for (; i < MAX_GRADIENT; i++) {
		points[index] = gradientT[i].start;
		++index;
		points[index] = gradientT[i].start + gradientT[i].length;
		++index;
	}
	i = 0;
	for (; i < MAX_CURVE; i++) {
		points[index] = curveT[i].start;
		++index;
		points[index] = curveT[i].start + curveT[i].length;
		++index;
	}
	i = 0;
	for (; i < MAX_TUNNEL; i++) {
		points[index] = tunnelT[i].start;
		++index;
		points[index] = tunnelT[i].start + tunnelT[i].length;
		++index;
	}
	//去除point中重复的点
	int upoints[maxLength];
	zeroIntArray(upoints, maxLength);
	int upointsIndex = 0;
	unique(points, maxLength, upoints, &upointsIndex);

	//     upointsIndex=upointsIndex-1;
	quickSort(upoints, 0, upointsIndex - 1);
	float tGradient[upointsIndex - 1];
	i = 0;
	zerofloatArray(tGradient, upointsIndex - 1);

	mGradients = (MGRADIENT*) malloc(
			sizeof(MGRADIENT) * MAX_MGRADIETTEMP);
	if(mGradients==NULL)
	{
		pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#mergeGradient#mGradients");
		return pre_ret;
	}
	mGradients[0].start = 0;
	mGradients[0].end = 0;
	mGradients[0].value = 0.0;
	i = 0;
	int tempStart=-1;
	int tempEnd=-1;
	for (; i < MAX_GRADIENT; i++) {
		tempStart = findIntegerIndex(upoints, upointsIndex, gradientT[i].start);
		if (tempStart < 0) {
			pre_exception_decorator(PRE_FIND_INDEX_ERROR,&pre_ret,"#mergeGradient#tmpStart");
			return pre_ret;
		}
		tempEnd = findIntegerIndex(upoints, upointsIndex,
				gradientT[i].start + gradientT[i].length);
		if (tempEnd < 0) {
			pre_exception_decorator(PRE_FIND_INDEX_ERROR,&pre_ret,"#mergeGradient#tmpEnd");
			return pre_ret;
		}
		float currentGradient = gradientT[i].value;
		int j = tempStart;
		for (; j <= tempEnd - 1; j++) {
			tGradient[j] = tGradient[j] + currentGradient;
		}
	}
	i = 0;
	for (; i < MAX_CURVE; i++) {
		tempStart = findIntegerIndex(upoints, upointsIndex, curveT[i].start);
		if (tempStart < 0) {
			pre_exception_decorator(PRE_FIND_INDEX_ERROR,&pre_ret,"#mergeGradient#tmpStart");
			return pre_ret;
		}
		tempEnd = findIntegerIndex(upoints, upointsIndex,
				curveT[i].start + curveT[i].length);
		if (tempEnd < 0) {
			pre_exception_decorator(PRE_FIND_INDEX_ERROR,&pre_ret,"#mergeGradient#tmpEnd");
			return pre_ret;
		}
		float currentGradient = curveT[i].value;
		int j = tempStart;
		for (; j <= tempEnd - 1; j++) {
			tGradient[j] = tGradient[j] + currentGradient;
		}
	}
	i=0;
	for(;i<MAX_TUNNEL;i++)
	{
		tempStart=findIntegerIndex(upoints,upointsIndex,tunnelT[i].start);
		if(tempStart<0)
		{
			pre_exception_decorator(PRE_FIND_INDEX_ERROR,&pre_ret,"#mergeGradient#tmpStart");
			return pre_ret;
		}
		tempEnd=findIntegerIndex(upoints,upointsIndex,tunnelT[i].start+tunnelT[i].length);
		if(tempEnd<0)
		{
			pre_exception_decorator(PRE_FIND_INDEX_ERROR,&pre_ret,"#mergeGradient#tmpEnd");
			return pre_ret;
		}
		float currentGradient=tunnelT[i].value;
		int j=tempStart;
		for(;j<=tempEnd-1;j++)
		{
			tGradient[j]=tGradient[j]+currentGradient;
		}
	}

	tempStart = upoints[0];
	tempEnd = upoints[1];
	float tempGradient = tGradient[0];
	int finalIndex = 0;
	i = 0;
	for (; i < upointsIndex - 1; i++) {
		if (i == 0)
			continue;
		float tempGradient2 = tGradient[i];
		if (tempGradient == tempGradient2)
			tempEnd = upoints[i + 1];
		else {
			if (tempEnd - tempStart <= 35) {
				tempEnd = upoints[i + 1];
			} else {
				mGradients[finalIndex].start = tempStart;
				mGradients[finalIndex].end = tempEnd;
				mGradients[finalIndex].value = tempGradient;
				tempStart = tempEnd;
				tempEnd = upoints[i + 1];
				tempGradient = tempGradient2;
				finalIndex = finalIndex + 1;
			}
		}

	}
	MAX_MGRADIENT = finalIndex;

	free(gradientT);
	gradientT = NULL;
	free(curveT);
	curveT = NULL;
	free(tunnelT);
	tunnelT = NULL;
	/**仅仅是文件保存*/
	opt_log_info("加算坡度结果写入开始");
	FILE* fid = fopen("././ret/mGradient", "w");
	if (fid == NULL) {
		fprintf(stderr, "open mGradient err!\n");
	}

	for (i = 0; i < MAX_MGRADIENT; i++) {
		fprintf(fid, "%d\t%d\t%f\n", mGradients[i].start,
				mGradients[i].end, mGradients[i].value);
	}
	fclose(fid);
	opt_log_info("加算坡度结果写入成功");

	pre_ret=newGradientGeneration(mGradients,finalIndex,lcars,endPost);//产生新的加算坡度值
	if(pre_ret.preprocess_result!=PRE_PROCESS_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#mergeGradients";
		strcat(desc,pre_ret.description);
		pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
		return pre_ret;
	}
	return pre_ret;
}
/**
 * @brief 对车站间时间进行初始化处理
 * @return 异常处理结果
 * @note 该函数修改了intervalRuntimeUnits结构后三个数据项
 */
PRE_PROCESS_RESULT generateRunTime() {
	//	printf("generateRunTime started!\n");
	//	printf("\r\n");
	PRE_PROCESS_RESULT pre_ret;
	pre_ret.preprocess_result=PRE_PROCESS_SUCCESS;
	int i = 0;
	int sxx = -1;
	if (stations[1].start > stations[0].start)
		sxx = 1;
	for (; i < MAX_INTERVALRUNTIME; i++) {
		int startGlb = 0;
		int j = 0;
		for (; j < MAX_STATION; j++)  //找到起始车站公里标
		{
			if (intervalRuntimeUnits[i].startCzh == stations[j].id) {
				startGlb = stations[j].start;
				break;
			}
		}
		int stime[3] = { 0 };
		split(intervalRuntimeUnits[i].startTime, stime, ':');
		int startTime = (stime[0] * 60 + stime[1]) * 60+stime[2];
		//获得起始车站的公里标所在的限速的index
		int sXsIndex = -1;
		j = 0;
		if (sxx == -1) {
			for (; j < MAX_LIMIT; j++) {
				if (startGlb <= limits[j].start
						&& startGlb >= limits[j].end) {
					sXsIndex = j;
					break;
				}
			}
		} else {
			for (; j < MAX_LIMIT; j++) {
				if (startGlb >= limits[j].start
						&& startGlb <= limits[j].end) {
					sXsIndex = j;
					break;
				}
			}
		}

		int endGlb = 0;
		j = 0;
		for (; j < MAX_STATION; j++)  //找到终止车站公里标
		{
			if (intervalRuntimeUnits[i].endCzh == stations[j].id) {
				endGlb = stations[j].start;
				if (j == MAX_STATION - 1)
					endGlb = limits[MAX_LIMIT - 1].end;
				break;
			}
		}

		int etime[3] = { 0 };
		split(intervalRuntimeUnits[i].arriveTime, etime, ':');
		int endTime = (etime[0] * 60 + etime[1]) * 60+etime[2];
		//获得终点车站的公里标所在的限速的index
		int eXsIndex = -1;
		j = 0;
		if (sxx == -1) {
			for (; j < MAX_LIMIT; j++) {
				if (endGlb <= limits[j].start
						&& endGlb >= limits[j].end) {
					eXsIndex = j;
					break;
				}
			}
		} else {
			for (; j < MAX_LIMIT; j++) {
				if (endGlb >= limits[j].start
						&& endGlb <= limits[j].end) {
					eXsIndex = j;
					break;
				}
			}
		}
		if( sXsIndex!=-1 && eXsIndex==-1)
			eXsIndex=MAX_LIMIT-1;

		float totalGlb = abs(startGlb - endGlb) / 1000.0; //区间公里
		float runTime;
		if(endTime<startTime || fabs(endTime-startTime)<opt_const.PRECISION)
			runTime=(endTime+(24*60*60-startTime))/60.0; //午夜12点-startTime+endTime为期间的时间间隔
		else
			runTime = abs(startTime - endTime) / 60.0; //运行时分
		float normalV = totalGlb / runTime * 60.0; //正常速度
		//      float carLength=locoInfo.totalLength; //列车长度
		float carLength = locoInfo.totalLength;
		float deltT = 0;
		if (sXsIndex == -1 || eXsIndex == -1 || (sXsIndex > eXsIndex)) {
			pre_exception_decorator(PRE_NOT_FIND_LIMIT,&pre_ret,"#geneRunTime");
			return pre_ret;
		} else {
			float jl; //慢行距离
			int v; //慢行速度
			if (sXsIndex == eXsIndex) {
				v = limits[sXsIndex].limit;
				jl = totalGlb * 1000.0;
				if (v <= 60) {
					deltT = 0.06
							* ((carLength + 22.0 + 40.0 + jl) /(float) (v - 3)
									- (carLength + 62.0 + jl) / normalV)
									+ (normalV -(float) v + 3.0) / 5.0 * 0.25;
				}
			} else {
				int temp = sXsIndex;
				while (temp <= eXsIndex) {
					if (temp == eXsIndex) {
						jl = (float)abs(limits[temp].start - endGlb);
					} else {
						if (temp == sXsIndex) {
							jl = (float)abs(limits[temp].end- startGlb);
						} else
							jl = (float)abs(
									limits[temp].end
									- limits[temp].start);
					}
					v = limits[temp].limit;
					if (v < 60)
						deltT = 0.06
						* ((carLength + 22.0 + 40.0 + jl) / (float)(v - 3)
								- (carLength + 62.0 + jl) / normalV)
								+ (normalV - (float)v + 3.0) / 5.0 * 0.25;
					++temp;
				}
			}
		}
		deltT =(float) roundInt(deltT * 60.0) / 60.0;
		intervalRuntimeUnits[i].runTime = runTime;
		intervalRuntimeUnits[i].averageV = totalGlb / (runTime + deltT )* 60.0; //这个地方之前有错误，已改（之前为deltT/60)
		intervalRuntimeUnits[i].timeDiff = deltT;
		if(deltT<0)
			intervalRuntimeUnits[i].totalRuntime =  runTime;
		else
			intervalRuntimeUnits[i].totalRuntime = deltT + runTime;
	}

	timeLimit=0;
	totalAvgV=0;
	int k=0;
	for(;k<MAX_INTERVALRUNTIME;k++)
	{
		timeLimit=timeLimit+intervalRuntimeUnits[k].totalRuntime*60;
		//		printf("%d, %d , %f, %f,%f\n",intervalRuntimeUnits[k].startCzh,intervalRuntimeUnits[k].endCzh,intervalRuntimeUnits[k].runTime,intervalRuntimeUnits[k].totalRuntime,intervalRuntimeUnits[k].averageV);
	}
	totalAvgV=((stations[MAX_STATION-1].start-stations[0].start)/timeLimit)*3.6;
	return pre_ret;
}

/**
 * @brief 分段预处理主入口函数
 * @param startPost   起始公里标
 * @param endPost   终止公里标
 * @return  异常处理结果
 */
OPTIMIZE_RESULT  preprocess(int startPost,int endPost) {
	PRE_PROCESS_RESULT pre_ret;
	OPTIMIZE_RESULT opt_ret;
	opt_ret.opt_rest=OPTIMIZE_SUCCESS;

	opt_log_info("加算坡度计算开始");
	pre_ret = mergeGradient(endPost);
	if (pre_ret.preprocess_result!=PRE_PROCESS_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#preprocess.c";
		strcat(desc,pre_ret.description);
		pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
		return  pre_result_adapter(pre_ret);
	}
	opt_log_info("计算坡度计算结束");

	opt_log_info("列车时刻表处理开始");
	pre_ret = generateRunTime();
	if (pre_ret.preprocess_result!=PRE_PROCESS_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#preprocess.c";
		strcat(desc,pre_ret.description);
		pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
		return  pre_result_adapter(pre_ret);
	}
	opt_log_info("列车时刻表处理结束");

	opt_log_info("坡段临界速度计算开始");
	CRITICAL_V*  cv = (CRITICAL_V*)malloc(sizeof(CRITICAL_V)*MAX_NEW_GRADIENTS);
	if(cv==NULL)
	{
		pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#cptCriticalV");
		return pre_result_adapter(pre_ret);
	}
	opt_ret=computeCriticalV(new_gradients,MAX_NEW_GRADIENTS,cv);
	if(opt_ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#preprocess.c";
		strcat(desc,opt_ret.description);
		optimize_exception_decorator(opt_ret.opt_rest,&opt_ret,desc);
		return opt_ret;
	}
	opt_log_info("坡段临界速度计算结束");

	opt_log_info("坡道分段开始");
	opt_ret=computeRoadCategory(new_gradients,MAX_NEW_GRADIENTS,cv,startPost,endPost);
	if(opt_ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#preprocess.c";
		strcat(desc,opt_ret.description);
		optimize_exception_decorator(opt_ret.opt_rest,&opt_ret,desc);
		return  opt_ret;
	}
	free(cv);
	free(new_gradients);
	opt_log_info("坡道分段结束");

	char desc[EXP_DESC_LENGTH]="优化初始化成功";
	strcpy(pre_ret.description,desc);
	return pre_result_adapter(pre_ret);
}

