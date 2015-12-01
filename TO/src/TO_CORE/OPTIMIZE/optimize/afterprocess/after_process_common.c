/**
 *  @file after_process_common.c
 *  @brief  优化档位后处里非主入口函数
 *  @date Created on: 2015年4月2日
 *   @author sinlly
 */
#include "after_process.h"
/**
 * @brief 获得两个位置的中间速度（中间公里标位置的速度）
 * @param optSch  优化速度曲线
 * @param sIndex  优化中起始位置的index
 * @param eIndex   优化中终止位置的index
 * @return   中间速度
 */
float getMidV(OPTSCH* optSch,int sIndex,int eIndex)
{
	float midV=0;
	float midDist=optSch[sIndex].start+(optSch[eIndex].start-optSch[sIndex].start)/2;
	int i=sIndex;
	while(optSch[i].start<midDist || fabs(optSch[i].start-midDist)<opt_const.PRECISION)
	{
		i=i+1;
	}
	midV=optSch[i].velocity;
	return midV;
}
/**
 * @brief 判断sIndex到eIndex中是否有点接近限速
 * @param optSch 优化速度曲线
 * @param sIndex 需要判断是否接近限速的起始位置index
 * @param eIndex 需要判断是否接近限速的终止位置index
 * @return 1 表示接近限速，0表示未接近限速
 */
int ifNearLimit(OPTSCH* optSch,int sIndex,int eIndex)
{
	int flag=0;
	int i,j;
	int limitS=0,limitE=MAX_LIMIT-1;
	//找到sIndex和eIndex范围内的限速
	for(i=0;i<MAX_LIMIT;i++)
	{
		if((optSch[sIndex].start> limits[i].start || fabs(optSch[sIndex].start-limits[i].start)<opt_const.PRECISION) && (optSch[sIndex].start< limits[i].end|| fabs(optSch[sIndex].start-limits[i].end)<opt_const.PRECISION))
			limitS=i;
		if((optSch[eIndex].start> limits[i].start || fabs(optSch[eIndex].start-limits[i].start)<opt_const.PRECISION) && (optSch[eIndex].start< limits[i].end || fabs(optSch[eIndex].start-limits[i].end)<opt_const.PRECISION))
		{
			limitE=i;
			break;
		}
	}
	//判断从sIndex和eIndex中的任意一个的点是否超出与限速的阈值
	for(i=sIndex;i<=eIndex;i++)
	{
		for(j=limitS;j<=limitE;j++)
		{
			if((optSch[i].start < (float)limits[j].end ||  fabs(optSch[i].start-(float)limits[j].end)<opt_const.PRECISION)&&(optSch[i].start>(float)limits[j].start || fabs(optSch[i].start-(float)limits[j].start)))
			{
				//				if(fabs(optSch[i].velocity-limits[j].limit) < NEARLIMIT || fabs(optSch[i].velocity-limits[j].limit-NEARLIMIT)<opt_const.PRECISION)
				//				{
				//					flag=1;
				//					break;
				//				}
				if((fabs(optSch[i].velocity - (float)limits[j].limit) <opt_const. NEARLIMIT || fabs(optSch[i].velocity - (float)limits[j].limit-opt_const.NEARLIMIT)<opt_const.PRECISION) && (fabs(optSch[i].velocity-opt_const.LIMITV) < opt_const.NEARLIMIT || fabs(optSch[i].velocity-opt_const.LIMITV- opt_const.NEARLIMIT)<opt_const.PRECISION) )
				{
					flag=1;
					break;
				}
			}
		}
	}
	return flag;
}
/**
 * @brief 判断临时限速中sIndex到eIndex中是否有点接近限速
 * @param optSch  临时限速的优化曲线片段
 * @param sIndex 需要判断是否接近临时限速的起始位置index
 * @param eIndex 需要判断是否接近临时限速的终止位置index
 * @param tmpStart 临时限速的起始公里标
 * @param tmpEnd 临时限速的终止公里标
 * @param tmpValue 临时限速值
 * @return 1 表示接近限速，0表示未接近限速
 */
