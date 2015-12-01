/**
 * @file     tempLImitOptimize.c
 * @brief    临时限速处理主入口函数
 * @date  2014.7.13
 * @author sinlly
 */
#include "opt_global_var.h"
#include "rawOptimize.h"
#include "math.h"
#include "optimizeTools.h"
#include "limitAdjust.h"
#include "optimize_curve.h"
#include "exception.h"
#include "after_process.h"
/**
 * @brief  临时限速之后的优化结果后处理（给自适应控制的最终结果）
 * @param opt_final_curves   原始优化速度曲线（信号机+越过距离的最终表示形式）
 * @param srcLen  原始优化速度曲线（信号机+越过距离的最终表示形式）的长度
 * @param optSch 原始优化速度曲线（连续公里标表示形式）
 * @param len 原始优化速度曲线（连续公里标表示形式）的长度
 * @param tmp_final_curves  最后生成的临时限速片段（信号机+越过距离的表示形式）
 * @startIndex  临时限速片段起始位置在optSch中的位置
 * @endIndex  临时限速片段终止位置在optSch中的位置
 * @return  处理结果正确性
 */
OPTIMIZE_RESULT tmp_post_process(OPTSCHCURVE* opt_final_curves,int srcLen,OPTSCH* optSch,int len,OPTSCHCURVE* tmp_final_curves,int startIndex,int endIndex);
/**
 * @brief 临时限速处理主入口函数
 * @param startPost  临时限速起始公里标
 * @param endPost  临时限速终止公里标
 * @param value   临时限速值
 * @param optSch    优化速度曲线
 * @param len  原始优化速度曲线的长度
 * @param tmp_len 临时限速最终结果片段的长度
 * @return   处理之后的优化速度曲线
 */
OPTSCHCURVE* tempLimitOpimize(int startPost, int endPost,int value,OPTSCH* optSch,int len,int* tmp_len){
	opt_log_info("临时限速处理开始");
	opt_log_info("起始位置: %d, 终止位置: %d, 限速值: %d", startPost,endPost,value);
	OPTIMIZE_RESULT ret;
	char desc[EXP_DESC_LENGTH]="#tempLimitOptimize";
	ret.opt_rest=OPTIMIZE_SUCCESS;
	OPTSCH* backupCurve=(OPTSCH*)malloc(sizeof(OPTSCH)*len);
	if(backupCurve==NULL)
	{
		optimize_exception_decorator(MALLOC_ERROR,&ret,"#tempLimitOptimize#backupCurve");
		exception_display(ret);
		return NULL;
	}
	int i;
	//建立一个备份
	for(i=0;i<len;i++)
	{
		backupCurve[i].start=optSch[i].start;
		backupCurve[i].velocity=optSch[i].velocity;
	}
	int limitIndexS,limitIndexE;//限速本身所在的起止公里标
	limitIndexS=0;
	limitIndexE=len-1;
	//进行临时限速处理
	opt_log_info("临时限速调整开始");
	ret=tempLimitAdjust(optSch,len,startPost,endPost,value,&limitIndexS,&limitIndexE);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		free(backupCurve);
		return NULL;
	}
	opt_log_info("临时限速调整结束");
	int limitS=0,limitE=len-1;
	//找到临时限速变化的最终起止公里标(limitS为档位变换的前一个点，limitE为档位变换的后一个点，与原始曲线不同的范围为[limitS+1,limitE],之所以要limitS=i，为在拉平中起始点需要上一个档位的最后一个点的档位及对应的速度和坐标
	//速度和坐标作为计算的起始点，但最终保存的为第一次计算之后的速度和坐标
	for(i=limitIndexS;i>=0;i--)
	{
		if(fabs(optSch[i].start-backupCurve[i].start)<opt_const.PRECISION && fabs(optSch[i].velocity-backupCurve[i].velocity)<opt_const.PRECISION)
		{
			limitS=i;
			break;
		}
	}
	for(i=limitIndexE;i<len;i++)
	{
		if(fabs(optSch[i].start-backupCurve[i].start)<opt_const.PRECISION && fabs(optSch[i].velocity-backupCurve[i].velocity)< opt_const.PRECISION)
		{
			limitE=i-1;
			break;
		}
	}
