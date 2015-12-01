/**
 * @file     db_opt_imp.c
 * @brief   机车制动行驶相关操作实现
 * @date  2014.7.23
 * @author sinlly
 */
#include "base_opt_fuc.h"
/**
 * @brief 以某个档位制动直到到达某个限速或者已经行驶到段末
 * @param  startGear  初始档位
 * @param  rcIndex  所在段的index
 * @param  localIndex  当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  limit  某一限速（可能为最大限速也可能为最小限速）
 * @param  flag  表示速度是到达了某个limit或者是行驶到了段末(0表示到段末，1表示到达某个limit)
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT   db_til_lmt_or_ed(int startGear, int rcIndex, int * localIndex,int* localCount, float limit, int* flag, OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t;
	int gear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#db_til_lmt_or_ed";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	gear=startGear;
	while (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION) { //计算最大制动走完该段速度曲线或最大制动至期望输出速度时
		trac_ret=DoCaculateByTime(headX, v, gear,opt_const. TSTEP, &count, &delta_s, &delta_v,
				&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		headX = headX + delta_s;
		v = v + delta_v;
		//记录
		(*optSch) = (OPTSCH*) realloc((*optSch), (index + 1) * sizeof(OPTSCH));
		if(*optSch==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
			return raw_result_adapter(opt_ret);
		}
		(*optSch)[index].start = headX;
		(*optSch)[index].gear = gear;
		(*optSch)[index].velocity = v;
		(*optSch)[index].consumption = delta_e;
		(*optSch)[index].time = opt_const.TSTEP;
		(*optSch)[index].acc = delta_v / opt_const.TSTEP;
		index = index + 1;

		//break;
		if (v > limit || fabs(v - limit)<opt_const.PRECISION ) {
			*flag = 0;
			break;
		}
		if (v < 10.0 || fabs(v - 10.0)<opt_const.PRECISION) { //这里为什么设置成10呢？
			break;
		}
	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 以动态的gear（保证曲线的速度一直递增的档位）牵引直到速度到达了vo或者超过了某一个限速或者已经行驶到段末
 * @param  startGear  初始档位
 * @param  rcIndex  所在段的index
 * @param  vo  段末的vo
 * @param  localIndex  当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  limit 某一限速（可能为最大限速也可能为最小限速)
 * @param  flag 表示速度是到达了vo或者是到达了Limit或者是行驶到了段末
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT   db_dyn_gr_til_vo_or_ed_or_lmt(int startGear,float vo, int rcIndex , int* localIndex, int* localCount, float limit, int* flag,OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t;
	int gear,testGear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#db_dyn_gr_til_vo_or_ed_or_lmt";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	gear = startGear;

	while (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION) {
		testGear = 0;
		trac_ret=DoCaculateByTime(headX, v, testGear, opt_const.TSTEP, &count, &delta_s,
				&delta_v, &delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		if (delta_v < 0) {
			gear = 0;
		} else {
			trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
					&delta_v, &delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
		}
		if (delta_v > 0) {
			gear = opt_const.MAXGEAR + 1;
			while (gear > 0) {
				gear = gear - 1;
				trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
						&delta_v, &delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				if (delta_v < 0)
					break;
			}
		}

		headX = headX + delta_s;
		v = v + delta_v;
		if (v > vo || fabs(v-vo)< opt_const.PRECISION) {
			*flag = 1;
			break;
		}
		if (v >  limit  || fabs(v-limit)<opt_const.PRECISION) {
			*flag = 2;
			break;
		}
		//记录
		(*optSch) = (OPTSCH*) realloc((*optSch), (index + 1) * sizeof(OPTSCH));
		if(*optSch==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
			return raw_result_adapter(opt_ret);
		}
		(*optSch)[index].start = headX;
		(*optSch)[index].gear = gear;
		(*optSch)[index].velocity = v;
		(*optSch)[index].consumption = delta_e;
		(*optSch)[index].time = opt_const.TSTEP;
		(*optSch)[index].acc = delta_v / opt_const.TSTEP;
		//break
		index=index+1;
	}
	*localIndex=index-1;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 以某个档位制动直到行驶到段末
 * @param  startGear  初始档位
 * @param  rcIndex  所在段的index
 * @param  localIndex  当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  db_gr_til_ed(int startGear, int rcIndex, int* localIndex,int* localCount, OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e; 	//,delta_t;
	int gear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#db_gr_til_ed";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	gear = startGear;
	while (headX < (float) rc[rcIndex].end || fabs(headX - (float) rc[rcIndex].end)<opt_const.PRECISION)  { //计算惰行走完该段速度曲线或惰行至期望输出速度时
		trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
				&delta_v, &delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		headX = headX + delta_s;
		v = v + delta_v;
		//记录
		(*optSch) = (OPTSCH*) realloc((*optSch),
				(index + 1) * sizeof(OPTSCH));
		if(*optSch==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
			return raw_result_adapter(opt_ret);
		}
		(*optSch)[index].start = headX;
		(*optSch)[index].gear = gear;
		(*optSch)[index].velocity = v;
		(*optSch)[index].consumption = delta_e;
		(*optSch)[index].time = opt_const.TSTEP;
		(*optSch)[index].acc = delta_v / opt_const.TSTEP;
		index = index + 1;
	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}
