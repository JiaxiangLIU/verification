/*
 * controller.c
 *
 *  Created on: Oct 14, 2014
 *      Author: root
 */

#include "rt_global_var.h"
#include "opt_global_var.h"
#include "optimizeTools.h"
#include "rt_controller.h"

RT_RESULT rt_controller(OPTSCHCURVE current_opt_result, RT_INPUT rt_in, RT_FLAG* rt_flag_p, int *gear_p)
{
	float rt_velocity = rt_in.rt_velocity;

	RT_FLAG rt_flag = *rt_flag_p;

	int threshold = 2; //threshold for active controller
	float flag_reset_threshold = 0.5;

	int opt_gear = current_opt_result.gear;
	float opt_v = current_opt_result.velocity;
	float delta_v = 0;

	int gear = opt_gear;
	*gear_p = gear;

	delta_v = opt_v - rt_velocity;

	if (opt_gear == -10 || opt_gear == -9)
	{
//		printf("Air Brake in work! gear = %d, opt_gear = %d, delta_v = %f\n", opt_gear, opt_gear, delta_v);
		log_debug("Air Brake in work! gear = %d, opt_gear = %d, delta_v = %f\n", opt_gear, opt_gear, delta_v);
		return RT_SUCCESS;
	}
	else
	{
		if (rt_flag.rt_ctr_flag == 0)
		{
			if (delta_v >= threshold || delta_v <= -threshold)
			{
				rt_flag.rt_ctr_flag = 1;
			}
			else
			{
//				printf("Run as optimized! gear = %d, opt_gear = %d, delta_v = %f\n", opt_gear, opt_gear, delta_v);
				log_debug("Run as optimized! gear = %d, opt_gear = %d, delta_v = %f\n", opt_gear, opt_gear, delta_v);
			}
		}
		if (rt_flag.rt_ctr_flag == 1)
		{
			gear = gear + round(delta_v);
			if (gear > 8)
			{
				gear = 8;
			}
			if (gear < -8)
			{
				gear = -8;
			}

//			printf("Run adaptively! gear = %d, opt_gear = %d, delta_v = %f\n", gear, opt_gear, delta_v);
			log_debug("Run adaptively! gear = %d, opt_gear = %d, delta_v = %f\n", gear, opt_gear, delta_v);

			//reset flag
			if (-flag_reset_threshold <= delta_v && delta_v <= flag_reset_threshold)
			{
				rt_flag.rt_ctr_flag = 0;
			}
		}
	}

	*gear_p = gear;
	*rt_flag_p = rt_flag;

	log_debug("rt_controller success @1\n");

	return RT_SUCCESS;
}

//float evaluate_avg(OPTIMIZERESULT* optSch, int start_count, int end_count){
//	float float_avg_gear = 0;
//	int sum_gear = 0;
//	int i;
//	for (i=start_count; i< end_count; i ++){
//		sum_gear = sum_gear + optSch[i].gear;
//	}
//	if (end_count == start_count){
//		float_avg_gear = sum_gear;
//	}else{
//		float_avg_gear = sum_gear/(end_count - start_count);
//	}
//	return float_avg_gear;
//}


//// find the right optSch element's count and opt_v
//void find_count(OPTIMIZERESULT* optSch, int *count, float *opt_v, float headX){
//
//	int count_tmp;
//	float v_tmp;
//	count_tmp = *count;
//	v_tmp = *opt_v;
//
//	while(1){
//		if (headX < optSch[count_tmp].start){
//			count_tmp = count_tmp - 1;
//		}
//		if (headX >= optSch[count_tmp + 1].start){
//			count_tmp = count_tmp + 1;
//		}
//		if (headX == optSch[count_tmp].start){
//			v_tmp = optSch[count_tmp].velocity;
//			break;
//		}else if(headX > optSch[count_tmp].start && headX <= optSch[count_tmp + 1].start){
//			v_tmp = (headX - optSch[count_tmp].start)/(optSch[count_tmp+1].start-
//					optSch[count_tmp].start)*(optSch[count_tmp+1].velocity-
//							optSch[count_tmp].velocity) + optSch[count_tmp].velocity;
//			break;
//		}
//	}
//
//	*count = count_tmp;
//	*opt_v = v_tmp;
//}