int ifTmpNearLimit(OPTSCH* optSch,int sIndex,int eIndex,float tmpStart,float tmpEnd,float tmpValue)
{
	int flag=0;
	int i,j;
	int limitS=0,limitE=MAX_LIMIT-1;
	//找到sIndex和eIndex范围内的限速
	if(optSch[eIndex].start<tmpStart || optSch[sIndex].start>tmpEnd)
	{
		for(i=0;i<MAX_LIMIT;i++)
		{
			if((optSch[sIndex].start> (float)limits[i].start || fabs(optSch[sIndex].start-(float)limits[i].start)<opt_const.PRECISION) && (optSch[sIndex].start<(float)limits[i].end|| fabs(optSch[sIndex].start-(float)limits[i].end)<opt_const.PRECISION))
				limitS=i;
			if((optSch[eIndex].start> (float)limits[i].start || fabs(optSch[eIndex].start-(float)limits[i].start)<opt_const.PRECISION) && (optSch[eIndex].start< (float)limits[i].end || fabs(optSch[eIndex].start-(float)limits[i].end)<opt_const.PRECISION))
			{
				limitE=i;
				break;
			}
		}
		//判断从sIndex和eIndex中的任意一个的点是否超出与限速的阈值
		for(i=sIndex;i<=eIndex;i++)
		{
			for(j=limitS;j<=limitE;j++)
			{
				if((optSch[i].start < (float)limits[j].end ||  fabs(optSch[i].start-(float)limits[j].end)<opt_const.PRECISION)&&(optSch[i].start>(float)limits[j].start || fabs(optSch[i].start- (float)limits[j].start)))
				{
					//				if(fabs(optSch[i].velocity-limits[j].limit) < NEARLIMIT || fabs(optSch[i].velocity-limits[j].limit-NEARLIMIT)<opt_const.PRECISION)
					//				{
					//					flag=1;
					//					break;
					//				}
					if((fabs(optSch[i].velocity - (float)limits[j].limit) < opt_const.NEARLIMIT || fabs(optSch[i].velocity- (float)limits[j].limit - opt_const.NEARLIMIT)<opt_const.PRECISION) && (fabs(optSch[i].velocity-opt_const.LIMITV) < opt_const.NEARLIMIT || fabs(optSch[i].velocity-opt_const.LIMITV- opt_const.NEARLIMIT)<opt_const.PRECISION) )
					{
						flag=1;
						break;
					}
				}
			}
		}
	}
	else
	{
		for(i=sIndex;i<=eIndex;i++)
		{
			if((optSch[i].start>tmpStart || fabs(optSch[i].start-tmpStart)<opt_const.PRECISION) && (optSch[i].start<tmpEnd || fabs(optSch[i].start-tmpEnd)<opt_const.PRECISION))
			{
                    if(fabs(optSch[i].velocity-tmpValue)<opt_const.NEARLIMIT)
                    {
                    	flag=1;
                    	break;
                    }
			}
		}
	}
	return flag;
}
/**
 * @brief 档位连续渐变处理函数
 * @param new_final_curve  指向最终的优化曲线的曲线
 * @param srcGear  变换前档位
 * @param destGear  变换后档位
 * @param cap  new_final_curve的容量
 * @param n   当前的最终的优化曲线点的个数-1（处理后需要返回）,相当于new_final_curve最后一个点的index
 * @param dist   当前优化曲线的公里标位置（处理后需要返回）,相当于当前需要gear_transfer的起始位置，调用函数之后，返回为处理后的终止位置
 * @param  velocity  当前优化曲线的速度（处理后需要返回），同dist类似
 * @return  异常结果
 */
