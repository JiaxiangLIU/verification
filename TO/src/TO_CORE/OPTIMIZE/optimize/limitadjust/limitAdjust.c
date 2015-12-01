/**
 * @file    limitAdjust.c
 * @brief  限速调整函数实现
 * @date  2014.4.8
 * @author danny
 * @note   限速调整整体流程
 */
#include "limitAdjust.h"

/**
 * @brief 获得限速类型
 * @param flagS  前一个坡段的类型
 * @param flagE  后一个坡段的类型
 * @param type  获得限速策略类型,若没找到则返回-1（这种情况几乎不存在）
 * @return 优化异常结果
 */
OPTIMIZE_RESULT getLimitStrategyType(int flagS,int flagE,int* type);
/**
 * @brief  限速调整主流程函数
 * @param optsch   优化速度曲线
 * @param len 优化速度曲线长度
 * @param startPost   起始公里标
 * @param endPost   终止公里标
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitCategory(OPTSCH* optsch,int len, int startPost,int endPost);
/**
 * @brief 限速策略匹配器
 * @param flag     限速策略类型标识
 * @param totalWeight      车重
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategyMatcher(int flag,float totalWeight);
/**
 * @brief  限速策略执行器
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速（手动区域与正常的坡段类型处理方式也属于长大限速处理方式）
 * @return  优化异常结果
 */
OPTIMIZE_RESULT  limitStrategyExecute(OPTSCH* optsch,int len, LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart);
/**
 * @brief 限速策略包装（包括获得限速策略类型，根据类型匹配策略，执行策略三个步骤）
 * @param optsch   原始优化速度曲线
 * @param len   原始优化速度曲线的长度
 * @param start   当前限速的起始公里标
 * @param ending  当前限速的终止公里标
 * @param value  限速阈值
 * @param flagS  限速起始位置所在的段类型
 * @param flagE  限速终止位置所在的段类型
 * @param indexS  限速起始位置所在的段中的index
 * @param indexE  限速终止位置所在的段中的index
 * @param index1  限速起始位置所在的原始优化曲线中的index
 * @param index2  限速终止位置所在的原始优化曲线中的index
 * @param flag   是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag 是否属于长大限速（手动区域与正常的坡段类型处理方式也属于长大限速处理方式）
 * @return 优化异常结果
 */
OPTIMIZE_RESULT limitManagement(OPTSCH* optsch,int len,float start,float srcStart,float ending,float value, int flagS,int flagE,int indexS,int indexE,int index1,int index2,int flag,int longflag);
/**
 *@brief  优化限速策略处理主入口函数
 *@param  optSch  原始优化曲线
 *@param  len 原始优化曲线的长度
 *@param  startPost 进行限速调整的起始位置
 *@param  endPost  进行限速调整的终止位置
 *@return 异常处理结果
 */
OPTIMIZE_RESULT  limitAdjust(OPTSCH* optsch,int len,int startPost,int endPost)
{
	char desc[EXP_DESC_LENGTH]="#limitAdjust";
	OPT_EXTR_RESULT extr_ret;
	OPTIMIZE_RESULT ret;
	opt_log_info("读取限速策略矩阵开始");
	extr_ret=extrLimitStrategyMetrics();
	if(extr_ret.file_result!=OPT_EXTR_SUCCESS)
	{
		strcat(desc,extr_ret.description);
		file_exception_decorator(extr_ret.file_result,&extr_ret,desc);
		return file_result_adapter(extr_ret);
	}
	opt_log_info("读取限速策略矩阵开始");

	ret=limitCategory(optsch,len,startPost,endPost);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		return ret;
	}
	return ret;
}

/**
 * @brief  临时限速处理
 * @param optSch 上一次的原始优化结果（若出现多次临时限速处理则为上一次原始结果，若为第一次，则为第一次的优化结果）
 * @param len 优化速度曲线长度
 * @param limitStart 临时限速的起始公里标
 * @param limitEnd 临时限速的终止公里标
 * @param value 临时限速值
 * @param limitIndexS  临时限速起始位置在optSch中的index
 * @param limitIndexE  临时限速终止位置在optSch中的index
 * @return 异常处理结果
 */
