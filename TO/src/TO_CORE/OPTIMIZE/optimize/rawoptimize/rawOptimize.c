/**
 * @file     rawOptimize.c
 * @brief  原始优化计算函数实现
 * @date  2014.4.9
 * @author sinlly
 */
#include "opt_global_var.h"
#include "optimize_curve.h"
#include "rawOptimize.h"

int optSchIndex;
int MAX_OPTSCHLENGTH;

/**
 * @brief  优化策略匹配器
 * @param roadIndex   当前坡段的index,通过该index可以获得其前后段的段类型
 * @param totalWeight   车总重
 * @param length   坡长
 * @param avgV   坡段的平均速度
 * @param partGradFlag    是否在当前坡段中存在局部别的类型的坡段
 * @param downPect  坡段所在车站间的下坡所占的百分比
 * @param upPect  坡段所在的车站间的上坡所占的百分比
 * @return  优化策略匹配类型
 */
OPTIMIZE_RESULT rawStrategyMatcher(int roadIndex,float totalWeight,int length,float avgV, int partGradFlag,int downPect,int upPect);
/**
 * @brief 优化策略执行器
 *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @return 优化曲线结果
 */
OPTSCH* rawStrategyExecute(float vo, float v_interval, int count, int roadIndex,
		int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT* ret);
/**
 * @brief  优化曲线最后调整速度为司机的结束速度
 * @param vEnd   司机终止速度
 * @param optSch   优化曲线
 * @param index   优化曲线最后一个点的index
 * @return   调整之后的优化速度曲线
 */
OPTSCH*  adjustCurveEndVelocity(float vEnd,OPTSCH* optSch,int index,OPTIMIZE_RESULT *ret);

/**
 * @brief  原始优化主流程入口函数
 * @param massType   车重量
 * @param startPost   起始公里标
 * @param endPost    终止公里标
 * @param vStart    起始速度
 * @param vEnd     终止速度
 * @param ret   异常处理
 * @return   优化速度曲线
 */
