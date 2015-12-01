#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "new_added.h"
#include "client_interface.h"
#include "to_status.h"
#include "log.h"

TRAIN_STATUS train_status = TRAIN_MAX;

void set_marshalling_info (MARSHALLING_INFO *marshalling_info, REALTIME_DATA realtime_data) {
	marshalling_info->trip_number = realtime_data.trip_number;
	marshalling_info->cargo_number = realtime_data.cargo_number;
	marshalling_info->equivalent_length = realtime_data.equivalent_length;
	marshalling_info->total_weight = realtime_data.total_weight;
	//marshalling_info->loco_number = realtime_data.loco_number;
}

void set_trip_info(TRIP_INFO *trip_info, REALTIME_DATA realtime_data)
{
	trip_info->start_station = realtime_data.start_station;
	trip_info->end_station = 0;
	trip_info->trip_number = realtime_data.trip_number;
}

/**********************************************
 *set train status according to realtime_data.trip_status
***********************************************/
void judge_trip_train_status(REALTIME_DATA realtime_data)
{
	if(realtime_data.trip_status & 0x02 == 0){
		set_train_status(TRAIN_DUTY);
	} 
	else if (realtime_data.trip_status & 0x02 == 2){
		set_train_status(TRAIN_ASSISTING);
	}
	else{
		log_debug("train_status not match.");
	}
}

void marshalling_info_print(MARSHALLING_INFO marshalling_info){
	printf("����: %x\n", marshalling_info.trip_number);
	printf("����: %x\n", marshalling_info.cargo_number);
	printf("����: %x\n", marshalling_info.equivalent_length);
	printf("�Ƴ�: %x\n", marshalling_info.total_weight);
	//printf("������: %x\n", marshalling_info.loco_number);
}

