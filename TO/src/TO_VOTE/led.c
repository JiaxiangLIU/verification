#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "led.h"

#if defined(__arm__)
#if __GNUC_MINOR__ < 4
/* am3517 */

static int gpio_pin[] = {34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 54, 55};

#else
/* imx287 */
static int gpio_pin[] = { 40, 41, 42, 43, 44,  51};
//static int gpio_pin[] = {39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50};

#endif
#endif

enum LED_STATUS {
	LED_ON,
	LED_OFF
};

static int gpio_init(unsigned int gpio)
{
	FILE *fp;
	char gpath[128];

	if (gpio >= sizeof(gpio_pin)/sizeof(int)) {
		return -1;
	}

	if ((fp = fopen("/sys/class/gpio/export","w")) == NULL) {
		fprintf(stderr, "Open [/sys/class/gpio/export] error!\n");
		return -1;
	}
	fprintf(fp, "%d", gpio_pin[gpio]);
	fclose(fp);

	sprintf(gpath, "/sys/class/gpio/gpio%d/direction", gpio_pin[gpio]);

	if ((fp = fopen(gpath, "w")) == NULL) {
		fprintf(stderr, "Open [%s] error!\n", gpath);
		return -1;
	}
	fprintf(fp,"out");
	fclose(fp);

	return 0;
}

static int gpio_func(unsigned int gpio)
{
	FILE *fp;

	if (gpio >= sizeof(gpio_pin)/sizeof(int)) {
		return -1;
	}
	if ((fp = fopen("/sys/class/gpio/unexport","w")) == NULL) {
		fprintf(stderr, "Open [/sys/class/gpio/unexport] error!\n");
		return -1;
	}
	fprintf(fp, "%d", gpio_pin[gpio]);
	fclose(fp);

	return 0;
}

static int gpio_ctrl(unsigned int gpio, int value)
{
	FILE *fp;
	char gpath[128];

	if (gpio >= sizeof(gpio_pin)/sizeof(int)) {
		return -1;
	}
	sprintf(gpath, "/sys/class/gpio/gpio%d/value", gpio_pin[gpio]);
	if ((fp = fopen(gpath, "w")) == NULL) {
		fprintf(stderr, "Open [%s] error!\n", gpath);
		return -1;
	}
	fprintf(fp, "%d", value);
	fclose(fp);

	return 0;
}

int led_open(void)
{
	int i;

	for (i = 0; i < sizeof(gpio_pin)/sizeof(int); i++) {
		gpio_init(i);
	}
	return 0;
}


int led_close(void)
{
	int i;

	for (i = 0; i < sizeof(gpio_pin)/sizeof(int); i++) {
		gpio_func(i);
	}
	return 0;

}

int led_on(unsigned int led)
{
	return gpio_ctrl(led, LED_ON);
}

int led_off(unsigned int led)
{
	return gpio_ctrl(led, LED_OFF);
}

int led_count(void)
{
	return sizeof(gpio_pin)/sizeof(int);
}