OPTIMIZE_RESULT  tempLimitAdjust(OPTSCH* optsch,int len, int limitStart,int limitEnd,int value,int *limitIndexS,int *limitIndexE)
{
	//	float tstep = opt_const.TSTEP;
	int count = 1;
	int i;
	int start;
	int  ending;
	float v1;
	float v2;
	int index1;
	int index2;
	int flag;
	int indexTemp;
	int j;
	int flagS,indexS,flagE,indexE,longflag;
	int flagS1,indexS1,flagE1,indexE1;
	int startTemp,endTemp;
	float v11,v21;
	int index11,index21;
	int Type;
	LIMITINFO limitInfo;
	int srcStart;
	//异常处理标识符
	char  stmp[EXP_DESC_LENGTH];
	sprintf(stmp,"%d",limitStart);
	char etmp[EXP_DESC_LENGTH];
	sprintf(etmp,"%d",limitEnd);
	char desc[EXP_DESC_LENGTH]="#tempLimitAdjust;start=";
	strcat(desc,stmp);
	char tmp[EXP_DESC_LENGTH]=";end=";
	strcat(desc,tmp);
	strcat(desc,etmp);
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;

	start=limitStart;
	ending=limitEnd+(int)locoInfo.totalLength;

	/**
	 * 在这里需要加入判断，是否存在当一个限速刚刚结束（车尾刚过终止公里标），然后在终止公里标处又出现了一个更低的限速值。。。
	 * 同时需要考虑临时限速中的判断问题，在限速中找到相应的临时限速
	 * */
	for(i=0; i<MAX_LIMIT;i++) {
		if(limits[i].start<limitEnd<limits[i].end && value> limits[i].limit) {
			ending= limitEnd;
//			printf("limit: %d\n",i);
		}
	}

	if(start<(int)(optsch[0].start))
		start=(int)(optsch[0].start);
	if(ending>(int)(optsch[len-1].start))
		ending=(int)(optsch[len-1].start);
	srcStart = start;
	value=value-opt_const.LIMIT_INTERVAL;
	getSchVelocity(optsch,len, (float)start,(float)ending,&v1,&index1,&v2,&index2);

	flag = 0;
	indexTemp = index1;
	if(v1< (float)value || fabs(v1-(float)value)<opt_const.PRECISION)
	{
		for(j=indexTemp;j<len;j++)
		{
			if(optsch[j].start < (float)ending && optsch[j].velocity>(float)value)
			{
				v1 = optsch[j].velocity;
				flag = 1;
				index1 = j;
				start = (int)optsch[j].start;
				break;
			}
		}
	}
	indexTemp = index2;
	if(v2<(float)value || fabs(v2-(float)value)<opt_const.PRECISION)
	{
		j = indexTemp;
		while (1)
		{
			if (j<0)
				break;
			if(optsch[j].start < (float)ending && optsch[j].start > (float)start
					&& optsch[j].velocity > (float)value)
			{
				v2 = optsch[j].velocity;
				ending = (int)optsch[j].start;
				index2 = j;
				break;
			}
			j--;
		}
	}
	*limitIndexS=index1;
	*limitIndexE=index2;
	if ((v1<(float)value || fabs(v1-(float)value)<opt_const.PRECISION) && (v2 < (float) value|| fabs(v2 - (float) value)<opt_const.PRECISION))
		return ret;
	getCategoryFlag(start,ending,&flagS,&indexS,&flagE,&indexE);
	longflag = -2;
	startTemp = start;
	endTemp = ending;
	int k;
	/*
	 *
	 */
	if (indexS != indexE && (indexE-indexS)!=1){
		for(k=indexS;k<=indexE;k++)
		{
			if(k==indexS)  //长大限速的第一段
			{
				endTemp = rc[k].end;
				longflag = -1;
			}
			else if(k==indexE) //长大限速的最后一段
			{
				startTemp = rc[k].start;
				srcStart = startTemp;
				endTemp = ending;
				longflag = 1;
			}
			else   //长大限速的中间段
			{
				startTemp = endTemp;
				srcStart = startTemp;
				endTemp = rc[k].end;
				longflag = 0;
			}
			getCategoryFlag(startTemp,endTemp,&flagS1,&indexS1,&flagE1,&indexE1);
			getSchVelocity(optsch,len,(float)startTemp,(float)endTemp,&v11,&index11,&v21,&index21);
			ret=limitManagement(optsch, len,(float)startTemp,(float)srcStart,(float)endTemp, value, flagS1,flagE1,indexS1,indexE1,index11,index21,flag,longflag);
			if(ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,ret.description);
				optimize_exception_decorator(ret.opt_rest,&ret,desc);
				return ret;
			}
		}
	}
	else
	{
		if((flagS==opt_const.THROUGHEXPERT &&  flagE!=opt_const.THROUGHEXPERT) || (flagS==opt_const.PUSHERENGINE && flagE!=opt_const.PUSHERENGINE))  //左边在手动区域，右边不在手动区域
		{
			for(k=0;k<2;k++)
			{
				if(k==0)  //手动区域部分
				{
					startTemp=start;
					srcStart = startTemp;
					endTemp = rc[indexS].end;
					indexS1=indexS;
					indexE1=indexS;
					flagS1=flagS;
					flagE1=flagS;
					longflag=-2;
				}
				else //非手动区域部分
				{
					startTemp = rc[indexE].start;
					srcStart = startTemp;
					endTemp = ending;
					indexS1=indexE;
					indexE1=indexE;
					flagS1=flagE;
					flagE1=flagE;
					longflag = 1;
				}
				getSchVelocity(optsch,len,(float)startTemp,(float)endTemp,&v11,&index11,&v21,&index21);
				ret=limitManagement(optsch, len,(float)startTemp,(float)srcStart,(float)endTemp, value, flagS1,flagE1,indexS1,indexE1,index11,index21,flag,longflag);
				if(ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,ret.description);
					optimize_exception_decorator(ret.opt_rest,&ret,desc);
					return ret;
				}
			}
		}
		else if((flagS!=opt_const.THROUGHEXPERT &&  flagE==opt_const.THROUGHEXPERT) || (flagS!=opt_const.PUSHERENGINE && flagE==opt_const.PUSHERENGINE)) //左边非手动区域，右边在手动区域
		{
			for(k=0;k<2;k++)
			{
				if(k==0)  //非手动区域
				{
					startTemp = start;
					srcStart = startTemp;
					endTemp = rc[indexS].end;
					indexS1=indexS;
					indexE1=indexS;
					flagS1=flagS;
					flagE1=flagS;
					longflag = -1;
				}
				else //手动区域部分
				{
					startTemp = rc[indexE].start;
					srcStart = startTemp;
					endTemp = ending;
					indexS1=indexE;
					indexE1=indexE;
					flagS1=flagE;
					flagE1=flagE;
					longflag=-2;
				}
				getSchVelocity(optsch,len,(float)startTemp,(float)endTemp,&v11,&index11,&v21,&index21);
				ret=limitManagement(optsch, len,(float)startTemp,(float)srcStart,(float)endTemp, value, flagS1,flagE1,indexS1,indexE1,index11,index21,flag,longflag);
				if(ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,ret.description);
					optimize_exception_decorator(ret.opt_rest,&ret,desc);
					return ret;
				}
			}
		}
		else
		{
			ret=limitManagement(optsch, len,(float)start,(float)srcStart,(float)ending, value, flagS,flagE,indexS,indexE,index1,index2,flag,longflag);
			if(ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,ret.description);
				optimize_exception_decorator(ret.opt_rest,&ret,desc);
				return ret;
			}
		}
	}
	return ret;
}
/**
 * @brief  限速调整主流程函数
 * @param optsch   优化速度曲线
 * @param len 优化速度曲线长度
 * @param startPost   起始公里标
 * @param endPost   终止公里标
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitCategory(OPTSCH* optsch,int len, int startPost,int endPost)
{
	//	float tstep = opt_const.TSTEP;
	int count = 1;
	int i;
	int start;
	int  ending;
	float value;
	float v1;
	float v2;
	int index1;
	int index2;
	int flag;
	int indexTemp;
	int j,k;
	int flagS,indexS,flagE,indexE,longflag;
	int flagS1,indexS1,flagE1,indexE1;
	int startTemp,endTemp;
	float v11,v21;
	int index11,index21;
	int Type;
	int srcStart;	//
	LIMITINFO limitInfo;
	//异常处理标识符
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	for (i = 0;i < MAX_LIMIT;i++)
	{
//		if(i ==20) {
//			printf("111111111\n");
//		}

		//异常处理部分描述（第几个限速）
		char desc[EXP_DESC_LENGTH]="#limitCategory;limitIndex=";
		char tmp[EXP_DESC_LENGTH];
		sprintf(tmp,"%d",i);
		strcat(desc,tmp);

		start =  limits[i].start; // - opt_const.LIMITTHRESHOLD; --减去限速的阈值380,暂时不需要处理
		srcStart = start;
		ending =limits[i].end + (int)locoInfo.totalLength;

		/**
		 * 在这里需要加入判断，是否存在当一个限速刚刚结束（车尾刚过终止公里标），然后在终止公里标处又出现了一个更低的限速值。。。
		 * 同时需要考虑临时限速中的判断问题，在限速中找到相应的临时限速
		 * */
		if(limits[i+1].limit<limits[i].limit) {
			ending = limits[i].end ;
			//printf("limit: %d\n",i);
		}

		if(ending<startPost)
			continue;
		else
		{
			if(start<startPost)
				start=startPost;
		}
		if(ending > endPost)
		{
			if(start<endPost)
				ending=endPost;
			else
				break;
		}
		value =(float)( limits[i].limit - opt_const.LIMIT_INTERVAL);
		getSchVelocity(optsch,len,(float)start,(float)ending,&v1,&index1,&v2,&index2);
		flag = 0;
		indexTemp = index1;
		if(v1<value || fabs(v1-value)<opt_const.PRECISION)
		{
			for(j=indexTemp;j<len;j++)
			{
				if(optsch[j].start < (float)ending && optsch[j].velocity>(float)value)
				{
					v1 = optsch[j].velocity;
					flag = 1;
					index1 = j;
					start = (int)optsch[j].start;
					break;
				}
			}
		}
		indexTemp = index2;
		if(v2<value || fabs(v2-value)<opt_const.PRECISION)
		{
			j = indexTemp;
			while (1)
			{
				if (j<0)
					break;
				if(optsch[j].start < (float)ending && optsch[j].start > (float)start
						&& optsch[j].velocity > value)
				{
					v2 = optsch[j].velocity;
					ending = (int)optsch[j].start;
					index2 = j;
					break;
				}
				j--;
			}
		}

		if ((v1<value|| fabs(v1-value)<opt_const.PRECISION) && (v2 < value || fabs(v2 - value)<opt_const.PRECISION))
			continue;
		getCategoryFlag(start,ending,&flagS,&indexS,&flagE,&indexE);
		longflag = -2;
		startTemp = start;
		endTemp = ending;

		/*
		 *
		 */
		if (indexS != indexE && (indexE-indexS)!=1)
		{
			for(k=indexS;k<=indexE;k++)
			{
				if(k==indexS)
				{
					endTemp = rc[k].end;
					longflag = -1;
				}
				else if(k==indexE)
				{
					startTemp = rc[k].start;
					srcStart = startTemp;
					endTemp = ending;
					longflag = 1;
				}
				else
				{
					startTemp = endTemp;
					srcStart = startTemp;
					endTemp = rc[k].end;
					longflag = 0;
				}
				getCategoryFlag(startTemp,endTemp,&flagS1,&indexS1,&flagE1,&indexE1);
				getSchVelocity(optsch,len,(float)startTemp,(float)endTemp,&v11,&index11,&v21,&index21);
				ret=limitManagement(optsch, len,(float)startTemp,(float)srcStart,(float)endTemp, value, flagS1,flagE1,indexS1,indexE1,index11,index21,flag,longflag);
				if(ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,ret.description);
					optimize_exception_decorator(ret.opt_rest,&ret,desc);
					return ret;
				}
			}
		}
		else
		{
			if((flagS==opt_const.THROUGHEXPERT &&  flagE!=opt_const.THROUGHEXPERT) || (flagS==opt_const.PUSHERENGINE && flagE!=opt_const.PUSHERENGINE))  //左边在手动区域，右边不在手动区域
			{
				for(k=0;k<2;k++)
				{
					if(k==0)  //手动区域部分
					{
						startTemp=start;
						srcStart = startTemp;
						endTemp = rc[indexS].end;
						indexS1=indexS;
						indexE1=indexS;
						flagS1=flagS;
						flagE1=flagS;
						longflag=-2;
					}
					else //非手动区域部分
					{
						startTemp = rc[indexE].start;
						srcStart = startTemp;
						endTemp = ending;
						indexS1=indexE;
						indexE1=indexE;
						flagS1=flagE;
						flagE1=flagE;
						longflag = 1;
					}
					getSchVelocity(optsch,len,(float)startTemp,(float)endTemp,&v11,&index11,&v21,&index21);
					ret=limitManagement(optsch, len,(float)startTemp,(float)srcStart,(float)endTemp, value, flagS1,flagE1,indexS1,indexE1,index11,index21,flag,longflag);
					if(ret.opt_rest!=OPTIMIZE_SUCCESS)
					{
						strcat(desc,ret.description);
						optimize_exception_decorator(ret.opt_rest,&ret,desc);
						return ret;
					}
				}
			}
			else if((flagS!=opt_const.THROUGHEXPERT &&  flagE==opt_const.THROUGHEXPERT) || (flagS!=opt_const.PUSHERENGINE && flagE==opt_const.PUSHERENGINE)) //左边非手动区域，右边在手动区域
			{
				for(k=0;k<2;k++)
				{
					if(k==0)  //非手动区域
					{
						startTemp = start;
						srcStart = startTemp;
						endTemp = rc[indexS].end;
						indexS1=indexS;
						indexE1=indexS;
						flagS1=flagS;
						flagE1=flagS;
						longflag = -1;
					}
					else //手动区域部分
					{
						startTemp = rc[indexE].start;
						srcStart = startTemp;
						endTemp = ending;
						indexS1=indexE;
						indexE1=indexE;
						flagS1=flagE;
						flagE1=flagE;
						longflag=-2;
					}
					getSchVelocity(optsch,len,(float)startTemp,(float)endTemp,&v11,&index11,&v21,&index21);
					ret=limitManagement(optsch, len,(float)startTemp,(float)srcStart,(float)endTemp, value, flagS1,flagE1,indexS1,indexE1,index11,index21,flag,longflag);
					if(ret.opt_rest!=OPTIMIZE_SUCCESS)
					{
						strcat(desc,ret.description);
						optimize_exception_decorator(ret.opt_rest,&ret,desc);
						return ret;
					}
				}
			}
			else
			{
				ret=limitManagement(optsch, len,(float)start,(float)srcStart,(float)ending, value, flagS,flagE,indexS,indexE,index1,index2,flag,longflag);
				if(ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,ret.description);
					optimize_exception_decorator(ret.opt_rest,&ret,desc);
					return ret;
				}
			}
		}
	}
	return ret;
}
/**
 * @brief 获得限速类型
 * @param flagS  前一个坡段的类型
 * @param flagE  后一个坡段的类型
 * @param type  获得限速策略类型,若没找到则返回-1（这种情况几乎不存在）
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  getLimitStrategyType(int flagS,int flagE,int* type)
{
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	LIMIT_OPTIMIZE_RESULT limit_ret;
	char desc[EXP_DESC_LENGTH]="#getLimitStrategyType";

	if((flagS==-3 && flagE==-3)||(flagS==-2 && flagE==-2)||(flagS==-3&&flagE==-2)||(flagS==-2 && flagE==-3)) //陡下坡/超陡下坡
	{
		*type=1;
		return ret;
	}
	if(flagS==2 && flagE==2)//陡上坡
	{
		*type=2;
		return ret;
	}
	if((flagS==0 || flagS==1 || flagS==-1)&&(flagE==0 || flagE==1 || flagE==-1))//缓坡
	{
		*type=3;
		return ret;
	}
	if((flagS==-3 || flagS==-2) && flagE==2)//前为陡下坡，后为陡上坡
	{
		*type=4;
		return ret;
	}
	if(flagS==2 && (flagE==-2 || flagE==-3))//前为陡上坡，后为陡下坡
	{
		*type=5;
		return ret;
	}
	if((flagS==-3||flagS==-2)&&(flagE==-1||flagE==0||flagE==1))//前为陡下坡，后为缓坡
	{
		*type=6;
		return ret;
	}
	if((flagS==0||flagS==1||flagS==-1)&&(flagE==-3||flagE==-2))//前为缓坡，后为陡下坡
	{
		*type=7;
		return ret;
	}
	if(flagS==2 && (flagE==0 || flagE==-1 || flagE==1))//前为陡上坡，后为缓坡
	{
		*type=8;
		return ret;
	}
	if((flagS==0||flagS==-1||flagS==1) && flagE==2)//前为缓坡，后为陡上坡
	{
		*type=9;
		return ret;
	}
	if((flagS==opt_const.PUSHERENGINE && flagE==opt_const.PUSHERENGINE) || (flagS==opt_const.THROUGHEXPERT && flagE==opt_const.THROUGHEXPERT)) //手动区域段
	{
		*type=10;
		return ret;
	}
	limit_opt_exception_decorator(LIMIT_NOT_FIND_STRATEGY,&limit_ret,desc);
	return limit_result_adapter(limit_ret);
}
/**
 * @brief 限速策略匹配器
 * @param flag     限速策略类型标识
 * @param totalWeight      车重
 * @return  优化异常结果  匹配成功标识,1表示成功,-1表示失败strcat(i,"\n")
 */
