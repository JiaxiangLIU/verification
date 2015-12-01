#include <time.h>
#include <errno.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include "client.h"
#include "client_interface.h"//add_0514
#include "global.h"
#include "new_added.h"//added 0527
#include "rt_definition.h"
#include "protocal.h"
#include "log.h"
#include "to_status.h"
#include "error_code.h"
#include "led.h"
#include "led_packet.h"
#include "sys/time.h"
#include "fun_packaging.h"
#include "new_global.h"
#include "system_error_code.h"
#define PORT_CAN2NET 8000//socket port with can2net
#define TEST 0
#define RECEIVE_TIME 5

struct timeval t1,t2;

extern unsigned short cal_crc16(unsigned short oldcrc16, void *buf, int len);
CLIENT client;
CLIENT	client_rt;
//client_rt.pkt_num = 0;
pthread_mutex_t mutex_s = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_s1 = PTHREAD_MUTEX_INITIALIZER;
unsigned char cli_flag_g;
STATUS_FLAG status_flag;

//memset(&group_set, 0, sizeof(GROUP_SET));//added 0527

void showhex(char *buf, int len) {
	int i;

	for (i = 0; i < len; i++) {
//		printf("%02x ", (unsigned char) buf[i]);
	}
	printf("\n");

	return;
}

int common_connect(char *ipaddr, int port) {
	int sock;
	struct sockaddr_in serverAddr;

	if (ipaddr == NULL) {
		log_error("%s\n", error_code_name(ERROR_CODE_PARAMETER));
	}
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_error("%s\n", error_code_name(ERROR_CODE_SOCKET_REGISTER));
	}
	memset(&serverAddr, 0, sizeof(struct sockaddr_in));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = inet_addr(ipaddr); //ipadaddr is server address

	if (connect(sock, (struct sockaddr *) &serverAddr, sizeof(serverAddr))
			< 0) {
		log_error("%s\n", error_code_name(ERROR_CODE_SOCKET_CONNECT));
		close(sock);
	}

	return sock;
}

int common_disconnect(int sock) {
	close(sock);
	return 0;
}

int common_send(int sock, int canid, void *buf, int len) //load "buf" and "canid" in "sendbuf", then to send through socket
{
	char sendbuf[256];
	int plen = len + 4; //"len" is the length of "buf+canid"

	memcpy(&sendbuf[0], &plen, 4);
	memcpy(&sendbuf[4], &canid, 4);
	memcpy(&sendbuf[8], buf, len);

	pthread_mutex_lock(&mutex_s);
	send(sock, sendbuf, plen + 4, 0); //"plen+4" is the length of "buf+len+canid"
	pthread_mutex_unlock(&mutex_s);

	return 0;
}

int common_recv(int sock, int *canid, void *buf) {
	int len, ret;
	char pkt[8];
    gettimeofday(&t1,NULL);
	do {
		ret = recv(sock, pkt, 8, MSG_WAITALL);
	} while (ret < 0 && errno == EAGAIN); 
   gettimeofday(&t2,NULL);
//   printf("rec1 cost %d us,RET is %d\n",(t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec,ret);
	if (ret <= 0)
		return ret;
	memcpy(canid, &pkt[4], 4);
	memcpy(&len, &pkt[0], 4); //"len" is the length of "canid + data"
    gettimeofday(&t1,NULL);
	do {
		ret = recv(sock, buf, len - 4, MSG_WAITALL); 
	} while (ret < 0 && errno == EAGAIN);
	gettimeofday(&t2,NULL);
//	printf("rec2 cost %d us,RET is %d\n",(t2.tv_sec-t1.tv_sec)*1000000+t2.tv_usec-t1.tv_usec,ret);
	return ret;
}

