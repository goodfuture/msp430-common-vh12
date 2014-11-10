#ifndef __DEFINE_H
#define __DEFINE_H

#include "test.h"
/*
typedef char INT8U;
typedef unsigned int  INT16U;
typedef unsigned long INT32U;
*/


#define width      30       //��ʾ�����
#define addr_w     0x0000   //�ı���ʾ���׵�ַ
#define addr_t     0x01e0   //ͼ����ʾ���׵�ַ

//#define VERSIONTEST 1

#ifdef VERSIONTEST
#define PROGTYPE    31
#define VERSION	    0
#define SUBVERSION  6
#else
#define PROGTYPE    32
#define VERSION	    1
#define SUBVERSION  2
#endif


#define PONUM 85

//----------------------LCD DRIVER LINE-----------

#define KEYREAD() P8IN|0XC0

#define WR_1  P9OUT|=BIT7
#define WR_0  P9OUT&=~BIT7

#define RD_1  P9OUT|=BIT6
#define RD_0  P9OUT&=~BIT6

#define CE_1  P9OUT|=BIT5
#define CE_0  P9OUT&=~BIT5

#define CD_1  P9OUT|=BIT4
#define CD_0  P9OUT&=~BIT4

#define RST_1 P9OUT|=BIT3
#define RST_0 P9OUT&=~BIT3

#define WR_DB P5OUT&=~BIT0
#define RD_DB P5OUT|=BIT0


//---------------------------------
#define KEY_ENTER 	0xDF
#define KEY_LEFT	0xf7
#define KEY_DOWN	0xfb
#define KEY_UP		0xFD
#define KEY_CANCEL	0xFE
#define KEY_RIGHT	  0xEF
#define KEY_RESET       KEY_CANCEL&KEY_DOWN&KEY_LEFT&KEY_ENTER//0xE8
#define LCD_RESET       KEY_ENTER&KEY_CANCEL//0xEE		
#define BACK_DOOR0      KEY_LEFT&KEY_RIGHT//0xDD
#define BACK_DOOR1      KEY_LEFT&KEY_RIGHT&KEY_CANCEL//0xCD

//--------------------extern variable---------------------------------------
INT8U IOP5;

 //INT8U parameter.sys_parameter.PO_Type[6];
 INT8U Device_Num; //��������
 //INT8U parameter.sys_parameter.LCD_Backtime; //����ʱ��
 INT8U Device_Protocol; //Э������
 //INT8U parameter.sys_parameter.LCD_Baud;	//����Ĭ��30��
 //INT8U Channel_Num[6];
 //INT8U parameter.sys_parameter.PO_SerialNum[6][16];
 //INT8U Symbol.rd_change;	//ʵʱ�����ϱ�����ı��־
 //INT8U Symbol.DO5_DI15;
 //INT8U Symbol.DO6_DI16;
 //INT8U ADP_change;
 //INT8U parameter.AD_i.AI_Type[8];

 struct MOD
{
    INT8U PO_code[4];
    INT8U modbus_address[2];
};
extern  const struct MOD MODBUS[PONUM];

extern const struct MOD  MOD_Gas[];
extern const struct MOD MOD_Water[];

//INT8U System_Time1[15];
//INT8U System_Time[15];
//INT8U ftstring[18];
//INT8U Mod_Pcode;

//INT8U Symbol.LCD_T;
//INT8U Symbol.LCD_DO;
//INT8U Symbol.LCD_DI;
//INT8U Symbol.LCD_AD;
//INT8U Symbol.Pass_id;
//INT8U s_set;
//extern bit LCD_DO90;

//--------------------------------------------------------------

//---------------------- LCD use variable--------------------------------------

INT8U LCD_Backlight=0;
INT8U Numlenth=0;
INT8U PO_Change=0;
INT8U X_Pos=0;
INT8U Y_Pos=0;
INT8U pre_X_Pos=0;
INT8U pre_Y_Pos=0;
INT8U AD_Y_Pos=0;
INT8U AD_pre_Y_Pos=0;
INT8U LCD_Channel_index[16][4];
INT8U NZ=0;
INT8U Device_ENable=0;

