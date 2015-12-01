/**
 * @file     limitStrategy04.c
 * @brief  限速范围跨陡下坡和陡上坡（陡下坡先，陡上坡后）
 * @date  2014.4.14
 * @author danny
 * @note
 */

#include "limitAdjustStrategy.h"
/**
 * @brief 限速范围跨陡下坡和陡上坡（陡下坡先，陡上坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT limitStrategy04(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart)
{
	int index;
//	float step = opt_const.TSTEP;
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
	char desc[EXP_DESC_LENGTH]="#limitStrategy04";
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;

	float headX ;
	int gear ;
	float v ;
	float delta_e ;
	float delta_t ;
	int flag;
	float s;
	float delta_v;
//	float T;
	int i ;
//	float t;
	int indexM;
	float headNX;
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

	if(longflag == 0 || longflag == 1) //属于长大限速的中间段和末尾段，前不需要往回接
		value = optsch[index1-1].velocity;

	index = indexM + 1;
	flag = 0;
	int unusualIndex=index;
	while (1)  //陡上坡开始处存在一些小的陡下坡，将小的陡下坡加入到陡下坡进行处理（寻找分界线）
	{
		if (optsch[index].start > optsch[index2].start)
			break;
		if (optsch[index].velocity < optsch[index+1].velocity)
		{
			flag = 1;
			unusualIndex=index;
			index = index + 1;
			continue;
		}
		index = index + 1;
	}

	if (flag == 1 && unusualIndex!=index2)
		index = unusualIndex + 1;
	else
		index = index2;

	int indexTemp = index;
	//	T=abs(optsch[index1].start-optsch[index].start)*3.6/value;
	//	t=T/abs(index-index1);
	//
	//	for (i=index1;i<=index;i++)
	//	{
	//		optsch[i].gear = -9;
	//		optsch[i].velocity = value;
	//		optsch[i].time = t;
	//	}
	//限速部分处理
	for(i = index1+1;i<=index;i++){
		float del_s=optsch[i+1].start-optsch[i].start;
		optsch[i].gear =  identifyGear(optsch, i, opt_const.AIRE);
		optsch[i].velocity = value;
		optsch[i].time = del_s*3.6/value;
	}

	if(longflag == 0||longflag == 1|| fabs(start-optsch[0].start)<opt_const.DELTA_S) //属于长大限速的中间段和末尾段，前不需要往回接,限速当前的起始位置于优化曲线的起始位置几乎重合，则不需要往回接
	{}
	else   //往后接，寻找空气制动的点，进行空气制动到限速起始位置
	{
		index = index1;
		while(1)
		{
			if(((optsch[index1].start-optsch[index].start)<opt_const.AIRLENGTH || fabs((optsch[index1].start-optsch[index].start)-opt_const.AIRLENGTH)<opt_const.PRECISION) && optsch[index].velocity>value)
				index=index-1;
			else
			{
				break;
			}

		}
		//		T=abs(optsch[index].start-optsch[index1-1].start)*3.6/(abs(optsch[index1-1].velocity+optsch[index].velocity)/2);
		//		t=T/abs(index-index1+1);
		//
		//		for (i=index ; i <index1-1 ;i++)
		//		{
		//			optsch[i].gear = -10;
		//			optsch[i].velocity = value;
		//			optsch[i].time = t;
		//		}
		float deltaV=optsch[index].velocity-value;
		float deltaS=optsch[index1-1].start-optsch[index].start;
		for (i=index+1;i<index1-1;i++)
		{
			float del_s=optsch[i].start-optsch[i-1].start;
			float del_s1=optsch[i+1].start-optsch[i].start;
			optsch[i].gear= identifyGear(optsch, i, opt_const.AIRS); //提醒空气制动标识
			optsch[i].velocity=optsch[i-1].velocity-del_s*deltaV/deltaS;//速度为最大限速
//			float time=del_s1*3.6/optsch[i].velocity;
			optsch[i].time=del_s1*3.6/optsch[i].velocity;
		}
	}

	if(longflag == -1 || longflag == 0 || fabs(ending-optsch[len-1].start)<opt_const.DELTA_S) //长大限速为首段或者中间段，或者限速当前的终止位置与优化曲线的终止位置几乎重合时，此时也只需从限速开始位置开始牵引知道限速结束
	{
		gear = opt_const.MAXGEAR;
		v = value;
		headX = optsch[indexTemp].start;
		indexTemp = indexTemp + 1;
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
			if (indexTemp >= index2)
				break;
			optsch[indexTemp].start = headX;
			optsch[indexTemp].gear =  identifyGear(optsch, indexTemp, gear);
			optsch[indexTemp].velocity = v;
			optsch[indexTemp].consumption = delta_e;
			optsch[indexTemp].time = delta_t;
			optsch[indexTemp].acc = delta_v / delta_t;
			indexTemp = indexTemp + 1;
		}
	}
	else
	{
		gear = opt_const.MAXGEAR;
		v = value;
		headX = optsch[indexTemp].start;
		indexTemp = indexTemp + 1;
		while (1)
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
			v = v+ delta_v;
			if (v > optsch[indexTemp].velocity || fabs(v - optsch[indexTemp].velocity)<opt_const.PRECISION)
				break;
			optsch[indexTemp].start = headX;
			optsch[indexTemp].gear =  identifyGear(optsch, indexTemp, gear);
			optsch[indexTemp].velocity = v;
			optsch[indexTemp].consumption = delta_e;
			optsch[indexTemp].time = delta_t;
			optsch[indexTemp].acc = delta_v / delta_t;
			indexTemp = indexTemp + 1;
		}
	}
	return ret;
}