//	free(backupCurve);
	int length=limitE-limitS+1;
	OPTSCH* tmpCurve=(OPTSCH*) malloc(sizeof(OPTSCH)*(length));
	if(tmpCurve==NULL)
	{
		optimize_exception_decorator(MALLOC_ERROR,&ret,"#tempLimitOptimize#tmpCurve");
		exception_display(ret);
		return NULL;
	}
	for(i=0;i<length;i++)
	{
		tmpCurve[i].velocity=optSch[i+limitS].velocity;
		tmpCurve[i].consumption=optSch[i+limitS].consumption;
		tmpCurve[i].gear=optSch[i+limitS].gear;
		tmpCurve[i].start=optSch[i+limitS].start;
		tmpCurve[i].time=optSch[i+limitS].time;
		tmpCurve[i].acc=optSch[i+limitS].acc;
	}
	//临时限速档位拉平操作
	opt_log_info("临时限速档位后处理开始");
	OPTSCH* new_tmp_curve;
	int tmp_final_len;
	int capacity;
	new_tmp_curve=tmp_after_process(tmpCurve,length,new_tmp_curve,&tmp_final_len,&capacity,&ret,(float)startPost,(float)endPost,(float)value);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		return NULL;
	}
	opt_log_info("临时限速档位后处理结束");

	//临时限速最后一个点的处理（需要保证档位连续，且速度能够追上原始曲线）
	opt_log_info("临时限速结尾档位连续性处理开始");
	new_tmp_curve=tmp_end_process(new_tmp_curve,&tmp_final_len,&capacity,backupCurve,&limitE,&ret);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		return NULL;
	}
	saveResult(new_tmp_curve,tmp_final_len,"tmpResult");
	opt_log_info("临时限速结尾档位连续性处理结束");
	//		free(tmpCurve);

	opt_log_info("临时限速自适应控制标识处理开始");
	OPTSCHCURVE* tmp_final_curves=(OPTSCHCURVE*)malloc(sizeof(OPTSCHCURVE)*(tmp_final_len));
	if(tmp_final_curves==NULL)
	{
		optimize_exception_decorator(MALLOC_ERROR,&ret,"#tempLimitOptimize#tmpCurve");
		exception_display(ret);
		free(tmpCurve);
		return NULL;
	}
	ret=tmp_post_process(opt_final_curves,MAX_FINAL_LENGTH,new_tmp_curve,tmp_final_len,tmp_final_curves,limitS,limitE);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		free(tmp_final_curves);
		return NULL;
	}
	opt_log_info("临时限速自适应控制标识处理结束");

	saveFinalResult(tmp_final_curves,tmp_final_len,"tmpFinalResult");
	opt_log_info("临时限速处理结束");
	*tmp_len=tmp_final_len;
	exception_display(ret);
	free(new_tmp_curve);
	return tmp_final_curves;
}
/**
 * @brief  临时限速之后的优化结果后处理（给自适应控制的最终结果）
 * @param opt_final_curves   原始优化速度曲线（信号机+越过距离的最终表示形式）
 * @param srcLen  原始优化速度曲线（信号机+越过距离的最终表示形式）的长度
 * @param optSch 原始优化速度曲线（连续公里标表示形式）
 * @param len 原始优化速度曲线（连续公里标表示形式）的长度
 * @param tmp_final_curves  最后生成的临时限速片段（信号机+越过距离的表示形式）
 * @startIndex  临时限速片段起始位置在optSch中的位置
 * @endIndex  临时限速片段终止位置在optSch中的位置
 * @return  处理结果正确性
 */
