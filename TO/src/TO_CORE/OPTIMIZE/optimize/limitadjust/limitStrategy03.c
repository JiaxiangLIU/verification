/**
 * @file     limitStrategy03.c
 * @brief  限速范围都在缓坡（缓上坡，缓下坡，平坡）
 * @date  2014.4.14
 * @author danny
 * @note
 */

#include "limitAdjustStrategy.h"
/**
 * @brief 限速范围都在缓坡（缓上坡，缓下坡，平坡）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT limitStrategy03(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart)
{
	int index;
	float step = opt_const.TSTEP;
	float ending = limitInfo.end;
	float value = limitInfo.limit;
	float start = limitInfo.start;
	int index1 = limitInfo.index1;
	if(flagS==1) {
		int i;
		for(i=index1;i>0;i--) {
			if(optsch[i].start<srcStart)
			{
				index1=i;
				start = optsch[index1].start;
				break;
			}
		}
	} else{
		while(optsch[index1].start > limitInfo.start) {
			index1= index1 -1;
		}
		start = optsch[index1].start;
	}

	int index2 = limitInfo.index2;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#limitStrategy03";
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	LIMIT_OPTIMIZE_RESULT limit_ret;

	int index_back ;
	float headX ;
	int gear ;
	float v ;
	float delta_e ;
	float delta_t ;
	OPTSCH* optBack = NULL;
	int count_back;
	int index0;
	int flag = 0;
	float s;
	float delta_v;
	int i ;

	if(longflag == 1 || longflag == 0)//属于长大限速的中间段和末尾段，前不需要往回接
		value = optsch[index1 - 1].velocity;
	else if(fabs(start-optsch[0].start)<opt_const.DELTA_S) //限速当前的起始位置于优化曲线的起始位置几乎重合，则不需要往回接
	{
		index1=0;
	}
	else   //从限速起始点往回接
	{
		index_back = -1;
		headX = optsch[index1].start;
		gear = 0;
		v = value;
		count_back = count ;
		index0 = index1;
		flag = 0 ;
		optsch[index1].velocity = v;
		optsch[index1].gear = 0;
		if(optsch[index1 - 1].velocity<value-1) //若在限速起始点的优化速度小于限速，则用高档位反求降速求交点
		{
			while(1)
			{
				index0 = index0-1;
				if(index0<0 && gear!=opt_const.MAXGEAR)  //若在反求过程中index0<0则档位递增，让其更快的与原曲线有交点
				{
					if(index_back!=-1)
						free(optBack);
					index_back=-1;
					gear=gear+1;
					if(gear>opt_const.MAXGEAR)
						gear=opt_const.MAXGEAR;
					v=value;
					headX=optsch[index1].start;
					index0=index1;
					continue;
				}
				if(index0<0) //在曲线起始位置，反求到了起始点，此时无需操作，退出即可（该限速距离线路起始位置非常近）
				{
					break;
				}
				s = headX - optsch[index0].start;
				trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				v = v-delta_v;
				headX = headX - s;
				if((optsch[index0].velocity>v || fabs(optsch[index0].velocity-v)<opt_const.PRECISION) && ((start-headX)<opt_const.MAXLENGTH || fabs((start-headX)-opt_const.MAXLENGTH)<opt_const.PRECISION))//反求与曲线相交且距离在特定的范围内则直接退出
					break;
				if((start-headX)>opt_const.MAXLENGTH  &&  gear!=opt_const.MAXGEAR) //若一定距离内没有追上。则扩大档位让其尽快追上
				{
					if(index_back!=-1)
						free(optBack);
					index_back=-1;
					gear=gear+1;
					if(gear>opt_const.MAXGEAR)
						gear=opt_const.MAXGEAR;
					v=value;
					headX=optsch[index1].start;
					index0=index1;
					continue;
				}
				else
				{
					if(start-headX>opt_const.MAXLENGTH && gear==opt_const.MAXGEAR)//通过最大档位反求都无法保证在特定的距离内，则采用空气制动
					{
						flag=1;
						break;
					}
				}

				index_back=index_back+1;
				if(index_back==0)
					optBack=(OPTSCH*) malloc(sizeof(OPTSCH));
				else
					optBack =(OPTSCH*) realloc(optBack,sizeof(OPTSCH)*(index_back+1));
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
			}
		}
		else //若在限速起始点的优化速度大于限速，则用低档位反求降速求交点
		{
			while(1)
			{
				index0 = index0 -1;
				if(index0<0 && gear!=opt_const.MINGEAR)  //到达起始点尚未得到交点，则降低档位让其尽快相交
				{
					if(index_back!=-1)
						free(optBack);
					index_back=-1;
					gear=gear-1;
					if(gear<opt_const.MINGEAR)
						gear=opt_const.MINGEAR;
					v=value;
					headX=optsch[index1].start;
					index0=index1;
					continue;
				}
				if(v<=0 && gear!=opt_const.MINGEAR) //反求过程中出现速度小于0，则降低档位
				{
					if(index_back!=-1)
						free(optBack);
					index_back=-1;
					gear=gear-1;
					if(gear<opt_const.MINGEAR)
						gear=opt_const.MINGEAR;
					v=value;
					headX=optsch[index1].start;
					index0=index1;
					continue;
				}
				if(index0<0)//在曲线起始位置，反求到了起始点，此时无需操作，退出即可（该限速距离线路起始位置非常近）
				{
					break;
				}
				s = headX - optsch[index0].start;
				trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count_back,s,&delta_v,&delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}

				v = v-delta_v;
				headX = headX - s;
				if((optsch[index0].velocity<v || fabs(optsch[index0].velocity-v)<opt_const.PRECISION) && ((start-headX)<opt_const.MAXLENGTH || fabs((start-headX)-opt_const.MAXLENGTH)<opt_const.PRECISION))//反求与曲线相交且距离在特定的范围内则直接退出
					break;
				if((start-headX)>opt_const.MAXLENGTH && gear!=opt_const.MINGEAR) //若一定距离内没有追上。则降低档位让其尽快追上
				{
					if(index_back!=-1)
						free(optBack);
					index_back=-1;
					gear=gear-1;
					if(gear<opt_const.MINGEAR)
						gear=opt_const.MINGEAR;
					v=value;
					headX=optsch[index1].start;
					index0=index1;
					continue;
				}
				else
				{
					if((start-headX)>opt_const.MAXLENGTH && gear==opt_const.MINGEAR)//通过最低档位反求都无法保证在特定的距离内，则采用空气制动
					{
						flag = 1;
						break;
					}
				}

				index_back = index_back + 1;
				if(index_back==0)
					optBack=(OPTSCH*) malloc(sizeof(OPTSCH));
				else
					optBack =(OPTSCH*) realloc(optBack,sizeof(OPTSCH)*(index_back+1));
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
			}
		}
		if ( flag == 1) //从特定距离开始，采用空气制动
		{
			index = index1;
			while(1)
			{
				if(index<=0)
					break;
				if(((optsch[index1].start-optsch[index].start)>opt_const.AIRLENGTH || fabs((optsch[index1].start-optsch[index].start)-opt_const.AIRLENGTH)<opt_const.PRECISION) && optsch[index].velocity>value)
					break;
				index = index - 1;
			}
			float deltaV=optsch[index].velocity-value;
			float deltaS=optsch[index1].start-optsch[index].start;
			for (i=index+1;i<=index1;i++)
			{
				float del_s=optsch[i].start-optsch[i-1].start;
				float del_s1=optsch[i+1].start-optsch[i].start;
				//				if(i==index1)
				//					optsch[i].gear= identifyGear(optsch, i, opt_const.AIRE);
				//				else
				optsch[i].gear= identifyGear(optsch, i, opt_const.AIRS); //提醒空气制动标识
				optsch[i].velocity=optsch[i-1].velocity-del_s*deltaV/deltaS;//速度为最大限速
				//				float time=del_s1*3.6/optsch[i].velocity;
				optsch[i].time=del_s1*3.6/optsch[i].velocity;
			}
		}
		else
		{
			if(index_back!=-1)
			{
				int ln=index_back;
				index_back=0;
				i=index1-1;
				while (index_back<=ln){
					optsch[i].start = optBack[index_back].start;
					optsch[i].gear =  identifyGear(optsch, i, optBack[index_back].gear);
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
	}
	//从限速起始点开始保证限速按照value-1的匀速阈值1的浮动空间上下变动直到限速末端，即速度的范围为[value-2,value]。先让速度下降后上升
	int tempGear;
	int indexTemp ;
	index = index1 + 1;
	headX = optsch[index1].start;
	optsch[index1].velocity = value;
	gear = opt_const.MAXGEAR;
	v = value ;
	count = 1;
	while (gear>=opt_const.MINGEAR)
	{
		s = optsch[index].start - headX;
		trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		if(delta_v < 0.0)
			break;
		gear = gear - 1;
	}
	if ( gear < opt_const.MINGEAR)
		gear = opt_const.MINGEAR ;
	optsch[index1].gear = gear;
	tempGear = gear;
	if(gear > 0)	optsch[index1].consumption = optsch[index1].time*locoInfo.dragConsumption[gear-1];
	else if (gear < 0)
		optsch[index1].consumption = optsch[index1].time*locoInfo.dragConsumption[-gear+1];
	v = value ;
	count = 1;
	while  (headX < optsch[index2].start || fabs(headX - optsch[index2].start)<opt_const.PRECISION)
	{
		gear = tempGear;
		s = optsch[index].start-headX;
		trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		v = v+delta_v;
		headX = headX + s;
		optsch[index].start = headX;
		optsch[index].gear =  identifyGear(optsch, index, gear);
		optsch[index].velocity = v;
		optsch[index].consumption = delta_e;
		optsch[index].time = delta_t;
		optsch[index].acc = delta_v/delta_t;
		if(v<value-2)
		{
			gear = opt_const.MINGEAR;
			while (gear<opt_const.MAXGEAR)
			{
				trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				if(delta_v > 0.0)
				{
					tempGear = gear;
					break;
				}
				gear = gear + 1;
			}
			if (gear>=opt_const.MAXGEAR)
				tempGear = opt_const.MAXGEAR;
		}
		if (v>value)
		{
			gear = opt_const.MAXGEAR;
			while (gear>opt_const.MINGEAR)
			{
				trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				if(delta_v<0.0)
				{
					tempGear = gear;
					break;
				}
				gear = gear - 1;
			}
			if(gear <= opt_const.MINGEAR)
			{
				tempGear = opt_const.MINGEAR;
			}
		}
		index = index + 1;
	}

	//从限速末端向前追直到与原始曲线有交点
	if (longflag == -1 || longflag == 0 || fabs(ending-optsch[len-1].start)<opt_const.DELTA_S) //长大限速为首段或者中间段，或者限速当前的终止位置与优化曲线的终止位置几乎重合时，此时也只需从限速开始位置开始牵引知道限速结束)
	{}
	else
	{
		indexTemp = index;
		gear = 0;
		headX = optsch[index-1].start;
		v=optsch[index-1].velocity;
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
		index_back=-1;
		while (1)
		{
			if(indexTemp>len)//牵引到原始优化曲线最末尾则无需再牵引
				break;
			if(v<0 && gear!=opt_const.MAXGEAR)  //中途速度小于零，则增大档位重新计算
			{
				if(index_back!=-1)
					free(optBack);
				gear=gear+1;
				indexTemp = index;
				headX = optsch[index-1].start;
				v=optsch[index-1].velocity;
				index_back=-1;
				continue;
			}
			s = optsch[indexTemp].start - headX;
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			headX = headX + s;
			v = v+delta_v;
			//已经与原优化曲线相交，且距离在1000米以内
			if(v>optsch[indexTemp].velocity  && (fabs(optsch[indexTemp].start-optsch[index-1].start)<opt_const.MAXLENGTH || fabs(optsch[indexTemp].start-optsch[index-1].start)-opt_const.MAXLENGTH<opt_const.PRECISION))
				break;
			//已经与原优化曲线相交，档位已经为最大档位
			if(v>optsch[indexTemp].velocity && gear==opt_const.MAXGEAR)
				break;
			else
			{
				if(fabs(optsch[indexTemp].start-optsch[index-1].start)>opt_const.MAXLENGTH  && gear!=opt_const.MAXGEAR)//当前距离已经超出1000米，且档位并非最大档位，则增大档位重新计算
				{
					if(index_back!=-1)
						free(optBack);
					gear=gear+1;
					indexTemp = index;
					headX = optsch[index-1].start;
					v=optsch[index-1].velocity;
					index_back=-1;
					continue;
				}
			}
			index_back=index_back+1;
			if(index_back==0)
				optBack=(OPTSCH*) malloc(sizeof(OPTSCH));
			else
				optBack =(OPTSCH*) realloc(optBack,sizeof(OPTSCH)*(index_back+1));
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
			indexTemp = indexTemp + 1;
		}
		if(index_back!=-1)
		{
			int ln=index_back;
			index_back=0;
			i=index;
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
	return ret;
}
