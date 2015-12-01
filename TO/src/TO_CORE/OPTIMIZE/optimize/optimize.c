/**
 * @file     optimize.c
 * @brief   原始优化主入口函数
 * @date  2014.4.6
 * @author sinlly
 */
#include  "opt_global_var.h"
#include  "optimize_curve.h"
#include "rawOptimize.h"
#include "limitAdjust.h"
#include "exception.h"
#include "read_binary_for_route.h"
#include "after_process.h"
#include "tractionCaculator.h"
OPTSCHCURVE* opt_final_curves;
OPTSCH* new_final_curve;
LOCOMARSHALLING locoMarshInfo;
int MAX_FINAL_LENGTH;
/**
 * @brief  优化计算的后处理
 * @param optSch   优化曲线
 * @param final_curves  优化最终输出给控制的曲线
 * @param len  optSch曲线的长度
 * @return  处理结果正确性
 */
OPTIMIZE_RESULT   post_process(OPTSCH* optSch,OPTSCHCURVE* final_curves,int len);
/**
 * @brief 原始优化主入口函数
 * @param routeNum  行程号
 * @return  优化速度曲线
 */
OPTSCHCURVE* optimize(int routeNum) {
	locoMarshInfo.totalWeight=5028.0;
	locoMarshInfo.totalNums=57;
	locoMarshInfo.totalLength=70.1;
	OPT_EXTR_RESULT extr_ret;
	OPTIMIZE_RESULT ret;
	char desc[EXP_DESC_LENGTH]="#optimize";
	opt_log_info("司机驾驶数据开始读取");
	//提取原始数据(司机),用作比较
	extrInitialData();
	int i;
	float time,consumption;
	for( i=0;i<1;i++)
	{
		opt_log_info("第%d次司机数据",i+1);
		time=initialDatas[i].time;
		consumption=initialDatas[i].consumption;
		opt_log_info("司机油耗:%f,司机驾驶时间: %f",consumption,time);
	}
	opt_log_info("司机驾驶数据读取成功");

	//基本数据初始化
	opt_log_info("基本数据初始化开始");
	extr_ret=extrData(routeNum);
	ret=file_result_adapter(extr_ret);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		return NULL;
	}
	opt_log_info("基本数据初始化成功");

	//优化预处理
	opt_log_info("优化预处理开始");
	float startPost=stations[0].start;
	float endPost=stations[MAX_STATION-1].start;
	ret=preprocess(startPost,endPost);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		return NULL;
	}
	opt_log_info("优化预处理结束");

	//在线优化输入参数准备
	float vStart,vEnd;
	int sIndex=0;
	int k;
	for(k=0;k<MAX_ROADCATEGORY;k++)
	{
		if (startPost>=rc[k].start && startPost<=rc[k].end)
		{
			sIndex=k;
			break;
		}
	}
	vStart=rc[sIndex].vi;
	opt_log_info("起始速度:%f",vStart);
	startPost=rc[sIndex].start;
	opt_log_info("起始公里标:%d",rc[sIndex].start);
	endPost=rc[MAX_ROADCATEGORY-1].end;
	vEnd=rc[MAX_ROADCATEGORY-1].vo;
	opt_log_info("终止速度:%f",vEnd);
	opt_log_info("终止公里标:%d",rc[MAX_ROADCATEGORY-1].end);

	//在线原始优化
	opt_log_info("原始优化开始");
	OPTSCH* optSch;
	optSch = rawOptimize(locoInfo.totalWeight, startPost, endPost,vStart,vEnd,&ret);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		return NULL;
	}
	opt_log_info("原始优化结果写入开始");
	saveResult(optSch,MAX_OPTSCHLENGTH,"rawOptimize");
	opt_log_info("原始优化结果写入成功");
	opt_log_info("原始优化结束");

	//原始优化空气制动测试
	char* filename="ret/airResult";
	airTest1(optSch,MAX_OPTSCHLENGTH,filename);
	opt_log_info("原始优化结果空气制动校验保存");
	// added over

	//优化曲线限速调整
	opt_log_info("限速调整开始");
	ret=limitAdjust(optSch,MAX_OPTSCHLENGTH,startPost,endPost);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		return NULL;
	}
	opt_log_info("限速调整结果写入开始");
	saveResult(optSch,MAX_OPTSCHLENGTH,"limitOptimize");
	opt_log_info("限速调整结果写入成功");
	opt_log_info("限速调整结束");


	char *filename2="ret/airResult_limit";
	airTest1(optSch,MAX_OPTSCHLENGTH,filename2);
    opt_log_info("限速中空气制动校验结果保存");

	//******************优化后处理步骤，后处理设计*********************
	opt_log_info("档位后处理开始");
	int new_final_len;
	new_final_curve= after_process(optSch,MAX_OPTSCHLENGTH,new_final_curve,&new_final_len,&ret);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		return NULL;
	}
	free(optSch);
	optSch=NULL;


	MAX_FINAL_LENGTH=new_final_len;
	opt_log_info("档位后处理结束");

	//对优化结果进行后处里，返回控制所需最终结果
	opt_log_info("自适应控制标识后处理开始");
	opt_final_curves=(OPTSCHCURVE*)malloc(sizeof(OPTSCHCURVE)*MAX_FINAL_LENGTH);
	if(opt_final_curves==NULL)
	{
		optimize_exception_decorator(MALLOC_ERROR,&ret,"optimize.c#final_curves");
		exception_display(ret);
		return NULL;
	}
	ret=post_process(new_final_curve,opt_final_curves,MAX_FINAL_LENGTH);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		exception_display(ret);
		return NULL;
	}
	opt_log_info("自适应控制标识后处理结束");

	//优化结果评估（时间，油耗）
	opt_log_info("优化结果评估开始");
	float E=0;
	float T=0;
	float diffTime=0;
	saveResult(new_final_curve,MAX_FINAL_LENGTH,"optimizeResult");
	evaluate(&E, &T, &diffTime, new_final_curve,MAX_FINAL_LENGTH,time,consumption);
	saveFinalResult(opt_final_curves,MAX_FINAL_LENGTH,"optFinalResult");
	opt_log_info("优化结果评估结束");

	exception_display(ret);
	return opt_final_curves;
}
/**
 * @brief  优化计算的后处理
 * @param optSch   优化曲线
 * @param final_curves  优化最终输出给控制的曲线
 * @param len  optSch曲线的长度
 * @return  处理结果正确性
 */
