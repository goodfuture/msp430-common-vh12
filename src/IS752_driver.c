#include <msp430xG46x.h>
#include "test.h"

//#define TEST752 1

const INT16U MaskTbl[16]={0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x100,0x200,0x400,0x800,
                  0x1000,0x2000,0x4000,0x8000};
/*
#define IS752_Enable_1()  P2OUT&=~MaskTbl[1]
#define IS752_Enable_2()  P1OUT&=~MaskTbl[5]
#define IS752_Disable()   P2OUT|=MaskTbl[1];P1OUT|=MaskTbl[5]*/

#ifdef MAINBOARD
  #define IS752_Enable_1()  P2OUT&=~MaskTbl[1]
  #define IS752_Enable_2()  P1OUT&=~MaskTbl[5]
  #define IS752_Disable()   P2OUT|=MaskTbl[1];P1OUT|=MaskTbl[5]

  #define IS752_RST1        P2OUT&=~MaskTbl[0];
  #define IS752_UNRST1      P2OUT|=MaskTbl[0];
  #define IS752_RST2        P1OUT&=~MaskTbl[4];
  #define IS752_UNRST2      P1OUT|=MaskTbl[4];
#else
  #define IS752_Enable_1()  P7OUT&=~BIT1
  #define IS752_Enable_2()  P4OUT&=~BIT2
  #define IS752_Disable()   P7OUT|=BIT1;P4OUT|=BIT2

  #define IS752_RST1        //P7OUT&=~MaskTbl[4];
  #define IS752_UNRST1      //P7OUT|=MaskTbl[4];
  #define IS752_RST2        //P5OUT&=~MaskTbl[6];
  #define IS752_UNRST2      //P5OUT|=MaskTbl[6];
#endif

#define IS752_GPIO  0x58
#define IS752_CH_A  0X00
#define IS752_CH_B  0X02

unsigned char symbol1=0;
unsigned char symbol2=0;



#define SEND(X);   Delay_10_uS();\
                   while(!(IFG2&UTXIFG1));\
                   U1TXBUF =  X;\
                   for(i=0;i<len;i++)\
                   {\
                   while(!(IFG2&UTXIFG1));\
                   U1TXBUF =*src++;\
                   }\
                   while((U1TCTL&TXEPT)!=0X01);\
                   Delay_10_uS();\
                   IS752_Disable();\
                   Delay_10_uS();

void intial752(INT8U x,INT8U y,INT8U which)
{
  if(which==1)
	  IS752_Enable_1();
  else
    IS752_Enable_2();
  //Delay_N_mS(20);
	Delay_10_uS();
  //while(!(IFG2&UTXIFG1));
  while((U1TCTL&TXEPT)!=0X01);
	U1TXBUF =x;	//дIER�Ĵ���
	while(!(IFG2&UTXIFG1));
	U1TXBUF =y;
  while((U1TCTL&TXEPT)!=0X01);
  Delay_10_uS();
	//while(!(IFG2&UTXIFG1));
	IS752_Disable();
  //Delay_N_mS(20);
	Delay_10_uS();
}

