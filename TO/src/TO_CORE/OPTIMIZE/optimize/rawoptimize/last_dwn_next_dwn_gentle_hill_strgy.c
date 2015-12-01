/**
 * @file      last_dwn_next_dwn_gentle_hill_strgy.c
 * @brief  缓坡策略  破段类型标记为flag=-1/0/1, (适用于上一段为缓下坡，下一段也为缓下坡的缓坡策略)
 * @date  2014.12.10
 * @author sinlly
 */
#include "opt_global_var.h"
#include "optimize_curve.h"
#include "rawStrategy.h"
#include "base_opt_fuc.h"
int optSchIndex;
/**
 * @brief  缓坡策略函数实现(用于上一段为缓下坡，下一段也为缓下坡的缓坡策略)
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
OPTSCH* last_dwn_next_dwn_gentle_hill_strgy(float vo, float v_interval, int count, int roadIndex,int index, OPTSCH* optSch, float avgV,OPTIMIZE_RESULT *ret) {
	int tempIndex, gear, flag;	//, gearup, geardown,index0,index_back;
//	float headX, v;	//,currentV;
//	OPTSCH* optBack;
	//异常处理标识符
	char  tmp[EXP_DESC_LENGTH];
	sprintf(tmp,"%d",roadIndex);
	char desc[EXP_DESC_LENGTH]="#last_dwn_next_dwn_gentle_hill_strgy;rcIndex=";
	strcat(desc,tmp);
	OPTIMIZE_RESULT fopt_ret;
	fopt_ret.opt_rest=OPTIMIZE_SUCCESS;

	//赋值
	tempIndex = index - 1;
	if (index == 0)
		tempIndex = 0;
//	headX = optSch[tempIndex].start;
//	v = optSch[tempIndex].velocity;
	flag=0;
	gear=0;
	fopt_ret= gr_trac_til_ed_or_lmt(gear ,roadIndex,opt_const.LIMITV , opt_const.MINV,&index, & flag,&count, &optSch);
	if(fopt_ret.opt_rest!=OPTIMIZE_SUCCESS)
	{
		strcat(desc,fopt_ret.description);
		optimize_exception_decorator(fopt_ret.opt_rest,&fopt_ret,desc);
//		ret=&fopt_ret;
		strcpy(ret->description, fopt_ret.description);
		ret->opt_rest =fopt_ret.opt_rest;
		return optSch;
	}
	optSchIndex = index; //最后将本策略optSch所得长度赋值给optSchIndex
//    ret=&fopt_ret;
	strcpy(ret->description, fopt_ret.description);
	ret->opt_rest =fopt_ret.opt_rest;
	return optSch;
}

