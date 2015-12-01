#ifndef __TO_STATUS_H__
#define __TO_STATUS_H__
/* changelog：ver1.000 2015-10-27 配合手自动新逻辑添加新事件、初始化新事件 */
/* changelog：ver1.100 2015-11-04 更新心跳事件、添加应答事件、去掉多余的事件与事件列表对应 */
/* changelog：ver1.200 2015-11-06 添加错误发送事件到公共事件中 */

/* 星期 */
typedef enum {
    TO_WEEKDAY_MONDAY = 1,
    TO_WEEKDAY_TUESDAY = 2,
    TO_WEEKDAY_WEDNESDAY = 3,
    TO_WEEKDAY_THURSDAY = 4,
    TO_WEEKDAY_FRIDAY = 5,
    TO_WEEKDAY_SATURDAY = 6,
    TO_WEEKDAY_SUNDAY = 7
} TO_WEEKDAY;

/* 日期 */
typedef struct {
    unsigned short year;      /* AD */
    unsigned char month;      /* 1=Jan */
    unsigned char day;        /* 1...31 */
    unsigned char wday;       /* 1=Monday...7=Sunday */
} TO_DATE;

/* 时间 */
typedef struct {
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned char hundredths;
} TO_TIME;

/* 日期和时间 */
typedef struct {
    TO_DATE date;
    TO_TIME time;
} TO_DATE_TIME;

/* 编组与揭示信息 */
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

/* 主机状态 */
typedef enum {
    SYS_INIT = 0, /* 系统初始化 */
    SYS_FAULT = 1, /* 系统故障 */
    SYS_MAINTENANCE = 2, /* 系统维护 */
    SYS_LAUNCHED = 3, /* 系统初始化完成 */
    SYS_STANDBY = 4, /* 系统待机 */
    SYS_NORMAL = 5, /* 系统正常 */
    SYS_AUTOCONTROL = 6, /*  */
	SYS_MAX
} SYSTEM_STATUS;

/* 本补状态 */
typedef enum {
   	TRAIN_DUTY = 0, /* 本务 */
	TRAIN_ASSISTING = 1, /* 补机 */
	TRAIN_MAX
} TRAIN_STATUS;

/* 行程状态 */
typedef enum {
	TRIP_DATA_RECVING = 0, /* 基础线路接收中 */
	TRIP_DATA_COMPLETE = 1, /* 基础线路数据接收完成 */
	TRIP_MARSHALLING_CONFIRM = 2, /* 编组与揭示信息确认 */
	TRIP_INIT = 3, /* 行程初始化 */
//	TRIP_INIT_COMPLETE = 4, /* 行程初始化完成 */
	TRIP_START = 4, /* 行程开始 */
	TRIP_PAUSE = 5, /* 行程暂停 */
	TRIP_COMPLETE = 6, /* 行程结束 */
	TRIP_ANOMALY = 7, /* 行程异常 */
	TRIP_MAX
} TRIP_STATUS;

/* 操控状态 */
typedef enum {
	CONTROL_MANNUL = 0, /* 手动 */
	CONTROL_AUTO = 1, /* 自动 */
	CONTROL_MAX
} CONTROL_STATUS;

/* 自动使能状态 */
typedef enum {
	AUTO_DISABLE = 0, /* 自动失能 */
	AUTO_ENABLE = 1, /* 自动使能 */
	AUTO_MAX
} AUTO_STATUS;

/* 事件源 */
typedef enum {
	SOURCE_COM_0 = 0x00, /* 通讯板0事件源 */
	SOURCE_COM_1 = 0x01, /* 通讯板1事件源 */
	SOURCE_CORE_0 = 0x02, /* 核心板0事件源 */
	SOURCE_CORE_1 = 0x03, /* 核心板1事件源 */
	SOURCE_CORE_2 = 0x04, /* 核心板2事件源 */
	SOURCE_CORE_3 = 0x05, /* 核心板3事件源 */
	SOURCE_CONTROL_0 = 0x06, /* 控制盒0事件源 */
	SOURCE_CONTROL_1 = 0x07, /* 控制盒1事件源 */
	SOURCE_VOTE_0 = 0x08, /* 表决板事件源 */
	SOURCE_INPUT_0 = 0x09, /* 输入板事件源 */
	SOURCE_OUTPUT_0 = 0x0a, /* 输出板事件源 */
	MAX_SOURCE
} EVENT_SOURCE;