void IS752_initial_1(INT8U baudA,INT8U baudB)	//SC16IS752��ʼ������
{
  INT8U Baud;
  INT16U baudrate;

  switch(baudA)
  {
    case 0:
      baudrate=BAUDRATE_9600;
    break;
    case 1:
      baudrate=BAUDRATE_57600;
    break;
    case 2:
      baudrate=BAUDRATE_115200;
    break;
    case 3:
      baudrate=BAUDRATE_4800;
    break;
    case 4:
      baudrate=BAUDRATE_1200;
    break;
    default:
      baudrate=BAUDRATE_9600;
    break;
  }
  /*
  IS752_RST1;
  Delay_N_mS(2000);
  IS752_UNRST1;
  Delay_N_mS(2000);*/
	//����Channel A
  //intial752(0x55,0x55,1);
	intial752(0x18,0xbf,1);//дLCR�Ĵ���
	 intial752(0x10,0x10,1);//дEFR�Ĵ��� enable enhanced function
	intial752(0x18,0x80,1);//дLCR�Ĵ���
  Baud=(INT8U)baudrate;
	intial752(0x00,Baud,1);//дDLL�Ĵ��� ������9600 ����14745600Hz 0X10 57600
	Baud=(INT8U)(baudrate>>8);
  intial752(0x08,Baud,1);//дDLH�Ĵ���
	 intial752(0x18,0x03,1);//дLCR�Ĵ���

	intial752(0x10,0x07,1);//дFCR�Ĵ��� enable TX&RX FIFO,clear all FIFOs
  Delay_N_mS(200);
	//intial752(0x10,0x00);//дFCR�Ĵ���
	
	intial752(0x08,0x01,1);//дIER�Ĵ��� enable RHR interrupt
	intial752(0x20,0x04,1);//дMCR�Ĵ���
	//intial752(0x20,0x00,1);//дMCR�Ĵ���

	//intial752(0x78,0x11,1);//дEFCR�Ĵ��� RS-485 mode and transmitter control RTS pin
	intial752(0x78,0x10,1);
  intial752(0x50,0xFF,1); //iodir  output
	intial752(0x38,0x32,1);//TLR TXFIFO TRIGGER LEVEL 8,RXFIFO 8

  switch(baudB)
  {
    case 0:
      baudrate=BAUDRATE_9600;
    break;
    case 1:
      baudrate=BAUDRATE_57600;
    break;
    case 2:
      baudrate=BAUDRATE_115200;
    break;
    case 3:
      baudrate=BAUDRATE_4800;
    break;
    case 4:
      baudrate=BAUDRATE_1200;
    break;
    default:
      baudrate=BAUDRATE_9600;
    break;
  }

	//����Channel B
	intial752(0x1a,0xbf,1);//дLCR�Ĵ���
	intial752(0x12,0x10,1);//дEFR�Ĵ���
	intial752(0x1a,0x80,1);//дLCR�Ĵ���
  Baud=(INT8U)baudrate;
	intial752(0x02,Baud,1);//дDLL�Ĵ���
	Baud=(INT8U)(baudrate>>8);
  intial752(0x0a,Baud,1);//дDLH�Ĵ���
	intial752(0x1a,0x03,1);//дLCR�Ĵ���

	intial752(0x12,0x07,1);//дFCR�Ĵ���	
  Delay_N_mS(200);
	//intial752(0x92,0x00);//дFCR�Ĵ���
	intial752(0x0a,0x01,1);//дIER�Ĵ���
	intial752(0x22,0x04,1);//дMCR�Ĵ���
	//intial752(0x22,0x00);//дMCR�Ĵ���
	//intial752(0x7a,0x11,1);//дEFCR�Ĵ���
  intial752(0x7a,0x10,1);
	intial752(0x52,0xFF,1); //iodir  output
	intial752(0x3a,0x32,1);//TLR
	
	IS752_Enable_1();
	Delay_10_uS();
	
	while(!(IFG2&UTXIFG1));
	U1TXBUF =IS752_GPIO;
	while(!(IFG2&UTXIFG1));
	U1TXBUF =0xFF;
	while(!(TXEPT&U1TCTL));
	IS752_Disable();
	Delay_10_uS();

  	IS752_Enable_1();
    Delay_N_mS(2000);
	Delay_10_uS();
	
	while(!(IFG2&UTXIFG1));
	U1TXBUF =IS752_GPIO;
	while(!(IFG2&UTXIFG1));
	U1TXBUF =0x00;
	while(!(TXEPT&U1TCTL));
	IS752_Disable();
	Delay_10_uS();
	
}

