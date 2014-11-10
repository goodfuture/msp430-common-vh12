#include <msp430xG46x.h>
#include <string.h>


#define MAINFILE 1
#include "test.h"

int IO_init(void);

//#define DITEST 1
//#define DOTEST 1
//#define FLASHTEST 1
//#define COMTEST 1
//#define EEPROMTEST 1
//#define AITEST 1
//#define UARTTEST 1
//#define EXTENDEDUARTTEST 1
//#define TIMETEST 1
INT8U stop_watch_devstatus=0;

extern const  char zimo240128[30*64];
INT16U ADResult[8][10];

INT8U AD_CONVERTER=0;
INT8U ADTimes=0;

//#pragma vector=RESET_VECTOR
//#define PROGRAMDEBUG  1
int main(void)
{
  INT16U i,temp,second;
  INT16U count,oldDI;
  INT8U RUN;
  //RUNRECORD runlog;
  unsigned char old__second;
	unsigned char old__minute;
	unsigned char old__hour;
	unsigned char old__day;
	unsigned char minute_counter=0;
	//unsigned char hour_counter=0;
  unsigned int DI_Convert=0;

  char checkcode[6]="vh11+\0";
	char DI_C[4]="DI=\0";
	char d[5];

	unsigned int Prog_Circle=0;
	//unsigned char cps[]="DataTime=\0";
	INT8U DI_Alter=0;
  INT8U Save_Hourdata=0;
  INT8U Save_Daydata=0;

  //INT8U a[16]="123456789ABCDEF";
  //INT8U b[17];
   // INT8U c[3]={0xf8,0x10,0x10};
  INT8U checknum[7];
  /***********************************************
                     device initializing
  ************************************************/
  CPU_init();
  P4OUT |=BIT0;   //flash rst
  GPRS_POWERON;
  RealData.R_D.DO=0;
  DIInput();
	DOOutput(RealData.R_D.DO);
   init_lcd ();
   clrram();
   LCD_light_ON;
   disp_img(0x01e0+960,30,64,zimo240128);
   //MemoryClear(Modbustore,sizeof(Modbustore));
   Modbustore =0;
   MemoryClear(local_mma_H,sizeof(local_mma_H));
   MemoryClear(local_mma_D,sizeof(local_mma_D));
   MemoryClear(local_mma_M,sizeof(local_mma_M));//100504
   MemoryClear(dev_mma_D,sizeof(dev_mma_D));
   MemoryClear(dev_mma_H,sizeof(dev_mma_H));
   MemoryClear(dev_mma_M,sizeof(dev_mma_M));  //100504
   MemoryClear(Hour_COD_avg,sizeof(Hour_COD_avg));
   MemoryClear(Hour_B01_avg,sizeof(Hour_B01_avg));
	delay11(3200);
  //GPRS_POWEROFF;
 //test

#ifdef PROGRAMDEBUG
 char c[3]={0x00,0,0};
 char aa='A';
 FlashBufferWrite(c,"abcdefg",7,0);
 //c[0]=0xf0;
 //FlashBuffertoMemory(c,0);
 Delay_N_mS(20000);
 c[0]=0;
 FlashBufferRead(c,checknum,5,0);
 checknum[0]=0;

 c[0]=0xf0;
 FlashMemorytoBuffer(c,0);
 c[0]=0;
 FlashBufferRead(c,checknum,5,0);
 checknum[0]=0;
 c[0]=0xf0;
 FlashMemoryRead(c,checknum,5);
 //FlashMemoryWrite(c,checkcode,5,0);
 Delay_N_mS(20000);
   //c[2]=0x05;
 //FlashMemoryWrite(c,&aa,1,0);
 Delay_N_mS(20000);
 c[2]=0;
 FlashMemoryRead(c,checknum,5);
#endif
 //---------------------------
 /****************************************************
                  system initializing
 ****************************************************/
 AT24C64_RS(checknum,0x00,6);
 Delay_N_mS(2000);
 AT24C64_RS(checknum,0x00,6);
 if(strcmp(checknum,checkcode)!=0)
	{
    v_charge1302();
		v_Set1302();
		
		for(count=0;count<6;count++)
			SPARA.pw[count]=(count+1)|0x30;  //初始密码123456
		
		for(count=0;count<9;count++)
		{
			SPARA.mn[count]=(count+1)|0x30;  //mn 12345678901234
			ml_mn[count]=parameter.sys_parameter.mn[count];
		}
			
		for(count=0;count<5;count++)
		{
			SPARA.mn[count+9]=count|0x30;
			ml_mn[count+9]=parameter.sys_parameter.mn[count+9];
		}
		
		SPARA.st[0]='3';             //st 32
		SPARA.st[1]='2';
		ml_st[0]='3';
		ml_st[1]='2';
		for(count=0;count<9;count++)
			SPARA.sim[count]=(count+1)|0x30;  //sim 123456789
		
		SPARA.rd_interval[0]='3';     //实时数据上报时间 60秒
		SPARA.rd_interval[1]='0';
		SPARA.rd_interval[2]='0';
		
		for(count=0;count<5;count++)
			SPARA.overtime[count]='0';    //超时时间 5秒
		SPARA.overtime[4]='5';
		SPARA.resendtime[0]='0';		//重发次数 3次
		SPARA.resendtime[1]='3';
		for(count=0;count<5;count++)
			SPARA.warntime[count]='0';    //报警时间 5
		SPARA.warntime[4]='5';
		
		for(count=0;count<4;count++)
			SPARA.reporttime[count]='0';	//上报时间 5
		SPARA.reporttime[3]='5';
		for(count=0;count<2;count++)
			SPARA.alarmtarge[count]=0x30+count;		//仪表污染物编号
		SPARA.alarmtarge[2]='\0';
		SPARA.alarmtarge[3]='\0';
							
		for(count=0;count<8;count++)
		{
			//ADPARA(count).po_id[0]='P';
			//ADPARA(count).po_id[1]=0x31+count;
			//ADPARA(count).po_id[2]='\0';
                        ADPARA(count).po_id[0]='0';
			ADPARA(count).po_id[1]='0';
			ADPARA(count).po_id[2]='0';
			ADPARA(count).ADUV.b=16.0;
			ADPARA(count).ADLV.b=0.0;
			ADPARA(count).U_Alarmlimt.b=10.0;
			ADPARA(count).L_Alarmlimt.b=0.0;
			ADPARA(count).rate.b=(16.0-0.0)/0x666;
					
			ADPARA(count).AI_Type=2;
			
		}
    for(count=0;count<6;count++)
    SPARA.Serial_Baud[count]=0;
		//LCD_Baud=0;
		for(checknum[0]=0;checknum[0]<6;checknum[0]++)
			for(count=0;count<16;count++)
				SPARA.PO_SerialNum[checknum[0]][count]=0xff;		

    SPARA.PO_SerialNum[0][0]=0x05;
		
    for(count=0;count<6;count++)
		{
			SPARA.PO_Type[count]=0;
			Channel_Num[count]=0;
		}
    SPARA.PO_Type[0]=1; //水,哈希
			Channel_Num[0]=1;
		SPARA.alarmtarge[10]=0;//baud rate
    SPARA.LCD_Backtime=1;
		
		AT24C64_W(parameter.sys_setting,0x10,sizeof(PARAMETER));	
                /*初始化历史数据标志组结构体，存储于Flash第一页（页地址0x00）*/
                INT8U  Rdaddr[3]={0,0,0};
                INT16U pageaddr=0;
                HISDATAFLAGS Sighisdata;
                MemoryClear(&Sighisdata,sizeof(Sighisdata));
                for(count=0;count<7;)
                {
                  Sighisdata.page =0x10+count*31*24;
                  Rdaddr[1]=(INT8U)(pageaddr>>8);
                  Rdaddr[2]=(INT8U)(pageaddr);
                  FlashBufferWrite(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1);
                  FlashBufferWrite(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1);
                  count++;
                  pageaddr=count*120;
                }
                Rdaddr[0]=0;
                Rdaddr[1]=0;
                Rdaddr[2]=0;
                FlashBuffertoMemory(Rdaddr,FLASHBUFFER1);

                HISDATAFLAGS_D sig_d;
                pageaddr=0;
                MemoryClear(&sig_d,sizeof(sig_d));
                for(count=0;count<13;)
                {
                  sig_d.page =5400+count*31;
                  Rdaddr[0]=0;
                  Rdaddr[1]=((INT8U)(pageaddr>>8))|0x28;  //page 5
                  Rdaddr[2]=(INT8U)(pageaddr);
                  FlashBufferWrite(Rdaddr,&sig_d,sizeof(sig_d),FLASHBUFFER1);
                  count++;
                  pageaddr=count*80;
                }
                Rdaddr[0]=0;
                Rdaddr[1]=0x28;
                Rdaddr[2]=0;
                FlashBuffertoMemory(Rdaddr,FLASHBUFFER1);

                HISDATAFLAGS_R sig_r;
                MemoryClear(&sig_r,sizeof(sig_r));
                pageaddr=0;
                for(count=0;count<3;)
                {
                  sig_r.page =6000+count*24*12;
                  Rdaddr[0]=0;
                  Rdaddr[1]=((INT8U)(pageaddr>>8))|0x50;
                  Rdaddr[2]=(INT8U)(pageaddr);
                  FlashBufferWrite(Rdaddr,&sig_r,sizeof(sig_r),FLASHBUFFER1);
                  count++;
                  pageaddr=count*200;
                }
                Rdaddr[0]=0;
                Rdaddr[1]=0x50; // page 10
                Rdaddr[2]=0;
                FlashBuffertoMemory(Rdaddr,FLASHBUFFER1);
                
                //运行日志存储于8190页（1FFE）
                RUNRECORD runlog;
                MemoryClear(&runlog,sizeof(runlog));
                pageaddr=0; 
                for(count=0;count<10;count++)
                {
                  pageaddr=count*64;
                  Rdaddr[0]=0xff;
                  Rdaddr[1]=((INT8U)(pageaddr>>8))|0xf0;
                  Rdaddr[2]=(INT8U)(pageaddr);
                  FlashBufferWrite(Rdaddr,&runlog,sizeof(runlog),FLASHBUFFER1);
                }
                Rdaddr[0]=0xff;
                Rdaddr[1]=0xf0;
                Rdaddr[2]=0;
                FlashBuffertoMemory(Rdaddr,FLASHBUFFER1);
                
                //Sighisdata.page =0;
                //FlashBufferWrite(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1);
		AT24C64_W(checkcode,0x00,6);


	}
	else
	{
      //用户参数在Flash第1页 页地址0x00;	
	//------------------------------------------------
    Delay_N_mS(20000);
		AT24C64_RS(parameter.sys_setting,0x10,ParaNum);

		for(count=0;count<8;count++)
		{
			//AD_parameter.AD_i[x].rate.b=(AD_parameter.AD_i[x].ADUV.b-AD_parameter.AD_i[x].ADLV.b)/0x1000;
			if(ADPARA(count).AI_Type==2)
			#ifdef ADSTDV_254
				ADPARA(count).rate.b=(ADPARA(count).ADUV.b-ADPARA(count).ADLV.b)/0x66C;
			#else
				ADPARA(count).rate.b=(ADPARA(count).ADUV.b-ADPARA(count).ADLV.b)/0x666;
			#endif
			else
				ADPARA(count).rate.b=(ADPARA(count).ADUV.b-ADPARA(count).ADLV.b)/0x1000;
		}
		for(checkcode[0]=0;checkcode[0]<6;checkcode[0]++)
		for(count=0;count<16;count++)
		{
			if(SPARA.PO_SerialNum[checkcode[0]][count]!=0xff)
			{
				Channel_Num[checkcode[0]]++;
			}
			else
				break;
				
		}
                
          if(SPARA.PO_Type[0]==1)
          {
            Channel_Num[0]=1;
          }
          else
          {
            Channel_Num[0]=0;
          }
    //SPARA.PO_Type[0]=1; //水,哈希
			
          SPARA.PO_SerialNum[0][0]=0x05;
	}
  /*-----------------------------------------------------------------------
                          IS752 INITIAL
  ------------------------------------------------------------------------*/
   IS752_initial_1(SPARA.Serial_Baud[1],SPARA.Serial_Baud[2]);
   IS752_initial_2(SPARA.Serial_Baud[3],SPARA.Serial_Baud[4]);
   Delay_N_mS(20000);

  Symbol.SymAll=0;
  Symbol.stop_watch=1;  //使能上传实时数据
	Symbol.dealcommand=0;
	
	for(count=0;count<6;count++)
	{
		LCD_user_pw[count]=parameter.sys_parameter.pw[count];		
	}
  for(count=0;parameter.sys_parameter.rd_interval[count]!='\0'&&count<3;count++);
	switch(count)
	{
		case 1:
			second=(int)(parameter.sys_parameter.rd_interval[0]&0xf);
		break;
		case 2:
			second=((int)(parameter.sys_parameter.rd_interval[0]&0xf))*10+(int)(parameter.sys_parameter.rd_interval[1]&0xf);
		break;
		case 3:
			second=((int)(parameter.sys_parameter.rd_interval[0]&0xf))*100+((int)(parameter.sys_parameter.rd_interval[1]&0xf))*10+(int)(parameter.sys_parameter.rd_interval[2]&0xf);
		break;
		default:
		break;
	}

  count=0;
	checknum[6]=2;
	System_Time[14]=0;
	v_Get1302();
	old__hour=current_hour;
  old__day=current_date;
  
  run_log_init(&RunLog);
        
        
  /***************************************************************
                      LCD initializing
  ***************************************************************/
  index_initial();//LCD 初始化
	Symbol.LCD_deal=1;
	LCD_page=10;
	Symbol.LCD_Page_Change=1;
  Symbol.Mod_checkover=1;
  Symbol.Pass_id=1;
	disp10();

  count=0;
  temp  =DIInput();//读入DI值
  RealData.R_D.DI[0]=(INT8U)(temp>>8);
  RealData.R_D.DI[1]=(INT8U)temp;
  b01h_cou =0;
  b01d_cou =0;
  _BIS_SR(GIE);//enable the interrupt
  /***************************************************
                    main loop
  ***************************************************/
  while(1)
  {
    Prog_Circle++;
    old__second=current_second;
    //	;
    if(Symbol.dealcommand)		//处理接收上位机命令
    {
      _BIC_SR(GIE);
      dealdata_wjj();							//	命令处理
      _BIS_SR(GIE);
      Symbol.dealcommand=0;							//  命令处理完成标志为0
      Receive_Package_length=0;				//接收数据包的长度
      Receive_Data_length=0;					//接收数据总长度
      Receive_Package_length=0;				//接收数据包的长度
			//EIE2|=0x1;//EA=1;
    }
    if(Prog_Circle%10==0)
    {
      v_Get1302();
      if(old__day!=current_date)
      {
        yestoday = old__day;
        old__day =current_date;
      }
    }

    if(old__second!=current_second)
    {
      INT8U kkk;
      for(kkk=0;kkk<8;kkk++)
      {
        if(ADPARA(kkk).po_id[0]=='B')
        {
          b01h_cou +=RealData.R_D.FF.ADF[kkk]/3600;
          //local_mma_H[kkk].max
          ADsimulate();
          break;
        }
      }
      //b01h_cou +=RealData.R_D.FF.ADF[i]/3600;
      count++;
      if(count>999)
              count=0;
      if(RUN)
      {
        RUN_ON;
        RUN=0;
      }
      else
      {
        RUN=1;
        RUN_OFF;
      }
      if((current_minute==0x00)&&(current_hour==0x00)&&(current_second==0x00))
      {
        Rpthisdata.sddaydata  =1;
      }
      if((current_minute==0x00)&&(current_hour==0x00)&&(current_second==0x05))
      {
        GPRS_POWEROFF;
        //Save_Daydata  =1;
        //Rpthisdata.sddaydata  =1;
      }
      if((current_minute==0x00)&&(current_hour==0x00)&&(current_second==0x06))
      {
        GPRS_POWERON;
      }
      //-------------------------------------------------------
      //-----------------查询仪表实时数据----------------------
      /*if((second-count)==0)
      {
        //if(Symbol.Mod_checkover==1)
        {
          Symbol.Mod_ontime=1;
          Mod_method=Mod_RealD;
          Symbol.Mod_checkover=0;
        }
      }*/
	
      /*if(((second-count)>59)&&((second-count)<=62))
      {
        if(Symbol.Mod_checkover==2)
          Symbol.Mod_checkover=1;

        //-------------------------------------------------------------------
        //---------------------读取历史数据----------------------------------
        if((Symbol.Mod_checkover!=0)&&(Symbol.Mod_symbol==0))
        {
          if(minute_counter>=60)
          {
            minute_counter=0;
            Mod_method=Mod_HourD;
            Symbol.Hour_data=1;
          }

          if((Symbol.Hour_data!=1)&&(hour_counter==24))
          {
            hour_counter=0;
            Mod_method=Mod_DayD;
            Symbol.Day_data=1;						
          }
          if((Symbol.Day_data)||(Symbol.Hour_data))
          {
            Symbol.Mod_ontime=1;
            Symbol.Mod_checkover=0;						
          }
        }
      }*/
			
      //------------LCD数据更新-----------------------------
      //---------------------------------------------
      if((Symbol.LCD_deal==0)&&(LCD_page==10))
      {
              Symbol.LCD_T=1;
              Symbol.LCD_deal=1;
      }
      else
              Symbol.LCD_T=0;
      //--------------------------------------------
      //-----------------LCD背光控制--------------

      if(SPARA.LCD_Backtime!=0)
        if(Symbol.Pass_id)
        {
          LCD_light++;
          if(SPARA.LCD_Backtime==1)
                  checkcode[0]=120;
          else
                  checkcode[0]=240;
          if(LCD_light>=checkcode[0])
          {
                  LCD_light=0;
                  Symbol.Pass_id=0;
                  LCD_light_OFF;		
                  Symbol.LCD_deal=1;
                  LCD_page=10;
                  Symbol.LCD_Page_Change=1;
          }
        }
      }
    //---------------------------------------------------------------
    //---------------Modbus 数据读取与处理---------------------------
    if(Symbol.Mod_Device_Check)
    {
      Mod_method  =Mod_RealD;
      Mod_Command(Mod_method);
      Symbol.Mod_Device_Check=0;
    }
    if(Symbol.COM2orCOM3)
    {
      Symbol.COM2orCOM3=0;
      Task_ReDataFrom752_1();
    }
    if(Symbol.COM4orCOM5)
    {
      Symbol.COM4orCOM5=0;
      Task_ReDataFrom752_2();
    }

    if(COMSymbol.COM5DataReady)
    {
      Symbol.Mod_symbol=0;
      COMSymbol.COM5DataReady=0;
      Mod_dealdata(COM5_Buffer);
    }
    if(COMSymbol.COM2DataReady)
    {
      COMSymbol.COM2DataReady=0;
      Symbol.Mod_symbol=0;
      if((COM2_Buffer[0]==0x05))
      {
        POdata_Receive|=(0x01<<POdata_Type);
        Mod_dealdata(COM2_Buffer);
      }
    }
    if(COMSymbol.COM3DataReady)
    {
      COMSymbol.COM3DataReady=0;
      Symbol.Mod_symbol=0;
      if((COM3_Buffer[0]==0x06))
      {
        POdata_Receive|=(0x01<<POdata_Type);
        Mod_dealdata(COM3_Buffer);
      }
    }
    /*if(Symbol.Mod_complete)
    {
      Symbol.Mod_complete=0;
      ModataUpload(Mod_method);
    }*/

    //----------------------------------------------------------------
    //---------------------检测GPRS是否在线---------------------------
    if(old__minute!=current_minute)	//GPRS检测
    {
            minute_counter++;
            old__minute=current_minute;
            if(current_minute%10==0)
              Rpthisdata.sdminutedata =1;
            if((GPRS_ONLINE))
                    GPRS_RST;
    }
    //   00:00:00 给GPRS断电,再上电


    //----------------------------------------------------------------
    if(old__hour!=current_hour)
    {
      //hour_counter++;
      //Save_Hourdata=1;
      Rpthisdata.sdhourdata  =1;//enable sending hour his_data
      old__hour=current_hour;
      b01d_cou +=b01h_cou;
    }
    if(AD_CONVERTER)
    {	
      AD_CONVERTER=0;
      //AD_Convert();
      AD_READ();			//读AD转换数据
      ADC12CTL0|=ADC12SC;
		
      //if(Symbol.LCD_AD_Update)
      if((Prog_Circle%1000)==0)
      {
        //Symbol.LCD_AD_Update=0;
        //-----------------------------------------------------------------------------------------------
        ADCalculation();
        if((Symbol.LCD_deal==0)&&(LCD_page==43))	//更新LCD AD数据
        {
          Symbol.LCD_AD=1;
          Symbol.LCD_deal=1;
          //ADCalculation();
        }
      }
      /*if((Prog_Circle%10000)==0)
      {
        ADCalculation();
        //ReportAlarm();
      }*/
    }
    oldDI=(RealData.R_D.DI[0]<<8)|RealData.R_D.DI[1];
    //---------------------------------------------------------------
    //-----------------------DI状态查询------------------------------
    if((Prog_Circle%5)==0)
    {
      temp=DIInput();		//查询DI状态
      RealData.R_D.DI[0]=(INT8U)(temp>>8);
      RealData.R_D.DI[1]=(INT8U)temp;
    }
    //----------------------DI15,16互锁------------------------------
    if(Symbol.DO5_DI15)
    {
      if((temp&0xc000)==0x4000)
      {
        Symbol.DO5_DI15=0;
        OldDO=RealData.R_D.DO;
        RealData.R_D.DO&=0xef;//清DO5
        DOOutput(RealData.R_D.DO);
        if(Symbol.LCD_Control==0)
        {
          e_result[0]='1';
          Response(2);
        }
        else
          Symbol.LCD_Control=0;
        DI_Convert=0;
        if((LCD_page==41)||(LCD_page==90))
        {
          Symbol.LCD_DO=1;
          Symbol.LCD_deal=1;
        }
      }
      else
      {	
        DI_Convert++;
        if(DI_Convert>=4500)
        {
          Symbol.DO5_DI15=0;
          DI_Convert=0;
          if(Symbol.LCD_Control==0)
          {
            e_result[0]='2';
            Response(2);
          }
          else
            Symbol.LCD_Control=0;
          OldDO=RealData.R_D.DO;
          RealData.R_D.DO&=0xef;//清DO5
          DOOutput(RealData.R_D.DO);
          if((LCD_page==41)||(LCD_page==90))
          {
            Symbol.LCD_DO=1;
            Symbol.LCD_deal=1;
          }
        }				
      }
    }
    if(Symbol.DO6_DI16)
    {
      if((temp&0xc000)==0x8000)
      {
        Symbol.DO6_DI16=0;
        OldDO=RealData.R_D.DO;
        RealData.R_D.DO&=0xdf;//清DO6
        DOOutput(RealData.R_D.DO);
        if(Symbol.LCD_Control==0)
        {
          e_result[0]='1';
          Response(2);
        }
        else
          Symbol.LCD_Control=0;	
        DI_Convert=0;
        if((LCD_page==41)||(LCD_page==90))
        {
          Symbol.LCD_DO=1;
          Symbol.LCD_deal=1;
        }
        //----------------------------------------------------------
        //------------------DI15,16逢变则报-------------------------
        //DI_Alter=1;
      }
      else
      {	
        DI_Convert++;
        if(DI_Convert>=4500)
        {
          Symbol.DO6_DI16=0;
          DI_Convert=0;
          if(Symbol.LCD_Control==0)
          {
            e_result[0]='2';
            Response(2);
          }
          else
            Symbol.LCD_Control=0;
          OldDO=RealData.R_D.DO;
          RealData.R_D.DO&=0xdf;//清DO6
          DOOutput(RealData.R_D.DO);
          if((LCD_page==41)||(LCD_page==90))
          {
            Symbol.LCD_DO=1;
            Symbol.LCD_deal=1;
          }
        }				
      }
    }

    if(temp!=oldDI)
    {
      if((temp&0x3fff)!=(oldDI&0x3fff))
      {
        DI_Alter=1;
      }
      else
      {
        if((temp&0xc000)!=0x0000)
          DI_Alter=1;
      }
      if((Symbol.LCD_deal==0)&&(LCD_page==42))
      {
        Symbol.LCD_DI=1;
        Symbol.LCD_deal=1;
      }
      else
        Symbol.LCD_DI=0;
    }

    if(DI_Alter)
    {
      DI_Alter=0;
      for(checknum[3]=0;checknum[3]<2;checknum[3]++)
      {
        checknum[5]=RealData.R_D.DI[checknum[3]];//InState[i];
        checknum[4]=(checknum[5]>>4)&0x0f;
        if(checknum[4]<10)
        {
          d[2*checknum[3]]=checknum[4]|0x30;		//输入状态
        }
        else
        {
          d[2*checknum[3]]=(checknum[4]-9)+0x40;		//输入状态
        }
        checknum[4]=RealData.R_D.DI[checknum[3]]&0x0f;
        if(checknum[4]<10)
        {
          d[2*checknum[3]+1]=checknum[4]|0x30;		//输入状态
        }
        else
        {
          d[2*checknum[3]+1]=(checknum[4]-9)|0x40;		//输入状态
        }
      }
      d[4]='\0';
      Symbol.ccc=1;
      Response(5);
      Symbol.ccc=0;

      strcat(CBAG.cbag_dynamic,DI_C);

      strcat(CBAG.cbag_dynamic,d);

      Add_Send();
    }
    //------------------------------------------------------
    /*-------------数据上报及保存-------------------------*/
    if(Symbol.rd_change)
    {
      Symbol.rd_change=0;
      count=0;
      for(checknum[2]=0;parameter.sys_parameter.rd_interval[checknum[2]]!='\0'&&checknum[2]<3;checknum[2]++);
      switch(checknum[2])
      {
        case 1:
          second=(int)(parameter.sys_parameter.rd_interval[0]&0xf);
        break;
        case 2:
          second=((int)(parameter.sys_parameter.rd_interval[0]&0xf))*10+(int)(parameter.sys_parameter.rd_interval[1]&0xf);
        break;
        case 3:
          second=((int)(parameter.sys_parameter.rd_interval[0]&0xf))*100+((int)(parameter.sys_parameter.rd_interval[1]&0xf))*10+(int)(parameter.sys_parameter.rd_interval[2]&0xf);
        break;
        default:
        break;
      }
    }

    if(count>=second)
    {		
      count=0;
      {
        run_log_record(&RunLog);
        _BIC_SR(GIE);
        DevCalculation();
        
        if( Symbol.stop_watch==1)
        {
                collect_realdata(Save_Hourdata,Save_Daydata);
                /*INT16U tempnum_ch=0;
                for(i=0;i<6;i++)
                  tempnum_ch  +=Channel_Num[i];
                if(tempnum_ch!=0)
                  ModataUpload(Mod_method);*/
        Save_Hourdata=0;
        if(Save_Daydata)
        Save_Daydata  =0;
        }
        if(stop_watch_devstatus==1)
        {
          Device_Status();
        }
        _BIS_SR(GIE);
      }	
    }
    if(Rpthisdata.sdhourdata) //report hour his_data
    {
      Rpthisdata.sdhourdata =0;
      _BIC_SR(GIE);
      Reporthisdata_H();
      _BIS_SR(GIE);
    }
    if(Rpthisdata.sddaydata)  //report day his data
    {
      Rpthisdata.sddaydata  =0;
      _BIC_SR(GIE);
      Reporthisdata_D();
      _BIS_SR(GIE);
    }
    //if((current_minute%10==0)&&(second==30))
    //if((Rpthisdata.sdminutedata)&&(second==30)) //wen 2010-03-12
    if(Rpthisdata.sdminutedata)   //wen 2010-03-12
    {
      Rpthisdata.sdminutedata =0;
      _BIC_SR(GIE);
      Reporthisdata_M();
      _BIS_SR(GIE);
    }
    //---------------------------------------------------------------
    //---------------------读取键盘值--------------------------------
    if(Prog_Circle%400==0)
            KeyV=getkey();
    if((KeyV!=0)||(Symbol.LCD_deal))		//LCD 处理
    {
            LCD_disp();
    }
    if(Symbol.LCD_para_set)
    {
            Symbol.LCD_para_set=0;
            _BIC_SR(GIE);
            AT24C64_W(parameter.sys_setting,0x10,ParaNum);
            _BIS_SR(GIE);
    }
    //---------------LCD复位-------------
    if(Screen_RST)
    {
       Screen_RST=0;
       LCD_RSTime=0;			
       init_lcd ();
    }
    //----------------------------
    if((Symbol.ADP_change)&&((LCD_page/10)!=7))
    {
      Symbol.ADP_change=0;
      for(i=0;i<8;i++)
      {		
        if(ADPARA(i).AI_Type==2)
        #ifdef ADSTDV_254
                ADPARA(i).rate.b=(ADPARA(i).ADUV.b-ADPARA(i).ADLV.b)/0x66C;
        #else
                ADPARA(i).rate.b=(ADPARA(i).ADUV.b-ADPARA(i).ADLV.b)/0x666;
        #endif
        else
                ADPARA(i).rate.b=(ADPARA(i).ADUV.b-ADPARA(i).ADLV.b)/0x1000;
      }
    }
    //------------------------------------------------------
    //-----------------------仪表反控-----------------------
    if((Symbol.Mod_checkover!=0)&&(Symbol.Mod_symbol==0))
    {
      if(Devctr.calibrate==1)
      {

        Devctr.calibrate  =0;
        //Symbol.Mod_checkover=0;
        dev_calibrate_sample('c');

      }
      if(Devctr.sample==1)
      {
        Devctr.sample =0;
        //Symbol.Mod_checkover=0;
        dev_calibrate_sample('s');
      }
      if(Devctr.operate==1)
      {
        Devctr.operate  =0;
        //Symbol.Mod_checkover=0;
        dev_operate();
      }
      if(Devctr.settimes==1)
      {
        Devctr.settimes =0;
        //Symbol.Mod_checkover=0;
        dev_settimes();
      }
    }

  }
}

