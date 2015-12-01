/**
 * @file     avgV_opt_imp.c
 * @brief   机车保持匀速行驶的相关操作实现
 * @date  2014.7.23
 * @author sinlly
 */
#include "base_opt_fuc.h"
/**
 *@brief   牵引机车使其保持某个平均速度上下浮动，默认先往下浮动
 *@param   startGear  初始档位
 *@param   avgV  需要保持的平均速度
 *@param   v_internal  保持平均速度上下浮动的阈值
 *@param   rcIndex   所在段的index
 *@param   localIndex   当前优化的初始index
 *@param   localCount  用于查找当前位置属于某个段的起始roadIndex
 *@param   optSch   优化结果曲线
 *@return 优化异常结果
 */
OPTIMIZE_RESULT constv_trac_til_ed(int startGear,float avgV, float  v_internal,int rcIndex, int* localIndex, int* localCount,OPTSCH** optSch)
{
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t,s;
	int gear,tempGear;
	int index=*localIndex;
	int count=*localCount;
	int tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
	headX = (*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;

	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#constv_trac_til_ed";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	tempGear=startGear;
	while (headX < (float)rc[rcIndex].end || fabs(headX - (float)rc[rcIndex].end)<opt_const.PRECISION) {
		gear=tempGear;
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
		if(v<avgV-v_internal)
		{
			gear=opt_const.MINGEAR;
			while(gear<opt_const.MAXGEAR)
			{
				trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,&delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				if(delta_v>0.0)
				{
					tempGear=gear;
					break;
				}
				gear=gear+1;
			}
			if(gear>=opt_const.MAXGEAR)
				tempGear=opt_const.MAXGEAR;
		}
		if(v>avgV+v_internal)
		{
			gear=opt_const.MAXGEAR;
			while(gear>opt_const.MINGEAR)
			{
				trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,&delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					return trac_result_adapter(trac_ret);
				}
				if(delta_v<0)
				{
					tempGear=gear;
					break;
				}
				gear=gear-1;
			}
			if(gear<=opt_const.MINGEAR)
				tempGear=opt_const.MINGEAR;
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
		(*optSch)[index].acc = delta_v /opt_const. TSTEP;

		index = index + 1;
	}

	*localIndex=index;
	*localCount=count;
	//		return optSch;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 当速度低于平均速度-阈值时，先后交替up和down档位运行，直到速度高于平均速度+阈值
 * @param gearUp  向上牵引档位
 * @param gearDown  向下制动档位
 * @param localIndex  当前优化的初始index
 * @param localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  flag  标志位，表示需要牵引或者制动（1：表示制动，-1表示牵引）
 * @param  v_internal  匀速允许的上下起伏的阈值
 * @param  avgV 需要保持的平均速度
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT constv_gr_down_up_til_end(float start,float startV,  int gearup, int geardown,int rcIndex,int *localIndex,int* localCount,  int* flag,float  v_internal, float avgV, OPTSCH** optSch)
{
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t,s;
	int gear;
	int index=*localIndex;
	int count=*localCount;

	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#constv_gr_down_up_til_end";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	v=startV;
	headX=start;
	while (headX < (float)rc[rcIndex].end) {
		if (*flag == 1) {		//使用推荐档位的低档位牵引
			gear = geardown;
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

			if (v < avgV - v_internal || fabs(v -(avgV - v_internal))<opt_const.PRECISION) {
				*flag = -1;
				gear = opt_const.MINGEAR;
				while (gear <=opt_const. MAXGEAR - 1) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count,
							&delta_s, &delta_v, &delta_e);
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
			}
			if ((v > avgV + v_internal || fabs(v -( avgV + v_internal))<opt_const.PRECISION) && delta_v > 0.0) {
				*flag = 1;
				gear = opt_const.MAXGEAR;
				while (gear >= opt_const.MINGEAR + 1) {
					trac_ret=DoCaculateByTime(headX, v, gear,opt_const. TSTEP, &count,
							&delta_s, &delta_v, &delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v <= 0.0)
						break;
					gear = gear - 1;
				}
				geardown = gear;
			}
		} else { //flag == -1,使用推荐档位的高挡位制动
			gear = gearup;
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
			if ((v < avgV - v_internal || fabs(v -( avgV - v_internal)) < opt_const.PRECISION) && delta_v < 0.0) {
				*flag = -1;
				gear = opt_const.MINGEAR;
				while (gear <= opt_const.MAXGEAR - 1) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count,
							&delta_s, &delta_v, &delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v >= 0)
						break;
					gear = gear + 1;
				}
				gearup = gear;
			}
			if (v > avgV + v_internal || fabs(v - ( avgV + v_internal))<opt_const.PRECISION) {
				*flag = 1;
				gear = opt_const.MAXGEAR;
				while (gear >= opt_const.MINGEAR + 1) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count,
							&delta_s, &delta_v, &delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v <= 0.0)
						break;
					gear = gear - 1;
				}
				geardown = gear;
			}
		}
	}
	*localIndex=index;
	*localCount=count;
	return raw_result_adapter(opt_ret);
}

/**
 * @brief 当速度低于平均速度-阈值时，先后交替up和down档位运行，直到速度高于平均速度+阈值；整个过程中不使用制动档位
 * @param gearUp  向上牵引档位
 * @param gearDown  向下制动档位
 * @param localIndex  当前优化的初始index
 * @param localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param  flag  标志位，表示需要牵引或者制动（1：表示制动，-1表示牵引）
 * @param  v_internal  匀速允许的上下起伏的阈值
 * @param  avgV 需要保持的平均速度
 * @param  optSch  优化结果曲线
 * @return 优化异常结果
 */
