#include "24L01.h"
#include "delay.h"
//Mini STM32������
//NRF24L01 �������� 

// SPI�����ٶ����� 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

const unsigned char TX_ADDRESS[TX_ADR_WIDTH]={0x22,0x22,0x33,0x44,0x55}; //���͵�ַ
const unsigned char RX_ADDRESS[RX_ADR_WIDTH]={0x22,0x22,0x33,0x44,0x55};  //���͵�ַ
							    
//��ʼ��24L01��IO��
void NRF24L01_Init(void)
	{
	GPIO_InitTypeDef GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE );	
	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;          //��ʼ��CE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	GPIO_SetBits(GPIOA,GPIO_Pin_4);	 


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;           //��ʼ��CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	Set_NRF24L01_CSN; 
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;          //��ʼ��IRQ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU  ;   //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	SPI1_Init();    //��ʼ��SPI1
	
	Clr_NRF24L01_CE; 	//ʹ��24L01
	
	Set_NRF24L01_CSN;	//SPIƬѡȡ��	


	}
	
	
	
	//SPI �ٶ����ú���
//SpeedSet:
//SPI_SPEED_2   2��Ƶ   (SPI 36M@sys 72M)
//SPI_SPEED_8   8��Ƶ   (SPI 9M@sys 72M)
//SPI_SPEED_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256��Ƶ (SPI 281.25K@sys 72M)
void SPIx_SetSpeed(u8 SpeedSet)
	{
	SPI1->CR1&=0XFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI_SPEED_2)//����Ƶ
		{
		SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
		}
	else if(SpeedSet==SPI_SPEED_8)//�˷�Ƶ 
		{
		SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
		}
	else if(SpeedSet==SPI_SPEED_16)//ʮ����Ƶ
		{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
		}
	else			 	 //256��Ƶ
		{
		SPI1->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz ����ģʽ
		}
	//SPI1->CR1|=1<<6; //SPI�豸ʹ��	
	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����  
	}
	
	
	
	//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
unsigned char SPIx_ReadWriteByte(unsigned char TxData)
	{		
		unsigned char retry=0;				 
		/* Loop while DR register in not emplty */
		
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
			{
				retry++;
				if(retry>200)return 0;
			}			  
		/* Send byte through the SPI1 peripheral */
		SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
			
		retry=0;
			
		/* Wait to receive a byte */
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
			{
				retry++;
				if(retry>200)return 0;
			}	  						    
		/* Return the byte read from the SPI bus */
			
		return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����
			
	}
	
	
	
	
	
	

//���24L01�Ƿ����
//����ֵ:0���ɹ�;1��ʧ��	
unsigned char NRF24L01_Check(void)
	{
		unsigned char buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
		unsigned char i;
		SPIx_SetSpeed(SPI_SPEED_16); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz�� 
		NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
		NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
		for(i=0;i<5;i++)
		{
		  if(buf[i]!=0XA5)break;	 
		}
		if(i!=5)return 1;//���24L01����	
		return 0;		 //��⵽24L01
	}	
 	 
//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
unsigned char NRF24L01_Write_Reg(unsigned char reg,unsigned char value)
	{
		unsigned char status;	
		Clr_NRF24L01_CSN;                 //ʹ��SPI����
		status =SPIx_ReadWriteByte(reg);//���ͼĴ����� 
		SPIx_ReadWriteByte(value);      //д��Ĵ�����ֵ
		Set_NRF24L01_CSN;                 //��ֹSPI����	 
		return(status);       			//����״ֵ̬
	}

//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
unsigned char NRF24L01_Read_Reg(unsigned char reg)
	{
		unsigned char reg_val;	
		Clr_NRF24L01_CSN;          //ʹ��SPI����		
		SPIx_ReadWriteByte(reg);   //���ͼĴ�����
		reg_val=SPIx_ReadWriteByte(0XFF);//��ȡ�Ĵ�������
		Set_NRF24L01_CSN;          //��ֹSPI����		    
		return(reg_val);           //����״ֵ̬
	}	

//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
unsigned char NRF24L01_Read_Buf(unsigned char reg,unsigned char *pBuf,unsigned char len)
	{
		unsigned char status,NUM;	  
		
		Clr_NRF24L01_CSN;           //ʹ��SPI����
		
		status=SPIx_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬  
		
		for( NUM=0;NUM<len;NUM++)pBuf[NUM]=SPIx_ReadWriteByte(0XFF);//��������
		
		Set_NRF24L01_CSN;       //�ر�SPI����
		
		return status;        //���ض�����״ֵ̬
		
	}

//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
unsigned char NRF24L01_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char len)
	{
		
		unsigned char status,NUM;	 
			
		Clr_NRF24L01_CSN;          //ʹ��SPI����
			
		status = SPIx_ReadWriteByte(reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
			
		for(NUM=0; NUM<len; NUM++)SPIx_ReadWriteByte(*pBuf++); //д������	
			
		Set_NRF24L01_CSN;       //�ر�SPI����
			
		return status;          //���ض�����״ֵ̬
		
	}
				   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
unsigned char NRF24L01_TxPacket(unsigned char *txbuf)
	{
	unsigned char sta;
	SPIx_SetSpeed(SPI_SPEED_16);//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	Clr_NRF24L01_CE;
		
	NRF24L01_Write_Buf(NRF24L01_WR_TX_PLOAD,txbuf,TX_PLOAD_WIDTH);//д���ݵ�TX BUF  32���ֽ�
		
	Set_NRF24L01_CE;//��������	
		
	while(NRF24L01_IRQ!=0);//�ȴ��������
		
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ	  
		
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
		
	if(sta&MAX_TX)//�ﵽ����ط�����
		{
			
		NRF24L01_Write_Reg(NRF24L01_FLUSH_TX,0xff);//���TX FIFO�Ĵ��� 
			
		  return MAX_TX; 
			
		}
	if(sta&TX_OK)//�������
		{
			
		  return TX_OK;
			
		}
	return 0xff;//����ԭ����ʧ��
	}

//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:0��������ɣ��������������
unsigned char NRF24L01_RxPacket(unsigned char *rxbuf)
	{
	unsigned char sta;	
		
	SPIx_SetSpeed(SPI_SPEED_16); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��  
		
	sta=NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ    	 
		
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS,sta); //���TX_DS��MAX_RT�жϱ�־
		
	if(sta&RX_OK)//���յ�����
		{
		  NRF24L01_Read_Buf(NRF24L01_RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);//��ȡ����
			
		  NRF24L01_Write_Reg(NRF24L01_FLUSH_RX,0xff);//���RX FIFO�Ĵ��� 
			
		  return 0; 
		}	
		
	return 1;//û�յ��κ�����
		
	}	
					    
//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void RX_Mode(void)
	{
	Clr_NRF24L01_CE;	  
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(unsigned char*)RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);    //ʹ��ͨ��0���Զ�Ӧ��    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ  	 
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,0);	     //����RFͨ��Ƶ��		  
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x0f);//����TX�������,0db����,2Mbps,���������濪��   
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG, 0x0f);//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	Set_NRF24L01_CE; //CEΪ��,�������ģʽ 
	}
							 
//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void TX_Mode(void)
	{														 
		Clr_NRF24L01_CE;	  
		NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,(unsigned char*)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
		NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(unsigned char*)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,0);       //����RFͨ��Ϊ40
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��   
		NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
		Set_NRF24L01_CE;//CEΪ��,10us����������
		delay_ms(200); //CEҪ����һ��ʱ��Ž��뷢��ģʽ
		
	}		  




