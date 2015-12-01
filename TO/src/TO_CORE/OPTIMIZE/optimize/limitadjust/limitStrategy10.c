/**
 * @file     limitStrategy10.c
 * @brief 手动区域的限速策略（限速范围部分在手动区域，或者限速范围完全在手动区域,包括贯通实验和补机段）
 * @date  2014.5.4
 * @author sinlly
 * @note
 */

#include "limitAdjustStrategy.h"
#define HAND_AREA_INTERVAL  10.0
/**
 * @brief 限速范围部分或者完全在手动区域段
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT limitStrategy10(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart)
{
	int index;
	float step = opt_const.TSTEP;
	float ending = limitInfo.end;
	float value = limitInfo.limit;
	int indexS = limitInfo.indexS;
	int index1 = limitInfo.index1;
	float start = optsch[index1].start;
	int indexE = limitInfo.indexE;
	int index2 = limitInfo.index2;
	float handAreaInternal=HAND_AREA_INTERVAL;
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#limitStrategy10";
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	LIMIT_OPTIMIZE_RESULT limit_ret;
	/**
	 * 限速左侧在手动区域内部，右侧刚好在手动区域终止位置或者（间隔不超过5米）flag=-1；
	 * 限速两侧都在手动区域内部  flag=0 ;
	 * 限速右侧在手动区域内部，左侧刚好在手动区域终止位置或者（间隔不超过5米） flag=1;
	 * 限速两侧都恰好在手动区域起始位置和终止位置（或者间隔不超过5米）flag=2
	 */
	int typeFlag=-2;
	float  handStart; //手动区域起始位置
	float  handEnd; //手动区域终止位置
	int handSIndex; //手动区域起始位置在优化曲线中的index
	int handEIndex; //手动区域终止位置在优化曲线中的index
	float handSV; //手动区域起始位置在优化曲线中对应的速度
	float handEV; //手动区域终止位置在优化曲线中对应的速度
	int i;
	if(rc[indexS].flag==opt_const.THROUGHEXPERT) //当前位置在贯通实验
	{
		if(fabs(optsch[index1].start-throughExpert.start)< handAreaInternal && fabs(optsch[index2].start-throughExpert.ending)< handAreaInternal) //限速两侧刚好和手动区域重合
		{
			handStart=optsch[index1].start;
			handEnd=optsch[index2].start;
			typeFlag=2;
		}
		else if(fabs(optsch[index1].start-throughExpert.start)< handAreaInternal  && optsch[index2].start<throughExpert.ending)//限速左和手动重合，右侧在手动内部
		{
			handStart=optsch[index1].start;
			handEnd=throughExpert.ending;
			typeFlag=1;
		}
		else if(optsch[index1].start>throughExpert.start && fabs(optsch[index2].start-throughExpert.ending)< handAreaInternal)//限速右和手动重合，左侧在手动内部
		{
			handStart=throughExpert.start;
			handEnd=optsch[index2].start;
			typeFlag=-1;
		}
		else if(optsch[index1].start>throughExpert.start && optsch[index2].start<throughExpert.ending) //限速两侧都在手动内部
		{
			handStart=throughExpert.start;
			handEnd=throughExpert.ending;
			typeFlag=0;
		}
	}
	else if(rc[indexS].flag==opt_const.PUSHERENGINE) //限速未在贯通实验区域,此时需要查看限速是否在补机区域
	{
		for(i=0;i<pusherEngines.len;i++)
		{
			float tmpS=pusherEngines.handAreas[i].start;
			float tmpE= pusherEngines.handAreas[i].ending;
			if(fabs(optsch[index1].start-tmpS)< handAreaInternal && fabs(optsch[index2].start-tmpE)< handAreaInternal) //限速两侧刚好和手动区域重合
			{
				handStart=optsch[index1].start;
				handEnd=optsch[index2].start;
				typeFlag=2;
			}
			else if(fabs(optsch[index1].start-tmpS)< handAreaInternal  && optsch[index2].start<tmpE)//限速左和手动重合，右侧在手动内部
			{
				handStart=optsch[index1].start;
				handEnd=tmpE;
				typeFlag=1;
			}
			else if(optsch[index1].start>tmpS && fabs(optsch[index2].start-tmpE)< handAreaInternal)//限速右和手动重合，左侧在手动内部
			{
				handStart=tmpS;
				handEnd=optsch[index2].start;
				typeFlag=-1;
			}
			else if(optsch[index1].start>tmpS && optsch[index2].start<tmpE) //限速两侧都在手动内部
			{
				handStart=tmpS;
				handEnd=tmpE;
				typeFlag=0;
			}
			if(typeFlag!=-2)//已经找到该限速所在的手动区域
				break;
		}
	}
	getSchVelocity(optsch,len,handStart,handEnd,&handSV,&handSIndex,&handEV,&handEIndex);//得到手动区域位置的点
	if(typeFlag==2) //限速两侧恰好和手动区域重合（保持限速即可）
	{
		for(i=index1;i<=index2;i++)
		{
			float del_s=optsch[i+1].start-optsch[i].start;
			optsch[i].gear =rc[indexS].flag;
			optsch[i].velocity = value;
			optsch[i].time = del_s*3.6/value;
		}
	}
	else if(typeFlag==-1) //限速左侧在手动区域中，右侧同手动区域重合（先拉直到限速，然后保持限速）
	{
		float deltaV=optsch[handSIndex].velocity-value;
		float deltaS=optsch[index1].start-optsch[handSIndex].start;
		for (i=handSIndex+1;i<=index1;i++)
		{
			float del_s=optsch[i].start-optsch[i-1].start;
			float del_s1=optsch[i+1].start-optsch[i].start;
			optsch[i].gear=rc[indexS].flag; //提醒空气制动标识
			optsch[i].velocity=optsch[i-1].velocity-del_s*deltaV/deltaS;
			optsch[i].time=del_s1*3.6/optsch[i].velocity;
		}

		for(i=index1+1;i<=handEIndex;i++)
		{
			float del_s=optsch[i+1].start-optsch[i].start;
			optsch[i].gear =rc[indexS].flag;
			optsch[i].velocity = value;
			optsch[i].time = del_s*3.6/value;
		}
	}
	else if(typeFlag==1) //限速右侧在手动区域中，左侧同手动区域重合（先保持限速，然后从限速拉直到手动区域终止点）
	{
		for(i=handSIndex;i<=index2;i++)
		{
			float del_s=optsch[i+1].start-optsch[i].start;
			optsch[i].gear =rc[indexS].flag;
			optsch[i].velocity = value;
			optsch[i].time = del_s*3.6/value;
		}

		float deltaV=optsch[handEIndex].velocity-value;
		float deltaS=optsch[handEIndex].start-optsch[index2].start;
		for (i=index2+1;i<=handEIndex;i++)
		{
			float del_s=optsch[i].start-optsch[i-1].start;
			float del_s1=optsch[i+1].start-optsch[i].start;
			optsch[i].gear=rc[indexS].flag; //提醒空气制动标识
			optsch[i].velocity=optsch[i-1].velocity-del_s*deltaV/deltaS;
			optsch[i].time=del_s1*3.6/optsch[i].velocity;
		}
	}
	else if(typeFlag==0) //两侧都在手动区域内部（先拉直到限速，保持限速，从限速拉直到手动区域终止位置）
	{
		float deltaV=optsch[handSIndex].velocity-value;
		float deltaS=optsch[index1].start-optsch[handSIndex].start;
		for (i=handSIndex+1;i<=index1;i++)
		{
			float del_s=optsch[i].start-optsch[i-1].start;
			float del_s1=optsch[i+1].start-optsch[i].start;
			optsch[i].gear=rc[indexS].flag; //提醒空气制动标识
			optsch[i].velocity=optsch[i-1].velocity-del_s*deltaV/deltaS;
			optsch[i].time=del_s1*3.6/optsch[i].velocity;
		}

		for(i=index1+1;i<=index2;i++)
		{
			float del_s=optsch[i+1].start-optsch[i].start;
			optsch[i].gear =rc[indexS].flag;
			optsch[i].velocity = value;
			optsch[i].time = del_s*3.6/value;
		}

		deltaV=optsch[handEIndex].velocity-value;
		deltaS=optsch[handEIndex].start-optsch[index2].start;
		for (i=index2+1;i<=handEIndex;i++)
		{
			float del_s=optsch[i].start-optsch[i-1].start;
			float del_s1=optsch[i+1].start-optsch[i].start;
			optsch[i].gear=rc[indexS].flag; //提醒空气制动标识
			optsch[i].velocity=optsch[i-1].velocity-del_s*deltaV/deltaS;
			optsch[i].time=del_s1*3.6/optsch[i].velocity;
		}
	}
	return ret;
}
