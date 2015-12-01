/**
 *  @file tmp_after_process.c
 *  @brief  临时限速调整之后的优化档位后处里生成新的曲线主函数入口
 *  @date Created on: 2015年4月2日
 *   @author sinlly
 */
#include "after_process.h"

/**
 * @brief  临时限速优化后处理生成最终曲线主函数
 * @param optSch   临时限速调整之后的优化曲线片段
 * @param len 临时限速调整之后的优化曲线片段长度
 * @param new_final_curve   将临时限速优化结果调整为连续档位且拉平的新生成的曲线
 * @param final_len  新生成的曲线长度
 * @param cap new_final_curve数组的容量
 * @param final_ret  是否有异常
 * @param tmpStart 临时限速的起始公里标
 * @param tmpEnd 临时限速的终止公里标
 * @param tmpValue 临时限速的限速值
 * @return 临时限速优化结果
 */
OPTSCH*  tmp_after_process(OPTSCH* optSch,int len,OPTSCH* new_final_curve,int* final_len,int *cap, OPTIMIZE_RESULT*final_ret,float tmpStart,float tmpEnd,float tmpValue)
{
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;
	char  desc[EXP_DESC_LENGTH]="#tmp_after_process";
	//初始化构建档位数组，保存档位的相关信息。其中若档位跨车站，则以车站为分界线。
	opt_log_info("临时限速档位分段开始");
	GEARSEGINFO* gearSegs=NULL;
	int i,curGear,gearIndex,lastStartIndex;
	float lastStart=0,midV,curTotalDist;
	gearIndex=-1; //gearSegInfo数组的索引
	lastStartIndex=0;
	curGear=optSch[0].gear;
	int statSIndex=0;
	int statEIndex=MAX_STATION-1;
	for(i=statSIndex;i<MAX_STATION;i++)
	{
		if(stations[i].start<optSch[0].start)
		{
			statSIndex=i+1;
		}
		if(stations[i].start>optSch[len-1].start)
		{
			statEIndex=i-1;
			break;
		}
	}
	int tmpStatSIndex=statSIndex;
	float curStatPost=stations[tmpStatSIndex].start;
	for(i=1;i<len;)
	{
		if(optSch[i].gear!=curGear)
		{
			gearIndex=gearIndex+1;
			if(gearIndex==0)
			{
				gearSegs=(GEARSEGINFO*)malloc(sizeof(GEARSEGINFO));
				if(gearSegs==NULL)
				{
					optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
					strcpy(final_ret->description,ret.description);
					final_ret->opt_rest=ret.opt_rest;
					return new_final_curve;
				}
			}
			else
			{
				gearSegs=(GEARSEGINFO*)realloc(gearSegs,sizeof(GEARSEGINFO)*(gearIndex+1));
				if(gearSegs==NULL)
				{
					optimize_exception_decorator(REALLOC_ERROR,&ret,desc);
					strcpy(final_ret->description,ret.description);
					final_ret->opt_rest=ret.opt_rest;
					return new_final_curve;
				}
			}
			midV=getMidV(optSch,lastStartIndex,i-1);
			gearSegs[gearIndex].gear=curGear;
			gearSegs[gearIndex].startIndex=lastStartIndex;
			gearSegs[gearIndex].endIndex=i-1;
			gearSegs[gearIndex].midV=midV;
			gearSegs[gearIndex].index=gearIndex;
			//新的档位
			lastStartIndex=i;
			curGear=optSch[i].gear;
		}
		if(optSch[i].start>curStatPost && tmpStatSIndex<=statEIndex)  //在车站位置
		{
			gearIndex=gearIndex+1;
			if(gearIndex==0)
			{
				gearSegs=(GEARSEGINFO*)malloc(sizeof(GEARSEGINFO));
				if(gearSegs==NULL)
				{
					optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
					strcpy(final_ret->description,ret.description);
					final_ret->opt_rest=ret.opt_rest;
					return new_final_curve;
				}
			}
			else
			{
				gearSegs=(GEARSEGINFO*)realloc(gearSegs,sizeof(GEARSEGINFO)*(gearIndex+1));
				if(gearSegs==NULL)
				{
					optimize_exception_decorator(REALLOC_ERROR,&ret,desc);
					strcpy(final_ret->description,ret.description);
					final_ret->opt_rest=ret.opt_rest;
					return new_final_curve;
				}
			}
			midV=getMidV(optSch,lastStartIndex,i-1);
			gearSegs[gearIndex].gear=curGear;
			gearSegs[gearIndex].startIndex=lastStartIndex;
			gearSegs[gearIndex].endIndex=i-1;
			gearSegs[gearIndex].midV=midV;
			gearSegs[gearIndex].index=gearIndex;
			//新的档位
			lastStartIndex=i;
			curGear=optSch[i].gear;
			tmpStatSIndex=tmpStatSIndex+1;
			curStatPost=stations[tmpStatSIndex].start;
		}
		i=i+1;
	}
	if(gearSegs[gearIndex].endIndex<len)  //对最后一个档位进行处理
	{
		gearIndex=gearIndex+1;
		if(gearIndex==0)
		{
			gearSegs=(GEARSEGINFO*)malloc(sizeof(GEARSEGINFO));
			if(gearSegs==NULL)
			{
				optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
				strcpy(final_ret->description,ret.description);
				final_ret->opt_rest=ret.opt_rest;
				return new_final_curve;
			}
		}
		else
		{
			gearSegs=(GEARSEGINFO*)realloc(gearSegs,sizeof(GEARSEGINFO)*(gearIndex+1));
			if(gearSegs==NULL)
			{
				optimize_exception_decorator(REALLOC_ERROR,&ret,desc);
				strcpy(final_ret->description,ret.description);
				final_ret->opt_rest=ret.opt_rest;
				return new_final_curve;
			}
		}
		midV=getMidV(optSch,lastStartIndex,len-1);
		gearSegs[gearIndex].gear=curGear;
		gearSegs[gearIndex].startIndex=lastStartIndex;
		gearSegs[gearIndex].endIndex=len-1;
		gearSegs[gearIndex].midV=midV;
		gearSegs[gearIndex].index=gearIndex;
	}
	for(i=0;i<=gearIndex;i++)
		opt_log_info("分段结果:%d; 档位: %d; 档位起始index: %d; 档位终止index: %d; 档位中间速度: %f; 跨越长度: %f",i+1,gearSegs[i].gear,gearSegs[i].startIndex,gearSegs[i].endIndex,gearSegs[i].midV,optSch[gearSegs[i].endIndex].start-optSch[gearSegs[i].startIndex].start);
	opt_log_info("临时限速档位分段结束");

	//进行档位拉平处理
	int capacity=opt_const.POSTINITLENGTH;
	new_final_curve=(OPTSCH*)malloc(sizeof(OPTSCH)*capacity);
	if(new_final_curve==NULL)
	{
		optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
		strcpy(final_ret->description,ret.description);
		final_ret->opt_rest=ret.opt_rest;
		return new_final_curve;
	}
	int num=-1;//新生成的曲线索引
	GEARSEGINFO* tmpGearSegs=NULL;
	int tmpGearIndex=-1;
	int sIndex,eIndex,gear;
	float totalDist=0;
	float headX,delta_s,delta_e,delta_v,v;
	int count=0;
	headX=optSch[0].start;
	v=optSch[0].velocity;
	int j,lastGear;
	float endS; //结束距离
	int k;
	int startGearIndex=0;
	for(k=statSIndex-1;k<=statEIndex;k++)
	{
		//计算两个车站位置所在的gearSegs中的index
		float startStatPost=stations[k].start;
		if(startStatPost<optSch[0].start)
			startStatPost=optSch[0].start+0.1; //之所以加0.1是为了避免下面的等号操作
		float endStatPost=stations[k+1].start;
		if(endStatPost>optSch[len-1].start)
			endStatPost=optSch[len-1].start-0.1; //之所以减去0.1是为了避免下面的等号操作
		int endGearIndex=gearIndex;
		for(i=0;i<=gearIndex;i++)
		{
			if(optSch[gearSegs[i].startIndex].start>endStatPost)
			{
				endGearIndex=i-1;
				break;
			}
		}
		if(k==statEIndex) //最后一个车站
			endGearIndex=gearIndex;
		//找到的车站终止档位边界可能会介于两个之间,最后一个车站除外
		if(gearSegs[endGearIndex].gear==gearSegs[endGearIndex-1].gear && k!=statEIndex)
			endGearIndex=endGearIndex-1;


		//以车站为单位进行拉平
		float tmpDeltaS=0;//用于对小于limitDIst的档位段进行累积，若累计的和超过limitDIst，则进行拉平操作，而不是所有将所有连续的满足要求的档位进行拉平
		if(fabs(optSch[gearSegs[startGearIndex].endIndex].start-optSch[gearSegs[startGearIndex].startIndex].start)==0)
			startGearIndex=startGearIndex+1;
		for(i=startGearIndex;i<=endGearIndex;i++)
		{
			gear=gearSegs[i].gear;
			if(i==startGearIndex)
			{
				opt_log_info("起始车站:%d,    档位:%d",startGearIndex,gear);
			}

			if(i==endGearIndex)
			{
				opt_log_info("终止车站:%d,    档位:%d",endGearIndex,gear);
			}
			sIndex=gearSegs[i].startIndex;
			eIndex=gearSegs[i].endIndex;
			totalDist=optSch[eIndex].start-optSch[sIndex].start;
			//			gear=gearSegs[i].gear;
			int airFlag=0;
			if(gear==opt_const.AIRE || gear==opt_const.AIRS) //空气制动标识
				airFlag=1;
			int  limitFlag=ifTmpNearLimit(optSch,sIndex,eIndex,tmpStart,tmpEnd,tmpValue);

			int handFlag=0;
			if(gear==opt_const.PUSHERENGINE || gear==opt_const.THROUGHEXPERT) //手动区域标识
				handFlag=1;

			/**
			 * 档位的跨越距离小于LENGTHLIMIT并且不接近限速,则将其加入到临时档位存放数组中用于此后的拉平
			 */
			if(((totalDist<opt_const.GEARLENGTHLIMIT && totalDist>0) || fabs(totalDist-opt_const.GEARLENGTHLIMIT)<opt_const.PRECISION) && limitFlag ==0 && airFlag!=1 && handFlag!=1)
			{
				if((totalDist>opt_const.GEARLENGTHLIMIT-100 && num!=-1 && fabs(new_final_curve[num].gear-gear)==1) || (i==startGearIndex && i>0 && num!=-1 && gear==new_final_curve[num].gear))  //当前档位虽然小于LENGTHLIMIT但是大于LIMITLENGTH-100,且和前面的档位相差1,则档位不拉平;位于车站的起始位置，且和上一个车站终止位置档位相等
				{

				}
				else if( fabs(optSch[gearSegs[i-1].startIndex].start-optSch[gearSegs[i-1].endIndex].start)==0)
				{}
				else
				{
					tmpGearIndex=tmpGearIndex+1;
					if(tmpGearIndex==0)
					{
						tmpGearSegs=(GEARSEGINFO*)malloc(sizeof(GEARSEGINFO));
						if(tmpGearSegs==NULL)
						{
							optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
							strcpy(final_ret->description,ret.description);
							final_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
					}
					else
					{
						tmpGearSegs=(GEARSEGINFO*)realloc(tmpGearSegs,sizeof(GEARSEGINFO)*(tmpGearIndex+1));
						if(tmpGearSegs==NULL)
						{
							optimize_exception_decorator(REALLOC_ERROR,&ret,desc);
							strcpy(final_ret->description,ret.description);
							final_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
					}
					tmpGearSegs[tmpGearIndex].startIndex=gearSegs[i].startIndex;
					tmpGearSegs[tmpGearIndex].endIndex=gearSegs[i].endIndex;
					tmpGearSegs[tmpGearIndex].gear=gearSegs[i].gear;
					tmpGearSegs[tmpGearIndex].midV=gearSegs[i].midV;
					tmpGearSegs[tmpGearIndex].index=gearSegs[i].index;
				}
			}

			/**
			 * 对之前的档位进行拉平，以及对当前档位进行处理
			 *当前档位按照原有档位及距离进行牵引所要 满足条件：
			 * a) 当前档位的跨越距离小于给定的限定距离（这里暂时为500)
			 * b) 当前档位的跨越距离大于给定的限定距离，但其中存在点靠近限速，则不对其进行拉平。
			 * c) 当前档位的下一个档位的跨越距离为0（跨越距离为0相当于档位对应只有1个点）,则需要再此对之前的档位进行拉平
			 * d) 当前档位的跨越距离大于限定距离-100并且其档位和上一个档位仅相差1
			 * 或者当此时的索引档位到达endGearIndex,则需要对之前的档位进行拉平。
			 *
			 *该过程包括2个步骤
			 *a) 对此前存放在tmpGearSegs中的档位进行拉平
			 *b) 拉平档位到现档位进行连续渐变处理
			 *c) 按照当前档位进行牵引
			 */
			if(totalDist>opt_const.GEARLENGTHLIMIT ||(totalDist < opt_const.GEARLENGTHLIMIT && limitFlag==1) || i==endGearIndex || (totalDist>opt_const.GEARLENGTHLIMIT-100 && num!=-1 && fabs(new_final_curve[num].gear-gear)==1 ) || new_final_curve[num].gear==gear || fabs(optSch[gearSegs[i+1].startIndex].start-optSch[gearSegs[i+1].endIndex].start)==0)
			{
				if(tmpGearIndex>=0 && gearSegs[tmpGearSegs[tmpGearIndex].index-1].gear>=opt_const.MINGEAR) //当有符合要求的档位（保存档位信息的数组不为空）. 对之前的档位进行拉平操作
				{
					float totalS=0;   //拉平后的档位运行的距离
					int fg; //拉平后的档位
					if(tmpGearIndex==0 ) //只有一个
					{
						float f3=0,f2=0;
						int lastGearIndex=tmpGearSegs[tmpGearIndex].index-1;
						trac_ret=getForce(gearSegs[lastGearIndex].gear,gearSegs[lastGearIndex].midV,&f3);//上一个档位的牵引力
						if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
						{
							strcat(desc,trac_ret.description);
							trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
							ret=trac_result_adapter(trac_ret);
							strcpy(final_ret->description,ret.description);
							final_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
						trac_ret=getForce(tmpGearSegs[tmpGearIndex].gear,tmpGearSegs[tmpGearIndex].midV,&f2);//当前档位的牵引力
						if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
						{
							strcat(desc,trac_ret.description);
							trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
							ret=trac_result_adapter(trac_ret);
							strcpy(final_ret->description,ret.description);
							final_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
						float s2=optSch[tmpGearSegs[tmpGearIndex].endIndex].start-optSch[tmpGearSegs[tmpGearIndex].startIndex].start;//当前档位的运行的距离
						//找到上一个档位同当前档位具有相同运行距离的中间点的速度
						float lastMidV;
						float tmpS=optSch[tmpGearSegs[tmpGearIndex].startIndex].start-s2/2;
						//												int tmpIndex=num;
						int tmpIndex=tmpGearSegs[tmpGearIndex].startIndex;
						//						while(new_final_curve[tmpIndex].start> tmpS || fabs(new_final_curve[tmpIndex].start-tmpS)<opt_const.PRECISION)
						//						{
						//							tmpIndex=tmpIndex-1;
						//						}
						while(optSch[tmpIndex].start> tmpS || fabs(optSch[tmpIndex].start-tmpS)<opt_const.PRECISION)
						{
							tmpIndex=tmpIndex-1;
						}
						lastMidV=optSch[tmpIndex].velocity;
						//						lastMidV=new_final_curve[tmpIndex].velocity;
						//计算拉平之后档位的f
						float ff;
						if(gearSegs[lastGearIndex].gear<tmpGearSegs[tmpGearIndex].gear)  //当前档位大于后面档位
						{
							fg=gearSegs[lastGearIndex].gear+(int)floor((tmpGearSegs[tmpGearIndex].gear-gearSegs[lastGearIndex].gear)/2);
						}
						else //当前档位小于后面档位
						{
							fg=gearSegs[lastGearIndex].gear-(int)floor((gearSegs[lastGearIndex].gear-tmpGearSegs[tmpGearIndex].gear)/2);
						}
						float fv=(lastMidV+tmpGearSegs[tmpGearIndex].midV)/2;
						trac_ret=getForce(fg,fv,&ff);//当前档位的牵引力
						if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
						{
							strcat(desc,trac_ret.description);
							trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
							ret=trac_result_adapter(trac_ret);
							strcpy(final_ret->description,ret.description);
							final_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
						if((int)(f3-ff)!=0) //产生的力相等
						{
							float s3=(s2*ff-s2*f2)/(f3-ff);
							totalS=s3+s2;
							tmpIndex=num;
							endS=optSch[tmpGearSegs[tmpGearIndex].startIndex].start-s3;
							while(new_final_curve[tmpIndex].start>endS || fabs(new_final_curve[tmpIndex].start-endS)<opt_const.PRECISION)
							{
								tmpIndex=tmpIndex-1;
							}
							headX=new_final_curve[tmpIndex].start;
							v=new_final_curve[tmpIndex].velocity;
							num=tmpIndex;
							gear=fg;
							opt_log_info("仅1个档位正常拉平(前后产生的力不相等) :%d , 档位: %d",tmpGearSegs[tmpGearIndex].index,fg);
						}
						else
						{
							gear=new_final_curve[num].gear;
							opt_log_info("仅一个档位特殊拉平(前后产生的力相等):%d , 档位: %d",tmpGearSegs[tmpGearIndex].index,gear);
							totalS=s2;
						}
						if(new_final_curve[num].gear!=opt_const.AIRE && new_final_curve[num].gear!=opt_const.AIRS && new_final_curve[num].gear!=opt_const.PUSHERENGINE && new_final_curve[num].gear!=opt_const.THROUGHEXPERT)
						{
							if(num==-1)
							{
								lastGear=optSch[0].gear;
								endS=optSch[0].start+totalS;
							}
							else
							{
								lastGear=new_final_curve[num].gear;
								endS=new_final_curve[num].start+totalS;
							}
							ret= gear_transfer(&new_final_curve,lastGear,gear,&capacity,&num,&headX,&v);
							if(ret.opt_rest!=OPTIMIZE_SUCCESS)
							{
								strcat(desc,ret.description);
								optimize_exception_decorator(ret.opt_rest,&ret,desc);
								strcpy(final_ret->description,ret.description);
								final_ret->opt_rest=ret.opt_rest;
								return new_final_curve;
							}
						}
						else
						{
							endS=headX+totalS;
						}
						//按照当前档位进行处理
						ret=dragByGear(&new_final_curve,gear,endS, &capacity, &num,&headX,&v	);
						if(ret.opt_rest!=OPTIMIZE_SUCCESS)
						{
							strcat(desc,ret.description);
							optimize_exception_decorator(ret.opt_rest,&ret,desc);
							strcpy(final_ret->description,ret.description);
							final_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
					}
					else //有两个以上需要拉平的档位
					{
						tmpDeltaS=0;
						int ifContinueF=0; //是否continue
						int lastTmpIndex=0;
						int k;
						for(j=0;j<=tmpGearIndex;j++)
						{
							tmpDeltaS=tmpDeltaS+(optSch[tmpGearSegs[j].endIndex].start-optSch[tmpGearSegs[j].startIndex].start);
							if(ifContinueF==1 && j!=tmpGearIndex)
								continue;
							//若从当前位置向后预瞄到最后一个档位的距离小于给定的距离，则将其一同加入进行处理
							if(tmpDeltaS>opt_const.GEARLENGTHLIMIT && j+1<=tmpGearIndex && getPostFromTmpSeg( tmpGearSegs,optSch,j+1,tmpGearIndex)<opt_const.GEARLENGTHLIMIT)
							{
								ifContinueF=1;
								continue;
							}
							if(tmpDeltaS>opt_const.GEARLENGTHLIMIT || j==tmpGearIndex)
							{
								float deltaFs=0; //档位对应的力*距离的累计量
								float deltaFv=0;//档位对应的力*速度的累计量
								opt_log_info("拉平:起始段index:%d - 终止段index: %d",tmpGearSegs[lastTmpIndex].index,tmpGearSegs[j].index);
								for(k=lastTmpIndex;k<=j;k++)
								{
									float tmpF;
									trac_ret=getForce(tmpGearSegs[k].gear,tmpGearSegs[k].midV,&tmpF);//当前档位对应的牵引力
									//									printf("%f\n",tmpF);
									if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
									{
										strcat(desc,trac_ret.description);
										trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
										ret= trac_result_adapter(trac_ret);
										strcpy(final_ret->description,ret.description);
										final_ret->opt_rest=ret.opt_rest;
										return new_final_curve;
									}
									deltaFs=deltaFs+tmpF*(optSch[tmpGearSegs[k].endIndex].start-optSch[tmpGearSegs[k].startIndex].start);
									//									printf("%f\n",(optSch[tmpGearSegs[k].endIndex].start-optSch[tmpGearSegs[k].startIndex].start));
									deltaFv=deltaFv+(optSch[tmpGearSegs[k].endIndex].start-optSch[tmpGearSegs[k].startIndex].start)*tmpGearSegs[k].midV;
									//							printf("%f,%f,%d\n",tmpF,tmpGearSegs[j].midV,tmpGearSegs[j].gear);
									totalS=totalS+optSch[tmpGearSegs[k].endIndex].start-optSch[tmpGearSegs[k].startIndex].start;
								}
								float ff=deltaFs/totalS;//拉平档位对应的F
								float fv=deltaFv/totalS;//拉平档位对应的速度
								int ffv=(int)(fv+0.5);//将拉平档位速度取整数，四舍五入}
								int g1,g2;
								float delF1,delF2,endS1,endS2;//用来保存从当前力和速度查询到的档位和距离.g1和delF1表示小的档位，g2和deltaF2表示大的档位
								fg=getGearByFV(ff,ffv,&g1,&g2,&delF1,&delF2);
								if(fg==MOREGEAR)
								{
									if(delF1>delF2)
									{
										if(delF2/delF1<0.1)
											fg=g1;
									}
									else
									{
										if(delF1/delF2<0.1)
											fg=g2;
									}
								}
								if(fg==MOREGEAR)  //当按照2个档位运行
								{
									opt_log_info("拉平档位: %d,%d",g1,g2);
									if(abs(g1-new_final_curve[num].gear)<abs(g2-new_final_curve[num].gear)) //距离上一个档位近的为第一个档位
									{
										if(new_final_curve[num].gear!=opt_const.AIRE && new_final_curve[num].gear!=opt_const.AIRS && new_final_curve[num].gear!=opt_const.PUSHERENGINE && new_final_curve[num].gear!=opt_const.THROUGHEXPERT)
										{
											if(num==-1)
											{
												endS1=optSch[0].start+totalS*delF1/(delF1+delF2); //始终表示最先运行的档位
												endS2=optSch[0].start+totalS; //始终表示后一个运行的档位
												lastGear=optSch[0].gear;
												endS=optSch[0].start+totalS;
											}
											else
											{
												endS1=new_final_curve[num].start+totalS*delF1/(delF1+delF2); //始终表示最先运行的档位
												endS2=new_final_curve[num].start+totalS; //始终表示后一个运行的档位
												lastGear=new_final_curve[num].gear;
												endS=new_final_curve[num].start+totalS;
											}
											ret= gear_transfer(&new_final_curve,lastGear,g1,&capacity,&num,&headX,&v); //档位变换
											if(ret.opt_rest!=OPTIMIZE_SUCCESS)
											{
												strcat(desc,ret.description);
												optimize_exception_decorator(ret.opt_rest,&ret,desc);
												strcpy(final_ret->description,ret.description);
												final_ret->opt_rest=ret.opt_rest;
												return new_final_curve;
											}
										}
										else
										{
											endS1=headX+totalS*delF1/(delF1+delF2);
											endS2=headX+totalS;
										}
										//按照g1档位进行处理
										ret=dragByGear(&new_final_curve,g1,endS1, &capacity, &num,&headX,&v	);
										if(ret.opt_rest!=OPTIMIZE_SUCCESS)
										{
											strcat(desc,ret.description);
											optimize_exception_decorator(ret.opt_rest,&ret,desc);
											strcpy(final_ret->description,ret.description);
											final_ret->opt_rest=ret.opt_rest;
											return new_final_curve;
										}
										//之所以需要再进行一次档位变换，是怕从上一个档位跳到g1变换之后的headX大于endS1,所以可能并不需要用g1档位进行牵引处理，而是直接到g2档位
										lastGear=new_final_curve[num].gear;
										ret= gear_transfer(&new_final_curve,lastGear,g2,&capacity,&num,&headX,&v); //档位变换
										if(ret.opt_rest!=OPTIMIZE_SUCCESS)
										{
											strcat(desc,ret.description);
											optimize_exception_decorator(ret.opt_rest,&ret,desc);
											strcpy(final_ret->description,ret.description);
											final_ret->opt_rest=ret.opt_rest;
											return new_final_curve;
										}

										//按照g2档位进行处理
										ret=dragByGear(&new_final_curve,g2,endS2, &capacity, &num,&headX,&v	);
										if(ret.opt_rest!=OPTIMIZE_SUCCESS)
										{
											strcat(desc,ret.description);
											optimize_exception_decorator(ret.opt_rest,&ret,desc);
											strcpy(final_ret->description,ret.description);
											final_ret->opt_rest=ret.opt_rest;
											return new_final_curve;
										}
									}
									else //距离上一个档位比较近的为第二个档位
									{
										if( new_final_curve[num].gear!=opt_const.AIRE && new_final_curve[num].gear!=opt_const.AIRS &&  new_final_curve[num].gear!=opt_const.PUSHERENGINE && new_final_curve[num].gear!=opt_const.THROUGHEXPERT)
										{
											if(num==-1)
											{
												endS1=optSch[0].start+totalS*delF2/(delF1+delF2); //始终表示最先运行的档位
												endS2=optSch[0].start+totalS; //始终表示后一个运行的档位
												lastGear=optSch[0].gear;
												endS=optSch[0].start+totalS;
											}
											else
											{
												endS1=new_final_curve[num].start+totalS*delF2/(delF1+delF2); //始终表示最先运行的档位
												endS2=new_final_curve[num].start+totalS; //始终表示后一个运行的档位
												lastGear=new_final_curve[num].gear;
												endS=new_final_curve[num].start+totalS;
											}
											ret= gear_transfer(&new_final_curve,lastGear,g2,&capacity,&num,&headX,&v); //档位变换
											if(ret.opt_rest!=OPTIMIZE_SUCCESS)
											{
												strcat(desc,ret.description);
												optimize_exception_decorator(ret.opt_rest,&ret,desc);
												strcpy(final_ret->description,ret.description);
												final_ret->opt_rest=ret.opt_rest;
												return new_final_curve;
											}
										}
										else
										{
											endS1=headX+totalS*delF2/(delF1+delF2);
											endS2=headX+totalS;
										}
										//按照g2档位进行处理
										ret=dragByGear(&new_final_curve,g2,endS1, &capacity, &num,&headX,&v	);
										if(ret.opt_rest!=OPTIMIZE_SUCCESS)
										{
											strcat(desc,ret.description);
											optimize_exception_decorator(ret.opt_rest,&ret,desc);
											strcpy(final_ret->description,ret.description);
											final_ret->opt_rest=ret.opt_rest;
											return new_final_curve;
										}
										//之所以需要再进行一次档位变换，是怕从上一个档位跳到g2变换之后的headX大于endS1,所以可能并不需要用g1档位进行牵引处理，而是直接到g2档位
										lastGear=new_final_curve[num].gear;
										ret= gear_transfer(&new_final_curve,lastGear,g1,&capacity,&num,&headX,&v); //档位变换
										if(ret.opt_rest!=OPTIMIZE_SUCCESS)
										{
											strcat(desc,ret.description);
											optimize_exception_decorator(ret.opt_rest,&ret,desc);
											strcpy(final_ret->description,ret.description);
											final_ret->opt_rest=ret.opt_rest;
											return new_final_curve;
										}
										//按照g1档位进行处理
										ret=dragByGear(&new_final_curve,g1,endS2, &capacity, &num,&headX,&v	);
										if(ret.opt_rest!=OPTIMIZE_SUCCESS)
										{
											strcat(desc,ret.description);
											optimize_exception_decorator(ret.opt_rest,&ret,desc);
											strcpy(final_ret->description,ret.description);
											final_ret->opt_rest=ret.opt_rest;
											return new_final_curve;
										}
									}
								}
								else //当只按照一个档位运行
								{
									opt_log_info("拉平档位: %d",fg);
									gear=fg;
									if(new_final_curve[num].gear!=opt_const.AIRE && new_final_curve[num].gear!=opt_const.AIRS && new_final_curve[num].gear!=opt_const.PUSHERENGINE && new_final_curve[num].gear!=opt_const.THROUGHEXPERT)
									{
										if(num==-1)
										{
											lastGear=optSch[0].gear;
											endS=optSch[0].start+totalS;
										}
										else
										{
											lastGear=new_final_curve[num].gear;
											endS=new_final_curve[num].start+totalS;
										}
										ret= gear_transfer(&new_final_curve,lastGear,gear,&capacity,&num,&headX,&v); //档位变换
										if(ret.opt_rest!=OPTIMIZE_SUCCESS)
										{
											strcat(desc,ret.description);
											optimize_exception_decorator(ret.opt_rest,&ret,desc);
											strcpy(final_ret->description,ret.description);
											final_ret->opt_rest=ret.opt_rest;
											return new_final_curve;
										}
									}
									else
									{
										endS=headX+totalS;
									}
									//按照当前档位进行处理
									ret=dragByGear(&new_final_curve,gear,endS, &capacity, &num,&headX,&v	);
									if(ret.opt_rest!=OPTIMIZE_SUCCESS)
									{
										strcat(desc,ret.description);
										optimize_exception_decorator(ret.opt_rest,&ret,desc);
										strcpy(final_ret->description,ret.description);
										final_ret->opt_rest=ret.opt_rest;
										return new_final_curve;
									}
								}
								lastTmpIndex=j+1;
								tmpDeltaS=0;
								ifContinueF=0;
							}
						}
					}
				}
				if(airFlag!=1 && handFlag!=1 && (totalDist>opt_const.GEARLENGTHLIMIT ||(totalDist<opt_const.GEARLENGTHLIMIT && limitFlag==1) || (tmpGearIndex==0 && gearSegs[tmpGearSegs[tmpGearIndex].index-1].gear<opt_const.MINGEAR) || (i==startGearIndex && i>0 && num!=-1 && gear==new_final_curve[num].gear) )) //当前档位的跨越距离大于给定的限定距离并且所有点都不靠近限速;前一个档位为空气制动（暂时不进行拉平）
				{
					endS=optSch[eIndex].start;
					gear=gearSegs[i].gear;
					if(new_final_curve[num].gear!=opt_const.AIRE && new_final_curve[num].gear!=opt_const.AIRS && new_final_curve[num].gear!=opt_const.PUSHERENGINE && new_final_curve[num].gear!=opt_const.THROUGHEXPERT)
					{
						if(num==-1)
							lastGear=optSch[0].gear;
						else
							lastGear=new_final_curve[num].gear;
						//进行档位渐变处理,从lastGear-gear
						ret= gear_transfer(&new_final_curve,lastGear,gear,&capacity,&num,&headX,&v);
						if(ret.opt_rest!=OPTIMIZE_SUCCESS)
						{
							strcat(desc,ret.description);
							optimize_exception_decorator(ret.opt_rest,&ret,desc);
							strcpy(final_ret->description,ret.description);
							final_ret->opt_rest=ret.opt_rest;
							return new_final_curve;
						}
					}
					//按照当前档位进行处理
					ret=dragByGear(&new_final_curve,gear,endS, &capacity, &num,&headX,&v);
					if(ret.opt_rest!=OPTIMIZE_SUCCESS)
					{
						strcat(desc,ret.description);
						optimize_exception_decorator(ret.opt_rest,&ret,desc);
						strcpy(final_ret->description,ret.description);
						final_ret->opt_rest=ret.opt_rest;
						return new_final_curve;
					}
				}

				if(tmpGearIndex!=-1) //对临时存放连续低于2000米的档位的数据进行重新初始化
				{
					free(tmpGearSegs);
					tmpGearSegs=NULL;
					tmpGearIndex=-1;
				}
			}
			/**
			 * 空气制动档位则跳过（直接拉成2点）
			 */
			if(airFlag==1 || handFlag==1)
			{
				num=num+1;
				gear=gearSegs[i].gear;
				if(num+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
				{
					capacity=capacity+opt_const.POSTINITLENGTH/2;
					new_final_curve=(OPTSCH*)realloc(new_final_curve,sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
					if(new_final_curve==NULL)
					{
						optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
						strcpy(final_ret->description,ret.description);
						final_ret->opt_rest=ret.opt_rest;
						return new_final_curve;
					}
				}
				float tmpE=0;
				float tmpT=0;
				for(j=sIndex;j<=eIndex;j++)
				{
					tmpE=tmpE+optSch[j].consumption;
					tmpT=tmpT+optSch[j].time;
				}
				new_final_curve[num].start = optSch[eIndex].start;
				new_final_curve[num].gear = gear;
				new_final_curve[num].velocity = optSch[eIndex].velocity;
				new_final_curve[num].consumption = tmpE;
				new_final_curve[num].time=tmpT;
				new_final_curve[num].acc = 0;
				headX=optSch[eIndex].start;
				v=optSch[eIndex].velocity;
				continue;
			}
			/**
			 * 当档位只存在一个点，则用上一个档位进行处理
			 */
			if(totalDist==0)
			{
				gear=new_final_curve[num].gear;
				trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,
						&delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					ret=trac_result_adapter(trac_ret);
					strcpy(final_ret->description,ret.description);
					final_ret->opt_rest=ret.opt_rest;
					return new_final_curve;
				}
				headX = headX + delta_s;
				v = v + delta_v;
				num=num+1;
				if(num+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
				{
					capacity=capacity+opt_const.POSTINITLENGTH/2;
					new_final_curve=(OPTSCH*)realloc(new_final_curve,sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
					if(new_final_curve==NULL)
					{
						optimize_exception_decorator(MALLOC_ERROR,&ret,desc);
						strcpy(final_ret->description,ret.description);
						final_ret->opt_rest=ret.opt_rest;
						return new_final_curve;
					}
				}
				new_final_curve[num].start = headX;
				new_final_curve[num].gear = gear;
				new_final_curve[num].velocity = v;
				new_final_curve[num].consumption = delta_e;
				new_final_curve[num].time = opt_const.TSTEP;
				new_final_curve[num].acc = delta_v / opt_const.TSTEP;
				continue;
			}
			/**
			 *  上一个档位跨越距离为0,当前档位
			 */
			if(i>0 && (totalDist<opt_const.GEARLENGTHLIMIT || fabs(totalDist-opt_const.GEARLENGTHLIMIT)<opt_const.PRECISION)&& fabs(optSch[gearSegs[i-1].startIndex].start-optSch[gearSegs[i-1].endIndex].start)==0)
			{
				endS=optSch[eIndex].start;
				gear=gearSegs[i].gear;
				if( new_final_curve[num].gear!=opt_const.AIRE && new_final_curve[num].gear!=opt_const.AIRS)
				{
					if(num==-1)
						lastGear=optSch[0].gear;
					else
						lastGear=new_final_curve[num].gear;
					//进行档位渐变处理,从lastGear-gear
					ret= gear_transfer(&new_final_curve,lastGear,gear,&capacity,&num,&headX,&v);
					if(ret.opt_rest!=OPTIMIZE_SUCCESS)
					{
						strcat(desc,ret.description);
						optimize_exception_decorator(ret.opt_rest,&ret,desc);
						strcpy(final_ret->description,ret.description);
						final_ret->opt_rest=ret.opt_rest;
						return new_final_curve;
					}
				}
				//按照当前档位进行处理
				ret=dragByGear(&new_final_curve,gear,endS, &capacity, &num,&headX,&v);
				if(ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,ret.description);
					optimize_exception_decorator(ret.opt_rest,&ret,desc);
					strcpy(final_ret->description,ret.description);
					final_ret->opt_rest=ret.opt_rest;
					return new_final_curve;
				}
				continue;
			}

		}

		startGearIndex=endGearIndex+1;
	}
	*final_len=num+1;
	*cap=capacity;
	free(optSch);
	return new_final_curve;
}
/**
 * @brief 对临时限速最后的点进行调整（包括档位渐变以及追上原始曲线）
 * @param new_tmp_curve  临时限速最终的速度曲线
 * @param tmp_final_len  临时限速最终速度曲线的长度
 * @param cap  new_tmp_curve的容量
 * @param optSch  原始经过临时限速处理之后的曲线
 * @param index  临时限速处理的最后一个点在原始速度曲线中的index
 * @param ret 优化异常结果
 * @return 调整之后的临时限速曲线
 */
OPTSCH* tmp_end_process(OPTSCH* new_tmp_curve ,int* tmp_final_len,int * cap,OPTSCH* optSch,int* inx,OPTIMIZE_RESULT* ret)
{
	OPTIMIZE_RESULT opt_ret;
	opt_ret.opt_rest=OPTIMIZE_SUCCESS;
	char  desc[EXP_DESC_LENGTH]="#tmp_end_process";
	TRAC_CAL_RESULT trac_ret;
	trac_ret.trac_result=TRAC_CAL_SUCCESS;
	float headX,delta_s,delta_e,delta_v,v;
	int count=0;
	int capacity=*cap;
	int index=*inx;
	float tStep=opt_const.COMGEARSTEP ;//普通档位换档持续的时间
	float idelStep=opt_const.IDELGEARSTEP;//惰行档位换档持续的时间

	int num=*tmp_final_len-1;
	while(new_tmp_curve[num].start>optSch[index].start)
	{
		index=index+1;
	}
	if(fabs(new_tmp_curve[num].velocity-optSch[index].velocity)<opt_const.TMPSPEEDACCURACY) //最后一个点的速度和原始曲线当前点的速度相差小于精度范围
		return new_tmp_curve;
	int srcIndex;
	if(new_tmp_curve[num].velocity>optSch[index].velocity)//最后一个点的速度比当前点的速度大
	{
		int flag=0; //当flag=1时候表示未找到
		while(1)
		{
			num=num-1;
			srcIndex=index;
			while(1)
			{
				srcIndex=srcIndex-1;
				if(optSch[srcIndex].start<new_tmp_curve[num].start)
					break;
			}
			if(new_tmp_curve[num].velocity<optSch[srcIndex+1].velocity)
				break;
		}
		if(flag==0)
		{
			int len=num+1;
			int srcInx=srcIndex+1;
			new_tmp_curve=gear_strategy_match(new_tmp_curve,&len, cap,new_tmp_curve[num].gear,optSch,optSch[srcInx].gear,&srcInx,&opt_ret);
			*tmp_final_len=len;
			*inx=srcInx;
			if(opt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,opt_ret.description);
				optimize_exception_decorator(opt_ret.opt_rest,&opt_ret,desc);
				strcpy(ret->description,opt_ret.description);
				ret->opt_rest=opt_ret.opt_rest;
				return new_tmp_curve;
			}
		}
	}
	else //最后一个点的速度比 当前点的速度小
	{
		headX=new_tmp_curve[num].start;
		v=new_tmp_curve[num].velocity;
		int numT=num;
		int gear=new_tmp_curve[numT].gear;
		if(gear<=optSch[index].gear)
		{
			gear=gear+1;
			if(gear>opt_const.MAXGEAR)
				gear=opt_const.MAXGEAR;
		}
		else if(gear>optSch[index].gear+1)//比当前原始曲线档位大1即可
		{
			gear=gear-1;
		}
		float gearTime=0;
		int srcInx=index;
		int i;
		while (1 )
		{
			for(i=srcInx;;i++) //找到第一个大于临时限速最后一个点的原始曲线中的点
			{
				if(optSch[i].start>new_tmp_curve[numT].start)
				{
					index=i;
					break;
				}
			}
			//当前临时限速点的对应的下一个原始曲线中的点为空气制动或则手动区域，则需要再用原档位计算一个点
			if(optSch[index].gear==opt_const.AIRE || optSch[index].gear==opt_const.AIRS || optSch[index].gear==opt_const.PUSHERENGINE || optSch[index].gear==opt_const.THROUGHEXPERT)
			{
				numT=numT+1;
				if(numT+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
				{
					capacity=capacity+opt_const.POSTINITLENGTH/2;
					new_tmp_curve=(OPTSCH*)realloc(new_tmp_curve,sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
					if(new_tmp_curve==NULL)
					{
						optimize_exception_decorator(MALLOC_ERROR,&opt_ret,desc);
						strcpy(ret->description,opt_ret.description);
						ret->opt_rest=opt_ret.opt_rest;
						return new_tmp_curve;
					}
				}
				trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,
						&delta_e);
				if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
				{
					strcat(desc,trac_ret.description);
					trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
					opt_ret=trac_result_adapter(trac_ret);
					strcpy(ret->description,opt_ret.description);
					ret->opt_rest=opt_ret.opt_rest;
					return  new_tmp_curve;
				}
				headX=headX+delta_s;
				v=v+delta_v;
				new_tmp_curve[numT].start = headX;
				new_tmp_curve[numT].gear = optSch[index].gear;
				new_tmp_curve[numT].velocity = v;
				new_tmp_curve[numT].consumption = delta_e;
				new_tmp_curve[numT].time = opt_const.TSTEP;
				new_tmp_curve[numT].acc = delta_v / opt_const.TSTEP;
				break;
			}
			if(gear==0 && gearTime>idelStep) //惰行档位
			{
				if(gear<=optSch[index].gear)
					gear=gear+1;
				else if(gear>optSch[index].gear+1)
					gear=gear-1;
				if(gear>opt_const.MAXGEAR)
					gear=opt_const.MAXGEAR;
				if(gear<opt_const.MINGEAR)
					gear=opt_const.MINGEAR;
				gearTime=0;
			}
			else if(gearTime>tStep) //普通档位
			{
				if(gear<=optSch[index].gear)
					gear=gear+1;
				else if(gear>optSch[index].gear+1)
					gear=gear-1;
				if(gear>opt_const.MAXGEAR)
					gear=opt_const.MAXGEAR;
				if(gear<opt_const.MINGEAR)
					gear=opt_const.MINGEAR;
				gearTime=0;
			}
			trac_ret=DoCaculateByTime(headX, v, gear, opt_const.TSTEP, &count, &delta_s, &delta_v,
					&delta_e);
			if(trac_ret.trac_result!=TRAC_CAL_SUCCESS)
			{
				strcat(desc,trac_ret.description);
				trac_exception_decorator(trac_ret.trac_result,&trac_ret,desc);
				opt_ret=trac_result_adapter(trac_ret);
				strcpy(ret->description,opt_ret.description);
				ret->opt_rest=opt_ret.opt_rest;
				return  new_tmp_curve;
			}
			headX = headX + delta_s;
			v = v + delta_v;
			gearTime=gearTime+opt_const.TSTEP;
			if(fabs(v-optSch[index].velocity)<opt_const.TMPSPEEDACCURACY)
			{
				break;
			}
			//记录
			numT=numT+1;
			index=index+1;
			if(numT+1>capacity)//加入点的个数大于给定容量，则进行扩容操作
			{
				capacity=capacity+opt_const.POSTINITLENGTH/2;
				new_tmp_curve=(OPTSCH*)realloc(new_tmp_curve,sizeof(OPTSCH)*(capacity)); //每次增加初始容量的一倍
				if(new_tmp_curve==NULL)
				{
					optimize_exception_decorator(MALLOC_ERROR,&opt_ret,desc);
					strcpy(ret->description,opt_ret.description);
					ret->opt_rest=opt_ret.opt_rest;
					return new_tmp_curve;
				}
			}
			new_tmp_curve[numT].start = headX;
			new_tmp_curve[numT].gear = gear;
			new_tmp_curve[numT].velocity = v;
			new_tmp_curve[numT].consumption = delta_e;
			new_tmp_curve[numT].time = opt_const.TSTEP;
			new_tmp_curve[numT].acc = delta_v / opt_const.TSTEP;
		}
		new_tmp_curve=gear_strategy_match(new_tmp_curve,&numT, cap,new_tmp_curve[numT].gear,optSch,optSch[index].gear,&index,&opt_ret);
		*cap=capacity;
		*tmp_final_len=numT;
		inx=&index;
	}
	strcpy(ret->description,opt_ret.description);
	ret->opt_rest=opt_ret.opt_rest;
	return new_tmp_curve;
}
