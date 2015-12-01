/**
 * @file     limitStrategy02.c
 * @brief  限速范围都在陡上坡
 * @date  2014.4.14
 * @author danny
 * @note
 */

#include "limitAdjustStrategy.h"
/**
 *  @brief 限速范围都在陡上坡
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT limitStrategy02(OPTSCH* optsch,int len ,LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart)
{
	int index;
//	float step = opt_const.TSTEP;
	float ending = limitInfo.end;
	float value=limitInfo.limit;
//	int indexS = limitInfo.indexS;
	int index1 = limitInfo.index1;
	while(optsch[index1].start > limitInfo.start) {
		index1= index1 -1;
	}
	float start = optsch[index1].start;
//	int indexE = limitInfo.indexE;
	int index2 = limitInfo.index2;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#limitStrategy02";
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	LIMIT_OPTIMIZE_RESULT limit_ret;

	float headX ;
	int gear ;
	float v ;
	float delta_e ;
	float delta_t ;
	OPTSCH* optBack=NULL;
//	int count_back;
	float s;
	float delta_v;
//	float T;
//	float t;
//	int ln;

	if (longflag==1 || longflag == 0)//属于长大限速的中间段和末尾段，前不需要往回接
	{
		int lmIndex;
		for(lmIndex=index1;lmIndex<index2;lmIndex++)  //寻找大与限速的最小起始点（长大限速会不准需要重新求）
		{
			if(optsch[lmIndex].velocity>value)
			{
				index1=lmIndex;
				break;
			}
		}
		index=index1-1;
	}
	else if(fabs(start-optsch[0].start)<opt_const.DELTA_S) //限速当前的起始位置于优化曲线的起始位置几乎重合，则不需要往回接
	{}
	else //通过惰行反求交点
	{
		int index_back=0 ;
		int index0;
		int flag;
		if(locoInfo.totalWeight<=4000.0)
			value=limitInfo.limit-1;
		index_back = -1;
		headX = optsch[index1].start;
		gear = 0 ;
		v = value ;
		delta_e = 0.0 ;
		delta_t = 0.0 ;
		optBack = (OPTSCH*) malloc (sizeof(OPTSCH));
		if(optBack==NULL)
		{
			limit_opt_exception_decorator(LIMIT_MALLOC_ERROR,&limit_ret,desc);
			return limit_result_adapter(limit_ret);
		}
		index_back=index_back+1;
		optBack[index_back].start = headX;
		optBack[index_back].gear = gear;
		optBack[index_back].velocity = v;
		optBack[index_back].consumption = delta_e;
		optBack[index_back].time = delta_t;
		optBack[index_back].acc = 0.0;
//		count_back = count ;
		index0 = index1;
		flag = 0 ;


		while(1)
		{
			index0 = index0 - 1;
			s = headX - optsch[index0].start;
			//	float start,float v,int gear,float *tStep,int *count,float delta_s,float *delta_v,float *delta_e
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			v = v-delta_v;
			headX = headX - s;
			if ((optsch[index0].velocity < v || fabs(optsch[index0].velocity - v)<opt_const.PRECISION) && ((start - headX )<opt_const.MAXLENGTH || fabs((start - headX )-opt_const.MAXLENGTH)<opt_const.PRECISION))
				break;
			if( (start-headX)>opt_const.MAXLENGTH ) //反求大于MAXLENGTH距离则采用空气制动
			{
				flag = 1;
				break;
			}
			index_back=index_back+1;
			optBack = (OPTSCH*) realloc (optBack,sizeof(OPTSCH)*(index_back+1));
			if(optBack==NULL)
			{
				limit_opt_exception_decorator(LIMIT_REALLOC_ERROR,&limit_ret,desc);
				return limit_result_adapter(limit_ret);
			}
			optBack[index_back].start = headX;
			optBack[index_back].gear = gear;
			optBack[index_back].velocity = v;
			optBack[index_back].consumption = delta_e;
			optBack[index_back].time = delta_t;
			optBack[index_back].acc = delta_v/delta_t;
		}
		if(flag == 1)  //采用空气制动
		{
			int i=index0;//index0处即为MAXLENGTH处
			int k ;
			float deltaV=optsch[i].velocity-value;
			float deltaS=optsch[index1].start-optsch[i].start;
			for (k=i+1;k<=index1;k++)
			{
				float del_s=optsch[k].start-optsch[k-1].start;
				float del_s1=optsch[k+1].start-optsch[k].start;
				optsch[k].gear=identifyGear(optsch, k , opt_const.AIRS); //提醒空气制动标识
				optsch[k].velocity=optsch[k-1].velocity-del_s*deltaV/deltaS;//速度为最大限速
				optsch[k].time=del_s1*3.6/optsch[k].velocity;
			}
		}
		else
		{
			if(index_back!=-1)
			{
				int ln=index_back;
				int i ;
				index_back=0;
				i=index1;
				while (index_back<=ln){
					optsch[i].start = optBack[index_back].start;
					optsch[i].gear =identifyGear(optsch, i , optBack[index_back].gear) ;
					optsch[i].velocity = optBack[index_back].velocity;
					optsch[i].consumption = optBack[index_back].consumption;
					optsch[i].time = optBack[index_back].time;
					optsch[i].acc = optBack[index_back].acc;
					index_back=index_back+1;
					i=i-1;
				}
				free(optBack);
				optBack=NULL;
			}
		}
		index=index1;
	}
	int indexTemp;
	if (longflag == 0 || longflag == -1 || fabs(ending-optsch[len-1].start)<opt_const.DELTA_S) //长大限速为首段或者中间段，或者限速当前的终止位置与优化曲线的终止位置几乎重合时，此时也只需从限速开始位置开始牵引知道限速结束
	{
		indexTemp = index+1;
		gear = opt_const.MAXGEAR;
		v = optsch[index].velocity;
		headX = optsch[index].start;
		while(1)
		{
			s = optsch[indexTemp].start - headX;
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			headX = headX + s;
			v = v + delta_v;
			if(indexTemp >= index2)
				break;
			optsch[indexTemp].start = headX;
			optsch[indexTemp].gear = identifyGear(optsch, indexTemp, gear);
			optsch[indexTemp].velocity = v;
			optsch[indexTemp].consumption = delta_e;
			optsch[indexTemp].time = delta_t;
			optsch[indexTemp].acc = delta_v/delta_t;
			indexTemp = indexTemp + 1;
		}
	}
	else
	{
		indexTemp = index + 1;
		int srcGear=4;
		if(locoInfo.totalWeight>4000.0)
			gear=opt_const.MAXGEAR;
		else
			gear = srcGear;//gear=8
		v = optsch[index].velocity ;
		headX = optsch[index].start;
		while(1)
		{
			s = optsch[indexTemp].start - headX ;
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			headX = headX + s;
			v = v + delta_v;
			if(v>optsch[indexTemp].velocity || fabs(v-optsch[indexTemp].velocity)<opt_const.PRECISION)
				break;
			optsch[indexTemp].start = headX;
			optsch[indexTemp].gear =  identifyGear(optsch, indexTemp, gear);
			optsch[indexTemp].velocity = v;
			optsch[indexTemp].consumption = delta_e;
			optsch[indexTemp].time = delta_t;
			optsch[indexTemp].acc = delta_v/delta_t;
			indexTemp = indexTemp + 1;
		}
	}
	return ret;
}
