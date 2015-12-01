/**
 * @file     empty_steep_up_strgy.c
 * @brief  空车 陡上坡策略  坡段类型为flag=2
 * @date  2014.7.23
 * @author sinlly
 */
#include "opt_global_var.h"
#include "optimize_curve.h"
#include "rawStrategy.h"
#include "base_opt_fuc.h"

int optSchIndex;
/**
 * @brief  空车的陡上坡策略
 * @param vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @note  空车的时候，无需将速度牵引到限速。而只需要将速度牵引到该段的平均速度，保持平均速度到段末
 * @return  优化曲线结果
 */
OPTSCH* empty_steep_up_strgy(float vo, float v_interval, int count, int roadIndex,
		int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) {

	int categoryStartIndex;
	int index_back;
	int flag;
	float v;
	int gear;
	OPTSCH* optBack;
	//异常处理标识符
	char  tmp[EXP_DESC_LENGTH];
	sprintf(tmp,"%d",roadIndex);
	char desc[EXP_DESC_LENGTH]="#empty_steep_up_strgy;rcIndex=";
	strcat(desc,tmp);
	OPTIMIZE_RESULT fopt_ret;
	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;

	categoryStartIndex = index;
	//通过最大档位牵引到限速后保持匀速运行到段末

	gear=opt_const.MAXGEAR;
	//	constv_trac_til_ed(gear, LIMITV- v_interval,v_interval, roadIndex,  &index, &count, &optSch);
	if(optSch[index-1].velocity-rc[roadIndex].avgV>opt_const.EMPTYINTERVAL)
	{
		fopt_ret=constv_trac_til_ed(gear, rc[roadIndex].avgV-opt_const.EMPTYINTERVAL,v_interval, roadIndex,  &index, &count, &optSch);
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
	else if(optSch[index-1].velocity-rc[roadIndex].avgV<-opt_const.EMPTYINTERVAL)
	{
		fopt_ret=constv_trac_til_ed(gear, rc[roadIndex].avgV+opt_const.EMPTYINTERVAL,v_interval, roadIndex,  &index, &count, &optSch);
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
	else
	{
		fopt_ret=constv_trac_til_ed(gear, rc[roadIndex].avgV,v_interval, roadIndex,  &index, &count, &optSch);
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
	v=optSch[index-1].velocity;
	if(rc[roadIndex+1].flag!=-3 && roadIndex+1<MAX_ROADCATEGORY)
		vo=rc[roadIndex+1].avgV;
	//如设计终止速度低一定值，需惰行，反求交点
	if (v > vo + v_interval || fabs(v -( vo + v_interval))<opt_const.PRECISION) {
		gear=0;
		fopt_ret=seg_ed_rvrs_cal(vo, gear,roadIndex,categoryStartIndex, &index, & flag, & count,&optSch, &optBack, &index_back);
		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,fopt_ret.description);
			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
			strcpy(ret->description, fopt_ret.description);
			ret->opt_rest =fopt_ret.opt_rest;
			return optSch;
		}

		if (flag == -1) {
			//惰性无交点，则需要从起始位置开始惰行
			gear=0;
			index=categoryStartIndex;
			fopt_ret=gr_trac_til_ed(gear, roadIndex, &index,&count,&optSch);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
				return optSch;
			}
		} else {
			//两段连接起来
			fopt_ret=conct_two_seg(&optSch, &index, flag,&optBack, index_back);
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
	}
//	ret=&fopt_ret;
	strcpy(ret->description, fopt_ret.description);
	ret->opt_rest =fopt_ret.opt_rest;
	optSchIndex = index; //最后将本策略optSch所得长度赋值给optSchIndex
	return optSch;
}

