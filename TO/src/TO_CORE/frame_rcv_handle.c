
/**
 * @file frame_rcv_handle.c
 * @brief 进行frame接收后的操作
 * @author danny
 * @date 2015.02.03
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

#include "comm_check.h"
#include "handle.h"
#include "global.h"
#include "to_event.h"
#include "schedule.h"
#include "route_data.h"
#include "realtime_data.h"
#include "protocal.h"
#include "to_status.h"
#include "led.h"
#include "rt_definition.h"
#include "to_time.h"
#include "ack_handle.h"


void command_frame_rcv_handle(void *cmd)
{
    char *command = (char *)cmd;
	int temp = 0;
    struct event_t *new_event = NULL;
    switch (command[0]) {
       case SOURCE_VOTE_0:
	   		switch (command[1]) {
				case EVENT_BROADCAST_COM_ADDR:
					if (command[2] == SOURCE_COM_0)
						temp = COM_0_DES;
					else if (command[2] == SOURCE_COM_1)
						temp = COM_1_DES;
					set_commu_addr(temp);
					log_debug("recv master commu addr %d.\n", temp);
					break;
				case EVENT_MASTER_VOTE_EXCHANGE:
					if (command[2] < SYS_MAX) {
						set_sys_status((SYSTEM_STATUS)command[2]);
						log_debug("recv sys status %d.\n", command[2]);
					} else
						log_warning("recv sys status error.\n");

					if (get_sys_status() == SYS_LAUNCHED)
						led_on(REGISTER_LED);
						
					break;
				//case EVENT_BOARD_REGISTER_SUCCESS:
				//	break;
				case EVENT_SYS_TRIP_STATUS_SEND:
					if (command[2] < TRIP_MAX) {
						set_trip_status((TRIP_STATUS)command[2]);
						log_debug("recv trip status %d.\n", command[2]);
					} else
						log_warning("recv trip status error.\n");

					break;
				//case EVENT_VOTE_BRANCH_TRANSFER_PASS:
				//case EVENT_VOTE_ROUTE_TRANSFER_PASS:
					/*new_event = event_create(EVENT_ROUTE_INIT, CONTROL_BOX, command[0]);
					if (new_event == NULL)
					log_error("system err: failed to creat event.\n");
					else
						add_command(new_event);*/
				//	break;
				//case EVENT_VOTE_SIDE_LINE_PASS:
				//	break;
				default:
					break;
	   		}
            break;
		default:
			break;
    }
	free(cmd);
}

static int recv_marsh_flag = -1;
static int recv_route_flag = -1;

static int get_marsh_flag(void)
{
	return recv_marsh_flag;
}

static void set_marsh_flag(int value)
{
	recv_marsh_flag = value;
}

static int get_route_flag(void)
{
	return recv_route_flag;
}

static void set_route_flag(int value)
{
	recv_route_flag = value;
}

static int route_version_check(int version)
{
	return -1;
}

static MARSHALLING_INFO recv_marshalling;
pthread_mutex_t marshalling_mutex = PTHREAD_MUTEX_INITIALIZER;


static int marsh_data_rcv_handle(char *data, int length)
{
	pthread_mutex_lock(&marshalling_mutex);

    memcpy(&recv_marshalling, data, sizeof(MARSHALLING_INFO));

	log_debug("trip_number: %d, total_weight: %d\n", recv_marshalling.trip_number, recv_marshalling.total_weight);

	pthread_mutex_unlock(&marshalling_mutex);
	
	return 1;
}

static void limit_data_rcv_handle(char *data, int length)
{

}

static route_data_ack(EVENT_TYPE route_cmd, int cnt)
{
	char cmd[2];
	char arg;
	cmd[0] = MY_SEND_EVENT_SOURCE;
	cmd[1] = (char)route_cmd;

	arg = (char)cnt;
	frame_encap(COM_SOC, get_commu_addr(), COMMAND_FRAME_CODE, cmd, &arg, 1);
}

