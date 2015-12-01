/**
 * @file     airbrake_opt_imp.c
 * @brief  机车进行空气制动相关操作实现
 * @date  2014.7.23
 * @author sinlly
 */
#include "base_opt_fuc.h"
/**
 *@brief  处理空气制动
 *@param   rcIndex  所在段的index
 *@param   localIndex  当前优化的初始index
 *@param   optSch  优化结果曲线
 *@return 优化异常结果
 */
OPTIMIZE_RESULT air_brake_op( int rcIndex , int*  localIndex, OPTSCH** optSch)
{
	int tempIndex;
	float end;
	float v;
	float start;
	float delta_v, delta_e,delta_t,s;
	int gear;
	int index=*localIndex;
	tempIndex = index - 1;
	if (index == 1)
		tempIndex = 1;
   //异常处理描述
	char desc[EXP_DESC_LENGTH]="#air_brake_op";
	RAW_OPTIMIZE_RESULT opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;

	start=(*optSch)[tempIndex].start;
	v = (*optSch)[tempIndex].velocity;
	float srcV=v;
	float srcStart=start;
	end =(float) rc[rcIndex].end;
	s=start+opt_const.SSTEP;
	float vo=rc[rcIndex].vo;
	if(rcIndex+1<MAX_ROADCATEGORY && rc[rcIndex+1].vi>rc[rcIndex].vo) //若下一段的vi大于本段的vo则无需空气制动到vo。
		vo=rc[rcIndex+1].vi;
	for(;s<end;s+=opt_const.SSTEP)
	{
		delta_v=opt_const.SSTEP*(vo-v)/(end-start);
		srcV=srcV+delta_v;
		delta_t=opt_const.SSTEP*3.6/(srcV-delta_v/2);
		delta_e=getConsumption(0,delta_t);
		gear=opt_const.AIRS;
		(*optSch) = (OPTSCH*) realloc((*optSch), (index + 1) * sizeof(OPTSCH));
		if(*optSch==NULL)
		{
			raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
			return raw_result_adapter(opt_ret);
		}
		(*optSch)[index].start = s;
		(*optSch)[index].gear = gear;
		(*optSch)[index].velocity = srcV;
		(*optSch)[index].consumption = delta_e;
		(*optSch)[index].time = delta_t;
		(*optSch)[index].acc = (delta_v -(*optSch)[index-1].velocity) / delta_t;
		index=index+1;
	}
	s=s-opt_const.SSTEP;
	//delta_v=srcV-(s-srcStart)*(srcV-vo)/(rc[rcIndex].end-srcStart);
	delta_v=vo-srcV;
	delta_t=(end-s)*3.6*2/(srcV+vo);
	delta_e=getConsumption(0,delta_t);
	(*optSch) = (OPTSCH*) realloc((*optSch), (index + 1) * sizeof(OPTSCH));
	if(*optSch==NULL)
	{
		raw_opt_exception_decorator(RAW_REALLOC_ERROR,&opt_ret,desc);
		return raw_result_adapter(opt_ret);
	}
	(*optSch)[index].start = end;
	(*optSch)[index].gear = gear;
	//(*optSch)[index].velocity = delta_v;
	(*optSch)[index].velocity = vo;
	(*optSch)[index].consumption = delta_e;
	(*optSch)[index].time = delta_t;
	(*optSch)[index].acc = (delta_v -(*optSch)[index-1].velocity) / delta_t;
	index=index+1;
	*localIndex=index;
	return raw_result_adapter(opt_ret);
}
int copy_optsch(OPTSCH* src,OPTSCH* des,int length){
	int i=0;
	for(;i<length;i++){
		des[i].acc=src[i].acc;
		des[i].consumption=src[i].consumption;
		des[i].gear=src[i].gear;
		des[i].start=src[i].start;
		des[i].time=src[i].time;
		des[i].velocity=src[i].velocity;}
	return i;
}