int put_rt_rs(char *buf, REALTIME_DATA *rs){
	int32_t tmp1;
	int32_t tmp2;
	int32_t tmp3;
	int32_t tmp4;
	uint32_t i;

	rs->num = buf[0];

	tmp1 = buf[2];
	tmp2 = buf[1];
	tmp1 = tmp1 << 8;
	rs->year = (int16_t) (tmp1 | tmp2);

	rs->month = buf[3];
	rs->day = buf[4];
	rs->hour = buf[5];
	rs->min = buf[6];
	rs->sec = buf[7];
	rs->percentsec = buf[8];

	tmp1 = buf[10];
	tmp2 = buf[9];
	tmp1 = tmp1 << 8;
	rs->train_model = (int16_t) (tmp1 | tmp2);

	tmp1 = buf[12];
	tmp2 = buf[11];
	tmp1 = tmp1 << 8;
	rs->trip_number =  (int16_t) (tmp1 | tmp2);

	rs->train_situation = buf[13];

	tmp1 = buf[17];
	tmp2 = buf[16];
	tmp3 = buf[15];
	tmp4 = buf[14];
	tmp1 = tmp1 << 24;
	tmp2 = tmp2 << 16;
	tmp3 = tmp3 << 8;
	rs->train_mark = tmp1 | tmp2 | tmp3 | tmp4;

	tmp1 = buf[20];
	tmp2 = buf[19];
	tmp3 = buf[18];
	tmp1 = tmp1 << 16;
	tmp2 = tmp2 << 8;
	rs->trip_number = tmp1 | tmp2 | tmp3;

	rs->trip_status = buf[21];

	tmp1 = buf[23];
	tmp2 = buf[22];
	tmp1 = tmp1 << 8;
	rs->cargo_number = tmp1 | tmp2;//����[22-23]

	tmp1 = buf[25];
	tmp2 = buf[24];
	tmp1 = tmp1 << 8;
	rs->total_weight = tmp1 | tmp2;//����[24-25]

	tmp1 = buf[27];
	tmp2 = buf[26];
	tmp1 = tmp1 << 8;
	rs->equivalent_length = tmp1 | tmp2;//�Ƴ�[26-27]

	tmp1 = buf[29];
	tmp2 = buf[28];
	tmp1 = tmp1 << 8;
	rs->load = tmp1 | tmp2;//�س�[28-29]

	tmp1 = buf[31];
	tmp2 = buf[30];
	tmp1 = tmp1 << 8;
	rs->no_load = tmp1 | tmp2;//�ճ�[30-31]

	tmp1 = buf[33];
	tmp2 = buf[32];
	tmp1 = tmp1 << 8;
	rs->train_type = tmp1 | tmp2;//�г�����[32-33]

	tmp1 = buf[35];
	tmp2 = buf[34];
	tmp1 = tmp1 << 8;
	rs->brake_type = tmp1 | tmp2;//�ƶ�����[34-35]

	tmp1 = buf[37];
	tmp2 = buf[36];
	tmp1 = tmp1 << 8;
	rs->reserve0 = tmp1 | tmp2;//����[36-37]

	rs->route_number = buf[38];//���뽻·��[38]
	rs->reserve1 = buf[39];//����[39]
	rs->data_line_No = buf[40];//���ݽ�·��[40]
	rs->reserve2 = buf[41];//����[41]

	tmp1 = buf[43];
	tmp2 = buf[42];
	tmp1 = tmp1 << 8;
	rs->start_station = tmp1 | tmp2;//��վ��[42-43]

	tmp1 = buf[45];
	tmp2 = buf[44];
	tmp1 = tmp1 << 8;
	rs->reserve3 = tmp1 | tmp2;//����[44-45]

	rs->partition_side_line = buf[46];//����������[46]
	rs->down_partition_side_line = buf[47];//�·�������[47]
	rs->partition_expand_line = buf[48];//������֧��[48]
	rs->down_partion_expand_line = buf[49];//�·���֧��[49]
	rs->singnal_equipment_type = buf[50];//�źŻ�����[50]

	tmp1 = buf[52];
	tmp2 = buf[51];
	tmp1 = tmp1 << 8;
	rs->singnal_equipment_No = tmp1 | tmp2;//�źŻ����[51-52]

	tmp1 = buf[54];
	tmp2 = buf[53];
	tmp1 = tmp1 << 8;
	rs->distance = tmp1 | tmp2;//����[53-54]

	tmp1 = buf[56];
	tmp2 = buf[55];
	tmp1 = tmp1 << 8;
	rs->train_singnal = tmp1 | tmp2;//�����ź�[55-56]

	rs->monitor_situation = buf[57];//���״̬[57]
	rs->brake_output = buf[58];//�ƶ����[58]

	tmp1 = buf[60];
	tmp2 = buf[59];
	tmp1 = tmp1 << 8;
	rs->speed = tmp1 | tmp2;//ʱ��[59-60]

	tmp1 = buf[62];
	tmp2 = buf[61];
	tmp1 = tmp1 << 8;
	rs->speed_restriction = tmp1 | tmp2;//����[61-62]

	i = (uint32_t)buf[65] << 16 | (uint32_t)buf[64] << 8 | (uint32_t)buf[63];
	printf("mile code %08x\n",i);
	i &= 0x000fffff;
	rs->kilmeter_coordinate = (int32_t)i;
	/*tmp1 = buf[65];
	tmp2 = buf[64];
	tmp3 = buf[63];
	tmp1 = tmp1 << 16;
	tmp2 = tmp2 << 8;
	rs->kilmeter_coordinate = (tmp1 | tmp2 | tmp3)&0x007fffff;//�����[63-65]*/
        
	tmp1 = buf[67];
	tmp2 = buf[66];
	tmp1 = tmp1 << 8;
	rs->speed_limit_level = tmp1 | tmp2;//����ĳ��ٵȼ�����[66-67]

	return 0;
}