void IS752_initial_2(INT8U baudA,INT8U baudB)	//SC16IS752��ʼ������
{
  INT8U Baud;
  INT16U baudrate;

  switch(baudA)
  {
    case 0:
      baudrate=BAUDRATE_9600;
    break;
    case 1:
      baudrate=BAUDRATE_57600;
    break;
    case 2:
      baudrate=BAUDRATE_115200;
    break;
    case 3:
      baudrate=BAUDRATE_4800;
    break;
    case 4:
      baudrate=BAUDRATE_1200;
    break;
    default:
      baudrate=BAUDRATE_9600;
    break;
  }
  /*
  IS752_RST2;
  Delay_N_mS(20);
  IS752_UNRST2;
  Delay_N_mS(20);*/
	//����Channel A
	intial752(0x18,0xbf,2);//дLCR�Ĵ���
	intial752(0x10,0x10,2);//дEFR�Ĵ���
	intial752(0x18,0x80,2);//дLCR�Ĵ���
	Baud=(INT8U)baudrate;
  intial752(0x00,Baud,2);//дDLL�Ĵ��� ������57600 ����14745600Hz
	Baud=(INT8U)(baudrate>>8);
  intial752(0x08,Baud,2);//дDLH�Ĵ���
	intial752(0x18,0x03,2);//дLCR�Ĵ���

	intial752(0x10,0x07,2);//дFCR�Ĵ���
  Delay_N_mS(200);
	//intial752(0x10,0x00);//дFCR�Ĵ���
	
	intial752(0x08,0x01,2);//дIER�Ĵ���
	intial752(0x20,0x04,2);//дMCR�Ĵ���
	//intial752(0x20,0x00,2);//дMCR�Ĵ���

	//intial752(0x78,0x11,2);//дEFCR�Ĵ���
  intial752(0x78,0x10,2);
	intial752(0x50,0xFF,2); //iodir  output
	intial752(0x38,0x32,2);//TLR

  switch(baudB)
  {
    case 0:
      baudrate=BAUDRATE_9600;
    break;
    case 1:
      baudrate=BAUDRATE_57600;
    break;
    case 2:
      baudrate=BAUDRATE_115200;
    break;
    case 3:
      baudrate=BAUDRATE_4800;
    break;
    case 4:
      baudrate=BAUDRATE_1200;
    break;
    default:
      baudrate=BAUDRATE_9600;
    break;
  }
	//����Channel B
	intial752(0x1a,0xbf,2);//дLCR�Ĵ���
	intial752(0x12,0x10,2);//дEFR�Ĵ���
	intial752(0x1a,0x80,2);//дLCR�Ĵ���
	Baud=(INT8U)baudrate;
  intial752(0x02,Baud,2);//дDLL�Ĵ���
  Baud=(INT8U)(baudrate>>8);
	intial752(0x0a,Baud,2);//дDLH�Ĵ���
	intial752(0x1a,0x03,2);//дLCR�Ĵ���

	intial752(0x12,0x07,2);//дFCR�Ĵ���
  Delay_N_mS(200);
	//intial752(0x92,0x00);//дFCR�Ĵ���
	intial752(0x0a,0x01,2);//дIER�Ĵ���
	intial752(0x22,0x04,2);//дMCR�Ĵ���
	//intial752(0x22,0x00);//дMCR�Ĵ���
	//intial752(0x7a,0x11,2);//дEFCR�Ĵ���
  intial752(0x7a,0x10,2);
	intial752(0x52,0xFF,2); //iodir  output
	intial752(0x3a,0x32,2);//TLR
	
	IS752_Enable_2();
	Delay_10_uS();
	
	while(!(IFG2&UTXIFG1));
	U1TXBUF =0x58;
	while(!(IFG2&UTXIFG1));
	U1TXBUF =0x00;
	while(!(TXEPT&U1TCTL));
	IS752_Disable();
	Delay_10_uS();
	
}

void Chang752Baud(INT8U baudnum,INT8U com)
{
  INT8U baud;
  INT16U baudrate;

  switch(baudnum)
  {
    case 0:
      baudrate=BAUDRATE_9600;
    break;
    case 1:
      baudrate=BAUDRATE_57600;
    break;
    case 2:
      baudrate=BAUDRATE_115200;
    break;
    case 3:
      baudrate=BAUDRATE_4800;
    break;
    case 4:
      baudrate=BAUDRATE_1200;
    break;
    default:
      baudrate=BAUDRATE_9600;
    break;
  }
  switch(com)
  {
    case COM4:
      intial752(0x18,0x80,1);//дLCR�Ĵ���
      baud=(INT8U)baudrate;
      intial752(0x00,baud,1);//дDLL�Ĵ��� ������57600 ����14745600Hz
      baud=(INT8U)(baudrate>>8);
      intial752(0x08,baud,1);//дDLH�Ĵ���
      intial752(0x18,0x03,1);//дLCR�Ĵ���
    break;
    case COM5:
      intial752(0x1a,0x80,1);//дLCR�Ĵ���
      baud=(INT8U)baudrate;
      intial752(0x02,baud,1);//дDLL�Ĵ���
      baud=(INT8U)(baudrate>>8);
      intial752(0x0a,baud,1);//дDLH�Ĵ���
      intial752(0x1a,0x03,1);//дLCR�Ĵ���
      break;
    case COM2:
      intial752(0x18,0x80,2);//дLCR�Ĵ���
      baud=(INT8U)baudrate;
      intial752(0x00,baud,2);//дDLL�Ĵ��� ������57600 ����14745600Hz
      baud=(INT8U)(baudrate>>8);
      intial752(0x08,baud,2);//дDLH�Ĵ���
      intial752(0x18,0x03,2);//дLCR�Ĵ���


      break;
    case COM3:
      intial752(0x1a,0x80,2);//дLCR�Ĵ���
      baud=(INT8U)baudrate;
      intial752(0x02,baud,2);//дDLL�Ĵ���
      baud=(INT8U)(baudrate>>8);
      intial752(0x0a,baud,2);//дDLH�Ĵ���
      intial752(0x1a,0x03,2);//дLCR�Ĵ���
      break;
  }
}