typedef enum {
	/* 公共应答事件 */
	COM_0_ACK = 0xF0,     /* 通信板0应答 */
	COM_1_ACK = 0xF1,     /* 通信板1应答 */
	CORE_0_ACK = 0xF2,    /* 核心板0应答 */
	CORE_1_ACK = 0xF3,    /* 核心板1应答 */
	CORE_2_ACK = 0xF4,    /* 核心板2应答 */
	CORE_3_ACK = 0xF5,    /* 核心板3应答 */
	CONTROL_0_ACK = 0xF6, /* 控制盒0应答 */
	CONTROL_1_ACK = 0xF7, /* 控制盒1应答 */
	VOTE_ACK = 0xF8,      /* 表决板应答 */
	EVENT_SEND_ERROR = 0xFF,      /* 发送错误事件 */

	/* 通信板事件 */
	EVENT_COM_HEARTBEAT = 0x00, /* 通信板心跳 */
	EVENT_COM_REGISTER = 0x01, /* 通信板注册 */
	//EVENT_COM_HEARTBEAT = 0x02, /* 通信板心跳 */
	EVENT_LKJ_VERSION_DIFFERENCE = 0x03, /* LKJ与安全信息平台数据版本不一致 */
	EVENT_TRIP_STATUS_NOTICE = 0x04, /* 行程状态通知（基础线路数据接收中/基础线路数据接收完成）*/
	
	EVENT_DUTY_ASSISTING_CHANGE = 0x06, /* 本补状态变化 */
	//EVENT_ROUTE_VERSION_DIFFERENCE = 0x07, /* 交路数据版本不一致处理：交路提取与发送 */
	
	//EVENT_BASE_LINE_DATA_RECVING = 0x01, /* 行程状态-基础线路数据接收中 */
	//EVENT_BASE_LINE_DATA_RECV_COMPLETE = 0x02, /* 行程状态-基础线路数据接收完成 */
	
	//EVENT_ROUTE_RECV_COMPLETE = 0xD1, /* 交路提取完成 */
	EVENT_ROUTE_DATA_SEND = 0xD1, /* 交路数据版本不一致处理：交路提取完成并发送：坡度 */
	EVENT_ROUTE_DATA_CURVE= 0xD2, /* 交路数据版本不一致处理：发送：曲线 */
	EVENT_ROUTE_DATA_LIMIT = 0xD3, /* 交路数据版本不一致处理：发送：限速 */
	EVENT_ROUTE_DATA_TELESEME = 0xD4, /* 交路数据版本不一致处理：发送：信号机 */
	EVENT_ROUTE_DATA_TUNNEL = 0xD5, /* 交路数据版本不一致处理：发送：隧道 */
	EVENT_ROUTE_DATA_STATION= 0xD6, /* 交路数据版本不一致处理：发送：车站 */
	EVENT_FRONT_SPEED_LIMIT = 0xD7, /* 前方临时限速 */
	EVENT_FRONT_BRANCH_TRANSFER = 0xD8, /* 前方支线转移 */
	//EVENT_FRONT_ROUTE_TRANSFER = 0xD9, /* 前方交路转移 */
	EVENT_FRONT_SIDE_LINE_PASS = 0xDA, /* 前方侧线通过 */
	EVENT_MARSHALLING_RE_CHANGE = 0xDB, /* 通信板编组信息与揭示信息（再）变化 */
	//EVENT_MARSHALLING_INFO
	//EVENT_OPT_ROUTE_DATA
	EVENT_REAL_TIME_INFO = 0xDC, /* 实时信息包 */
	EVENT_ROUTE_VERSION_SEND = 0xDD, /* 通信板交路数据版本号提取与发送 */
	EVENT_TRIP_CHANGE = 0xDE, /* 行程信息变化 */
	EVENT_MARSHALLING_CHANGE = 0xDF, /* 揭示信息变化 */
	EVENT_CLOCK_SYNC = 0xE1, /* 时钟同步 */
	//EVENT_COM_INIT_FAILUER
	
	EVENT_REAL_TIME_ERROR = 0xA1, /* 实时信息解析错误 */
	EVENT_BASE_LINE_DATA_ERROR = 0xA2, /* 基础线路数据接收错误 */
	EVENT_ROUTE_EXTRACT_FAILUER = 0xA3, /*行程初始化交路提取失败 */
	EVENT_MARSHALLING_ERROR = 0xA4, /* 揭示信息接收错误 */
	EVENT_STATION_ERROR = 0xA5, /* 车站信息接收错误 */
	EVENT_COM_EXTERNAL_CAN_FAULT = 0xA6, /* 主通信板外CAN通信异常 */
	EVENT_COM_INTERNAL_CAN_FAULT = 0xA7, /* 主通信板内CAN通信异常 */
	
    EVENT_COM_MAX,
//} COM_EVENT_TYPE;

//typedef enum {
	/* 核心板事件 */
	EVENT_CORE_HEARTBEAT = 0x00, /* 核心板心跳 */
	EVENT_CORE_REGISTER = 0x01, /* 核心板注册 */
	//EVENT_CORE_HEARTBEAT = 0x02, /* 核心板心跳 */
	//EVENT_MASTER_CORE_EXCHANGE = 0x03, /* 核心板主机状态变化请求 */
	EVENT_ROUTE_VERSION_RESULT = 0x04, /* 核心板交路数据版本一致性校验结果发布 */
	EVENT_TRIP_OPT_COMPLETE = 0x05, /* 行程初始化优化计算完成 */
	
	//EVENT_UPDATE_OPT_MARSHALLING = 0xD1, /* 更新优化用揭示信息与编组信息 */
	EVENT_REAL_TIME_CTRL = 0xD1, /* 实时控制与状态数据 */
	//EVENT_CORE_INIT_FAILUER
	
	EVENT_CORE_CAN_FAULT = 0xA1, /* 核心板卡0/1/2/3CAN通信异常 */
	EVENT_TRIP_ROUTE_ERROR = 0xA2, /* 行程交路错误 */
	EVENT_TRIP_POSITION_ERROR = 0xA3, /* 行程定位错误 */
	EVENT_TRIP_PARA_ERROR = 0xA4, /* 行程参数错误 */
	EVENT_TRIP_OPT_ERROR = 0xA5, /* 行程初始化优化计算异常 */
	EVENT_TEMP_OPT_ERROR = 0xA6, /* 临时优化计算异常 */
	
    EVENT_CORE_MAX,
//} CORE_EVENT_TYPE;

//typedef enum {
	/* 控制盒事件 */
	EVENT_CTRL_BOX_HEARTBEAT = 0x00, /* 控制盒心跳 */
	EVENT_CTRL_BOX_REGISTER = 0x01, /* 控制盒注册 */
	//EVENT_CTRL_BOX_HEARTBEAT = 0x02, /* 控制盒心跳 */
	EVENT_CTRL_BOX_MASTER_SET = 0x03, /* 主交互单元设定 */
	EVENT_CTRL_BOX_MASTER_RELIEVE = 0x04, /* 主交互单元解除 */
	//EVENT_MARSHALLING_CONFIRM = 0x05, /* 编组与揭示信息确认 */
	EVENT_TRIP_INIT_START = 0x05, /* 控制盒行程初始化启动命令 */
	EVENT_MANNAL_TO_AUTO = 0x06, /* 手动转自动命令 */
	EVENT_AUTO_TO_MANNAL = 0x07, /* 自动转手动命令 */
	
	EVENT_TWO_GEAR_TO_MANNUL = 0x08, /* （高2档以上）确认转手动操作 */
	EVENT_SYS_TO_MAINTENANCE = 0x09, /* 进入测试维护模式（按4键） */
	EVENT_EXIT_MAINTENANCE = 0x0A, /* 退出测试维护模式（按3键） */
	EVENT_KEEP_AUTO = 0x0B, /* 保持自动状态（继电器常开端闭合）（按1键） */
	EVENT_KEEP_MANNAL = 0x0C, /* 保持手动状态（继电器常闭端闭合）（按2键） */
	EVENT_SEND_AUTO_GEAR = 0x0D, /* 发送自动测试档位：X */
	EVENT_GEAR8_AUTO_CONFIRM_ACK = 0x0E, /* （8档）确认转自动操作 */
	EVENT_TRIP_OVER = 0x0F, /* 发起行程结束 */
	
	EVENT_VOTE_HEARTBEAT_STOP = 0xA1, /* 表决板心跳异常 */
	EVENT_CTRL_BOX_CAN_FAULT = 0xA2, /* 控制盒0/1CAN通信异常 */
	EVENT_CTRL_BOX_PARSE_ERROR = 0xA3, /* 控制盒0/1数据解析异常 */
	
    EVENT_CTRL_MAX,
//} CTRL_EVENT_TYPE;

//typedef enum {
	/* 表决板事件 */
	EVENT_VOTE_HEARTBEAT = 0x00, /* 表决板心跳 */
	EVENT_BROADCAST_COM_ADDR = 0x02, /* 主通信板地址发布 */
	EVENT_MASTER_VOTE_EXCHANGE = 0x03, /* 系统主机状态变化发布 */
	EVENT_LOCK_SCREEN = 0x04, /* LKJ与安全信息平台数据版本不一致提示，锁屏 */
	EVENT_VOTE_TRIP_INIT_START = 0x05, /* 表决板行程初始化命令 */
	EVENT_SYS_TRIP_STATUS_SEND = 0x06, /* 系统行程状态发布 */
	//EVENT_VOTE_BASE_DATA_RECVING = 0x05, /* 行程状态交互-基础线路数据接收中 */
	//EVENT_VOTE_BASE_DATA_RECV_COMPLETE = 0x06, /* 行程状态交互-基础线路数据接收完成 */
	//EVENT_VOTE_BRANCH_TRANSFER_PASS = 0x07, /* 支线转移点通过 */
	//EVENT_VOTE_ROUTE_TRANSFER_PASS = 0x08, /* 交路转移点通过 */
	//EVENT_VOTE_SIDE_LINE_PASS = 0x09, /* 侧线终点通过 */
	EVENT_MANNAL_GEAR_CHANGE = 0x0A, /* 发送当前司控器手柄位：X */
	//EVENT_AIR_BRAKE_START = 0x0B, /* 空气制动启动 */
	//EVENT_SPEED_TOO_LOW = 0x0C, /* 速度低于20Km/h */
	//EVENT_AUTO_START = 0x0D, /* 转自动驾驶流程 */
	//EVENT_MANNAL_START = 0x0E, /* 转手动驾驶流程 */
	EVENT_TWO_GEAR_CONFIRM = 0x0E, /* 转手动高出2档位需控制盒确认 */
	EVENT_MANNAL_COMPLETE = 0x0F, /* 转手动驾驶完成 */
	EVENT_AUTO_COMPLETE = 0x10, /* 转自动驾驶完成 */
	EVENT_BRANCH_TRANSFER_TIME = 0x11, /* 支线转移距离过近计算时间不够提醒 */
	EVENT_SPEED_LIMIT_TIME = 0x12, /* 临时限速距离过近计算时间不够提醒 */
	EVENT_SIG_CHANGE_TIME = 0x13, /* 信号变化距离过近计算时间不够提醒 */
	EVENT_AUTO_TIME_ALARM = 0x14, /* 30s转换自动超时报警 */
	EVENT_MANNUL_TIME_ALARM = 0x15, /* 30s转换手动超时报警 */
	EVENT_BRAKE_GEAR0 = 0x16, /* 紧急制动提醒，拨动惰性档位 */
	EVENT_GEAR8_AUTO_CONFIRM = 0x17, /* 转自动手柄位是8档，需确认 */
	
	
	EVENT_VOTE_UPDATE_MARSHALLING = 0xD1, /* 表决板编组信息与揭示信息更新 */
	EVENT_VOTE_SENT_MARSHALLING = 0xD2, /* 发送编组信息与揭示信息至核心板 */
	//EVENT_SIG_LIGHT_CHANGE = 0xD3, /* 前方信号灯变化 */
	EVENT_DISP_AUTO = 0xD4, /* 转自动流程交互：30秒内扳至惰性档位 */
	EVENT_DISP_MANNAL = 0xD5, /* 转手动流程交互：30秒内扳至运行档位 */
	EVENT_DISP_REAL_TIME_INFO = 0xD6, /* 实时信息显示与自动控车档位 */
	EVENT_VOTE_TRIP_INIT_COMPLETE = 0xD7, /* 发布行程初始化完成状态，行程启动，发布行程编号*/
	EVENT_VOTE_BRANCH_TRANSFER_PASS = 0xD8, /* 前方支线转移 */
	//EVENT_VOTE_ROUTE_TRANSFER_PASS = 0xD9, /* 前方交路转移 */
	EVENT_VOTE_SIDE_LINE_PASS_START = 0xDA, /* 前方侧线通过起点 */
	EVENT_VOTE_SIDE_LINE_PASS_END = 0xDB, /* 前方侧线通过终点 */
	
	EVENT_TRIP_UPDATE = 0xDC, /* 表决板行程信息更新 */
	EVENT_SEND_TRIP_TO_CORE = 0xDD, /* 发送行程信息至核心板 */
	EVENT_MAR_UPDATE = 0xDE, /* 表决板揭示信息更新 */
	EVENT_SEND_MAR_TO_CORE = 0xDF, /* 发送揭示信息至核心板 */
	
	EVENT_BOARD_REGISTER_FAILUER = 0xA1, /* 某几块板启动注册失败 */
	EVENT_MASTER_COM_HEARTBEAT_STOP = 0xA2, /* 主通信板无心跳 */
	EVENT_ALL_COM_HEARTBEAT_STOP = 0xA3, /* 通信板0/1均无心跳 */
	EVENT_CORE_HEARTBEAT_STOP = 0xA4, /* 核心板(0&&1)||(2&&3)无心跳 */
	EVENT_MASTER_CTRL_HEARTBEAT_STOP = 0xA5, /* 主控制盒无心跳 */
	//EVENT_INPUT_HEARTBEAT_STOP = 0xA6, /* 输入板无心跳 */
	//EVENT_OUTPUT_HEARTBEAT_STOP = 0xA7, /* 输出板无心跳 */
	EVENT_RELAY_FAULT = 0xA8, /* 继电器工作状态异常 */
	EVENT_COM_FAULT_HANDLE = 0xA9, /* 通信板异常处理 */
	EVENT_CORE_FAULT_HANDLE = 0xAA, /* 核心板异常处理 */
	EVENT_CTRL_FAULT_HANDLE = 0xAB, /* 控制盒异常处理 */
	EVENT_VOTE_FAULT_HANDLE = 0xAC, /* 表决板程序异常 */
	//EVENT_CONFIRM_MARSHALLING_INFO
	
    EVENT_VOTE_MAX,
//} VOTE_EVENT_TYPE;

//typedef enum {
	/* 输入板事件 */
	EVENT_DIGITAL_ANALOG = 0x01, /* 数模信号变化 */
	
    EVENT_INPUT_MAX,
//} INPUT_EVENT_TYPE;


//typedef enum {
	/* 输出板事件 */
	EVENT_HEARTBEAT_STOP = 0x01, /* 表决板心跳异常 */
	
    EVENT_OUTPUT_MAX,
//} OUTPUT_EVENT_TYPE;
} EVENT_TYPE;


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/* 设置 获取 系统状态 */
	void set_sys_status(SYSTEM_STATUS status);
	SYSTEM_STATUS get_sys_status(void);
	
	/* 设置 获取 本补状态 */
	void set_train_status(TRAIN_STATUS status);
	TRAIN_STATUS get_train_status(void);
	
	/* 设置 获取 行程状态 */
	void set_trip_status(TRIP_STATUS status);
	TRIP_STATUS get_trip_status(void);
	
	/* 设置 获取 操控状态 */
	void set_control_status(CONTROL_STATUS status);
	CONTROL_STATUS get_control_status(void);
	
	/* 设置 获取 自动使能状态 */
	void set_auto_status(AUTO_STATUS status);
	AUTO_STATUS get_auto_status(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif 
