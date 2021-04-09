#include "LED.h"


LED_HANDLE led_handle[2]={
	{"LED0",(GPIO_TypeDef*)GPIOA_BASE,8,OFF},
	{"LED1",(GPIO_TypeDef*)GPIOD_BASE,2,OFF},
};


void LED_Init(void)
{
	int i=0;
	int temp;
	RCC->APB2ENR|=1<<2; 		//GPIOA
	RCC->APB2ENR|=1<<5; 		//GPIOD
	
	for(i=0;i<sizeof(led_handle)/sizeof(led_handle[0]);i++)
	{
		if(led_handle[i].gpio_pin<=7){
			(led_handle[i].gpio_group)->CRL&=~(0x3<<(4*led_handle[i].gpio_pin+2));
			(led_handle[i].gpio_group)->CRL|= (0x3<<(4*led_handle[i].gpio_pin));
		}
		else{
			temp=led_handle[i].gpio_pin-8;
			(led_handle[i].gpio_group)->CRH&=~(0x3<<(4*temp+2));
			(led_handle[i].gpio_group)->CRH|= (0x3<<(4*temp));
		}
	}
}

void LED_ON(LED_ENUM led_num)
{
	led_handle[led_num].gpio_group->ODR|=(1<<led_handle[led_num].gpio_pin);
	led_handle[led_num].status=ON;
	
}

void LED_OFF(LED_ENUM led_num)
{
	led_handle[led_num].gpio_group->ODR&=~(1<<led_handle[led_num].gpio_pin);
	led_handle[led_num].status=OFF;
}

void LED_TOGGLE(LED_ENUM led_num)
{
	if(led_handle[led_num].status==ON)
		LED_OFF(led_num);
	else
		LED_ON(led_num);
}