int pkt_reply(int ptype, int ret, int fd) {
	int tmp = 0, i;
	int canid = 0x581; //Correspond to 0xBO:1
	char buf[8];
	unsigned char sum = 0;
	if (ptype != 0)  //other data except realtime data
			{
		buf[0] = client.fswitch;
		tmp = ((client.pkt_total & 0xfff) << 12) + (client.pkt_num & 0xfff); //packet total and packet number
		memcpy(&buf[1], &tmp, 3);
		tmp = client.data_len & 0xffff;  //the length received successfully
		memcpy(&buf[4], &tmp, 2);
		buf[6] = ret;  //reply info, 0:correct; 1:CRC error; 2:packet number error; 3:frame error
		for (i = 0; i < 7; i++)
			sum += (unsigned char) buf[i];

		sum += 0x5a;
		sum = (~sum) + 1;
		buf[7] = (char) sum;  //frame check

		common_send(fd, canid, buf, 8);
	} else  //realtime data
	{
	}
	return 0;
}

int pkt_header_parse(char *buf, int len, int fd) {
	int fswitch, tmp = 0, val = 0;
	char fpath[32];
	char cmd[2] = {0, 0};
	fswitch = buf[0];  //the 0th byte in description frame is used to design file type
	if (!fswitch) {  //realtime
		client_rt.fswitch = fswitch;
		memcpy(&tmp, &buf[1], 3);//packet total and packet number
		val = (tmp >> 12);  //"tmp >> 12" is packet total
		if (client_rt.pkt_total == 0) {
			client_rt.pkt_total = val;
		} else {
			if (client_rt.pkt_total == val) {
				//log_info("Next pocket header!:rt\n");
				val = (tmp & 0xfff);  //"tmp & 0xfff" is packet number
				if (client_rt.pkt_num != val) {
					log_info("Wrong pocket number! pkt:%d,val:%d\n", client_rt.pkt_num + 1, val);
					pkt_reply(1, 2, fd);
				}
			}
		}

		//log_info("pkt total:%d current pkt num:%d\n", client_rt.pkt_total, client_rt.pkt_num + 1);

	} else {  //other data except realtime data
		switch (fswitch) {
		case 1:
			strcpy(fpath, "./recvdat/base.dat");
			/*to tell vote borad that commu_brd is receiving base data*/
				cmd[0] = SOURCE_COM_0;
				cmd[1] = EVENT_TRIP_STATUS_NOTICE;
				char trip_data_recving = TRIP_DATA_RECVING;
				if(get_sys_status() == SYS_STANDBY && \
					get_train_status() == TRAIN_DUTY){
					frame_encap(SOCKET_TYPE, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, &trip_data_recving, sizeof(TRIP_STATUS));
				}
			break;
		case 3:
			strcpy(fpath, "./recvdat/data2k.dat");
			break;
		case 4:
			strcpy(fpath, "./recvdat/exp.dat");
			break;
		default:
			log_error("Pocket header: unknow filetype %d!\n", buf[0]);
			return -1;
		}
		client.fswitch = fswitch;
		if (client.fp[fswitch] == NULL) {
			client.fp[fswitch] = fopen(fpath, "w+");
			if (client.fp[fswitch] == NULL) {
				log_error("Open %s failed!\n", fpath);
				return -1;
			}
		}
		memcpy(&tmp, &buf[1], 3);
		val = (tmp >> 12);  //packet total
		if (client.pkt_total == 0) {
			client.pkt_total = val;

		} else {
			if (client.pkt_total == val) {
				log_info("Next pocket header!\n");
				
				val = (tmp & 0xfff);
#if TEST
				log_info("pkt_num = %d\n", val);
				log_info("client.pkt_num = %d\n", client.pkt_num);
#endif
				if (client.pkt_num != val) {
#if TEST
					log_info("Wrong pocket number! pkt:%d,val:%d\n", client.pkt_num + 1, val);
#endif
					pkt_reply(1, 2, fd);
				}
			}
		}
#if TEST
		log_info("pkt total:%d current:%d\n", client.pkt_total, client.pkt_num + 1);
#endif
	}

	if (fswitch == REALTIME) {
		client_rt.data_len = 0;
		client_rt.data_exp = 0;
		client_rt.pktcrc16 = 0;
		memcpy(&(client_rt.data_exp), &buf[4], 2);  //packet length
		memcpy(&(client_rt.pktcrc16_exp), &buf[6], 2);  //CRC16 in header frame
	} else {
		client.data_len = 0;
		client.data_exp = 0;
		memcpy(&(client.data_exp), &buf[4], 2);
		memcpy(&(client.pktcrc16_exp), &buf[6], 2);
		client.pktcrc16 = 0;
	}
	return 0;
}

