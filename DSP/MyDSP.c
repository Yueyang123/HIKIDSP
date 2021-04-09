#include "MyDSP.h"
#include "math.h"
#include "ADC.h"

long 	FFT_256Pointln[N];
long 	FFT_256PointOut[N/2];
float IBufMagArray[N/2];
float Fs = 44800;	//采样频率, Fs>2*max , FS/N<min 
float F = 44800/N;

void InitBuflnArray(int index)
{
	unsigned short i;
	float fx;
	Fs=ADC_GetFREQ();//获取目前采样频率
	F = Fs/N;
	for(i = 0; i < N; i++)
	{
		fx = ADC_SourceData[i][index];
		FFT_256Pointln[i] = ((signed short)fx) << 16;	//short为16位，则强转取低16位，再左移，所以：高16位有值，低16位为0
	}
}

void GetPowerMag(void)
{
	signed short IX, IY;
	float X, Y, Mag;
	unsigned short i;
	for(i = 0; i < N/2;i++)
	{
		IX = (FFT_256PointOut[i] << 16) >> 16; 	//取低16位，为实部
		IY = (FFT_256PointOut[i] >> 16);				//取高16位，为虚部
		X = N * ((float)IX) / 32768;
		Y = N * ((float)IY) / 32768;
		Mag = sqrt(X*X + Y*Y) / N;
		if(i == 0)
		{
			IBufMagArray[i] = (Mag * 32768);
		}
		else
		{
			IBufMagArray[i] = (Mag * 65536);
		}
	}
}


float GetMaxData(void)	//获得N中最大的值，不计算直流
{
	int i = 0;
	float temp = 0;
	for(i = 1; i < N/2; i++)
	{
		if(temp < IBufMagArray[i])
		{
			temp = IBufMagArray[i];
		}
	}
	return temp;
}

int GetMaxData_Index(void)	//获得N中最大的值，不计算直流
{
	int i = 0;
	float temp = 0;
	int tempindex=0;
	for(i = 1; i < N/2; i++)
	{
		if(temp < IBufMagArray[i])
		{
			temp = IBufMagArray[i];
			tempindex=i;
		}
	}
	return tempindex;
}


void DSP(void)	//DSP算法处理
{
	cr4_fft_256_stm32(FFT_256PointOut, FFT_256Pointln, N);
	GetPowerMag();	//数据处理和显示
}

int Get_Adc_Max_Min(int index)
{
	int i = 0;
	int max = 0,min=0;
	if(index>1)index=0;
	for(i = 0; i < SAMPLS_NUM; i++)
	{
		if(max < ADC_SourceData[i][index])
			max = ADC_SourceData[i][index];
		if(min > ADC_SourceData[i][index])
			min = ADC_SourceData[i][index];
	}
	return max-min;
}

float Get_Signal_Freq(int index)
{
	InitBuflnArray(index);
	DSP();
	return ((float)GetMaxData_Index()/256)*ADC_GetFREQ();
}