/**
 * @file    trac_opt_imp.c
 * @brief   机车牵引行驶相关操作实现
 * @date  2014.7.23
 * @author sinlly
 */
#include "base_opt_fuc.h"
/**
 * @brief  以动态的gear牵引（保证曲线的速度一直递增的档位）直到速度为Vi或到达段末或到达最大限速值
 * @param  startGear  初始档位
 * @param  rcIndex   所在段的index
 * @param limitV  最大限速值
 * @param  localIndex  当前优化的初始index
 * @param   flag  表示是结束还是v已经到达vi
 * @param   localCount  用于查找当前位置属于某个段的起始roadIndex
 * @param   optSch  优化结果曲线
 * @return  优化异常结果
 */
OPTIMIZE_RESULT dyn_gr_trac_til_vi_or_ed_or_lmt(int startGear, int rcIndex , float limitV,int* localIndex, int* flag,int* localCount,OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t;
	int gear,tempGear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#dyn_gr_trac_til_vi_or_ed_or_lmt";
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

	while (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION) { //计算车头走完该段速度曲线

		tempGear = gear;
		trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,
				&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		if (delta_v >= 0.0) {
			gear = tempGear;
		} else {
			gear = 1;
			while (gear <= opt_const.MAXGEAR) {
				trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
						&delta_v, &delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				if (delta_v >= 0.0)
					break;
				gear = gear + 1;
			}
			if (gear > opt_const.MAXGEAR) {
				gear =opt_const. MAXGEAR;
			}
		}
		headX = headX + delta_s;
		v = v + delta_v;
		if (v > rc[rcIndex].vi || fabs(v - rc[rcIndex].vi)<opt_const.PRECISION)
		{
			*flag=0;
			break;
		}
		if(v > limitV || fabs(v - limitV)<opt_const.PRECISION )
		{
			*flag=0;
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
		index = index + 1;

	}
	*localCount=count;
	*localIndex=index;
	return raw_result_adapter(opt_ret);
}
/**
 *@brief 以某个档位牵引直到速度大于limit或达到段末
 *@param  startGear   初始档位
 *@param   rcIndex  所在段的Index
 *@param   limitV  限速大小
 *@param   localIndex  当前优化的初始index
 *@param   flag  是否超出限速标识
 *@param   localCount  用于查找当前位置属于某个段的起始roadIndex
 *@param   optSch  优化结果曲线
 *@return 优化异常结果
 */
OPTIMIZE_RESULT gr_trac_til_ed_or_lmt(int startGear ,int rcIndex, float limitV, float minV, int* localIndex, int* flag,int* localCount, OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t;
	int gear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#gr_trac_til_ed_or_lmt";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempIndex = index - 1;
	if (index == 1)
		tempIndex = 1;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	gear=startGear;
	while (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION) {
		if(v<minV)
		{
			*flag=-1;
			break;
		}
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
		if ( (v > limitV || fabs(v-limitV)<opt_const.PRECISION)&& (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION ))
		{
			*flag = 0;
			break;
		}
	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 从起始位置通过某个档位牵引直到段末
 * @param startGear 起始的gear
 * @param   rcIndex  所在段的index
 * @param  localIndex 当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  optSch 优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  gr_trac_til_ed(int startGear, int rcIndex, int* localIndex,int* localCount, OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t;
	int gear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#gr_trac_til_ed";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempIndex = index - 1;
	if (index == 1)
		tempIndex = 1;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	gear = startGear;
	while (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION) { //计算车头走完该段速度曲线
		trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,&delta_v, &delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		headX = headX + delta_s;
		v = v + delta_v;
		//记录
		(*optSch) = (OPTSCH*) realloc((*optSch),(index + 1) * sizeof(OPTSCH));
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
		(*optSch)[index].acc = delta_v /opt_const. TSTEP;
		index = index + 1;
	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 以动态的gear（保证曲线的速度一直递增的档位）牵引直到速度到达了vo或者超过了某一个限速或者已经行驶到段末
 * @param  startGear  初始档位
 * @param  vo  段末的vo
 * @param  rcIndex  所在段的index
 * @param  localIndex  当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  limit 某一限速（可能为最大限速也可能为最小限速)
 * @param  flag 表示速度是到达了vo或者是到达了Limit或者是行驶到了段末
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT   dyn_gr_trac_til_vo_or_ed_or_lmt(int startGear,float vo, int rcIndex , int* localIndex, int* localCount, float limit, int* flag,OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t;
	int gear,testGear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#dyn_gr_trac_til_vo_or_ed_or_lmt";
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
	while (headX < (float) rc[rcIndex].end || fabs(headX - (float) rc[rcIndex].end)<opt_const.PRECISION) {
		testGear = 0;
		trac_ret=DoCaculateByTime(headX, v, testGear, opt_const.TSTEP, &count, &delta_s,&delta_v, &delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		if (delta_v > 0.0) {
			gear = 0;
		} else {
			trac_ret=DoCaculateByTime(headX, v, gear,opt_const. TSTEP, &count, &delta_s,&delta_v, &delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
		}
		if (delta_v < 0.0) {
			while (gear < opt_const.MAXGEAR) {
				gear = gear + 1;
				trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,&delta_v, &delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				if (delta_v > 0.0)
					break;
			}
		}
		(*optSch) = (OPTSCH*) realloc((*optSch), (index + 1) * sizeof(OPTSCH));
		if(*optSch==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
			return raw_result_adapter(opt_ret);
		}
		headX = headX + delta_s;
		v = v + delta_v;
		//记录
		(*optSch)[index].start = headX;
		(*optSch)[index].gear = gear;
		(*optSch)[index].velocity = v;
		(*optSch)[index].consumption = delta_e;
		(*optSch)[index].time = opt_const.TSTEP;
		(*optSch)[index].acc = delta_v / opt_const.TSTEP;
		//break
		if (v > vo || fabs(v - vo)<opt_const. PRECISION) {
			*flag = 1;
			break;
		}
		if (v < limit) {
			*flag = 2;
			break;
		}
		index=index+1;

	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 通过某个档位牵引曲线至某个平均速度，然后保持该平均速度行驶到段末
 * @param gear  牵引到平均速度的档位
 * @param avgV  需要保持的平均速度
 * @param rcIndex  所在段的index
 * @param  localIndex 当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  flag   是否低于匀速
 * @param  gearup  低于匀速之后让其速度上升的档位
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  gr_trac_to_avgv_till_ed(int gear, float avgV, int rcIndex,float  v_internal,  int * localIndex,int* localCount,  int* flag, OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t;
	int gearup,geardown;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#gr_trac_to_avgv_till_ed";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	if(*flag==0)
	{
		while (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION) {
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
			while(v>opt_const.LIMITV && gear!=opt_const.MINGEAR)
			{
				gear=gear-1;
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
			(*optSch)[index].time =opt_const. TSTEP;
			(*optSch)[index].acc = delta_v /opt_const. TSTEP;
			index = index + 1;

			//break;
			if (v <  avgV - v_internal || fabs(v - ( avgV - v_internal))<opt_const.PRECISION) {
				*flag = -1;
				gear = opt_const.MINGEAR;
				while (gear <= opt_const.MAXGEAR - 1) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
							&delta_v, &delta_e); //源程序两个未赋值
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v >= 0.0)
						break;
					gear = gear + 1;
				}
				gearup = gear;
				break;
			}
		}
		if(*flag==-1)
		{
			OPTIMIZE_RESULT  ret;
			ret=constv_gr_down_up_til_end(headX, v,  gearup, geardown, rcIndex,&index,&count,  &(*flag), v_internal,  avgV, &(*optSch));
			if(ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,ret.description);
				optimize_exception_decorator(ret.opt_rest,&ret,desc);
				return ret;
			}
		}
	}
	else if(*flag==-1)
	{
		while (headX < (float) rc[rcIndex].end || fabs(headX - (float) rc[rcIndex].end)<opt_const.PRECISION) {
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
			(*optSch)[index].time =opt_const. TSTEP;
			(*optSch)[index].acc = delta_v /opt_const. TSTEP;
			index = index + 1;

			//break;
			if (v > avgV + v_internal || fabs(v -( avgV + v_internal))<opt_const.PRECISION) {
				*flag = 1;
				gear =opt_const. MAXGEAR;
				while (gear >= opt_const.MINGEAR + 1) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
							&delta_v, &delta_e); //源程序两个未赋值
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v <= 0)
						break;
					gear = gear - 1;
				}
				geardown = gear;
				break;
			}
		}
		if(*flag==1)
		{
			OPTIMIZE_RESULT  ret;
			ret=constv_gr_down_up_til_end(headX, v,  gearup, geardown, rcIndex,&index,&count,  &(*flag), v_internal,  avgV, &(*optSch));
			if(ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,ret.description);
				optimize_exception_decorator(ret.opt_rest,&ret,desc);
				return ret;
			}
		}
	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}

/**
 * @brief 通过某个档位牵引曲线至某个平均速度，然后保持该平均速度行驶到段末；整个过程中不是用制动档位
 * @param gear  牵引到平均速度的档位
 * @param avgV  需要保持的平均速度
 * @param rcIndex  所在段的index
 * @param  localIndex 当前优化的初始index
 * @param  localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  flag   是否低于匀速
 * @param  gearup  低于匀速之后让其速度上升的档位
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  gr_trac_to_avgv_till_ed_no_db_gear(int gear, float avgV, int rcIndex,float  v_internal,  int * localIndex,int* localCount,  int* flag, OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t;
	int gearup,geardown;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#gr_trac_to_avgv_till_ed_no_db_gear";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	if(*flag==0)
	{
		while (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION) {
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
			while(v>opt_const.LIMITV && gear!=0)
			{
				gear=gear-1;
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
			index = index + 1;

			//break;
			if (v <  avgV - v_internal || fabs(v - ( avgV - v_internal))<opt_const.PRECISION) {
				*flag = -1;
				gear = 0;
				while (gear <= opt_const.MAXGEAR - 1) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
							&delta_v, &delta_e); //源程序两个未赋值
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v >= 0.0)
						break;
					gear = gear + 1;
				}
				gearup = gear;
				break;
			}
		}
		if(*flag==-1)
		{
			OPTIMIZE_RESULT ret;
			ret=constv_gr_down_up_til_end_no_db_gear(headX, v,  gearup, geardown, rcIndex,&index,&count,  &(*flag), v_internal,  avgV, &(*optSch));
			if(ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,ret.description);
				optimize_exception_decorator(ret.opt_rest,&ret,desc);
				return ret;
			}
		}
	}
	else if(*flag==-1)
	{
		while (headX < (float) rc[rcIndex].end || fabs(headX - (float) rc[rcIndex].end)<opt_const.PRECISION) {
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
			(*optSch)[index].time =opt_const. TSTEP;
			(*optSch)[index].acc = delta_v / opt_const.TSTEP;
			index = index + 1;

			//break;
			if (v > avgV + v_internal || fabs(v -( avgV + v_internal))<opt_const.PRECISION) {
				*flag = 1;
				gear = opt_const.MAXGEAR;
				while (gear > 0 ) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
							&delta_v, &delta_e); //源程序两个未赋值
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v <= 0)
						break;
					gear = gear - 1;
				}
				geardown = gear;
				break;
			}
		}
		if(*flag==1)
		{
			OPTIMIZE_RESULT ret;
			ret=constv_gr_down_up_til_end_no_db_gear(headX, v,  gearup, geardown, rcIndex,&index,&count,  &(*flag), v_internal,  avgV, &(*optSch));
			if(ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,ret.description);
				optimize_exception_decorator(ret.opt_rest,&ret,desc);
				return ret;
			}
		}
	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}
/**
 * @note sup_steep专用的
 * @brief  以动态的gear牵引（保证曲线的速度一直递增的档位）直到速度为Vi或到达指定距离
 * @param  startGear  初始档位
 * @param  rcIndex   所在段的index
 * @param  dis       指定距离
 * @param  localIndex  当前优化的初始index
 * @param   flag  111标志 到段末且速度未异常101标识 走到指定距离且速度正常 100标志 为走到指定距离距离且速度达到限制(101与100其实可以合并)
 * @param   localCount  用于查找当前位置属于某个段的起始roadIndex
 * @param   optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT dyn_gr_trac_til_vi_or_ed_or_dis(int startGear,int rcIndex,float dis,float limitV,int* localIndex, int* flag,int* localCount,OPTSCH** optSch){
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e,delta_t;
	int gear,tempGear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#dyn_gr_trac_til_vi_or_ed_or_lmt";
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

	while (headX < dis || fabs(headX - dis)>opt_const.PRECISION) { //计算车头走完该段速度曲线

		tempGear = gear;
		trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,
				&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		if (delta_v >= 0.0) {
			gear = tempGear;
		} else {
			gear = 1;
			while (gear <= opt_const.MAXGEAR) {
				trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
						&delta_v, &delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				if (delta_v >= 0.0)
					break;
				gear = gear + 1;
			}
			if (gear > opt_const.MAXGEAR) {
				gear =opt_const. MAXGEAR;
			}
		}
		headX = headX + delta_s;
		v = v + delta_v;
		if (v > rc[rcIndex].vi || fabs(v - rc[rcIndex].vi)<opt_const.PRECISION)
		{
			*flag=100;
			break;
		}
		if(v > limitV || fabs(v - limitV)<opt_const.PRECISION )
		{
			*flag=100;
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
		index = index + 1;

	}
	*localCount=count;
	*localIndex=index;
	if(*flag==0){
		if(fabs((float) rc[rcIndex].end-dis)<opt_const.PRECISION)
				*flag=111;
		else
				*flag=101;
	}

	return raw_result_adapter(opt_ret);
}

OPTIMIZE_RESULT gr_trac_til_dis_or_lmt(int startGear ,int rcIndex,float dis, float limitV, float minV, int* localIndex, int* flag,int* localCount, OPTSCH** optSch)
{
	int tempIndex;
	float headX;
	float v;
	float delta_s, delta_v, delta_e,delta_t;
	int gear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#gr_trac_til_ed_or_lmt";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempIndex = index - 1;
	if (index == 1)
		tempIndex = 1;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	gear=startGear;
	while (headX < dis || fabs(headX - dis)<opt_const.PRECISION) {
		if(v<minV)
		{
			*flag=-1;
			break;
		}
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
		if ( (v > limitV || fabs(v-limitV)<opt_const.PRECISION)&& (headX < dis|| fabs(headX - dis)<opt_const.PRECISION ))
		{
			*flag = 0;
			break;
		}
	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}