OPTSCH* rawOptimize(int massType,int startPost, int endPost,float vStart,float vEnd,OPTIMIZE_RESULT *ret) {
	//异常处理描述
	char desc[EXP_DESC_LENGTH]="#rawOptimize";
	OPTIMIZE_RESULT fopt_ret;
	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;
	//	获取策略匹配矩阵

	opt_log_info("读取原始优化策略矩阵开始");
	OPT_EXTR_RESULT extr_ret;
	extr_ret=extrRawStrategyMetrics();
	if(extr_ret.file_result!=OPT_EXTR_SUCCESS)
	{
		strcat(desc,extr_ret.description);
		file_exception_decorator(extr_ret.file_result,&extr_ret,desc);
		fopt_ret=file_result_adapter(extr_ret);
//		ret=&fopt_ret;
		strcpy(ret->description,fopt_ret.description);
		ret->opt_rest=fopt_ret.opt_rest;
		return NULL;
	}
	opt_log_info("读取原始优化策略矩阵结束");

	// 初始化
	float avgV;
	float vo;
	float headX;
	float v_interval; //速度阈值
	int count;
	int index;
	int roadIndex;
	int gear;
	OPTSCH* optSch;
	int k;
	// 赋初值
	headX =(float) startPost;
	gear = 0;
	//	optSchIndex = 0;
	index = 0;
	optSch = (OPTSCH*) malloc(sizeof(OPTSCH));
	if(optSch==NULL)
	{
		RAW_OPTIMIZE_RESULT opt_ret;
		raw_opt_exception_decorator(RAW_MALLOC_ERROR,&opt_ret,desc);
		fopt_ret=raw_result_adapter(opt_ret);
//		ret=&fopt_ret;
		strcpy(ret->description,fopt_ret.description);
		ret->opt_rest=fopt_ret.opt_rest;
		return optSch;
	}
	optSch[index].start = headX;
	optSch[index].gear = gear;
	optSch[index].velocity = vStart;
	optSch[index].consumption = 0;
	optSch[index].time = 0;
	optSch[index].acc = 0;
	index = index + 1;
	optSchIndex = index;

	count = 0;
	int i;
	for (i = 0; i < MAX_MGRADIENT; i++) {
		if (((float)mGradients[i].start < headX || fabs((float)mGradients[i].start - headX)) && (float)mGradients[i].end > headX) { //此处较原matlab程序有改动，>=改成了>
			count = i;
			break;
		}
	}
	int categoryCount = 0;
	for( i = 0;i<MAX_ROADCATEGORY;i++)
	{
		if(((float)rc[i].start < headX || fabs((float)rc[i].start - headX)<opt_const.PRECISION) && ((float)rc[i].end > headX || fabs((float)rc[i].end - headX)<opt_const.PRECISION))
		{
			categoryCount = i;
			break;
		}
	}
	v_interval = 1.0;
	// 各路段优化计算
	for (i = categoryCount; i < MAX_ROADCATEGORY; i++) {
		//取當前路段狀態信息并執行當前路段計算
		if(rc[i].flag==opt_const.PUSHERENGINE || rc[i].flag==opt_const.THROUGHEXPERT)
		{
			int index = optSchIndex - 1;
			float start,v,end,s,delta_v,delta_t,delta_e;
			start=optSch[index-1].start;
			v = optSch[index-1].velocity;
			float srcV=v;
			float srcStart=start;
			end =(float) rc[i].end;
			s=start+opt_const.SSTEP;
			float vo=rc[i].vo;
			for(;s<end;s+=opt_const.SSTEP)
			{
				delta_v=srcV-(s-srcStart)*(srcV-vo)/(rc[i].end-srcStart);
				delta_t=opt_const.SSTEP*3.6/delta_v;
				delta_e=getConsumption(0,delta_t);

				gear=rc[i].flag;
				optSch = (OPTSCH*) realloc(optSch, (index + 1) * sizeof(OPTSCH));
				if(optSch==NULL)
				{
					strcat(desc,fopt_ret.description);
					optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//					ret=&fopt_ret;
					strcpy(ret->description,fopt_ret.description);
					ret->opt_rest=fopt_ret.opt_rest;
					return optSch;
				}
				optSch[index].start = s;
				optSch[index].gear = gear;
				optSch[index].velocity = delta_v;
				optSch[index].consumption = delta_e;
				optSch[index].time = delta_t;
				optSch[index].acc = (delta_v -optSch[index-1].velocity)/ delta_t;
				index=index+1;
			}
			s=s-opt_const.SSTEP;
			delta_v=srcV-(s-srcStart)*(srcV-vo)/(rc[i].end-srcStart);
			delta_t=(end-s)*3.6/delta_v;
			delta_e=getConsumption(0,delta_t);
			optSch = (OPTSCH*) realloc(optSch, (index + 1) * sizeof(OPTSCH));
			if(optSch==NULL)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description,fopt_ret.description);
				ret->opt_rest=fopt_ret.opt_rest;
				return optSch;
			}
			optSch[index].start = end;
			optSch[index].gear = gear;
			optSch[index].velocity = delta_v;
			optSch[index].consumption = delta_e;
			optSch[index].time = delta_t;
			optSch[index].acc =  (delta_v -optSch[index-1].velocity) / delta_t;
			index=index+1;
			optSchIndex=index;
		}
		else
		{
			int partGradFlag=0;
			//        if(rc[i].partGradNum!=0)
			//        	partGradFlag=1;
			int k=0;
			int  downPect=0;
			int  upPect=0;
			for(;k<MAX_INTERVALRUNTIME;k++)   //找到当前坡段所在的车站之间的上坡和下坡的百分比。以此来决定需要多考虑哪些因素来制定策略
			{
				int startCzh=intervalRuntimeUnits[k].startCzh;
				int endCzh=intervalRuntimeUnits[k].endCzh;
				int m=0;
				int startGlb=0;
				int endGlb=0;
				int czhIndex;
				for(;m<MAX_STATION;m++)
				{
					if(stations[m].id==startCzh)
					{
						czhIndex=m;
						break;
					}
				}
				startGlb=stations[czhIndex].start;
				endGlb=stations[czhIndex+1].start;
				if(rc[i].start>=startGlb && rc[i].end<=endGlb) //若坡段完全在 两个相邻车站之间
				{
					downPect=intervalRuntimeUnits[k].steepDownPect;
					upPect=intervalRuntimeUnits[k].steepUpPect;
					break;
				}
				if(startGlb>=rc[i].start && startGlb<=rc[i].end)  //若坡段间有一个车站
				{
					if(startGlb-rc[i].start >=rc[i].end-startGlb  && k!=0)//坡段首距离车站较远，且该车站不属于第一个车站
					{
						downPect=intervalRuntimeUnits[k-1].steepDownPect;
						upPect=intervalRuntimeUnits[k-1].steepUpPect;
						break;
					}
					else if((startGlb-rc[i].start >=rc[i].end-startGlb  && k==0) || (startGlb-rc[i].start<rc[i].end-startGlb))//坡段首距离车站较远，且该车站属于第一个车站或坡段尾距离车站较远
					{
						downPect=intervalRuntimeUnits[k].steepDownPect;
						upPect=intervalRuntimeUnits[k].steepUpPect;
						break;
					}

				}
			}
			fopt_ret= rawStrategyMatcher(i,locoInfo.totalWeight,rc[i].length,rc[i].avgV,partGradFlag,downPect,upPect);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description,fopt_ret.description);
				ret->opt_rest=fopt_ret.opt_rest;
				return optSch;
			}
			if (fopt_ret.opt_rest== OPTIMIZE_SUCCESS) {
				avgV = rc[i].avgV;
				//定义该段建议出段速度vo
				if (i == MAX_ROADCATEGORY - 1) {
					vo = rc[MAX_ROADCATEGORY - 1].vo;
				} else {
					if (rc[i + 1].flag == -3)
						vo = min(rc[i].vo, rc[i + 1].vi);
					else if (mean(rc[i].vo,rc[i+1].vi) >= avgV)
						vo = max(rc[i].vo, rc[i + 1].vi);
					else
						vo = min(rc[i].vo, rc[i + 1].vi);
				}
				//执行程序调用
				roadIndex = i;
				index = optSchIndex;
				optSch = rawStrategyExecute(vo, v_interval, count, roadIndex, index,optSch, avgV,&fopt_ret);
				if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,fopt_ret.description);
					optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//					ret=&fopt_ret;
					strcpy(ret->description,fopt_ret.description);
					ret->opt_rest=fopt_ret.opt_rest;
					return optSch;
				}
			}
		}
		index = optSchIndex;
		if (rc[i].end >= endPost) {
			index = index - 1;
			while (index > 0 && (optSch[index].start > (float) endPost || fabs(optSch[index].start - (float) endPost)<opt_const.PRECISION)) {
				index = index - 1;
			}
			break;
		}
	}
	MAX_OPTSCHLENGTH=index+1;
