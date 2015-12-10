/*
 * rt_main.c
 *
 *  Created on: AUGEST 5, 2015
 *      Author: root
 */

#include "opt_definition.h"
#include "opt_global_var.h"
#include "rt_definition.h"
#include "inter_rtopt_definition.h"
#include "rt_global_var.h"
#include "rt_process.h"
#include "running_train.h"
#include "linked_list_operations.h"
#include "realtime_data.h"
#include "error_code.h"
#include "log.h"
#include "abnormal_code.h"
#include "led.h"

// #if definition for #if use

#if !TESTING
#include "protocal.h"
#endif

#if TESTING
void save_driving_records(DRIVING_RECORD* driving_records, int record_index);
#else
void save_output_records(RT_CORE rt_output_records[], int record_index);
#endif

int rt_main() {
	//declaration
	RT_RESULT result = RT_SUCCESS; // return result for functions.
	RT_CORE rt_core; // the output for rt_process
	RT_INPUT rt_in; //the input for rt_process
	RT_INOUT rt_inout; //the parameters in and then out
	RT_FLAG rt_flag; //the flags in rt_process
	RT_INDEX rt_index; //the indexes in rt_process

	// parameter for estimations
	RT_INOUT ahead_rt_inout; //the parameters in and then out
	RT_FLAG ahead_rt_flag; //the flags in rt_process
	RT_INDEX ahead_rt_index; //the indexes in rt_process

//	//initialize rt_out
//	RT_PAR rt_par; //the current control gear
//	rt_par.opt_gear = 0;
//	rt_par.opt_v = 0;
//	rt_par.rt_cont_post = 0;
//	rt_par.rt_gear = 0;
//	rt_par.rt_light = SIG_GREEN_LIGHT;
//	rt_par.tele_post.cross_dist = 0;
//	rt_par.tele_post.tel_kp = 0;
//	rt_par.tele_post.tel_num = 0;
//	rt_status.rt_loco_working_status = RT_WAITING;
//	rt_status.rt_trip_status = RT_DUTY;
//	rt_status.rt_airbrake_status = BRAKE_RESERVED;
//	NEXT_STATION_INFO ns_info; //next station
//	ns_info.cont_ns_start = 0;
//	ns_info.ns_id = 0;
//	ns_info.t_to_ns = 0;
//	OVER_SPEED_INFO os_info;  //over speed
//	os_info.cont_os_start = 0;
//	os_info.os_flag = 0;
//	GRADE_ABILITY_INFO ga_info; //grade ability
//	ga_info.cont_ga_start = 0;
//	ga_info.ga_flag = 0;
//	ANTI_STOP_INFO as_info; //anti stop
//	as_info.anti_stop_type = RT_ANTI_STOP_NULL;
//	as_info.con_post_start = 0;
//	as_info.con_post_end = 0;

	RT_STATUS rt_status; //status
	rt_status.rt_enable_status = RT_AUTO_UNABLE;
	rt_status.rt_post_status = BEFORE_OPR_AREA;
//	MANUAL_AREA_INFO ma_info;
//	ma_info.ma_flag = 0;
//	ma_info.cont_ma_start = 0;

	RT_OPT rt_opt;
	rt_opt.opt_gear = 0;
	rt_opt.opt_v = 0;
	rt_opt.rt_gear = 0;

	rt_core.rt_opt = rt_opt;
	rt_core.rt_status = rt_status;

//	rt_out.rt_par = rt_par;
//	rt_out.ga_info = ga_info;
//	rt_out.ns_info = ns_info;
//	rt_out.os_info = os_info;
//	rt_out.as_info = as_info;
//	rt_out.ma_info = ma_info;
//	rt_out.rt_status = rt_status;


	//initialize rt_in
	rt_in.rt_con_start_post = opt_final_curves[0].con_tel_kp; //start post and end post of continuous type
	rt_in.rt_con_end_post = opt_final_curves[MAX_FINAL_LENGTH - 1].con_tel_kp; //start post and end post of continuous type
#if TESTING
	//	rt_in.rt_con_tel_kp = 0.0; //real time post of continuous type
	rt_in.rt_con_tel_kp = rt_in.rt_con_start_post + 10;
	rt_in.rt_velocity = 20.0; //real time velocity
	rt_in.rt_airbrake_status = BRAKE_RESERVED;
	rt_in.rt_light = SIG_GREEN_LIGHT;
#endif

	//initialize rt_inout
	rt_inout.rt_temp_start = 0.0;
	rt_inout.rt_temp_end = 0.0;

	//initialize rt_index
	rt_index.rt_global_index = 0; //index in globle opt result
	rt_index.rt_temp_index = 0; //index in temp opt result

	//initialize rt_flag
	rt_flag.rt_opt_flag = 1; //is current loop in globle opt? yes = RT_GLOBAL_OPT, no = RT_TEMP_OPT.
	rt_flag.rt_ctr_flag = 0; // flag for Run as optimized
	rt_flag.rt_check_temp_flag = 0; //


	/*
	 * initialization for estimations
	 */
	//initialize rt_inout
	ahead_rt_inout.rt_temp_start = 0.0;
	ahead_rt_inout.rt_temp_end = 0.0;

	//initialize rt_index
	ahead_rt_index.rt_global_index = 0; //index in globle opt result
	ahead_rt_index.rt_temp_index = 0; //index in temp opt result

	//initialize rt_flag
	ahead_rt_flag.rt_opt_flag = 1; //is current loop in globle opt? yes = RT_GLOBAL_OPT, no = RT_TEMP_OPT.
	ahead_rt_flag.rt_ctr_flag = 0; // flag for Run as optimized
	ahead_rt_flag.rt_check_temp_flag = 0; //

	AHEAD_EST_FLAGS ahead_est_flags;
//	ahead_est_flags.as_check_flag = 0;
//	ahead_est_flags.as_enter_flag = 0;
	ahead_est_flags.ma_check_flag = 0;
	ahead_est_flags.ma_enter_flag = 0;

	/*
	 * initialize the linked list
	 */
	LL_RESULT ll_result;
	ll_result = ll_create();
	if (ll_result != LL_SUCCESS) {
#if ENABLE_LOG_ERROR
		//		printf("linked list initialization error");
		log_error("real time process error: %s\n", error_code_name(ERROR_CODE_MUTEX_INIT));
#endif
		return ERROR_CODE_MUTEX_INIT;
	}

	log_info("rt_main initialization success!\n");

	//the records index
	int records_length = 10000;
	int record_index = 0;

#if !TESTING
	RT_CORE* rt_output_records;
	if ((rt_output_records = (RT_CORE*) malloc(sizeof(RT_CORE) * records_length)) == NULL)
	{
#if ENABLE_LOG_ERROR
		log_error("rt_main error: %s\n", error_code_name(ERROR_CODE_MALLOC));
#endif
		return ERROR_CODE_MALLOC;
	}

#else
	/*
	 * initialization for train model
	 */
	int route_index = 0;

	DRIVING_RECORD driving_record;
	driving_record.start = 0;
	driving_record.gear = 0;
	driving_record.time = 0;
	driving_record.velocity = 0;
	driving_record.energy = 0;
	DRIVING_RECORD* driving_records;
	if ((driving_records = (DRIVING_RECORD*)malloc(sizeof(DRIVING_RECORD)*records_length)) == NULL)
	{
#if ENABLE_LOG_ERROR
		log_error("rt_main error: %s\n", error_code_name(ERROR_CODE_MALLOC));
#endif
		return ERROR_CODE_MALLOC;
	}

#endif

	/*
	 * here starts the real time process
	 */
	int loop = 1;
	while (1) {

		//		printf("here starts loop: %d\n", loop);
#if ENABLE_LOG_INFO
		log_info("rt_main real time loop: %d\n", loop);
#endif

#if !TESTING
		// wait until real time data received
		// jiaxiang: 不太懂这里是干嘛？
		// jiaxiang: 行程开始前的优化计算在哪里进行，存在哪里？
		pthread_mutex_lock(&(rt_arr_flag.rt_mutex));
		while (!rt_arr_flag.rt_flag) {
			pthread_cond_wait(&(rt_arr_flag.rt_cond), &(rt_arr_flag.rt_mutex));
		}

		rt_arr_flag.rt_flag = 0;
		/*将realtime的公里标和速度赋给rt_process的输入*/
		rt_in.rt_con_tel_kp =(float) rt_comm.speed_and_location.new_kilmeter_coordinate;
		rt_in.rt_velocity = (float)rt_comm.speed_and_location.speed;
		rt_in.rt_airbrake_status = rt_comm.brake_out;
		rt_in.rt_light = rt_comm.train_sig;
//		rt_in.tele_post.tel_num = rt_data.speed_and_location.singnal_equipment_No;
//		rt_in.tele_post.tel_kp = rt_data.speed_and_location.kilmeter_coordinate;
//		rt_in.tele_post.cross_dist = rt_data.speed_and_location.distance;

		// copy rt_comm into rt_core
		rt_core.rt_comm = rt_comm;

		pthread_mutex_unlock(&(rt_arr_flag.rt_mutex));
		//		printf("here starts loop: %d loc: %d speed: %d\n", loop,
		//				rt_data.speed_and_location.new_kilmeter_coordinate,
		//				rt_data.speed_and_location.speed);
#if ENABLE_LOG_INFO
		log_info("here starts loop: %d, loc: %d, speed: %d\n", loop, rt_data.speed_and_location.new_kilmeter_coordinate,
				rt_data.speed_and_location.speed);
#endif

#endif


		/*
		 * call real time process function
		 */
		loop = loop + 1;

		result = rt_process(rt_in, &rt_inout, &rt_flag, &rt_index, &ahead_rt_inout, &ahead_rt_flag, &ahead_rt_index, &ahead_est_flags, &rt_core);

		log_debug("rt_main: rt_process finished loop: %d\n", loop-1);
		//		printf("rt_process finished loop: %d\n", loop-1);

//		printf("over speed flag: %d, rt_velocity: %f, opt_velocity: %f \n", rt_out.os_info.os_flag, rt_in.rt_velocity, rt_out.rt_par.opt_v);
//
//		if(rt_out.os_info.os_flag == 1)
//		{
//			break;
//		}

//#if !TESTING
//		rt_out.rt_par.rt_post=rt_data.speed_and_location.kilmeter_coordinate;
//#endif

		if (result != RT_SUCCESS)
		{
#if ENABLE_LOG_ERROR
			log_error("rt_main: rt_process not running properly!\n");
			//			break;
#endif
			set_abnormal_code(SYS_ERROR_OPTIMIZE_FAILURE, VOTE_0_DES);
			led_on(OPT_LED);
		}

#if !TESTING

		char frame_cmd[2];
		char frame_arg[200];
		int frame_size;
		frame_cmd[0] = 0x02;
		frame_cmd[1] = 0xD1;

//#if ENABLE_LOG_PRINTF
//		printf("档位: %d\n", rt_out.rt_par.rt_gear);
//#endif

		frame_size = sizeof(RT_CORE);
		memcpy(frame_arg, &rt_core, frame_size);


		// jiaxiang: 将计算结果发送出去
		frame_encap(0x40, 0x008, DATA_FRAME_CODE, frame_cmd, frame_arg,
				frame_size);

#if ENABLE_LOG_INFO
		log_info("rt_main frame_encap success, loop: %d\n", loop-1);
#endif

		/*
		 * keep the output records
		 */
		// jiaxiang: 这里应该是把计算结果存起来吧？
		if (record_index > records_length - 1) //if reach the max length, double the length
		{
			records_length = 2 * records_length;
			// jiaxiang: bug? 新申请的内存会导致之前的record都丢失吧？
			if ((rt_output_records = (RT_CORE*) malloc(sizeof(RT_CORE) * records_length)) == NULL)
			{
#if ENABLE_LOG_ERROR
				log_error("rt_main error: %s\n", error_code_name(ERROR_CODE_MALLOC));
#endif
				return ERROR_CODE_MALLOC;
			}

		}
		rt_output_records[record_index] = rt_core;
		record_index = record_index + 1;
#else // jiaxiang: 这里是测试代码

		//1 for test
		//0 for core_board using

		/////////////////////////
		//add the current_opt_result into running_train
		////////////////////////
		/*
		 * call the train model for returning the train running status
		 */
		result = running_train(rt_core, &route_index, &rt_in, &driving_record);

		/*
		 * break condition: if not running right
		 */
		if(result != RT_SUCCESS)
		{
#if ENABLE_LOG_PRINTF
			printf("Program error");
#endif
			set_abnormal_code(SYS_ERROR_CORE_SOFTWARE_ERROR, VOTE_0_DES);
			led_on(OPT_LED);
			break;
		}

		/*
		 * keep the records
		 */
		if (record_index > records_length - 1) //if reach the max length, double the length
		{
			records_length = 2*records_length;
			// jiaxiang: bug? 跟上面问题类似
			if ((driving_records = (DRIVING_RECORD*)malloc(sizeof(DRIVING_RECORD)*records_length)) == NULL)
			{
#if ENABLE_LOG_ERROR
				log_error("rt_main error: %s\n", error_code_name(ERROR_CODE_MALLOC));
#endif
				return ERROR_CODE_MALLOC;
			}
		}
		driving_records[record_index] = driving_record;
		record_index = record_index + 1;
#endif

		/*
		 * break condition: if the train is out of the optimized scope
		 */
		if (rt_in.rt_con_tel_kp < rt_in.rt_con_start_post || rt_in.rt_con_tel_kp > rt_in.rt_con_end_post)
		{
#if ENABLE_LOG_PRINTF
			printf("Out of the optimized scope, rt_con_post: %f\n", rt_in.rt_con_tel_kp);
			log_warning("Out of the optimized scope, rt_con_post: %f\n", rt_in.rt_con_tel_kp);
#endif

#if TESTING
			break;
#endif
		}

	} // jiaxiang: 大循环结束

#if !TESTING
	//saving the output records
	save_output_records(rt_output_records, record_index);
	free(rt_output_records);
#if ENABLE_LOG_DEBUG
	log_debug("memory of rt_output_records freed!\n");
#endif

#else
	//1 for test
	//0 for core_board using
	//saving the testing driving records from the train model
	save_driving_records(driving_records, record_index);
	free(driving_records);
#if ENABLE_LOG_DEBUG
	log_debug("memory of driving_records freed!\n");
#endif
#endif

#if ENABLE_LOG_INFO
	log_info("real time process finished!\n");
#endif
	return 0;

}

