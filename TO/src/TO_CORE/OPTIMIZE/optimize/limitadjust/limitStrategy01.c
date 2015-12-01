/**
 * @file     limitStrategy01.c
 * @brief  限速范围全在陡下坡/超抖下坡 或者跨陡下坡和超陡下坡
 * @date  2014.4.14
 * @author danny
 * @note
 */
#include "limitAdjustStrategy.h"
/**
 * @brief 限速范围全在陡下坡/超抖下坡 或者跨陡下坡和超陡下坡
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategy01(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart)
{
	int index;
//	float step = opt_const.TSTEP;
	float ending = limitInfo.end;
	float value = limitInfo.limit;
//	int indexS = limitInfo.indexS;
	int index1 = limitInfo.index1;
	float start = limitInfo.start;
//	int indexE = limitInfo.indexE;
	int index2 = limitInfo.index2;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#limitStrategy01";
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;

	int gear ;
	float delta_e ;
	float delta_t ;
	int s;
	float delta_v;
//	float T;
	int i ;
//	float t;


	if (longflag==0||longflag==1) //属于长大限速的中间段和末尾段，前不需要往回接
		value=optsch[index1-1].velocity;
	else if(fabs(start-optsch[0].start)<opt_const.DELTA_S) //限速当前的起始位置于优化曲线的起始位置几乎重合，则不需要往回接
	{}
	else
	{
		if(flagS == 0) //往后距离限速AIRLENGTH位置开始空气制动
		{
			s = index1;
			index = index1;
			while(1)
			{
				if(index<0) //已经反求到路线开头
					break;
				if(((optsch[s].start-optsch[index].start)<opt_const.AIRLENGTH || fabs((optsch[s].start-optsch[index].start)-opt_const.AIRLENGTH)<opt_const.PRECISION) && optsch[index].velocity>value)
					index=index-1;
				else
				{
					break;
				}
			}

			float deltaV=optsch[index].velocity-value;
			float deltaS=optsch[index1].start-optsch[index].start;
			for (i=index+1;i<=index1;i++)
			{
				float del_s=optsch[i].start-optsch[i-1].start;
				float del_s1=optsch[i+1].start-optsch[i].start;
				optsch[i].gear= identifyGear(optsch, i , opt_const.AIRS); //提醒空气制动标识
				optsch[i].velocity=optsch[i-1].velocity-del_s*deltaV/deltaS;//速度为最大限速
//				float time=del_s1*3.6/optsch[i].velocity;
				optsch[i].time=del_s1*3.6/optsch[i].velocity;
			}
		}
	}
	//限速部分通过空气制动保证匀速
	for(i = index1-1;i<=index2;i++){
		float del_s=optsch[i+1].start-optsch[i].start;
		optsch[i].gear =identifyGear(optsch, i , opt_const.AIRE);
		optsch[i].velocity = value;
		optsch[i].time = del_s*3.6/value;
	}

	if(longflag == 0|| longflag == -1) //属于长大限速的中间段和开始段，前不需要往后
	{}
	else if(fabs(ending-optsch[len-1].start)<opt_const.DELTA_S) //限速当前的终止位置于优化曲线的终止位置几乎重合，则不需要往后接
	{}
	else
	{
		float headX;
		float v ;
		float s1;
		int indexTemp ;
		int tempGear ;
		indexTemp = index2 + 1;
		gear = 0 ;
		v = value ;
		headX = optsch[index2].start;
		while(1)
		{
			if(indexTemp>=len)  //已经追到末尾
				break;
			s1 = optsch[indexTemp].start - headX ;
			tempGear = gear;
			trac_ret=DoCaculateByDistance(headX,v,tempGear,&delta_t,&count,s1,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			if(delta_v >0.0 || fabs(delta_v -0.0)<opt_const.PRECISION)
				gear = tempGear;
			else
			{
				gear = 1;
				while (gear<=opt_const.MAXGEAR)
				{
					trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s1,&delta_v,&delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if(delta_v>0.0 ||fabs(delta_v-0.0)<opt_const.PRECISION)
						break;
					gear = gear + 1;
				}
				if(gear > opt_const.MAXGEAR)gear =opt_const. MAXGEAR;
			}
			headX = headX + s1;
			v = v + delta_v;
			if(v>optsch[indexTemp].velocity || fabs(v-optsch[indexTemp].velocity)<opt_const.PRECISION)
				break;
			optsch[indexTemp].start = headX;
			optsch[indexTemp].gear = identifyGear(optsch, indexTemp , gear);
			optsch[indexTemp].velocity = v;
			optsch[indexTemp].consumption = delta_e;
			optsch[indexTemp].time = delta_t;
			optsch[indexTemp].acc = delta_v/delta_t;
			indexTemp = indexTemp + 1;
		}
	}
	return ret;
}
