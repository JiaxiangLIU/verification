#include "vote_status.h"
static CTRL_STATUS ctrl_status=CTRL_UNSET;
static pthread_mutex_t ctrl_status_mutex = PTHREAD_MUTEX_INITIALIZER;

int get_throttle(){
	int  status;
	pthread_mutex_lock(&throttle_gear1.throttle_gear_mutex);
	status = throttle_gear1.gear;
    pthread_mutex_unlock(&throttle_gear1.throttle_gear_mutex);
    return status;
}

/*转换自动的时候档位*/
static int to_auto_gear;
static pthread_mutex_t to_auto_throttle_mutex = PTHREAD_MUTEX_INITIALIZER;
int get_to_auto_gear(){
	int status;
	pthread_mutex_lock(&to_auto_throttle_mutex);
	status = to_auto_gear;
    pthread_mutex_unlock(&to_auto_throttle_mutex);
    return status;
}
void set_to_auto_gear(int status){
	pthread_mutex_lock(&to_auto_throttle_mutex);
	to_auto_gear = status;
    pthread_mutex_unlock(&to_auto_throttle_mutex);
}


/*转换自动的标志*/
static int to_auto_status=0;
static pthread_mutex_t to_auto_status_mutex = PTHREAD_MUTEX_INITIALIZER;
int get_to_auto_status(){
	int status;
	pthread_mutex_lock(&to_auto_status_mutex);
	status = to_auto_status;
    pthread_mutex_unlock(&to_auto_status_mutex);
    return status;
}
void set_to_auto_status(int status){
	pthread_mutex_lock(&to_auto_status_mutex);
	to_auto_status = status;
    pthread_mutex_unlock(&to_auto_status_mutex);
}

/*转换手动的标志*/
static int to_manual_status=0;
static pthread_mutex_t to_manual_status_mutex = PTHREAD_MUTEX_INITIALIZER;
int get_to_manual_status(){
	int status;
	pthread_mutex_lock(&to_manual_status_mutex);
	status = to_manual_status;
    pthread_mutex_unlock(&to_manual_status_mutex);
    return status;
}
void set_to_manual_status(int status){
	pthread_mutex_lock(&to_manual_status_mutex);
	to_manual_status = status;
    pthread_mutex_unlock(&to_manual_status_mutex);
}

/*转换自动的确认标志*/
static int auto_ack_status=0;
static pthread_mutex_t auto_ack_status_mutex = PTHREAD_MUTEX_INITIALIZER;
int get_auto_ack_status(){
	int status;
	pthread_mutex_lock(&auto_ack_status_mutex);
	status = auto_ack_status;
    pthread_mutex_unlock(&auto_ack_status_mutex);
    return status;
}
void set_auto_ack_status(int status){
	pthread_mutex_lock(&auto_ack_status_mutex);
	auto_ack_status = status;
    pthread_mutex_unlock(&auto_ack_status_mutex);
}

/*转换手动的确认标志*/
static int manual_ack_status=0;
static pthread_mutex_t manual_ack_status_mutex = PTHREAD_MUTEX_INITIALIZER;
int get_manual_ack_status(){
	int status;
	pthread_mutex_lock(&manual_ack_status_mutex);
	status = manual_ack_status;
    pthread_mutex_unlock(&manual_ack_status_mutex);
    return status;
}
void set_manual_ack_status(int status){
	pthread_mutex_lock(&manual_ack_status_mutex);
	manual_ack_status = status;
    pthread_mutex_unlock(&manual_ack_status_mutex);
}








CTRL_STATUS get_ctrl_status(){
	CTRL_STATUS status;
	pthread_mutex_lock(&ctrl_status_mutex);
	status = ctrl_status;
    pthread_mutex_unlock(&ctrl_status_mutex);
    return status;
}
void set_ctrl_status(CTRL_STATUS status){
	pthread_mutex_lock(&ctrl_status_mutex);
	ctrl_status = status;
    pthread_mutex_unlock(&ctrl_status_mutex);
}
int broadcast_actboard(char arg){
	log_info("broadcast active board %d\n",arg);
	char cmd[2];
	cmd[0]=VOTE_0_SOURCE;
	EVENT_TYPE event_code=EVENT_BROADCAST_COM_ADDR ;
	cmd[1]=(char)event_code;
	char par=arg;
	if(frame_encap(COM_SOC,CORE_0_DES,COMMAND_FRAME_CODE,cmd,&par,1)!=PROTOCAL_SUCCESS)
		log_error("send error \n");
	if(frame_encap(COM_SOC,act_com,COMMAND_FRAME_CODE,cmd,&par,1)!=PROTOCAL_SUCCESS)
		log_error("send error \n");
	return 0;
}
int broadcast_sys_status(void){
	log_info("broadcast sys status %d\n",get_sys_status());
	char cmd[2];
	cmd[0]=VOTE_0_SOURCE;
	EVENT_TYPE event_code=EVENT_MASTER_VOTE_EXCHANGE ;
	cmd[1]=(char)event_code;
	SYSTEM_STATUS sys=get_sys_status();
	if(frame_encap(COM_SOC,CORE_0_DES,COMMAND_FRAME_CODE,cmd,&sys,1)!=PROTOCAL_SUCCESS)
		log_error("send error \n");
	if(frame_encap(COM_SOC,act_com,COMMAND_FRAME_CODE,cmd,&sys,1)!=PROTOCAL_SUCCESS)
		log_error("send error \n");
	if(frame_encap(COM_SOC,act_con,COMMAND_FRAME_CODE,cmd,&sys,1)!=PROTOCAL_SUCCESS)
		log_error("send error \n");
	return 0;
}
int broadcast_trip_status(void){
	log_info("broadcast trip status %d\n",get_trip_status());
	char cmd[2];
	cmd[0]=VOTE_0_SOURCE;
	EVENT_TYPE event_code=EVENT_SYS_TRIP_STATUS_SEND ;
	cmd[1]=(char)event_code;
	TRIP_STATUS trip=get_trip_status();
	if(frame_encap(COM_SOC,CORE_0_DES,COMMAND_FRAME_CODE,cmd,&trip,1)!=PROTOCAL_SUCCESS)
		log_error("send error \n");
	if(frame_encap(COM_SOC,act_com,COMMAND_FRAME_CODE,cmd,&trip,1)!=PROTOCAL_SUCCESS)
		log_error("send error \n");
	if(frame_encap(COM_SOC,act_con,COMMAND_FRAME_CODE,cmd,&trip,1)!=PROTOCAL_SUCCESS)
		log_error("send error \n");
	return 0;
}


