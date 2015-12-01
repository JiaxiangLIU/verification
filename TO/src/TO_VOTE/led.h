#ifndef LED_H
#define LED_H
#define led_card_nomal_1  0
#define led_card_nomal_2   1
#define led_sys_nomal_1    2
#define led_sys_maintain  3

#define led_sys_wait      4
#define led_sys_incan     5
/*
 * open led device
 */

/*
 *
 * fpga led*/
#define led_outcan 1
#define led_auto 2

#define led_throttle 9
#define led_control_0 10
#define led_comm_0 7
#define led_comm_1 8
#define led_core_0 11
#define led_core_1 12
#define led_core_2 5
#define led_core_3 6

#define led_inboard 3
#define led_outboard 4
char get_core_led(int i);
char get_comm_led(int i);
char get_control_led(int i);
int led_open(void);

/*
 * close led device
 */
int led_close(void);

/*
 * turn on led
 */
int led_on(unsigned int led);

/*
 * turn off led
 */
int led_off(unsigned int led);

/*
 * get led count
 */
int led_count(void);

int VoteLedCtrl(char LED_NUM , char LED_OP);
#endif