//	ret=&fopt_ret;
	strcpy(ret->description,fopt_ret.description);
	ret->opt_rest=fopt_ret.opt_rest;
	//保证末尾的速度为需要的速度
	//	optSch=adjustCurveEndVelocity(vEnd,optSch,index);
	return optSch;
}
/**
 * @brief  优化曲线最后调整速度为司机的结束速度
 * @param vEnd   司机终止速度
 * @param optSch   优化曲线
 * @param index   优化曲线最后一个点的index
 * @return   调整之后的优化速度曲线
 */
OPTSCH*  adjustCurveEndVelocity(float vEnd,OPTSCH* optSch,int index,OPTIMIZE_RESULT *ret)
{
	//异常处理标识符
	char desc[EXP_DESC_LENGTH]="#adjustCurveEndVelocity";
	RAW_OPTIMIZE_RESULT opt_ret;
	TRAC_CAL_RESULT trac_ret;
	OPTIMIZE_RESULT fopt_ret;
	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;

	if(vEnd<optSch[index].velocity)
	{
		float headX=optSch[MAX_OPTSCHLENGTH-1].start;
		float v=vEnd;
		int count=1;
		int gear=0; //以缓坡档位原有档位牵引追上曲线
		float s=headX-optSch[MAX_OPTSCHLENGTH-2].start;
		float delta_t,delta_v,delta_e;
		while(gear>opt_const.MINGEAR)
		{
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				fopt_ret=trac_result_adapter(trac_ret);
//				ret=&fopt_ret;
				strcpy(ret->description,fopt_ret.description);
				ret->opt_rest = fopt_ret.opt_rest;
				return optSch;
			}
			if(delta_v<0)
				break;
			gear=gear-1;
		}
		index=MAX_OPTSCHLENGTH-2;
		optSch[index+1].velocity=v;
		optSch[index+1].gear=gear;
		optSch[index+1].time=delta_t;
		optSch[index+1].consumption=delta_e;
		optSch[index+1].acc=delta_v/delta_t;
		int index_back=-1;
		OPTSCH* optBack;
		while(1)
		{

			s=headX-optSch[index].start;
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				fopt_ret=trac_result_adapter(trac_ret);
//				ret=&fopt_ret;
				strcpy(ret->description,fopt_ret.description);
				ret->opt_rest = fopt_ret.opt_rest;
				return optSch;
			}
			v=v-delta_v;
			headX=headX-s;
			if((headX-optSch[MAX_OPTSCHLENGTH].start> 2000.0 || fabs(headX-optSch[MAX_OPTSCHLENGTH].start-2000.0)<opt_const.PRECISION) && gear!=opt_const.MINGEAR)//若超出2000米，尚未追上，则更换档位重头开始
			{
				gear=gear-1;
				headX=optSch[MAX_OPTSCHLENGTH-1].start;
				v=vEnd;
				index=MAX_OPTSCHLENGTH-2;
				if(index_back!=-1)
					free(optBack);
				index_back=-1;
				continue;
			}
			if(v>optSch[index].velocity)
				break;
			index_back=index_back+1;
			if(index_back==0)
				optBack=(OPTSCH*) malloc (sizeof(OPTSCH));
			else
				optBack=(OPTSCH*) realloc(optBack,sizeof(OPTSCH)*(index_back+1));

			if(optBack==NULL)
			{
				raw_opt_exception_decorator(RAW_MALLOC_ERROR,&opt_ret,desc);
				fopt_ret=raw_result_adapter(opt_ret);
//				ret=&fopt_ret;
				strcpy(ret->description,fopt_ret.description);
				ret->opt_rest = fopt_ret.opt_rest;
				return optSch;
			}

			optBack[index_back].start=headX;
			optBack[index_back].gear=gear;
			optBack[index_back].velocity=v;
			optBack[index_back].consumption=delta_e;
			optBack[index_back].time=delta_t;
			optBack[index_back].acc=delta_v/delta_t;
			index=index-1;
		}
		index=MAX_OPTSCHLENGTH-2;
		if(index_back!=-1)
		{
			int i;
			for(i=0;i<=index_back;i++)
			{
				optSch[index].start=optBack[i].start;
				optSch[index].gear=optBack[i].gear;
				optSch[index].velocity=optBack[i].velocity;
				optSch[index].consumption=optBack[i].consumption;
				optSch[index].time=optBack[i].time;
				optSch[index].acc=optBack[i].acc;
				index=index-1;
			}
			free(optBack);
		}
	}
	else
	{
		float headX=optSch[MAX_OPTSCHLENGTH-1].start;
		float v=vEnd;
		int count=1;
		int gear=0; //以缓坡档位原有档位牵引追上曲线
		float s=headX-optSch[MAX_OPTSCHLENGTH-2].start;
		float delta_t,delta_v,delta_e;
		while(gear<opt_const.MAXGEAR)
		{
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				fopt_ret=trac_result_adapter(trac_ret);
//				ret=&fopt_ret;
				strcpy(ret->description,fopt_ret.description);
				ret->opt_rest = fopt_ret.opt_rest;
				return optSch;
			}
			if(delta_v>0)
				break;
			gear=gear+1;
		}
		index=MAX_OPTSCHLENGTH-2;
		optSch[index+1].velocity=v;
		optSch[index+1].gear=gear;
		optSch[index+1].time=delta_t;
		optSch[index+1].consumption=delta_e;
		optSch[index+1].acc=delta_v/delta_t;
		int index_back=-1;
		OPTSCH* optBack;
		while(1)
		{

			s=headX-optSch[index].start;
			trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				fopt_ret=trac_result_adapter(trac_ret);
//				ret=&fopt_ret;
				strcpy(ret->description,fopt_ret.description);
				ret->opt_rest = fopt_ret.opt_rest;
				return optSch;
			}
			while(gear!=opt_const.MAXGEAR)
			{
				if(delta_v>0)
					break;
				else
				{
					gear=gear+1;
					if(gear>opt_const.MAXGEAR)
						gear=opt_const.MAXGEAR;
					trac_ret=DoCaculateByDistance(headX,v,gear,&delta_t,&count,s,&delta_v,&delta_e);
					if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
					{
						strcat(desc,trac_ret.description);
						trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
						fopt_ret=trac_result_adapter(trac_ret);
//						ret=&fopt_ret;
						strcpy(ret->description,fopt_ret.description);
						ret->opt_rest = fopt_ret.opt_rest;
						return optSch;
					}
				}
			}
			v=v-delta_v;
			headX=headX-s;
			if((headX-optSch[MAX_OPTSCHLENGTH].start> 2000.0 || fabs(headX-optSch[MAX_OPTSCHLENGTH].start- 2000.0)<opt_const.PRECISION)  && gear!=opt_const.MAXGEAR)//若超出2000米，尚未追上，则更换档位重头开始
			{
				gear=gear+1;
				headX=optSch[MAX_OPTSCHLENGTH-1].start;
				v=vEnd;
				index=MAX_OPTSCHLENGTH-2;
				if(index_back!=-1)
					free(optBack);
				index_back=-1;
				continue;
			}
			if(v<optSch[index].velocity)
				break;
			index_back=index_back+1;
			if(index_back==0)
				optBack=(OPTSCH*) malloc (sizeof(OPTSCH));
			else
				optBack=(OPTSCH*) realloc(optBack,sizeof(OPTSCH)*(index_back+1));
			if(optBack==NULL)
			{
				raw_opt_exception_decorator(RAW_MALLOC_ERROR,&opt_ret,desc);
				fopt_ret=raw_result_adapter(opt_ret);
//				ret=&fopt_ret;
				strcpy(ret->description,fopt_ret.description);
				ret->opt_rest = fopt_ret.opt_rest;
				return optSch;
			}

			optBack[index_back].start=headX;
			optBack[index_back].gear=gear;
			optBack[index_back].velocity=v;
			optBack[index_back].consumption=delta_e;
			optBack[index_back].time=delta_t;
			optBack[index_back].acc=delta_v/delta_t;
			index=index-1;
		}
		index=MAX_OPTSCHLENGTH-2;
		if(index_back!=-1)
		{
			int i;
			for(i=0;i<=index_back;i++)
			{
				optSch[index].start=optBack[i].start;
				optSch[index].gear=optBack[i].gear;
				optSch[index].velocity=optBack[i].velocity;
				optSch[index].consumption=optBack[i].consumption;
				optSch[index].time=optBack[i].time;
				optSch[index].acc=optBack[i].acc;
				index=index-1;
			}
			free(optBack);
		}
	}
