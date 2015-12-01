/**
 * @file  dataDefinition.h
 * @brief   全局变量中会使用到的一些结构体的定义
 *  @date  2014.3.31
 * @author sinlly
 * @note  	定义整个项目中需要用的数据结构以及预留最大空间,其他文件均需引用该头文件,同时定义其他需要的参数常量
 */
#ifndef DATADEFINITION_H_
#define DATADEFINITION_H_

/**
 * 坡度
 */
struct gradient
{
	int start;   /**<  起始公里标 */
	int length;/**< 坡长*/
	float value; /**< 坡度值 */
};
typedef struct gradient GRADIENT;
/**
 *  限速
 */
struct limit
{
	int start;  /**< 起始公里标*/
	int end;  /**< 终止公里标*/
	int limit; /**< 限速值 */
};
typedef struct limit LIMIT;
/**
 * 信号机
 */
struct teleseme
{
	int post; /**< 公里标*/
	int tel_num;/**<信号机编号*/
};
typedef struct teleseme TELESEME;
/**
 * 隧道
 */
struct tunnel //隧道
{
	int start;  /**< 起始公里标*/
	int end; /**< 终止公里标*/
};
typedef struct tunnel TUNNEL;
/**
 *  曲线
 */
struct curve  //曲线
{
	int start; /**< 起始公里标*/
	int end; /**< 终止公里标*/
	int dir; /**< 方向*/
	int radius; /**< 曲率半径*/
};
typedef struct curve CURVE;


/**
 * 车站
 */
struct station //车站
{
	int id;  /**< 车站号*/
	int start; /**< 公里标*/
};
typedef struct station STATION;

/**
 *  机车参数
 */
struct locoParameter  //机车参数
{
	int count;	/**< 辆数 */
	float weight[2];  /**< 包括车头weight[0]为车头的重量,weight[1]为单节车厢平均重量 */
	float length[2]; /**< length[0]100为车头的质量，length[1]为单节车厢平均长度 */
	float totalLength; /**< 车总长度  include train head */
	float totalWeight;/**< 车总重量  include train head */
	float davis[3]; /**< 戴维斯系数 */
	float dragConsumption[8];/**< dGearE 牵引各档位耗油量 */
	float idleConsumption; /**< cGearE  惰性档位耗油量*/
	float brakeConsumption[8]; /**< bGearE 制动各档位耗油量 */
};
typedef struct locoParameter LOCOPARAMETER;
/**
 * 机车编组信息结构(每节车厢的长度和重量)
 */
struct carStruct
{
	float carLength; /**< 每节车厢的长度 */
	float carWeight;/**< 每节车厢的重量 */
	float loadWeight;/**< 载重 */
};
typedef struct carStruct CARSTRUCT;

/**
 *   加算坡度
 */
struct mGradient
{
	int start;  /**< 起始公里标*/
	int end; /**< 终止 公里标*/
	float value; /**< 坡度值 */
};
typedef struct mGradient MGRADIENT;

/**
 *  时刻表单元结构
 */
struct intervalRunTimeUnit
{
   int startCzh;  /**<   起始车站号*/
   int endCzh; /**<  终止车站号*/
   char startTime[10]; /**<  起始车站号时刻*/
   char arriveTime[10]; /**< 终止车站号时刻*/
   float averageV; /**< 车站间的平均速度 */
   float runTime; /**< 该段路程的标准时间*/
   float timeDiff; /**< 该段路程的附加时分*/
   float totalRuntime; /**< 该段路程的总运行时间(标准时间+附加时分) */
   int steepUpPect;  /**< 车站间陡上坡所占的比例 */
   int steepDownPect; /**< 车站间陡下坡所占的比例 */
};
typedef struct intervalRunTimeUnit INTERVALRUNTIMEUNIT;
/**
 * 坡度(段中的非该段类型的坡度)
 */
struct partGradient
{
	int start; /**< 起始公里标 */
	int end;  /**< 终止公里标 */
	float value; /**< 坡度值 */
	int flag; /**<  坡度类型 */
};
typedef struct partGradient PARTGRADIENT;
/**
 *  坡段
 */
struct roadCategory
{
	int start;  /**< 起始公里标 */
	int end; /**< 终止公里标*/
	float value; /**< 坡度值 */
	int length; /**< 坡长*/
	int flag; /**< 坡度类型 */
	float sugGear; /**< 推荐档位*/
	float vi;  /**< 坡段首速度*/
	float vo; /**< 坡段尾速度 */
	float avgV; /**< 段平均速度*/
	PARTGRADIENT*  partGradients; /**< 段中存在非该坡段类型的局部小坡段*/
	int partGradNum; /**<段中存在非该坡段类型的局部小坡段的个数*/
};
typedef struct roadCategory ROADCATEGORY;