void SendData(INT8U *src,INT16U len,INT8U com)
{
  INT16U i;
//  OS_CPU_SR     cpu_sr = 0;

  switch(com)
  {
    case COM1:
      //����ģʽ 485
      COM1_SEND;
      for(i=0;i<len;i++)
      {
        while(!(IFG2&UCA0TXIFG));
        UCA0TXBUF =*src++;
      }
      COM1_RECEIVE;
      //����ģʽ 485
      break;
    case COM2:
      COM2_SEND;
      IS752_Enable_1();
        SEND(IS752_CH_A);
      COM2_RECEIVE;
      break;
    case COM3:
      COM3_SEND;
      //for(i=0;i<len;i++)
      {
        IS752_Enable_1();
        SEND(IS752_CH_B);
      }
      COM3_RECEIVE;
      break;
    case COM4:
      COM4_SEND;
      //for(i=0;i<len;i++)
      {
        IS752_Enable_2();
        SEND(IS752_CH_A);
      }
      COM4_RECEIVE;
      break;
    case COM5:
      COM5_SEND;
      //for(i=0;i<len;i++)
      {
        IS752_Enable_2();
        SEND(IS752_CH_B);
      }
      COM5_RECEIVE;
      break;
    default:
    break;
  }

}

inline INT8U ReadRegister_752(INT8U reg,INT8U com)
{
  INT8U buff;

  switch(com)
  {
    case IS752_1:
      IS752_Enable_1();
    break;
    case IS752_2:
      IS752_Enable_2();
    break;
    default:
    break;
  }
  //OSTimeDly(5);
  Delay_10_uS();
  while(!(IFG2&UTXIFG1));
  U1TXBUF =reg;	//��Channel A�Ĵ���IIR	0x90
  while(!(IFG2&UTXIFG1));
  U1TXBUF =0;
  while(!(IFG2&URXIFG1));
  buff=U1RXBUF;

  while(!(IFG2&UTXIFG1));
  U1TXBUF =0;
  while(!(IFG2&URXIFG1));
  buff=U1RXBUF;

  IS752_Disable();
  //OSTimeDly(5);
  Delay_10_uS();
  return(buff);
}
/*
void Read_ReFIFO(INT8U *des,INT8U channel,INT8U com,INT8U len,INT8U flags)
{

}*/
void Task_ReDataFrom752_1(void /*pdata*/)  //12
{
  //INT8U err;
  INT8U buff;
  INT8U count,counta;
  //INT8U sign;
  //OS_FLAGS  flags;

  //pdata=pdata;
  //for(;;)
  {
    //OSFlagPend(DataReFlags,IS752_1_HAS_DATA,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0,&err);
    //CHANNEL A RECEIVE
    //OSMutexPend(SPIMutex,0,&err);
    buff=ReadRegister_752(REG_IIR_A,IS752_1);
    if((buff&0X01)==0x00)
    {
      buff=ReadRegister_752(REG_RXLVL_A,IS752_1);
      IS752_Enable_1();
		  //OSTimeDly(5);
      Delay_10_uS();
		  while(!(IFG2&UTXIFG1));
		  U1TXBUF =REG_REFIFO_A;			
		
      while(!(IFG2&UTXIFG1));//READ A DUMY BYTE
      U1TXBUF =0;
      while(!(IFG2&URXIFG1));
      COM2_Buffer[0]=U1RXBUF;

      for(count=0,counta=0;counta<buff;count++,counta++)
      {
        while(!(IFG2&UTXIFG1));
        U1TXBUF =0;
        while(!(IFG2&URXIFG1));
        COM2_Buffer[count]=U1RXBUF;
        if(count>=MAX_LENGTH_EXTERN)
          count=0;
        //OSFlagPost(DataReFlags,COM2_HAS_DATA,OS_FLAG_SET,&err); //set flags
      }
		  IS752_Disable();
      COMSymbol.COM2DataReady=1;
      Symbol.Mod_symbol=1;
      //OSTimeDly(5);
      POdata_Receive|=(0x01<<POdata_Type);
      //Delay_10_uS();
    }
    //CHANNEL B RECEIVE
    buff=ReadRegister_752(REG_IIR_B,IS752_1);
    if((buff&0x01)==0x00)
    {
      buff=ReadRegister_752(REG_RXLVL_B,IS752_1);
      IS752_Enable_1();
      Delay_10_uS();
		  //OSTimeDly(5);
		  while(!(IFG2&UTXIFG1));
		  U1TXBUF =REG_REFIFO_B;			
		
      while(!(IFG2&UTXIFG1));	
      U1TXBUF =0;
      while(!(IFG2&URXIFG1));
      COM3_Buffer[0]=U1RXBUF;

      for(count=0,counta=0;counta<buff;count++,counta++)
      {
        while(!(IFG2&UTXIFG1));	
        U1TXBUF =0;
        while(!(IFG2&URXIFG1));
        COM3_Buffer[count]=U1RXBUF;
        if(count>=MAX_LENGTH_EXTERN)
          count=0;
        //OSFlagPost(DataReFlags,COM3_HAS_DATA,OS_FLAG_SET,&err); //set flags
      }
		  IS752_Disable();
      COMSymbol.COM3DataReady=1;
      Symbol.Mod_symbol=1;
     // OSTimeDly(5);
      //Delay_10_uS();
      POdata_Receive|=(0x01<<POdata_Type);
    }
    //OSMutexPost(SPIMutex);
  }
}

