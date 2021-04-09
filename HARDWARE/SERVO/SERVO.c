#include "SERVO.h"
#include "delay.h"
void TIM4_PWM_Init(u16 arr,u16 psc) //arr�趨�������Զ���װֵ   
                                    //pscԤ��Ƶ������Ƶ,psc=0
{
	RCC->APB1ENR|=1<<2;       //TIM4ʱ��ʹ��    
	GPIOB->CRH&=0XFFFFFF00;//PB8,9���
	GPIOB->CRH|=0X000000BB;//���ù������ 	  
	GPIOB->ODR|=3<<8;//PB8.9����	
	TIM4->ARR=arr;//�趨�������Զ���װֵ 
	TIM4->PSC=psc;//Ԥ��Ƶ������Ƶ
	TIM4->CCMR2|=6<<4;  //CH3 PWM1ģʽ	�ߵ�ƽ��Ч	 
	TIM4->CCMR2|=1<<3; //CH3Ԥװ��ʹ��
	TIM4->CCMR2|=6<<12;  //CH4 PWM1ģʽ		 
	TIM4->CCMR2|=1<<11; //CH4Ԥװ��ʹ��	   
	TIM4->CCER|=1<<8;   //OC1 ���ʹ��
	TIM4->CCER|=1<<12;   //OC2 ���ʹ��	   
	TIM4->CR1=0x0080;   //ARPEʹ�� 
	TIM4->CR1|=0x01;    //ʹ�ܶ�ʱ��3 
}

void Servo_Init(void)
{
	TIM4_PWM_Init(20000,71);//50HZ
	TIM4->CCR3 =10000;
	TIM4->CCR4 =15000;
}
//���1�������������ƶ˿�TIM4_CH3��PB8��
void SERVO1(u8 degree)
{
	u16 k;
	k = 500 + degree * 11;
	TIM4->CCR3 = k;
}
//���2�������������ƶ˿�TIM4_CH4(PB9)
void SERVO2(u8 degree)
{
	u16 k;
	k = 500 + degree * 11;
	TIM4->CCR4 = k;
}

