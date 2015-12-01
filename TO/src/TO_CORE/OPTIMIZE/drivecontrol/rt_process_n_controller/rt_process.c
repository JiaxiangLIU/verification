/*
 * realtime_process.c
 *
 *  Created on: Jan 26, 2015
 *      Author: root
 */

#include <string.h>
#include <stdlib.h>

#include "opt_definition.h"
#include "opt_global_var.h"
#include "rt_definition.h"
#include "rt_global_var.h"
#include "rt_process.h"
#include "rt_controller.h"

// parameter definition
LL_NODE* ll_head;
LL_NODE* ll_current;
LL_NODE* ll_last;

int MAX_FINAL_LENGTH;


RT_RESULT rt_process(RT_INPUT rt_in, RT_INOUT* rt_inout_p, RT_FLAG* rt_flag_p, RT_INDEX* rt_index_p, RT_INOUT* ahead_rt_inout_p,
		RT_FLAG* ahead_rt_flag_p, RT_INDEX* ahead_rt_index_p, AHEAD_EST_FLAGS* ahead_est_flags_p, RT_CORE* rt_out_p)
{
	//declaration and initialize
	RT_RESULT result = RT_SUCCESS;

	RT_CORE rt_out = *rt_out_p;
//	RT_OPT rt_opt = rt_out.rt_opt;
	int gear = 0;

	OPTSCHCURVE current_opt_result;
	OPTSCHCURVE ahead_opt_result;

//	//initialization
//	OVER_SPEED_INFO os_info;  //over speed
//	os_info.cont_os_start = 0;
//	os_info.os_flag = 0;

//	RT_PAR rt_par = rt_out.rt_par; //real time parameters
//	RT_STATUS rt_status = rt_out.rt_status;
//	NEXT_STATION_INFO ns_info = rt_out.ns_info;  //information for next station, including...
//	GRADE_ABILITY_INFO ga_info = rt_out.ga_info;
//	ANTI_STOP_INFO as_info = rt_out.as_info;
//	MANUAL_AREA_INFO ma_info = rt_out.ma_info;
	MANUAL_AREA_INFO ma_info;

	/*
	 * if not in the optimized scope, the post is always growing in the moving direction
	 */
	if (rt_in.rt_con_tel_kp < rt_in.rt_con_start_post)
	{
		rt_out.rt_status.rt_post_status = BEFORE_OPR_AREA;
#if ENABLE_LOG_INFO
		log_info("rt_process: out of the optimized result scope: not reach yet!\n");
#endif
		return RT_NOREACH_OPTRESULT;
	}
	else
	{
		if (rt_in.rt_con_tel_kp >= rt_in.rt_con_end_post)
		{
			rt_out.rt_status.rt_post_status = BEFORE_OPR_AREA;
#if ENABLE_LOG_INFO
			log_info("rt_process: out of the optimized result scope: passed the scope!\n");
#endif
			return RT_PASS_OPTRESULT;
		}
		else
		{
			rt_out.rt_status.rt_post_status = IN_OPR_AREA;
		}
	}

	// do opt location for real time process
	result = rt_locate(rt_in, rt_inout_p, rt_flag_p, rt_index_p, &current_opt_result);
	if (result != RT_SUCCESS)
	{
#if ENABLE_LOG_ERROR
		log_error("rt_process error: rt_locate failure! \n");
#endif
		return result;
	}

	// do opt location for ahead estimation
	RT_INPUT ahead_rt_in = rt_in;
	ahead_rt_in.rt_con_tel_kp = rt_in.rt_con_tel_kp + DELTA_AHEAD;
	result = rt_locate(ahead_rt_in, ahead_rt_inout_p, ahead_rt_flag_p, ahead_rt_index_p, &ahead_opt_result);
	if (result != RT_SUCCESS)
	{
#if ENABLE_LOG_ERROR
		log_error("rt_process error: rt_locate failure! \n");
#endif
		return result;
	}

	//call the controller for gear

	result = rt_controller(current_opt_result, rt_in, rt_flag_p, &gear);
	rt_out.rt_opt.rt_gear = gear;
	rt_out.rt_opt.opt_gear = gear;
	rt_out.rt_opt.opt_v = current_opt_result.velocity;
	if (result != RT_SUCCESS)
	{
#if ENABLE_LOG_ERROR
		log_error("rt_process error: rt_controller failure! \n");
#endif
		return result;
	}

//	//get other rt_par's.
//	rt_par.rt_cont_post = rt_in.rt_con_tel_kp; //need to be changed into original ones
//	rt_par.rt_v = rt_in.rt_velocity;
//	rt_par.rt_light = rt_in.rt_light;
////	rt_par.opt_gear = current_opt_result.gear;
//	rt_par.opt_gear = rt_par.rt_gear;
//	rt_par.opt_v = current_opt_result.velocity;
//	rt_par.tele_post = rt_in.tele_post;
//
//	// info for next station
//	ns_info.ns_id = current_opt_result.ford_station_num;
//	ns_info.cont_ns_start = 0; //add later
//	ns_info.t_to_ns = current_opt_result.ford_time;

	// info for auto enable status
	log_info("rt_process: check on the auto enable status starts!\n");
	OPTFLAG opt_flag_en = current_opt_result.manual_flag;
	RT_ENABLE_STATUS rt_enable_status = RT_AUTO_UNABLE;
	switch (opt_flag_en)
	{
	case NONE_FLAG:
		rt_enable_status = RT_AUTO_ENABLE;
		break;
	case ADD_LOCO_AREA:
		rt_enable_status = RT_AUTO_UNABLE;
		break;
	case RUN_THROUGH_TEST:
		rt_enable_status = RT_AUTO_UNABLE;
		break;
	case AIR_DAMPING_BRAKE:
		rt_enable_status = RT_AUTO_UNABLE;
		break;
	case AIR_DAMPING_AVG:
		rt_enable_status = RT_AUTO_UNABLE;
		break;
	default:
		rt_enable_status = RT_AUTO_UNABLE;
		break;
	}
	rt_out.rt_status.rt_enable_status = rt_enable_status;

//	if (rt_in.rt_airbrake_status == BRAKE_EMERGENCY || rt_in.rt_airbrake_status == BRAKE_AIR_PRESSURE || rt_in.rt_airbrake_status == BRAKE_AIR_SHUTOFF)
//	{
//		rt_enable_status = RT_AIR_BRAKING;
//		rt_par.rt_gear = 0;
//	}
//	rt_status.rt_enable_status = rt_enable_status;

//	if (rt_in.rt_velocity > 0)
//	{
//		rt_status.rt_loco_working_status = RT_RUNNING;
//	}
//	else
//	{
//		rt_status.rt_loco_working_status = RT_WAITING;
//	}
//#if ENABLE_LOG_INFO
//	log_info("rt_process: rt_status set!\n");
//#endif

//	// info for anti stop
//	result = rt_anti_stop_search(rt_in, ahead_opt_result, ahead_est_flags_p, &as_info);
//#if ENABLE_LOG_INFO
//	log_info("rt_process: anti_stop info set!\n");
//#endif

//	// over speed estimation
//	result = rt_manual_area_search(rt_in, ahead_opt_result, ahead_est_flags_p, &ma_info);
//	if (result != RT_SUCCESS)
//	{
//#if ENABLE_LOG_ERROR
//		log_error("rt_process: rt_manual_area_search failure!\n");
//#endif
//		return RT_OSPEED_EST_FAILURE;
//	}
//#if ENABLE_LOG_INFO
//	log_info("rt_process: manual_area info set!\n");
//#endif


//	// over speed estimation
//	result = rt_over_speed_est(rt_in, current_opt_result, gear, &os_info);
//	if (result != RT_SUCCESS)
//	{
//#if ENABLE_LOG_ERROR
//		log_error("rt_process: rt_over_speed_est failure!\n");
//#endif
//		return RT_OSPEED_EST_FAILURE;
//	}
//#if ENABLE_LOG_INFO
//	log_info("rt_process: over_speed info set!\n");
//#endif


//	// grade ability estimation
//	result = rt_gradeability_est(rt_in, current_opt_result, gear, &ga_info);
//	if (result != RT_SUCCESS)
//	{
//#if ENABLE_LOG_ERROR
//		log_error("rt_process: rt_gradeability_est failure!\n");
//#endif
//		return RT_OSPEED_EST_FAILURE;
//	}
//#if ENABLE_LOG_INFO
//	log_info("rt_process: rt_gradeability_est info set!\n");
//#endif


	// the outputs
//	rt_out.rt_par = rt_par;
//	rt_out.ns_info = ns_info;
//	rt_out.os_info = os_info;
//	rt_out.ga_info = ga_info;
//	rt_out.as_info = as_info;
//	rt_out.ma_info = ma_info;
//	rt_out.rt_status = rt_status;

	//returns
	*rt_out_p = rt_out;
#if ENABLE_LOG_INFO
	log_info("rt_process: rt_out is ready for the loop!\n");
#endif


	return RT_SUCCESS;
}