//volatile INT8U LCD_page; //��ǰLCDҳ
volatile INT8U Pre_LCD_page;//ǰһLCDҳ
volatile INT8U Prein_LCD_page;
//volatile INT8U KeyV=0;		//����ֵ
 INT8U LCD_DOP;	//��������λ��
//volatile INT8U LCD_RSTime=0;	//��λ����������
//INT8U LCD_RST=0;
//INT8U Screen_RST=0;
  //INT8U LCD_backdoor=0;
  //INT8U LCD_Dot=0;
INT8U Pre_LCD_Dot[4]={0,0,0,0};
//INT8U Symbol.LCD_deal=0;	//�������־
/*
volatile INT8U LCD_set=0;	//���ñ�־
volatile INT8U LCD_back=0;	//���÷���
volatile INT8U LCD_DOS=0;	//״ָ̬ʾ
*/
struct _LCD
{
  unsigned LCD_DOS:1;
  unsigned LCD_set:1;
  unsigned LCD_back:1;
  unsigned LCD_backdoor:1;
  unsigned LCD_Dot:1;
  unsigned LCD_EQU:1;
  unsigned LCD_LOG:1;
}LCDSymbol;

//volatile INT8U Symbol.LCD_para_set=0;	//��LCD����ϵͳ������־ 1��ʾ�������ã�0��ʾδ����
//volatile INT8U Symbol.LCD_Page_Change=0;	//��ҳ��־λ
//volatile INT8U Symbol.LCD_Control;

signed char LCD_NoPt;		//����ָ��
INT8U LCD_NumPos; //����λ��ָ��
INT8U LCD_comfirm[7];		//ȷ������
INT8U LCD_Temp[15];		//��ʾ��ʱ����
INT8U LCD_index1[5][15];
INT8U LCD_index2[4][6];
//INT8U LCD_user_pw[7]="000000\0";
//INT8U LCD_light=0;
INT8U LCD_indexAD[8][5][14];

INT8U  Menu_Select=0;
INT8U Par_Select=0;
INT8U Par_Setting=0;

//INT8U ftstring[18];

//-----------------------------------------------------------------------------

INT8U Revers(INT8U data1);
void DB_WR(INT8U data);
INT8U DB_RD();
void  Delay_10_uS(void);
void delay11 (unsigned int ms);
void wr_od (INT8U dat,INT8U comm);
void wr_td (INT8U datl,INT8U dath,INT8U comm);
void wr_xd (INT16U dat,INT8U comm);
void wr_auto (INT8U dat);
void wr_comm (INT8U comm);
void wr_data (INT8U dat);
void chk_busy (INT8U autowr);
void init_lcd (void);
void clrram (void);
void clrramBlock (INT8U X0,INT8U Y0,INT8U nbyte);
void disp_chn1616_1(INT8U X0,INT8U Y0, INT8U index,INT8U PoN);
void disp_chn1616(INT8U X0,INT8U Y0,const INT8U *index,INT8U PoN);
void disp_img (INT16U addr,INT8U xl,INT8U yl,const INT8U *img);
void disp_chn816_1(INT8U X0,INT8U Y0,INT8U index,INT8U PoN);
void disp_chn816(INT8U X0,INT8U Y0,const INT8U *index,INT8U PoN);
void disp_yline(INT8U X0,INT8U Y0);
void disp_xline(INT8U Y0);
void LCD_indexs(INT8U *Dnum,INT8U lenth,INT8U *L_ind);
void disp10();
void disp30();
void disp20();
void disp41();
void disp42();
void disp43();
void disp_SetNum_5x(INT8U Numlenth);
void disp50();
void disp56();
void disp57();
void disp58();
void disp_SetNum_6x(INT8U Numlenth);
void disp60();
void disp_SetNum_10x(INT8U Numlenth);
void disp100();
void disp101();
INT8U posselect(INT8U Pos_X,INT8U Pos_Y);
void disp200();
void disp_SetNum_ADx(INT8U Numlenth);
void disp70();
void disp90();
INT8U KeyRead();
INT8U getkey();
void LCD_disp();
void index_initial();


void DI_Status();

#endif
