/**************************************************************************
*
* Copyright (C) 2015 MuHongWei <muhongwei@thit.com.cn>
*
*
*
*********************************************************************/
#ifndef LED_H
#define LED_H

#define NORMAL_LED 0
#define REGISTER_LED 1
#define REALTIME_LED 2
#define CRC_LED 3
#define VOTE_LED 4
#define COMMU_LED 5
#define OPT_LED 6

enum LED_STATUS {
	LED_ON,
	LED_OFF
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	/*********************************************
	描述：初始化gpio_pin定义的控制LED的IO。
	输入：无
	输出：无
	返回：0正常返回
	*********************************************/
	int led_open(void);
	
	/*********************************************
	描述：去初始化gpio_pin定义的控制LED的IO
	输入：无
	输出：无
	返回：0正常返回
	*********************************************/
	int led_close(void);
	
	/*********************************************
	描述：开启gpio_pin定义的控制LED的IO
	输入：gpio_pin数组的下标
	输出：无
	返回：0正常，正常返回
		  -1错误，错误返回
	*********************************************/
	int led_on(unsigned int led);
	
	/*********************************************
	描述：关闭gpio_pin定义的控制LED的IO
	输入：gpio_pin数组的下标
	输出：无
	返回：0正常，正常返回
		  -1错误，错误返回
	*********************************************/
	int led_off(unsigned int led);
	
	/*********************************************
	描述：读取gpio_pin数组定义IO的数量
	输入：无
	输出：无
	返回：返回gpio_pin数组成员个数
	*********************************************/
	int led_count(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
