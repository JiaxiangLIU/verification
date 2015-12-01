#ifndef __TO_STATUS_H__
#define __TO_STATUS_H__
/* changelog��ver1.000 2015-10-27 ������Զ����߼�������¼�����ʼ�����¼� */
/* changelog��ver1.100 2015-11-04 ���������¼������Ӧ���¼���ȥ��������¼����¼��б��Ӧ */
/* changelog��ver1.200 2015-11-06 ��Ӵ������¼��������¼��� */
/* changelog��ver1.300 2015-11-11 ��ӽ�·���ݽ���Ӧ���¼� */

/* ���� */
typedef enum {
    TO_WEEKDAY_MONDAY = 1,
    TO_WEEKDAY_TUESDAY = 2,
    TO_WEEKDAY_WEDNESDAY = 3,
    TO_WEEKDAY_THURSDAY = 4,
    TO_WEEKDAY_FRIDAY = 5,
    TO_WEEKDAY_SATURDAY = 6,
    TO_WEEKDAY_SUNDAY = 7
} TO_WEEKDAY;

/* ���� */
typedef struct {
    unsigned short year;      /* AD */
    unsigned char month;      /* 1=Jan */
    unsigned char day;        /* 1...31 */
    unsigned char wday;       /* 1=Monday...7=Sunday */
} TO_DATE;

/* ʱ�� */
typedef struct {
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned char hundredths;
} TO_TIME;

/* ���ں�ʱ�� */
typedef struct {
    TO_DATE date;
    TO_TIME time;
} TO_DATE_TIME;

/* �������ʾ��Ϣ */
/*
typedef struct  {
	int32_t trip_number;//che ci[18-20]
	int16_t cargo_number;//liang shu[22-23]
	int16_t equivalent_length;//ji chang[26-27]
	int16_t total_weight;//zong zhong[24-25]
	int16_t loco_number;//ji che hao[11-12]
	int8_t trip_status;//ben bu/ke huo[21]
	int16_t start_station;//che zhan hao[42-43]
	int16_t speed_limit_level;//che su deng ji xian zhi[66-67]
	int8_t route_number;//shu ru jiao lu hao[38]
} MARSHALLING_INFO;*/

/* ����״̬ */
typedef enum {
    SYS_INIT = 0, /* ϵͳ��ʼ�� */
    SYS_FAULT = 1, /* ϵͳ���� */
    SYS_MAINTENANCE = 2, /* ϵͳά�� */
    SYS_LAUNCHED = 3, /* ϵͳ��ʼ����� */
    SYS_STANDBY = 4, /* ϵͳ���� */
    SYS_NORMAL = 5, /* ϵͳ���� */
    SYS_AUTOCONTROL = 6, /*  */
	SYS_MAX
} SYSTEM_STATUS;

/* ����״̬ */
typedef enum {
   	TRAIN_DUTY = 0, /* ���� */
	TRAIN_ASSISTING = 1, /* ���� */
	TRAIN_MAX
} TRAIN_STATUS;

/* �г�״̬ */
typedef enum {
	TRIP_DATA_RECVING = 0, /* ������·������ */
	TRIP_DATA_COMPLETE = 1, /* ������·���ݽ������ */
	TRIP_MARSHALLING_CONFIRM = 2, /* �������ʾ��Ϣȷ�� */
	TRIP_INIT = 3, /* �г̳�ʼ�� */
//	TRIP_INIT_COMPLETE = 4, /* �г̳�ʼ����� */
	TRIP_START = 4, /* �г̿�ʼ */
	TRIP_PAUSE = 5, /* �г���ͣ */
	TRIP_COMPLETE = 6, /* �г̽��� */
	TRIP_ANOMALY = 7, /* �г��쳣 */
	TRIP_MAX
} TRIP_STATUS;

/* �ٿ�״̬ */
typedef enum {
	CONTROL_MANNUL = 0, /* �ֶ� */
	CONTROL_AUTO = 1, /* �Զ� */
	CONTROL_MAX
} CONTROL_STATUS;