static inline void route_data_rcv_handle(char *data, uint16_t length, char type, int amount, int count)
{
	FILE *fp;
	struct event_t *new_event = NULL;
	char command[2] = {0, 0};
	char argument;
	int i, ret;
	static int frame_total = 0;
	static int frame_cnt = 0;
	static char frame_cmd = EVENT_ROUTE_DATA_SEND;
	static unsigned int recv_flag[6]= {0, 0, 0, 0, 0, 0};

	if (amount < 0 || count < 0 /*|| frame_cmd != type*/)
		return;

	switch (type) {
		case EVENT_ROUTE_DATA_SEND:
			if ((recv_flag[0] & 0x00000001 << count) != 0 && count < amount) {
				route_data_ack(EVENT_ROUTE_DATA_SEND_ACK, count);
				log_debug("gradient_b ack T:%d N:%d\n", amount, count);
			}
			break;
		case EVENT_ROUTE_DATA_CURVE:
			if ((recv_flag[1] & 0x00000001 << count) != 0 && count < amount) {
				route_data_ack(EVENT_ROUTE_DATA_CURVE_ACK, count);
				log_debug("curve_b ack T:%d N:%d\n", amount, count);
			}
			break;
		case EVENT_ROUTE_DATA_LIMIT:
			if ((recv_flag[2] & 0x00000001 << count) != 0 && count < amount) {
				route_data_ack(EVENT_ROUTE_DATA_LIMIT_ACK, count);
				log_debug("limit_b ack T:%d N:%d\n", amount, count);
			}
			break;
		case EVENT_ROUTE_DATA_TELESEME:
			if ((recv_flag[3] & 0x00000001 << count) != 0 && count < amount) {
				route_data_ack(EVENT_ROUTE_DATA_TELESEME_ACK, count);
				log_debug("teleseme_b ack T:%d N:%d\n", amount, count);
			}
			break;
		case EVENT_ROUTE_DATA_TUNNEL:
			if ((recv_flag[4] & 0x00000001 << count) != 0 && count < amount) {
				route_data_ack(EVENT_ROUTE_DATA_TUNNEL_ACK, count);
				log_debug("tunnel_b ack T:%d N:%d\n", amount, count);
			}
			break;
		case EVENT_ROUTE_DATA_STATION:
			if ((recv_flag[5] & 0x00000001 << count) != 0 && count < amount) {
				route_data_ack(EVENT_ROUTE_DATA_STATION_ACK, count);
				log_debug("station_b ack T:%d N:%d\n", amount, count);
			}
			break;
		default:
			return;
	}

	if (frame_cmd != type)
		return;
	
  switch(frame_cmd) {
    case EVENT_ROUTE_DATA_SEND:  //gradient
      if (count == 0) {
				frame_total = amount;
				frame_cnt = 0;
        fp = fopen("./route_data/gradient_b", "w");
      } else if (count < amount && amount == frame_total && count == frame_cnt) {
         fp = fopen("./route_data/gradient_b", "a");
      } else {
      	//if (count < amount) {
				//	route_data_ack(EVENT_ROUTE_DATA_SEND_ACK, count);
      	//}
    		log_debug("gradient_b open error, P:%d T:%d N:%d; NEED:%d\n", type, amount, count, frame_cnt);
				break;
      }
			if (fp < 0){
				perror("EVENT_ROUTE_DATA_SEND \n");
				exit(0);
			}
      ret = fwrite(data, 1, length, fp);
      if (ret == length) {
				route_data_ack(EVENT_ROUTE_DATA_SEND_ACK, frame_cnt);
				recv_flag[0] |= 0x00000001 << frame_cnt;
				if (frame_total - 1 == frame_cnt)
					frame_cmd = EVENT_ROUTE_DATA_CURVE;
				frame_cnt++;
	    	log_debug("gradient_b already write %d,left %d frame\n",length,amount-count-1);
      } else
      	log_debug("gradient_b write error\n");
      fclose(fp);
      break;
      
    case EVENT_ROUTE_DATA_CURVE:  //curve
      if (count == 0) {
				frame_total = amount;
				frame_cnt = 0;
        fp = fopen("./route_data/curve_b", "w");
      } else if (count < amount && amount == frame_total && count == frame_cnt) {
        fp = fopen("./route_data/curve_b", "a");
      } else {
        //if (count < amount) {
				//	route_data_ack(EVENT_ROUTE_DATA_CURVE_ACK, count);
      	//}
      	log_debug("curve_b open error, P:%d T:%d N:%d; NEED:%d\n", type, amount, count, frame_cnt);
				break;
      }
			if (fp < 0) {
				perror("EVENT_ROUTE_DATA_CURVE \n");
				exit(0);
			}
      ret = fwrite(data, 1, length, fp);
      if (ret == length) {
				route_data_ack(EVENT_ROUTE_DATA_CURVE_ACK, frame_cnt);
				recv_flag[1] |= 0x00000001 << frame_cnt;
				if (frame_total - 1 == frame_cnt)
					frame_cmd = EVENT_ROUTE_DATA_LIMIT;
				frame_cnt++;
      	log_debug("curve_b already write %d,left %d frame\n",length,amount-count-1);
      } else
      	log_debug("curve_b write error\n");
      fclose(fp);
      break;
      
    case EVENT_ROUTE_DATA_LIMIT:
      if (count == 0) {
				frame_total = amount;
				frame_cnt = 0;
        fp = fopen("./route_data/limit_b", "w");
      } else if (count < amount && amount == frame_total && count == frame_cnt) {
        fp = fopen("./route_data/limit_b", "a");
      } else {
        //if (count < amount) {
				//	route_data_ack(EVENT_ROUTE_DATA_LIMIT_ACK, count);
      	//}
      	log_debug("limit_b open error, P:%d T:%d N:%d; NEED:%d\n", type, amount, count, frame_cnt);
				break;
      }
			if (fp < 0) {
				perror("EVENT_ROUTE_DATA_LIMIT \n");
				exit(0);
			}
      ret = fwrite(data, 1, length, fp);
      if (ret == length) {
				route_data_ack(EVENT_ROUTE_DATA_LIMIT_ACK, frame_cnt);
				recv_flag[2] |= 0x00000001 << frame_cnt;
				if (frame_total - 1 == frame_cnt)
					frame_cmd = EVENT_ROUTE_DATA_TELESEME;
				frame_cnt++;
      	log_debug("limit_b already write %d,left %d frame\n",length,amount-count-1);
      } else
      	log_debug("limit_b write error\n");
      fclose(fp);
      break;
    case EVENT_ROUTE_DATA_TELESEME:
      if (count == 0) {
				frame_total = amount;
				frame_cnt = 0;
        fp = fopen("./route_data/teleseme_b", "w");
      } else if (count < amount && amount == frame_total && count == frame_cnt) {
        fp = fopen("./route_data/teleseme_b", "a");
      } else {
        //if (count < amount) {
				//	route_data_ack(EVENT_ROUTE_DATA_TELESEME_ACK, count);
      	//}
      	log_debug("teleseme_b open error, P:%d T:%d N:%d; NEED:%d\n", type, amount, count, frame_cnt);
				break;
      }
			if (fp < 0) {
				perror("EVENT_ROUTE_DATA_TELESEME \n");
				exit(0);
			}
      ret = fwrite(data, 1, length, fp);
      if (ret == length) {
				route_data_ack(EVENT_ROUTE_DATA_TELESEME_ACK, frame_cnt);
				recv_flag[3] |= 0x00000001 << frame_cnt;
				if (frame_total - 1 == frame_cnt)
					frame_cmd = EVENT_ROUTE_DATA_TUNNEL;
				frame_cnt++;
      	log_debug("teleseme_b already write %d,left %d frame\n",length,amount-count-1);
      } else
      	log_debug("teleseme_b write error\n");
      fclose(fp);
      break;        
    case EVENT_ROUTE_DATA_TUNNEL:
      if (count == 0) {
				frame_total = amount;
				frame_cnt = 0;
        fp = fopen("./route_data/tunnel_b", "w");
      } else if (count < amount && amount == frame_total && count == frame_cnt) {
        fp = fopen("./route_data/tunnel_b", "a");
      } else {
        //if (count < amount) {
				//	route_data_ack(EVENT_ROUTE_DATA_TUNNEL_ACK, count);
      	//}
      	log_debug("tunel_b open error, P:%d T:%d N:%d; NEED:%d\n", type, amount, count, frame_cnt);
				break;
      }
			if (fp < 0) {
				perror("EVENT_ROUTE_DATA_TUNNEL \n");
				exit(0);
			}
      ret = fwrite(data, 1, length, fp);
      if (ret == length) {
				route_data_ack(EVENT_ROUTE_DATA_TUNNEL_ACK, frame_cnt);
				recv_flag[4] |= 0x00000001 << frame_cnt;
				if (frame_total - 1 == frame_cnt)
					frame_cmd = EVENT_ROUTE_DATA_STATION;
				frame_cnt++;
      	log_debug("tunel_b already write %d,left %d frame\n",length,amount-count-1);
      } else
      	log_debug("tunel_b write error\n");
      fclose(fp);
      break;
    case EVENT_ROUTE_DATA_STATION:
      if (count == 0) {
				frame_total = amount;
				frame_cnt = 0;
        fp = fopen("./route_data/station_b", "w");
      } else if (count < amount && amount == frame_total && count == frame_cnt) {
        fp = fopen("./route_data/station_b", "a");
      } else {
        //if (count < amount) {
				//	route_data_ack(EVENT_ROUTE_DATA_STATION_ACK, count);
      	//}
      	log_debug("route_b open error, P:%d T:%d N:%d; NEED:%d\n", type, amount, count, frame_cnt);
				break;
      }
			if (fp < 0) {
				perror("EVENT_ROUTE_DATA_STATION \n");
				exit(0);
			}
      ret = fwrite(data, 1, length, fp);
      if (ret == length) {
				route_data_ack(EVENT_ROUTE_DATA_STATION_ACK, frame_cnt);
				recv_flag[5] |= 0x00000001 << frame_cnt;
				if (frame_total - 1 == frame_cnt)
					frame_cmd = EVENT_ROUTE_DATA_SEND;
				frame_cnt++;
      	log_debug("route_b already write %d,left %d frame\n",length,amount-count-1);
      } else
      	log_debug("route_b write error\n");
      fclose(fp);
			if (amount - 1 == count) {
				set_route_flag(1);
				if (get_marsh_flag() == 1) {
	        log_info("route data receive over,begin to optimize\n");
	        opt_init();
#ifdef BOARDID 
					command[0] = get_core_event_source(BOARDID);
					command[1] = EVENT_TRIP_OPT_COMPLETE;
#else
					command[0] = SOURCE_CORE_0;
					command[1] = EVENT_TRIP_OPT_COMPLETE;
#endif
	        argument = 0x01; 
	        if(frame_encap(COM_SOC, VOTE_0_DES, COMMAND_FRAME_CODE, command, &argument, 1) != PROTOCAL_SUCCESS) {  
	          log_error("send opt complete command error.\n");
	        }
				}
			}
		break;
	default:
		break;

  }
}

