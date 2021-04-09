#ifndef LED_H
#define LED_H
#include "sys.h"

//目标：初始化三个LED灯以及一个BUZZ

typedef enum{LED0=0,LED1}LED_ENUM; 
typedef enum{OFF,ON}LED_STATUS;
typedef struct {
	char* 		 				led_name;
	GPIO_TypeDef*   	gpio_group;
	uint8_t	 	 				gpio_pin;
	LED_STATUS 			  status;
}LED_HANDLE;

void LED_Init(void);
void LED_ON(LED_ENUM led_num);
void LED_OFF(LED_ENUM led_num);
void LED_TOGGLE(LED_ENUM led_num);
#endif
