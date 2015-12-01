/*
 * runningTrain.c
 *
 *  Created on: Oct 14, 2014
 *      Author: root
 */

//#include "controller.h"
#include "opt_global_var.h"
#include "opt_definition.h"
#include "rt_definition.h"
#include "rt_global_var.h"
#include "tractionCaculator.h"
#include "optimizeTools.h"
#include "rt_controller.h"
#include "rt_process.h"
#include "running_train.h"

//LOCOPARAMETER locoInfo;
//MGRADIENT* mGradients;
//OPTIMIZERESULT* optSch;
//int MAX_OPTSCHLENGTH;
//int MAX_MGRADIENT;
//float m_loco; // train head weight
//float l_loco; // train head length
//float l_car; //average length for a car
//float m_car; //average weight for a car

// for train running test
RT_RESULT running_train(RT_CORE rt_core, int *route_index_p, RT_INPUT* rt_in_p, DRIVING_RECORD* driving_record_p)
{
	int gear = rt_core.rt_opt.rt_gear;
//	OPTSCHCURVE curnt_opt_reslt = rt_out.rt_par.crnt_opt_reslt;

	RT_INPUT rt_in = *rt_in_p;
	DRIVING_RECORD driving_record = *driving_record_p;

	TRAC_CAL_RESULT cal_reslt;

	float t_step = 0.13;

	float delta_s = 0;
	float delta_v = 0;
	float delta_e = 0;

	if (gear == -9 || gear == -10)
	{
		float s_step = 10; //change it into v*t_step;

		rt_in.rt_con_tel_kp = rt_in.rt_con_tel_kp + s_step;
		rt_in.rt_velocity = (float)rt_core.rt_comm.speed_and_location.speed;

		printf("post: %f, gear: %d, route_index: %d, delta_s: %f, delta_v: %f, delta_e: %f\n",
				rt_in.rt_con_tel_kp, gear, *route_index_p, s_step, delta_v, delta_e);

		*rt_in_p = rt_in;

		//keep the record
		driving_record.start = rt_in.rt_con_tel_kp;
		driving_record.velocity = rt_in.rt_velocity; ///////////////////////remember to change it into the optimized velocity
		driving_record.gear = gear;
		driving_record.time = driving_record.time + t_step;
		driving_record.energy = driving_record.energy + delta_e; //delta_e = 0 in such situation
	}
	else
	{
		//call the calculation
		cal_reslt =	DoCaculateByTime(rt_in.rt_con_tel_kp, rt_in.rt_velocity, gear, t_step, route_index_p, &delta_s, &delta_v, &delta_e);

		printf("post: %f, gear: %d, route_index: %d, delta_s: %f, delta_v: %f, delta_e: %f\n", rt_in.rt_con_tel_kp, gear, *route_index_p, delta_s, delta_v, delta_e);

		rt_in.rt_con_tel_kp = rt_in.rt_con_tel_kp + delta_s;
		rt_in.rt_velocity = rt_in.rt_velocity + delta_v;

		//keep the record
		driving_record.start = rt_in.rt_con_tel_kp;
		driving_record.velocity = rt_in.rt_velocity;
		driving_record.gear = gear;
		driving_record.time = driving_record.time + t_step;
		driving_record.energy = driving_record.energy + delta_e;
	}

	*driving_record_p = driving_record;
	*rt_in_p = rt_in;

	return RT_SUCCESS;
}