static void realtime_data_rcv_handle(char *data, int length) 
{
	static char led_status = 0;

	if (led_status == 0) {
		led_on(REALTIME_LED);
		led_status = 1;
	} else {
		led_off(REALTIME_LED);
		led_status = 0;
	}

	pthread_mutex_lock(&(rt_arr_flag.rt_mutex));

    memcpy(&rt_comm, data, sizeof(RT_COMM));
#if 0
          printf("speed: %d; location: %d sigmnal: %d airbrake: %d telenum : %d distance: %d, new distance: %d\n",
    		rt_data.speed_and_location.speed, rt_data.speed_and_location.kilmeter_coordinate,
			rt_data.train_sig,
			 rt_data.brake_out,
			 rt_data.speed_and_location.singnal_equipment_No,
			 rt_data.speed_and_location.distance,
			 rt_data.speed_and_location.new_kilmeter_coordinate);//公里标);
#if 0
#include "opt_definition.h"
#include "opt_global_var.h"
#include "rt_definition.h"
#include "rt_global_var.h"
#include "rt_process.h"
#include "running_train.h"
#include "linked_list_operations.h"
#include "realtime_data.h"
#include "error_code.h"
#include "log.h"
#include "to_status.h"

		char frame_cmd[2];
		char frame_arg[200];
		int frame_size;
		RT_DATA xxx;
		
#ifdef BOARDID 
		frame_cmd[0] = get_core_event_source(BOARDID);
		frame_cmd[1] = EVENT_REAL_TIME_CTRL;
#else
	    frame_cmd[0] = SOURCE_CORE_0;
	    frame_cmd[1] = EVENT_REAL_TIME_CTRL;

#endif
		xxx.rt_par.rt_v = (float)rt_data.speed_and_location.speed;
		xxx.rt_par.rt_cont_post = (float)rt_data.speed_and_location.new_kilmeter_coordinate;

		frame_size = sizeof(RT_DATA);
		memcpy(frame_arg, &(xxx), frame_size);
		
		frame_encap(DAT_VOTE_SOC, VOTE_0_DES, DATA_FRAME_CODE, frame_cmd, frame_arg,
				frame_size);
#endif

			 
#endif
	

	// jiaxiang: 触发rt_main中的实时优化计算循环
	rt_arr_flag.rt_flag = 1;
	pthread_cond_signal(&(rt_arr_flag.rt_cond));
	pthread_mutex_unlock(&(rt_arr_flag.rt_mutex));

}

