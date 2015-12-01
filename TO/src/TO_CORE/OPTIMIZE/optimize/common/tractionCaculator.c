/**
 * @file    tractionCaculator.c
 * @brief  提供牵引计算模型的基本方法实现
 * @date  2014.4.1
 * @author sinlly
 * @note 包括多质点模型以及bdforce的相关计算
 */
#include "tractionCaculator.h"

LOCOPARAMETER locoInfo;
MGRADIENT* mGradients;
int MAX_MGRADIENT;
float m_loco; // train head weight
float l_loco; // train head length
float l_car; //average length for a car
float m_car; //average weight for a car

/**
 * @brief 根据机车的档位和速度获得机车的作用力
 * @param gear  驾驶档位
 * @param v   速度
 * @return  作用力
 */
TRAC_CAL_RESULT  getForce(int gear, float v,float *force) {
	TRAC_CAL_RESULT ret;
	ret.trac_result=TRAC_CAL_SUCCESS;
	int v1 = roundInt(v); //对v进行四舍五入
	if(v1<0)
	{
		trac_exception_decorator(VELOCITY_UNDER_ZERO,&ret,"#getForce");
		return ret;
	}
	if (v1 > 120) {
		trac_exception_decorator(VELOCITY_OVER_MAX,&ret,"#getForce");
		return ret;
	}

	if (gear < -8 || gear > 8) {
		trac_exception_decorator(INVALID_GEAR,&ret,"#getForce");
		return ret;
	}
	if (gear < 0) {
		 *force = -brakeForce[abs(gear) - 1][v1 - 1];
	} else {
		if (gear > 0)
			 *force= dragForce[gear - 1][v1 - 1];
		else
			*force= 0.0;
	}
	return ret;
}
/**
 * @brief 根据档位和时间获得油耗
 * @param gear  档位
 * @param tStep  时间
 * @return   油耗
 */
float getConsumption(int gear, float tStep) {
	float delta_e = 0;
	if (gear > 0)
		delta_e = tStep * locoInfo.dragConsumption[gear - 1];
	else {
		if (gear == 0)
			delta_e = tStep * locoInfo.idleConsumption;
		else
			delta_e = tStep * locoInfo.brakeConsumption[-gear - 1];
	}
	return delta_e;
}
/**
 * @brief 多指点模型计算坡度
 * @param start  当前公里标
 * @param count  roadCategory的index,只是为了快速索引，从0开始也可以
 * @return  多质点计算之后的坡度
 */
float multiParticalModel(float start, int *count) {
//	m_loco = locoInfo.weight[0]; // train head weight
//	l_loco = locoInfo.length[0]; // train head length
	m_loco = 0.0; // train head weight
	l_loco =0.0; // train head length
//	l_car = (locoInfo.totalLength - l_loco) / locoInfo.count; //average length for a car
//	m_car = (locoInfo.totalWeight - m_loco) / locoInfo.count; //average weight for a car
	float w_loco = 0;
	float x_loco = start - l_loco / 2.0;
	float grad = 0.0;
	if (x_loco < (float)(mGradients[*count].start)) {   //前行方向为前方，反向为后方
		if (*count == 0) //首段后方
			grad = 0.0;
		else {
			while (1) {
				*count = *count - 1; //后移至相应段
				if (x_loco > (float)mGradients[*count].start || fabs(x_loco-(float)mGradients[*count].start)<opt_const.PRECISION) {
					grad = mGradients[*count].value;
					break;
				} else {
					if (*count == 0) {
						grad = 0.0;
						break;
					}
				}
			}
		}
	} else {
		if (x_loco >(float) mGradients[*count].end) {
			if (*count == MAX_MGRADIENT - 1) //末端前方
				grad = 0.0;
			else {
				while (1) {
					*count = *count + 1;
					if (x_loco <  (float)mGradients[*count].end || fabs(x_loco-(float)mGradients[*count].end)<opt_const.PRECISION) {
						grad = mGradients[*count].value;
						break;
					} else {
						if (*count == MAX_MGRADIENT - 1) {
							grad = 0.0;
							break;
						}
					}
				}
			}
		} else
			grad = mGradients[*count].value;
	}
	w_loco = w_loco + m_loco * grad;
	int count_car = *count;
	l_car=carStruct[0].carLength;
	float x_car = x_loco - l_loco / 2.0 - l_car / 2.0;
	float w_cars = 0.0;
	float temp_start = (float)mGradients[count_car].start;
	float temp_end =(float) mGradients[count_car].end;
	int i = 0;
	for (i = 0; i < locoInfo.count; i++) {    //第n个car坡度阻力
		if ((x_car > temp_start || fabs(x_car-temp_start) <opt_const.PRECISION ) && (x_car <  temp_end || fabs(x_car-temp_end)<opt_const.PRECISION))
			grad = mGradients[count_car].value;
		else {
			if (x_car < temp_start)
			//前行方向为前方，反向为后方
					{
				if (count_car == 0) //首段后方
					grad = 0.0;
				else {
					while (1) {
						count_car = count_car - 1; //后段移至相应段
						if (x_car > (float) mGradients[count_car].start || fabs(x_car - (float) mGradients[count_car].start )< opt_const.PRECISION) {
							grad = mGradients[count_car].value;
							break;
						} else {
							if (count_car == 1) {
								grad = 0.0;
								break;
							}
						}
					}
					temp_start = (float)mGradients[count_car].start;
					temp_end = (float)mGradients[count_car].end;
				}
			} else {
				if (x_car > (float)mGradients[count_car].end) {
					if (count_car == MAX_MGRADIENT - 1) //末端前方
						grad = 0.0;
					else {
						while (1) {
							count_car = count_car + 1;
							if (x_car <  (float)mGradients[count_car].end || fabs(x_car - (float)mGradients[count_car].end)< opt_const.PRECISION) {
								grad = mGradients[count_car].value;
								break;
							} else {
								if (count_car == MAX_MGRADIENT - 1) {
									grad = 0.0;
									break;
								}
							}
						}
						temp_start = (float)mGradients[count_car].start;
						temp_end = (float)mGradients[count_car].end;
					}
				}
			}
		}
        m_car=carStruct[i].carWeight+carStruct[i].loadWeight;
		float w_car = m_car * grad;
		w_cars = w_cars + w_car;
		if(i+1!=CARNUMS)
	        	l_car=carStruct[i+1].carLength;
		x_car = x_car - l_car;
	}
	return (w_loco + w_cars) / (locoInfo.totalWeight);
}

