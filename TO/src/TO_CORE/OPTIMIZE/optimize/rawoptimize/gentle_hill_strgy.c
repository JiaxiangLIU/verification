/**
 * @file      gentle_hill_strgy.c
 * @brief  缓坡策略  破段类型标记为flag=-1/0/1
 * @date  2014.4.14
 * @author sinlly
 */
#include "opt_global_var.h"
#include "optimize_curve.h"
#include "rawStrategy.h"
#include "base_opt_fuc.h"
int optSchIndex;
/**
 * @brief  缓坡策略函数实现
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
OPTSCH* gentle_hill_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) {
	int tempIndex, gear, flag, gearup, geardown,index0,index_back;
	float headX, v,currentV;
	OPTSCH* optBack;
	//异常处理标识符
	char  tmp[EXP_DESC_LENGTH];
	sprintf(tmp,"%d",roadIndex);
	char desc[EXP_DESC_LENGTH]="#gentle_hill_strgy;rcIndex=";
	strcat(desc,tmp);
	OPTIMIZE_RESULT fopt_ret;
	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;

	//赋值
	tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
	headX = optSch[tempIndex].start;
	v = optSch[tempIndex].velocity;
	//原程序count=10取消
	if (v > avgV + 5.0 || fabs(v - (avgV + 5.0))<opt_const.PRECISION) { //起始速度较高，先惰行至均速附近，再按均速牵引
		flag = 0;
		gear = 0;
		fopt_ret=gr_trac_to_avgv_till_ed(gear,  avgV,  roadIndex, v_interval, &index,&count,  &flag, &optSch);
		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,fopt_ret.description);
			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
			strcpy(ret->description, fopt_ret.description);
			ret->opt_rest =fopt_ret.opt_rest;
			return optSch;
		}
	} else if (v < avgV - 5.0 || fabs(v -( avgV - 5.0))<opt_const.PRECISION) //起始速度较低，则牵引到匀速附近，再按匀速牵引
	{
		flag = -1;
		gear = opt_const.MAXGEAR;
		fopt_ret=gr_trac_to_avgv_till_ed(gear,  avgV,  roadIndex, v_interval, &index,&count,  &flag, &optSch);
		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,fopt_ret.description);
			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
			strcpy(ret->description, fopt_ret.description);
			ret->opt_rest =fopt_ret.opt_rest;
			return optSch;
		}
	} else { //起始速度在均速附近，按均速牵引至段末
		//设定最大上下浮速度区间
		fopt_ret=get_avg_up_down_gear(&headX,&v,  avgV,&count,&flag,&gearup,&geardown);
		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,fopt_ret.description);
			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
			strcpy(ret->description, fopt_ret.description);
			ret->opt_rest =fopt_ret.opt_rest;
			return optSch;
		}
		fopt_ret=constv_gr_down_up_til_end(headX, v,  gearup, geardown, roadIndex,&index,&count,  &flag, v_interval,  avgV, &optSch);
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


	//下一段是上坡，需要最大加速到下一段入口速度附近
	if (optSch[index - 1].start > (float) rc[roadIndex].end || fabs(optSch[index - 1].start - (float)rc[roadIndex].end)<opt_const.PRECISION) {
		optSchIndex = index; //最后将本策略optSch所得长度赋值给optSchIndex
//		ret=&fopt_ret;
		strcpy(ret->description, fopt_ret.description);
		ret->opt_rest =fopt_ret.opt_rest;
		return optSch;
	}
	currentV = optSch[index - 1].velocity;

	//	if ((roadIndex != MAX_ROADCATEGORY && rc[roadIndex].flag == 1)
	//				|| (rc[roadIndex].flag == 2 && currentV < rc[roadIndex + 1].vi)) {
	if ((roadIndex+1 != MAX_ROADCATEGORY && rc[roadIndex+1].flag == 2
			&& currentV < rc[roadIndex + 1].vi)) {
		gear = opt_const.MAXGEAR;
		//从下一段的起始速度进行以最大档位反求
		v = rc[roadIndex + 1].vi;
		seg_ed_rvrs_cal_til_mark_or_inter(v,gear,roadIndex,&index, & flag, &count,& optSch,&optBack, &index_back,rc[roadIndex+1].start);
		if(flag==-1) //若在段内没有交点，则最大档位牵引到段末
		{
			index0=index-index_back-1;
			index=index0;
			gear=opt_const.MAXGEAR;
			fopt_ret=gr_trac_til_ed(gear, roadIndex, &index,&count, &optSch);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
				return optSch;
			}

		}else {  //若在段内有交点，则两段连接在一起,三段连接起来
			fopt_ret=conct_two_seg(& optSch, & index, &flag, &optBack, index_back);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
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
	}     //下一段是下坡，需要减速到下一段入口速度附近
	else if (roadIndex+1 != (MAX_ROADCATEGORY) && rc[roadIndex+1].flag == -2
			&& currentV > rc[roadIndex + 1].vi) {
		//从下一段的起始速度进行以惰行进行反求
		gear = 0;
		v = rc[roadIndex + 1].vi;
		fopt_ret=seg_ed_rvrs_cal_til_mark_or_inter(v,gear,roadIndex,&index, &flag, &count,&optSch,&optBack, &index_back,rc[roadIndex+1].start);
		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,fopt_ret.description);
			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
			strcpy(ret->description, fopt_ret.description);
			ret->opt_rest =fopt_ret.opt_rest;
			return optSch;
		}
		if (flag!=-1) { //可以惰行, 三段连接起来
			fopt_ret=conct_two_seg(&optSch, &index, &flag, &optBack, index_back);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
				return optSch;
			}
			if(optBack != NULL)
			{
				free(optBack);
				optBack = NULL;
			}
		} else { //不可惰行，尝试制动
			gear = opt_const.MINGEAR;
			v = rc[roadIndex + 1].vi;
			fopt_ret=seg_ed_rvrs_cal_til_mark_or_inter(v,gear,roadIndex,&index, & flag, &count,& optSch,&optBack, &index_back,rc[roadIndex+1].start);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
				return optSch;
			}
			if(flag==-1) //若在段内没有交点，则最大档位牵引到段末
			{
				index0=index-index_back-1;
				index=index0;
				gear=opt_const.MINGEAR;
				fopt_ret=gr_trac_til_ed(gear, roadIndex, &index,&count, &optSch);
				if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,fopt_ret.description);
					optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//					ret=&fopt_ret;
					strcpy(ret->description, fopt_ret.description);
					ret->opt_rest =fopt_ret.opt_rest;
					return optSch;
				}
			}else {  //若在段内有交点，则两段连接在一起,三段连接起来
				fopt_ret=conct_two_seg(& optSch, & index, &flag, &optBack, index_back);
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
		}
	}
	optSchIndex = index; //最后将本策略optSch所得长度赋值给optSchIndex
//    ret=&fopt_ret;
	strcpy(ret->description, fopt_ret.description);
	ret->opt_rest =fopt_ret.opt_rest;
	return optSch;
}

