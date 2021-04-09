#include "24L01.h"
#include "delay.h"
//Mini STM32开发板
//NRF24L01 驱动函数 

// SPI总线速度设置 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

const unsigned char TX_ADDRESS[TX_ADR_WIDTH]={0x22,0x22,0x33,0x44,0x55}; //发送地址
const unsigned char RX_ADDRESS[RX_ADR_WIDTH]={0x22,0x22,0x33,0x44,0x55};  //发送地址
							    
//初始化24L01的IO口
void NRF24L01_Init(void)
	{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE );	
	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;          //初始化CE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	GPIO_SetBits(GPIOA,GPIO_Pin_4);	 


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           //初始化CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	Set_NRF24L01_CSN; 
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;          //初始化IRQ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	SPI1_Init();    //初始化SPI1
	
	Clr_NRF24L01_CE; 	//使能24L01
	
	Set_NRF24L01_CSN;	//SPI片选取消	


	}
	
	
	
	//SPI 速度设置函数
//SpeedSet:
//SPI_SPEED_2   2分频   (SPI 36M@sys 72M)
//SPI_SPEED_8   8分频   (SPI 9M@sys 72M)
//SPI_SPEED_16  16分频  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256分频 (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
	{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI_SPEED_2)//二分频
		{
		SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
		}
	else if(SpeedSet==SPI_SPEED_8)//八分频 
		{
		SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
		}
	else if(SpeedSet==SPI_SPEED_16)//十六分频
		{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
		}
	else			 	 //256分频
		{
		SPI1->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz 低速模式
		}
	//SPI1->CR1|=1<<6; //SPI设备使能	
	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设  
	}
	
	
	
	//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
unsigned char SPIx_ReadWriteByte(unsigned char TxData)
	{		
		unsigned char retry=0;				 
		/* Loop while DR register in not emplty */
		
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
			{
				retry++;
				if(retry>200)return 0;
			}			  
		/* Send byte through the SPI1 peripheral */
		SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
			
		retry=0;
			
		/* Wait to receive a byte */
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
			{
				retry++;
				if(retry>200)return 0;
			}	  						    
		/* Return the byte read from the SPI bus */
			
		return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
			
	}
	
	
	
	
	
	

//检测24L01是否存在
//返回值:0，成功;1，失败	
unsigned char NRF24L01_Check(void)
	{
		unsigned char buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
		unsigned char i;
		SPIx_SetSpeed(SPI_SPEED_16); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz） 
		NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.	
		NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址  
		for(i=0;i<5;i++)
		{
		  if(buf[i]!=0XA5)break;	 
		}
		if(i!=5)return 1;//检测24L01错误	
		return 0;		 //检测到24L01
	}	
 	 
//SPI写寄存器
//reg:指定寄存器地址
//value:写入的值
unsigned char NRF24L01_Write_Reg(unsigned char reg,unsigned char value)
	{
		unsigned char status;	
		Clr_NRF24L01_CSN;                 //使能SPI传输
		status =SPIx_ReadWriteByte(reg);//发送寄存器号 
		SPIx_ReadWriteByte(value);      //写入寄存器的值
		Set_NRF24L01_CSN;                 //禁止SPI传输	 
		return(status);       			//返回状态值
	}

//读取SPI寄存器值
//reg:要读的寄存器
unsigned char NRF24L01_Read_Reg(unsigned char reg)
	{
		unsigned char reg_val;	
		Clr_NRF24L01_CSN;          //使能SPI传输		
		SPIx_ReadWriteByte(reg);   //发送寄存器号
		reg_val=SPIx_ReadWriteByte(0XFF);//读取寄存器内容
		Set_NRF24L01_CSN;          //禁止SPI传输		    
		return(reg_val);           //返回状态值
	}	

//在指定位置读出指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值 
unsigned char NRF24L01_Read_Buf(unsigned char reg,unsigned char *pBuf,unsigned char len)
	{
		unsigned char status,NUM;	  
		
		Clr_NRF24L01_CSN;           //使能SPI传输
		
		status=SPIx_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值  
		
		for( NUM=0;NUM<len;NUM++)pBuf[NUM]=SPIx_ReadWriteByte(0XFF);//读出数据
		
		Set_NRF24L01_CSN;       //关闭SPI传输
		
		return status;        //返回读到的状态值
		
	}