int realtime_translation(REALTIME_DATA rs, FILE* fd){
	fprintf(fd, "������: %d\n", (unsigned char) rs.num);
	fprintf(fd, "��: %d\n", (rs.year) & 0x0fff);
	fprintf(fd, "��: %d\n", rs.month & 0x0f);
	fprintf(fd, "��: %d\n", rs.day & 0x1f);
	fprintf(fd, "ʱ: %d\n", rs.hour & 0x1f);
	fprintf(fd, "��: %d\n", rs.min & 0x3f);
	fprintf(fd, "��: %d\n", rs.sec & 0x3f);

	fprintf(fd, "�ٷ���: %d ", rs.percentsec & 0x7f);
	if((rs.percentsec & 0x80) == 0){
		fprintf(fd, "ʱ������\n");
	}
	else if((rs.percentsec & 0x80) == 0x80){
		fprintf(fd, "ʱ�ӹ���\n");
	}

	fprintf(fd, "�����ͺ�: %d\n", rs.train_model & 0x3ff);

	fprintf(fd, "������: %d\n", rs.trip_number & 0xffff);

	fprintf(fd, "��������: ");
	if((rs.train_situation & 0x40) == 0x40){
		fprintf(fd, "���϶Ͽ� ");
	}
	else if((rs.train_situation & 0x40) == 0){
		fprintf(fd, "���ϱպ� ");
	}
	fprintf(fd, "�ƶ�(%d) ", ((rs.train_situation & 0x10) >> 4));
	fprintf(fd, "ǣ��(%d) ", ((rs.train_situation & 0x08) >> 3));
	fprintf(fd, "���(%d) ", ((rs.train_situation & 0x04) >> 2));
	fprintf(fd, "��ǰ(%d) ", ((rs.train_situation & 0x02) >> 1));
	fprintf(fd, "��λ(%d)\n", (rs.train_situation & 0x01));

	fprintf(fd, "���ֱ�ʶ: %x\n", rs.train_mark);

	fprintf(fd, "����: %d\n", rs.trip_number & 0x01ffff);

	fprintf(fd, "�����ͻ�: ");
	fprintf(fd, "������(%d) ", ((rs.trip_status & 0x40) >> 6));
	fprintf(fd, "�ͻ�����(%d) ", ((rs.trip_status & 0x1c) >> 2));
	fprintf(fd, "����0�Ǳ���1(%d) ", ((rs.trip_status & 0x02) >> 1));
	fprintf(fd, "����0�ͳ�1(%d)\n", (rs.trip_status & 0x01));

	fprintf(fd, "����: %d\n", rs.cargo_number);
	fprintf(fd, "����: %d\n", rs.total_weight);
	fprintf(fd, "�Ƴ�: %d\n", rs.equivalent_length);
	fprintf(fd, "�س�: %d\n", rs.load);
	fprintf(fd, "�ճ�: %d\n", rs.no_load);
	fprintf(fd, "�г�����: %d\n", rs.train_type);
	fprintf(fd, "�ƶ�����: %d\n", rs.brake_type);
	fprintf(fd, "����0: %d\n", rs.reserve0);
	fprintf(fd, "���뽻·��: %d\n", rs.route_number);
	fprintf(fd, "����1: %d\n", rs.reserve1);

	fprintf(fd, "���ݽ�·��: ");
	fprintf(fd, "����(%d) ",  (rs.data_line_No & 0x80) >> 7);
	fprintf(fd, "��վ��(%d) ", (rs.data_line_No & 0x60) >> 4);
	fprintf(fd, "���ݽ�·��(%d)\n",rs.data_line_No & 0x1f);

	fprintf(fd, "����2: %d\n", rs.reserve2);
	fprintf(fd, "��վ��: %d\n", rs.start_station);
	fprintf(fd, "����3: %d\n", rs.reserve3);

	fprintf(fd, "����������: ");
	fprintf(fd, "������Ч1��Ч0(%d) ", (rs.partition_side_line & 0x80) >> 7);
	fprintf(fd, "���߹ɵ���(%d)\n", rs.partition_side_line & 0x7f);

	fprintf(fd, "�·�������: ");
	fprintf(fd, "������Ч1��Ч0(%d) ", (rs.down_partition_side_line & 0x80) >> 7);
	fprintf(fd, "���߹ɵ���(%d)\n", rs.down_partition_side_line & 0x7f);

	fprintf(fd, "������֧��: ");
	fprintf(fd, "֧����Ч1��Ч0(%d) ", (rs.partition_expand_line & 0x80) >> 7);
	fprintf(fd, "֧�ߺ�(%d)\n", rs.partition_expand_line & 0x7f);

	fprintf(fd, "�·���֧��: ");
	fprintf(fd, "֧����Ч1��Ч0(%d) ", (rs.partition_expand_line & 0x80) >> 7);
	fprintf(fd, "֧�ߺ�(%d)\n", rs.partition_expand_line & 0x7f);

	fprintf(fd, "�źŻ�����: %d\n", (rs.singnal_equipment_type & 0xf0) >> 4);
	fprintf(fd, "�źŻ����: %d\n", rs.singnal_equipment_No);
	fprintf(fd, "����: %d\n", rs.distance);

	fprintf(fd, "�����ź�: ");
	fprintf(fd, "�����ź�����(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x2000) >> 13);
	fprintf(fd, "��Ե��(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x1000) >> 12);
	fprintf(fd, "��ʽ(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x0800) >> 11);
	fprintf(fd, "�ٶȵȼ�3(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x0400) >> 10);
	fprintf(fd, "�ٶȵȼ�2(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x0200) >> 9);
	fprintf(fd, "�ٶȵȼ�1(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x0100) >> 8);
	fprintf(fd, "��(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x80) >> 7);
	fprintf(fd, "��(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x40) >> 6);
	fprintf(fd, "���(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x20) >> 5);
	fprintf(fd, "˫��(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x10) >> 4);
	fprintf(fd, "��2(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x08) >> 3);
	fprintf(fd, "��(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x04) >> 2);
	fprintf(fd, "�̻�(1��Ч0��Ч)(%d) ", (rs.train_singnal & 0x02) >> 1);
	fprintf(fd, "��(1��Ч0��Ч)(%d)\n", rs.train_singnal & 0x01);

	fprintf(fd, "���״̬: ");
	fprintf(fd, "ͨ������(1��Ч0��Ч)(%d) ", (rs.monitor_situation & 0x80) >> 7);
	fprintf(fd, "��������(1��Ч0��Ч)(%d) ", (rs.monitor_situation & 0x40) >> 6);
	fprintf(fd, "ƽ�����(1��Ч0��Ч)(%d) ", (rs.monitor_situation & 0x20) >> 5);
	fprintf(fd, "��������(1��Ч0��Ч)(%d) ", (rs.monitor_situation & 0x10) >> 4);
	fprintf(fd, "���(1��Ч0��Ч)(%d) ", (rs.monitor_situation & 0x08) >> 3);
	fprintf(fd, "�Ǳ������(1��Ч0��Ч)(%d) ", (rs.monitor_situation & 0x04) >> 2);
	fprintf(fd, "���з������(1��Ч0��Ч)(%d) ", (rs.monitor_situation & 0x02) >> 1);
	fprintf(fd, "����/����װ̬(1��Ч0��Ч)(%d)\n", rs.monitor_situation & 0x01);

	fprintf(fd, "�ƶ����: ");
	fprintf(fd, "����(%d) ", (rs.brake_output & 0x80) >> 7);
	fprintf(fd, "�����ƶ�(1��Ч0��Ч)(%d) ", (rs.brake_output & 0x40) >> 6);
	fprintf(fd, "����(%d) ", (rs.brake_output & 0x20) >> 5);
	fprintf(fd, "����(%d) ", (rs.brake_output & 0x10) >> 4);
	fprintf(fd, "����(%d) ", (rs.brake_output & 0x08) >> 3);
	fprintf(fd, "���÷��(1��Ч0��Ч)(%d) ", (rs.brake_output & 0x04) >> 2);
	fprintf(fd, "���ü�ѹ(1��Ч0��Ч)(%d) ", (rs.brake_output & 0x02) >> 1);
	fprintf(fd, "�����ƶ�(1��Ч0��Ч)(%d)\n", rs.brake_output & 0x01);

	fprintf(fd, "ʱ��: ");
	fprintf(fd, "����(%d) ", (rs.speed & 0xfe00) >> 9);
	fprintf(fd, "ʱ��(%d)\n", rs.speed & 0x01ff);

	fprintf(fd, "����: ");
	fprintf(fd, "����(%d) ", (rs.speed_restriction & 0xfe00) >> 9);
	fprintf(fd, "����(%d)\n", rs.speed_restriction & 0x01ff);

	fprintf(fd, "�����: %d\n", rs.kilmeter_coordinate);

	fprintf(fd, "����ĳ��ٵȼ�����: %d\n", rs.speed_limit_level);

	return 0;
}


/*get communication borar event source according to BORARID in makefile*/
char get_commu_event_source(int board_id)
{
	int temp;
	
	if (board_id == COMMU0_ID){
		temp = SOURCE_COM_0;
	}else if (board_id == COMMU1_ID){
		temp = SOURCE_COM_1; 
	}else{
		temp = SOURCE_COM_0;
	}
	return (char)temp;
}


/*get and set master communication board */
static pthread_mutex_t master_commu_mutex = PTHREAD_MUTEX_INITIALIZER;
static int master_commu = -1;

void set_master_commu(EVENT_SOURCE master)
{
    pthread_mutex_lock(&master_commu_mutex);
    master_commu = master;
    pthread_mutex_unlock(&master_commu_mutex);
}

SYSTEM_STATUS get_master_commu(void)
{
    pthread_mutex_lock(&master_commu_mutex);
    EVENT_SOURCE temp = master_commu;
    pthread_mutex_unlock(&master_commu_mutex);
    return temp;
}

/*
 *check commu_brd source number  accordding to BOARDID in makefile
 */
int check_commu_num(void)
{
	int board_id = BOARDID;
	char temp;
	temp = get_commu_event_source(board_id);
	return temp;
}

int judge_branch_route(REALTIME_DATA realtime_data)
{
	char branch_route_num;
	if(realtime_data.partition_expand_line >> 7 == 1){
		branch_route_num = realtime_data.partition_expand_line & 0x80;
		if(get_sys_status() == SYS_NORMAL){
			if(get_trip_status() == SYS_STANDBY){
							
			}
		}
	}
}