//void train_running_and_keep_record(OPTIMIZERESULT* driving_records,float headX,int gear, float current_t,
//		float v, float delta_s, float delta_v, float delta_e, float delta_t, int control_count, float t_step, int gradient_count)
//{
//	//record
//	if (index ==0){
//		driving_records = (OPTIMIZERESULT*)malloc(sizeof(OPTIMIZERESULT));
//		if(driving_records==NULL)
//		{
//			printf("runningTrain.c:  driving_records allocate  memory error-0\n");
//			exit(0);
//		}
//		driving_records[index].gear = gear;
//		driving_records[index].start = headX;
//		driving_records[index].time = current_t;
//		driving_records[index].velocity = v;
//		index = index + 1;
//	} else{
//		driving_records = (OPTIMIZERESULT*) realloc(driving_records, (index+1) * sizeof(OPTIMIZERESULT));
//		if(driving_records==NULL)
//		{
//			printf("runningTrain.c:  driving_records allocate  memory error-1\n");
//			exit(0);
//		}
//		driving_records[index].gear=gear;
//		driving_records[index].start=headX;
//		driving_records[index].time = current_t;
//		driving_records[index].velocity = v;
//		index = index + 1;
//	}
//
//	// for air brake working status
//	if (gear == -10 || gear == -9) {
//		delta_t = 0;
//		while(1){
//			if (control_count < MAX_OPTSCHLENGTH ){
//				if (gear == optSch[control_count + 1].gear){
//					control_count = control_count + 1;
//					delta_t = delta_t + optSch[control_count].time;
//				} else {
//					break;
//				}
//			} else {
//				control_count = MAX_OPTSCHLENGTH;
//				break;
//			}
//		}
//		// do record
//		headX = optSch[control_count].start;
//		current_t = current_t + delta_t;
//		v = optSch[control_count].velocity;
//
//		driving_records = (OPTIMIZERESULT*) realloc(driving_records, (index+1) * sizeof(OPTIMIZERESULT));
//		if(driving_records==NULL)
//		{
//			printf("runningTrain.c:  driving_records allocate  memory error-1\n");
//			exit(0);
//		}
//		driving_records[index].gear=gear;
//		driving_records[index].start=headX;
//		driving_records[index].time = current_t;
//		driving_records[index].velocity = v;
//		index = index + 1;
//		control_count = control_count + 1;
//
//		// record one more no matter airbrake continues or not
//		gear = optSch[control_count].gear;
//		headX = optSch[control_count].start;
//		v = optSch[control_count].velocity;
//		current_t = current_t + optSch[control_count].time;
//
//		driving_records = (OPTIMIZERESULT*) realloc(driving_records, (index+1) * sizeof(OPTIMIZERESULT));
//		if(driving_records==NULL)
//		{
//			printf("runningTrain.c:  driving_records allocate  memory error-1\n");
//			exit(0);
//		}
//		driving_records[index].gear=gear;
//		driving_records[index].start=headX;
//		driving_records[index].time = current_t;
//		driving_records[index].velocity = v;
//		index = index + 1;
//	}else{
//		float delta_s_sum = 0;
//		while (1) {
//			if (delta_s_sum < 1){
//				DoCalculateByTime_test(headX, v, gear, t_step, &gradient_count, &delta_s, &delta_v,
//						&delta_e);
//				delta_s_sum = delta_s_sum + delta_s;
//			}else{
//				break;
//			}
//		}
//		headX = headX + delta_s;
//		v = v + delta_v;
//		current_t = current_t + t_step;
//	}
//}
//
//// for train model running on board
//void running_train_ob(float *post_input, int *v_input, int gear_input, int *count, float *delta_t) {
//	// parameters initial
//	float headX, v;
//	int round_v;
//	int gear;
//	int control_count;
//	float delta_s, delta_v, delta_e, delta_t_tmp;
//	int gradient_count = 0;
//	int index = 0;
//	float current_t = 0;
//
//	float t_step = 0.13;
//
//	control_count = *count;
//	headX = *post_input;
//	v = *v_input;
//
//	// Read optimized result
//	OPTIMIZERESULT* optSch =read_optSch();
//
//	if (gear == -10 || gear == -9) { // for air brake working status
//		printf("Airbrake working for a while");
//		delta_t_tmp = 0;
//		while(1){
//			if (control_count < MAX_OPTSCHLENGTH ){
//				if (gear == optSch[control_count + 1].gear){
//					control_count = control_count + 1;
//					delta_t_tmp = delta_t_tmp + optSch[control_count].time;
//				} else {
//					break;
//				}
//			} else {
//				control_count = MAX_OPTSCHLENGTH;
//				break;
//			}
//		}
//		control_count = control_count + 1;
//		headX = optSch[control_count].start;
//		v = optSch[control_count].velocity;
//	}else{
//		float delta_s_sum = 0;
//		delta_t_tmp = 0;
//		while (1) {
//			if (delta_s_sum < 1){
//				DoCalculateByTime_test(headX, v, gear, t_step, &gradient_count, &delta_s, &delta_v,
//						&delta_e);
//				delta_s_sum = delta_s_sum + delta_s;
//				delta_t_tmp = delta_t_tmp + t_step;
//			}else{
//				break;
//			}
//		}
//		headX = headX + delta_s;
//		v = v + delta_v;
//	}
//
//	*count = control_count;
//	*post_input = headX;
//	*v_input = v;
//	*delta_t = delta_t_tmp;
//}



//int DoCalculateByTime_test(float start, float v, int gear, float tStep, int *count,
//		float *delta_s, float *delta_v, float *delta_e) {
//	int count_tmp = *count;
//	//优化后的参数
//	float w_o = locoInfo.reDavis[0] * v * v + locoInfo.reDavis[1] * v
//			+ locoInfo.reDavis[2];
//	float gradient = multiParticalModel_test(start, &count_tmp);
//	*count  = count_tmp;
//	float w = w_o + gradient; //总阻力
//	float df = getForce(gear, v);
//	//	float dft = 1000/G*df/(m_loco + locoInfo.totalWeight); //转换后的最大牵引力
//	float dft = 1000 / G * df / locoInfo.totalWeight; //转换后的最大牵引力
//	float c = dft - w;
//	float acc = c / 30;
//	*delta_v = acc * tStep;
//	*delta_s = (v / 3.6) * tStep;
//	*delta_e = getConsumption(gear, tStep);
//	return 1;
//}