/**
 * @brief  对单位时间行驶的距离，油耗，速度变化量计算
 * @param start  起始位置
 * @param v   当前速度
 * @param gear  当前档位
 * @param tStep   单位时间
 * @param count   roadCategory的index,只是为了快速索引，从0开始也可以
 * @param delta_s  单位时间所行驶的距离
 * @param delta_v  单位时间速度变化量
 * @param delta_e  单位时间消耗油耗
 * @return  成功标识为1,非成功标识0
 */
TRAC_CAL_RESULT  DoCaculateByTime(float start, float v, int gear, float tStep, int *count,
		float *delta_s, float *delta_v, float *delta_e) {
	//优化后的参数
	TRAC_CAL_RESULT  ret;
	ret.trac_result=TRAC_CAL_SUCCESS;
	float w_o = locoInfo.davis[0] * v * v + locoInfo.davis[1] * v
			+ locoInfo.davis[2];
	float gradient = multiParticalModel(start, count);
	float w = w_o + gradient; //总阻力
	float df;
	ret=getForce(gear, v,&df);
	if(ret.trac_result!=TRAC_CAL_SUCCESS)
	{
		char desc[EXP_DESC_LENGTH]="#DoCalByTime";
		strcat(desc,ret.description);
		trac_exception_decorator(ret.trac_result,&ret,desc	);
		return ret;
	}
//	float dft = 1000/G*df/(m_loco + locoInfo.totalWeight); //转换后的最大牵引力
	float dft = 1000.0 / opt_const.G * df / locoInfo.totalWeight; //转换后的最大牵引力
	float c = dft - w;
	float acc = c / 30.0;
	*delta_v = acc * tStep;
	*delta_s = (v / 3.6) * tStep;
	*delta_e = getConsumption(gear, tStep);
	return ret;
}
/**
 * @brief 单位距离行驶的时间，油耗，速度变化量计算
 * @param start   起始位置
 * @param v   当前速度
 * @param gear  当前档位
 * @param tStep   单位距离行驶的时间
 * @param count   roadCategory的index,只是为了快速索引，从0开始也可以
 * @param delta_s  单位距离
 * @param delta_v  单位时间速度变化量
 * @param delta_e  单位时间消耗油耗
 * @return  成功标识为1,非成功标识为0
 */