OPTIMIZE_RESULT  limitStrategyMatcher(int flag,float totalWeight)
{
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	LIMIT_OPTIMIZE_RESULT limit_ret;
	char desc[EXP_DESC_LENGTH]="#limitStrategyMatcher";

	int i;
	char* name="";
	for (i = 0; i < MAX_LIMITSTRATEGY; i++) {
		if (flag == limitStrategyMetrics[i].type && (int)totalWeight>=limitStrategyMetrics[i].minMass && (int)totalWeight<=limitStrategyMetrics[i].maxMass ) {
			name=limitStrategyMetrics[i].name;
			break;
		}
	}
	if(name!=NULL)
		opt_log_info("限速策略: %s",name);
	else
	{
		limit_opt_exception_decorator(LIMIT_NOT_FIND_STRATEGY,&limit_ret,desc);
		return limit_result_adapter(limit_ret);
	}
	if(strcmp(name,"limitStrategy01")==0)
	{
		limitStrategy=&limitStrategy01;
		return ret;
	}
	if(strcmp(name,"limitStrategy02")==0)
	{
		limitStrategy=&limitStrategy02;
		return ret;
	}
	if(strcmp(name,"limitStrategy03")==0)
	{
		limitStrategy=&limitStrategy03;
		return ret;
	}
	if(strcmp(name,"limitStrategy04")==0)
	{
		limitStrategy=&limitStrategy04;
		return ret;
	}
	if(strcmp(name,"limitStrategy05")==0)
	{
		limitStrategy=&limitStrategy05;
		return ret;
	}
	if(strcmp(name,"limitStrategy06")==0)
	{
		limitStrategy=&limitStrategy06;
		return ret;
	}
	if(strcmp(name,"limitStrategy07")==0)
	{
		limitStrategy=&limitStrategy07;
		return ret;
	}
	if(strcmp(name,"limitStrategy08")==0)
	{
		limitStrategy=&limitStrategy08;
		return ret;
	}
	if(strcmp(name,"limitStrategy09")==0)
	{
		limitStrategy=&limitStrategy09;
		return ret;
	}
	if(strcmp(name,"limitStrategy10")==0)
	{
		limitStrategy=&limitStrategy10;
		return ret;
	}
	limit_opt_exception_decorator(LIMIT_NOT_FIND_STRATEGY,&limit_ret,desc);
	return limit_result_adapter(limit_ret);
}
/**
 * @brief  限速策略执行器
 * @param optSch  优化速度曲线
 * @param len 优化速度曲线长度
 * @param limitInfo  同限速调整相关的结构体，里面保存的数据如上结构体所示
 * @param count 用于查找当前位置属于某个段的起始roadIndex
 * @param flagS 是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag  是否是长大限速
 * @return 优化异常结果
 */
