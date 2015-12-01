#ifndef LED_H
#define LED_H
/*
 * open led device
 */
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

#endif