void data_frame_rcv_handle(char *data, uint16_t length, char *command, int amount, int count)
{
	unsigned short commu_addr;
	int master_commu_source;
	char cmd[2];
    char arg;
	
	switch (command[0]) {
	    case SOURCE_VOTE_0:
			switch (command[1]) {
				case EVENT_VOTE_SENT_MARSHALLING:
					set_marsh_flag(marsh_data_rcv_handle(data, length));
					log_info("recv arsh data.\n");
					if (get_route_flag() == 1) {
			            log_info("marsh data receive over,begin to optimize\n");
			            opt_init();
#ifdef BOARDID 
						cmd[0] = get_core_event_source(BOARDID);
						cmd[1] = EVENT_TRIP_OPT_COMPLETE;
#else
						cmd[0] = SOURCE_CORE_0;
						cmd[1] = EVENT_TRIP_OPT_COMPLETE;
#endif

			            arg = 0x01; 
			            if(frame_encap(COM_SOC, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, &arg, 1) != PROTOCAL_SUCCESS) {
			                
			                log_error("send opt complete command error.\n");
		            	}
					}
					break;
				//case EVENT_SIG_LIGHT_CHANGE:
				//	break;
				default:
					break;
			}
			break;
		case SOURCE_COM_0:
		case SOURCE_COM_1:
			//log_info("recv commu event start.\n");
			commu_addr = get_commu_addr();
			if (COM_0_DES == commu_addr)
				master_commu_source = SOURCE_COM_0;
			else if (COM_1_DES == commu_addr)
				master_commu_source = SOURCE_COM_1;
			if ((int)command[0] != master_commu_source) {
				log_warning("recv wrong master commu addr.\n");
				return;
			}
			switch (command[1]) {
				//log_info("commu event command[1] = %d.\n", command[1]);
				case EVENT_ROUTE_DATA_SEND:
				case EVENT_ROUTE_DATA_CURVE:
				case EVENT_ROUTE_DATA_LIMIT:
				case EVENT_ROUTE_DATA_TELESEME:
				case EVENT_ROUTE_DATA_TUNNEL:
				case EVENT_ROUTE_DATA_STATION:
					route_data_rcv_handle(data, length, command[1], amount, count);  
					break;
				case EVENT_FRONT_SPEED_LIMIT:
					log_info("recv speed limit data.\n");
					limit_data_rcv_handle(data, length);
					break;
				case EVENT_REAL_TIME_INFO:
					log_info("recv real time data.\n");
					realtime_data_rcv_handle(data, length);    
					break;
				case EVENT_ROUTE_VERSION_SEND:
					/*if (get_trip_status() != TRIP_INIT || get_train_status() != TRAIN_DUTY 
						|| get_sys_status() != SYS_STANDBY) {
						log_error("check route version error.\n");
						return;
					}*/
#ifdef BOARDID 
					cmd[0] = get_core_event_source(BOARDID);
					cmd[1] = EVENT_ROUTE_VERSION_RESULT;
#else
				    cmd[0] = SOURCE_CORE_0;
				    cmd[1] = EVENT_ROUTE_VERSION_RESULT;
#endif
					int temp_version;
					memcpy(&temp_version, data, sizeof(int));
					if (route_version_check(temp_version) < 0) {
						arg = 0x01;
						log_info("recv wrong version %d.\n", temp_version);
					} else {
						arg = 0x00;
						log_info("recv reight version %d.\n", temp_version);
					}
					if ((frame_encap(COM_SOC, get_commu_addr(), COMMAND_FRAME_CODE, cmd, &arg, 1)) != PROTOCAL_SUCCESS) {
				    	log_error("failed to send route version.\n");
				    }
					if (route_version_check(temp_version) < 0) {
						break;
					}
					set_route_flag(1);
					if (get_marsh_flag() == 1) {
			            log_info("reight version,begin to optimize\n");
			            opt_init();

						cmd[1] = EVENT_TRIP_OPT_COMPLETE;

			            arg = 0x01; 
			            if(frame_encap(COM_SOC, VOTE_0_DES, COMMAND_FRAME_CODE, command, &arg, 1) != PROTOCAL_SUCCESS) {
			                
			                log_error("send opt complete command error.\n");
			            }
					}
			
					break;
				case EVENT_CLOCK_SYNC:
					set_to_time(data, length);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}