RT_RESULT rt_locate(RT_INPUT rt_in, RT_INOUT* rt_inout_p, RT_FLAG* rt_flag_p, RT_INDEX* rt_index_p, OPTSCHCURVE* current_opt_result_p)
{
	/*
	 * here starts the initialization
	 */

	//initialize return
	RT_RESULT result = RT_SUCCESS;

	//get value of flags and indexes
	RT_INOUT rt_inout = *rt_inout_p;
	RT_FLAG rt_flag = *rt_flag_p;
	RT_INDEX rt_index = *rt_index_p;
	int rt_opt_flag = rt_flag.rt_opt_flag;
	int rt_check_temp_flag = rt_flag.rt_check_temp_flag;
	int rt_ctr_flag = rt_flag.rt_ctr_flag;
	int rt_global_index = rt_index.rt_global_index;
	int rt_temp_index = rt_index.rt_temp_index;
	float rt_temp_start = rt_inout.rt_temp_start;
	float rt_temp_end = rt_inout.rt_temp_end;

	//get the initialized value of opt result
	OPTSCHCURVE current_opt_result = *current_opt_result_p;

	LL_NODE* p_node = ll_current;
	int data_length;


	/*
	 * here starts the location
	 */
	if (rt_opt_flag == 1) 	//current loop in global opt result
	{
#if ENABLE_LOG_DEBUG
		log_debug("rt_process:rt_locate: current loop in global opt result!\n");
#endif
		if (ll_current->next != NULL) 	//temp opt result is not empty
		{
#if ENABLE_LOG_DEBUG
			log_debug("rt_process:rt_locate: temp opt result is not empty!\n");
#endif
			if (rt_check_temp_flag == 0 || ll_flag == 1) 	//have not checked the temp result, or there is new temp result present
			{
#if ENABLE_LOG_DEBUG
				log_debug("rt_process:rt_locate: have not checked the temp result, or there is new temp result present!\n");
#endif

				p_node = ll_current->next;
				rt_temp_start = p_node->data[0].start;
				data_length = p_node->data_length;
				rt_temp_end = p_node->data[data_length].start;
#if ENABLE_LOG_DEBUG
				log_debug("rt_process:rt_locate: next temp result start: %d, data_length: %d, temp result end: %d\n",
						rt_temp_start, data_length, rt_temp_end);
#endif

				//once checked, set flags into:
				rt_check_temp_flag = 1;
				ll_flag = 0;
			}

			if (rt_in.rt_con_tel_kp >= rt_temp_start && rt_in.rt_con_tel_kp < rt_temp_end)
			{
#if ENABLE_LOG_DEBUG
				log_debug("rt_process:rt_locate: current post %d in temp optimized result scope", rt_in.rt_con_tel_kp);
#endif
				rt_temp_index = 0;
				result = rt_temp_locate(rt_in.rt_con_tel_kp, &rt_index, p_node);
				rt_opt_flag = 0;
				if (result != RT_SUCCESS)
				{
#if ENABLE_LOG_ERROR
					log_error("rt_process:rt_locate: rt_temp_locate failure @1 !\n");
#endif
					return RT_LOCATE_FAILURE;
				}
#if ENABLE_LOG_DEBUG
				log_debug("rt_process:rt_locate: rt_temp_locate success @1 !\n");
#endif
			}
			else
			{
				result = rt_global_locate(rt_in.rt_con_tel_kp, &rt_global_index);
				if (result != RT_SUCCESS)
				{
#if ENABLE_LOG_ERROR
					log_error("rt_process:rt_locate: rt_global_locate failure @1 !\n");
#endif
					return RT_LOCATE_FAILURE;
				}
#if ENABLE_LOG_DEBUG
				log_debug("rt_process:rt_locate: rt_global_locate success @1 !\n");
#endif
			}
		}
		else
		{
			result = rt_global_locate(rt_in.rt_con_tel_kp, &rt_global_index);
			current_opt_result = opt_final_curves[rt_global_index];

			if (result != RT_SUCCESS)
			{
#if ENABLE_LOG_ERROR
				log_error("rt_process:rt_locate: rt_global_locate failure @2 !\n");
#endif
				return RT_LOCATE_FAILURE;
			}
#if ENABLE_LOG_DEBUG
			log_debug("rt_process:rt_locate: rt_global_locate success @2 !\n");
#endif
		}
	}
	else //current loop not in global opt result
	{
		if (rt_in.rt_con_tel_kp > rt_temp_end)
		{
			ll_last = ll_current;
			ll_current = ll_current->next;

			if (ll_current->next != NULL) //temp opt result is not empty
			{
				p_node = ll_current->next;
				rt_temp_start = p_node->data[0].start;
				data_length = p_node->data_length;
				rt_temp_end = p_node->data[data_length].start;

				//once checked, set flags into:
				rt_check_temp_flag = 1;
				ll_flag = 0;

				if (rt_in.rt_con_tel_kp >= rt_temp_start && rt_in.rt_con_tel_kp < rt_temp_end)
				{
					rt_temp_index = 0;
					result = rt_temp_locate(rt_in.rt_con_tel_kp, &rt_index, p_node);
					rt_opt_flag = 0;
					if (result != RT_SUCCESS)
					{
#if ENABLE_LOG_ERROR
						log_error("rt_process:rt_locate: rt_temp_locate failure @2 !\n");
#endif
						return RT_LOCATE_FAILURE;
					}
#if ENABLE_LOG_DEBUG
					log_debug("rt_process:rt_locate: rt_temp_locate success @2 !\n");
#endif
				}
			}
			else
			{
				//set rt_check_temp_flag = 0 whenever a get done.
				rt_check_temp_flag = 0;
				rt_opt_flag = 1;

				rt_global_index = rt_global_index + rt_temp_index;
				result = rt_global_locate(rt_in.rt_con_tel_kp, &rt_global_index);
				if (result != RT_SUCCESS)
				{
#if ENABLE_LOG_ERROR
					log_error("rt_process:rt_locate: rt_global_locate failure @3 !\n");
#endif
					return RT_LOCATE_FAILURE;
				}
#if ENABLE_LOG_DEBUG
				log_debug("rt_process:rt_locate: rt_global_locate success @3 !\n");
#endif
			}
		}
		else
		{
			result = rt_temp_locate(rt_in.rt_con_tel_kp, &rt_index, p_node);
			if (result != RT_SUCCESS)
			{
#if ENABLE_LOG_ERROR
				log_error("rt_process:rt_locate: rt_temp_locate failure @3 !\n");
#endif
				return RT_LOCATE_FAILURE;
			}
#if ENABLE_LOG_DEBUG
			log_debug("rt_process:rt_locate: rt_temp_locate success @3 !\n");
#endif

		}
	}

	/*
	 * here starts the returns
	 */

	//value return for flags and indexes
	rt_flag.rt_opt_flag = rt_opt_flag;
	rt_flag.rt_check_temp_flag = rt_check_temp_flag;
	rt_flag.rt_ctr_flag = rt_ctr_flag;
	rt_index.rt_global_index = rt_global_index;
	rt_index.rt_temp_index = rt_temp_index;
	*rt_flag_p = rt_flag;
	*rt_index_p = rt_index;

	//value return for current_opt_result;
	*current_opt_result_p = current_opt_result;

#if ENABLE_LOG_DEBUG
	log_debug("rt_process:rt_locate: rt_locate success !\n");
#endif

	//return
	return RT_SUCCESS;
}