OPTIMIZE_RESULT constv_gr_down_up_til_end_no_db_gear(float start,float startV,  int gearup, int geardown,int rcIndex,int *localIndex,int* localCount,  int* flag,float  v_internal, float avgV, OPTSCH** optSch)
{
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t,s;
	int gear;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#constv_gr_down_up_til_end_no_db_gear";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	v=startV;
	headX=start;
	while (headX < (float)rc[rcIndex].end) {
		if (*flag == 1) {		//使用推荐档位的低档位牵引
			gear = geardown;
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

			if (v < avgV - v_internal || fabs(v -(avgV - v_internal))<opt_const.PRECISION) {
				*flag = -1;
				gear = 0;
				while (gear <= opt_const.MAXGEAR - 1) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count,
							&delta_s, &delta_v, &delta_e);
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
			}
			if ((v > avgV + v_internal || fabs(v -( avgV + v_internal))<opt_const.PRECISION) && delta_v > 0.0) {
				*flag = 1;
				gear = opt_const.MAXGEAR;
				while (gear > 0) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count,
							&delta_s, &delta_v, &delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v <= 0.0)
						break;
					gear = gear - 1;
				}
				geardown = gear;
			}
		} else { //flag == -1,使用推荐档位的高挡位制动
			gear = gearup;
			trac_ret=DoCaculateByTime(headX, v, gear,opt_const. TSTEP, &count, &delta_s,
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
			(*optSch)[index].time =opt_const.TSTEP;
			(*optSch)[index].acc = delta_v / opt_const.TSTEP;
			index = index + 1;
			if ((v < avgV - v_internal || fabs(v -( avgV - v_internal)) <opt_const. PRECISION) && delta_v < 0.0) {
				*flag = -1;
				gear = 0;
				while (gear <= opt_const.MAXGEAR - 1) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count,
							&delta_s, &delta_v, &delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v >= 0)
						break;
					gear = gear + 1;
				}
				gearup = gear;
			}
			if (v > avgV + v_internal || fabs(v - ( avgV + v_internal))<opt_const.PRECISION) {
				*flag = 1;
				gear = opt_const.MAXGEAR;
				while (gear >0 ) {
					trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count,
							&delta_s, &delta_v, &delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						return trac_result_adapter(trac_ret);
					}
					if (delta_v <= 0.0)
						break;
					gear = gear - 1;
				}
				geardown = gear;
			}
		}
	}
	*localIndex=index;
	*localCount=count;
	return  raw_result_adapter(opt_ret);
}
/**
 *  @brief 根据速度与平均速度的差别获得向下制动（速度递减）和向上牵引（速度递增）的档位
 *  @param  start  初始位置/结束后为最终的位置
 *  @param  startV 初始速度/结束后为最终的速度
 *  @param  avgV  平均速度
 *  @param  localCount   用于查找当前位置属于某个段的起始roadIndex
 *  @param  flag  标志位，表示需要牵引或者制动（1：表示制动，-1表示牵引）
 *  @param  gearup  向上牵引的档位
 *  @param  geardown 向下制动的档位
 *  @return 优化异常结果
 */
OPTIMIZE_RESULT get_avg_up_down_gear(float* start,float* startV, float avgV,int *localCount,int *flag,int* gearup,int* geardown)
{
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t,s;
	int gear;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#get_avg_up_down_gear";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	int count=*localCount;
	headX=*start;
	v=*startV;
	if(v<avgV)
	{
		*flag = -1;
		gear = opt_const.MINGEAR;
		while (gear <= opt_const.MAXGEAR - 1) {
			trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
					&delta_v, &delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			if (delta_v >= 0)
				break;
			gear = gear + 1;
		}
		*gearup = gear;
	}
	else
	{
		* flag = 1;
		gear =opt_const. MAXGEAR;
		while (gear >= opt_const.MINGEAR + 1) {
			trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
					&delta_v, &delta_e);
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
		*geardown = gear;
	}
	*localCount=count;
	*start=headX;
	*startV=v;
	return raw_result_adapter(opt_ret);
}
/**
 *  @brief 根据速度与平均速度的差别获得向下制动（速度递减）和向上牵引（速度递增）的档位；整个过程中不使用制动
 *  @param  start  初始位置/结束后为最终的位置
 *  @param  startV 初始速度/结束后为最终的速度
 *  @param  avgV  平均速度
 *  @param  localCount   用于查找当前位置属于某个段的起始roadIndex
 *  @param  flag  标志位，表示需要牵引或者制动（1：表示制动，-1表示牵引）
 *  @param  gearup  向上牵引的档位
 *  @param  geardown 向下制动的档位
 *  @return 优化异常结果
 */
OPTIMIZE_RESULT get_avg_up_down_gear_no_db_gear(float* start,float* startV, float avgV,int *localCount,int *flag,int* gearup,int* geardown)
{
	float headX;
	float v;
	float delta_s, delta_v, delta_e;	//,delta_t,s;
	int gear;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#get_avg_up_down_gear_no_db_gear";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	int count=*localCount;
	headX=*start;
	v=*startV;
	if(v<avgV)
	{
		*flag = -1;
		gear = 0;
		while (gear <= opt_const.MAXGEAR - 1) {
			trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
					&delta_v, &delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			if (delta_v >= 0)
				break;
			gear = gear + 1;
		}
		*gearup = gear;
	}
	else
	{
		* flag = 1;
		gear = opt_const.MAXGEAR;
		while (gear > 0) {
			trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s,
					&delta_v, &delta_e);
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
		*geardown = gear;
	}
	*localCount=count;
	*start=headX;
	*startV=v;
	return raw_result_adapter(opt_ret);
}
