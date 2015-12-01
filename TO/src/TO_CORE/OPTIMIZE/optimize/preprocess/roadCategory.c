/**
 * @file    roadCategory.c
 * @brief  坡段划分主函数实现
 * @date  2014.4.2
 * @author danny
 * @note  包括加算坡度计算,时刻表附加时分计算等
 */

#include "roadCategory.h"
#include <stdio.h>
#include <stdlib.h>
// 定义全局变量
int MAX_ROADCATEGORY;
ROADCATEGORY* rc;
/**
 * 车站临时结果（用于保存坡段内的车站个数）
 */
struct cz
{
	int czh;  /**< 车站号*/
	int start;  /**< 车站公里标*/
	float avgV; /**<车站与前一个车站之间的平均速度*/
	int index;  /**< 车站在当前坡段的序列号，可能有多个车站存在*/
};
typedef struct cz CZ;
CZ czs[STATIONCOUNT];

/**
 * @brief 获得每种坡段的平均速度
 * @param start  坡段的起始位置
 * @param  ending 坡段的终止位置
 * @param startPost 线路的起始位置
 * @param endPost  线路的终止位置
 * @param finalAvg  平均速度
 * @return 异常处理结果
 */
PRE_PROCESS_RESULT getRoadCategoryAvgV(int start,int ending,int startPost,int endPost,float* finalAvg);
/**
 * @brief 获得车站对应的平均速度；
 * @param  czh  车站号
 * @param avgV（前一个车站到当前车站）的平均速度，若没有找到则返回-1
 * @return 异常处理结果
 */
PRE_PROCESS_RESULT getCzhAvgV(int czh,float* avgV);

/**
 * @brief 获得车站对应的平均速度；
 * @param  czh  车站号
 * @param （前一个车站到当前车站）的平均速度，若没有找到则返回-1
 * @return 异常处理结果
 */
PRE_PROCESS_RESULT getCzhAvgV(int czh,float* avgV)
{
	PRE_PROCESS_RESULT ret;
	ret.preprocess_result=PRE_PROCESS_SUCCESS;
	int i=0;
	for(;i<MAX_INTERVALRUNTIME;i++)
	{
		if(intervalRuntimeUnits[i].endCzh==czh)
		{
			*avgV=intervalRuntimeUnits[i].averageV;
			return ret;
		}
	}
	char desc[EXP_DESC_LENGTH]="#getCzhAvgV";
	strcat(desc,ret.description);
	pre_exception_decorator(PRE_NOT_FIND_CZ,&ret,desc);
	return  ret;
}
/**
 * @brief  临界速度计算
 * @param new_gradients   多质点模型下的加算坡度
 * @param max_length   加算坡度的长度
 * @param cv   指向临界速度数组的指针
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  computeCriticalV(MGRADIENT* new_gradients,int max_length,CRITICAL_V* cv)
{
	float maxV =opt_const. MAXV;
	float step = opt_const.TSTEP;
	float g = opt_const.G;
	int len_mgra =  max_length;
	int i;
	PRE_PROCESS_RESULT pre_ret;
	pre_ret.preprocess_result=PRE_PROCESS_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;
	float gradient;
	for(i = 0; i < len_mgra ; i++)
	{
		gradient = new_gradients[i].value;
		/*
		 * 上坡情况
		 */
		if(gradient > 0.0  || fabs(gradient-0.0)<opt_const.PRECISION)
		{
			int flag = 1;
			float v = 1.0;
			float cv0 = 0.0;
			for(v = 1.0;v < maxV || fabs(v-maxV)<opt_const.PRECISION ;v=v+step)
			{
				float w_o = v*v*locoInfo.davis[0] + v*locoInfo.davis[1]+locoInfo.davis[2];
				float w = w_o + gradient ;   //单位阻力
				float df;
				trac_ret=getForce(8,v,&df);//牵引力
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					char desc[EXP_DESC_LENGTH]="#cptCriticalV";
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				float dft = 1000/g*df/(locoInfo.totalWeight);	//单位牵引力
				float delta = dft - w;
				if(delta < 0.0)	//8档牵引,合力仍然小于0,此时速度为临界速度
				{
					cv0 = v;
					flag = 0;
					break;
				}
			}
			if((v > maxV || fabs(v-maxV)<opt_const.PRECISION )&& flag == 1)	//当速度大于maxV且未达到临界速度条件时
			{
				cv0 = maxV + 20.0;
			}
			if(gradient > 0.0)		//上坡
				cv[i].graflg = 1;
			else
				cv[i].graflg = 0;	//平坡
			cv[i].value = cv0;
		}
		if(gradient < 0.0)		//下坡
		{
			int flag = 1;
			float v = 0.0;
			float cv0 = 0.0;
			for(v = 0.0; v < maxV || fabs(v-maxV)<opt_const.PRECISION ; v=v+step)
			{
				float w_o = v*v*locoInfo.davis[0] + v*locoInfo.davis[1]+locoInfo.davis[2];
				float w = w_o + gradient ;
				if(w > 0.0)	//临界速度为令单位阻力为正的速度
				{
					cv0 = v;
					flag = 0;
					break;
				}
			}
			if( (v > maxV || fabs(v-maxV)<opt_const.PRECISION) && flag ==1)
				cv0 = maxV + 20.0;
			cv[i].graflg = -1;
			cv[i].value = cv0;
		}
		cv[i].mGra = gradient;
	}
	return pre_result_adapter(pre_ret);
}
/**
 *  @brief 道路分段
 * @param new_gradients  多质点模型下的加算坡度
 * @param max_length  加算坡度的长度
 * @param cv   临界速度
 * @param startPost   起始公里标
 * @param endPost   终止公里标
 * @param 优化异常结果
 */