/* �Զ�ʹ��״̬ */
typedef enum {
	AUTO_DISABLE = 0, /* �Զ�ʧ�� */
	AUTO_ENABLE = 1, /* �Զ�ʹ�� */
	AUTO_MAX
} AUTO_STATUS;

/* �¼�Դ */
typedef enum {
	SOURCE_COM_0 = 0x00, /* ͨѶ��0�¼�Դ */
	SOURCE_COM_1 = 0x01, /* ͨѶ��1�¼�Դ */
	SOURCE_CORE_0 = 0x02, /* ���İ�0�¼�Դ */
	SOURCE_CORE_1 = 0x03, /* ���İ�1�¼�Դ */
	SOURCE_CORE_2 = 0x04, /* ���İ�2�¼�Դ */
	SOURCE_CORE_3 = 0x05, /* ���İ�3�¼�Դ */
	SOURCE_CONTROL_0 = 0x06, /* ���ƺ�0�¼�Դ */
	SOURCE_CONTROL_1 = 0x07, /* ���ƺ�1�¼�Դ */
	SOURCE_VOTE_0 = 0x08, /* ������¼�Դ */
	SOURCE_INPUT_0 = 0x09, /* ������¼�Դ */
	SOURCE_OUTPUT_0 = 0x0a, /* ������¼�Դ */
	MAX_SOURCE
} EVENT_SOURCE;

