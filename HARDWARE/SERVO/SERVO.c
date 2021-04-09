#include "SERVO.h"
#include "delay.h"
void TIM4_PWM_Init(u16 arr,u16 psc) //arr设定计数器自动重装值   
                                    //psc预分频器不分频,psc=0
{
	RCC->APB1ENR|=1<<2;       //TIM4时钟使能    
	GPIOB->CRH&=0XFFFFFF00;//PB8,9输出
	GPIOB->CRH|=0X000000BB;//复用功能输出 	  
	GPIOB->ODR|=3<<8;//PB8.9上拉	
	TIM4->ARR=arr;//设定计数器自动重装值 
	TIM4->PSC=psc;//预分频器不分频
	TIM4->CCMR2|=6<<4;  //CH3 PWM1模式	高电平有效	 
	TIM4->CCMR2|=1<<3; //CH3预装载使能
	TIM4->CCMR2|=6<<12;  //CH4 PWM1模式		 
	TIM4->CCMR2|=1<<11; //CH4预装载使能	   
	TIM4->CCER|=1<<8;   //OC1 输出使能
	TIM4->CCER|=1<<12;   //OC2 输出使能	   
	TIM4->CR1=0x0080;   //ARPE使能 
	TIM4->CR1|=0x01;    //使能定时器3 
}

void Servo_Init(void)
{
	TIM4_PWM_Init(20000,71);//50HZ
	TIM4->CCR3 =10000;
	TIM4->CCR4 =15000;
}
//舵机1驱动函数，控制端口TIM4_CH3（PB8）
void SERVO1(u8 degree)
{
	u16 k;
	k = 500 + degree * 11;
	TIM4->CCR3 = k;
}
//舵机2驱动函数，控制端口TIM4_CH4(PB9)
void SERVO2(u8 degree)
{
	u16 k;
	k = 500 + degree * 11;
	TIM4->CCR4 = k;
}

