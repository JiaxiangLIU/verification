#ifndef VOTE_STATUS_H
#define VOTE_STATUS_H
#include <pthread.h>
#include "to_status.h"
#include "protocal.h"
#include  "global.h"
#include  "log.h"

#define distance_safe 300



typedef enum control_status{
	CTRL_UNSET=0,
	CTRL_SET=1
}CTRL_STATUS;
CTRL_STATUS get_ctrl_status();
void set_ctrl_status(CTRL_STATUS ctrl_status);





typedef enum marshall_status
{
    MAR_INIT = 0,
	MAR_ACK=1
}MAR_STATUS;


typedef enum branch_status
{
    BRANCH_INIT = 0,
	BRANCH_CHANGE=1
}BRANCH_STATUS;


typedef enum side_status{
	SIDE_INIT=0,
	SIDE_START=1,
}SIDE_STATUS;





void set_mar_status(MAR_STATUS status);
MAR_STATUS get_mar_status(void);


void set_trip_info(TRIP_INFO info);
TRIP_INFO get_trip_info();
void trip_handle();
void trip_ack_handle();

void set_mar_info(MARSHALLING_INFO info);
MARSHALLING_INFO get_mar_info();
void marshall_handle();
void mar_ack_handle();

void set_jieshi_info(JIESHI_INFO info);
JIESHI_INFO get_jieshi_info();
void jieshi_handle();
void jieshi_ack_handle();



void set_branch_status(BRANCH_STATUS status);
BRANCH_STATUS get_branch_status(void);

void set_branch_info(AHEAD_BRANCH_ROUTE_INFO info);
AHEAD_BRANCH_ROUTE_INFO get_branch_info();

void set_side_status(SIDE_STATUS status);
SIDE_STATUS get_side_status(void);

void set_side_info(AHEAD_SIDE_ROUTE_INFO info);
AHEAD_SIDE_ROUTE_INFO get_side_info();





/**
 * auto manual state
 */
int get_to_auto_gear();
void set_to_auto_gear(int status);
int get_to_auto_status();
void set_to_auto_gear(int status);
int get_to_manual_status();
void set_to_manual_gear(int status);
int get_auto_ack_status();
void set_auto_ack_status(int status);
int get_manual_ack_status();
void set_manual_ack_status(int status);




int broadcast_actboard(char arg);
int broadcast_sys_status(void);
int broadcast_trip_status(void);
#endif