void Task_ReDataFrom752_2(void /*pdata*/)  //13
{
  //INT8U err;
  INT8U buff;
  INT8U count,counta;
  //INT8U sign;
  //OS_FLAGS  flags;

  //pdata=pdata;
  //for(;;)
  {
    //OSFlagPend(DataReFlags,IS752_2_HAS_DATA,OS_FLAG_WAIT_SET_ANY+OS_FLAG_CONSUME,0,&err);
    //CHANNEL A RECEIVE
    //OSMutexPend(SPIMutex,0,&err);
    buff=ReadRegister_752(REG_IIR_A,IS752_2);
    if((buff&0x01)==0x00)
    {
      buff=ReadRegister_752(REG_RXLVL_A,IS752_2);
      IS752_Enable_2();
		  //OSTimeDly(5);
      Delay_10_uS();
		  while(!(IFG2&UTXIFG1));
		  U1TXBUF =REG_REFIFO_A;			
		  	
      while(!(IFG2&UTXIFG1)); //READ A DUMY BYTE
      U1TXBUF =0;
      while(!(IFG2&URXIFG1));
      COM4_Buffer[0]=U1RXBUF;

      for(count=0,counta=0;counta<buff;count++,counta++)
      {
        while(!(IFG2&UTXIFG1));
        U1TXBUF =0;
        while(!(IFG2&URXIFG1));
        COM4_Buffer[count]=U1RXBUF;
        if(count>=MAX_LENGTH_EXTERN)
          count=0;
        //OSFlagPost(DataReFlags,COM4_HAS_DATA,OS_FLAG_SET,&err); //set flags
      }
		  IS752_Disable();
      COMSymbol.COM4DataReady=1;
      Symbol.Mod_symbol=1;
      POdata_Receive|=(0x01<<POdata_Type);
    }
    //CHANNEL B RECEIVE

    buff=ReadRegister_752(REG_IIR_B,IS752_2);
    if((buff&0x01)==0x00)
    {
      buff=ReadRegister_752(REG_RXLVL_B,IS752_2);
      IS752_Enable_2();
      Delay_10_uS();
		  //OSTimeDly(5);
		  while(!(IFG2&UTXIFG1));
		  U1TXBUF =REG_REFIFO_B;			
		
      while(!(IFG2&UTXIFG1));	
      U1TXBUF =0;
      while(!(IFG2&URXIFG1));
      COM5_Buffer[0]=U1RXBUF;

      for(count=0,counta=0;counta<buff;count++,counta++)
      {
        while(!(IFG2&UTXIFG1));	
        U1TXBUF =0;
        while(!(IFG2&URXIFG1));
        COM5_Buffer[count]=U1RXBUF;
        if(count>=MAX_LENGTH_EXTERN)
          count=0;
        //OSFlagPost(DataReFlags,COM3_HAS_DATA,OS_FLAG_SET,&err); //set flags
      }
		  IS752_Disable();
      COMSymbol.COM5DataReady=1;
      Symbol.Mod_symbol=1;
     // OSTimeDly(5);
      //Delay_10_uS();
      POdata_Receive|=(0x01<<POdata_Type);
    }
    //OSMutexPost(SPIMutex);
  }
}
/*******************************************
             external interrupt function
            used for IS752(extended uart)
*******************************************/
#pragma vector=PORT2_VECTOR
__interrupt void EXTERNALISR(void)
{
  if(P2IFG&BIT1)
  {
    P2IFG&=~BIT1;
    Symbol.COM2orCOM3=1;
  }
  if(P2IFG&BIT2)
  {
    P2IFG&=~BIT2;
    Symbol.COM4orCOM5=1;
  }

}
/********************************************
            UART interrupt function
********************************************/
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR(void)
{
  /*COM2_Buffer[lenghcom++]=UCA0RXBUF;
  if(lenghcom>=16)
    lenghcom=0;*/
  INT8U temp;
  temp=UCA0RXBUF;
	if((Symbol.dealcommand==0))											//�ж��Ƿ�����������
	{
		if(temp=='#') 								//�жϽ��յ��ĵ�һ���ַ��Ƿ���"#"
		{
			if(Receive_Package_length!=1)
			{
				Receive_Package_length=0;				//�������ݰ��ĳ���
				Receive_Data_length=0;					//���������ܳ���
        Receive_Data_len=0;
				ReData.package_head[0]=temp;
				Receive_Package_length=0x01;				//�������ݰ��ĳ���
				Symbol.isover=0;
			}
			else
			{
						Receive_Data_length=0;					//���������ܳ���
						ReData.package_head[1]=temp;	
						Receive_Package_length=0x02;				//�������ݰ��ĳ���										
			}
		}
		else
		{
			if((Receive_Package_length>1) &&(Receive_Package_length<6)) 				// �ж��Ƿ��ǵ������ַ�___���ݶγ��ȵڣ�λ
			{				
				ReData.data_length[Receive_Package_length-2]=temp;	
				if(Receive_Package_length==5)				// �ж��Ƿ��ǵ��ĸ��ַ�___���ݶγ��ȵڣ�λ
				{

						Receive_Data_length=(ReData.data_length[0]&0x0f)*1000+(ReData.data_length[1]&0x0f)*100+(ReData.data_length[2]&0x0f)*10+(ReData.data_length[3]&0x0f);
						Receive_Data_len=0;
				}
				Receive_Package_length++;				//�������ݰ��ĳ���				
			}
			else
			{
				if((Receive_Package_length>5)&&(Receive_Package_length<306))				// ��������
				{
					if(Receive_Data_len<Receive_Data_length)
					{
						ReData.data_segme[Receive_Data_len]=temp;
						Receive_Data_len++;
						Receive_Package_length++;
					}
					else
					{
						ReData.data_segme[Receive_Data_len]='\0';
						ReData.CRC[Receive_Package_length-6-Receive_Data_length]=UCA0RXBUF;//����У����
						Receive_Package_length++;
					}
			
				}
        else
        {
          Receive_Package_length=0;				//�������ݰ��ĳ���
          Receive_Data_length=0;					//���������ܳ���
          Receive_Data_len=0;
        }
			}
		}


		if(temp==0x0d)				//���հ�β�����жϽ��յ��ĵ�һ���ַ��Ƿ���"0x0d"
		{
			ReData.CRC[4]='\0';
			Receive_Package_length++;
			Symbol.isover=0;
		}
		if((temp==0x0a) && (Symbol.isover==0))
		{
			Receive_Package_length++;
			Symbol.isover=1;
			Symbol.dealcommand=1;			
		}
  }
}
/*
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
  //INT8U err;
  //OSIntEnter();
  P2IFG&= ~MaskTbl[2];  //P2.2
  symbol1=1;
  //OSFlagPost(DataReFlags,IS752_1_HAS_DATA,OS_FLAG_SET,&err);
  //OSIntExit();
}
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  //INT8U err;
  //OSIntEnter();
  P1IFG&= ~MaskTbl[6];  //P1.6
  symbol2=1;
  //OSFlagPost(DataReFlags,IS752_2_HAS_DATA,OS_FLAG_SET,&err);
  //OSIntExit();
}*/