static pthread_mutex_t mar_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static  MAR_STATUS  mar_status = MAR_INIT;
static pthread_mutex_t mar_info_mutex = PTHREAD_MUTEX_INITIALIZER;
static  MARSHALLING_INFO mar_info;


static pthread_mutex_t branch_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static BRANCH_STATUS branch_status=BRANCH_INIT;
static AHEAD_BRANCH_ROUTE_INFO branch_info;
static pthread_mutex_t branch_info_mutex=PTHREAD_MUTEX_INITIALIZER;


static pthread_mutex_t side_status_mutex = PTHREAD_MUTEX_INITIALIZER;
static BRANCH_STATUS side_status=SIDE_INIT;
static AHEAD_SIDE_ROUTE_INFO side_info;
static pthread_mutex_t side_info_mutex=PTHREAD_MUTEX_INITIALIZER;


void set_mar_status(MAR_STATUS status)
{
    pthread_mutex_lock(&mar_status_mutex);
    mar_status = status;
    pthread_mutex_unlock(&mar_status_mutex);
}

MAR_STATUS get_mar_status(void)
{
    pthread_mutex_lock(&mar_status_mutex);
    MAR_STATUS temp = mar_status;
    pthread_mutex_unlock(&mar_status_mutex);
    return temp;
}
void set_branch_status(BRANCH_STATUS status)
{
    pthread_mutex_lock(&branch_status_mutex);
    branch_status = status;
    pthread_mutex_unlock(&branch_status_mutex);
}

BRANCH_STATUS get_branch_status(void)
{
    pthread_mutex_lock(&branch_status_mutex);
    BRANCH_STATUS temp = branch_status;
    pthread_mutex_unlock(&branch_status_mutex);
    return temp;
}
void set_side_status(SIDE_STATUS status)
{
    pthread_mutex_lock(&side_status_mutex);
    side_status = status;
    pthread_mutex_unlock(&side_status_mutex);
}

SIDE_STATUS get_side_status(void)
{
    pthread_mutex_lock(&side_status_mutex);
    SIDE_STATUS temp = side_status;
    pthread_mutex_unlock(&side_status_mutex);
    return temp;
}



void set_mar_info(MARSHALLING_INFO info){
	pthread_mutex_lock(&mar_info_mutex);
	mar_info=info;
	pthread_mutex_unlock(&mar_info_mutex);
}

MARSHALLING_INFO get_mar_info(){
	MARSHALLING_INFO temp;
	pthread_mutex_lock(&mar_info_mutex);
	temp=mar_info;
	pthread_mutex_unlock(&mar_info_mutex);
	return temp;
}


void set_branch_info(AHEAD_BRANCH_ROUTE_INFO info){
	pthread_mutex_lock(&branch_info_mutex);
	branch_info=info;
	pthread_mutex_unlock(&branch_info_mutex);
}

AHEAD_BRANCH_ROUTE_INFO get_branch_info(){
	AHEAD_BRANCH_ROUTE_INFO temp;
	pthread_mutex_lock(&branch_info_mutex);
	temp=branch_info;
	pthread_mutex_unlock(&branch_info_mutex);
	return temp;
}
void set_side_info(AHEAD_SIDE_ROUTE_INFO info){
	pthread_mutex_lock(&side_info_mutex);
	side_info=info;
	pthread_mutex_unlock(&side_info_mutex);
}

AHEAD_SIDE_ROUTE_INFO get_side_info(){
	AHEAD_SIDE_ROUTE_INFO temp;
	pthread_mutex_lock(&side_info_mutex);
	temp=side_info;
	pthread_mutex_unlock(&side_info_mutex);
	return temp;
}


static pthread_mutex_t trip_info_mutex = PTHREAD_MUTEX_INITIALIZER;
static  TRIP_INFO trip_info;

void set_trip_info(TRIP_INFO info){
	pthread_mutex_lock(&trip_info_mutex);
	trip_info=info;
	pthread_mutex_unlock(&trip_info_mutex);
}

TRIP_INFO get_trip_info(){
	TRIP_INFO temp;
	pthread_mutex_lock(&trip_info_mutex);
	temp=trip_info;
	pthread_mutex_unlock(&trip_info_mutex);
	return temp;
}

static pthread_mutex_t jieshi_info_mutex = PTHREAD_MUTEX_INITIALIZER;
static  JIESHI_INFO jieshi_info;

void set_jieshi_info(JIESHI_INFO info){
	pthread_mutex_lock(&jieshi_info_mutex);
	jieshi_info=info;
	pthread_mutex_unlock(&jieshi_info_mutex);
}

JIESHI_INFO get_jieshi_info(){
	JIESHI_INFO temp;
	pthread_mutex_lock(&jieshi_info_mutex);
	temp=jieshi_info;
	pthread_mutex_unlock(&jieshi_info_mutex);
	return temp;
}







