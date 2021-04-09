#include "sys.h"
#include "usart.h"
#include "ADC.h"

u16 ADC_SourceData[SAMPLS_NUM][ADC_CHANNEL_NUMS] = {0};
u32 ADC_FREQ=100000;

void ADC_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_TIM2_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_TIM2_Configuration(void)
{ 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	ADC_TIM2_GPIO_Configuration();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_DeInit(TIM2);
	TIM_TimeBaseStructure.TIM_Period = TIM2_PERIOD - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;		
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, & TIM_TimeBaseStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = (TIM2_PERIOD - 1) / 2;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;		
	TIM_OC3Init(TIM2, & TIM_OCInitStructure);
}

void ADC_TIM3_Configuration(u16 TIM3_PERIOD)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure; 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimeBaseInitStructure.TIM_Period = TIM3_PERIOD - 1;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;		
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);			
	TIM_Cmd(TIM3,ENABLE);									
}

void ADC_DMA_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel	= DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority		 = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	DMA_ClearITPendingBit(DMA1_IT_TC1);
	DMA_ITConfig(DMA1_Channel1,DMA1_IT_TC1,ENABLE);
}

void ADC_DMA_Configuration(void)
{
	DMA_InitTypeDef  DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)ADC_SourceData;
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize         = ADC_CHANNEL_NUMS*SAMPLS_NUM;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);	
	ADC_DMA_NVIC_Configuration();
}
//ADCCLK=12M
//ADC_SampleTime_28Cycles5 转换周期28.5+12.5=41个周期
//ADC_SampleTime_1Cycles5  转换周期1 .5+12.5=14个周期
//最短转换时间： 14/12=1.17us
//采样率最高0.85M
void ADC_Init_Configuration(void)
{
	ADC_InitTypeDef  ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//AFCCLK:72/6=12MHz
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode       = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel       = ADC_CHANNEL_NUMS;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0,  1, ADC_SampleTime_1Cycles5);	//AI_VS_A1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1,  2, ADC_SampleTime_1Cycles5);	//AI_VS_B1

	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);
}




void Adc_Init(void)
{
	ADC_GPIO_Configuration();
	ADC_TIM3_Configuration(TIM3_DEFAULT);
	ADC_DMA_Configuration();
	ADC_Init_Configuration();
}
u32 ADC_GetFREQ(void)
{
	return ADC_FREQ;
}
//TIM3定时器1M
//最高触发 PERIOD=0 				采样率=1M
//				 PERIOD=1 				采样率=0.5M
//				 PERIOD=3 				采样率=0.25M		250K
//				 PERIOD=3 				采样率=0.1M			100K
//				 PERIOD=999 		  采样率=1K
//				 PERIOD=60000    	采样率=16
void ADC_SetFreq(u32 FREQ)
{
	u16 PERIOD=((float)1000000/FREQ)-1;
	TIM3->ARR=PERIOD;
	ADC_FREQ=FREQ;
}

void DMA1_Channel1_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
	{
		DMA_ClearITPendingBit(DMA1_IT_TC1);
	}
}
