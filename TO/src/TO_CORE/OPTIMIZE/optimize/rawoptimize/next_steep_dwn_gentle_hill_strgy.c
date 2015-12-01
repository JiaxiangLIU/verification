/**
 * @file      next_steep_dwn_gentle_hill_strgy.c
 * @brief  缓坡策略  破段类型标记为flag=-1/0/1  用于下一段为陡下坡或超陡下坡的缓坡策略
 * @date  2014.12.10
 * @author sinlly
 */
#include "opt_global_var.h"
#include "optimize_curve.h"
#include "rawStrategy.h"
#include "base_opt_fuc.h"
int optSchIndex;
/**
 * @brief  缓坡策略函数实现,用于下一段为陡下坡或超陡下坡的缓坡策略（error 可能振荡，目前符合要求）
 *@param  vo  段出速度
 * @param v_interval  允许的速度跳跃阈值
 * @param count  用于查找当前位置属于某个段的起始roadIndex
 * @param roadIndex  段所在的index
 * @param index  当前优化的初始index
 * @param optSch  优化曲线
 * @param avgV  当前段的平均速度
 * @param ret 异常处理结果
 * @return 优化曲线结果
 */
OPTSCH* next_steep_dwn_gentle_hill_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) {
	int srcIndex=index;
	int srcCount=count;
	//异常处理标识符
	char  tmp[EXP_DESC_LENGTH];
	sprintf(tmp,"%d",roadIndex);
	char desc[EXP_DESC_LENGTH]="#next_steep_dwn_gentle_hill_strgy;rcIndex=";
	strcat(desc,tmp);
	OPTIMIZE_RESULT fopt_ret;
	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;

	int flag=1;
	int gear=0;
	float dis=(float)rc[roadIndex].end;
	int useair=0;
	fopt_ret=gr_trac_til_ed_or_lmt(gear ,roadIndex,opt_const.LIMITV ,opt_const.MINV, &index, & flag,&count, &optSch);
	if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,fopt_ret.description);
		optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//		ret=&fopt_ret;
		strcpy(ret->description, fopt_ret.description);
		ret->opt_rest =fopt_ret.opt_rest;
		return optSch;
	}

	while(flag!=1)
	{
		if(flag==-1 && gear<opt_const.MAXGEAR) //低于最低速度，需要档位进行牵引
		{
			gear=gear+1;
			flag=1;
			index=srcIndex;
			count=srcCount;
			fopt_ret=gr_trac_til_ed_or_lmt(gear ,roadIndex,opt_const.LIMITV ,opt_const.MINV, &index, & flag,&count, &optSch);
			if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
			{
				strcat(desc,fopt_ret.description);
				optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//				ret=&fopt_ret;
				strcpy(ret->description, fopt_ret.description);
				ret->opt_rest =fopt_ret.opt_rest;
				return optSch;
			}
		}
		else
		{
			if(flag==0 && gear>opt_const.MINGEAR)
			{
				gear=gear-1;
				index=srcIndex;
				count=srcCount;
				flag=1;
				fopt_ret=gr_trac_til_dis_or_lmt(gear ,roadIndex,dis,opt_const.LIMITV , opt_const.MINV,&index, & flag,&count, &optSch);
				if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
				{
					strcat(desc,fopt_ret.description);
					optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//					ret=&fopt_ret;
					strcpy(ret->description, fopt_ret.description);
					ret->opt_rest =fopt_ret.opt_rest;
					return optSch;
				}
			}
			if(flag==0||useair==1)
			{
				float air_start=optSch[index-1].start;
				float air_end=(float)rc[roadIndex].end;
				float v_in=optSch[index-1].velocity;
				float v_out=rc[roadIndex].vo;
				if(roadIndex+1<MAX_ROADCATEGORY && rc[roadIndex+1].vi>rc[roadIndex].vo) //若下一段的vi大于本段的vo则无需空气制动到vo。
					v_out=rc[roadIndex+1].vi;
				float real_distance,cal_common_distance,cal_ergence_distance;
				int air_flag=0;
				airBrakeDistance(air_start,air_end,v_in,v_out,&real_distance,&cal_common_distance,&cal_ergence_distance,&air_flag);
				if(air_flag==2){
					fopt_ret=air_brake_op( roadIndex ,&index, &optSch);
					flag=1;
					if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
					{
						strcat(desc,fopt_ret.description);
						optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//						ret=&fopt_ret;
						strcpy(ret->description, fopt_ret.description);
						ret->opt_rest =fopt_ret.opt_rest;
						return optSch;
					}
				}//air_flag if
				else{
					dis=(float)rc[roadIndex].end-cal_common_distance-50;
					useair=1;
					gear=0;// 重新进行循环，使档位递减试探。
				}

			}//if
		}//else加速还是减速

	}//while
//     ret=&fopt_ret;
	strcpy(ret->description, fopt_ret.description);
	ret->opt_rest =fopt_ret.opt_rest;
	optSchIndex = index; //最后将本策略optSch所得长度赋值给optSchIndex
	return optSch;
}

