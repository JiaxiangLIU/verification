/**
 * @file     limitStrategy07.c
 * @brief 限速范围跨缓坡和陡下坡（缓坡先，陡下坡后）
 * @date  2014.4.14
 * @author danny
 * @note
 */

#include "limitAdjustStrategy.h"
/**
 * @brief  限速范围跨缓坡和陡下坡（缓坡先，陡下坡后）
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT limitStrategy07(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart)
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
	OPTSCH* optBack=NULL;
//	int ln = 0;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#limitStrategy07";
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
	if(longflag == -1 || longflag == 0 || fabs(ending-optsch[len-1].start)<opt_const.DELTA_S) //长大限速为首段或者中间段，或者限速当前的终止位置与优化曲线的终止位置几乎重合，无需往前牵引))
	{}
	else  //通过惰行从限速末端追上曲线，若超出该段才追上曲线则变换档位让其在段内追上
	{
		gear = 0;
		v = value;
		headX = optsch[index2].start;
		delta_e = 0.0;
		optsch[index2].start = headX;
		optsch[index2].gear =  identifyGear(optsch, index2, gear);
		optsch[index2].velocity = v;
		optsch[index2].consumption = delta_e;
		optsch[index2].time = step;
		indexTemp=index2+1;
		while (1)
		{
			s = optsch[indexTemp].start - headX;
			if(v<opt_const.MINV && gear!=opt_const.MAXGEAR)
			{
				gear=gear+1;
				headX=optsch[index2].start;
				v=value;
				indexTemp=index2+1;
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
			if(optsch[indexTemp].start>rc[indexE].end)
			{
				gear=gear+1;
				headX=optsch[index2].start;
				v=value;
				indexTemp=index2+1;
				continue;
			}
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

	indexTemp = index2 - 1; //通过惰行反求到分界点
	gear = 0;
	v = value;
	int flagZeroV=0;
	headX = optsch[index2].start;
	int index_back=-1;
	flag=0;
	while(1)
	{
		s = headX - optsch[indexTemp].start;
		if(v<0 && gear!=opt_const.MINGEAR)  //若中途出现速度小于0,则向下降低一个档位，并设置flagMinV=1作为标记
		{
			if(index_back!=-1)
				free(optBack);
			indexTemp=index2-1;
			gear=gear-1;
			v=value;
			headX=optsch[index2].start;
			index_back=-1;
			flagZeroV=1;
			continue;
		}
		if(v>value && gear!=opt_const.MAXGEAR) //若出现速度大于限速，向上提高一个档位，若此时该档位的上一个档位使得速度降低到小于0,则不能再提高档位，则标记为空气制动
		{
			if(gear<opt_const.MAXGEAR && flagZeroV!=1)
			{
				if(index_back!=-1)
					free(optBack);
				indexTemp=index2-1;
				gear=gear+1;
				v=value;
				headX=optsch[index2].start;
				index_back=-1;
				continue;
			}
			else
			{
				flag=1;
				break;
			}
		}
		trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		v = v-delta_v;
		headX = headX - s;
		if (headX < optsch[indexM].start)
			break;
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
		indexTemp = indexTemp -1;
	}
	if(index_back!=-1)
	{
		int ln=index_back;
		index_back = 0;
		indexTemp=index2-1;
		while(index_back<=ln)
		{
			optsch[indexTemp].start = optBack[index_back].start;
			optsch[indexTemp].gear =  identifyGear(optsch, indexTemp, optBack[index_back].gear);
			optsch[indexTemp].velocity = optBack[index_back].velocity;
			optsch[indexTemp].consumption = optBack[index_back].consumption;
			optsch[indexTemp].time = optBack[index_back].time;
			optsch[indexTemp].acc = optBack[index_back].acc;
			index_back=index_back+1;
			indexTemp=indexTemp-1;
		}
		free(optBack);
		optBack=NULL;
	}
	int j;
	if(flag==1)  //对反求超出限速进行空气制动（这种情况只存在，上一个档位使得速度小于0,当前档位又使得速度超过限速，则只能通过空气制动运行）
	{
		if(index2-indexTemp-1<5) //相差较小的点则直接忽略，保证档位能够不变换频繁
			indexTemp=index2-1;
		v=optsch[indexTemp+1].velocity;
		T =fabs(optsch[indexTemp+1].start - optsch[indexM].start)*3.6/v;
		t =T/(float)abs(indexTemp+1-indexM);
		for (j = indexM;j<=indexTemp+1;j++)
		{
			optsch[j].gear =  identifyGear(optsch, j, opt_const.AIRE);
			optsch[j].velocity = v;
			optsch[j].time = t;
		}
		indexTemp=indexM-1; //为反求做准备
	}

	index = indexTemp;   //从两个坡段的中间位置进行反求，其方式同从限速末端进行反求的方式一致
	gear = 0;
	headX  = optsch[indexTemp+1].start;
	v = optsch[indexTemp+1].velocity;
	flag = 0;
	index_back = -1;
	flagZeroV=0;
	while(1)
	{
		s = headX - optsch[index].start;
		if(v<0 && gear!=opt_const.MINGEAR)  //若反求过程中速度小于0,则降低档位，并将其标记为flagZeroV=1;(这种情况只可能出现在较低的限速（一般小于10左右的限速）
		{
			if(index_back!=-1)
				free(optBack);
			gear=gear-1;
			index_back=-1;
			index=indexTemp;
			headX=optsch[indexTemp+1].start;
			v=optsch[indexTemp+1].velocity;
			flagZeroV=1;
			continue;
		}
		trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		v = v - delta_v;
		headX = headX - s;
		if(v>value && (headX > optsch[index1].start || fabs(headX - optsch[index1].start)<opt_const.PRECISION)) //缓坡反求惰行超出限速，则如果继续使用制动档位则使得反求的速度上升更快，因此需要使用牵引档位
		{
			if (gear <opt_const.MAXGEAR && flagZeroV!=1)
			{
				if(index_back!=-1)
					free(optBack);
				gear = gear + 1;
				headX = optsch[indexTemp + 1].start;
				v = optsch[indexTemp+1].velocity;
				if(gear >opt_const.MAXGEAR )gear=opt_const.MAXGEAR;
				index =indexTemp;
				index_back=-1;
				continue;
			}
			else
			{
				flag = 1;
				break;
			}
		}
		if(headX < optsch[index1].start || fabs(headX - optsch[index1].start)<opt_const.PRECISION)
			break;
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

		index=index-1;
	}
	if(index_back!=-1)
	{
		int ln=index_back;
		index_back = 0;
		i = indexTemp;
		while(index_back<=ln)
		{
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

	v = optsch[i].velocity;
	if (flag == 1)
	{
		T =fabs(optsch[i].start - optsch[index1].start)*3.6/v;
		t =T/(float)abs(i-index1);
		for (j = index1;j<=i;j++)
		{
			optsch[j].gear = opt_const.AIRE;
			optsch[j].velocity = v;
			optsch[j].time = t;
		}
		i=index1-1; //为反求做准备
	}
	if(longflag==0 || longflag==1 || fabs(start-optsch[0].start)<opt_const.DELTA_S) //长大限速为中间段或者末尾段，或者限速当前的起始位置与优化曲线的起始位置几乎重合时，无需往回牵引))
	{}
	else
	{
		gear = 0;
		indexTemp = i;
		headX = optsch[i+1].start;
		v = optsch[i+1].velocity;
		index_back=-1;
		while(1)
		{
			s = headX-optsch[indexTemp].start;
			if(v<0 && gear>opt_const.MINGEAR)
			{
				if(index_back!=-1)
					free(optBack);
				gear=gear-1;
				indexTemp=i;
				headX=optsch[i+1].start;
				v=optsch[i+1].velocity;
				index_back=-1;
				continue;
			}
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			headX = headX - s;
			v = v-delta_v;
			if(v>optsch[indexTemp].velocity)
				break;
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
			indexTemp = indexTemp - 1;
		}
		if(index_back!=-1)
		{
			int ln=index_back;
			index_back = 0;
			indexTemp=i;
			while(index_back<=ln)
			{
				optsch[indexTemp].start = optBack[index_back].start;
				optsch[indexTemp].gear =  identifyGear(optsch, indexTemp, optBack[index_back].gear);
				optsch[indexTemp].velocity = optBack[index_back].velocity;
				optsch[indexTemp].consumption = optBack[index_back].consumption;
				optsch[indexTemp].time = optBack[index_back].time;
				optsch[indexTemp].acc = optBack[index_back].acc;
				index_back=index_back+1;
				indexTemp=indexTemp-1;
			}
			free(optBack);
			optBack=NULL;
		}
	}
	return ret;
}
