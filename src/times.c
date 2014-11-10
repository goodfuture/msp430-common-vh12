#include <msp430xG46x.h>
#include "test.h"


#define DELAYNUM 2
void v_RTInputByte(INT8U ucDa) ;         //��ʵʱʱ��DS1302����һ���ֽ��ӳ���
void v_W1302(INT8U ucAddr, INT8U ucDa);//��ʵʱʱ��DS1302ĳ�Ĵ���дһ���ֽ�
INT8U uc_RTOutputByte(void);//��ʵʱʱ��DS1302��ȡһ���ֽ��ӳ���
INT8U uc_R1302(INT8U ucAddr); //��ʵʱʱ��DS1302ָ����ַ��ȡһ���ֽ��ӳ���
void v_Set1302( void ); //����ʵʱʱ��DS1302
void v_Get1302( void ); //��ʵʱʱ��DS1302�ж�ȡʱ��
void delays(INT16U m);  //��ʱ����
/**********************************************************************************************************
                     ʵʱʱ��ʱ�ӿ����ź��߶���
**********************************************************************************************************/
/*�������������ź��߶��塪��������������*/
#ifdef MAINBOARD
  #define RST10320  P2OUT&=~BIT5
  #define RST10321  P2OUT|=BIT5
  #define IO1032W   P2OUT&=~BIT4
  #define IO1032R   P2OUT|=BIT4
  #define CLK10320  P2OUT&=~BIT6
  #define CLK10321  P2OUT|=BIT6

  #define IO_IN     P2DIR&=~BIT4
  #define IO_OUT    P2DIR|=BIT4

  #define T_IO      P2IN&BIT4
#else
  #define RST10320  P3OUT&=~BIT5
  #define RST10321  P3OUT|=BIT5
  #define IO1032W   P3OUT&=~BIT7
  #define IO1032R   P3OUT|=BIT7
  #define CLK10320  P3OUT&=~BIT6
  #define CLK10321  P3OUT|=BIT6

  #define IO_IN     P3DIR&=~BIT7
  #define IO_OUT    P3DIR|=BIT7

  #define T_IO      P3IN&BIT7
#endif
/*���������������塪��������������*/
INT8U current_year; 		/*	��		*/
INT8U current_moth; 		/*	��		*/
INT8U current_date; 		/*	��		*/
INT8U current_hour; 		/*	Сʱ	*/
INT8U current_minute;		/*	����	*/
INT8U yestoday;
volatile INT8U current_second;		/*	��	*/
INT8U old_minute;
INT8U System_Time[15];
INT8U System_Time1[15]="20080101000000\0";

