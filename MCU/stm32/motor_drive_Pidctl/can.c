#include "can.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "control.h" 
 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEKս��STM32������
//CAN���� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/11
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ.��Χ:1~3; CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.��Χ:1~8;
//tbs1:ʱ���1��ʱ�䵥Ԫ.��Χ:1~16;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//ע�����ϲ����κ�һ����������Ϊ0,�������.
//������=Fpclk1/((tsjw+tbs1+tbs2)*brp);
//mode:0,��ͨģʽ;1,�ػ�ģʽ;
//Fpclk1��ʱ���ڳ�ʼ����ʱ������Ϊ36M,�������CAN_Normal_Init(1,8,7,5,1);
//������Ϊ:36M/((1+8+7)*5)=450Kbps
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode)
{

	  GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
 	  CAN_FilterInitTypeDef  CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE 
   	NVIC_InitTypeDef  NVIC_InitStructure;
#endif

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PORTAʱ��	                   											 

  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);		//��ʼ��IO
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��IO
	  
 	//CAN��Ԫ����
 	CAN_InitStructure.CAN_TTCM=DISABLE;						 //��ʱ�䴥��ͨ��ģʽ  //
 	CAN_InitStructure.CAN_ABOM=DISABLE;						 //�����Զ����߹���	 //
  	CAN_InitStructure.CAN_AWUM=DISABLE;						 //˯��ģʽͨ����������(���CAN->MCR��SLEEPλ)//
  	CAN_InitStructure.CAN_NART=ENABLE;						 	//��ֹ�����Զ����� //
  	CAN_InitStructure.CAN_RFLM=DISABLE;						 //���Ĳ�����,�µĸ��Ǿɵ� // 
  	CAN_InitStructure.CAN_TXFP=DISABLE;						 //���ȼ��ɱ��ı�ʶ������ //
  	CAN_InitStructure.CAN_Mode= mode;	         //ģʽ���ã� mode:0,��ͨģʽ;1,�ػ�ģʽ; //
  	//���ò�����
  	CAN_InitStructure.CAN_SJW=tsjw;				//����ͬ����Ծ����(Tsjw)Ϊtsjw+1��ʱ�䵥λ  CAN_SJW_1tq	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=tbs1; //Tbs1=tbs1+1��ʱ�䵥λCAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=tbs2;//Tbs2=tbs2+1��ʱ�䵥λCAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=brp;            //��Ƶϵ��(Fdiv)Ϊbrp+1	//
  	CAN_Init(CAN1, &CAN_InitStructure);            // ��ʼ��CAN1 

 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
   	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//������0������FIFO0
 	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //���������0

  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	
#if CAN_RX0_INT_ENABLE
	
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0��Ϣ�Һ��ж�����.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // �����ȼ�Ϊ0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            // �����ȼ�Ϊ2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}   
 
#if CAN_RX0_INT_ENABLE	//ʹ��RX0�ж�
//�жϷ�����			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
//	i++;
//	if(i==100)
//	{
//		LED0=!LED0;//��ʾϵͳ��������	
//		i=0;
//	}	
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	if(RxMessage.StdId == 0x01)
	{
		if(ptForkLift->u8CarID==0)//ǰ��
		{
			//ptForkLift->bDrection    = (RxMessage.Data[0]&0x01); // 0Ϊǰ��  1Ϊ����
			if (RxMessage.Data[0]&0x01)
			     ptForkLift->s16speedwant = -40*RxMessage.Data[1];//����  �Ŵ���ע���� ��λ������һ��
			else ptForkLift->s16speedwant = 40*RxMessage.Data[1]; //ǰ��
		}
		else if(ptForkLift->u8CarID==1)//ǰ��
		{
			//ptForkLift->bDrection  = ((RxMessage.Data[0]&0x02)>>1);
			if ((RxMessage.Data[0]&0x02)>>1)
			     ptForkLift->s16speedwant =  -40*RxMessage.Data[2];
			else ptForkLift->s16speedwant =  40*RxMessage.Data[1]; //ǰ��
		}	
		else if(ptForkLift->u8CarID==2)//����
		{
			//ptForkLift->bDrection  = ((RxMessage.Data[0]&0x04)>>2);
			if ((RxMessage.Data[0]&0x04)>>2)
			    ptForkLift->s16speedwant  = -40*RxMessage.Data[3];
			else ptForkLift->s16speedwant = 40*RxMessage.Data[3];
		}
		else if(ptForkLift->u8CarID==3)//����
		{
			//ptForkLift->bDrection  = ((RxMessage.Data[0]&0x08)>>3);
			if ((RxMessage.Data[0]&0x08)>>3)
			    ptForkLift->s16speedwant  = -40*RxMessage.Data[4];
			else ptForkLift->s16speedwant = 40*RxMessage.Data[4];
		}
		else if(ptForkLift->u8CarID==4)//ǰ��
		{
			//ptForkLift->bDrection  = ((RxMessage.Data[0]&0x10)>>4);
			if ((RxMessage.Data[0]&0x10)>>4)
			     ptForkLift->s16speedwant  = -40*RxMessage.Data[5];
			else ptForkLift->s16speedwant  = 40*RxMessage.Data[5];
		}
		else if(ptForkLift->u8CarID==5)//ǰ��
		{
			//ptForkLift->bDrection  = ((RxMessage.Data[0]&0x20)>>5);
			if ((RxMessage.Data[0]&0x20)>>5)
			    ptForkLift->s16speedwant   = -40*RxMessage.Data[6];
			else ptForkLift->s16speedwant  = 40*RxMessage.Data[6];
		}		
	}
	
	ptForkLift->Encoder_can_Tx_flag = 1;//����can��Ϣ���ͱ�־
	   
  ptForkLift->u16CanComCount[0]++;    // can ��Ϣ���ռ���
	
	// ptForkLift->u16RunPIDControl = 1;   //wyw ���� ����pid���Ʊ�ʶ  ��ϵͳ 1msʱ���ж��ڽ��п���
	
	//for(i=0;i<8;i++)
	//printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
}
#endif

//can����һ������(�̶���ʽ:IDΪ0X12,��׼֡,����֡)	
//len:���ݳ���(���Ϊ8)				     
//msg:����ָ��,���Ϊ8���ֽ�.
//����ֵ:0,�ɹ�;
//		 ����,ʧ��;
u8 Can_Send_Msg(void)
{	
  u8 mbox;
  u16 i=0;  	          
  mbox= CAN_Transmit(CAN1, &ptForkLift->Can_Txmessage);   
  while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//�ȴ����ͽ���
  if(i>=0XFFF)return 1;
  return 0;		

}
//can�ڽ������ݲ�ѯ
//buf:���ݻ�����;	 
//����ֵ:0,�����ݱ��յ�;
//����,���յ����ݳ���;
u8 Can_Receive_Msg(u8 *buf)
{		   		   
 	u32 i;
	CanRxMsg RxMessage;
    if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//û�н��յ�����,ֱ���˳� 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//��ȡ����	
    for(i=0;i<8;i++)
      buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}