OPTIMIZE_RESULT gear_transfer(OPTSCH** new_final_curve,int srcGear,int destGear,int *cap,int *n,float* dist,float* velocity)
{
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;
	char  desc[EXP_DESC_LENGTH]="#gear_transfer";
	float delta_s,delta_v,delta_e;
	float headX,v;
	int num,capacity,count,j;
	count=1;
	headX=*dist;
	v=*velocity;
	capacity=*cap;
	num=*n;
	if(srcGear-1>destGear) //档位递减
	{
		for(j=srcGear-1;j>destGear;j--)
		{
			int gearCount=1;
			int maxCount=(int)(opt_const.COMGEARSTEP/opt_const.TSTEP); //档位不等于0的点个数
			if(j==0)
			{
				maxCount=(int)(opt_const.IDELGEARSTEP/opt_const.TSTEP);//档位等于0的点个数
			}
			while(gearCount<=maxCount)
			{
				trac_ret=DoCaculateByTime(headX, v, j, opt_const.TSTEP, &count, &delta_s, &delta_v, &delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				headX = headX + delta_s;
				v = v + delta_v;
				num=num+1;
				if(num+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
				{
					capacity=capacity+opt_const.POSTINITLENGTH/2;
					(*new_final_curve)=(OPTSCH*)realloc((*new_final_curve),sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
					if((*new_final_curve)==NULL)
					{
						optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
						return ret;
					}
				}
				(*new_final_curve)[num].start = headX;
				(*new_final_curve)[num].gear = j;
				(*new_final_curve)[num].velocity =v;
				(*new_final_curve)[num].consumption = delta_e;
				(*new_final_curve)[num].time = opt_const.TSTEP;
				(*new_final_curve)[num].acc = delta_v / opt_const.TSTEP;
				gearCount=gearCount+1;
			}
		}
	}
	else
	{
		if(srcGear+1<destGear) //档位递增
		{
			for(j=srcGear+1;j<destGear;j++)
			{
				int gearCount=1;
				int maxCount=(int)(opt_const.COMGEARSTEP/opt_const.TSTEP); //档位不等于0的点个数
				if(j==0)
				{
					maxCount=(int)(opt_const.IDELGEARSTEP/opt_const.TSTEP);//档位等于0的点个数
				}
				while(gearCount<=maxCount)
				{
					trac_ret=DoCaculateByTime(headX, v, j, opt_const.TSTEP, &count, &delta_s, &delta_v, &delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					headX = headX + delta_s;
					v = v + delta_v;
					num=num+1;
					if(num+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
					{
						capacity=capacity+opt_const.POSTINITLENGTH/2;
						(*new_final_curve)=(OPTSCH*)realloc((*new_final_curve),sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
						if((*new_final_curve)==NULL)
						{
							optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
							return ret;
						}
					}
					(*new_final_curve)[num].start = headX;
					(*new_final_curve)[num].gear = j;
					(*new_final_curve)[num].velocity = v;
					(*new_final_curve)[num].consumption = delta_e;
					(*new_final_curve)[num].time = opt_const.TSTEP;
					(*new_final_curve)[num].acc = delta_v / opt_const.TSTEP;
					gearCount=gearCount+1;
				}
			}
		}
	}
	*dist=headX;
	*velocity=v;
	*cap=capacity;
	*n=num;
	return ret;
}
/**
 * @brief 根据给定的速度和力，通过牵引力和速度对应的表查询到相对应的档位，求出的力对应在g1,和g2对应的力之间，g1小于g2
 * @param force  给定的牵引力或者制动力
 * @param velocity  当前的速度
 * @param g1  较低的档位
 * @param g2  较高的档位
 * @param delF1 较低的档位对应的力同force的差值（绝对值）
 * @param delF2 较高的档位对应的力同force的差值（绝对值）
 * @return  若对应的力不再两个档位之间，则返回单个档位，若在两个档位之间则返回MOREGEAR的标识
 */
int getGearByFV(float force,int velocity,int *g1,int *g2,float *delF1,float* delF2)
{
	int gear,j;
	if(force>0) //牵引力
	{
		if(fabs(force)<dragForce[0][119]) //判断是否小于最小的力，如果小于则为惰行档位
			gear=0;
		else
		{
			for (j = 0 ; j < 7 ;j++){
				if(j==6 &&  fabs(force)>dragForce[j+1][velocity] )
				{
					gear=j+1+1;//索引从0开始，因此档位需要+1
					break;
				}
				if(fabs(force)<dragForce[j+1][velocity] && fabs(force)>dragForce[j][velocity])
				{
					*g1=j+1;
					*delF1=fabs( fabs(force)-dragForce[j][velocity]);
					*g2=j+1+1;
					*delF2=fabs(dragForce[j+1][velocity]-fabs(force)) ;
					gear=MOREGEAR;
					break;
				}
				if(j==0 && fabs(force)<dragForce[j][velocity])
				{
					*g1=0;
					*delF1=fabs(force);
					*g2=j+1;
					*delF2=abs(dragForce[j][velocity]-fabs(force));
					gear=MOREGEAR;
					break;
				}
			}
		}
	}
	else
	{
		if(fabs(force)<brakeForce[0][119])
			gear=0;
		else
		{
			for (j = 0 ; j < 7 ;j++){
				if(j==6 && fabs(force)>brakeForce[j+1][velocity])
				{
					gear=-(j+1+1);
					break;
				}
				if(fabs(force)<brakeForce[j+1][velocity] && fabs(force)>brakeForce[j][velocity])
				{
					*g1=-(j+1);
					*delF1=fabs( fabs(force)-dragForce[j][velocity]);
					*g2=-(j+1+1);
					*delF2=fabs(dragForce[j+1][velocity]-fabs(force)) ;
					gear=MOREGEAR;
					break;
				}
				if(j==0 && fabs(force)<dragForce[j][velocity])
				{
					*g1=0;
					*delF1=fabs(force);
					*g2=j+1;
					*delF2=abs(dragForce[j][velocity]-fabs(force));
					gear=MOREGEAR;
					break;
				}
			}
		}
	}
	if(gear<opt_const.MINGEAR)
		printf("gear error");
	return gear;
}
/**
 * @brief 根据某个档位进行牵引
 * @param new_final_curve  新计算的优化速度曲线
 * @param gear  牵引的档位（可能为正档位也可能为负档位）
 * @param endS 牵引的结束位置
 * @param cap  new_final_curve的容量,存在扩容操作
 * @param n    new_final_curve的实际的点的个数（处理后需要返回）
 * @param dist   当前优化曲线的公里标位置（处理后需要返回）
 * @param  velocity  当前优化曲线的速度（处理后需要返回）
 * @return 异常结果
 */
OPTIMIZE_RESULT dragByGear(OPTSCH** new_final_curve,int gear,float endS, int * cap, int* n,float* dist,float* velocity)
{
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;
	char  desc[EXP_DESC_LENGTH]="#dragByGear";
	float headX,delta_s,delta_v,delta_e,v;
	int count,capacity,num;
	headX=*dist;
	v=*velocity;
	count=0;
	capacity=*cap;
	num=*n;
	int overFlag=0;//是否超出限速
	int srcGear;
	while (headX <endS || fabs(headX-endS)<opt_const.PRECISION) {
		trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,
				&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		headX = headX + delta_s;
		v = v + delta_v;
		if(v>opt_const.LIMITV-1.5)
		{
			overFlag=1;
			srcGear=gear;
			headX=headX-delta_s;
			v=v-delta_v;
			opt_log_info("over velocity,需重新拉平: %f",v);
			break;
		}
		if(v<0)
		{
			opt_log_info("below zero velocity");
			break;
		}
		//记录
		num=num+1;
		if(num+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
		{
			capacity=capacity+opt_const.POSTINITLENGTH/2;
			(*new_final_curve)=(OPTSCH*)realloc((*new_final_curve),sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
			if((*new_final_curve)==NULL)
			{
				optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
				return ret;
			}
		}
		(*new_final_curve)[num].start = headX;
		(*new_final_curve)[num].gear = gear;
		(*new_final_curve)[num].velocity = v;
		(*new_final_curve)[num].consumption = delta_e;
		(*new_final_curve)[num].time = opt_const.TSTEP;
		(*new_final_curve)[num].acc = delta_v / opt_const.TSTEP;
	}

	if(overFlag==1)
	{
		gear=gear-1;
		while(1)
		{
			trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,
					&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			if(delta_v<0)
				break;
			gear=gear-1;
			if(gear<opt_const.MINGEAR)
			{
				gear=opt_const.MINGEAR;
				break;
			}
		}
		gear_transfer(new_final_curve,srcGear,gear,&capacity,&num,&headX, &v);
		if(ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,ret.description);
			optimize_exception_decorator(ret.opt_rest,&ret,desc);
			return ret;
		}
		while (headX <endS || fabs(headX-endS)<opt_const.PRECISION) {
			trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,
					&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			headX = headX + delta_s;
			v = v + delta_v;
			if(v>opt_const.LIMITV)
			{
				opt_log_info("over velocity: %f",v);
			}
			if(v<0)
			{
				opt_log_info("below zero velocity");
				break;
			}
			//记录
			num=num+1;
			if(num+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
			{
				capacity=capacity+opt_const.POSTINITLENGTH/2;
				(*new_final_curve)=(OPTSCH*)realloc((*new_final_curve),sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
				if((*new_final_curve)==NULL)
				{
					optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
					return ret;
				}
			}
			(*new_final_curve)[num].start = headX;
			(*new_final_curve)[num].gear = gear;
			(*new_final_curve)[num].velocity = v;
			(*new_final_curve)[num].consumption = delta_e;
			(*new_final_curve)[num].time = opt_const.TSTEP;
			(*new_final_curve)[num].acc = delta_v / opt_const.TSTEP;
		}
	}
	*cap=capacity;
	*dist=headX;
	*velocity=v;
	*n=num;
	return ret;
}
/**
 * @brief 从tmpSegGear结构中获得从startIndex位置到endIndex位置的距离
 * @param tmpSegGears 保存档位长度小于给定长度的档位结构
 * @param optSch  原始优化曲线,需要通过其获得公里标记
 * @param startIndex tmpSegGears中的startIndex
 * @param endIndex tmpSegGears中的endIndex
 * @return 两个index对应的档位之间的距离
 */
float getPostFromTmpSeg(GEARSEGINFO*  tmpSegGears,OPTSCH* optSch,int startIndex,int endIndex)
{
	float totalPost=0;
	int i;
	for(i=startIndex;i<=endIndex;i++)
	{
		totalPost=totalPost+(optSch[tmpSegGears[i].endIndex].start-optSch[tmpSegGears[i].startIndex].start);
	}
	return totalPost;
}