OPTIMIZE_RESULT  post_process(OPTSCH* optSch,OPTSCHCURVE* final_curves,int len)
{
	int i=1,j;
	int lastStatIndex=-1;
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;

	//计算点距离前方车站的时间
	i=1;
	float lastTime,runTime;
	for(;i<MAX_STATION;i++)
	{
		lastTime=0;
		int startIndex=0;
		float start;
		start=stations[i].start;
		for( j=1;j<len;j++)
		{
			if ((fabs(start-optSch[j-1].start)<opt_const.PRECISION || start>optSch[j-1].start)  && ( fabs(optSch[j].start-start)<opt_const.PRECISION || start<optSch[j].start))
			{
				startIndex=j-1;
				break;
			}
		}
		if(startIndex==0)
			startIndex=len-1;
		runTime=0;
		for(j=lastStatIndex+1;j<=startIndex;j++)
		{
			runTime=runTime+optSch[j].time;
		}
		//		printf("index=:%f,%d,%d\n",start,lastStatIndex+1,startIndex);
		for(j=lastStatIndex+1;j<=startIndex;j++)
		{
			final_curves[j].gear=optSch[j].gear;
			final_curves[j].velocity=optSch[j].velocity;
			final_curves[j].ford_time=runTime-lastTime;
			final_curves[j].ford_station_num=i;
			final_curves[j].time=optSch[j].time;
			final_curves[j].anti_stop_flag=NONE_FLAG;
			final_curves[j].manual_flag=NONE_FLAG;
			final_curves[j].con_tel_kp=optSch[j].start;
			lastTime=lastTime+optSch[j].time;
		}
		lastStatIndex=startIndex;
	}
	if(lastStatIndex+1<len) // 超出最后一个车站的点
	{
		for(j=lastStatIndex+1;j<len;j++)
		{
			final_curves[j].gear=optSch[j].gear;
			final_curves[j].velocity=optSch[j].velocity;
			final_curves[j].ford_time=-1;
			final_curves[j].ford_station_num=i;
			final_curves[j].time=optSch[j].time;
			final_curves[j].anti_stop_flag=NONE_FLAG;
			final_curves[j].manual_flag=NONE_FLAG;
			final_curves[j].con_tel_kp=optSch[j].start;
			lastTime=lastTime+optSch[j].time;
		}
	}
	//禁止坡停区域标识（车子或者车身属于陡上坡或者缓坡部分）
	i=0;
	float start,end;
	for(;i<MAX_ROADCATEGORY;i++)
	{
		if(rc[i].flag==2 || rc[i].flag==1)  //车头处于陡上坡或者缓坡区段
		{
			start=rc[i].start;
			end=rc[i].end+locoInfo.totalLength;
			int startIndex,endIndex;
			get_segment_index(optSch,start,end,&startIndex,&endIndex,len);
			for(j=startIndex;j<=endIndex;j++)
				final_curves[j].anti_stop_flag=FORBID_STOP_ON_GRAD;
		}
	}
	//禁止坡停区域标识(车子或者车身属于局部陡上坡小段)
	i=0;
	for(;i<MAX_MGRADIENT;i++)
	{
		if(mGradients[i].value > 3.0 )
		{
			start=mGradients[i].start;
			end=mGradients[i].end+locoInfo.totalLength;
			int startIndex,endIndex;
			get_segment_index(optSch,start,end,&startIndex,&endIndex,len);
			for(j=startIndex;j<=endIndex;j++)
			{
				if(final_curves[j].anti_stop_flag!=FORBID_STOP_ON_GRAD)
					final_curves[j].anti_stop_flag=FORBID_STOP_ON_GRAD;
			}
		}
	}
	//禁止夸坡停车区域标识(1、陡上坡转平坡、缓下坡  2、陡上坡转下坡（非陡坡和陡坡） 3、陡下坡、超陡下坡转陡上坡
	i=0;
	for(;i<MAX_ROADCATEGORY-1;i++)
	{
		if((rc[i].flag==2 && (rc[i+1].flag==0 || rc[i+1].flag==1)) || (rc[i].flag==2 && (rc[i+1].flag==-2 || rc[i+1].flag==-3)) ||  ((rc[i].flag==-2 || rc[i].flag==-3) && rc[i+1].flag==2))
		{
			start=rc[i+1].start;
			end=rc[i+1].start+locoInfo.totalLength;
			int startIndex,endIndex;
			get_segment_index(optSch,start,end,&startIndex,&endIndex,len);
			for(j=startIndex;j<=endIndex;j++)
			{
				final_curves[j].anti_stop_flag=FORBID_STOP_CROSS_GRAD;
			}
		}
	}
	//空气制动或者手动区域
	i=0;
	for(;i<len;i++)
	{
		if(optSch[i].gear==opt_const.AIRS)
			final_curves[j].manual_flag=AIR_DAMPING_BRAKE;
		else if(optSch[i].gear==opt_const.AIRE)
			final_curves[j].manual_flag=AIR_DAMPING_AVG;
		else if(optSch[i].gear==opt_const.PUSHERENGINE)
			final_curves[j].manual_flag=ADD_LOCO_AREA;
		else if(optSch[i].gear==opt_const.THROUGHEXPERT)
			final_curves[j].manual_flag=RUN_THROUGH_TEST;
	}
	//将公里标转换为信号机+越过距离形式
	struct tele_t *tele;
	int length;
	tele=(struct tele_t*)read_binary_file(0x04, &length);
	OPT_EXTR_RESULT file_ret;
	if(tele==NULL)
	{
		file_exception_decorator(OPT_OPEN_ERROR,&file_ret,"#post_process");
		return file_result_adapter(file_ret);
	}
	i=1;
	int lastIndex=-1;
	for(;i<length;i++)
	{
		float next_start=tele[i].cont_km_post;
		for(j=lastIndex+1;j<len;j++)
		{
			if(optSch[j].start< next_start)
			{
				final_curves[j].tel_num=tele[i-1].t_num;
				final_curves[j].tel_kp=tele[i-1].discont_km_post;
				final_curves[j].cross_dist=optSch[j].start-tele[i-1].cont_km_post;
			}
			else
				break;
		}
		lastIndex=j-1;
	}
	return ret;
}