OPTIMIZE_RESULT  tmp_post_process(OPTSCHCURVE* opt_final_curves,int srcLen,OPTSCH* optSch,int len, OPTSCHCURVE* tmp_final_curves,int startIndex,int endIndex)
{
	int i=1,j;
	int lastStatIndex=-1;
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	float start;
	//计算点距离前方车站的时间
	int startStatIndex=0;
	int endStatIndex=MAX_STATION-1;
	//车站的公里标在该临时曲线的外侧
	for(i=0;i<MAX_STATION;i++)
	{
		start=stations[i].start;
		if ((fabs(start-optSch[0].start)<opt_const.PRECISION || start>optSch[0].start))
		{
			if(i>0)
				startStatIndex=i-1;
			else
				startStatIndex=i;
			break;
		}
	}
	for(i=0;i<MAX_STATION;i++)
	{
		start=stations[i].start;
		if((fabs(start-optSch[len-1].start)<opt_const.PRECISION || start>optSch[len-1].start))
		{
			endStatIndex=i;
			break;
		}
	}
	//寻找最外层车站在opt_final_curves中的index
	int optSIndex=0;
	int optEIndex=srcLen-1;
	for(i=startIndex;i>0;i--)
	{
		start=stations[startStatIndex].start;
		if((fabs(start-opt_final_curves[i].con_tel_kp)<opt_const.PRECISION || start<opt_final_curves[i].con_tel_kp)&& (fabs(opt_final_curves[i-1].con_tel_kp-start)<opt_const.PRECISION || opt_final_curves[i-1].con_tel_kp<start))
		{
			optSIndex=i-1;
		}
	}
	for(i=endIndex+1;i<srcLen;i++)
	{
		start=stations[endStatIndex].start;
		if((fabs(start-opt_final_curves[i].con_tel_kp)<opt_const.PRECISION || start<opt_final_curves[i].con_tel_kp)&& (fabs(opt_final_curves[i-1].con_tel_kp-start)<opt_const.PRECISION || opt_final_curves[i-1].con_tel_kp<start))
		{
			optEIndex=i-1;
		}
	}
	//对每个点求距离最近一个车站的时间
	int statIndexTmp;
	for(i=startStatIndex+1;i<endStatIndex;i++)
	{
		float lastTime=0;
		statIndexTmp=0;
		start=stations[i].start;
		for( j=1;j<len;j++)
		{
			if ((fabs(start-optSch[j-1].start)<opt_const.PRECISION || start>optSch[j-1].start)  && ( fabs(optSch[j].start-start)<opt_const.PRECISION || start<optSch[j].start))
			{
				statIndexTmp=j-1;
				break;
			}
		}
		float runTime=0;
		if(i==startStatIndex+1)
		{
			for(j=optSIndex;j<=startIndex;j++)
			{
				runTime=runTime+opt_final_curves[j].time;
				lastTime=lastTime+opt_final_curves[j].time;
			}
			for(j=0;j<=statIndexTmp;j++)
			{
				runTime=runTime+optSch[j].time;
			}
			for(j=0;j<=statIndexTmp;j++)
			{
				tmp_final_curves[j].gear=optSch[j].gear;
				tmp_final_curves[j].velocity=optSch[j].velocity;
				tmp_final_curves[j].ford_time=runTime-lastTime;
				tmp_final_curves[j].time=optSch[j].time;
				tmp_final_curves[j].ford_station_num=stations[i].id;
				tmp_final_curves[j].manual_flag=NONE_FLAG;
				tmp_final_curves[j].con_tel_kp=optSch[j].start;
				lastTime=lastTime+optSch[j].time;
			}
		}
		else
		{
			for(j=lastStatIndex+1;j<=statIndexTmp;j++)
			{
				runTime=runTime+optSch[j].time;
			}
			for(j=lastStatIndex+1;j<=statIndexTmp;j++)
			{
				tmp_final_curves[j].gear=optSch[j].gear;
				tmp_final_curves[j].velocity=optSch[j].velocity;
				tmp_final_curves[j].ford_time=runTime-lastTime;
				tmp_final_curves[j].ford_station_num=stations[i].id;
				tmp_final_curves[j].time=optSch[j].time;
				tmp_final_curves[j].manual_flag=NONE_FLAG;
				tmp_final_curves[j].con_tel_kp=optSch[j].start;
				lastTime=lastTime+optSch[j].time;
			}
		}
		lastStatIndex=statIndexTmp;
	}
	//对最后一部分距离临时曲线外的车站进行处理
	float runTime=0;
	for(j=lastStatIndex+1;j<len;j++)
	{
		runTime=runTime+optSch[j].time;
	}
	for(j=endIndex;j<=optEIndex;j++)
	{
		runTime=runTime+opt_final_curves[j].time;
	}
	float lastTime=0;
	for(j=lastStatIndex+1;j<len;j++)
	{
		tmp_final_curves[j].gear=optSch[j].gear;
		tmp_final_curves[j].velocity=optSch[j].velocity;
		tmp_final_curves[j].ford_time=runTime-lastTime;
		tmp_final_curves[j].time=optSch[j].time;
		tmp_final_curves[j].ford_station_num=stations[endStatIndex].id;
		tmp_final_curves[j].manual_flag=NONE_FLAG;
		tmp_final_curves[j].con_tel_kp=optSch[j].start;
		lastTime=lastTime+optSch[j].time;
	}

	//禁止坡停区域标识（车子或者车身属于陡上坡或者缓坡部分;车子或者车身属于局部陡上坡小段）;禁止夸坡停车区域标识(1、陡上坡转平坡、缓下坡  2、陡上坡转下坡（非陡坡和陡坡） 3、陡下坡、超陡下坡转陡上坡
	float curPost;
	int curSrcIndex; //当前公里标在原始曲线中的位置
	for(i=0;i<len;i++)
	{
		curPost=optSch[i].start;
		for(j=startIndex;j<endIndex;j++)
		{
			if(j==startIndex && curPost<opt_final_curves[j].con_tel_kp) //当前点小于原始曲线限速调整片段最开始的点
			{
				curSrcIndex=j;
				break;
			}
			else if((j==endIndex-1 && curPost>opt_final_curves[j+1].con_tel_kp))//当前点大于原始曲线限速调整片段最后一个点
			{
				curSrcIndex=j+1;
				break;
			}
			if((curPost>opt_final_curves[j].con_tel_kp || fabs(curPost-opt_final_curves[j].con_tel_kp)<opt_const.PRECISION) && (curPost<opt_final_curves[j+1].con_tel_kp || fabs(curPost-opt_final_curves[j+1].con_tel_kp)<opt_const.PRECISION))
			{
				if(fabs(curPost-opt_final_curves[j].con_tel_kp) < fabs(curPost-opt_final_curves[j+1].con_tel_kp))
					curSrcIndex=j;
				else
					curSrcIndex=j+1;
				break;
			}
		}
		tmp_final_curves[i].anti_stop_flag=opt_final_curves[j].anti_stop_flag;
	}

	//空气制动区域
	i=0;
	for(;i<len;i++)
	{
		if(optSch[i].gear==opt_const.AIRS)
			tmp_final_curves[i].manual_flag=AIR_DAMPING_BRAKE;
		else if(optSch[i].gear==opt_const.AIRE)
			tmp_final_curves[i].manual_flag=AIR_DAMPING_AVG;
	}
	//将公里标转换为信号机+越过距离形式
	struct tele_t *tele;
	int tel_length;
	tele=(struct tele_t*)read_binary_file(0x04, &tel_length);
	OPT_EXTR_RESULT file_ret;
	if(tele==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&file_ret,"#post_process");
		return file_result_adapter(file_ret);
	}
	i=1;
	int lastIndex=-1;
	for(;i<tel_length;i++)
	{
		float next_start=tele[i].cont_km_post;
		for(j=lastIndex+1;j<len;j++)
		{
			if(optSch[j].start< next_start)
			{
				tmp_final_curves[j].tel_num=tele[i-1].t_num;
				tmp_final_curves[j].tel_kp=tele[i-1].discont_km_post;
				tmp_final_curves[j].cross_dist=optSch[j].start-tele[i-1].cont_km_post;
			}
			else
				break;
		}
		lastIndex=j-1;
	}
	return ret;
}
