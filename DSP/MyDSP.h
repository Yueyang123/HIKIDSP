#ifndef __MYDSP_H
#define __MYDSP_H

#include "stm32f10x.h"
#include "stm32_dsp.h"
#include "math.h"
#include "sys.h"
#include "usart.h"
#include "delay.h"



#define N 256
#define PI2 6.28318530717959

extern long  FFT_256Pointln[N];
extern long  FFT_256PointOut[N/2];
extern float IBufMagArray[N/2];
extern float Fs;
extern float F;

void 	InitBuflnArray(int index);
void  GetPowerMag(void);
float GetMaxData(void);	//获得N中最大的值，不计算直流
void  DSP(void);	//DSP算法处理
float Get_Signal_Freq(int index);
int Get_Adc_Max_Min(int index);
#endif