///**
// * 原始优化策略匹配参数结构
// */
//struct strategyFlags
//{
//   int maxMass;  /**< 最大车重*/
//   int minMass; /**< 最小车重 */
//   int minLen; /**< 最小坡长*/
//   int maxLen; /**<最大坡长*/
//   float maxAvg; /**<最大平均速度*/
//   float minAvg; /**<最小平均速度*/
//   int partGradFlag; /**< 是否有局部非坡段类型的坡度*/
//   int minSteepUpPect; /**<最小抖上坡百分比*/
//   int maxSteepUpPect; /**<最大抖上坡百分比*/
//   int minSteepDownPect; /**< 最小抖下坡百分比*/
//   int maxSteepDownPect; /**< 最大抖下坡百分比*/
//   int gradientType; /**< 坡段类型*/
//   char  name[40];  /**<策略名称*/
//   int lastGraType;/**<上一段坡段类型，若没有该元素，则该元素值为100*/
//   int nextGraType;/**<下一段坡段类型,若没有该元素，则该元素值为100*/
//};
typedef struct strategyFlags RAWSTRFLAGS;
///**
// * 时间调整策略匹配参数结构
// */
//struct timeStrategyFlags
//{
//   int direction; /**< 前(左)或后(右)调整速度*/
//   int leftRType; /**< 前(左)坡度类型*/
//   int rightRType;/**<后(右)坡度类型*/
//   int singleR;/**< 是否处理单独的段*/
//   int up; /**< 升速或降速*/
//   char name[40]; /** 策略名称*/
//};
//typedef struct timeStrategyFlags TIMESTRFLAGS;
/**
 * 限速调整策略匹配参数结构
 */
//struct limitStrategyFLags
//{
//	int type;  /**< 限速类型标号*/
//    char  name[40];  /**<策略名称*/
//    int maxMass; /**< 最大车重*/
//    int minMass; /**< 最小车重*/
//};
typedef struct limitStrategyFLags LIMITSTRFLAGS;
/**
 *  优化结果
 */
struct optimizeResult
{
	float start;  /**<公里标*/
	int gear;  /**<档位*/
	float velocity; /**<速度*/
	float time; /**<时间*/
};
typedef struct optimizeResult OPTIMIZERESULT;

struct geData
{
	float start;
};
typedef struct geData GEDATA;

/**
 *  司机运行线路油耗数据信息
 */
struct initialData
{
	 float startV; /**<起始速度*/
	 float endV; /**< 终止速度*/
	 float consumption; /**< 油耗*/
	 float time; /**< 运行时间*/
	 float avgV; /**< 全局平均速度*/
	 float totalWeight; /**< 车总重 */
	 float carLengh; /**< 车长*/
	 int carNum; /**< 车厢节数*/
};
typedef struct initialData INITIALDATA;
/**
 * 优化结果标识
 */
enum opt_flag
{
     ADD_LOCO_AREA = 78,/**<补机区域*/       //!< PUSHER_ENGING_AREA
	 RUN_THROUGH_TEST = 79,/**<贯通实验*/            //!< RUN_THROUGH_TEST
	 AIR_DAMPING_BRAKE = 80 ,/**<空气制动降速部分*/      //!< AIR_DAMPING_BRAKE
	 AIR_DAMPING_AVG = 81, /**<空气制动匀速部分*/        //!< AIR_DAMPING_AVG
	 FORBID_STOP_ON_GRAD = 82,/**<禁止坡停区域标识*/     //!< FORBID_STOP_ON_GRAD
	 FORBID_STOP_CROSS_GRAD = 83, /**<禁止跨坡停车区域标识*///!< FORBID_STOP_CROSS_GRAD
	 NONE_FLAG  = 0  /**没有flag*/
};
typedef enum opt_flag OPTFLAG;

/**
 * 优化最终交给控制的结果
 */
struct opt_sch_curve
{
	int tel_num;/**<信号机编号*/
	float cross_dist;/**<越过距离*/
	float tel_kp;/**<信号机实际公里标*/
	float con_tel_kp;/**<连续的公里标*/
	float ford_time;/**<距离前方车站时间距离*/
	int ford_station_num; /**<前方车站号*/
	int gear;/**<优化档位*/
	float velocity;/**<速度*/
	float time;/**<行走的时间*/
	OPTFLAG anti_stop_flag;/**<用于判断坡度的标识符*/
	OPTFLAG manual_flag;/**<用于手动区域的标识符*/
};
typedef struct opt_sch_curve OPTSCHCURVE;
/**
 *  优化曲线的点单元结构
 */