OPTIMIZE_RESULT  computeRoadCategory(MGRADIENT* new_gradients,int max_length,CRITICAL_V* cv,int startPost, int endPost)
{
	PRE_PROCESS_RESULT pre_ret;
	pre_ret.preprocess_result=PRE_PROCESS_SUCCESS;
	float averageV[max_length];
	int len_mgra =  max_length;
	CATEGORY cat[max_length];	//临时存放数组
	int loco_length = (int)locoInfo.totalLength;
	float minV = opt_const.MINV;
	float limitV = opt_const.LIMITV;
//	float  maxV = limitV;
	float g = opt_const.G;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

//	float f_down;
//	float df_down;
	float w_o;
	float gDownSteep = -5.0;	// 8档制动，以最大速度运行，制动力和运行阻力的合力


	float step =opt_const. TSTEP;
	int len_cat ;
	float avgVe;
//	float f_avgV ;
//	float df_avgVt ;

//	float gUphill =2.0;
//	float gDownhill=-2.0;
	int i,j;
	/*
	 * 分类原始坡度段
	 */
	for(i = 0;i < len_mgra ; i++)
	{
		cat[i].start = new_gradients[i].start;
		cat[i].end = new_gradients[i].end;
		cat[i].gradient = new_gradients[i].value;

		//获得路段平均速度
		pre_ret = getRoadCategoryAvgV(cat[i].start,cat[i].end,startPost,endPost,&avgVe);
		if(pre_ret.preprocess_result!=PRE_PROCESS_SUCCESS)
		{
			char desc[EXP_DESC_LENGTH]="#ptRdCat";
			strcat(desc,pre_ret.description);
			pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
			return pre_result_adapter(pre_ret);
		}
		if(cv[i].graflg == 1)
		{
			cat[i].flag = 1;
			if(cv[i].value< avgVe || fabs(cv[i].value-avgVe)<opt_const.PRECISION)
			{
				cat[i].flag = 2;	//陡上坡
			}
		}
		else
		{
			if( cv[i].mGra >  -2.0 || fabs(cv[i].mGra+2.0)<opt_const.PRECISION)
				cat[i].flag = -1;
			else if(cv[i].mGra > gDownSteep || fabs(cv[i].mGra-gDownSteep)<opt_const.PRECISION)
			{
				cat[i].flag = -2;
			}
			else
				cat[i].flag = -3;
		}
	}
	/*
	 * 合并相邻同类坡度段
	 */
	int lastIndex = -1;	//当前下标
	int lastFlag = 0;	//当前下标下的flag
	int index = len_mgra-1;
	int index1 = 0;																																																																																	;

	int tempStart ;
	int tempEnd ;
	float tempValue ;

	i = 0;
	while (i <= index)
	{
		if (lastIndex == -1)	//第一次进入循环
		{
			lastIndex = i;
			lastFlag = cat[i].flag;
		}
		else{
			if(cat[i].flag == lastFlag){//do nothing
			}
			else
			{
				//合并同类flag路段
				tempStart = cat[lastIndex].start;
				tempEnd = cat[i-1].end;
				tempValue = 0.0;
				for(j = lastIndex ; j <=i-1 ; j++)
				{
					tempValue = tempValue + (float)(abs(cat[j].end - cat[j].start))*cat[j].gradient;
				}
				tempValue = tempValue / (float)(abs(tempEnd - tempStart));	//平均值
				cat[index1].start = tempStart;
				cat[index1].end = tempEnd;
				cat[index1].gradient = tempValue;
				cat[index1].flag = cat[lastIndex].flag;
				//移到当前的下标
				lastIndex = i;
				lastFlag = cat[i].flag;
				index1 = index1 + 1;
			}
		}
		i = i+1;
	}
	//对于最后一段，合并同类flag路段
	tempStart = cat[lastIndex].start;
	tempEnd = cat[index].end;
	tempValue = 0;
	for(j = lastIndex ; j<=index ; j++){
		tempValue = tempValue + (float)(abs(cat[j].end -cat[j].start))*cat[j].gradient;
	}
	tempValue = tempValue /(float)(abs(tempEnd - tempStart));
	cat[index1].start = tempStart;
	cat[index1].end = tempEnd;
	cat[index1].gradient = tempValue;
	cat[index1].flag = cat[lastIndex].flag;
	//index1+1代表当前cat实际长度
	//index仍然为mgradient的长度，下可替换
	/*
	 * 处理独立短段
	 * 处理两段中间夹一段的情况
	 */
	while(1)
	{
		int isFinish = 1;
		index = 0;
		i = 0;
		while(i<=index1)//index表示最后一个
		{
			/*
			 *  i ~ i+2的类型相同，i+1长度小于1KM
			 * 	i+1的长度小于整个长度的3/10
			 * 	处理方式：将中间的短段忽略，并入长段中
			 */
			//if i <= index1 - 2 &&
			//category(i,4) == category(i+2,4) &&
			//abs(category(i+1,2) - category(i+1,1)) <= 0.5*loco_length &&
			//abs(category(i+1,2) - category(i+1,1)) <= abs(category(i+2,2)- category(i,1)) * 0.3
			if((i<=index1-2)&&
					cat[i].flag == cat[i+2].flag &&
					((float)(abs(cat[i+1].end-cat[i+1].start))<0.5*(float)loco_length || fabs((float)(abs(cat[i+1].end-cat[i+1].start))-0.5)<opt_const.PRECISION) &&
					((float)(abs(cat[i+1].end-cat[i+1].start))< (float)abs(cat[i+2].end-cat[i].start)*0.3 || fabs((float)(abs(cat[i+1].end-cat[i+1].start)) -  (float)abs(cat[i+2].end-cat[i].start)*0.3)<opt_const.PRECISION)  )
			{
				cat[index].start = cat[i].start;
				cat[index].end = cat[i+2].end;
				tempValue = 0.0;
				for(j=i;j<=i+2;j++)
				{
					tempValue = tempValue + (float)(abs(cat[j].end-cat[j].start))*cat[j].gradient;
				}
				cat[index].gradient =tempValue/(float)(abs(cat[index].end-cat[index].start));
				cat[index].flag = cat[i].flag;
				i = i+2;
				isFinish = 0;
			}
			/*
			 * 如果不是独立短段的情况，则继续进行
			 */
			else
			{
				cat[index].start = cat[i].start;
				cat[index].end = cat[i].end;
				cat[index].gradient = cat[i].gradient;
				cat[index].flag = cat[i].flag;
			}
			index = index + 1;
			i = i+1;
		}
		//多次处理直到整个路段中没有满足独立短段的情况
		if (isFinish == 1)
		{
			break;
		}
		index1 = index - 1;
	}
	//index代表当前cat的实际长度

	/*
	 * 合并连续短段
	 */
	index=index-1;
	index1 = 0;
	lastIndex = -1;
	i = 0;
	while (i<=index )
	{
		/*
		 *	波浪段
		 */
		if((float)(abs(cat[i].end-cat[i].start))>0.5*(float)loco_length || fabs((float)(abs(cat[i].end-cat[i].start))-0.5*(float)loco_length) < opt_const.PRECISION)
		{
			if(lastIndex == -1)
			{
				cat[index1].start = cat[i].start;
				cat[index1].end = cat[i].end;
				cat[index1].gradient = cat[i].gradient;
				cat[index1].flag = cat[i].flag;
				index1 = index1 + 1;
				i = i+1;
			}
			else
			{
				tempStart = cat[lastIndex].start;
				tempEnd = cat[i-1].end;
				tempValue = 0;
				for(j=lastIndex;j<=i-1;j++)
				{
					tempValue = tempValue +(float) abs(cat[j].end-cat[j].start)*cat[j].gradient;
				}
				tempValue =tempValue/(float)abs(tempEnd-tempStart);
				cat[index1].start = tempStart;
				cat[index1].end = tempEnd;
				cat[index1].gradient = tempValue;
				cat[index1].flag = 0;
				index1 = index1 + 1;
				cat[index1].start = cat[i].start;
				cat[index1].end = cat[i].end;
				cat[index1].gradient = cat[i].gradient;
				cat[index1].flag = cat[i].flag;
				index1 = index1 + 1;

				i = i + 1;
				lastIndex = -1;
			}
		}
		else
		{
			if( lastIndex == -1)
			{
				lastIndex = i ;
				i = i+ 1;
			}
			else
				i = i+1;
		}
	}
	if(lastIndex != -1)	//处理尾段满足的情况
	{

		tempStart = cat[lastIndex].start;
		tempEnd = cat[index].end;
		tempValue = 0.0;
		for(j = lastIndex;j<=index;j++)
		{
			tempValue =  tempValue + (float)abs(cat[j].end-cat[j].start)*cat[j].gradient;
		}
		tempValue = tempValue/(float)abs(tempEnd-tempStart);
		cat[index1].start = tempStart;
		cat[index1].end = tempEnd;
		cat[index1].flag = 0;
		cat[index1].gradient = tempValue;
		index1 = index1+1;
	}
	index = index1 - 1;

	/*
	 * 短段处理
	 */
	i = 0;
	while(i<=index)
	{

		if((float)(abs(cat[i].end - cat[i].start))<0.5*(float)loco_length || fabs((float)(abs(cat[i].end - cat[i].start))-0.5*(float)loco_length)<opt_const.PRECISION)
		{
			if(i == 0)
			{
				if((float)abs(cat[i+1].end-cat[i+1].start)*3.0>(float)abs(cat[i].end - cat[i].start)*10.0 || fabs((float)abs(cat[i+1].end-cat[i+1].start)*3.0-(float)abs(cat[i].end - cat[i].start)*10.0)<opt_const.PRECISION)	//满足从属关系
				{
					cat[i].flag = cat[i+1].flag;
				}
			}
			else if(i == index)
			{
				if( (float)abs(cat[i-1].end - cat[i-1].start)*3.0 > (float)abs(cat[i].end - cat[i].start)*10.0 ||
						fabs((float)abs(cat[i-1].end - cat[i-1].start)*3.0 -(float)abs(cat[i].end - cat[i].start)*10.0)<opt_const.PRECISION)	//最后一段满足从属关系
				{
					cat[i].flag = cat[i-1].flag;
				}
			}
			else
			{
				if( ((float)abs(cat[i+1].end - cat[i+1].start)*3.0 > (float)abs(cat[i].end - cat[i].start)*10.0 || fabs((float)abs(cat[i+1].end - cat[i+1].start)*3.0 -(float) abs(cat[i].end - cat[i].start)*10.0)<opt_const.PRECISION)&&	//两端都满足从属关系
						((float)abs( cat[i-1].end - cat[i-1].start )*3.0 > (float)abs(cat[i].end - cat[i].start)*10.0 || fabs((float)abs( cat[i-1].end - cat[i-1].start )*3.0 - (float)abs(cat[i].end - cat[i].start)*10.0) <opt_const.PRECISION))
				{
					if((float)abs(cat[i].gradient - cat[i+1].gradient) > (float)abs(cat[i].gradient - cat[i-1].gradient) || fabs((float)abs(cat[i].gradient - cat[i+1].gradient) - (float)abs(cat[i].gradient - cat[i-1].gradient))<opt_const.PRECISION)
					{
						cat[i].flag = cat[i+1].flag;
					}
					else
					{
						cat[i].flag = cat[i-1].flag;
					}
				}
				else if((float)(cat[i+1].end - cat[i+1].start)*3.0 >(float) (cat[i].end - cat[i].start)*10.0 || fabs((float)(cat[i+1].end - cat[i+1].start)*3.0 -(float) (cat[i].end - cat[i].start)*10.0)<opt_const.PRECISION)	//后一段满足从属关系
				{
					cat[i].flag = cat[i+1].flag;
				}
				else if((float)(cat[i-1].end - cat[i-1].start)*3.0 > (float)(cat[i].end - cat[i].start)*10.0 || fabs((float)(cat[i-1].end - cat[i-1].start)*3.0 > (float)(cat[i].end - cat[i].start)*10.0)<opt_const.PRECISION)	//前一段满足从属关系
				{
					cat[i].flag = cat[i-1].flag;
				}
			}
		}
		i = i+1;
	}


	/*
	 * 再次合并相邻同类坡度段，短段处理后
	 */
	lastIndex = -1;
	lastFlag = 0;
	i = 0;
	index1 = 0;
	while (i <= index)
	{
		if (lastIndex == -1)
		{
			lastIndex = i;
			lastFlag = cat[i].flag;
		}
		else{
			if(cat[i].flag == lastFlag)
			{}
			else
			{
				tempStart = cat[lastIndex].start;
				tempEnd = cat[i-1].end;
				tempValue = 0;
				for(j = lastIndex ; j <=i-1 ; j++)
				{
					tempValue = tempValue + (float)abs(cat[j].end - cat[j].start)*cat[j].gradient;
				}
				tempValue = tempValue / (float)abs(tempEnd - tempStart);
				cat[index1].start = tempStart;
				cat[index1].end = tempEnd;
				cat[index1].gradient = tempValue;
				cat[index1].flag = cat[lastIndex].flag;
				lastIndex = i;
				lastFlag = cat[i].flag;
				index1 = index1 + 1;
			}
		}
		i = i+1;
	}
	tempStart = cat[lastIndex].start;
	tempEnd = cat[index].end;
	tempValue = 0;
	for(j = lastIndex ; j<=index ; j++){
		tempValue = tempValue + (float)abs(cat[j].end -cat[j].start)*cat[j].gradient;
	}
	tempValue = tempValue /(float)abs(tempEnd - tempStart);
	cat[index1].start = tempStart;
	cat[index1].end = tempEnd;
	cat[index1].gradient = tempValue;
	cat[index1].flag = cat[lastIndex].flag;

	/*
	 * 再次处理两段夹一段的短段情况
	 */
	while(1)
	{
		int isFinish = 1;
		index = 0;
		i = 0;
		while(i<=index1)
		{
			if((i<=index1-2)&&(cat[i].flag == cat[i+2].flag) && ((float)( abs(cat[i+1].end-cat[i+1].start))<0.5*(float)loco_length || fabs((float)( abs(cat[i+1].end-cat[i+1].start))-0.5*(float)loco_length)<opt_const.PRECISION)
					&&(float)( abs(cat[i+1].end-cat[i+1].start)*10.0<(float)abs(cat[i+2].end-cat[i].start)*3.0 || fabs((float)abs(cat[i+1].end-cat[i+1].start)*10.0-(float)abs(cat[i+2].end-cat[i].start)*3.0)<opt_const.PRECISION))
			{
				cat[index].start = cat[i].start;
				cat[index].end = cat[i+2].end;
				tempValue = 0.0;
				for(j=i;j<=i+2;j++)
				{
					tempValue = tempValue + (float)abs(cat[j].end-cat[j].start)*cat[j].gradient;
				}
				cat[index].gradient =tempValue/(float)abs(cat[index].end-cat[index].start);
				cat[index].flag = cat[i].flag;
				i = i+2;
				index = index +1;
				isFinish = 0;
			}
			else
			{
				cat[index].start = cat[i].start;
				cat[index].end = cat[i].end;
				cat[index].gradient = cat[i].gradient;
				cat[index].flag = cat[i].flag;
				index = index + 1;

			}
			i = i+1;
		}

		if (isFinish == 1)
		{
			break;
		}
		index1 = index - 1;
	}

	/*
	 * 再次合并相同flag段
	 */
	index = index -1;
	for(i=0;i<=index;i++){
		if(cat[i].flag!=-3)
		{
			if(cat[i].gradient >3.0 || fabs(cat[i].gradient - 3.0)< opt_const.PRECISION)
			{
				cat[i].flag = 2;
			}
			else if(cat[i].gradient >1.0 || fabs(cat[i].gradient - 1.0)<opt_const.PRECISION)
			{
				cat[i].flag = 1;
			}
			else if(cat[i].gradient >-1.0 || fabs(cat[i].gradient + 1.0)<opt_const.PRECISION){
				cat[i].flag = 0;
			}
			else if(cat[i].gradient >-3.0 || fabs(cat[i].gradient + 3.0)<opt_const.PRECISION){
				cat[i].flag = -1;
			}
			else if(cat[i].gradient >-5.0 || fabs(cat[i].gradient + 5.0)<opt_const.PRECISION){
				cat[i].flag = -2;
			}
			else
				cat[i].flag = -3;
		}
	}

	lastIndex = -1;
	lastFlag = 0;
	i = 0;
	index1 = 0;
	while (i <= index)
	{
		if (lastIndex == -1)
		{
			lastIndex = i;
			lastFlag = cat[i].flag;
		}
		else{
			if(cat[i].flag == lastFlag)
			{}
			else
			{
				tempStart = cat[lastIndex].start;
				tempEnd = cat[i-1].end;
				tempValue = 0;
				for(j = lastIndex ; j <=i-1 ; j++)
				{
					tempValue = tempValue + (float)abs(cat[j].end - cat[j].start)*cat[j].gradient;
				}
				tempValue = tempValue / (float)abs(tempEnd - tempStart);
				cat[index1].start = tempStart;
				cat[index1].end = tempEnd;
				cat[index1].gradient = tempValue;
				cat[index1].flag = cat[lastIndex].flag;
				lastIndex = i;
				lastFlag = cat[i].flag;
				index1 = index1 + 1;
			}
		}
		i = i+1;
	}
	tempStart = cat[lastIndex].start;
	tempEnd = cat[lastIndex].end;
	tempValue = 0;
	for(j = lastIndex ; j<=index ; j++){
		tempValue = tempValue + (float)abs(cat[j].end -cat[j].start)*cat[j].gradient;
	}
	tempValue = tempValue /(float)abs(tempEnd - tempStart);
	cat[index1].start = tempStart;
	cat[index1].end = tempEnd;
	cat[index1].gradient = tempValue;
	cat[index1].flag = cat[lastIndex].flag;

	len_cat = index1+1;
	int rc_tmp_len=0;
	ROADCATEGORY* rcTmp;
	//根据手动区域进行重新分段，把手动区域分割成独立的段
	rcTmp=reRoadCategory(cat,len_cat,rcTmp,&rc_tmp_len,&pre_ret);
	if(pre_ret.preprocess_result!=PRE_PROCESS_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#cptRdCat#reRoadCategory";
		strcat(desc,pre_ret.description);
		pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
		return pre_result_adapter(pre_ret);
	}
	// 入口速度 & 出口速度计算
	for (i = 0; i < rc_tmp_len; i++)
	{
		float gra = rcTmp[i].value;
		float hlength = (float)(rcTmp[i].length);
		pre_ret = getRoadCategoryAvgV(rcTmp[i].start,rcTmp[i].end,startPost,endPost,&avgVe);
		if(pre_ret.preprocess_result!=PRE_PROCESS_SUCCESS)
		{
			char desc[EXP_DESC_LENGTH]="#cptRdCat#inOutV";
			strcat(desc,pre_ret.description);
			pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
			return pre_result_adapter(pre_ret);
		}
		averageV[i]=avgVe;
		float s ;
		float v ;
		int flag = rcTmp[i].flag;
		float w ;
		float df;
		float dft;
		float c ;
		float acc ;
		float delta_v ;
		float delta_s ;
//		float t;
		float tempGear;
		int gear;
		float dfl;
		float dfu;
		float dflt;
		float dfut;
		if( flag == -3)
		{
			if(i>=1 && (rcTmp[i-1].flag!=opt_const.PUSHERENGINE || rcTmp[i-1].flag!=opt_const.THROUGHEXPERT))
			{
				rcTmp[i].vo=2*avgVe-rcTmp[i-1].vo;
			}
			else
				rcTmp[i].vo = limitV ;
			s = 0.0;
			v = rcTmp[i].vo ;
			while( s<hlength )
			{
				w_o = locoInfo.davis[0]*v*v + locoInfo.davis[1]*v + locoInfo.davis[2];
				w = w_o + gra ;		//总阻力
				trac_ret=getForce(-8,v,&df);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					char desc[80]="#cptRdCat#inOutVelociy#flag=-3";
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				dft = 1000/g*df/locoInfo.totalWeight;
				c = -(dft - w);
				acc = c/30;
				delta_v = acc*step;
				v = v + delta_v ;
				delta_s = (v/3.6)*step;
				s = s+delta_s;
			}
			rcTmp[i].vi = v ;
			rcTmp[i].sugGear = -9.0;
		}
		else if(flag == 2)
		{
			rcTmp[i].sugGear = 8.0;
//			t = 0.0;
			s = 0.0;
			v = avgVe;
			while(1)
			{
				w_o = locoInfo.davis[0]*v*v + locoInfo.davis[1]*v + locoInfo.davis[2];
				w = w_o + gra ;		//总阻力
				trac_ret=getForce(-8,v,&df);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					char desc[EXP_DESC_LENGTH]="#cptRdCat#inOutVelociy#flag=2";
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				dft = 1000/g*df/locoInfo.totalWeight;
				c = -(dft - w);
				acc = c/30;
				delta_v = acc*step;
				v = v + delta_v ;
				delta_s = (v/3.6)*step;
				s = s+delta_s;
				if(v >  limitV  || fabs(v-limitV)<opt_const.PRECISION)
				{
					break;
				}
				if(s >  hlength/2 ||fabs(s-hlength/2) <opt_const.PRECISION)
				{
					break;
				}
			}
			rcTmp[i].vi = v;
//			t = 0.0;
			s = 0.0;
			v = avgVe;
			while(1)
			{
				w_o = locoInfo.davis[0]*v*v + locoInfo.davis[1]*v + locoInfo.davis[2];
				w = w_o + gra ;		//总阻力
				trac_ret=getForce(8,v,&df);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					char desc[EXP_DESC_LENGTH]="#cptRdCat#inOutVelociy#flag=2";
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				dft = 1000/g*df/locoInfo.totalWeight;
				c = (dft - w);
				acc = c/30;
				delta_v = acc*step;
				v = v + delta_v ;
				delta_s = (v/3.6)*step;
				s = s+delta_s;
				if(v < 0.0 || fabs(v-0.0)<opt_const.PRECISION)
				{
					break;
				}
				if(v>opt_const.LIMITV  || fabs(v-limitV)<opt_const.PRECISION)
					break;
				if(s > hlength/2 ||fabs(s-hlength/2) <opt_const.PRECISION)
				{
					break;
				}
			}
			rcTmp[i].vo = v;
		}
		else if(flag == -2)
		{
			tempGear = 0.0;
			v = limitV ;
			w_o = locoInfo.davis[0]*v*v + locoInfo.davis[1]*v + locoInfo.davis[2];
			w = -(w_o + gra) ;		//总阻力
			for(gear=1;gear<=7;gear++){
				trac_ret=getForce(-gear,v,&dfl);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					char desc[EXP_DESC_LENGTH]="#cptRdCat#inOutVelociy#flag=-2";
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}

				trac_ret=getForce(-gear-1,v,&dfu);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					char desc[EXP_DESC_LENGTH]="#cptRdCat#inOutVelociy#flag=-2";
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				dflt = 1000/g*dfl/locoInfo.totalWeight;
				dfut = 1000/g*dfu/locoInfo.totalWeight;
				if (gear == 1 && w < dfut)
				{
					tempGear = - w/dfut;
					break;
				}
				else if((dfut < w || fabs(dfut - w)<opt_const.PRECISION) && (w < dflt || fabs(w - dflt)<opt_const.PRECISION))
				{
					tempGear = - gear - (w-dfut)/(dflt-dfut);
					break;
				}
				else if(gear == 7 && dflt < w)
				{
					tempGear =-9.0 ;
					break;
				}
			}
			rcTmp[i].sugGear = tempGear;
//			t = 0.0;
			s = 0.0;
			v = avgVe;
			while(1)
			{
				w_o = locoInfo.davis[0]*v*v + locoInfo.davis[1]*v + locoInfo.davis[2];
				w = w_o + gra ;		//总阻力
				c =  w;
				acc = c/30;
				delta_v = acc*step;
				v = v + delta_v ;
				delta_s = (v/3.6)*step;
				s = s+delta_s;
				if( v<minV || fabs(v-minV)<opt_const.PRECISION)
				{
					break;
				}
				if (s > hlength/2 || fabs(s-hlength/2) <opt_const.PRECISION)
				{
					break;
				}
			}
			rcTmp[i].vi = v;
//			t = 0.0;
			s = 0.0;
			v = avgVe;
			while(1)
			{
				w_o = locoInfo.davis[0]*v*v + locoInfo.davis[1]*v + locoInfo.davis[2];
				w = w_o + gra ;		//总阻力
				c =  -w;
				acc = c/30;
				delta_v = acc*step;
				v = v + delta_v ;
				delta_s = (v/3.6)*step;
				s = s+delta_s;
				if( v> limitV || fabs(v-limitV)<opt_const.PRECISION)
				{
					break;
				}
				if (s > hlength/2 || fabs(s-hlength/2)<opt_const.PRECISION)
				{
					break;
				}
			}
			rcTmp[i].vo = v;
		}
		else if(flag==1 || flag==-1 || flag == 0)
		{
			v = avgVe;
			w_o = locoInfo.davis[0]*v*v + locoInfo.davis[1]*v + locoInfo.davis[2];
			w = w_o + gra ;
			tempGear = 0.0;
			for(gear=1;gear<=7;gear++){
				trac_ret=getForce(gear,v,&dfl);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					char desc[EXP_DESC_LENGTH]="#cptRdCat#inOutVelociy#flag=1/-1/0";
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				trac_ret=getForce(gear+1,v,&dfu);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					char desc[EXP_DESC_LENGTH]="#cptRdCat#inOutVelociy#flag=1/-1/0";
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				dflt = 1000/g*dfl/locoInfo.totalWeight;
				dfut = 1000/g*dfu/locoInfo.totalWeight;
				if((dflt < w || fabs(dflt - w) <opt_const.PRECISION)  && (w < dfut || fabs(w-dfut)<opt_const.PRECISION))
				{
					tempGear =  gear + (w-dflt)/(dfut-dflt);
					break;
				}
			}
			rcTmp[i].sugGear = tempGear;
			rcTmp[i].vi = avgVe;
			rcTmp[i].vo = avgVe;
		}
		rcTmp[i].avgV=avgVe;
	}
	//针对手动区域得到手动区域结尾的速度
	for (i = 0; i < rc_tmp_len; i++)
	{
		if(rcTmp[i].flag==opt_const.PUSHERENGINE)
		{
			if(i+1<rc_tmp_len)
				rcTmp[i].vo=rcTmp[i+1].vi;
			if(i-1>=0)
				rcTmp[i].vi=rcTmp[i-1].vo;
		}
	}
	FILE *fid;
	fid = fopen("././ret/roadCategory_new", "w");
	if (fid == NULL) {
		fprintf(stderr, "open roadCategory_new err!\n");
	}

	for (i = 0; i < rc_tmp_len; i++) {
		fprintf(fid,"%d  %d  %f  %d  %f  %f  %f  %f \n",rcTmp[i].start,rcTmp[i].end,rcTmp[i].value,rcTmp[i].flag,rcTmp[i].vi,rcTmp[i].vo,rcTmp[i].sugGear,rcTmp[i].avgV);
	}
	fclose(fid);

	rc = (ROADCATEGORY*)malloc(sizeof(ROADCATEGORY)*rc_tmp_len);
	if(rc==NULL)
	{
		pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#cptRdCat#rc");
		return pre_result_adapter(pre_ret);
	}
	MAX_ROADCATEGORY = rc_tmp_len;
	//获得每个类型坡段中的相关数据（每个破段类型中的存在的其余分段类型的小分段集合）
	for(i = 0;i < rc_tmp_len ; i++){
		rc[i].start = rcTmp[i].start;
		rc[i].end = rcTmp[i].end;
		rc[i].flag = rcTmp[i].flag;
		rc[i].length = rcTmp[i].length;
		rc[i].sugGear = rcTmp[i].sugGear;
		rc[i].value = rcTmp[i].value;
		rc[i].vi = rcTmp[i].vi;
		rc[i].vo = rcTmp[i].vo;
		rc[i].avgV = rcTmp[i].avgV;
		int partIndex=0;
		int lastFlag=3;
		int lastFlagIndex=0;
//		int dIndex=max_length;
		for(j=0 ; j<max_length;j++)
		{
			if(new_gradients[j].start>=rc[i].start && new_gradients[j].end<=rc[i].end)
			{
				int flag;
				if(rc[i].flag==2) //陡上坡
				{
					if ((new_gradients[j].value>-3.0 || fabs(new_gradients[j].value+3.0)<opt_const.PRECISION) && (new_gradients[j].value<3.0 || fabs(new_gradients[j].value- 3.0)<opt_const.PRECISION))//陡上坡中出现缓坡
					{
						if((new_gradients[j].value<3.0 || fabs(new_gradients[j].value - 3.0)<opt_const.PRECISION) && ( new_gradients[j].value>1.0 || fabs(new_gradients[j].value-1.0)<opt_const.PRECISION))  //缓上坡
						{
							flag=1;
						}
						else
						{
							if(new_gradients[j].value<1.0 && (new_gradients[j].value>-1.0 || fabs(new_gradients[j].value+1.0)<opt_const.PRECISION)) //平坡
							{
								flag=0;
							}
							else
							{
								if(new_gradients[j].value<-1.0 && ( new_gradients[j].value>-3.0 || fabs(new_gradients[j].value+3.0)<opt_const.PRECISION))  //缓下坡
									flag=-1;
							}
						}

					}
					else	if(new_gradients[j].value<-3.0 ) //陡上坡中出现陡下坡
					{
						if(new_gradients[j].value>-5.0 || fabs(new_gradients[j].value+5.0)<opt_const.PRECISION) //陡下坡
							flag=-2;
						else  //超陡下坡
							flag=-3;
					}

					if(new_gradients[j].value>3.0)
					{
						if(lastFlag!=3)
						{
							if(new_gradients[j].start-new_gradients[lastFlagIndex].start<=opt_const.DISTANCE)
							{
								lastFlag=3;
								lastFlagIndex=j;
								continue;
							}
							if(partIndex==0)
								rc[i].partGradients=(PARTGRADIENT*) malloc( (partIndex + 1) * sizeof(PARTGRADIENT));
							else
								rc[i].partGradients=(PARTGRADIENT*) realloc(rc[i].partGradients, (partIndex + 1) * sizeof(PARTGRADIENT));
							if(rc[i].partGradients==NULL)
							{
								pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#cptRdCat#partGradients");
								return pre_result_adapter(pre_ret);
							}
							rc[i].partGradients[partIndex].start=new_gradients[lastFlagIndex].start;
							rc[i].partGradients[partIndex].end=new_gradients[j].start;
							int k=0;
							rc[i].partGradients[partIndex].value=0.0;
							for(k=lastFlagIndex;k<j;k++)
								rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value+new_gradients[k].value;
							rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value/(float)(j-lastFlagIndex);
							rc[i].partGradients[partIndex].flag=lastFlag;
							partIndex=partIndex+1;
						}
						lastFlag=3;
						lastFlagIndex=j;
					}
					else
					{
						if(lastFlag==3 )
						{
							lastFlag=flag;
							lastFlagIndex=j;
							continue;
						}
						else if(lastFlag==flag)
						{
							continue;
						}
						else
						{
							if(new_gradients[j].start-new_gradients[lastFlagIndex].start<=opt_const.DISTANCE)
							{
								lastFlag=3;
								lastFlagIndex=j;
								continue;
							}
							if(partIndex==0)
								rc[i].partGradients=(PARTGRADIENT*) malloc( (partIndex + 1) * sizeof(PARTGRADIENT));
							else
								rc[i].partGradients=(PARTGRADIENT*) realloc(rc[i].partGradients, (partIndex + 1) * sizeof(PARTGRADIENT));
							if(rc[i].partGradients==NULL)
							{
								pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#cptRdCat#partGradients");
								return pre_result_adapter(pre_ret);
							}
							rc[i].partGradients[partIndex].start=new_gradients[lastFlagIndex].start;
							rc[i].partGradients[partIndex].end=new_gradients[j].start;
							int k=0;
							rc[i].partGradients[partIndex].value=0.0;
							for(k=lastFlagIndex;k<j;k++)
								rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value+new_gradients[k].value;
							rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value/(float)(j-lastFlagIndex);
							rc[i].partGradients[partIndex].flag=lastFlag;
							lastFlag=flag;
							lastFlagIndex=j;
							partIndex=partIndex+1;
						}
					}
				}
				else if (rc[i].flag==-2 || rc[i].flag==-3) //陡下坡
				{
					if ((new_gradients[j].value>-3.0 || fabs(new_gradients[j].value+3.0)<opt_const.PRECISION) && new_gradients[j].value<3.0) //陡下坡中出现缓坡
					{
						if((new_gradients[j].value<3.0 || fabs(new_gradients[j].value-3.0)<opt_const.PRECISION) && (new_gradients[j].value>1.0 || fabs(new_gradients[j].value-1.0)<opt_const.PRECISION))  //缓上坡
						{
							flag=1;
						}
						else
						{
							if(new_gradients[j].value<1.0 && (new_gradients[j].value>-1.0 || fabs(new_gradients[j].value+1.0)<opt_const.PRECISION)) //平坡
							{
								flag=0;
							}
							else
							{
								if(new_gradients[j].value<-1.0  && (new_gradients[j].value>-3.0 || fabs(new_gradients[j].value+3.0)<opt_const.PRECISION))  //缓下坡
									flag=-1;
							}
						}

					}
					else if( new_gradients[j].value>3.0 || fabs(new_gradients[j].value-3.0)<opt_const.PRECISION) //陡下坡中出现上坡
					{
						flag=2;

					}
					if(new_gradients[j].value<-3.0)
					{
						if(lastFlag!=3)
						{
							if(new_gradients[j].start-new_gradients[lastFlagIndex].start<=opt_const.DISTANCE)
							{
								lastFlag=3;
								lastFlagIndex=j;
								continue;
							}
							if(partIndex==0)
								rc[i].partGradients=(PARTGRADIENT*) malloc( (partIndex + 1) * sizeof(PARTGRADIENT));
							else
								rc[i].partGradients=(PARTGRADIENT*) realloc(rc[i].partGradients, (partIndex + 1) * sizeof(PARTGRADIENT));
							if(rc[i].partGradients==NULL)
							{
								pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#cptRdCat#partGradients");
								return pre_result_adapter(pre_ret);
							}
							rc[i].partGradients[partIndex].start=new_gradients[lastFlagIndex].start;
							rc[i].partGradients[partIndex].end=new_gradients[j].start;
							int k=0;
							rc[i].partGradients[partIndex].value=0.0;
							for(k=lastFlagIndex;k<j;k++)
								rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value+new_gradients[k].value;
							rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value/(float)(j-lastFlagIndex);
							rc[i].partGradients[partIndex].flag=lastFlag;
							partIndex=partIndex+1;
						}
						lastFlag=3;
						lastFlagIndex=j;
					}
					else
					{
						if(lastFlag==3 )
						{
							lastFlag=flag;
							lastFlagIndex=j;
							continue;
						}
						else if(lastFlag==flag)
						{
							continue;
						}
						else
						{
							if(new_gradients[j].start-new_gradients[lastFlagIndex].start<=opt_const.DISTANCE)
							{
								lastFlag=3;
								lastFlagIndex=j;
								continue;
							}
							if(partIndex==0)
								rc[i].partGradients=(PARTGRADIENT*) malloc( (partIndex + 1) * sizeof(PARTGRADIENT));
							else
								rc[i].partGradients=(PARTGRADIENT*) realloc(rc[i].partGradients, (partIndex + 1) * sizeof(PARTGRADIENT));
							if(rc[i].partGradients==NULL)
							{
								pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#cptRdCat#partGradients");
								return pre_result_adapter(pre_ret);
							}
							rc[i].partGradients[partIndex].start=new_gradients[lastFlagIndex].start;
							rc[i].partGradients[partIndex].end=new_gradients[j].start;
							int k=0;
							for(k=lastFlagIndex;k<j;k++)
								rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value+new_gradients[k].value;
							rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value/(float)(j-lastFlagIndex);
							rc[i].partGradients[partIndex].flag=lastFlag;
							lastFlag=flag;
							lastFlagIndex=j;
							partIndex=partIndex+1;
						}
					}
				}
				else if(rc[i].flag==-1|| rc[i].flag==0 || rc[i].flag==1)//缓坡
				{
					if(new_gradients[j].value<-3.0 ) //缓坡中出现陡下坡
					{
						if(new_gradients[j].value>-5.0 || fabs(new_gradients[j].value+5.0)<opt_const.PRECISION) //陡下坡
							flag=-2;
						else  //超陡下坡
							flag=-3;
					}
					else if( new_gradients[j].value>3.0 || fabs(new_gradients[j].value-3.0)<opt_const.PRECISION) //陡下坡中出现上坡
					{
						flag=2;
					}
					if(new_gradients[j].value<3.0 &&( new_gradients[j].value>-3.0 || fabs( new_gradients[j].value+3.0)<opt_const.PRECISION))
					{
						if(lastFlag!=3)
						{
							if(new_gradients[j].start-new_gradients[lastFlagIndex].start<=opt_const.DISTANCE)
							{
								lastFlag=3;
								lastFlagIndex=j;
								continue;
							}
							if(partIndex==0)
								rc[i].partGradients=(PARTGRADIENT*) malloc( (partIndex + 1) * sizeof(PARTGRADIENT));
							else
								rc[i].partGradients=(PARTGRADIENT*) realloc(rc[i].partGradients, (partIndex + 1) * sizeof(PARTGRADIENT));
							if(rc[i].partGradients==NULL)
							{
								pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#cptRdCat#partGradients");
								return pre_result_adapter(pre_ret);
							}
							rc[i].partGradients[partIndex].start=new_gradients[lastFlagIndex].start;
							rc[i].partGradients[partIndex].end=new_gradients[j].start;
							int k=0;
							rc[i].partGradients[partIndex].value=0.0;
							for(k=lastFlagIndex;k<j;k++)
								rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value+new_gradients[k].value;
							rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value/(float)(j-lastFlagIndex);
							rc[i].partGradients[partIndex].flag=lastFlag;
							partIndex=partIndex+1;
						}
						lastFlag=3;
						lastFlagIndex=j;
					}
					else
					{
						if(lastFlag==3 )
						{
							lastFlag=flag;
							lastFlagIndex=j;
							continue;
						}
						else if(lastFlag==flag)
						{
							continue;
						}
						else
						{
							if(new_gradients[j].start-new_gradients[lastFlagIndex].start<=opt_const.DISTANCE)
							{
								lastFlag=3;
								lastFlagIndex=j;
								continue;
							}
							if(partIndex==0)
								rc[i].partGradients=(PARTGRADIENT*) malloc( (partIndex + 1) * sizeof(PARTGRADIENT));
							else
								rc[i].partGradients=(PARTGRADIENT*) realloc(rc[i].partGradients, (partIndex + 1) * sizeof(PARTGRADIENT));
							if(rc[i].partGradients==NULL)
							{
								pre_exception_decorator(PRE_MALLOC_ERROR,&pre_ret,"#cptRdCat#partGradients");
								return pre_result_adapter(pre_ret);
							}
							rc[i].partGradients[partIndex].start=new_gradients[lastFlagIndex].start;
							rc[i].partGradients[partIndex].end=new_gradients[j].start;
							int k=0;
							for(k=lastFlagIndex;k<j;k++)
								rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value+new_gradients[k].value;
							rc[i].partGradients[partIndex].value=rc[i].partGradients[partIndex].value/(float)(j-lastFlagIndex);
							rc[i].partGradients[partIndex].flag=lastFlag;
							lastFlag=flag;
							lastFlagIndex=j;
							partIndex=partIndex+1;
						}
					}
				}
			}

		}
		rc[i].partGradNum=partIndex;
		if(partIndex!=0)
		{
			int k=0;
			for(;k<partIndex;k++)
			{
				//				printf("index%d : %d,%d,%f ,%d\n",i+1,rc[i].partGradients[k].start,rc[i].partGradients[k].end,rc[i].partGradients[k].value,rc[i].partGradients[k].flag);
			}
		}
	}
	OPT_EXTR_RESULT file_ret;
	file_ret.file_result=OPT_EXTR_SUCCESS;
	getTwoStationSteepPect(new_gradients, max_length);
	opt_log_info("分段结果写入开始");
	fid = fopen("././ret/roadcategory.txt","w");
	if(fid==NULL){
		file_exception_decorator(OPT_OPEN_ERROR,&file_ret,"#cptRcCat");
		return file_result_adapter(file_ret);
	}
	for(i=0;i<MAX_ROADCATEGORY ;i++)
	{
		fprintf(fid,"%d  %d  %f  %d  %f  %f  %f  %f  %d\n",rc[i].start,rc[i].end,rc[i].value,rc[i].flag,rc[i].vi,rc[i].vo,rc[i].sugGear,rc[i].avgV,rc[i].partGradNum);
	}
	fclose(fid);
	opt_log_info("分段结果写入成功");
	return pre_result_adapter(pre_ret);
}
/**
 * @brief  获得两个车站之间陡下坡和陡上坡所占据的比例，将其存放于intervalRunTimeUnit（保存两个车站之间的运行时间等数据）的结构中
 * @param  new_gradients  加入多质点模型的加算坡度
 * @param  max_length  多质点模型的加算坡度的长度
 */