void delays(INT16U m)
{
	INT16U n;
	n=0;
	while(n < m)
	{n++;}
	return;
}
/*
*********************************************************************************************************
*                           ʵʱʱ��DS1302����
*	˵ ����	ʵʱʱ��DS1302����
*����˵����
*		����˳���� �� ʱ �� �� ���� ��
*			
*********************************************************************************************************
*/
/*---------------------��ʵʱʱ��DS1302����һ���ֽ��ӳ���------------------------*/
void v_RTInputByte(INT8U ucDa) //reentrant
{
	INT8U i;
	INT8U AC = ucDa;

  IO_OUT;
  delays(DELAYNUM);
  CLK10320;//;//T_CLK = 0;
	for(i=8; i>0; i--)			/*�˴�ѭ��*/
	{
		if( ( AC&0x01))		/*���λ�Ƿ�==1*/
			{
				IO1032R;////P4|=0x02;//T_IO = 1;			/*���λ=1*/
			}
		else
			{
				IO1032W;//;//T_IO = 0; 			/*���λ=0*/
			}
    //delays(DELAYNUM);
		CLK10321;//;//T_CLK = 1;
		delays(DELAYNUM);
		CLK10320;//;//T_CLK = 0;
    delays(DELAYNUM);
		AC = AC >>1; 			/*����������һλ*/
	}
  //delays(DELAYNUM);
}
/*---------------------��ʵʱʱ��DS1302ĳ�Ĵ���дһ���ֽ�------------------------*/
void v_W1302(INT8U ucAddr, INT8U ucDa)// reentrant
{
	RST10320;//;//T_RST = 0;
	delays(DELAYNUM);
	CLK10320;//;//T_CLK = 0;
	delays(DELAYNUM);
	RST10321;//;//T_RST = 1;
	delays(DELAYNUM);
	v_RTInputByte(ucAddr); /* д��ַ������ */
	v_RTInputByte(ucDa); /* д1Byte����*/
	CLK10320;//;//T_CLK = 0;
	delays(DELAYNUM);
	RST10320;//;//T_RST =0;
  delays(DELAYNUM);
}
/*---------------------��ʵʱʱ��DS1302��ȡһ���ֽ��ӳ���------------------------*/
INT8U uc_RTOutputByte(void) //reentrant
{
	INT8U i;
	INT8U AC;

  IO_IN;
  delays(DELAYNUM);
	for(i=8; i>0; i--)
	{
		AC = AC >>1; 	/*����������һλ*/
		//x=P4;
		if((T_IO)==0)// T_IO == 0)
		{
			AC = AC & 0x7f;
		}
		else
			AC = AC | 0x80;
    //delays(DELAYNUM);
		CLK10321;//;//T_CLK = 1;
		delays(DELAYNUM);
		CLK10320;//;//T_CLK = 0;
    delays(DELAYNUM);
			
	}
	return(AC);
}
/*---------------------��ʵʱʱ��DS1302ָ����ַ��ȡһ���ֽ��ӳ���------------------------*/
INT8U uc_R1302(INT8U ucAddr) //reentrant
{
	INT8U ucDa;

	RST10320;//;//T_RST = 0;
	delays(DELAYNUM);
	CLK10320;//;//T_CLK = 0;
	delays(DELAYNUM);
	RST10321;//;//T_RST = 1;
	v_RTInputByte(ucAddr); /* ��ַ������ */
	ucDa = uc_RTOutputByte(); /* ��1Byte���� */
	CLK10321;//;//T_CLK = 1;
	delays(DELAYNUM);
	RST10320;//;//T_RST =0;
  delays(DELAYNUM);
	return(ucDa);
}
/*---------------------���ó��ģʽ------------------------------*/
void v_charge1302(void)
{
  //INT8U ucAddr = 0x90;    //trickle charge

  v_W1302(0x8e,0x00); /* ��������,WP=0,д���� */
  v_W1302(0x90,0xc5); //one diode,2K register Ima=(3.3-0.7)/2K ~1.2mA
  v_W1302(0x8e,0x80); /* ��������,WP=1,д����*/
}
/*---------------------����ʵʱʱ��DS1302------------------------*/
void v_Set1302( void )	//reentrant
{
	INT8U i;
	INT8U hour,minute,second;
	INT8U year,moth,date;
	INT8U pSecDa[7];
	INT8U ucAddr = 0x80;

	year=((System_Time1[2]<<4)&0xf0) |(System_Time1[3]&0x0f);
	moth=((System_Time1[4]<<4)&0xf0) |(System_Time1[5]&0x0f);
	date=((System_Time1[6]<<4)&0xf0) |(System_Time1[7]&0x0f);
	hour=((System_Time1[8]<<4)&0xf0) |(System_Time1[9]&0x0f);
	minute=((System_Time1[10]<<4)&0xf0) |(System_Time1[11]&0x0f);
  second=((System_Time1[12]<<4)&0xf0) |(System_Time1[13]&0x0f);
	//current_moth=((System_Time1[2]<<4)&0xf0) |(System_Time1[2]&0x0f));

	/*current_year=07;
	current_moth=07;
	current_date=03;
	hour = ( current_hour / 10 ) * 16 + current_hour%10;
	hour = hour & 0x3f;
	minute = ( current_minute / 10 ) * 16 + current_minute%10;
	minute = minute & 0x7f;
	year=( current_year / 10 ) * 16 + current_year%10;
	moth=( current_moth / 10 ) * 16 + current_moth%10;
	date=( current_date / 10 ) * 16 + current_date%10;
	*/
	/*********************��д��ǰ��*****************************
	second = ( current_second / 10 ) * 16 + current_second%10;
	second = second & 0x7f;

	************************************************************/
	//second = uc_R1302(0x81);/*������ǰ��*/
	second = second & 0x7f;
	/*****************************************************/
	pSecDa[0] = second;
	pSecDa[1] = minute;
	pSecDa[2] = hour;
	pSecDa[3] = date;
	pSecDa[4] = moth;
	pSecDa[6] = year;
	v_W1302(0x8e,0x00); /* ��������,WP=0,д���� */
	for(i = 0; i <6; i++)
	{
		v_W1302(ucAddr,pSecDa[i]); /* �� �� ʱ �� �� ���� �� */
		ucAddr +=2;
	}
	v_W1302(ucAddr,pSecDa[6]); /* �� �� ʱ �� �� ���� �� */
	v_W1302(0x8e,0x80); /* ��������,WP=1,д����*/
}
/*---------------------��ʵʱʱ��DS1302�ж�ȡʱ��------------------------*/
//����˳���� �� ʱ �� �� ���� ��
void v_Get1302( void ) //reentrant
{
	INT8U ucCurtime[7];
	INT8U i;
	INT8U ucAddr = 0x81;
	
	for (i=0;i<7;i++)
	{
		//System_Time[i] = uc_R1302(ucAddr);/*��ʽΪ: �� �� ʱ �� �� ���� �� */
		ucCurtime[i] = uc_R1302(ucAddr);/*��ʽΪ: �� �� ʱ �� �� ���� �� */
		ucAddr += 2;
	}
  if(((ucCurtime[0]&0x0f)>9)||((ucCurtime[0]&0xf0)>0x50))
  {
    for(i=0;i<14;i++)
    {
      System_Time1[i] =System_Time[i];
    }
    v_Set1302();
    return;
  }
		System_Time[0] = 0x32;							//���ǧλ
		System_Time[1] = 0x30;							//��İ�λ	
		System_Time[2] = (ucCurtime[6]>>4)&0x0f|0x30;		//���ʮλ
		System_Time[3] = ucCurtime[6]&0x0f|0x30;				//��ĸ�λ
		System_Time[4] = (ucCurtime[4]>>4)&0x0f|0x30;		//�µ�ʮλ	
		System_Time[5] = ucCurtime[4]&0x0f|0x30;				//�µĸ�λ
		System_Time[6] = (ucCurtime[3]>>4)&0x0f|0x30;		//�յ�ʮλ
		System_Time[7] = ucCurtime[3]&0x0f|0x30;				//�յĸ�λ
		System_Time[8] = (ucCurtime[2]>>4)&0x0f|0x30;		//ʱ��ʮλ
		System_Time[9] = ucCurtime[2]&0x0f|0x30;				//ʱ�ĸ�λ
		System_Time[10] = (ucCurtime[1]>>4)&0x0f|0x30;		//�ֵ�ʮλ
		System_Time[11] = ucCurtime[1]&0x0f|0x30;			//�ֵĸ�λ
		System_Time[12] = (ucCurtime[0]>>4)&0x0f|0x30;		//���ʮλ
		System_Time[13] = ucCurtime[0]&0x0f|0x30;			//��ĸ�λ
		
		System_Time[14]='\0';
    ucCurtime[0] = ucCurtime[0] & 0x7f;
	current_second = ( ucCurtime[0] >> 4 ) * 10 + (ucCurtime[0]&0x0f);
  current_minute = ( ucCurtime[1] >> 4 ) * 10 + (ucCurtime[1]&0x0f);
  current_hour = ( ucCurtime[2] >> 4) * 10 + (ucCurtime[2]&0x0f);
  current_date = ( ucCurtime[3] >> 4 ) * 10 + (ucCurtime[3]&0x0f);
  current_moth= ( ucCurtime[4] >> 4 ) * 10 + (ucCurtime[4]&0x0f);
  current_year = ( ucCurtime[6]>> 4 ) * 10 + (ucCurtime[6]&0x0f);
    //if(((ucCurtime[0]&0x0f)>9)||((ucCurtime[0]&0xf0)>0x50))
  /*if((current_second>59)||(current_minute>59)||(current_hour>24)||(current_moth>12))
    v_Set1302();
  */
  /*
	ucCurtime[0] = ucCurtime[0] & 0x7f;
	current_second = ( ucCurtime[0] / 16 ) * 10 + ucCurtime[0]%16;
	if(current_second>59)
		v_Set1302();
	ucCurtime[1] = ucCurtime[1] & 0x7f;
	current_minute = ( ucCurtime[1] / 16 ) * 10 + ucCurtime[1]%16;
	if(current_minute>59)
		v_Set1302();
	//ucCurtime[2] = ucCurtime[2] & 0x3f;
	ucCurtime[2] = ucCurtime[2] & 0xff;
	current_hour = ( ucCurtime[2] / 16 ) * 10 + ucCurtime[2]%16;
	if(current_hour>24)
		v_Set1302();
	ucCurtime[3] = ucCurtime[3] & 0xff;
	current_date = ( ucCurtime[3] / 16 ) * 10 + ucCurtime[3]%16;
	if(current_date>31)
		v_Set1302();
	ucCurtime[4] = ucCurtime[4] & 0xff;
	current_moth= ( ucCurtime[4] / 16 ) * 10 + ucCurtime[4]%16;
	if(current_moth>12)
		v_Set1302();
	ucCurtime[6] = ucCurtime[6] & 0xff;
	current_year = ( ucCurtime[6] / 16 ) * 10 + ucCurtime[6]%16;
	if(current_year>99)
		v_Set1302();
	*/
}