#if TESTING

void save_driving_records(DRIVING_RECORD* driving_records, int record_index)
{

	int i;
	FILE* fid = fopen("././driving_records.txt", "w");
	if (fid == NULL) {
		fprintf(stderr, "open driving_records file err!\n");
#if ENABLE_LOG_ERROR
		log_error("open driving_records file err!\n");
#endif
		exit(0);
	}

	for (i = 0; i < record_index; i++) {
		fprintf(fid, "%d\t%f\t%d\t%f\t%f\t%f\n", i, driving_records[i].start,
				driving_records[i].gear, driving_records[i].velocity,
				driving_records[i].time, driving_records[i].energy);
	}
#if ENABLE_LOG_INFO
	log_info("save_driving_records done\n");
#endif
}

#else

void save_output_records(RT_CORE rt_output_records[], int record_index) {

	int i;
	FILE* fid = fopen("././rt_output_records.txt", "w");
	if (fid == NULL) {
		fprintf(stderr, "open rt_output_records file err!\n");
#if ENABLE_LOG_ERROR
		log_error("open rt_output_records file err!\n");
#endif

		exit(0);
	}

	for (i = 0; i < record_index; i++) {
		fprintf(fid, "%d\t%d\t%d\t%d\n", i,
		rt_output_records[i].rt_comm.speed_and_location.new_kilmeter_coordinate,
		rt_output_records[i].rt_comm.speed_and_location.speed,
		rt_output_records[i].rt_opt.rt_gear);
	}
#if ENABLE_LOG_PRINTF
	printf("save_output_records done\n");
#endif
}

#endif