struct optSch
{
	float start;   /**<公里标*/
	int gear;      /**<档位*/
	float velocity;  /**<速度 */
	float consumption;  /**<油耗 */
	float time;  /**<时间 */
	float acc;  /**< 加速度 */
};
typedef struct optSch OPTSCH;
/**
 * 优化中用到的常量值，从配置文件中读取
 */
struct optConstParam
{
	float G; /**<重力加速度 */
	float PRECISION; /**<判断浮点类型是否相等的精度*/
	float EMPTYINTERVAL;/**<空车陡上坡的用于调整平均速度的阈值 */
	int LIMIT_INTERVAL;/**<限速调整中，所用限速比正常限速低的阈值*/
	float DELTA_S;/**<距离步长*/
	float TSTEP;/**<计算时间步长*/
	float SSTEP;/**<计算空气制动的距离步长*/
	float MAXV;/**<标记机车及限速理论允许的最大恒速值*/
	float AIRLENGTH;/**<空气制动提前开启的距离*/
	float MAXLENGTH;/**<设定的追上曲线的最大距离*/
	float LIMITV;/**<设定的最大限速值*/
	float MINV;/**<标记自动驾驶允许的最低速度*/
    int MAXGEAR;/**<标记机车最大牵引档位*/
    int MINGEAR;/**<标记机车最小的制动档位*/
    int AIRS;/**<用于标记空气制动降速起始位置档位*/
    int AIRE;/**<用于标记空气制动降速结束匀速开始位置档位*/
	int DISTANCE;/**<坡段中划分出小的坡段的长度阈值*/
	int FEATUREINVALID;/**<策略配置文件中对应特征无效的标识*/
	int LIMITTHRESHOLD;/**<限速调整阈值，调整限速起始公里标的值*/
	int THROUGHEXPERT;/**<手动区域中贯通实验标识*/
	int PUSHERENGINE;/**<手动区域中非贯通实验即补机段标识*/
	float COMGEARSTEP;/**<档位渐变中普通档位换档的持续时间*/
	float IDELGEARSTEP;/**<档位渐变中惰行档位换档的持续时间*/
	float NEARLIMIT;/**<档位拉平过程中，判断是否距离距离限速较近的速度阈值*/
	int POSTINITLENGTH;/**<优化中拉平档位所新生成的曲线的初始长度（临时限速拉平所新生成的曲线）*/
	float GEARLENGTHLIMIT;/**<档位拉平中，需要进行档位拉平的最大的档位长度，若档位长度小于该值，则需要进行拉平*/
	float TMPSPEEDACCURACY;/**<临时限速中档位跳跃追上原始曲线的点的速度差阈值(连接起来的精度)*/
};
typedef struct optConstParam OPTCONSTPARAM;

struct airBrakeResult
{
	float start_mile;
	float start_velocity;
	float end_mile;
	float end_velocity;
	float real_distance;
	float cal_common_distance  ;//计算出来一般距离
	float cal_ergence_distance;//计算出来 紧急距离
	int flag;//2ok 1 ergence  ok  0  problem
};
typedef struct airBrakeResult AIRBRAKERESULT;

/**
 * 手动区域（补机、贯通实验区域存储结构）
 */
struct handArea
{
     float start;   /**<起始公里标（连续的）*/
     float ending;/**<终止公里标（连续的）*/
     float endV; /**<终止位置的速度，贯通实验通过配置得到，其余区域通过criticalVelocity方式得到该速度*/
};
typedef struct handArea HANDAREA;
/**
 * 手动区域非贯通实验存储结构，包括手动区域段及对应的手动区域的个数
 */
struct pusherEngine
{
	HANDAREA* handAreas; /** 手动区域段结构体数组（除去贯通实验）*/
	int len; /**<手动区域段的个数*/
};
typedef struct pusherEngine PUSHERENGINE;
/**
 * 外部传入的机车编组信息
 */
struct locoMarshalling
{
	float totalLength; /**< 车厢总长度 */
	float totalWeight;/**< 车厢总重量 */
	float totalNums;/**< 车厢节数 */
};
typedef struct locoMarshalling  LOCOMARSHALLING;
#endif /* DATADEFINITION_H_ */
