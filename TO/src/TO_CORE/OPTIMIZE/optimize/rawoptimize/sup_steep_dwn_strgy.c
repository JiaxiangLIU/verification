/**
 * @file     sup_steep_dwn_strgy
 * @brief   超陡下坡策略  坡段类型为flag=-3
 * @date  2014.4.14
 * @author sinlly
 */
#include "opt_global_var.h"
#include "rawStrategy.h"
#include "base_opt_fuc.h"
#include "tractionCaculator.h"
int optSchIndex;
/**
 * @brief  超陡下坡策略函数实现
 *@param vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
//OPTSCH* sup_steep_dwn_strgy(float vo, float v_interval, int count, int roadIndex,
//		int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) {
//		printf("sup_steep  from %d to %d,vo is%f,optsch is %d,mile is%f\n",rc[roadIndex].start,rc[roadIndex].end,rc[roadIndex].vo,index,optSch[index-1].start);
//	int gear;
//	int flag; //标记是否惰性至末尾，1表示是，0表示否
//	//异常处理标识符
//	char  tmp[EXP_DESC_LENGTH];
//	sprintf(tmp,"%d",roadIndex);
//	char desc[EXP_DESC_LENGTH]="#sup_steep_dwn_strgy;rcIndex=";
//	strcat(desc,tmp);
//	OPTIMIZE_RESULT fopt_ret;
//	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;
//
//	flag=0;
//	//计算车头通过惰行走完该段，直到速度达到vi或者行驶到段末
//	gear=0;
//	fopt_ret=dyn_gr_trac_til_vi_or_ed_or_lmt(gear, roadIndex,opt_const. LIMITV,&index, & flag,&count,&optSch);
//	if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
//	{
//		strcat(desc,fopt_ret.description);
//		optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//		ret=&fopt_ret;
//		return optSch;
//	}
//	//	 gr_trac_til_ed_or_lmt(gear ,roadIndex,LIMITV , MINV,&index, & flag,&count, &optSch);
//	int srcIndex=index;
//	int srcCount=count;
//	//判断是否能够惰行到结尾,不能惰性到结尾则从临界速度最大值动到末尾
//	while(flag==0 && gear>opt_const.MINGEAR)
//	{
//		flag=1;
//		index=srcIndex;
//		count=srcCount;
//		gear=gear-1;
//		fopt_ret=gr_trac_til_ed_or_lmt(gear ,roadIndex,opt_const. LIMITV, opt_const.MINV,&index, & flag,&count,  &optSch);
//		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
//		{
//			strcat(desc,fopt_ret.description);
//			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
//			return optSch;
//		}
//	}
//	if (flag == 0)
//	{
//		fopt_ret=air_brake_op(roadIndex , &index, &optSch);
//		if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
//		{
//			strcat(desc,fopt_ret.description);
//			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//			ret=&fopt_ret;
//			return optSch;
//		}
//	}
//	ret=&fopt_ret;
//	optSchIndex = index; //最后将本策略optSch所得长度赋值给optSchIndex
//	return optSch;
//
//}

// add air vertify to modify the strategy


OPTSCH* sup_steep_dwn_strgy(float vo, float v_interval, int count, int roadIndex,
		int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) {
	int gear;
	int flag_e;
	flag_e=0;
	float dis=(float)rc[roadIndex].end;
	int tempCount=0;
	int tempIndex=0;
	int useair=0;
	OPTSCH* tempOptsch=NULL;
	//异常处理标识符
	char  tmp[EXP_DESC_LENGTH];
	sprintf(tmp,"%d",roadIndex);
	char desc[EXP_DESC_LENGTH]="#sup_steep_dwn_strgy;rcIndex=";
	strcat(desc,tmp);
	OPTIMIZE_RESULT fopt_ret;
	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;


    while(flag_e!=111){
    	// for every loop just recovery starting
    	int flag=0;
    	flag_e=0;
    	tempCount=count;
    	tempIndex=index;
    	tempOptsch=(OPTSCH*)malloc(sizeof(OPTSCH)*index);
    	int a=copy_optsch(optSch,tempOptsch,index);
    	if(a!=index)
    		printf("优化曲线copy error\n");


    	gear=0;
    	fopt_ret=dyn_gr_trac_til_vi_or_ed_or_dis(gear, roadIndex,dis,opt_const. LIMITV,&tempIndex, & flag_e,&tempCount,&tempOptsch);
    	if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
    		{
    			strcat(desc,fopt_ret.description);
    			optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//    			ret=&fopt_ret;
    			strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
    			return optSch;
    		}
    	if(flag_e==101||flag_e==100){
    		int srcIndex=tempIndex;
    		int srcCount=tempCount;
    		while(flag==0 && gear>opt_const.MINGEAR)
    		{
    			flag=1;
    			tempIndex=srcIndex;
    			tempCount=srcCount;
    			gear=gear-1;
    			fopt_ret=gr_trac_til_dis_or_lmt(gear ,roadIndex,dis,opt_const. LIMITV, opt_const.MINV,&tempIndex, & flag,&tempCount,  &tempOptsch);
    			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
    			{
    				strcat(desc,fopt_ret.description);
    				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//    				ret=&fopt_ret;
    				strcpy(ret->description, fopt_ret.description);
					ret->opt_rest =fopt_ret.opt_rest;
    				return optSch;
    			}
    		}
    		if(flag==0||useair==1){
    			float air_start=tempOptsch[tempIndex-1].start;
    			float air_end=(float)rc[roadIndex].end;
    			float v_in=tempOptsch[tempIndex-1].velocity;
    			float v_out=rc[roadIndex].vo;
    			if(roadIndex+1<MAX_ROADCATEGORY && rc[roadIndex+1].vi>rc[roadIndex].vo) //若下一段的vi大于本段的vo则无需空气制动到vo。
    				v_out=rc[roadIndex+1].vi;
    			float real_distance,cal_common_distance,cal_ergence_distance;
    			int air_flag=0;
    			airBrakeDistance(air_start,air_end,v_in,v_out,&real_distance,&cal_common_distance,&cal_ergence_distance,&air_flag);
    			if(air_flag==2){
    				fopt_ret=air_brake_op(roadIndex , &tempIndex, &tempOptsch);
    				if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
    				{
    					strcat(desc,fopt_ret.description);
    					optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//    					ret=&fopt_ret;
    					strcpy(ret->description, fopt_ret.description);
						ret->opt_rest =fopt_ret.opt_rest;
    					return tempOptsch;
    				}
    				flag_e=111;
    			}else{
    				free(tempOptsch);
    				tempOptsch=NULL;
    				dis=(float)rc[roadIndex].end-cal_common_distance-50;// 50是一个估计值，确保存在于范围内
    				useair=1;
    			}
    		}// flag
    		else{
    			flag_e=111;
    		}
    	}//ed if(flag_e)
    }//end while.
	//计算车头通过惰行走完该段，直到速度达到vi或者行驶到段末

    // 合并参数
//    ret=&fopt_ret;
    strcpy(ret->description, fopt_ret.description);
	ret->opt_rest =fopt_ret.opt_rest;
    optSchIndex = tempIndex; //最后将本策略optSch所得长度赋值给optSchIndex
    return tempOptsch;
}