int pkt_body_parse(char *buf, int len, int type, int fd) {
	int i;
	char fpath_rt[32];
	char cmd[2];//added 0914
	if (type == 0) { //realtime
	/*************************************************
	*****logout writing realtime data into file to reduce recieving realtime delay time*****
		if (client_rt.fp[REALTIME] == NULL) {
			strcpy(fpath_rt, "./recvdat/realtime.dat");
			client_rt.fp[REALTIME] = fopen(fpath_rt, "w+");
		}

		if (client_rt.fp[REALTIME] == NULL) {
			log_error("Open RealTime file fail!\n");
			return -1;
		}

#if 0	//Saved as a binary file
		fwrite(buf, sizeof(char), len, client_rt.fp[REALTIME]);
		client_rt.data_len += len;
#endif

#if 0   //Save as formatted file
		for (i = 0; i < len; i++) {
			fprintf(client_rt.fp[REALTIME], "%02x ", *(buf + i));
		}
		fprintf(client_rt.fp[REALTIME], "\n");
		client_rt.data_len += len;
#endif
#if 0//
		log_info("client_rt.data_len: %d\n", client_rt.data_len);
		log_info("client_rt.data_exp: %d\n", client_rt.data_exp);
#endif
		if (client_rt.data_len >= client_rt.data_exp) { //receiving realtime data done
			client_rt.data_len = 0;
			client_rt.pkt_total = 0;
		}
*********************************************************************/
	}else { //other data except realtime
		if (client.fswitch == 0 || client.fp[client.fswitch] == NULL) {
			log_info("client.fswitch = %d!\n", client.fswitch);
			log_info("Header not received!\n");
			exit(0);
		}

		client.data_len += len;
		if (client.data_len > client.data_exp) { //??(?????????????????) > (????????��????)??????????????????????
												 //8bytes, ????????��?????2bytes?,
												 //(client.data_len - client.data_exp) = 6?????????
			fwrite(buf, len - (client.data_len - client.data_exp), 1,
					client.fp[client.fswitch]);

		} else {
			fwrite(buf, len, 1, client.fp[client.fswitch]);
		}

		client.pktcrc16 = cal_crc16(client.pktcrc16, buf, len);
		if (client.data_len >= client.data_exp) { /* pocket finished */
			// check verify here 
			//client.pktcrc16 = cal_crc16(client.pktcrc16, buf, len);
#if TEST
			log_info("client.data_len = %d, data_exp = %d\n", client.data_len, client.data_exp);
#endif
			client.data_len = 0;
			client.data_exp = 0;
			client.pkt_num = client.pkt_num + 1;
#if TEST
			log_info("recv a pocket [%d] done!\n", client.pkt_num);
#endif
			if (client.pktcrc16 != client.pktcrc16_exp) {
#if TEST
				log_info("client.pktcrc16 = %d\n", client.pktcrc16);
				log_info("client.pktcrc16_exp = %d\n", client.pktcrc16_exp);
#endif
				log_info("Pocket crc16 error!\n");
				//pkt_reply(1, 1, fd);
				pkt_reply(1, 0, fd);//shield CRC checksum
			} else {
				pkt_reply(1, 0, fd);
			}
		}
		if (client.pkt_total <= client.pkt_num) { //All pocket finished
			fclose(client.fp[client.fswitch]);
			if (client.fswitch == 1) { //base data
				pthread_mutex_lock(&mutex_s1);
				cli_flag_g = (cli_flag_g & 0x0f) | 0x10;
				log_info("base.dat received done\n");
				pthread_mutex_unlock(&mutex_s1);

				/*to tell vote borad that base data has been received*/
				cmd[0] = SOURCE_COM_0;
				cmd[1] = EVENT_TRIP_STATUS_NOTICE;
				char trip_data_complete = TRIP_DATA_COMPLETE;
				if(get_sys_status() == SYS_STANDBY && \
					get_train_status() == TRAIN_DUTY){
					frame_encap(SOCKET_TYPE, VOTE_0_DES, COMMAND_FRAME_CODE, cmd, &trip_data_complete, sizeof(TRIP_STATUS));
				}
				
			} else if (client.fswitch == 3) { //station name form
				pthread_mutex_lock(&mutex_s1);
				cli_flag_g = (cli_flag_g & 0x0f) | 0x30;
				log_info("data2k.dat received done\n");
				pthread_mutex_unlock(&mutex_s1);
			} else if (client.fswitch == 4) {//jieshi data
				pthread_mutex_lock(&mutex_s1);
				cli_flag_g = (cli_flag_g & 0x0f) | 0x40;
				log_info("exp.dat received done\n");
				pthread_mutex_unlock(&mutex_s1);
			}
			
			log_info("pkt_total = %d, pkt_num = %d\n", client.pkt_total, client.pkt_num);
			client.fp[client.fswitch] = 0;
			client.pkt_total = 0;
			client.pkt_num = 0;
			log_info("recv all pocket done!\n");
		}
	}
	return 0;
}