RT_RESULT rt_global_locate(float rt_con_tel_kp, int *rt_global_index_p)
{
	float global_located_post;
	float global_located_post_next;

	int rt_global_index = *rt_global_index_p;

	// locate the index in opt result
	if (rt_global_index == MAX_FINAL_LENGTH-1) //if reached the end of the opt result
	{
		*rt_global_index_p = rt_global_index;
#if ENABLE_LOG_DEBUG
		log_debug("rt_process:rt_locate:rt_global_locate: reached the end index: %d \n", rt_global_index);
#endif
		return RT_SUCCESS;
	}
	else
	{
		global_located_post = opt_final_curves[rt_global_index].con_tel_kp;
		global_located_post_next = opt_final_curves[rt_global_index+1].con_tel_kp;
		if (global_located_post <= rt_con_tel_kp && rt_con_tel_kp < global_located_post_next)
		{
			*rt_global_index_p = rt_global_index;
#if ENABLE_LOG_DEBUG
			log_debug("rt_process:rt_locate:rt_global_locate: the located index: %d \n", rt_global_index);
#endif
			return RT_SUCCESS;
		}

		//locate forward
		if (rt_con_tel_kp >= global_located_post_next)
		{
			while(1)
			{
				rt_global_index = rt_global_index + 1;
				if (rt_global_index <= MAX_FINAL_LENGTH) //if not pass the end of the opt result
				{
					global_located_post = opt_final_curves[rt_global_index].con_tel_kp;
					global_located_post_next = opt_final_curves[rt_global_index+1].con_tel_kp;

					if (rt_con_tel_kp >= global_located_post)
					{
#if ENABLE_LOG_DEBUG
						log_debug("rt_process:rt_locate:rt_global_locate: @1 locate post: %d \n", rt_con_tel_kp);
#endif
						break;
					}
				}
				else
				{
#if ENABLE_LOG_DEBUG
					log_debug("rt_process:rt_locate:rt_global_locate: locate failure @1 \n");
#endif
					return RT_LOCATE_FAILURE;
				}
			}
		}
		else
		{
			while(1)
			{
				rt_global_index = rt_global_index - 1;
				if (rt_global_index >= 0) //if not pass the beginning of the opt result
				{
					global_located_post = opt_final_curves[rt_global_index].con_tel_kp;
					global_located_post_next = opt_final_curves[rt_global_index+1].con_tel_kp;

					if (rt_con_tel_kp < global_located_post_next)
					{
#if ENABLE_LOG_DEBUG
						log_debug("rt_process:rt_locate:rt_global_locate: @2 locate post: %d \n", rt_con_tel_kp);
#endif
						break;
					}
				}
				else
				{
#if ENABLE_LOG_DEBUG
					log_debug("rt_process:rt_locate:rt_global_locate: locate failure @2 \n");
#endif
					return RT_LOCATE_FAILURE;
				}
			}
		}
	}
	/*
	 * here starts the returns
	 */
	*rt_global_index_p = rt_global_index;
#if ENABLE_LOG_DEBUG
	log_debug("rt_process:rt_locate:rt_global_locate: locate success \n");
#endif

	return RT_SUCCESS;
}