//float multiParticalModel_test(float start, int *count) {
////	m_loco = locoInfo.weight[0]; // train head weight
////	l_loco = locoInfo.length[0]; // train head length
//	m_loco = 0; // train head weight
//	l_loco =0; // train head length
////	l_car = (locoInfo.totalLength - l_loco) / locoInfo.count; //average length for a car
////	m_car = (locoInfo.totalWeight - m_loco) / locoInfo.count; //average weight for a car
//	float w_loco = 0;
//	float x_loco = start - l_loco / 2;
//	float grad = 0;
//	if (x_loco < (float)(mGradients[*count].start)) {   //前行方向为前方，反向为后方
//		if (*count == 0) //首段后方
//			grad = 0;
//		else {
//			while (1) {
//				*count = *count - 1; //后移至相应段
//				if (x_loco > (float)mGradients[*count].start || fabs(x_loco-(float)mGradients[*count].start)<PRECISION) {
//					grad = mGradients[*count].value;
//					break;
//				} else {
//					if (*count == 0) {
//						grad = 0;
//						break;
//					}
//				}
//			}
//		}
//	} else {
//		if (x_loco >(float) mGradients[*count].end) {
//			if (*count == MAX_MGRADIENT - 1) //末端前方
//				grad = 0;
//			else {
//				while (1) {
//					*count = *count + 1;
//					if (x_loco <  (float)mGradients[*count].end || fabs(x_loco-(float)mGradients[*count].end)<PRECISION) {
//						grad = mGradients[*count].value;
//						break;
//					} else {
//						if (*count == MAX_MGRADIENT - 1) {
//							grad = 0;
//							break;
//						}
//					}
//				}
//			}
//		} else
//			grad = mGradients[*count].value;
//	}
//	w_loco = w_loco + m_loco * grad;
//	int count_car = *count;
//	l_car=carStruct[0].carLength;
//	float x_car = x_loco - l_loco / 2 - l_car / 2;
//	float w_cars = 0;
//	float temp_start = (float)mGradients[count_car].start;
//	float temp_end =(float) mGradients[count_car].end;
//	int i = 0;
//	for (i = 0; i < locoInfo.count; i++) {    //第n个car坡度阻力
//		if ((x_car > temp_start || fabs(x_car-temp_start) <PRECISION ) && (x_car <  temp_end || fabs(x_car-temp_end)<PRECISION))
//			grad = mGradients[count_car].value;
//		else {
//			if (x_car < temp_start)
//			//前行方向为前方，反向为后方
//					{
//				if (count_car == 0) //首段后方
//					grad = 0;
//				else {
//					while (1) {
//						count_car = count_car - 1; //后段移至相应段
//						if (x_car > (float) mGradients[count_car].start || fabs(x_car - (float) mGradients[count_car].start )< PRECISION) {
//							grad = mGradients[count_car].value;
//							break;
//						} else {
//							if (count_car == 1) {
//								grad = 0;
//								break;
//							}
//						}
//					}
//					temp_start = (float)mGradients[count_car].start;
//					temp_end = (float)mGradients[count_car].end;
//				}
//			} else {
//				if (x_car > (float)mGradients[count_car].end) {
//					if (count_car == MAX_MGRADIENT - 1) //末端前方
//						grad = 0;
//					else {
//						while (1) {
//							count_car = count_car + 1;
//							if (x_car <  (float)mGradients[count_car].end || fabs(x_car - (float)mGradients[count_car].end)< PRECISION) {
//								grad = mGradients[count_car].value;
//								break;
//							} else {
//								if (count_car == MAX_MGRADIENT - 1) {
//									grad = 0;
//									break;
//								}
//							}
//						}
//						temp_start = (float)mGradients[count_car].start;
//						temp_end = (float)mGradients[count_car].end;
//					}
//				}
//			}
//		}
//        m_car=carStruct[i].carWeight+carStruct[i].loadWeight;
//		float w_car = m_car * grad;
//		w_cars = w_cars + w_car;
//		if(i+1!=CARNUMS)
//	        	l_car=carStruct[i+1].carLength;
//		x_car = x_car - l_car;
//	}
//	return (w_loco + w_cars) / (locoInfo.totalWeight);
//	return 1;
//}

////get optSch from file
//OPTIMIZERESULT* read_optSch(){
//
//	OPTIMIZERESULT* optSch;
//	int i;
//	int line;
//	FILE* fid = fopen("./optimizeResult","r");
//	if(fid==NULL){
//		fprintf(stderr,"open curve err!\n");
//	}
//	line = getLine(fid);
//	MAX_OPTSCHLENGTH = line - 1;
//	optSch = (OPTIMIZERESULT*) malloc(sizeof(OPTIMIZERESULT)*line);
//	for(i=0;i<line;i++)
//	{
//		fscanf(fid,"%f%d%f%f",
//				&optSch[i].start,&optSch[i].gear,
//				&optSch[i].velocity,&optSch[i].time);
//	}
//
//	return optSch;
//
//}
//
//void save_driving_records(OPTIMIZERESULT* driving_records, int index){
//
//	int i;
//	FILE* fid = fopen("./driving_records", "w");
//	if (fid == NULL) {
//		fprintf(stderr, "open driving_records file err!\n");
//		exit(0);
//	}
//
//	for (i = 0; i < index; i++) {
//		fprintf(fid, "%d\t%f\t%d\t%f\t%f\n", i, driving_records[i].start,
//				driving_records[i].gear, driving_records[i].velocity,
//				driving_records[i].time);
//	}
//
//}