int status_recv(unsigned int idx, char *buf, int len) //"idx" correspond argument is "canid & 0x7ff"
{
	int i;
	static int FrameNum = 0;
	if (client.fp[STATUS] == NULL) {
		client.fp[STATUS] = fopen("./recvdat/status.dat", "w+");
	}
	if (client.fp[STATUS] == NULL) {
		log_error("Open status.dat failed!\n");
		return -1;
	}
	log_info("Status Recv %d Time\n",FrameNum++);

	/*stored as binary file*/
	//fwrite(&idx, sizeof(idx), 1, client.fp[STATUS]);
	//printf("sizeof(idx) = %d\n", sizeof(idx));
	//fwrite(buf, sizeof(char), len, client.fp[STATUS]);

	
	/*sotred as formatting text*/
	//char fpath[32], fpath_rt[32];
	fprintf(client.fp[STATUS], "%x : ", idx);
	for (i = 0; i < len; i++)
		fprintf(client.fp[STATUS], "%02x ", (unsigned char) *(buf + i));
	fprintf(client.fp[STATUS], "\n");
	return 0;
}

void *net_thread(void *arg) {
	char find_header = 0;
	int fd = *(int *) arg;
	int canid, len, got;
	char buf[128];
	got = 0;
	canid = 0;
	int num = 0;	//"num" is counter of realtime data frame. realtime data include 8 bytes.
	int offset = 0;	//offset when write buf into buf_to_bussiness
	int num1 = 0;
	REALTIME_DATA realtime_data;
	REALTIME_DATA realtime_data_buf;
	int renew_group_set_flag;	//added 0527
	MARSHALLING_INFO marshalling_info_buf;//used to extract marshalling_info from realtime_info
	MARSHALLING_INFO marshalling_info_send; //used to send to vote_brd
	TRIP_INFO trip_info_buf;//
	TRIP_INFO trip_info_send;//
	char buf_to_bussiness[128];
	memset(&trip_info_buf, 0, sizeof(TRIP_INFO));
	memset(&trip_info_send, 0, sizeof(TRIP_INFO));
	struct timeval start,start1,end1,end,start_send,start_rec;
	char frame_cmd[2] = {0, 0};//"frame_cmd[0]" used to store event_source, "frame_cmd[1]" used to store event name
	//memset(&group_set_buf, '0', sizeof(GROUP_SET));
	char led_status = 0;
	do {
		gettimeofday(&start_rec,NULL);
		len = common_recv(fd, &canid, buf);
		gettimeofday(&end1,NULL);
		//printf("rec cost %d us,len is %d\n",(end1.tv_sec-start_rec.tv_sec)*1000000+end1.tv_usec-start_rec.tv_usec,len);
		//log_debug("common_recv() return value len = %d", len);
		if (len < 0) {
			log_error("%s\n", error_code_name(ERROR_CODE_SOCKET_RECV));
			common_disconnect(fd);
			exit(-1);
		}
		if (len == 0) {//don't know the different between "len < 0" and "len == 0"
			log_warning("Peer closed connection: %s\n", error_code_name(ERROR_CODE_SOCKET_RECV));
			//common_disconnect(fd);
			continue;
		}

		got = (unsigned int) (((unsigned int) canid) & 0x7);//Got offset priority(3bits)
													//got == 0: explanation frame; got == 1~7:data frame
		switch ((unsigned int) canid >> 3) {                //Priority(8bits)
		case 0x20:	// status message
			if (parse_status_info(canid, buf) == PARSE_STATUS_INFO_ERR ) {
				set_abnormal_code(SYS_ERROR_STATUS_INFO_RESOLVING_FAILURE, VOTE_0_DES);
				//send_abnormal_code();//call send_abnormal_code() in heartbeat_check()
			}
			//logout status_recv() to avoid spending time when write status_data into file
			//status_recv((unsigned int) (((unsigned int) canid) & 0x07ff), buf, len);
			break;

		case 0x40:	//realtime message
			gettimeofday(&start,NULL);
			if (got == 0) {
				gettimeofday(&start1,NULL);
				pkt_header_parse(buf, len, fd);
				led_flicker(&led_status, GPIOX_48);
#if 0				
				if(led_status == 0){
					led_packet_on(GPIOX_48);
				}else{
					led_packet_off(GPIOX_48);
				}
				led_status = ~led_status;
#endif				
				find_header = 1;
				num = 0;
				offset = 0;
				gettimeofday(&end1,NULL);
//				printf("got=0 cost %d us\n",(end1.tv_sec-start1.tv_sec)*1000000+end1.tv_usec-start1.tv_usec);
			} else {
				if (find_header == 0)
					break;
				memcpy(buf_to_bussiness + offset, buf, len);
				offset = offset + len;
				num++;
				if (num == 9) {//realtime data has 9 frames
					num = 0;
					offset = 0;
					find_header = 0;
					if (got == 2) { //when got == 2, the first frame data in "buf_to_bussiness" is realtime_data's first frame data
						gettimeofday(&start1, NULL);
						num1++;
						if (num1 == RECEIVE_TIME) {
						num1 = 0;
						pkt_body_parse (buf_to_bussiness, 72, 0, fd);
						put_rt_rs (buf_to_bussiness, &realtime_data_buf);

						//get time from realtime_info then send out
						if (get_sys_status() == SYS_STANDBY
							&& get_train_status() == TRAIN_DUTY) {
							get_time_then_send(realtime_data_buf);
						}

						/***send train_status(ben bu ji)***/
						judge_trip_train_status(realtime_data_buf);
						frame_cmd[0] = SOURCE_COM_0;//event source
						frame_cmd[1] = EVENT_DUTY_ASSISTING_CHANGE;
														
						
						/***extract group info, if group info changed, update***/
						set_marshalling_info(&marshalling_info_buf, realtime_data_buf); //extract group info
						log_debug("marshalling_info is:\n");
						log_debug("cargo_number is: %d\n", marshalling_info_buf.cargo_number);
						log_debug("equivalent_length: %d\n", marshalling_info_buf.equivalent_length);
						log_debug("total_weight: %d\n", marshalling_info_buf.total_weight);
						log_debug("trip_number: %d\n", marshalling_info_buf.trip_number);
						log_debug("get_sys_status is: %d, get_train_status is: %d\n", get_sys_status(),get_train_status());

						/*set trip_info_buf according to realtime_data_buf*/
						set_trip_info(&trip_info_buf, realtime_data_buf);

						
							if (get_sys_status() == SYS_STANDBY
									&& get_train_status() == TRAIN_DUTY) {
								renew_group_set_flag = memcmp(
										&marshalling_info_send,
										&marshalling_info_buf,
										sizeof(MARSHALLING_INFO));
								log_debug("renew_group_set_flag is: %d\n",
										renew_group_set_flag);
								if (renew_group_set_flag != 0) { //when marshalling_info has changed, update marshalling_info_send
									log_debug(
											"marshalling_info has changed.\n");
									memcpy(&marshalling_info_send,
											&marshalling_info_buf,
											sizeof(MARSHALLING_INFO));
									log_debug(
											"marshalling_info_send is ready.\n");

									/*TODO: send marshalling_info_send to vote board*/
									frame_cmd[0] = SOURCE_COM_0; //event source
									frame_cmd[1] = EVENT_MARSHALLING_RE_CHANGE; //event name, "0x08" present marshalling_info event
									log_debug(
											"marshalling_info_send is ready to send to vote_brd.\n");
									frame_encap(0x40, VOTE_0_DES,
											DATA_FRAME_CODE, frame_cmd,
											(char *) &marshalling_info_send,
											sizeof(MARSHALLING_INFO));
									log_debug("marshalling_info_send has been sent to bote_brd.\n");
									
									/*TODO: send jieshi_info to vote board*/
									send_jieshi_info();
									
								}
								
								/*TODO: send trip_info_send to vote board*/
									send_trip_info(&trip_info_buf, &trip_info_send);
								
							}

						/*realtime changing cause event*/
						EVENT_FLAG event_flag;
						int seq;
						memset(&event_flag, 0, sizeof(EVENT_FLAG));
						memcpy(&realtime_data, &realtime_data_buf,
							sizeof(REALTIME_DATA));
						

							/**/
							if (((realtime_data.brake_output >> 6) == 1)
									|| (((realtime_data.brake_output & 0x04)
											>> 2) == 1)
									|| (((realtime_data.brake_output & 0x02)
											>> 1) == 1)
									|| ((realtime_data.brake_output & 0x01) == 1)) { //brake output changing
								//event_flag.brake_output_flag = 1;
								log_info("brake out changing\n");
								/****TODO: turn the auto_drive into disable, send update out*******
								 *****turn to manual_drive*********************************************
								 *****travel turnto manual_drive***********************************/
							}
							if (memcmp(&(realtime_data.singnal_equipment_No),
									&(realtime_data_buf.singnal_equipment_No),
									2) != 0) {    //signal equipment No changing
									if(get_sys_status() == SYS_STANDBY){
										
									}

							}
							if ((((realtime_data.monitor_situation & 0x40) >> 6)
									== 1)
									|| (((realtime_data.monitor_situation & 0x20)
											>> 5) == 1)
									|| (((realtime_data.monitor_situation & 0x10)
											>> 4) == 1)
									|| (((realtime_data.monitor_situation & 0x08)
											>> 3) == 1)
									|| (((realtime_data.monitor_situation & 0x04)
											>> 2) == 1)
									|| (((realtime_data.monitor_situation & 0x02)
											>> 1) == 1)
									|| ((realtime_data.brake_output & 0x01) == 1)) { //monitor status changing
								event_flag.monitor_situation_flag = 1;
								log_info("monitor status changing\n");
								/******TODO: turn the auto_drive into disable, send update out******
								 *****turn to manual_drive********************************************
								 *****travel turnto manual_drive************************************/
							}
							if (memcmp(&(realtime_data_buf.partition_side_line),
									&(realtime_data_buf.partition_side_line), 2)
									!= 0) {//Lateral line changing, ce xian zhuan yi

							}
							if (memcmp(&(realtime_data_buf.train_singnal),
									&(realtime_data_buf.train_singnal), 2)
									!= 0) {//signal changed
								
							}
							if (memcmp(
									&(realtime_data_buf.partition_expand_line),
									&(realtime_data_buf.partition_expand_line),
									2) != 0) {//branch changing

							}
							if (memcmp(&(realtime_data_buf.total_weight),
									&(realtime_data_buf.total_weight), 4) != 0) { //Traction changing, means weight and length changing

							}
							/*struct sotred speed and location*/
							SPEED_AND_LOCATION speed_and_location;
							RT_COMM to_send;

							/**/
							memcpy(&to_send.event_flag, &event_flag,
									sizeof(EVENT_FLAG));
							to_send.speed_and_location.speed =
									realtime_data.speed;
							to_send.speed_and_location.kilmeter_coordinate =
									realtime_data.kilmeter_coordinate;
							to_send.speed_and_location.singnal_equipment_No =
									realtime_data.singnal_equipment_No;
							to_send.speed_and_location.distance =
									realtime_data.distance;
							//to_send.speed_limit = realtime_data.speed_limit_level;
							seq=convert_seq_mile(realtime_data.singnal_equipment_No,realtime_data.distance);
							to_send.speed_and_location.new_kilmeter_coordinate = seq;
							int i;
							for (i = 0; i < SIG_MAX; i++)
								if (((uint16_t) realtime_data.train_singnal >> i
										& 0x0001) == 1)
									break;
							if (i < SIG_MAX)
								to_send.train_sig = i;
							else
								to_send.train_sig = SIG_RESERVED;

							for (i = 0; i < BRAKE_MAX; i++)
								if (((uint8_t) realtime_data.brake_output >> i
										& 0x01) == 1)
									break;
							if (i < BRAKE_MAX)
								to_send.brake_out = i;
							else {
								if (((uint8_t) realtime_data.brake_output
										>> BRAKE_EMERGENCY & 0x01) == 1)
									to_send.brake_out = BRAKE_EMERGENCY;
								else
									to_send.brake_out = BRAKE_RESERVED;
							}

							/*****TODO:send TO_SEND struct***********/
							//char frame_cmd[2];
							gettimeofday(&start_send,NULL);
							uint8_t status = get_sys_status();
							if (status == SYS_STANDBY && get_train_status() == TRAIN_DUTY && get_trip_status() == TRIP_START){
								frame_cmd[0] = SOURCE_COM_0;
								frame_cmd[1] = EVENT_REAL_TIME_INFO;
								log_info("comm status:%d speed:%d location : %d seq:%d \n",status,to_send.speed_and_location.speed,to_send.speed_and_location.kilmeter_coordinate,to_send.speed_and_location.new_kilmeter_coordinate);
								frame_encap(0x40, CORE_0_DES, DATA_FRAME_CODE,
										frame_cmd, (char *) &to_send,
										sizeof(RT_COMM));
							}

							gettimeofday(&end1,NULL);
//							printf("send cost %d us\n",(end1.tv_sec-start_send.tv_sec)*1000000+end1.tv_usec-start_send.tv_usec);
						}
						gettimeofday(&end1,NULL);
//						printf("got=2 cost %d us\n",(end1.tv_sec-start1.tv_sec)*1000000+end1.tv_usec-start1.tv_usec);
					} else
						memset(buf_to_bussiness, 0, sizeof(buf_to_bussiness));
				}
			}
			gettimeofday(&end,NULL);
//			printf("realtime cost %d us\n",(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec);
			break;
		case 0xb8:	// package message

			if (got == 0) {
				pkt_header_parse(buf, len, fd);
			} else {
				pkt_body_parse(buf, len, 1, fd);
			}
			break;
		default:
			break;
		}

	} while (1);

	return NULL;
}