/*******************************************
          AD interrupt function
*******************************************/
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR(void)
{
  ADResult[0][2*ADTimes]=ADC12MEM0;
  ADResult[0][2*ADTimes+1]=ADC12MEM1;
  ADResult[1][2*ADTimes]=ADC12MEM2;
  ADResult[1][2*ADTimes+1]=ADC12MEM3;
  ADResult[2][2*ADTimes]=ADC12MEM4;
  ADResult[2][2*ADTimes+1]=ADC12MEM5;
  ADResult[3][2*ADTimes]=ADC12MEM6;
  ADResult[3][2*ADTimes+1]=ADC12MEM7;
  ADResult[4][2*ADTimes]=ADC12MEM8;
  ADResult[4][2*ADTimes+1]=ADC12MEM9;
  ADResult[5][2*ADTimes]=ADC12MEM10;
  ADResult[5][2*ADTimes+1]=ADC12MEM11;
  ADResult[6][2*ADTimes]=ADC12MEM12;
  ADResult[6][2*ADTimes+1]=ADC12MEM13;
  ADResult[7][2*ADTimes]=ADC12MEM14;
  ADResult[7][2*ADTimes+1]=ADC12MEM15;

  ADTimes++;
  if(ADTimes>=5)
  {
    ADTimes=0;
    AD_CONVERTER=1;
  }
}
/*******************************************
              AD Converter
*******************************************/
void AD_READ(void)
{
  INT8U i,j;
  INT16U *max,*min,*mid;

  for(i=0;i<8;i++)
		{
      ADValue[i]=0;
			max=&ADResult[i][0];
			min=max;
			mid=&ADResult[i][1];
			for(j=0;j<9;j++)           //查找最高最低值
			{
				if(*max<*mid)
			        max=mid;
				if(*min>*mid)
					min=mid;
				mid=&ADResult[i][j+2];
			}
			*max=0;      //去掉最高值
			*min=0;      //去掉最低值
			for(j=0;j<10;j++)
				ADValue[i]+=ADResult[i][j];
			ADValue[i]=(ADValue[i]>>3);   //取平均值
      if(ADValue[i]<=0x100)
        ADValue[i]-=2;
      else
        ADValue[i]+=3;
		}
}
void ADCalculation(void)
{
  INT8U i;
  INT16U AD_f;
  float AD_k;
  for(i=0;i<8;i++)
  {
    AD_f=ADValue[i];
    switch(ADPARA(i).AI_Type)
    {
      case 0:
        AD_k=ADPARA(i).rate.b*AD_f+ADPARA(i).ADLV.b;	
      break;
      case 1:
        AD_k=ADPARA(i).rate.b*AD_f*2+ADPARA(i).ADLV.b;
      break;
      case 2:
        #ifdef ADSTDV_254
        if(AD_f<0x193)
          AD_f=0x193;
        AD_k=ADPARA(i).rate.b*(AD_f-0x193)+ADPARA(i).ADLV.b;
        #else
        if(AD_f<0x199)
          AD_f=0x199;
        AD_k=ADPARA(i).rate.b*(AD_f-0x199)+ADPARA(i).ADLV.b;
        #endif
      break;
      default:
      break;
    }
    if(AD_k>ADPARA(i).ADUV.b)
      RealData.R_D.FF.ADF[i]=ADPARA(i).ADUV.b;
    else
      RealData.R_D.FF.ADF[i]=AD_k;
    
      //local_mma_H[i].max
  }
}
void ADsimulate(void)
{
  INT16U i;
  for(i=0;i<8;i++)
  {
    if(local_mma_M[i].num==0)
    {
      local_mma_M[i].max  =local_mma_M[i].min =local_mma_M[i].sum =RealData.R_D.FF.ADF[i];
      local_mma_M[i].num++;
    }
    else
    {
      if(RealData.R_D.FF.ADF[i]<local_mma_M[i].min)
        local_mma_M[i].min  =RealData.R_D.FF.ADF[i];
      if(RealData.R_D.FF.ADF[i]>local_mma_M[i].max)
        local_mma_M[i].max  =RealData.R_D.FF.ADF[i];
      local_mma_M[i].sum  +=RealData.R_D.FF.ADF[i];
      local_mma_M[i].num++;
    }
    
    if(local_mma_H[i].num==0)
    {
      local_mma_H[i].max  =local_mma_H[i].min =local_mma_H[i].sum =RealData.R_D.FF.ADF[i];
      local_mma_H[i].num++;
    }
    else
    {
      if(RealData.R_D.FF.ADF[i]<local_mma_H[i].min)
        local_mma_H[i].min  =RealData.R_D.FF.ADF[i];
      if(RealData.R_D.FF.ADF[i]>local_mma_H[i].max)
        local_mma_H[i].max  =RealData.R_D.FF.ADF[i];
      local_mma_H[i].sum  +=RealData.R_D.FF.ADF[i];
      local_mma_H[i].num++;
    }
    if(local_mma_D[i].num==0)
    {
      local_mma_D[i].max  =local_mma_D[i].min  =RealData.R_D.FF.ADF[i];
      //local_mma_D[i].num++;
    }
    else
    {
      if(RealData.R_D.FF.ADF[i]<local_mma_D[i].min)
        local_mma_D[i].min  =RealData.R_D.FF.ADF[i];
      if(RealData.R_D.FF.ADF[i]>local_mma_D[i].max)
        local_mma_D[i].max  =RealData.R_D.FF.ADF[i];
      //local_mma_D[i].sum  +=RealData.R_D.FF.ADF[i];
      //local_mma_D[i].num++;
    }
  }
}
/****************************************
                DO OUTPUT
*****************************************/
void DOOutput(INT8U dout)
{
    CS_4245_EN;
    P1DIR=0XFF;   //direction output
    DO_ENABLE;
    Delay_N_mS(2);
    DOOUT =IOOUTPUTs[dout];
    Delay_N_mS(2);
    DO_DISABLE;
    CS_4245_DIS;
    P1DIR=0X00;//direction input
}
/****************************************
              DI INPUT
*****************************************/
INT16U DIInput(void)
{
    INT16U DValue;
    DI1_ENABLE;
    DValue  =DIN;
    DI1_DISABLE;
    Delay_N_mS(2);
    DI2_ENABLE;
    DValue  |=((INT16U)DIN)<<8;
    DI2_DISABLE;
    DValue=~DValue;
    return(DValue);
}


