#include "nrf24l01.h"
#include "spi.h"
#include "delay.h"
#include "led.h"
#include "Control_200Hz.h"
/*
                  ////////////

            /////////////////////////                   /////////////////////////////
           ////                /////                               //////
          ////                /////                               //////
         /////               /////                  /////////////////////////////////////////
        //////////////////////////                             //////  /////
       /////                                                  //////     /////
     /////    ///////////////////                            //////        /////
    ////      ////          /////                           /////            //////
   ////       ////          /////                          /////              ///////
  ////        ////          /////                         /////                ////////
 /////        ///////////////////                        /////                   /////////
//����Ƽ���Ʒ
//�з��Ŷӣ����������Ŷ�
//��ϵ��ʽ��QQȺ��471023785
            ���䣺qitiansizhou@163.com
            �Ա���https://shop128265493.taobao.com/
//�������˳���ֻ��������ѧϰ��ѧ����ֹ������ҵ��;���������ؾ���
//�汾��V4.0
//���ڣ�20151018
//�޸�˵����
//
*/
uint8_t NRF24L01_RXDATA[RX_PLOAD_WIDTH] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//nrf24l01���յ�������
uint8_t TX_ADDRESS[TX_ADR_WIDTH]= {0x13,0x31,0x63,0x66,0x85};	//���ص�ַ
uint8_t RX_ADDRESS[RX_ADR_WIDTH]= {0x13,0x31,0x63,0x66,0x85};	//���յ�ַ				

/*
*****************************************************************
* д�Ĵ���
*****************************************************************
*/
uint8_t NRF_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;
	SPI_CSN_L();					  
	status = SPI_RW(reg);  
	SPI_RW(value);		  /* д���� */
	SPI_CSN_H();					  /* ��ֹ������ */
    return 	status;
}
/*
*****************************************************************
* ���Ĵ���
*****************************************************************
*/
uint8_t NRF_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;
	SPI_CSN_L();					 
	SPI_RW(reg);			  
	reg_val = SPI_RW(0);	  /* ��ȡ�üĴ����������� */
	SPI_CSN_H();					  /* ��ֹ������ */
    return 	reg_val;
}
/*
*****************************************************************
* д������
*****************************************************************
*/
uint8_t NRF_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	SPI_CSN_L();				        /* ѡͨ���� */
	status = SPI_RW(reg);	/* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		SPI_RW(pBuf[i]);		/* д���� */
	}
	SPI_CSN_H();						/* ��ֹ������ */
    return 	status;	
}
/*
*****************************************************************
* ��������
*****************************************************************
*/
uint8_t NRF_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t uchars)
{
	uint8_t i;
	uint8_t status;
	SPI_CSN_L();						/* ѡͨ���� */
	status = SPI_RW(reg);	/* д�Ĵ�����ַ */
	for(i=0; i<uchars; i++)
	{
		pBuf[i] = SPI_RW(0); /* ��ȡ�������� */ 	
	}
	SPI_CSN_H();						/* ��ֹ������ */
    return 	status;
}

/******************************************************************
* д���ݰ�
******************************************************************/
void NRF_TxPacket(uint8_t * tx_buf, uint8_t len)
{	
	SPI_CE_L();		 //StandBy Iģʽ	
	NRF_Write_Buf(WR_TX_PLOAD, tx_buf, len); 			 // װ������	
	SPI_CE_H();		 //�ø�CE���������ݷ���
}
/*
*****************************************************************
* ���NRF�Ƿ�����������1������
*****************************************************************
*/    
u8 NRF_CHECK(void)
{
	u8 buf[5]={0XA5,0XA5,0XA5,0XA5,0XA5};
	u8 i; 
	NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);		//��nRF�ļĴ���д�뼸������ 
	buf[1] = 0X11;
	NRF_Read_Buf(TX_ADDR,buf,5); //��nRF�ļĴ���д�뼸������  
	for(i=0;i<5;i++)
		if(buf[i]!=0XA5)
			break;                                                                   
	if(i!=5)return 0;//��nRF�ļĴ���д�뼸������  
	return 1;         
}

void NRF24L01_INIT(void)
{
	SPI_CE_L();
	NRF_Write_Buf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);//дRX�ڵ��ַ 
	//NRF_Write_Buf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH); //дTX�ڵ��ַ  
	NRF_Write_Reg(NRF_WRITE_REG+EN_AA,0x01); //ʹ��ͨ��0���Զ�Ӧ�� 
	NRF_Write_Reg(NRF_WRITE_REG+EN_RXADDR,0x01);//ʹ��ͨ��0�Ľ��յ�ַ 
	//NRF_Write_Reg(NRF_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10�� 
	NRF_Write_Reg(NRF_WRITE_REG+RF_CH,40); //����RFͨ��ΪCHANAL
	NRF_Write_Reg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ��� 
	NRF_Write_Reg(NRF_WRITE_REG+RF_SETUP,0x26); //����TX�������,0db����,2Mbps,���������濪��

	NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	
	SPI_CE_H();
}
/*
*****************************************************************
* ����ģʽ
*****************************************************************
*/
void SetRX_Mode(void)
{
	SPI_CE_L();
	NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
	SPI_CE_H();
} 
/*
*****************************************************************
* ����ģʽ
*****************************************************************
*/
void SetTX_Mode(void)
{
	SPI_CE_L();
	NRF_Write_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	SPI_CE_H();
} 
/*======================================================================*/
void NRF_IRQ(void)
{
	uint8_t sta = NRF_Read_Reg(NRF_READ_REG + NRFRegSTATUS);
	if(sta & (1<<RX_DR))//�����ж�
	{
		NRF_Read_Buf(0x61,NRF24L01_RXDATA,32);// read receive payload from RX_FIFO buffer
		NRF_Write_Reg(0x27, 0xff);//���nrf���жϱ�־λ
		//LEDALL_OFF;
	}
	if(sta & (1<<TX_DS))//������ɲ����յ�Ӧ���ź��ж�
	{
	}
	if(sta & (1<<MAX_RT))//�ﵽ����ط������ж�
	{
	}
	NRF_Write_Reg(0x27, 0xff);//���nrf���жϱ�־λ
}
//ң���ź�2.4G�����жϵĳ�ʼ��
void NRF_GPIO_Interrupt_Init(void)	//nRFʹ�õ��ⲿ�жϵ����ų�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource6);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_ClearITPendingBit(EXTI_Line6);
}

