#ifndef ADC_H
#define ADC_H

#include "sys.h"
#define ADC_CHANNEL_NUMS 		2
#define SAMPLS_NUM        	1024

#define TIM3_DEFAULT 9
#define TIM2_PERIOD  999

extern u16 ADC_SourceData[SAMPLS_NUM][ADC_CHANNEL_NUMS];

//ADCCLK=12M
//ADC_SampleTime_28Cycles5 转换周期28.5+12.5=41个周期
//ADC_SampleTime_1Cycles5  转换周期1 .5+12.5=14个周期
//最短转换时间： 14/12=1.17us
void Adc_Init(void);
void ADC_SetFreq(u32 FREQ);
u32 ADC_GetFREQ(void);
#endif