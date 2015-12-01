/**
 * @file    optimizeTools.c
 * @brief  优化部分会用到的公共方法实现
 * @date  2014.4.8
 * @author sinlly
 * @note  主要包括油耗评估等
 */

#include "optimizeTools.h"
int MAX_OPTSCHLENGTH;
float timeLimit;
int MAX_LIMIT;
LIMIT* limits;
ROADCATEGORY* rc;
OPTIMIZERESULT* optimizeResult;

/**
 * @brief 评估油耗
 * @param E  整体油耗
 * @param T  整体时间
 * @param diffTime  时间偏差
 * @param optSch 优化曲线结果
 * @param srcTime  司机运行时间
 * @param srcConsumption 司机运行油耗
 */
void evaluate(float* E, float* T, float* diffTime, OPTSCH* optSch,int len,float srcTime,float srcConsumption) {
	*E = 0;
	*T = 0;
	float srcE=srcConsumption;
	int i = 0;
	for (; i < len; i++) {
		*E = *E + optSch[i].consumption;
		*T = *T + optSch[i].time;
	}
	FILE* fid = fopen("././ret/online.txt","w");
	if(fid==NULL){
		fprintf(stderr,"open online.txt err!\n");
	}
	float delta_t=0;
	float delta_e=0;
	float schedule=0;
	for(i=1;i<MAX_STATION;i++)
	{
		int j=0;
		float runTime=0;
		for(;j<MAX_INTERVALRUNTIME;j++)
		{
			if(intervalRuntimeUnits[j].endCzh==stations[i].id)
			{
				runTime= intervalRuntimeUnits[j].totalRuntime;
				break;
			}
		}
		float start,end;
		start=stations[i-1].start;
		end=stations[i].start;
		if(start<stations[0].start)
			continue;
		if(end>stations[MAX_STATION-1].start)
			continue;
		int startIndex=0;
		int endIndex=len;
		for( j=1;j<len;j++)
		{
			if ((fabs(start-optSch[j-1].start)<opt_const.PRECISION || start>optSch[j-1].start)  && ( fabs(optSch[j].start-start)<opt_const.PRECISION || start<optSch[j].start))
			{
				startIndex=j-1;
				break;
			}
		}
		for( j=len-1;j>=0;j--)
		{
			if((fabs(optSch[j].start-end )<opt_const.PRECISION  || end<optSch[j].start)  && ( fabs(end-optSch[j-1].start)<opt_const.PRECISION || end>optSch[j-1].start))
			{
				endIndex=j-1;
				break;
			}
		}
		float t=0;
		float e=0;
		for(j=startIndex;j<=endIndex;j++)
		{
			t=t+optSch[j].time;
			e=e+optSch[j].consumption;
		}
		//		startIndex=0;
		//		for(j=1;MAX_GEDATA;j++)
		//		{
		//              float curGlb=mOffset-geDatas[j].start;
		//              float lastGlb=mOffset-geDatas[j-1].start;
		//              if (start>=lastGlb&& start<=curGlb)
		//              {
		//                       startIndex=j-1;
		//                       break;
		//               }
		//		}
		//		endIndex=MAX_GEDATA;
		//		for( j=MAX_GEDATA-1;j>=0;j--)
		//				{
		//			        float curGlb=mOffset-geDatas[j].start;
		//			        float lastGlb=mOffset-geDatas[j-1].start;
		//					if(end<=curGlb && end>=lastGlb)
		//					{
		//					       endIndex=j-1;
		//					       break;
		//					}
		//				}
		//		double t2=(endIndex-startIndex+1);
		schedule=schedule+runTime*60;
		delta_t=delta_t+t;
		delta_e=delta_e+e;
		opt_log_info("(司机%d-时刻表%d)%d - %d 时间差:%f; 时间: %f, 油耗: %f , 时刻表时间: %f",i-1,i,stations[i-1].id,stations[i].id,t-runTime*60,t,e,runTime*60);
		fprintf(fid,"%d  %d  %f  %f  %f\n",stations[i-1].id,stations[i].id,e,t,runTime*60);
	}
	fclose(fid);
	*diffTime = *T-timeLimit ;
	printf("总油耗:%f\n", *E);
	printf("总时间:%f\n", *T);
	printf("显示车站的时刻表总时间:%f\n",schedule);
	printf("显示车站的总油耗:%f\n",delta_e);
	printf("显示车站的总时间:%f\n",delta_t);
	printf("总油耗节省百分比:%f\n",(srcE-*E)/srcE);
	printf("时间偏差:%f\n", *diffTime);

	opt_log_info("总油耗:%f", *E);
	opt_log_info("总时间:%f", *T);
	opt_log_info("显示车站的时刻表总时间:%f",schedule);
	opt_log_info("显示车站的总油耗:%f",delta_e);
	opt_log_info("显示车站的总时间:%f",delta_t);
	opt_log_info("总油耗节省百分比:%f",(srcE-*E)/srcE);
	opt_log_info("时间偏差:%f", *diffTime);
}


/**
 * @brief 计算一定范围内的时间
 * @param  indexS   初始index
 * @param indexE  终止index
 * @param  optSch  优化结果
 * @return 返回indexS-indexE之间的时间
 */