OPTIMIZE_RESULT  limitStrategyExecute(OPTSCH* optsch,int len,LIMITINFO limitInfo,int count,int flagS,int longflag, float srcStart)
{
	return limitStrategy(optsch,len,limitInfo,count,flagS,longflag, srcStart);
}
/**
 * @brief 限速策略包装（包括获得限速策略类型，根据类型匹配策略，执行策略三个步骤）
 * @param optsch   原始优化速度曲线
 * @param len   原始优化速度曲线的长度
 * @param start   当前限速的起始公里标
 * @param ending  当前限速的终止公里标
 * @param value  限速阈值
 * @param flagS  限速起始位置所在的段类型
 * @param flagE  限速终止位置所在的段类型
 * @param indexS  限速起始位置所在的段中的index
 * @param indexE  限速终止位置所在的段中的index
 * @param index1  限速起始位置所在的原始优化曲线中的index
 * @param index2  限速终止位置所在的原始优化曲线中的index
 * @param flag   是否存在限速的起始位置的优化速度小于限速,中间有速度大于限速的点,1表示是,0表示否
 * @param longflag 是否属于长大限速（手动区域与正常的坡段类型处理方式也属于长大限速处理方式）
 * @return 优化异常结果
 */
OPTIMIZE_RESULT limitManagement(OPTSCH* optsch,int len,float start, float srcStart, float ending,float value, int flagS,int flagE,int indexS,int indexE,int index1,int index2,int flag,int longflag)
{
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	int Type;
	char desc[EXP_DESC_LENGTH]="#limitManagement";
	LIMITINFO limitInfo;
	int count = 0;

	ret = getLimitStrategyType(flagS,flagE,&Type);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		return ret;
	}
	ret=limitStrategyMatcher(Type,locoInfo.totalWeight);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		return ret;
	}
	limitInfo.end = ending;
	limitInfo.start = start;
	limitInfo.limit = value;
	limitInfo.index1 = index1;
	limitInfo.index2 = index2;
	limitInfo.indexS = indexS;
	limitInfo.indexE = indexE;
	ret=limitStrategyExecute(optsch,len,limitInfo,count,flag,longflag,srcStart);
	if(ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,ret.description);
		optimize_exception_decorator(ret.opt_rest,&ret,desc);
		return ret;
	}
	return ret;
}
/**
 * @brief 判断当前index所在的点是否是手动区域，若是手动区域则返回原始档位，若不是手动区域则返回newGear;
 * @param optSch  原始优化速度曲线
 * @param index 当前点所在的原始优化速度曲线中所在的index
 * @param srcGgear  新的档位
 * @return 返回的档位
 */
int identifyGear(OPTSCH* optSch, int index , int newGear)
{
	int gear=newGear;
	if(optSch[index].gear==opt_const.THROUGHEXPERT || optSch[index].gear==opt_const.PUSHERENGINE)
	{
		return optSch[index].gear;
	}
	else
		return gear;
}