void verify_fill(void *buf) {
	unsigned char *p = buf;
	unsigned char t = 0;
	int i;

	for (i = 0; i < 7; i++)
		t += p[i];
	t += 0x5a;
	t = (~t) + 1;

	p[7] = t;

	return;
}

void *realtime_parse(REALTIME_DATA realtime_parse_data) {

	return NULL;
}

int main_thread(void) {
	int comm, port; 
	char *ip, buf[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int canid = 0;
	pthread_t tid;
	pthread_attr_t attr;
	port = PORT_CAN2NET;
	ip = "127.0.0.1";
	comm = common_connect(ip, port);//creat socket conecting with can2net
	if (comm < 0) {
		log_error("%s\n",error_code_name(ERROR_CODE_SOCKET_REGISTER));
		return -1;
	}

	memset(&client, 0, sizeof(CLIENT));
	memset(&client_rt, 0, sizeof(CLIENT));
	if ((pthread_attr_init(&attr)) < 0) {
		log_error("%s\n",error_code_name(ERROR_CODE_PTHREAD_CREATE));
		return -1;
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if ((pthread_create(&tid, &attr, net_thread, (void *) &comm)) < 0) {
		log_error("%s\n",error_code_name(ERROR_CODE_PTHREAD_CREATE));
		return -1;
	}
	/*******************************************************************/
/*TODO: request to send packetized data*///add0908	
	char commnd_recg = 0;
	//pthread_init(&mutex_s1, NULL);
	while (1) {
		memset(buf, 0, sizeof(buf));
		//pthread_mutex_lock(&mutex_s1);////////////
		commnd_recg = 0x0f & cli_flag_g;///////////////
		//pthread_mutex_unlock(&mutex_s1);
		switch (data_request) {//0xb8:0
		case 0x01:
			buf[0] = 0x01;
			verify_fill(buf);//
			canid = 0x580;
			common_send(comm, canid, buf, 8);
			pthread_mutex_lock(&mutex_s2);
			data_request = 0;
			pthread_mutex_unlock(&mutex_s2);
			break;
		case 0x03:
			buf[0] = 0x03;
			verify_fill(buf);
			canid = 0x580;
			common_send(comm, canid, buf, 8);
			pthread_mutex_lock(&mutex_s1);
			cli_flag_g = cli_flag_g & 0;
			pthread_mutex_unlock(&mutex_s1);
			break;
		case 0x04:
			buf[0] = 0x04;
			verify_fill(buf);
			canid = 0x580;
			common_send(comm, canid, buf, 8);
			pthread_mutex_lock(&mutex_s1);
			cli_flag_g = cli_flag_g & 0;
			pthread_mutex_unlock(&mutex_s1);
			break;
			//default:
			//printf("b - req basedata\nk - req 2kdata\ne - req exposition\nq - quit\n");
			//break;
		}
		usleep(10000);
		canid = 0;

	}
	if (client.fp[STATUS] != NULL)                //??
		fclose(client.fp[STATUS]);
	if (client_rt.fp[REALTIME] != NULL)                //??
		fclose(client_rt.fp[REALTIME]);
	return 0;
	/*******************************************************/
}

/************************************************/
void comm_client() {
	pthread_t tid_2;
	pthread_attr_t attr_2;

	//????can2net????
	//???can2net????????

	if ((pthread_attr_init(&attr_2)) < 0) {
		log_info("pthread error!:init\n");
		return;
	}
	//pthread_attr_setdetachstate(&attr_2, PTHREAD_CREATE_JOINABLE);
	if ((pthread_create(&tid_2, &attr_2, (void *) main_thread, NULL)) < 0) {
		log_info("thread error!:create\n");
		return;
	}

	while (cli_flag_g != 0xff)
		;
}
/*******************************************************************/

/***************************************************
 int main(){
 pthread_t tid_2;
 pthread_attr_t attr_2;
 int cmd;

 //????can2net????
 //???can2net????????
 if(socket_connect(0x80) == -1)
 {
 perror("socket connect err.");
 return -1;
 }
 if ((pthread_attr_init(&attr_2)) < 0) {
 #if TEST
 printf("pthread error!:init\n");
 #endif
 return -1;
 }
 //pthread_attr_setdetachstate(&attr_2, PTHREAD_CREATE_JOINABLE);
 if ((pthread_create(&tid_2,  &attr_2, (void *)main_thread, NULL)) < 0) {
 #if TEST
 printf("thread error!:create\n");
 #endif
 return -1;
 }
 #if TEST
 printf("before thread.\n");
 #endif
 while((cmd = getc(stdin)) != 'q'){
 switch(cmd){
 case 'b':
 pthread_mutex_lock(&mutex_s1);
 cli_flag_g  = 0x01;
 pthread_mutex_unlock(&mutex_s1);
 break;
 case 'k':
 pthread_mutex_lock(&mutex_s1);
 cli_flag_g  = 0x03;
 pthread_mutex_unlock(&mutex_s1);
 break;
 case 'e':
 pthread_mutex_lock(&mutex_s1);
 cli_flag_g = 0x04;
 pthread_mutex_unlock(&mutex_s1);
 break;
 //default:
 //printf("b - req basedata\nk - req 2kdata\ne - req exposition\nq - quit\n");
 //break;
 }
 }
 #if TEST
 printf("out of while\n");
 #endif
 return 0;
 }
 ****************************************************************/