void  getTwoStationSteepPect(MGRADIENT* new_gradients,int max_length)
{
	int i = 0;
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
		int steepUpLength=0;
		int steepDownLength=0;
		int k=0;
		for(;k<max_length;k++)
		{

			int startTemp=new_gradients[k].start;
			int endTemp=new_gradients[k].end;
			if(endTemp<startGlb)
				continue;
			if(startTemp>endGlb)
				break;
			if(startTemp<startGlb)
				startTemp=startGlb;
			if(endTemp>endGlb)
				endGlb=new_gradients[k].end;
			if(startTemp>=startGlb && endTemp<=endGlb)
			{
				if(new_gradients[k].value<-3.0)
					steepDownLength=steepDownLength+(endTemp-startTemp);
				else if(new_gradients[k].value>3.0 || fabs(new_gradients[k].value-3.0)<opt_const.PRECISION)
					steepUpLength=steepUpLength+(endTemp-startTemp);
			}
		}
		intervalRuntimeUnits[i].steepDownPect=steepDownLength*100/(endGlb-startGlb);
		intervalRuntimeUnits[i].steepUpPect=steepUpLength*100/(endGlb-startGlb);
		//		printf("%d-%d:  down =%d, up=%d\n",intervalRuntimeUnits[i].startCzh,intervalRuntimeUnits[i].endCzh,intervalRuntimeUnits[i].steepDownPect,intervalRuntimeUnits[i].steepUpPect);
	}
}
/**
 * @brief 获得每种坡段的平均速度
 * @param start  坡段的起始位置
 * @param  ending 坡段的终止位置
 * @param startPost 线路的起始位置
 * @param endPost  线路的终止位置
 * @param finalAvg 平均速度
 * @return 异常处理结果
 */