RT_RESULT rt_temp_locate(float rt_con_tel_kp, RT_INDEX* rt_index_p, LL_NODE* p_node)
{
	float temp_located_post;
	float temp_located_post_next;

	//get value of the flags
	RT_INDEX rt_index = *rt_index_p;

	// locate the index in opt result
	temp_located_post = opt_final_curves[rt_index.rt_temp_index].con_tel_kp;
	temp_located_post_next = opt_final_curves[rt_index.rt_temp_index+1].con_tel_kp;
	if (temp_located_post <= rt_con_tel_kp && rt_con_tel_kp < temp_located_post_next)
	{
#if ENABLE_LOG_DEBUG
		log_debug("rt_process:rt_locate:rt_temp_locate: @1 > located temp index: %d, post: %d \n", rt_index.rt_temp_index, rt_con_tel_kp);
#endif
		return RT_SUCCESS;
	}

	//locate forward
	if (rt_con_tel_kp >= temp_located_post_next)
	{
		while(1)
		{
			rt_index.rt_temp_index = rt_index.rt_temp_index + 1;
			temp_located_post = opt_final_curves[rt_index.rt_temp_index].con_tel_kp;
			temp_located_post_next = opt_final_curves[rt_index.rt_temp_index+1].con_tel_kp;

			if (rt_con_tel_kp >= temp_located_post)
			{
#if ENABLE_LOG_DEBUG
				log_debug("rt_process:rt_locate:rt_temp_locate: @2 > located temp index: %d, post: %d \n", rt_index.rt_temp_index, rt_con_tel_kp);
#endif
				break;
			}
		}
	}
	else
	{
		while(1)
		{
			rt_index.rt_temp_index = rt_index.rt_temp_index - 1;
			temp_located_post = opt_final_curves[rt_index.rt_temp_index].con_tel_kp;
			temp_located_post_next = opt_final_curves[rt_index.rt_temp_index+1].con_tel_kp;

			if (rt_con_tel_kp < temp_located_post_next)
			{
#if ENABLE_LOG_DEBUG
				log_debug("rt_process:rt_locate:rt_temp_locate: @3 <located temp index: %d, post: %d \n", rt_index.rt_temp_index, rt_con_tel_kp);
#endif
				break;
			}
		}
	}

	/*
	 * here starts the returns
	 */
	*rt_index_p = rt_index;
#if ENABLE_LOG_DEBUG
	log_debug("rt_process:rt_locate:rt_temp_locate success!\n");
#endif

	return RT_SUCCESS;
}