TRAC_CAL_RESULT  DoCaculateByDistance(float start, float v, int gear, float *delta_t,
		int *count, float delta_s, float *delta_v, float *delta_e) //单位距离行驶时间的计算
{
	TRAC_CAL_RESULT ret;
	ret.trac_result=TRAC_CAL_SUCCESS;
	float w_o = locoInfo.davis[0] * v * v + locoInfo.davis[1] * v
			+ locoInfo.davis[2];
	float gradient = multiParticalModel(start, count);
	float w = w_o + gradient; //总阻力
	float df;
	ret=getForce(gear, v,&df);
	if(ret.trac_result!=TRAC_CAL_SUCCESS)
	{
		char desc[80]="#DoCalByDist";
		strcat(desc,ret.description);
		trac_exception_decorator(ret.trac_result,&ret,desc);
		return ret;
	}
//	float dft = 1000/G*df/(m_loco + locoInfo.totalWeight); //转换后最大单位牵引力
	float dft = 1000.0 / opt_const.G * df / locoInfo.totalWeight; //转换后最大单位牵引力
	float c = dft - w;
	float acc = c / 30.0;
	*delta_t = delta_s / (v / 3.6);
	*delta_v = acc * (*delta_t);
	*delta_e = getConsumption(gear, *delta_t);
	return ret;
}
TRAC_CAL_RESULT airBrakeDistance(float start,float end,float v_in,float v_out,float*real_distance,float* cal_common_distance,float *cal_ergence_distance,int* flag){
	TRAC_CAL_RESULT ret;
	ret.trac_result=TRAC_CAL_SUCCESS;
	float v=0;
	v=(v_in+v_out)/2;
	float w_o =0;
	w_o=locoInfo.davis[0] * v * v + locoInfo.davis[1] * v+ locoInfo.davis[2];//计算基本阻力,用平均速度
	float r=0;
	r=0.322*(v+150)/(2*v+150); //计算 换算摩擦系数
	int c=1;
	int *n=&c;
	int milestart=0;
	milestart=start;
	int mileend=0;
	mileend=end;
	int a=0;
	a=mileend-milestart+1;
	float i_gradient=0;
	int i=0;
	for( i=milestart; i<=mileend; i++)
		i_gradient=i_gradient+multiParticalModel((float)i,n);
	float i_mgradient=0;
	i_mgradient=i_gradient/a;// 计算出相应路段的平均坡度
	float t=0;
	float ergence_t=0;
	if(i_mgradient>0){
		t=3.6+0.00176*120*locoInfo.count;
		ergence_t=1.6+0.065*locoInfo.count;
	}
	else{
		t=(3.6+0.00176*120*locoInfo.count)*(1-0.032*i_mgradient);//计算 空走时间
		ergence_t=(1.6+0.065*locoInfo.count)*(1-0.028*i_mgradient);
	}
	float s=0;
	float ergence_s=0;
	s=v_in*t/3.6+4.17*(v_in*v_in-v_out*v_out)/(1000*r*0.18*0.8+w_o+i_mgradient);
	ergence_s=v_in*ergence_t/3.6+4.17*(v_in*v_in-v_out*v_out)/(1000*r*0.18*0.8+w_o+i_mgradient);
	*real_distance=end-start;
	*cal_common_distance=s;
	*cal_ergence_distance=ergence_s;
	if(*real_distance<*cal_ergence_distance)
		*flag= 0;
	else if(*real_distance<*cal_common_distance)
		*flag= 1;
	else
		*flag= 2;
	if(ret.trac_result!=TRAC_CAL_SUCCESS)
		{
			char desc[EXP_DESC_LENGTH]="#airBrakeDistance";
			strcat(desc,ret.description);
			trac_exception_decorator(ret.trac_result,&ret,desc	);
			return ret;
		}
	return ret;
}
void airTest1(OPTSCH* optSch,int optSch_count,char* file){
	//lizengkun added test
	//printf("test start\n");
		AIRBRAKERESULT*air_result=NULL;
		int mark=0;
		int i=0;
		int count= optSch_count;
		int flag=0;
		while(flag<count){
			//printf("data select %d\n",lzk_i);
			   air_result=(AIRBRAKERESULT*)realloc(air_result,sizeof(AIRBRAKERESULT)*(i+1));
			   if(air_result==NULL){printf(" data space allowance wrong \n");}
				while(optSch[flag].gear!=-10&&flag<count){
					flag++;
					//printf("%d start %d\n",lzk_i,lzk_flag);
				}
				if(flag<count){
					air_result[i].start_mile=optSch[flag-1].start;
					air_result[i].start_velocity=optSch[flag-1].velocity;
					mark=1;
				}
				while(optSch[flag].gear==-10&&flag<count){
					flag++;
					//printf("%d end %d\n",lzk_i,lzk_flag);
				}
				if(flag<count||(flag==count&&mark==1)){
					air_result[i].end_mile=optSch[flag-1].start;
					air_result[i].end_velocity=optSch[flag-1].velocity;
					//printf("%f %f %f %f \n",air_result[lzk_i].start_mile,air_result[lzk_i].end_mile,air_result[lzk_i].start_velocity,air_result[lzk_i].end_velocity);
					airBrakeDistance(air_result[i].start_mile,air_result[i].end_mile,air_result[i].start_velocity,air_result[i].end_velocity,&(air_result[i].real_distance),&(air_result[i].cal_common_distance),&(air_result[i].cal_ergence_distance),&(air_result[i].flag));
					i++;
					mark=0;
				}
	}
	if(i>0){
		    printf("共处理%d段\n",i);
			int j=0;
			FILE* fid = fopen(file, "w");
			if (fid == NULL) {
				fprintf(stderr, "open %s err!\n",file);
			}
	        fprintf(fid,"起始公里表          起始速度       终止公里表           终止速度      实际距离       计算普通距离 计算紧急距离 标记\n");
			for (j = 0; j < i; j++) {
				fprintf(fid, "%f\t%f\t%f\t%f%15.6f%15.6f%15.6f\t%2d\n", air_result[j].start_mile,air_result[j].start_velocity,air_result[j].end_mile,air_result[j].end_velocity,air_result[j].real_distance,air_result[j].cal_common_distance,air_result[j].cal_ergence_distance,air_result[j].flag);
			}
			free(air_result);
			fclose(fid);
	}
	else
		printf("no data test\n");
}
