/**
 * @file  tmp_gear_trasfer.c
 * @brief 临时限速档位渐变处理主函数
 * @date Created on: 2015年3月10日
 *  @author Author: sinlly chen
 */
#include "tmp_gear_trasfer.h"
/**
 *@brief  档位连续变换策略匹配函数
 * @param new_tmp_curve   临时限速调整片段曲线
 * @param tmp_final_len  临时限速调整片段曲线长度(当前最后一个点的index为tmp_fianl_len-1)
 * @param cap 临时限速调整片段结构的当前最大容量
 * @param sGear  临时限速需要渐变的档位
 * @param optSch    进行临时限速调整之后的完整的曲线
 * @param eGear  临时限速片段同完整曲线衔接部分需要连续变换的终止档位
 * @param srcInx 临时限速与完整曲线衔接部分在完整曲线中的index
 * @param opt_ret 异常结果
 * @return  临时限速调整之后的经过档位连续变换后的完整曲线
 */
OPTSCH*  gear_strategy_match(OPTSCH* new_tmp_curve,int * tmp_final_len, int* cap, int sGear,OPTSCH* optSch,  int eGear,int* srcInx,OPTIMIZE_RESULT* opt_ret)
{
	OPTIMIZE_RESULT ret;
	ret.opt_rest=OPTIMIZE_SUCCESS;
	float  comGearStep= opt_const.COMGEARSTEP;   //普通档位换档持续的时间
	float  idelGearStep = opt_const.IDELGEARSTEP; //惰行档位换档持续的时间
	char desc[EXP_DESC_LENGTH]="#gear_strategy_match";
	if(sGear*eGear<0)
	{
		if((abs(sGear-eGear)-1)%2==0)
			new_tmp_curve=jump_even_gear_trasfer(new_tmp_curve,tmp_final_len,cap,optSch,srcInx,comGearStep,idelGearStep,&ret);
		else
			new_tmp_curve=jump_odd_gear_trasfer(new_tmp_curve,tmp_final_len,cap,optSch,srcInx,comGearStep,idelGearStep,&ret);
		if(ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,ret.description);
			optimize_exception_decorator(ret.opt_rest,&ret,desc);
			strcpy(opt_ret->description,ret.description);
			opt_ret->opt_rest=ret.opt_rest;
			return new_tmp_curve;
		}
	}
	else
	{
		if((abs(sGear-eGear)-1)%2==0)
			new_tmp_curve=even_gear_trasfer(new_tmp_curve,tmp_final_len,cap,optSch,srcInx,comGearStep,idelGearStep,&ret);
		else
			new_tmp_curve=odd_gear_trasfer(new_tmp_curve,tmp_final_len,cap,optSch,srcInx,comGearStep,idelGearStep,&ret);
		if(ret.opt_rest!=OPTIMIZE_SUCCESS)
		{
			strcat(desc,ret.description);
			optimize_exception_decorator(ret.opt_rest,&ret,desc);
			strcpy(opt_ret->description,ret.description);
			opt_ret->opt_rest=ret.opt_rest;
			return new_tmp_curve;
		}
	}
	strcpy(opt_ret->description,ret.description);
	opt_ret->opt_rest=ret.opt_rest;
	return new_tmp_curve;
}
///**
// * @brief 往前判断从index开始的档位是否是我们需要的档位，同时该档位的运行时间至少为最低限定时间
// * @param optSch  原始速度曲线
// * @param len   原始速度曲线的长度
// * @param srcGear   需要的档位
// * @param index   判断的起始index
// * @param time  限定的时间
// * @return
// */
//int verifyFwdGear(OPTSCH* optSch,int len, int srcGear,int index,float time)
//{
//	//	int i;
//	//	float delta_t=0;
//	//	for(i=index;i<len;i++)
//	//	{
//	//		if(optSch[i].gear==srcGear)
//	//		{
//	//			delta_t=delta_t+optSch[i].time;
//	//			continue;
//	//		}
//	//		else
//	//			break;
//	//	}
//	//	if(delta_t>time || fabs(delta_t-time)<opt_const.PRECISION)
//	//		return 1;
//	//	else
//	//		return -1;
//	return 1;
//}
/**
 *@brief 计算档位调整之前和档位调整之后的曲线的交点位置
 * @param optSch  原始曲线
 * @param srcIndex  原始需要调整的index
 * @param optBack   调整之后的部分曲线
 * @param newIndex  调整之后的点
 * @param midStart  交点的公里标
 * @param midV  交点的速度
 */
void get_mid_position(OPTSCH*optSch, int srcIndex,OPTSCH* optBack,int newIndex,float* midStart,float* midV)
{
	float startS=optSch[srcIndex].start;
	float startV=optSch[srcIndex].velocity;
	float endS=optSch[srcIndex+1].start;
	float endV=optSch[srcIndex+1].velocity;
	float newStartV=optBack[newIndex].velocity;
	float newEndV=optBack[newIndex+1].velocity;
	*midV=(startV*newEndV-newStartV*endV)/(newEndV-newStartV-endV+startV);
	*midStart=startS+(*midV-newStartV)*(endS-startS)/(newEndV-newStartV);
}