//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
//返回值,此次读到的状态寄存器值
unsigned char NRF24L01_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char len)
	{
		
		unsigned char status,NUM;	 
			
		Clr_NRF24L01_CSN;          //使能SPI传输
			
		status = SPIx_ReadWriteByte(reg);//发送寄存器值(位置),并读取状态值
			
		for(NUM=0; NUM<len; NUM++)SPIx_ReadWriteByte(*pBuf++); //写入数据	
			
		Set_NRF24L01_CSN;       //关闭SPI传输
			
		return status;          //返回读到的状态值
		
	}
				   
//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:发送完成状况
unsigned char NRF24L01_TxPacket(unsigned char *txbuf)
	{
	unsigned char sta;
	SPIx_SetSpeed(SPI_SPEED_16);//spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）   
	Clr_NRF24L01_CE;
		
	NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//写数据到TX BUF  32个字节
		
	Set_NRF24L01_CE;//启动发送	
		
	while(NRF24L01_IRQ!=0);//等待发送完成
		
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值	  
		
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
		
	if(sta&MAX_TX)//达到最大重发次数
		{
			
		NRF24L01_Write_Reg(NRF24L01_FLUSH_TX,0xff);//清除TX FIFO寄存器 
			
		  return MAX_TX; 
			
		}
	if(sta&TX_OK)//发送完成
		{
			
		  return TX_OK;
			
		}
	return 0xff;//其他原因发送失败
	}

//启动NRF24L01发送一次数据
//txbuf:待发送数据首地址
//返回值:0，接收完成；其他，错误代码
unsigned char NRF24L01_RxPacket(unsigned char *rxbuf)
	{
	unsigned char sta;	
		
	SPIx_SetSpeed(SPI_SPEED_16); //spi速度为9Mhz（24L01的最大SPI时钟为10Mhz）  
		
	sta=NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值    	 
		
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta); //清除TX_DS或MAX_RT中断标志
		
	if(sta&RX_OK)//接收到数据
		{
		  NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//读取数据
			
		  NRF24L01_Write_Reg(NRF24L01_FLUSH_RX,0xff);//清除RX FIFO寄存器 
			
		  return 0; 
		}	
		
	return 1;//没收到任何数据
		
	}	
					    
//该函数初始化NRF24L01到RX模式
//设置RX地址,写RX数据宽度,选择RF频道,波特率和LNA HCURR
//当CE变高后,即进入RX模式,并可以接收数据了		   
void RX_Mode(void)
	{
	Clr_NRF24L01_CE;	  
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(unsigned char*)RX_ADDRESS,RX_ADR_WIDTH);//写RX节点地址
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);    //使能通道0的自动应答    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01);//使能通道0的接收地址  	 
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,0);	     //设置RF通信频率		  
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//选择通道0的有效数据宽度 	    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x0f);//设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG, 0x0f);//配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式 
	Set_NRF24L01_CE; //CE为高,进入接收模式 
	}
							 
//该函数初始化NRF24L01到TX模式
//设置TX地址,写TX数据宽度,设置RX自动应答的地址,填充TX发送数据,选择RF频道,波特率和LNA HCURR
//PWR_UP,CRC使能
//当CE变高后,即进入RX模式,并可以接收数据了		   
//CE为高大于10us,则启动发送.	 
void TX_Mode(void)
	{														 
		Clr_NRF24L01_CE;	  
		NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,(unsigned char*)TX_ADDRESS,TX_ADR_WIDTH);//写TX节点地址 
		NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(unsigned char*)RX_ADDRESS,RX_ADR_WIDTH); //设置TX节点地址,主要为了使能ACK	  
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);     //使能通道0的自动应答    
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01); //使能通道0的接收地址  
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+SETUP_RETR,0x1a);//设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,0);       //设置RF通道为40
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启   
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG,0x0e);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中断
		Set_NRF24L01_CE;//CE为高,10us后启动发送
		delay_ms(200); //CE要拉高一段时间才进入发送模式
		
	}		  