PRE_PROCESS_RESULT getRoadCategoryAvgV(int start,int ending,int startPost,int endPost,float* finalAvg)
{
	PRE_PROCESS_RESULT pre_ret;
	pre_ret.preprocess_result=PRE_PROCESS_SUCCESS;
	int midCzIndex=-1;
	int czIndex=0;
	int flag=-2;
	float avgV=0.0;
	int indexTemp=-1;
	if (ending>endPost)
	{
		ending=endPost;
	}
	int i=0;
	float averageV=0.0;

	for(;i<MAX_STATION;i++)
	{
		if (startPost<=stations[i].start)
		{
			indexTemp=i;
			break;
		}
	}

	i=0;
	for(;i<MAX_STATION;i++)
	{
		int glb;
		glb=stations[i].start;
		if((i==0 && ending<glb)||ending<=startPost) //所求的坡段在第一个车站前面
		{
			flag=-1;
			break;
		}
		if (i>0&&(start>=stations[i-1].start) && ending<=glb)
		{
			czIndex=i;
			flag=0;
			break;
		}
		if(glb>=start && glb<ending)//车站在该两个车站之间
		{
			flag=1;
			midCzIndex++;
			int index;
			if(i<=indexTemp)
			{
				pre_ret=getCzhAvgV(stations[indexTemp+1].id,&averageV);
				if(pre_ret.preprocess_result==PRE_PROCESS_SUCCESS)
				{
					index=indexTemp+1;
				}
				else
				{
					char desc[EXP_DESC_LENGTH]="#rdCat.c#getRdCatAvgV";
					strcat(desc,pre_ret.description);
					pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
					return pre_ret;
				}
			}
			else
			{
				pre_ret=getCzhAvgV(stations[i].id,&averageV);
				if(pre_ret.preprocess_result==PRE_PROCESS_SUCCESS	)
				{
					index=i;
				}
				else
				{
					char desc[EXP_DESC_LENGTH]="#rdCat.c#getRdCatAvgV";
					strcat(desc,pre_ret.description);
					pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
					return pre_ret;
				}
			}
			czs[midCzIndex].czh=stations[index].id;
			czs[midCzIndex].start=glb;
			czs[midCzIndex].avgV=averageV;
			czs[midCzIndex].index=index;

		}

	}
	midCzIndex++;
	if(flag==0)
	{
		if(czIndex==indexTemp) //第一个车站
			pre_ret=getCzhAvgV(stations[czIndex+1].id,&averageV);
		else
			pre_ret=getCzhAvgV(stations[czIndex].id,&averageV);
		if(pre_ret.preprocess_result==PRE_PROCESS_SUCCESS)
			avgV=averageV;
		else
		{
			char desc[EXP_DESC_LENGTH]="#rdCat.c#getRdCatAvgV";
			strcat(desc,pre_ret.description);
			pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
			return pre_ret;
		}
	}
	else
	{
		if(flag==1)
		{
			avgV=0.0;
			i=0;
			for(;i<midCzIndex;i++)
			{
				if(i==0)
				{
					float jl=(float)(czs[i].start-start);
					avgV=avgV+czs[i].avgV*(jl/(float)(ending-start));
					if(i==midCzIndex-1 && czs[i].index<MAX_STATION-1)
					{
						jl=(float)(ending-czs[i].start);
						pre_ret=getCzhAvgV(stations[czs[i].index+1].id,&averageV);
						if(pre_ret.preprocess_result==PRE_PROCESS_SUCCESS)
							avgV=avgV+jl/(float)(ending-start)*averageV;
						else
						{
							char desc[EXP_DESC_LENGTH]="#rdCat.c#getRdCatAvgV";
							strcat(desc,pre_ret.description);
							pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
							return pre_ret;
						}
					}
					else
					{
						if(i==midCzIndex-1 && czs[i].index==MAX_STATION-1)
						{
							jl=(float)(ending-czs[i].start);
							pre_ret=getCzhAvgV(stations[czs[i].index].id,&averageV);
							if(pre_ret.preprocess_result==PRE_PROCESS_SUCCESS)
								avgV=avgV+jl/(float)(ending-start)*averageV;
							else
							{
								char desc[EXP_DESC_LENGTH]="#rdCat.c#getRdCatAvgV";
								strcat(desc,pre_ret.description);
								pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
								return pre_ret;
							}
						}
					}
				}
				else
				{
					float jl=(float)(czs[i].start-czs[i-1].start);
					avgV=avgV+jl/(float)(ending-start)*czs[i].avgV;
					if(i==midCzIndex-1 && czs[i].index<MAX_STATION-1)
					{
						jl=(float)(ending-czs[i].start);
						pre_ret=getCzhAvgV(stations[czs[i].index+1].id,&averageV);
						if(pre_ret.preprocess_result==PRE_PROCESS_SUCCESS)
							avgV=avgV+jl/(float)(ending-start)*averageV;
						else
						{
							char desc[EXP_DESC_LENGTH]="#rdCat.c#getRdCatAvgV";
							strcat(desc,pre_ret.description);
							pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
							return pre_ret;
						}
					}
					else
					{
						if(i==midCzIndex-1 && czs[i].index==MAX_STATION-1)
						{
							jl=(float)(ending-czs[i].start);
							pre_ret=getCzhAvgV(stations[czs[i].index].id,&averageV);
							if(pre_ret.preprocess_result==PRE_PROCESS_SUCCESS)
								avgV=avgV+jl/(float)(ending-start)*averageV;
							else
							{
								char desc[EXP_DESC_LENGTH]="#rdCat.c#getRdCatAvgV";
								strcat(desc,pre_ret.description);
								pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
								return pre_ret;
							}
						}
					}
				}
			}
		}
		else
		{
			if(flag==-1)
			{
				pre_ret=getCzhAvgV(stations[indexTemp+1].id,&averageV);
				if(pre_ret.preprocess_result==PRE_PROCESS_SUCCESS)
					avgV=averageV;
				else
				{
					char desc[EXP_DESC_LENGTH]="#rdCat.c#getRdCatAvgV";
					strcat(desc,pre_ret.description);
					pre_exception_decorator(pre_ret.preprocess_result,&pre_ret,desc);
					return pre_ret;
				}
			}
			else
				avgV=totalAvgV;
		}
	}
	*finalAvg=avgV;
	return pre_ret;
}

