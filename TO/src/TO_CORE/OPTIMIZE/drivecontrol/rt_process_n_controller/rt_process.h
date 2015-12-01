/*
 * realtime_process.h
 *
 *  Created on: Jan 26, 2015
 *      Author: root
 */

#ifndef REALTIME_PROCESS_H_
#define REALTIME_PROCESS_H_


///*
// *@brief	initialization for real time process
// */
//RT_RESULT rt_initialize();

/*
 * @brief	locate the right post in optimized result
 * @param
 */
RT_RESULT rt_locate(RT_INPUT rt_in, RT_INOUT* rt_inout_p, RT_FLAG* rt_flag_p, RT_INDEX* rt_index_p, OPTSCHCURVE* current_opt_result_p);

/*
 * @brief	locate the right post in optimized result
 * @param
 */
RT_RESULT rt_global_locate(float rt_con_tel_kp, int *rt_global_index);

/*
 * @brief	locate the right post in temp optimized result
 * @param
 */
RT_RESULT rt_temp_locate(float rt_con_tel_kp, RT_INDEX* rt_index_p, LL_NODE* p_node);


///*
// * @brief	over speed estimation in front
// * @param
// */
//RT_RESULT rt_over_speed_est(RT_INPUT rt_in, OPTSCHCURVE current_opt_result, int rt_gear, OVER_SPEED_INFO* os_info);
//
///*
// * @brief	uphill gradeability estimation
// * @param
// */
//RT_RESULT rt_gradeability_est(RT_INPUT rt_in, OPTSCHCURVE current_opt_result, int rt_gear, GRADE_ABILITY_INFO* ga_info);
//
//
//
//RT_RESULT rt_anti_stop_search(RT_INPUT rt_in, OPTSCHCURVE ahead_opt_result, AHEAD_EST_FLAGS* ahead_est_flags, ANTI_STOP_INFO* as_info);


RT_RESULT  rt_manual_area_search(RT_INPUT rt_in, OPTSCHCURVE ahead_opt_result, AHEAD_EST_FLAGS* ahead_est_flags, MANUAL_AREA_INFO* ma_info);

/*
 * @brief	rt_process
 * @param	rt_con_start_post -	start post of continuous type for real time control
 * @param	rt_con_end_post	- end post of continuous type for real time control
 * @param	rt_con_tel_kp - current post of continuous type for real time control
 * @param	rt_velocity - real time velocity
 * @param	rt_out - the output of the real time process
 * @return	program running status
 */
RT_RESULT rt_process(RT_INPUT rt_in, RT_INOUT* rt_inout_p, RT_FLAG* rt_flag_p, RT_INDEX* rt_index_p, RT_INOUT* ahead_rt_inout_p,
		RT_FLAG* ahead_rt_flag_p, RT_INDEX* ahead_rt_index_p, AHEAD_EST_FLAGS* ahead_est_flags, RT_CORE* rt_out_p);


#endif /* REALTIME_PROCESS_H_ */
