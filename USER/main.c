#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "KEY.h"
#include "LCD_X.h"
#include "W25X16.h"
#include "malloc.h"
#include "SERVO.h"
#include "ADC.h"
#include "MyDSP.h"
u8 CHKEY=0; //0 CH0 1 CH1 2 同时

void DSO_Win()
{
	u8 data[3];
	int t=0;
	POINT_COLOR=GRAY;
	LCD_DrawFillRectangle(0,0,128,116);

	//CH1信息
	POINT_COLOR=BLUE;
	LCD_DrawFillRectangle(0,116,55,128);
	LCD_Show_Str(0,116,RED,BLUE,(u8*)"1HZ1V",12,1);
	//CH2信息
	POINT_COLOR=YELLOW;
	LCD_DrawFillRectangle(55,116,110,128);
	LCD_ShowString(55,116,12,(u8*)"1HZ1V",1);
	//采集频率
	POINT_COLOR=GREEN;
	LCD_DrawFillRectangle(110,116,128,128);
	t=ADC_GetFREQ();
	sprintf((char*)data,"%3.1fK",(float)t/1000);
	LCD_ShowString(110,116,12,(u8*)data,1);
}

void CH1_Update(u32 freq,float vos)
{
	u8 data[10]={0};
	POINT_COLOR=BLUE;
	LCD_DrawFillRectangle(0,116,55,128);
	sprintf((char*)data,"%dHZ%1.1fV",freq,vos);
	LCD_Show_Str(0,116,RED,BLUE,(u8*)data,12,1);
}

void CH2_Update(int freq,float vos)
{
	u8 data[10]={0};
	POINT_COLOR=YELLOW;
	LCD_DrawFillRectangle(55,116,110,128);
	sprintf((char*)data,"%dHZ%1.1fV",freq,vos);
	LCD_Show_Str(55,116,RED,BLUE,(u8*)data,12,1);
}

void FREQ_Update(int freq)
{
	u8 data[3]={0};
	POINT_COLOR=GREEN;
	LCD_DrawFillRectangle(110,116,128,128);
	sprintf((char*)data,"%3.1f",(float)freq/1000);
	LCD_Show_Str(110,116,RED,BLUE,(u8*)data,12,1);
}
 int main(void)
 {	
	int i;
	u8 key;
	u32 freq1,freq2;
	float vos1,vos2;
	u32 freq;
	u8 line0[128];
	u8 line1[128];
	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 
	uart_init(115200); 
	LED_Init();		  		//初始化与LED连接的硬件接口
	KEY_Init();
	LCD_Init();
	LCD_Clear(WHITE);
	SPI_Flash_Init();
	Adc_Init();
	ADC_SetFreq(100);
	Servo_Init();
	DSO_Win();

	while(1){
		 key=KEY_Scan(0);
		 if(key==KEY0_PRES){
				CHKEY++;
				if(CHKEY>=3)CHKEY=0;
		 }
		 if(key==KEY1_PRES){
				freq=ADC_GetFREQ();
			  freq*=2;//不能超过500K
			  if(freq>=500000)freq=100;
			  ADC_SetFreq(freq);
				FREQ_Update(freq);
		 }
			POINT_COLOR=GRAY;
			LCD_DrawFillRectangle(0,0,128,116);
		if(CHKEY==0){
			for(i=1;i<128;i++){
				line0[i]=116-ADC_SourceData[i][0]/37;
				LCD_DrawLine_Color(128-(i-1),line0[i-1],128-i,line0[i],BLUE);
			}
		}
		else if(CHKEY==1){
			for(i=1;i<128;i++){
				line1[i]=116-ADC_SourceData[i][1]/37;
				LCD_DrawLine_Color(128-(i-1),line1[i-1],128-i,line1[i],YELLOW);
			}		
		}
		else
		{
			for(i=1;i<128;i++){
				line0[i]=116-ADC_SourceData[i][0]/37;
				LCD_DrawLine_Color(128-(i-1),line0[i-1],128-i,line0[i],BLUE);
			}
			for(i=1;i<128;i++){
				line1[i]=116-ADC_SourceData[i][1]/37;
				LCD_DrawLine_Color(128-(i-1),line1[i-1],128-i,line1[i],YELLOW);
			}	
		}
		freq1=Get_Signal_Freq(0);
		freq2=Get_Signal_Freq(1);
		vos1=3.3*(float)Get_Adc_Max_Min(0)/4096;
		vos2=3.3*(float)Get_Adc_Max_Min(1)/4096;
		CH1_Update(freq1,vos1);
		CH2_Update(freq2,vos2);
		//printf("[FF FR]:%f,%d",Get_Signal_Freq(0),Get_Adc_Max_Min(0));
	}
}