ROADCATEGORY* reRoadCategory(CATEGORY* cat,int len_cat,ROADCATEGORY* rcTmp,int* rc_tmp_len,PRE_PROCESS_RESULT* pre_ret)
{
	int loco_length = (int)locoInfo.totalLength;
	rcTmp = (ROADCATEGORY*)malloc(sizeof(ROADCATEGORY)*len_cat);
	if(rcTmp==NULL)
	{
		pre_exception_decorator(PRE_MALLOC_ERROR,pre_ret,"#reRoadCategory#rcTmp");
		return rcTmp;
	}
	int i;
	int maxPntLen=len_cat+1;
	if(!(throughExpert.start<opt_const.PRECISION)) //相当于!=0
		maxPntLen=maxPntLen+2;
	if(pusherEngines.len!=0)
		maxPntLen=maxPntLen+pusherEngines.len*2;
	CATPOINT points[maxPntLen];//原始坡段的点个数+贯通实验点个数+手动区域点个数
	int index=-1;
	index=index+1;
	points[index].value=cat[0].start;
	points[index].flag=0;
	for(i=0;i<len_cat;i++)
	{
		index=index+1;
		points[index].value=cat[i].end;
		points[index].flag=0;
	}

	if(!(throughExpert.start<opt_const.PRECISION))  //相当于!=0
	{
		index=index+1;
		points[index].value=(int)throughExpert.start;
		points[index].flag=opt_const.THROUGHEXPERT;
		index=index+1;
		points[index].value=(int)throughExpert.ending;
		points[index].flag=opt_const.THROUGHEXPERT;
	}

	if(pusherEngines.len!=0)
	{
		for(i=0;i<pusherEngines.len;i++)
		{
			index=index+1;
			points[index].value=(int)pusherEngines.handAreas[i].start;
			points[index].flag=opt_const.PUSHERENGINE;
			index=index+1;
			points[index].value=(int)pusherEngines.handAreas[i].ending;
			points[index].flag=opt_const.PUSHERENGINE;
		}
	}
	CATPOINT upoints[maxPntLen];
	int upointsMaxLen = 0;
	uniqueByCat(points, maxPntLen, upoints, &upointsMaxLen);

	quickSortByCat(upoints, 0, upointsMaxLen - 1);
	FILE* fid = fopen("././ret/upoints", "w");
	if (fid == NULL) {
		fprintf(stderr, "open roadCategory_new err!\n");
	}
	for(i=0;i<upointsMaxLen;i++)
		fprintf(fid, "%d\t%d\t\n", upoints[i].value,upoints[i].flag);
	fclose(fid);
	int pFlag=0;//补机段标识，1标识是补机
	int tFlag=0; //贯通实验标识，1标识是贯通实验
	int rdInx=-1;//最终分段的索引index
	int cInx=-1;//cat的索引index
	int pInx=-1;//补机段的索引index
	int j;
	int leftFlag=0; //判断手动区域左侧的位置是否和原始坡段位置相差小于500米，若是则需要合并
	int ifLastHandArea=0;//表示上一段是手动段
	for(i=0;i<upointsMaxLen-1;)
	{
		if((tFlag==1 || pFlag==1) && upoints[i].flag==0) //已经在手动区域则跳过
		{
			i++;
			continue;
		}
		else if(tFlag==0 && upoints[i].flag==opt_const.THROUGHEXPERT) //贯通实验开始位置
		{
			tFlag=1;
			if(i>0 && (upoints[i].value-upoints[i-1].value<loco_length/2))
			{
				leftFlag=1;
			}
			i++;
		}
		else if(pFlag==0 && upoints[i].flag==opt_const.PUSHERENGINE) //补机开始位置
		{
			pInx=pInx+1;
			pFlag=1;
			if(i>0 && (upoints[i].value-upoints[i-1].value)<loco_length/2)
			{
				leftFlag=1;
			}
			i++;
		}
		else if(tFlag==1 && upoints[i].flag==opt_const.THROUGHEXPERT)//贯通实验结束位置
		{
			if(leftFlag==1) //左侧需要合并
			{
				rcTmp[rdInx].end=(int)throughExpert.ending;
				rcTmp[rdInx].length=rcTmp[rdInx].end-rcTmp[rdInx].start;
				rcTmp[rdInx].flag=opt_const.THROUGHEXPERT;
				rcTmp[rdInx].vo=throughExpert.endV;
			}
			else
			{
				rdInx=rdInx+1;
				if(rdInx+1>len_cat)
				{
					rcTmp=(ROADCATEGORY*)realloc(rcTmp,sizeof(ROADCATEGORY)*(rdInx+1));
					if(rcTmp==NULL)
					{
						pre_exception_decorator(PRE_MALLOC_ERROR,pre_ret,"#reRoadCategory#rcTmp");
						return rcTmp;
					}
				}
				rcTmp[rdInx].start=(int)throughExpert.start;
				rcTmp[rdInx].vo=throughExpert.endV;
				rcTmp[rdInx].flag=opt_const.THROUGHEXPERT;
			}

			if(i+1<upointsMaxLen && upoints[i+1].value-throughExpert.ending < loco_length/2) //右侧需要合并
			{
				rcTmp[rdInx].end=upoints[i+1].value;
				rcTmp[rdInx].length=rcTmp[rdInx].end-rcTmp[rdInx].start;
				i=i+1;
			}
			else
			{
				rcTmp[rdInx].end=(int)throughExpert.ending;
				rcTmp[rdInx].length=rcTmp[rdInx].end-rcTmp[rdInx].start;
				upoints[i].flag=0;
			}
			leftFlag=0;
			tFlag=0;
			ifLastHandArea=1;
		}
		else if(pFlag==1 && upoints[i].flag==opt_const.PUSHERENGINE)//补机结束位置
		{
			if(leftFlag==1) //左侧需要合并
			{
				rcTmp[rdInx].end=(int)pusherEngines.handAreas[pInx].ending;
				rcTmp[rdInx].length=rcTmp[rdInx].end-rcTmp[rdInx].start;
				rcTmp[rdInx].flag=opt_const.PUSHERENGINE;
			}
			else
			{
				rdInx=rdInx+1;
				if(rdInx+1>len_cat)
				{
					rcTmp=(ROADCATEGORY*)realloc(rcTmp,sizeof(ROADCATEGORY)*(rdInx+1));
					if(rcTmp==NULL)
					{
						pre_exception_decorator(PRE_MALLOC_ERROR,pre_ret,"#reRoadCategory#rcTmp");
						return rcTmp;
					}
				}
				rcTmp[rdInx].start=(int)pusherEngines.handAreas[pInx].start;
				rcTmp[rdInx].flag=opt_const.PUSHERENGINE;
			}

			if(i+1<upointsMaxLen && upoints[i+1].value-(int)pusherEngines.handAreas[pInx].ending < loco_length/2) //右侧需要合并
			{
				rcTmp[rdInx].end=upoints[i+1].value;
				rcTmp[rdInx].length=rcTmp[rdInx].end-rcTmp[rdInx].start;
				i=i+1;
			}
			else
			{
				rcTmp[rdInx].end=(int)pusherEngines.handAreas[pInx].ending;
				rcTmp[rdInx].length=rcTmp[rdInx].end-rcTmp[rdInx].start;
				upoints[i].flag=0;
			}
			leftFlag=0;
			pFlag=0;
			ifLastHandArea=1;
		}
		else if(pFlag==0 && tFlag==0)//普通段
		{
			rdInx=rdInx+1;
			if(rdInx+1>len_cat)
			{
				rcTmp=(ROADCATEGORY*)realloc(rcTmp,sizeof(ROADCATEGORY)*(rdInx+1));
				if(rcTmp==NULL)
				{
					pre_exception_decorator(PRE_MALLOC_ERROR,pre_ret,"#reRoadCategory#rcTmp");
					return rcTmp;
				}
			}
			rcTmp[rdInx].start=upoints[i].value;
			rcTmp[rdInx].end=upoints[i+1].value;
			rcTmp[rdInx].length=rcTmp[rdInx].end-rcTmp[rdInx].start;
			if(ifLastHandArea==0)//上一段为非手动区域段，则用当前段的起始距离查找匹配的原始段的分段类型
			{
				for(j=cInx;j<len_cat;j++)
				{
					if(rcTmp[rdInx].start==cat[j].start)
					{
						cInx=j+1;
						rcTmp[rdInx].value=cat[j].gradient;
						rcTmp[rdInx].flag=cat[j].flag;
						break;
					}
				}
			}
			else //上一段为手动区域段，则用当前段的末尾距离查找匹配的原始段的分段类型
			{
				for(j=cInx-1;j<len_cat;j++)
				{
					if(rcTmp[rdInx].end==cat[j].end)
					{
						rcTmp[rdInx].value=cat[j].gradient;
						rcTmp[rdInx].flag=cat[j].flag;
						break;
					}
				}
			}
			ifLastHandArea=0;
			i++;
		}
	}
	//	fid = fopen("././ret/points", "w");
	//	if (fid == NULL) {
	//		fprintf(stderr, "open roadCategory_new err!\n");
	//	}
	//	for(i=0;i<maxPntLen;i++)
	//		fprintf(fid, "%d\t%d\t\n", points[i].value,points[i].flag);
	//	fclose(fid);
	//
	//	fid = fopen("././ret/roadCategory_new", "w");
	//	if (fid == NULL) {
	//		fprintf(stderr, "open roadCategory_new err!\n");
	//	}
	//
	//	for (i = 0; i < rdInx+1; i++) {
	//		fprintf(fid, "%d\t%d\t%d\t%d\t%f\t%f\n", rcTmp[i].start,rcTmp[i].end,rcTmp[i].length,rcTmp[i].flag,rcTmp[i].value,rcTmp[i].vo);
	//	}
	//	fclose(fid);
	*rc_tmp_len=rdInx+1;
	return rcTmp;
}
/**
 * @brief 交换数据
 * @param  src  原始数组
 * @param  i   用于交换的数的下标
 * @param j    用于交换的数的下标
 */