float computeTime(int indexS, int indexE, OPTSCH* optSch) {
	float time = 0;
	int i = indexS;
	for (; i < indexE; i++)
		time = time + optSch[i].time;
	return time;
}
/**
 * @brief 获得所在坡度段的限速大小
 * @param start  坡段的起始公里标
 * @param ending  坡段的终止公里标
 * @return  返回该段的限速
 */
int getCategoryLimitV(int start, int ending) {
	int indexS = 0;
	int indexE = 0;
	int limitV = 0;
	int i;
	for (i = 0; i < MAX_LIMIT; i++) {
		int startL =  limits[i].start;
		int endingL = limits[i].end;
		if (start >= startL && start <= endingL) //找到start位置的限速index
			indexS = i;
		if (ending >= startL && ending <= endingL) //找到ending位置的限速index
			indexE = i;
		if (indexS != 0 && indexE != 0)
			break;

	}
	if (indexS != 0 && indexE != 0 && indexS != indexE) {
		for (i = indexS; i <= indexE; i++)
			limitV = limitV + limits[i].limit;
		limitV = limitV / (indexE - indexS);
	} else {
		if (indexS != 0 && indexE != 0 && indexS == indexE) {
			limitV = limits[i].limit; //这里错误，应该改为limits[indexS].limit;
		} else
			limitV = 80 - 5;
	}
	return limitV;
}
/**
 * @brief 获得当前的start位置和ending位置所在优化结果optSch中的对应的速度和index
 * @param optSch  优化结果
 * @param  start  起始公里标
 * @param  ending  终止公里标
 * @param  v1   起始公里标在optSch中对应的速度
 * @param  index1  起始公里标在optSch中对应的index
 * @param  v2  终止公里标在optSch中对应的速度
 * @param  index2  终止公里标在optSch中对应的index
 *
 */
void getSchVelocity(OPTSCH* optSch, int len, float start, float ending, float* v1,
		int* index1, float* v2, int* index2) {
	int flag1 = 0;
	int flag2 = 0;
	*v1 = optSch[0].velocity;
	*index1 = 0;
	int i = 0;
	float delta1 = 0;
	float delta2 = 0;
	for (; i < len; i++) {
		//查找起始公里标所在的位置，找到距离该点间隔最远的那个点
		if (start < optSch[i].start) {
			if (i > 0 && (start > optSch[i - 1].start || fabs(start-optSch[i-1].start)<opt_const.PRECISION)&& flag1 == 0) {
				delta1 = optSch[i].start - start;
				delta2 = start - optSch[i - 1].start;
				if (delta1 > delta2 || fabs(delta1-delta2)<opt_const.PRECISION) {
					*v1 = optSch[i].velocity;
					*index1 = i;
					flag1 = 1;
				} else {
					*v1 = optSch[i - 1].velocity;
					*index1 = i - 1;
					flag1 = 1;
				}
			} else {
				if (flag1 == 0) {
					*v1 = optSch[i].velocity;
					*index1 = i;
					flag1 = 1;
				}
			}
		}
		if (fabs(start- optSch[i].start)<opt_const.PRECISION) {
			if (flag1 == 0) {
				*v1 = optSch[i].velocity;
				*index1 = i;
				flag1 = 1;
			}
		}
		//查找终止公里标所在的位置,找到距离该点间隔最远的那个点
		if (ending < optSch[i].start) {
			if (i > 0 && (ending > optSch[i - 1].start || fabs(ending-optSch[i-1].start)<opt_const.PRECISION) && flag2 == 0) {
				delta1 = optSch[i].start - ending;
				delta2 = ending - optSch[i - 1].start;
				if (delta1 > delta2 || fabs(delta1-delta2)< opt_const.PRECISION) {
					*v2 = optSch[i].velocity;
					*index2 = i;
					flag2 = 1;
				} else {
					*v2 = optSch[i - 1].velocity;
					*index2 = i - 1;
					flag2 = 1;
				}
			} else {
				if (flag2 == 0) {
					*v2 = optSch[i].velocity;
					*index2 = i;
					flag2 = 1;
				}
			}
		}
		if (fabs(ending - optSch[i].start)< opt_const.PRECISION) {
			if (flag2 == 0) {
				*v2 = optSch[i].velocity;
				*index2 = i;
				flag2 = 1;
			}
		}
		if (flag1 == 1 && flag2 == 1) {
			break;
		}
	}
	//若终止公里标大于optSch中的最大公里标则默认最后一个公里标所在的位置为该终点公里标所在的位置
	if (ending > optSch[len - 1].start) {
		*v2 = optSch[len - 1].velocity;
		*index2 = len - 1;
	}
}
/**
 *@brief 获得当前start位置和ending位置所在的roadCategory中对应的坡度类型以及index
 *@param start  起始公里标
 *@param ending  终止公里标
 *@param flagS  起始公里标所在的roadCategory中对应的坡度类型
 *@param indexS  起始公里标所在的roadCategory中对应的index
 *@param flagE  终止公里标记所在的roadCategory中对应的坡度类型
 *@param indexE  终止公里标所在的roadCategory中对应的index
 *
 */