RT_RESULT  rt_manual_area_search(RT_INPUT rt_in, OPTSCHCURVE ahead_opt_result, AHEAD_EST_FLAGS* ahead_est_flags_p, MANUAL_AREA_INFO* ma_info_p)
{
	MANUAL_AREA_INFO ma_info = *ma_info_p;
	AHEAD_EST_FLAGS ahead_flags = *ahead_est_flags_p;

	if (ahead_flags.ma_check_flag == 0) //if no manual area marked
	{
		if (ahead_flags.ma_enter_flag == 0) // if not entering the area
		{
			if (ahead_opt_result.manual_flag == ADD_LOCO_AREA ||ahead_opt_result.manual_flag == RUN_THROUGH_TEST
					|| ahead_opt_result.manual_flag == AIR_DAMPING_BRAKE || ahead_opt_result.manual_flag == AIR_DAMPING_AVG)
			{
				ma_info.ma_flag = ahead_opt_result.manual_flag;
				ma_info.cont_ma_start = ahead_opt_result.con_tel_kp;
				ahead_flags.ma_check_flag = 1;
			}
		}
	}
	else //anti stop already marked
	{
		if (ahead_flags.ma_enter_flag == 0) // if not entering the area
		{
			//check entered or not
			if (rt_in.rt_con_tel_kp >= ma_info.cont_ma_start)
			{
				ahead_flags.ma_enter_flag = 1;
			}
		}
		else // entered
		{
			if (ahead_opt_result.manual_flag != ADD_LOCO_AREA && ahead_opt_result.manual_flag != RUN_THROUGH_TEST
					&& ahead_opt_result.manual_flag != AIR_DAMPING_BRAKE && ahead_opt_result.manual_flag != AIR_DAMPING_AVG)
			{
				ahead_flags.ma_check_flag = 0;
				ahead_flags.ma_enter_flag = 0;
			}
		}
	}

	*ma_info_p = ma_info;
	*ahead_est_flags_p = ahead_flags;

	return RT_SUCCESS;
}


