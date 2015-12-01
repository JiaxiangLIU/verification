/*
 * running_train.h
 *
 *  Created on: Jan 26, 2015
 *      Author: root
 */


#ifndef RUNNING_TRAIN_H_
#define RUNNING_TRAIN_H_



struct air_brake_change
{
	int opt_index;
	float delta_t;
};
typedef struct air_brake_change AIRBRAKECHANGE;


struct route_par
{
	int route_index;
//	int record_index;
};
typedef struct route_par ROUTE_PAR;

struct driving_record
{
	float start;  /**<公里标*/
	int gear;  /**<档位*/
	float velocity; /**<速度*/
	float time; /**<时间*/
	float energy;
};
typedef struct driving_record DRIVING_RECORD;


int getLine(FILE* fp);

int DoCalculateByTime_test(float start, float v, int gear, float tStep, int *count,
		float *delta_s, float *delta_v, float *delta_e);

float multiParticalModel_test(float start, int *count);

OPTIMIZERESULT* read_optSch();

//void save_driving_records(OPTIMIZERESULT* driving_records, int index);

RT_RESULT  running_train(RT_CORE rt_out, int *route_index_p, RT_INPUT* rt_in_p, DRIVING_RECORD* driving_record_p);

#endif /* RUNNING_TRAIN_H_ */
