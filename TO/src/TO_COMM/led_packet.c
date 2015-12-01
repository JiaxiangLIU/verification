#include <stdio.h>
#include <unistd.h>
#include "led.h"
#include "led_packet.h"

void led_packet_on(int led_num){
	int count = 0;
	count = led_count();
	if(led_num >= count){
		printf("argument is out of scope\n");
	}
	led_on(led_num);
}

void led_packet_off(int led_num){
	int count = 0;
	count = led_count();
	if(led_num >= count){
		printf("argument is out of scope\n");
	}
	led_off(led_num);
}

void led_packet_init(void)
{
	led_open();
	led_packet_off(GPIOX_39);
	led_packet_off(GPIOX_40);
	led_packet_off(GPIOX_41);
	led_packet_off(GPIOX_42);
	led_packet_off(GPIOX_43);
	led_packet_off(GPIOX_44);
	led_packet_off(GPIOX_45);
	led_packet_off(GPIOX_46);
	led_packet_off(GPIOX_47);
	led_packet_off(GPIOX_48);
	led_packet_off(GPIOX_49);
	led_packet_off(GPIOX_50);
}

