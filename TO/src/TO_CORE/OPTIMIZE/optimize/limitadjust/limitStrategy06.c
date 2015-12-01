/**
 * @file     limitStrategy06.c
 * @brief 限速范围跨陡下坡和缓坡（陡下坡先，缓坡后）
 * @date  2014.4.14
 * @author danny
 * @note
 */

#include "limitAdjustStrategy.h"
/**
 * @brief  限速范围跨陡下坡和缓坡（陡下坡先，缓坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT limitStrategy06(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart)
{
	int index;
	float step = opt_const.TSTEP;
	float ending = limitInfo.end;
	float value = limitInfo.limit;
	int indexS = limitInfo.indexS;
	int index1 = limitInfo.index1;
	while(optsch[index1].start > limitInfo.start) {
		index1= index1 -1;
	}
	float start = optsch[index1].start;
	int indexE = limitInfo.indexE;
	int index2 = limitInfo.index2;
	//异常处理描述
	char desc[EXP_DESC_LENGTH] ="#limitStrategy06";
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	LIMIT_OPTIMIZE_RESULT limit_ret;

	float headX ;
	int gear ;
	float v ;
	float delta_e ;
	float delta_t ;
	int flag;
	float s;
	float delta_v;
	float T;
	int i ;
	float t;
	int indexM;
	float headNX;
	int indexTemp;
	int index_back;
	OPTSCH* optBack=NULL;


	headX = optsch[index1].start;
	indexM = index1;
	for(i=index1;i<=index2-1;i++)
	{
		headX = optsch[i].start;
		headNX = optsch[i+1].start;
		if((rc[indexS].start < headX || fabs(rc[indexS].start - headX)<opt_const.PRECISION) && ( rc[indexS].end > headX || fabs( rc[indexS].end - headX)<opt_const.PRECISION)
				&& (rc[indexE].start< headNX || fabs(rc[indexE].start-headNX)<opt_const.PRECISION) && (rc[indexE].end> headNX || fabs(rc[indexE].end - headNX)<opt_const.PRECISION))
		{
			indexM = i;break;
		}
	}

	if (longflag == -1 || longflag == 0 || fabs(ending-optsch[len-1].start)<opt_const.DELTA_S) //长大限速为首段或者中间段，或者限速当前的终止位置与优化曲线的终止位置几乎重合，无需往前牵引)
	{}
	else  //尽量使用较低的牵引档位保证速度能够递增且在1000米之内追上曲线
	{
		index_back=-1;
		gear = 0;
		headX = optsch[index2].start;
		v = value;
		delta_e = step*locoInfo.dragConsumption[gear-1];
		optsch[index2].start = headX;
		optsch[index2].gear = identifyGear(optsch, index2, gear);
		optsch[index2].velocity = v;
		optsch[index2].consumption = delta_e;
		optsch[index2].time = step;
		indexTemp=index2+1;
		while(1)
		{
			s = optsch[indexTemp].start - headX;
			if(v<opt_const.MINV && gear<opt_const.MAXGEAR)
			{
				if(index_back!=-1)
					free(optBack);
				index_back=-1;
				v=value;
				headX=optsch[index2].start;
				indexTemp=index2+1;
				gear=gear+1;
				continue;
			}
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			headX = headX + s;
			v = v + delta_v;
			if (v>optsch[indexTemp].velocity || fabs(v-optsch[indexTemp].velocity)<opt_const.PRECISION)
			{
				break;
			}
			if(fabs(optsch[indexTemp].start-optsch[index2].start)>opt_const.MAXLENGTH && gear<opt_const.MAXGEAR)
			{
				if(index_back!=-1)
					free(optBack);
				index_back=-1;
				v=value;
				headX=optsch[index2].start;
				indexTemp=index2+1;
				gear=gear+1;
				continue;
			}
			index_back=index_back+1;
			if (index_back == 0){
				optBack =(OPTSCH*) malloc(sizeof(OPTSCH));
			}
			else{
				optBack = (OPTSCH*) realloc(optBack,sizeof(OPTSCH)*(index_back+1));
			}
			if(optBack==NULL)
			{
				limit_opt_exception_decorator(LIMIT_MALLOC_ERROR,&limit_ret,desc);
				return limit_result_adapter(limit_ret);
			}
			optBack[index_back].start = headX;
			optBack[index_back].gear = gear;
			optBack[index_back].velocity = v;
			optBack[index_back].consumption = delta_e;
			optBack[index_back].time = delta_t;
			optBack[index_back].acc = delta_v/delta_t;
			//
			//			optsch[indexTemp].start = headX;
			//			optsch[indexTemp].gear = gear;
			//			optsch[indexTemp].velocity = v;
			//			optsch[indexTemp].consumption = delta_e;
			//			optsch[indexTemp].time = delta_t;
			//			optsch[indexTemp].acc = delta_v / delta_t;
			indexTemp = indexTemp + 1;
		}
		if(index_back!=-1)
		{
			int ln=index_back;
			index_back=0;
			i=index2+1;
			while (index_back<=ln){
				optsch[i].start = optBack[index_back].start;
				optsch[i].gear =  identifyGear(optsch, i, optBack[index_back].gear);
				optsch[i].velocity = optBack[index_back].velocity;
				optsch[i].consumption = optBack[index_back].consumption;
				optsch[i].time = optBack[index_back].time;
				optsch[i].acc = optBack[index_back].acc;
				index_back=index_back+1;
				i=i+1;
			}
			free(optBack);
			optBack=NULL;
		}
	}

	indexTemp=index2-1;  //反求尽量使用较低的档位反求
	gear=0;
	while(1)  //找到最低递增档位
	{
		trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		if(delta_v>0)
			break;
		else
			gear=gear+1;
		if(gear>opt_const.MAXGEAR)
		{
			gear=opt_const.MAXGEAR;
			break;
		}
	}
	v=value;
	headX=optsch[index2].start;
	while (1)
	{
		s = headX - optsch[indexTemp].start;
		trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		if(delta_v<0)
		{       gear=0;
		while(1)
		{
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			if(delta_v>0)
				break;
			else
				gear=gear+1;
			if(gear>opt_const.MAXGEAR)
			{
				gear=opt_const.MAXGEAR;
				break;
			}
		}
		}
		//		printf("%d\n",gear);
		v = v-delta_v;
		headX = headX - s;
		if (headX < optsch[indexM].start)
			break;
		optsch[indexTemp].start = headX;
		optsch[indexTemp].gear =  identifyGear(optsch, indexTemp, gear);
		optsch[indexTemp].velocity = v;
		optsch[indexTemp].consumption = delta_e;
		optsch[indexTemp].time = delta_t;
		optsch[indexTemp].acc = delta_v/delta_t;
		indexTemp = indexTemp -1;
	}

	flag = 0;
	index = indexTemp;
	v = optsch[indexTemp+1].velocity;
	float delv1,delv2,delv;
	while(1)
	{
		delv1=v-optsch[index].velocity;
		delv2=v-optsch[index-1].velocity;
		delv=delv1*delv2;
		if( delv1>0.0 && delv2>0.0 && ( delv< 0.0 || fabs(delv - 0.0)<opt_const.PRECISION)
				&& (optsch[index].start > optsch[index1].start || fabs(optsch[index].start - optsch[index1].start) < opt_const.PRECISION)) //找到E点
		{
			flag=1;
			break;
		}
		if(index < index1)break;
		index = index - 1;
	}

	v = optsch[indexTemp+1].velocity;
	if(flag == 1)
	{
		int sIndex;
		sIndex = index1;
		T = fabs(optsch[sIndex].start-optsch[index+1].start)*3.6/v;
		if((sIndex-index+1) == 0)
			t=0.0;
		else
			t=T/(float)abs(index+1-sIndex);
		for(i=sIndex;i<=index+1;i++)
		{
			optsch[i].gear= identifyGear(optsch, i, opt_const.AIRE);
			optsch[i].velocity = v;
			optsch[i].time = t;
		}
	}
	else
	{
		index = indexTemp;
		T=fabs(optsch[index].start-optsch[index1].start)*3.6/(v);
		if(index-index1==0)
			t=0.0;
		else
			t=T/(float)abs(index-index1);
		while(1)
		{
			if(index<index1)
				break;
			optsch[index].gear= identifyGear(optsch, index, opt_const.AIRE);
			optsch[index].velocity=v;
			optsch[index].time=t;
			index=index-1;
		}
	}
	if(longflag==0 || longflag==1 || fabs(start-optsch[0].start)<opt_const.DELTA_S) //长大限速为中间段或者末尾段，或者限速当前的起始位置与优化曲线的起始位置几乎重合时，无需往回牵引)
	{}
	else
	{
		int s1;
		s1 = index1;
		index = index1 - 1;
		//		while(1)
		//		{
		//			if((optsch[s1].start-optsch[index].start)>1000 && optsch[index]>value)break;
		//			index = index -1;
		//		}
		while(1)
		{
			if(((optsch[s1].start-optsch[index].start)<opt_const.AIRLENGTH  || fabs((optsch[s1].start-optsch[index].start) - opt_const.AIRLENGTH )<opt_const.PRECISION)&& optsch[index].velocity>optsch[index1].velocity)
				index=index-1;
			else
			{
				break;
			}
		}
		//		T=abs(optsch[index1-1].start-optsch[index].start)*3.6/(abs(optsch[index].velocity+v)/2);
		//		if(index1-1-index == 0)
		//			t=0;
		//		else
		//		{
		//			t = T/abs(index1-1-index);
		//			for(i=index;i<=index1-1;i++)
		//			{
		//				optsch[i].gear=AIRS;
		//				optsch[i].velocity=v;
		//				optsch[i].time=t;
		//
		//			}
		//		}
		float deltaV=optsch[index].velocity-optsch[index1].velocity;
		float deltaS=optsch[index1].start-optsch[index].start;
		for (i=index;i<=index1;i++)
		{
			float del_s=optsch[i].start-optsch[i-1].start;
			float del_s1=optsch[i+1].start-optsch[i].start;
			optsch[i].gear= identifyGear(optsch, i, opt_const.AIRS); //提醒空气制动标识
			optsch[i].velocity=optsch[i-1].velocity-del_s*deltaV/deltaS;//速度为最大限速
//			float time=del_s1*3.6/optsch[i].velocity;
			optsch[i].time=del_s1*3.6/optsch[i].velocity;
		}
	}
	return ret;
}