void  getCategoryFlag(int start, int ending, int* flagS, int* indexS, int* flagE,
		int* indexE) {
	int flag1 = 0;
	int flag2 = 0;
	*flagS = 0;
	*indexS = 0;
	*flagE = 0;
	*indexE = 0;
	if (start == 0) {
		*flagS = rc[0].flag;
		*indexS = 0;
	}
	int i;
	for (i = 0; i < MAX_ROADCATEGORY; i++) {
		if (start <= rc[i].end && start >= rc[i].start) {
			*flagS = rc[i].flag;
			*indexS = i;
			flag1 = 1;
		}
		if (ending <= rc[i].end && ending >= rc[i].start) {
			*flagE = rc[i].flag;
			*indexE = i;
			flag2 = 1;
		}
		if (flag1 == 1 && flag2 == 1)
			break;
	}

}
/**
 * @brief 存取OPTSCH形式的优化结果
 * @param fileName 文件名
 * @param optSch  优化结果
 */
void saveResult(OPTSCH* optSch,int len,char* fileName) {
	int i;
	char prefix[64]="./ret/";
	FILE* fid = fopen(strcat(prefix,fileName), "w");
	if (fid == NULL) {
		char result[64]="open  ";
		fprintf(stderr, strcat(strcat(result,fileName)," err!\n"));
	}

	for (i = 0; i < len; i++) {
		fprintf(fid, "%f\t%d\t%f\t%f\t%f\t%f\n", optSch[i].start,
				optSch[i].gear, optSch[i].velocity, optSch[i].time,
				optSch[i].consumption, optSch[i].acc);
	}
	fclose(fid);
}
/**
 * @brief 存取最终OPTSCHCURVE形式的结果
 * @param optSch  优化结果
 */
void saveFinalResult(OPTSCHCURVE* final_curve,int len,char* fileName) {
	int i;
	char prefix[64]="./ret/";
	FILE* fid = fopen(strcat(prefix,fileName), "w");
	if (fid == NULL) {
		char result[64]="open  ";
		fprintf(stderr, strcat(strcat(result,fileName)," err!\n"));
	}

	for (i = 0; i < len; i++) {
		//		char* flag;
		//		switch(final_curve[i].flag)
		//		{
		//		   case PUSHER_ENGING_AREA:
		//			  flag="pusher_enging_area";break;
		//		   case RUN_THROUGH_TEST:
		//			   flag="run_through_test";break;
		//		   case AIR_DAMPING_BRAKE:
		//			    flag="air_damping_brake";break;
		//		   case AIR_DAMPING_AVG:
		//			    flag="air_damping_avg";break;
		//		   case FORBID_STOP_ON_GRAD:
		//			     flag="forbid_stop_on_grad";break;
		//		   case FORBID_STOP_CROSS_GRAD:
		//			     flag="forbid_stop_cross_grad";break;
		//		   default:
		//			      flag="nothing";
		//		}
		fprintf(fid, "%d\t%f\t%f\t%f\t%d\t%f\t%f\t%f\n", final_curve[i].tel_num,
				final_curve[i].cross_dist, final_curve[i].tel_kp, final_curve[i].ford_time,
				final_curve[i].gear, final_curve[i].time,final_curve[i].con_tel_kp,final_curve[i].velocity);
		//		printf("%s\n",flag);
	}
	fclose(fid);
}

/**
 * @brief 从优化结果OPTSCH中寻找start和end所在OPTSCH中的index,取最外层
 * @param start   起始公里标
 * @param end   终止公里标
 * @param startIndex    找到的起始公里标对应的OPTSCH中的index
 * @param endIndex   找到的终止公里标对应的OPTSCH中的index
 */
void get_segment_index(OPTSCH* optSch,float start,float end,int* startIndex,int* endIndex,int len)
{
	int j;
	*startIndex=0;
	*endIndex=0;
	for(j=1;j<len;j++)
	{
		if ((fabs(start-optSch[j-1].start)<opt_const.PRECISION || start>optSch[j-1].start)  && ( fabs(optSch[j].start-start)<opt_const.PRECISION || start<optSch[j].start))
		{
			*startIndex=j-1;
			break;
		}
	}
	for(j=*startIndex;j<len-1;j++)
	{
		if ((fabs(end-optSch[j].start)<opt_const.PRECISION || end>optSch[j].start)  && ( fabs(optSch[j+1].start-end)<opt_const.PRECISION || end<optSch[j+1].start))
		{
			*endIndex=j;
			break;
		}
	}
	if(*endIndex==0)
		*endIndex=len-1;
}
/**
 * @brief 给定信号机及越过距离，获得连续的公里标
 * @param tel_num 信号机编号
 * @param cross_dist 越过距离
 * @return 连续的公里标
 */
float get_connect_kp(int tel_num,int cross_dist)
{
	int i;
	float dist=0;
	for(i=0;i<MAX_TELESEME;i++)
	{
		if(tel_num==telesemes[i].tel_num)
		{
			dist=telesemes[i].post+cross_dist;
			break;
		}
	}
	return dist;
}
