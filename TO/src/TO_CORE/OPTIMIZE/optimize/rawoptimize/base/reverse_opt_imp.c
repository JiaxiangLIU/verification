/**
 * @file    reverse_opt_imp.c
 * @brief   机车反求相关操作实现
 * @date  2014.7.23
 * @author sinlly
 */
#include "base_opt_fuc.h"
/**
 *@brief  从段末进行反求，直到同原曲线相交为止
 *@param   startV  反求的初始速度
 *@param   gear  进行反求所采用的档位
 *@param   rcIndex  所在段的index
 *@param   categoryStartIndex   段首所在optSch中的index
 *@param   localIndex  当前优化的初始index
 *@param   flag  标志位（0 表示与曲线有交点且交点位置在段内；1 表示与曲线在当前段内没有交点
 *@param   localCount   用于查找当前位置属于某个段的起始roadIndex
 *@param   optSch  优化结果曲线
 *@param   backOpt 反求的优化曲线片段
 *@param   backLength 反求的优化曲线片段长度
 *@return 优化异常结果
 */
OPTIMIZE_RESULT  seg_ed_rvrs_cal(float startV,int gear, int rcIndex, int categoryStartIndex,int* localIndex, int* flag, int* localCount,OPTSCH** optSch, OPTSCH** backOpt, int* backLength)
{
	int index_back;
//	int countback;
	int index0;
	float headX;
	float v, s;
	float delta_v, delta_e, delta_t;	//delta_s
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#seg_ed_rvrs_cal";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	index_back = 0;
	v = startV;
	headX = rc[rcIndex].end;
	delta_e = 0;
	delta_t = 0;
	(*backOpt) = (OPTSCH*) malloc((index_back + 1) * sizeof(OPTSCH));
	if(*backOpt==NULL)
	{
		raw_opt_exception_decorator(RAW_MALLOC_ERROR,&opt_ret,desc);
		return raw_result_adapter(opt_ret);
	}
	(*backOpt)[index_back].start = headX;
	(*backOpt)[index_back].gear = gear;
	(*backOpt)[index_back].velocity = v;
	(*backOpt)[index_back].consumption = delta_e;
	(*backOpt)[index_back].time = delta_t;
	(*backOpt)[index_back].acc = 0;
	index_back = index_back + 1;

	//反求过程计算
//	countback = count;
	index0 = index - 1;
	headX =(* optSch)[index0].start;
	while (1) {
		index0 = index0 - 1;
		s = headX - (*optSch)[index0].start;

		trac_ret=DoCaculateByDistance(headX, v, gear, &delta_t, &count, s, &delta_v,
				&delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		v = v - delta_v;
		headX = headX - s;
		(*backOpt) = (OPTSCH*) realloc((*backOpt),
				(index_back + 1) * sizeof(OPTSCH));
		if(*backOpt==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
			return raw_result_adapter(opt_ret);
		}
		(*backOpt)[index_back].start = headX;
		(*backOpt)[index_back].gear = gear;
		(*backOpt)[index_back].velocity = v;
		(*backOpt)[index_back].consumption = delta_e;
		(*backOpt)[index_back].time = delta_t;
		(*backOpt)[index_back].acc = -delta_v / delta_t;
		index_back = index_back + 1;
		if (index0 < categoryStartIndex) {
			*flag = -1;
			break;
		}
		if (v > (*optSch)[index0].velocity || fabs(v - (*optSch)[index0].velocity)<opt_const.PRECISION) {
			*flag = index0;
			break;
		}
	}
	*localCount=count;
	*localIndex=index;
	*backLength=index_back;
	return raw_result_adapter(opt_ret);
}
/**
 *  @brief 将两个曲线片段连接在一起（后段desc连接到前段src，在实际中是将反求的曲线片段连接到原始曲线中
 *  @param src  已经优化的曲线
 *  @param  srcIndex  已经优化的点的index
 *  @param  markIndex  之前反求与原曲线交汇处的index
 *  @param  desc  反求之后的曲线片段
 *  @param  descLength  反求之后的曲线片段长度
 *  @return 优化异常结果
 */
OPTIMIZE_RESULT conct_two_seg(OPTSCH** src, int* srcIndex,  int markIndex, OPTSCH**  desc, int descLength)
{
	int j;
	int tempindex;
	int temp=*srcIndex;
	int index=*srcIndex;
	//异常处理描述
	char tmp[EXP_DESC_LENGTH]="#conct_two_seg";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;

	for (j = 0; j < descLength - 2; j++) {
		tempindex = descLength - 2 - j;
		index = markIndex + j;
		if (index>temp)
			(*src) = (OPTSCH*) realloc((*src),
					(index + 1) * sizeof(OPTSCH));
		if(*src==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,tmp);
			return raw_result_adapter(opt_ret);
		}
		(*src)[index].start = (*desc)[tempindex].start; //公里标取该段后面的值
		(*src)[index].gear = (*desc)[tempindex + 1].gear;
		(*src)[index].velocity = (*desc)[tempindex + 1].velocity;
		(*src)[index].consumption = (*desc)[tempindex + 1].consumption;
		(*src)[index].time = (*desc)[tempindex + 1].time;
		(*src)[index].acc = (*desc)[tempindex].acc;
	}
	index=index+1;
	*srcIndex=index;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 从段末反求同原曲线相交或者公里标已经到达某个mark的位置
 * @param  startV  反求的初始速度
 * @param  gear  反求的gear
 * @param  rcIndex  所在段的index
 * @param    localIndex  当前优化的初始index
 * @param    localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param    flag  标志位（0：表示与曲线在段内有交点，1：表示同曲线在段内没有交点）
 * @param    optSch  优化结果曲线
 * @param    backOpt  反求的优化曲线片段
 * @param    backLength  反求的优化曲线片段长度
 * @param    mark  某个mark位置（公里标）
 * @return    优化异常结果
 */
OPTIMIZE_RESULT  seg_ed_rvrs_cal_til_mark(float startV,int gear, int rcIndex, int* localIndex, int* localCount, int* flag, OPTSCH** optSch, OPTSCH** backOpt, int* backLength, float mark)
{
	int index_back;
//	int countback;
	float headX;
	float v;	//, s;
	float delta_s, delta_v, delta_e, delta_t;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#seg_ed_rvrs_cal_til_mark";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	index_back = 0;
	v = startV;
	headX = rc[rcIndex].end;
	delta_e = 0;
	delta_t = 0;
	//记录
	(*backOpt) = (OPTSCH*) malloc((index_back + 1) * sizeof(OPTSCH));
	if(*backOpt==NULL)
	{
		raw_opt_exception_decorator(RAW_MALLOC_ERROR,&opt_ret,desc);
		return raw_result_adapter(opt_ret);
	}
	(*backOpt)[index_back].start = headX;
	(*backOpt)[index_back].gear = gear;
	(*backOpt)[index_back].velocity = v;
	(*backOpt)[index_back].consumption = delta_e;
	(*backOpt)[index_back].time = delta_t;
	(*backOpt)[index_back].acc = 0;
	//反求过程计算
//	countback = count;
	//按时间步长反算
	while (1) {
		trac_ret=DoCaculateByTime(headX, v, gear,opt_const. TSTEP, &count, &delta_s,
				&delta_v, &delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		v = v - delta_v;
		headX = headX - delta_s;
		(*backOpt) = (OPTSCH*) realloc((*backOpt),
				(index_back +1)* sizeof(OPTSCH));
		if(*backOpt==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
			return raw_result_adapter(opt_ret);
		}
		(*backOpt)[index_back].start = headX;
		(*backOpt)[index_back].gear = gear;
		(*backOpt)[index_back].velocity = v;
		(*backOpt)[index_back].consumption = delta_e;
		(*backOpt)[index_back].time = opt_const.TSTEP;
		(*backOpt)[index_back].acc = delta_v / delta_t;
		index_back = index_back + 1;
		if (headX < mark || fabs(headX -  mark)<opt_const.PRECISION) //当车头到达标记点mark后，开始判断使用原计算中的步长
			break;
	}
	*localIndex=index;
	*localCount=count;
	*backLength=index_back;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 从某个位置开始反求同曲线有交点则退出（只需要有交点，无需其他位置）
 * @param gear  反求采用的档位
 * @param  localIndex  反求的起始index(optSch中的index)
 * @param localCount 用于查找当前位置属于某个段的起始roadIndex
 * @param backLength  反求的曲线片段的长度
 * @param flag：是否追上原始曲线（同原始曲线有交点）的flag
 * @param optSch  优化速度曲线
 * @param backOpt 反求的曲线片段
 * @return 优化异常结果
 */
OPTIMIZE_RESULT rvrs_cal_til_inctersect(int gear, int *localIndex, int*localCount, int* backLength, int* flag,int roadIndex, OPTSCH** optSch,OPTSCH** backOpt)
{
	int index0;
	float headX;
	float v, s;
	float delta_v, delta_e, delta_t;	//delta_s
	int index_back=0;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#rvrs_cal_til_inctersect";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	v=rc[roadIndex].vo;
	index0=index-1;
	headX=(*optSch)[index0].start;
	while (1) {
		index0 = index0 - 1;
		s = headX - (*optSch)[index0].start;
		trac_ret=DoCaculateByDistance(headX, v, gear, &delta_t, &count,
				s, &delta_v, &delta_e);
		if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			strcat(desc,trac_ret.description);
			trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
			return trac_result_adapter(trac_ret);
		}
		v = v - delta_v;
		headX = headX - s;
		(*backOpt) = (OPTSCH*) realloc((*backOpt),
				(index_back+1)* sizeof(OPTSCH));
		if(*backOpt==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
			return raw_result_adapter(opt_ret);
		}
		(*backOpt)[index_back].start = headX;
		(*backOpt)[index_back].gear = gear;
		(*backOpt)[index_back].velocity = v;
		(*backOpt)[index_back].consumption = delta_e;
		(*backOpt)[index_back].time = delta_t;
		(*backOpt)[index_back].acc = -delta_v / delta_t;
		index_back = index_back + 1;
		if (v > (*optSch)[index0].velocity || fabs(v - (*optSch)[index0].velocity)<opt_const.PRECISION) {
			*flag = index0;
			break;
		}
	}
	*localIndex=index;
	*localCount=count;
	*backLength=index_back;
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 从段末以某个速度和档位反求，直到和原曲线有在段内有交点或者到某个mark位置仍旧没有交点结束
 * @param startV  初始速度
 * @param gear  初始档位
 * @param rcIndex  所在段的index
 * @param localIndex  当前优化的初始index
 * @param localCount  用于查找当前位置属于某个段的起始roadIndex
 * @param optSch  优化结果曲线
 * @param  backOpt  反求的曲线片段
 * @param  backLength  反求的曲线片段长度
 * @param mark   某个标记位置
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  seg_ed_rvrs_cal_til_mark_or_inter(float startV,int gear, int rcIndex,int* localIndex, int* flag, int* localCount,OPTSCH** optSch, OPTSCH** backOpt, int* backLength,float mark)
{
	int index0;
	float headX;
	float v, s;
	float delta_v, delta_e, delta_t;	//delta_s
	int index_back;
	int index=*localIndex;
	int count=*localCount;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#seg_ed_rvrs_cal_til_mark_or_inter";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;

	index0 = index - 1;
	headX =(* optSch)[index0].start;
	index_back = 0;
	v = startV;
	if(gear>0)
	{
		while (1) {
			index0 = index0 - 1;
			if (index0 < 0) //由源程序中<=改为<
				break;
			s = headX - (*optSch)[index0].start;
			trac_ret=DoCaculateByDistance(headX, v, gear, &delta_t, &count, s,
					&delta_v, &delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			v = v - delta_v;
			headX = headX - s;
			(*backOpt) = (OPTSCH*) realloc((*backOpt),( index_back+1) * sizeof(OPTSCH));
			if(*backOpt==NULL)
			{
				raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
				return raw_result_adapter(opt_ret);
			}
			(*backOpt)[index_back].start = headX;
			(*backOpt)[index_back].gear = gear;
			(*backOpt)[index_back].velocity = v;
			(*backOpt)[index_back].consumption = delta_e;
			(*backOpt)[index_back].time = delta_t;
			(*backOpt)[index_back].acc = -delta_v / delta_t;
			index_back = index_back + 1;

			if (v < (*optSch)[index0].velocity || fabs(v - (*optSch)[index0].velocity)<opt_const.PRECISION) {
				*flag = index0;
				break;
			}
			if (headX < (float)rc[rcIndex].start || fabs(headX - (float)rc[rcIndex].start)<opt_const.PRECISION) { //反向超过路段头
				//从头最大档牵引到尾
				*flag=-1;
				break;
			}
		}
	}
	else
	{
		while (1) {
			index0 = index0 - 1;
			s = headX - (*optSch)[index0].start;
			trac_ret=DoCaculateByDistance(headX, v, gear, &delta_t, &count, s,&delta_v, &delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				return trac_result_adapter(trac_ret);
			}
			v = v - delta_v;
			headX = headX - s;
			(*backOpt) = (OPTSCH*) realloc((*backOpt), (index_back+1) * sizeof(OPTSCH));
			if(*backOpt==NULL)
			{
				raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
				return raw_result_adapter(opt_ret);
			}
			(*backOpt)[index_back].start = headX;
			(*backOpt)[index_back].gear = gear;
			(*backOpt)[index_back].velocity = v;
			(*backOpt)[index_back].consumption = delta_e;
			(*backOpt)[index_back].time = delta_t;
			(*backOpt)[index_back].acc = -delta_v / delta_t;
			index_back = index_back + 1;
			if (v >(* optSch)[index0].velocity || fabs(v - (* optSch)[index0].velocity)<opt_const.PRECISION) {
				*flag = index0;
				break;
			}
			if (headX < (float)rc[rcIndex].start || fabs(headX - (float)rc[rcIndex].start)<opt_const.PRECISION) { // 反向超过路段头
				//惰行不可行，尝试最大制动
				* flag=-1;
				break;
			}
		}
	}
	*localIndex=index;
	*localCount=count;
	*backLength=index_back;
	return raw_result_adapter(opt_ret);
}
