#ifndef ADC_H
#define ADC_H

#include "sys.h"
#define ADC_CHANNEL_NUMS 		2
#define SAMPLS_NUM        	1024

#define TIM3_DEFAULT 9
#define TIM2_PERIOD  999

extern u16 ADC_SourceData[SAMPLS_NUM][ADC_CHANNEL_NUMS];

//ADCCLK=12M
//ADC_SampleTime_28Cycles5 ת������28.5+12.5=41������
//ADC_SampleTime_1Cycles5  ת������1 .5+12.5=14������
//���ת��ʱ�䣺 14/12=1.17us
void Adc_Init(void);
void ADC_SetFreq(u32 FREQ);
u32 ADC_GetFREQ(void);
#endif