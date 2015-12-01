/**
 * @file      empty_steep_dwn_Strgy.c
 * @brief  空车 陡下坡策略函数实现, 陡下坡策略  坡段类型标记为flag=-2
 * @date  2014.7.23
 * @author sinlly
 */
#include "opt_global_var.h"
#include "optimize_curve.h"
#include "rawStrategy.h"
#include "base_opt_fuc.h"

int optSchIndex;
/**
 * @brief 空车 陡下坡策略函数实现
 *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* empty_steep_dwn_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) {
	int tempIndex, markIndex, index_back;
	float headX, v;	//s
	int gear, gearup, geardown;
	int flag;
	//异常处理标识符
	char  tmp[EXP_DESC_LENGTH];
	sprintf(tmp,"%d",roadIndex);
	char desc[EXP_DESC_LENGTH]="#empty_steep_dwn_strgy;rcIndex=";
	strcat(desc,tmp);
	OPTIMIZE_RESULT fopt_ret;
	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;

	OPTSCH* optBack;

	//赋值
	tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
	markIndex = tempIndex;
	headX = optSch[tempIndex].start;
	v = optSch[tempIndex].velocity;
	flag=0;
	//策略开始
	if (v > vo) { //用微减速状态至期望速度
		gear=0;
		fopt_ret=db_dyn_gr_til_vo_or_ed_or_lmt(gear, rc[roadIndex].avgV,roadIndex , &index, &count, opt_const.LIMITV, &flag,&optSch);
		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,fopt_ret.description);
			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
			strcpy(ret->description, fopt_ret.description);
			ret->opt_rest =fopt_ret.opt_rest;
			return optSch;
		}
	} else { //用微加速状态至期望速度
		gear=0;
		fopt_ret=dyn_gr_trac_til_vo_or_ed_or_lmt(gear, rc[roadIndex].avgV,roadIndex , &index, &count, opt_const.MINV, &flag,&optSch);
		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,fopt_ret.description);
			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
			strcpy(ret->description, fopt_ret.description);
			ret->opt_rest =fopt_ret.opt_rest;
			return optSch;
		}
	}

	v=optSch[index].velocity;
	headX=optSch[index].start;

	if (flag == 2) { //惰行速度会突破天际，从头开始计算
		index = markIndex;
		v = optSch[index].velocity;
		headX = optSch[index].start;
		index = index + 1;
	}
	if (flag == 1 || flag == 2) { //已到达输出速度且未走完全程
		float markX;
		int recordIndex;
		float markV;
		markX = headX; //标记当前车头位置
		recordIndex = index; //记录当前index
		markV = v;

		gear = opt_const.MINGEAR;
		int isOK=TRUE;//最大制动是否能够达到要求
		index=index+1;
		//重新计算剩余部分是否能够通过最大制动达到要求，计算最大制动走完该段速度曲线或最大制动至限速时
		fopt_ret=db_til_lmt_or_ed(gear, roadIndex,&index,&count,opt_const. LIMITV, &isOK, &optSch);
		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,fopt_ret.description);
			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
			strcpy(ret->description, fopt_ret.description);
			ret->opt_rest =fopt_ret.opt_rest;
			return optSch;
		}
		index = recordIndex;
		headX = markX;
		v = markV;
		if (isOK == FALSE) { //最大制动不能满足需求时，反求交点位置
			gear=opt_const.MINGEAR;
			fopt_ret=seg_ed_rvrs_cal_til_mark(vo,gear, roadIndex, &index, &count, &flag, &optSch, &optBack, &index_back,markX);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
				return optSch;
			}
			//判断速度大小关系，决定是拼三段还是直接开最大制动到结束
			if (optBack[index_back - 1].velocity
					> optSch[index-1].velocity || fabs(optBack[index_back - 1].velocity
							- optSch[index-1].velocity)<opt_const.PRECISION) {
				//直接开最大制动到结束

				gear=-2;
				fopt_ret=db_gr_til_ed(gear, roadIndex, & index,& count, &optSch);
				if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,fopt_ret.description);
					optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//					ret=&fopt_ret;
					strcpy(ret->description, fopt_ret.description);
					ret->opt_rest =fopt_ret.opt_rest;
					return optSch;
				}
			} else {
				//拼三段
				//开始按距离步长反求过程计算
				gear=opt_const.MINGEAR;
				fopt_ret=rvrs_cal_til_inctersect(gear, &index, &count, & index_back, & flag, roadIndex,&optSch,&optBack);
				if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,fopt_ret.description);
					optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//					ret=&fopt_ret;
					strcpy(ret->description, fopt_ret.description);
					ret->opt_rest =fopt_ret.opt_rest;
					return optSch;
				}
				//两段连接起来
				fopt_ret=conct_two_seg(&optSch, &index, flag, &optBack, index_back);
				if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,fopt_ret.description);
					optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//					ret=&fopt_ret;
					strcpy(ret->description, fopt_ret.description);
					ret->opt_rest =fopt_ret.opt_rest;
					return optSch;
				}
				if(optBack != NULL)
				{
					free(optBack);
					optBack = NULL;
				}
			}
		} else { //制动可以满足需求时，选择合适档位以限速上下运行
			//计算当前输出速度vo下合适档位
			fopt_ret=get_avg_up_down_gear(&headX,&v,  avgV,&count,&flag,&gearup,&geardown);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
				return optSch;
			}
			fopt_ret=constv_gr_down_up_til_end(headX,v,gearup, geardown, roadIndex, &index,&count,  & flag, v_interval,  avgV, &optSch);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
				return optSch;
			}
		}
	}
	optSchIndex = index; //最后将本策略optSch所得长度赋值给optSchIndex
//	ret=&fopt_ret;
	strcpy(ret->description, fopt_ret.description);
	ret->opt_rest =fopt_ret.opt_rest;
	return optSch;
}