//RT_RESULT rt_over_speed_est(RT_INPUT rt_in, OPTSCHCURVE current_opt_result, int rt_gear, OVER_SPEED_INFO* os_info_p)
//{
//	OVER_SPEED_INFO os_info;  //over speed
//	os_info.cont_os_start = 0;
//	os_info.os_flag = 0;
//
//	int delta_v_greater = 3;
//	int dager_gear = -5;
//
//	int opt_gear = current_opt_result.gear; // optimized gear
//
//	float opt_velocity = current_opt_result.velocity; // optimized velocity
//	float rt_velocity = rt_in.rt_velocity;
//
//	if (rt_velocity - opt_velocity >= delta_v_greater)
//	{
//		if (rt_gear <= dager_gear && opt_gear != -9 && opt_gear != -10)
//		{
//			os_info.os_flag = 1;
//			os_info.cont_os_start = 0;
//		}
//	}
//
//	*os_info_p = os_info;
//
//#if ENABLE_LOG_DEBUG
//	log_debug("rt_over_speed_est: estimate done at the post: %f\n", current_opt_result.con_tel_kp);
//#endif
//
//	return RT_SUCCESS;
//}
//
//
//RT_RESULT rt_gradeability_est(RT_INPUT rt_in, OPTSCHCURVE current_opt_result, int rt_gear, GRADE_ABILITY_INFO* ga_info_p)
//{
//	GRADE_ABILITY_INFO ga_info;  //over speed
//	ga_info.ga_flag = 0;
//	ga_info.cont_ga_start = 0;
//
//	int delta_v_greater = 3;
//	int dager_gear = 8;
//
//	int opt_gear = current_opt_result.gear; // optimized gear
//
//	float opt_velocity = current_opt_result.velocity; // optimized velocity
//	float rt_velocity = rt_in.rt_velocity;
//
//	if (rt_velocity - opt_velocity <= -delta_v_greater)
//	{
//		if ((rt_gear >= dager_gear) && (opt_gear == dager_gear))
//		{
//			ga_info.ga_flag = 1;
//			ga_info.cont_ga_start = 0;
//		}
//	}
//
//	*ga_info_p = ga_info;
//
//#if ENABLE_LOG_DEBUG
//	log_debug("rt_grade_ability_est: estimate done at the post: %f\n", current_opt_result.con_tel_kp);
//#endif
//
//	return RT_SUCCESS;
//}
//
//RT_RESULT rt_anti_stop_search(RT_INPUT rt_in, OPTSCHCURVE ahead_opt_result, AHEAD_EST_FLAGS* ahead_est_flags_p, ANTI_STOP_INFO* as_info_p)
//{
//	ANTI_STOP_INFO as_info = *as_info_p;
//	AHEAD_EST_FLAGS ahead_flags = *ahead_est_flags_p;
//
//	if (ahead_flags.as_check_flag == 0) //if no anti stop area marked
//	{
//		if (ahead_flags.as_enter_flag == 0) // if not entering the area
//		{
//			if (ahead_opt_result.anti_stop_flag == FORBID_STOP_ON_GRAD ||ahead_opt_result.anti_stop_flag == FORBID_STOP_CROSS_GRAD)
//			{
//				as_info.anti_stop_type = ahead_opt_result.anti_stop_flag;
//				as_info.con_post_start = ahead_opt_result.con_tel_kp;
//				ahead_flags.as_check_flag = 1;
//			}
//		}
//	}
//	else //anti stop already marked
//	{
//		if (ahead_flags.as_enter_flag == 0) // if not entering the area
//		{
//			//check entered or not
//			if (rt_in.rt_con_tel_kp >= as_info.con_post_start)
//			{
//				ahead_flags.as_enter_flag = 1;
//			}
//		}
//		else // entered
//		{
//			if (ahead_opt_result.anti_stop_flag != FORBID_STOP_ON_GRAD && ahead_opt_result.anti_stop_flag != FORBID_STOP_CROSS_GRAD)
//			{
//				ahead_flags.as_check_flag = 0;
//				ahead_flags.as_enter_flag = 0;
//			}
//		}
//	}
//
//	*as_info_p = as_info;
//	*ahead_est_flags_p = ahead_flags;
//
//	return RT_SUCCESS;
//}









