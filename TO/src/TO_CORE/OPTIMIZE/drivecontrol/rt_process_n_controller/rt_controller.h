/*
 * controller.h
 *
 *  Created on: Oct 14, 2014
 *      Author: root
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_


#include "optimizeTools.h"
#include "rt_controller.h"


RT_RESULT rt_controller(OPTSCHCURVE current_opt_result, RT_INPUT rt_in, RT_FLAG* rt_flag_p, int *gear_p);


//void find_count(OPTIMIZERESULT* optSch, int *count, float *opt_v, float headX);
//
//float evaluate_avg();

#endif /* CONTROLLER_H_ */