//	ret=&fopt_ret;
	strcpy(ret->description,fopt_ret.description);
	ret->opt_rest = fopt_ret.opt_rest;
	return optSch;
}
/**
 * @brief 存储优化结果
 * @param optSch  优化计算曲线
 */
void saveoptSch(OPTSCH* optSch){
	FILE* fid2 = fopen("././saveoptSch","w");
	if(fid2==NULL){
		fprintf(stderr,"open curve err!\n");
	}
	int i;
	for(i=0;i<MAX_OPTSCHLENGTH;i++)
	{
		fprintf(fid2,"%f\t%f\t%d\n",optSch[i].start,optSch[i].velocity,optSch[i].gear);
	}
	fclose(fid2);
}
/**
 * @brief  优化策略匹配器
 * @param flag   坡段类型
 * @param totalWeight   车总重
 * @param length   坡长
 * @param avgV   坡段的平均速度
 * @param partGradFlag    是否在当前坡段中存在局部别的类型的坡段
 * @param downPect  坡段所在车站间的下坡所占的百分比
 * @param upPect  坡段所在的车站间的上坡所占的百分比
 * @return  优化策略匹配类型
 */
OPTIMIZE_RESULT rawStrategyMatcher(int roadIndex,float totalWeight,int length,float avgV, int partGradFlag,int downPect,int upPect) {
	int i;
	char* name;
	//异常处理描述
	//异常处理描述
	char  tmp[EXP_DESC_LENGTH];
	sprintf(tmp,"%d",roadIndex);
	char desc[EXP_DESC_LENGTH]="#rawStrategyMatcher;rcIndex=";
	strcat(desc,tmp);
	RAW_OPTIMIZE_RESULT  opt_ret;
	opt_ret.rawopt_result=RAW_OPTIMIZE_SUCCESS;

	int lastRoadFlag,nextRoadFlag;  //上一段和下一段的标记，如果上一段不是缓下坡段则标记为100，是则为缓下坡标记。如果下一段不是缓下坡或者陡下坡，或者超陡下坡，则标记为100,若是则标记为对应的坡段标识
	lastRoadFlag=opt_const.FEATUREINVALID;
	nextRoadFlag= opt_const.FEATUREINVALID;
	if(totalWeight>=4000)
	{
		if(rc[roadIndex].flag ==-1 || rc[roadIndex].flag== 1 || rc[roadIndex].flag==0)
		{
			if(roadIndex!=0  && rc[roadIndex-1].flag==-1 && roadIndex!=MAX_ROADCATEGORY-1 && rc[roadIndex+1].flag ==-1)
			{
				lastRoadFlag=rc[roadIndex-1].flag;
				nextRoadFlag=rc[roadIndex+1].flag;
			}
			else
			{
				if(roadIndex!=MAX_ROADCATEGORY-1 && (  rc[roadIndex+1].flag==-2 || rc[roadIndex+1].flag==-3))
					nextRoadFlag=rc[roadIndex+1].flag;
				else
				{
					if(roadIndex!=MAX_ROADCATEGORY-1 && (  rc[roadIndex+1].flag==2))
						nextRoadFlag=rc[roadIndex+1].flag;
				}
			}
		}
	}
	for (i = 0; i < MAX_RAWSTRATEGY; i++) {
		if(rc[roadIndex].flag==rawStrategyMetrics[i].gradientType && (int)totalWeight>=rawStrategyMetrics[i].minMass && (int)totalWeight<=rawStrategyMetrics[i].maxMass
				&& length>=rawStrategyMetrics[i].minLen && length <= rawStrategyMetrics[i].maxLen && (avgV > rawStrategyMetrics[i].minAvg || fabs(avgV - rawStrategyMetrics[i].minAvg)<opt_const.PRECISION)
				&& (avgV < rawStrategyMetrics[i].maxAvg || fabs(avgV - rawStrategyMetrics[i].maxAvg)<opt_const.PRECISION) && partGradFlag == rawStrategyMetrics[i].partGradFlag && downPect>=rawStrategyMetrics[i].minSteepDownPect
				&& downPect<=rawStrategyMetrics[i].maxSteepDownPect && upPect >= rawStrategyMetrics[i].minSteepUpPect  && upPect <= rawStrategyMetrics[i].maxSteepUpPect
				&& lastRoadFlag==rawStrategyMetrics[i].lastGraType && nextRoadFlag==rawStrategyMetrics[i].nextGraType)
		{
			name=rawStrategyMetrics[i].name;
			break;
		}
	}

	if(name==NULL)
	{
		raw_opt_exception_decorator(RAW_NOT_FIND_STRATEGY,&opt_ret,desc);
		return raw_result_adapter(opt_ret);
	}
	opt_log_info("原始优化策略: %s ",name);
	if(strcmp(name,"sup_steep_dwn_strgy")==0)
	{
		rawStrategy = &sup_steep_dwn_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"steep_up_strgy")==0)
	{
		rawStrategy = &steep_up_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"steep_dwn_strgy")==0)
	{
		rawStrategy = &steep_dwn_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"gentle_hill_strgy")==0)
	{
		rawStrategy = &gentle_hill_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"empty_steep_dwn_strgy")==0)
	{
		rawStrategy = &empty_steep_dwn_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"steep_dwn_more_gentle_hill_strgy")==0)
	{
		rawStrategy = &steep_dwn_more_gentle_hill_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"empty_steep_up_strgy")==0)
	{
		rawStrategy = &empty_steep_up_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"last_dwn_next_dwn_gentle_hill_strgy")==0)
	{
		rawStrategy= &last_dwn_next_dwn_gentle_hill_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"next_steep_dwn_gentle_hill_strgy")==0)
	{
		rawStrategy= &next_steep_dwn_gentle_hill_strgy;
		return raw_result_adapter(opt_ret);
	}
	if(strcmp(name,"next_steep_up_gentle_hill_strgy")==0)
	{
		rawStrategy=&next_steep_up_gentle_hill_strgy;
		return raw_result_adapter(opt_ret);
	}
	raw_opt_exception_decorator(RAW_NOT_FIND_STRATEGY,&opt_ret,desc);
	return raw_result_adapter(opt_ret);
}
/**
 * @brief 优化策略执行器
 *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @return 优化曲线结果
 */
OPTSCH* rawStrategyExecute(float vo, float  v_interval, int count, int roadIndex,
		int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT* ret) {
	optSch = rawStrategy(vo, v_interval, count, roadIndex, index, optSch, avgV,ret);
	return optSch;
}