void swapByCat(CATPOINT* src,int i,int j)
{
	int key=src[i].value;
	int flag=src[i].flag;
	src[i].value=src[j].value;
	src[i].flag=src[j].flag;
	src[j].value=key;
	src[j].flag=flag;
}
/**
 * @brief 快排序获得分割点
 * @param src  原始数组
 * @param  i   起始index
 * @param  j 终止index
 * @return  分割点的index
 */
int partitionByCat(CATPOINT* src,int i,int j)
{
	int key=src[i].value;
	int flag=src[i].flag;
	int m=i+1;
	int n=j;
	while(m<=n)
	{
		while(m<=n && src[m].value<=key) m++;
		while(m<=n && src[n].value>=key) n--;
		if(m>=n)
			break;
		swapByCat(src,m,n);
	}
	src[i].value=src[n].value;
	src[i].flag=src[n].flag;
	src[n].value=key;
	src[n].flag=flag;
	return n;
}
/**
 * @brief 快速排序
 * @param src  原始数组
 * @param  i  起始index
 * @param  j  终止index
 */
void quickSortByCat(CATPOINT* src,int i,int j)
{
	if(i<j)
	{
		int p=partitionByCat(src,i,j);
		quickSortByCat(src,i,p-1);
		quickSortByCat(src,p+1,j);
	}
}
/**
 * @brief  去除重复点
 * @param src  原始数组
 * @param srcLength 原始数组长度
 * @param dest 目的数组
 * @param destLength  目的数组长度(去除重复点之后的长度)
 */
void uniqueByCat(CATPOINT src[],int srcLength,CATPOINT* dest,int* desLength)
{
	int i=0;
	for(;i<srcLength;i++)
	{
		if (i==0)
		{
			dest[*desLength].value=src[i].value;
			dest[*desLength].flag=src[i].flag;
			*desLength=*desLength+1;
		}
		else
		{
			int j=0;
			int flag=0;
			for(;j<*desLength;j++)
			{
				if(src[i].value==dest[j].value)
				{
					flag=1;
					if(src[i].flag==opt_const.PUSHERENGINE || src[i].flag==opt_const.THROUGHEXPERT)
						dest[j].flag=src[i].flag;
					break;
				}
			}
			if(flag==0)
			{
				dest[*desLength].value=src[i].value;
				dest[*desLength].flag=src[i].flag;
				*desLength=*desLength+1;
			}
		}
	}
}