typedef enum {
	/* ����Ӧ���¼� */
	COM_0_ACK = 0xF0,     /* ͨ�Ű�0Ӧ�� */
	COM_1_ACK = 0xF1,     /* ͨ�Ű�1Ӧ�� */
	CORE_0_ACK = 0xF2,    /* ���İ�0Ӧ�� */
	CORE_1_ACK = 0xF3,    /* ���İ�1Ӧ�� */
	CORE_2_ACK = 0xF4,    /* ���İ�2Ӧ�� */
	CORE_3_ACK = 0xF5,    /* ���İ�3Ӧ�� */
	CONTROL_0_ACK = 0xF6, /* ���ƺ�0Ӧ�� */
	CONTROL_1_ACK = 0xF7, /* ���ƺ�1Ӧ�� */
	VOTE_ACK = 0xF8,      /* �����Ӧ�� */
	EVENT_SEND_ERROR = 0xFF,      /* ���ʹ����¼� */

	/* ͨ�Ű��¼� */
	EVENT_COM_HEARTBEAT = 0x00, /* ͨ�Ű����� */
	EVENT_COM_REGISTER = 0x01, /* ͨ�Ű�ע�� */
	//EVENT_COM_HEARTBEAT = 0x02, /* ͨ�Ű����� */
	EVENT_LKJ_VERSION_DIFFERENCE = 0x03, /* LKJ�밲ȫ��Ϣƽ̨���ݰ汾��һ�� */
	EVENT_TRIP_STATUS_NOTICE = 0x04, /* �г�״̬֪ͨ��������·���ݽ�����/������·���ݽ�����ɣ�*/
	
	EVENT_DUTY_ASSISTING_CHANGE = 0x06, /* ����״̬�仯 */
	//EVENT_ROUTE_VERSION_DIFFERENCE = 0x07, /* ��·���ݰ汾��һ�´�����·��ȡ�뷢�� */
	
	//EVENT_BASE_LINE_DATA_RECVING = 0x01, /* �г�״̬-������·���ݽ����� */
	//EVENT_BASE_LINE_DATA_RECV_COMPLETE = 0x02, /* �г�״̬-������·���ݽ������ */
	
	//EVENT_ROUTE_RECV_COMPLETE = 0xD1, /* ��·��ȡ��� */
	EVENT_ROUTE_DATA_SEND = 0xD1, /* ��·���ݰ汾��һ�´�����·��ȡ��ɲ����ͣ��¶� */
	EVENT_ROUTE_DATA_CURVE = 0xD2, /* ��·���ݰ汾��һ�´������ͣ����� */
	EVENT_ROUTE_DATA_LIMIT = 0xD3, /* ��·���ݰ汾��һ�´������ͣ����� */
	EVENT_ROUTE_DATA_TELESEME = 0xD4, /* ��·���ݰ汾��һ�´������ͣ��źŻ� */
	EVENT_ROUTE_DATA_TUNNEL = 0xD5, /* ��·���ݰ汾��һ�´������ͣ���� */
	EVENT_ROUTE_DATA_STATION = 0xD6, /* ��·���ݰ汾��һ�´������ͣ���վ */
	EVENT_FRONT_SPEED_LIMIT = 0xD7, /* ǰ����ʱ���� */
	EVENT_FRONT_BRANCH_TRANSFER = 0xD8, /* ǰ��֧��ת�� */
	//EVENT_FRONT_ROUTE_TRANSFER = 0xD9, /* ǰ����·ת�� */
	EVENT_FRONT_SIDE_LINE_PASS = 0xDA, /* ǰ������ͨ�� */
	EVENT_MARSHALLING_RE_CHANGE = 0xDB, /* ͨ�Ű������Ϣ���ʾ��Ϣ���٣��仯 */
	//EVENT_MARSHALLING_INFO
	//EVENT_OPT_ROUTE_DATA
	EVENT_REAL_TIME_INFO = 0xDC, /* ʵʱ��Ϣ�� */
	EVENT_ROUTE_VERSION_SEND = 0xDD, /* ͨ�Ű彻·���ݰ汾����ȡ�뷢�� */
	EVENT_TRIP_CHANGE = 0xDE, /* �г���Ϣ�仯 */
	EVENT_MARSHALLING_CHANGE = 0xDF, /* ��ʾ��Ϣ�仯 */
	EVENT_CLOCK_SYNC = 0xE1, /* ʱ��ͬ�� */
	//EVENT_COM_INIT_FAILUER
	
	EVENT_REAL_TIME_ERROR = 0xA1, /* ʵʱ��Ϣ�������� */
	EVENT_BASE_LINE_DATA_ERROR = 0xA2, /* ������·���ݽ��մ��� */
	EVENT_ROUTE_EXTRACT_FAILUER = 0xA3, /*�г̳�ʼ����·��ȡʧ�� */
	EVENT_MARSHALLING_ERROR = 0xA4, /* ��ʾ��Ϣ���մ��� */
	EVENT_STATION_ERROR = 0xA5, /* ��վ��Ϣ���մ��� */
	EVENT_COM_EXTERNAL_CAN_FAULT = 0xA6, /* ��ͨ�Ű���CANͨ���쳣 */
	EVENT_COM_INTERNAL_CAN_FAULT = 0xA7, /* ��ͨ�Ű���CANͨ���쳣 */
	
    EVENT_COM_MAX,
//} COM_EVENT_TYPE;

//typedef enum {
	/* ���İ��¼� */
	EVENT_CORE_HEARTBEAT = 0x00, /* ���İ����� */
	EVENT_CORE_REGISTER = 0x01, /* ���İ�ע�� */
	//EVENT_CORE_HEARTBEAT = 0x02, /* ���İ����� */
	//EVENT_MASTER_CORE_EXCHANGE = 0x03, /* ���İ�����״̬�仯���� */
	EVENT_ROUTE_VERSION_RESULT = 0x04, /* ���İ彻·���ݰ汾һ����У�������� */
	EVENT_TRIP_OPT_COMPLETE = 0x05, /* �г̳�ʼ���Ż�������� */
	
	EVENT_ROUTE_DATA_SEND_ACK = 0x06, /* ��·���ݰ汾��һ�´�����·��ȡ��ɲ����ͣ��¶�Ӧ�� */
	EVENT_ROUTE_DATA_CURVE_ACK = 0x07, /* ��·���ݰ汾��һ�´������ͣ�����Ӧ��  */
	EVENT_ROUTE_DATA_LIMIT_ACK = 0x08, /* ��·���ݰ汾��һ�´������ͣ�����Ӧ��  */
	EVENT_ROUTE_DATA_TELESEME_ACK = 0x09, /* ��·���ݰ汾��һ�´������ͣ��źŻ�Ӧ��  */
	EVENT_ROUTE_DATA_TUNNEL_ACK = 0x0A, /* ��·���ݰ汾��һ�´������ͣ����Ӧ��  */
	EVENT_ROUTE_DATA_STATION_ACK = 0x0B, /* ��·���ݰ汾��һ�´������ͣ���վӦ��  */
	
	//EVENT_UPDATE_OPT_MARSHALLING = 0xD1, /* �����Ż��ý�ʾ��Ϣ�������Ϣ */
	EVENT_REAL_TIME_CTRL = 0xD1, /* ʵʱ������״̬���� */
	//EVENT_CORE_INIT_FAILUER
	
	EVENT_CORE_CAN_FAULT = 0xA1, /* ���İ忨0/1/2/3CANͨ���쳣 */
	EVENT_TRIP_ROUTE_ERROR = 0xA2, /* �г̽�·���� */
	EVENT_TRIP_POSITION_ERROR = 0xA3, /* �г̶�λ���� */
	EVENT_TRIP_PARA_ERROR = 0xA4, /* �г̲������� */
	EVENT_TRIP_OPT_ERROR = 0xA5, /* �г̳�ʼ���Ż������쳣 */
	EVENT_TEMP_OPT_ERROR = 0xA6, /* ��ʱ�Ż������쳣 */
	
    EVENT_CORE_MAX,
//} CORE_EVENT_TYPE;

//typedef enum {
	/* ���ƺ��¼� */
	EVENT_CTRL_BOX_HEARTBEAT = 0x00, /* ���ƺ����� */
	EVENT_CTRL_BOX_REGISTER = 0x01, /* ���ƺ�ע�� */
	//EVENT_CTRL_BOX_HEARTBEAT = 0x02, /* ���ƺ����� */
	EVENT_CTRL_BOX_MASTER_SET = 0x03, /* ��������Ԫ�趨 */
	EVENT_CTRL_BOX_MASTER_RELIEVE = 0x04, /* ��������Ԫ��� */
	//EVENT_MARSHALLING_CONFIRM = 0x05, /* �������ʾ��Ϣȷ�� */
	EVENT_TRIP_INIT_START = 0x05, /* ���ƺ��г̳�ʼ���������� */
	EVENT_MANNAL_TO_AUTO = 0x06, /* �ֶ�ת�Զ����� */
	EVENT_AUTO_TO_MANNAL = 0x07, /* �Զ�ת�ֶ����� */
	
	EVENT_TWO_GEAR_TO_MANNUL = 0x08, /* ����2�����ϣ�ȷ��ת�ֶ����� */
	EVENT_SYS_TO_MAINTENANCE = 0x09, /* �������ά��ģʽ����4���� */
	EVENT_EXIT_MAINTENANCE = 0x0A, /* �˳�����ά��ģʽ����3���� */
	EVENT_KEEP_AUTO = 0x0B, /* �����Զ�״̬���̵��������˱պϣ�����1���� */
	EVENT_KEEP_MANNAL = 0x0C, /* �����ֶ�״̬���̵������ն˱պϣ�����2���� */
	EVENT_SEND_AUTO_GEAR = 0x0D, /* �����Զ����Ե�λ��X */
	EVENT_GEAR8_AUTO_CONFIRM_ACK = 0x0E, /* ��8����ȷ��ת�Զ����� */
	EVENT_TRIP_OVER = 0x0F, /* �����г̽��� */
	
	EVENT_VOTE_HEARTBEAT_STOP = 0xA1, /* ����������쳣 */
	EVENT_CTRL_BOX_CAN_FAULT = 0xA2, /* ���ƺ�0/1CANͨ���쳣 */
	EVENT_CTRL_BOX_PARSE_ERROR = 0xA3, /* ���ƺ�0/1���ݽ����쳣 */
	
    EVENT_CTRL_MAX,
//} CTRL_EVENT_TYPE;

//typedef enum {
	/* ������¼� */
	EVENT_VOTE_HEARTBEAT = 0x00, /* ��������� */
	EVENT_BROADCAST_COM_ADDR = 0x02, /* ��ͨ�Ű��ַ���� */
	EVENT_MASTER_VOTE_EXCHANGE = 0x03, /* ϵͳ����״̬�仯���� */
	EVENT_LOCK_SCREEN = 0x04, /* LKJ�밲ȫ��Ϣƽ̨���ݰ汾��һ����ʾ������ */
	EVENT_VOTE_TRIP_INIT_START = 0x05, /* ������г̳�ʼ������ */
	EVENT_SYS_TRIP_STATUS_SEND = 0x06, /* ϵͳ�г�״̬���� */
	//EVENT_VOTE_BASE_DATA_RECVING = 0x05, /* �г�״̬����-������·���ݽ����� */
	//EVENT_VOTE_BASE_DATA_RECV_COMPLETE = 0x06, /* �г�״̬����-������·���ݽ������ */
	//EVENT_VOTE_BRANCH_TRANSFER_PASS = 0x07, /* ֧��ת�Ƶ�ͨ�� */
	//EVENT_VOTE_ROUTE_TRANSFER_PASS = 0x08, /* ��·ת�Ƶ�ͨ�� */
	//EVENT_VOTE_SIDE_LINE_PASS = 0x09, /* �����յ�ͨ�� */
	EVENT_MANNAL_GEAR_CHANGE = 0x0A, /* ���͵�ǰ˾�����ֱ�λ��X */
	//EVENT_AIR_BRAKE_START = 0x0B, /* �����ƶ����� */
	//EVENT_SPEED_TOO_LOW = 0x0C, /* �ٶȵ���20Km/h */
	//EVENT_AUTO_START = 0x0D, /* ת�Զ���ʻ���� */
	//EVENT_MANNAL_START = 0x0E, /* ת�ֶ���ʻ���� */
	EVENT_TWO_GEAR_CONFIRM = 0x0E, /* ת�ֶ��߳�2��λ����ƺ�ȷ�� */
	EVENT_MANNAL_COMPLETE = 0x0F, /* ת�ֶ���ʻ��� */
	EVENT_AUTO_COMPLETE = 0x10, /* ת�Զ���ʻ��� */
	EVENT_BRANCH_TRANSFER_TIME = 0x11, /* ֧��ת�ƾ����������ʱ�䲻������ */
	EVENT_SPEED_LIMIT_TIME = 0x12, /* ��ʱ���پ����������ʱ�䲻������ */
	EVENT_SIG_CHANGE_TIME = 0x13, /* �źű仯�����������ʱ�䲻������ */
	EVENT_AUTO_TIME_ALARM = 0x14, /* 30sת���Զ���ʱ���� */
	EVENT_MANNUL_TIME_ALARM = 0x15, /* 30sת���ֶ���ʱ���� */
	EVENT_BRAKE_GEAR0 = 0x16, /* �����ƶ����ѣ��������Ե�λ */
	EVENT_GEAR8_AUTO_CONFIRM = 0x17, /* ת�Զ��ֱ�λ��8������ȷ�� */
	
	
	EVENT_VOTE_UPDATE_MARSHALLING = 0xD1, /* ����������Ϣ���ʾ��Ϣ���� */
	EVENT_VOTE_SENT_MARSHALLING = 0xD2, /* ���ͱ�����Ϣ���ʾ��Ϣ�����İ� */
	//EVENT_SIG_LIGHT_CHANGE = 0xD3, /* ǰ���źŵƱ仯 */
	EVENT_DISP_AUTO = 0xD4, /* ת�Զ����̽�����30���ڰ������Ե�λ */
	EVENT_DISP_MANNAL = 0xD5, /* ת�ֶ����̽�����30���ڰ������е�λ */
	EVENT_DISP_REAL_TIME_INFO = 0xD6, /* ʵʱ��Ϣ��ʾ���Զ��س���λ */
	EVENT_VOTE_TRIP_INIT_COMPLETE = 0xD7, /* �����г̳�ʼ�����״̬���г������������г̱��*/
	EVENT_VOTE_BRANCH_TRANSFER_PASS = 0xD8, /* ǰ��֧��ת�� */
	//EVENT_VOTE_ROUTE_TRANSFER_PASS = 0xD9, /* ǰ����·ת�� */
	EVENT_VOTE_SIDE_LINE_PASS_START = 0xDA, /* ǰ������ͨ����� */
	EVENT_VOTE_SIDE_LINE_PASS_END = 0xDB, /* ǰ������ͨ���յ� */
	
	EVENT_TRIP_UPDATE = 0xDC, /* ������г���Ϣ���� */
	EVENT_SEND_TRIP_TO_CORE = 0xDD, /* �����г���Ϣ�����İ� */
	EVENT_MAR_UPDATE = 0xDE, /* ������ʾ��Ϣ���� */
	EVENT_SEND_MAR_TO_CORE = 0xDF, /* ���ͽ�ʾ��Ϣ�����İ� */
	
	EVENT_BOARD_REGISTER_FAILUER = 0xA1, /* ĳ���������ע��ʧ�� */
	EVENT_MASTER_COM_HEARTBEAT_STOP = 0xA2, /* ��ͨ�Ű������� */
	EVENT_ALL_COM_HEARTBEAT_STOP = 0xA3, /* ͨ�Ű�0/1�������� */
	EVENT_CORE_HEARTBEAT_STOP = 0xA4, /* ���İ�(0&&1)||(2&&3)������ */
	EVENT_MASTER_CTRL_HEARTBEAT_STOP = 0xA5, /* �����ƺ������� */
	//EVENT_INPUT_HEARTBEAT_STOP = 0xA6, /* ����������� */
	//EVENT_OUTPUT_HEARTBEAT_STOP = 0xA7, /* ����������� */
	EVENT_RELAY_FAULT = 0xA8, /* �̵�������״̬�쳣 */
	EVENT_COM_FAULT_HANDLE = 0xA9, /* ͨ�Ű��쳣���� */
	EVENT_CORE_FAULT_HANDLE = 0xAA, /* ���İ��쳣���� */
	EVENT_CTRL_FAULT_HANDLE = 0xAB, /* ���ƺ��쳣���� */
	EVENT_VOTE_FAULT_HANDLE = 0xAC, /* ���������쳣 */
	//EVENT_CONFIRM_MARSHALLING_INFO
	
    EVENT_VOTE_MAX,
//} VOTE_EVENT_TYPE;

//typedef enum {
	/* ������¼� */
	EVENT_DIGITAL_ANALOG = 0x01, /* ��ģ�źű仯 */
	
    EVENT_INPUT_MAX,
//} INPUT_EVENT_TYPE;


//typedef enum {
	/* ������¼� */
	EVENT_HEARTBEAT_STOP = 0x01, /* ����������쳣 */
	
    EVENT_OUTPUT_MAX,
//} OUTPUT_EVENT_TYPE;
} EVENT_TYPE;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/* ���� ��ȡ ϵͳ״̬ */
	void set_sys_status(SYSTEM_STATUS status);
	SYSTEM_STATUS get_sys_status(void);
	
	/* ���� ��ȡ ����״̬ */
	void set_train_status(TRAIN_STATUS status);
	TRAIN_STATUS get_train_status(void);
	
	/* ���� ��ȡ �г�״̬ */
	void set_trip_status(TRIP_STATUS status);
	TRIP_STATUS get_trip_status(void);
	
	/* ���� ��ȡ �ٿ�״̬ */
	void set_control_status(CONTROL_STATUS status);
	CONTROL_STATUS get_control_status(void);
	
	/* ���� ��ȡ �Զ�ʹ��״̬ */
	void set_auto_status(AUTO_STATUS status);
	AUTO_STATUS get_auto_status(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif 
