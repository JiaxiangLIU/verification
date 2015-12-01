/**
 *@file jump_gear_trasfer.c
 *@brief 临时限速最后同原始曲线交叉点档位连续变换策略，其中中间档位是从负档位跳跃到正档位或者从正档位跳跃到负档位
 * @date  Created on: 2015年3月10日
 * @author Author: root
 */
#include "tmp_gear_trasfer.h"
/**
 * @brief 档位连续变换策略，其中中间档位是从负档位跳跃到正档位或者从正档位跳跃到负档位
 * @param new_tmp_curve   临时限速调整片段曲线
 * @param tmp_final_len  临时限速调整片段曲线长度(当前最后一个点的index为tmp_fianl_len-1)
 * @param cap tmp_final_len的容量
 * @param optSch    进行临时限速调整之后的完整的曲线
 * @param srcInxT 临时限速与完整曲线衔接部分在完整曲线中的index
 * @param tStep  普通档位跳跃的时间长度
 * @param idelStep 惰行档位跳跃的时间长度
 * @param opt_ret 优化异常结果
 * @return  临时限速调整之后的经过档位连续变换后的完整曲线
 */
OPTSCH*  jump_even_gear_trasfer(OPTSCH* new_tmp_curve,int * tmp_final_len,int*cap, OPTSCH* optSch,int *srcInxT,float tStep,float idelStep,OPTIMIZE_RESULT* opt_ret)
{
	int num=*tmp_final_len-1;
	int sGear=new_tmp_curve[num].gear;
	int srcInx=*srcInxT;
	int eGear=optSch[srcInx].gear;
	int capacity=*cap;
	int midGear;
	int i;
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	char desc[EXP_DESC_LENGTH]="#jump_even_gear_trasfer";
	float delta_t=0;
	float headX,v,delta_v,delta_e,s;
	int gear;
	OPTSCH* optBack=NULL;
	int index_back=-1;
	int count=1;
	int flag=-3;
	int srcIndex;
	int gen=0;
	int index;
	if(eGear-sGear>0)  //档位是递增趋势
	{
		midGear=sGear+(eGear-sGear)/2;
		float needTime=0;
		for(i=sGear+1;i<=midGear;i++)
		{
			if(i==0) //如果是惰行档位
			{
				needTime=needTime+idelStep;
			}
			else  //如果不是惰行档位
			{
				needTime=needTime+tStep;
			}
		}
		//找到起始点
		delta_t=0;
		for(i=num;i>=0;i--)
		{
			delta_t=delta_t+new_tmp_curve[i].time;
			if(delta_t>needTime || fabs(delta_t-needTime)<opt_const.PRECISION)
			{
				break;
			}
		}
		/**
		 * flag: 0 表示有交点，1:表示档位变换之后追不上 , -1:表示提前追上 , -2为超出限速值, -3为默认初始值
		 */
		while(flag!=0)//递增的档位若没有焦点，则需要把档位往后移（index减小的方向）
		{
			if(flag==1) //追不上,集体往后移
			{
				gen=gen+1;//迭代次数（从0代开始）
				if(gen>MAXGEN)
					break;
				srcIndex=i+1-gen;
				index=i-gen;
			}
			else if(flag==-1) //提前追上，集体往前移动
			{
				gen=gen+1;
				if(gen>MAXGEN)
					break;
				srcIndex=i+1+gen;
				index=i+gen;
				if(index>num) //超出临时限速片段区域，则扩张临时限速曲线
				{
					headX=new_tmp_curve[num].start;
					v=new_tmp_curve[num].velocity;
					gear=new_tmp_curve[num].gear;
					trac_ret=DoCaculateByTime(headX,v,gear,opt_const.TSTEP,&count,&s,&delta_v,&delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						ret=trac_result_adapter(trac_ret);
						strcpy(opt_ret->description,ret.description);
						opt_ret->opt_rest=ret.opt_rest;
						return new_tmp_curve;
					}
					headX=headX+s;
					v=v+delta_v;
					if(index>capacity)//加入点的个数大于给定容量，则进行扩容操作
					{
						capacity=capacity+opt_const.POSTINITLENGTH/2;
						new_final_curve=(OPTSCH*)realloc(new_final_curve,sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
						if(new_final_curve==NULL)
						{
							optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
							strcpy(opt_ret->description,ret.description);
							opt_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
					}
					new_tmp_curve[index].start = headX;
					new_tmp_curve[index].gear = gear;
					new_tmp_curve[index].velocity = v;
					new_tmp_curve[index].consumption = delta_e;
					new_tmp_curve[index].time = opt_const.TSTEP;
					new_tmp_curve[index].acc = delta_v/opt_const.TSTEP;
					num=index;
					while(optSch[srcInx].start<new_tmp_curve[index].start)
						srcInx=srcInx+1;
				}
			}
			else if(flag==-3)
			{
				srcIndex=i+1;
				index=i;
			}
			headX=new_tmp_curve[index].start;
			v=new_tmp_curve[index].velocity;
			count=1;
			if(index_back!=-1)
			{
				free(optBack);
				optBack =NULL;
			}
			index_back=-1;
			flag=-3;//计算是否成功
			gear=sGear+1;
			int overFlag=0;
			for(;gear<=eGear;gear++)
			{
				float gearTime=0;
				while(1)
				{
					index = index + 1;
					if(index>num)
					{
						if(overFlag==0)
						{
							index=srcInx;
							overFlag=1;
						}
						s =optSch[index].start-headX;
					}
					else
						s=new_tmp_curve[index].start-headX;
					trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						ret=trac_result_adapter(trac_ret);
						strcpy(opt_ret->description,ret.description);
						opt_ret->opt_rest=ret.opt_rest;
						return new_final_curve;
					}
					if(index>num)
					{
						if(optSch[index].gear!=opt_const.AIRE&&optSch[index].gear!=opt_const.AIRS&&optSch[index].gear>eGear) //如果当前的gear大于原始的终止gear则将原始的终止gear重新赋值
							eGear=optSch[index].gear;
					}
					else
					{
						if(new_tmp_curve[index].gear!=opt_const.AIRE&&new_tmp_curve[index].gear!=opt_const.AIRS&&new_tmp_curve[index].gear>eGear) //如果当前的gear大于原始的终止gear则将原始的终止gear重新赋值
							eGear=new_tmp_curve[index].gear;
					}
					v = v+delta_v;
					if(v>opt_const.LIMITV || v<0)
					{
						flag=-2;
						break;
					}
					headX = headX + s;
					gearTime=gearTime+delta_t;
					index_back=index_back+1;
					if(index_back==0)
					{
						optBack = (OPTSCH*) malloc (sizeof(OPTSCH)*(index_back+1));
						if(optBack==NULL)
						{
							optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
							strcpy(opt_ret->description,ret.description);
							opt_ret->opt_rest=ret.opt_rest;
							return new_tmp_curve;
						}
					}
					else
					{
						optBack = (OPTSCH*) realloc (optBack,sizeof(OPTSCH)*(index_back+1));
						if(optBack==NULL)
						{
							optimize_exception_decorator(REALLOC_ERROR,&ret,desc);
							strcpy(opt_ret->description,ret.description);
							opt_ret->opt_rest=ret.opt_rest;
							return new_tmp_curve;
						}
					}
					optBack[index_back].start = headX;
					optBack[index_back].gear = gear;
					optBack[index_back].velocity = v;
					optBack[index_back].consumption = delta_e;
					optBack[index_back].time = delta_t;
					optBack[index_back].acc = delta_v/delta_t;
					if(gear==0)
					{
						if(gearTime>idelStep)
							break;
					}
					else
					{
						if(gearTime>tStep)
							break;
					}
				}
				if(flag==-2)//超出限速，直接退出
					break;
			}
			if(flag==-2)//超出限速，直接退出
				break;
			if(index>num)
			{
				//已经追上，档位为最后一个档位
				if( fabs(optSch[index].velocity-v)<opt_const.TMPSPEEDACCURACY || fabs(fabs(optSch[index].velocity-v)-opt_const.TMPSPEEDACCURACY)<opt_const.PRECISION)
					flag=0;
				else if(optSch[index].velocity<v-opt_const.TMPSPEEDACCURACY) //超出指定速度（相当于提前追上）
					flag=-1;
				else //没有追上
					flag=1;
			}
			else
			{
				//已经追上，档位为最后一个档位
				if( fabs(new_tmp_curve[index].velocity-v)<opt_const.TMPSPEEDACCURACY || fabs(fabs(new_tmp_curve[index].velocity-v)-opt_const.TMPSPEEDACCURACY)<opt_const.PRECISION)
					flag=0;
				else if(new_tmp_curve[index].velocity<v-opt_const.TMPSPEEDACCURACY) //超出指定速度（相当于提前追上）
					flag=-1;
				else //没有追上
					flag=1;
			}
		}
		if(flag==0)
		{
			if(index_back!=-1)
			{
				int ln=index_back;
				index_back=0;
				i=srcIndex;
				while (index_back<=ln){  //临时限速部分
					if(i+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
					{
						capacity=capacity+opt_const.POSTINITLENGTH/2;
						new_final_curve=(OPTSCH*)realloc(new_final_curve,sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
						if(new_final_curve==NULL)
						{
							optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
							strcpy(opt_ret->description,ret.description);
							opt_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
					}
					new_tmp_curve[i].start = optBack[index_back].start;
					new_tmp_curve[i].gear = optBack[index_back].gear;
					new_tmp_curve[i].velocity = optBack[index_back].velocity;
					new_tmp_curve[i].consumption = optBack[index_back].consumption;
					new_tmp_curve[i].time = optBack[index_back].time;
					new_tmp_curve[i].acc = optBack[index_back].acc;
					index_back=index_back+1;
					if(i>num)
						srcInx=srcInx+1;
					i=i+1;
				}
				num=i-1;
				free(optBack);
				optBack=NULL;
			}
		}
		else
		{
			printf("jump_even_gear_trasfer Error\n");
		}
	}
	else //档位是递减趋势
	{
		midGear=sGear+(eGear-sGear)/2;
		float needTime=0;
		for(i=sGear-1;i>=midGear;i--)
		{
			if(i==0)
			{
				needTime=needTime+idelStep;
			}
			else
			{
				needTime=needTime+tStep;
			}
		}
		//找到起始点
		delta_t=0;
		for(i=num;i>=0;i--)
		{
			delta_t=delta_t+new_tmp_curve[i].time;
			if(delta_t>needTime || fabs(delta_t-needTime)<opt_const.PRECISION)
			{
				break;
			}
		}
		/**
		 * flag: 0 表示有交点，1:表示档位变换之后追不上 , -1:表示提前追上 , -2为超出限速值, -3为默认初始值
		 */
		while(flag!=0) //档位是递减趋势,若没有焦点，则需要把档位往前移（index增大的方向）
		{
			if(flag==1) //追不上
			{
				gen=gen+1;//迭代次数（从0代开始）
				if(gen>MAXGEN)
					break;
				srcIndex=i+gen+1;
				index=i+gen;
				if(index>num) //超出临时限速片段区域，则扩张临时限速曲线
				{
					headX=new_tmp_curve[num].start;
					v=new_tmp_curve[num].velocity;
					gear=new_tmp_curve[num].gear;
					trac_ret=DoCaculateByTime(headX,v,gear,opt_const.TSTEP,&count,&s,&delta_v,&delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						ret=trac_result_adapter(trac_ret);
						strcpy(opt_ret->description,ret.description);
						opt_ret->opt_rest=ret.opt_rest;
						return new_tmp_curve;
					}
					headX=headX+s;
					v=v+delta_v;
					if(index>capacity)//加入点的个数大于给定容量，则进行扩容操作
					{
						capacity=capacity+opt_const.POSTINITLENGTH/2;
						new_final_curve=(OPTSCH*)realloc(new_final_curve,sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
						if(new_final_curve==NULL)
						{
							optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
							strcpy(opt_ret->description,ret.description);
							opt_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
					}
					new_tmp_curve[index].start = headX;
					new_tmp_curve[index].gear = gear;
					new_tmp_curve[index].velocity = v;
					new_tmp_curve[index].consumption = delta_e;
					new_tmp_curve[index].time = opt_const.TSTEP;
					new_tmp_curve[index].acc = delta_v/opt_const.TSTEP;
					num=index;
					while(optSch[srcInx].start<new_tmp_curve[index].start)
						srcInx=srcInx+1;
				}
			}
			if(flag==-1)// 提前追上
			{
				gen=gen+1;//迭代次数（从0代开始）
				if(gen>MAXGEN)
					break;
				srcIndex=i+1-gen;
				index=i-gen;
			}
			else if(flag==-3)
			{
				srcIndex=i+1;
				index=i;
			}
			headX=new_tmp_curve[index].start;
			v=new_tmp_curve[index].velocity;
			count=1;
			if(index_back!=-1)
			{
				free(optBack);
				optBack =NULL;
			}
			index_back=-1;
			flag=-3;//计算是否成功
			gear=sGear-1;
			int overFlag=0;
			for(;gear>eGear;gear--)
			{
				float gearTime=0;
				while(1)
				{
					index = index + 1;
					if(index>num)
					{
						if(overFlag==0)
						{
							index=srcInx;
							overFlag=1;
						}
						s =optSch[index].start-headX;
					}
					else
						s=new_tmp_curve[index].start-headX;
					trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						ret=trac_result_adapter(trac_ret);
						strcpy(opt_ret->description,ret.description);
						opt_ret->opt_rest=ret.opt_rest;
						return new_tmp_curve;
					}
					if(index>num)
					{
						if(optSch[index].gear!=opt_const.AIRE&& optSch[index].gear!=opt_const.AIRS&&optSch[index].gear<eGear) //如果当前的gear小于原始的终止gear则将原始的终止gear重新赋值
							eGear=optSch[index].gear;
					}
					else
					{
						if(new_tmp_curve[index].gear!=opt_const.AIRE&& new_tmp_curve[index].gear!=opt_const.AIRS&&new_tmp_curve[index].gear<eGear) //如果当前的gear小于原始的终止gear则将原始的终止gear重新赋值
							eGear=new_tmp_curve[index].gear;
					}
					v = v+delta_v;
					if(v>opt_const.LIMITV || v<0)
					{
						flag=-2;
						break;
					}
					headX = headX + s;
					gearTime=gearTime+delta_t;
					index_back=index_back+1;
					if(index_back==0)
					{
						optBack = (OPTSCH*) malloc (sizeof(OPTSCH)*(index_back+1));
						if(optBack==NULL)
						{
							optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
							strcpy(opt_ret->description,ret.description);
							opt_ret->opt_rest=ret.opt_rest;
							return new_tmp_curve;
						}
					}
					else
					{
						optBack = (OPTSCH*) realloc (optBack,sizeof(OPTSCH)*(index_back+1));
						if(optBack==NULL)
						{
							optimize_exception_decorator(REALLOC_ERROR,&ret,desc);
							strcpy(opt_ret->description,ret.description);
							opt_ret->opt_rest=ret.opt_rest;
							return new_tmp_curve;
						}
					}
					optBack[index_back].start = headX;
					optBack[index_back].gear = gear;
					optBack[index_back].velocity = v;
					optBack[index_back].consumption = delta_e;
					optBack[index_back].time = delta_t;
					optBack[index_back].acc = delta_v/delta_t;
					if(gear==0)
					{
						if(gear>idelStep)
							break;
					}
					else
					{
						if(gear>tStep)
							break;
					}
				}
				if(flag==-2)
					break;
			}
			if(flag==-2)
				break;
			if(index>num)
			{
				//已经追上，档位为最后一个档位
				if( fabs(optSch[index].velocity-v)<opt_const.TMPSPEEDACCURACY || fabs(fabs(optSch[index].velocity-v)-opt_const.TMPSPEEDACCURACY)<opt_const.PRECISION)
					flag=0;
				else if(optSch[index].velocity<v- opt_const.TMPSPEEDACCURACY) //超出指定速度（相当于提前追上）
					flag=-1;
				else //没有追上
					flag=1;
			}
			else
			{
				//已经追上，档位为最后一个档位
				if( fabs(new_tmp_curve[index].velocity-v)<opt_const.TMPSPEEDACCURACY || fabs(fabs(new_tmp_curve[index].velocity-v)-opt_const.TMPSPEEDACCURACY)<opt_const.PRECISION)
					flag=0;
				else if(new_tmp_curve[index].velocity<v-opt_const.TMPSPEEDACCURACY) //超出指定速度（相当于提前追上）
					flag=-1;
				else //没有追上
					flag=1;
			}
		}
		if(flag==0)
		{
			if(index_back!=-1)
			{
				int ln=index_back;
				index_back=0;
				i=srcIndex;
				while (index_back<=ln){  //临时限速部分
					if(i+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
					{
						capacity=capacity+opt_const.POSTINITLENGTH/2;
						new_final_curve=(OPTSCH*)realloc(new_final_curve,sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
						if(new_final_curve==NULL)
						{
							optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
							strcpy(opt_ret->description,ret.description);
							opt_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
					}
					new_tmp_curve[i].start = optBack[index_back].start;
					new_tmp_curve[i].gear = optBack[index_back].gear;
					new_tmp_curve[i].velocity = optBack[index_back].velocity;
					new_tmp_curve[i].consumption = optBack[index_back].consumption;
					new_tmp_curve[i].time = optBack[index_back].time;
					new_tmp_curve[i].acc = optBack[index_back].acc;
					index_back=index_back+1;
					if(i>num)
						srcInx=srcInx+1;
					i=i+1;
				}
				num=i-1;
				free(optBack);
				optBack=NULL;
			}
		}
		else
		{
			printf("jump_even_gear_trasfer Error\n");
		}
	}
	*tmp_final_len=num+1;
	strcpy(opt_ret->description,ret.description);
	opt_ret->opt_rest=ret.opt_rest;
	*cap=capacity;
	*srcInxT=srcInx;
	return new_tmp_curve;
}
