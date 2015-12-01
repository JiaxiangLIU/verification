#ifndef _LED_PACKET_H
#define _LED_PACKET_H

#define GPIOX_39 0
#define GPIOX_40 1
#define GPIOX_41 2
#define GPIOX_42 3
#define GPIOX_43 4
#define GPIOX_44 5
#define GPIOX_45 6
#define GPIOX_46 7
#define GPIOX_47 8
#define GPIOX_48 9
#define GPIOX_49 10
#define GPIOX_50 11

void led_packet_on(int led_num);

void led_packet_off(int led_num);
#endif

