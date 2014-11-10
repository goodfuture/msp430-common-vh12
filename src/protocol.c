#include <msp430xG46x.h>
#include "test.h"
#include <string.h>
#include <stdlib.h>

INT16U Alarmtimes=0;      //������¼����

//static struct _hisdata Sighisdata;

const INT8U keyb[6][5]={"QN","ST","CN","PW","MN","Flag"};
INT8U ml_qn[18]="12345678901234567\0";               /*���������*/
INT8U ml_mn[15]="12345678901234\0";                   /*�����洢ϵͳΨһ��ʶ*/
INT8U ml_st[3]="00\0";
INT8U ml_cn[5]="0000\0";
INT8U ml_flag;
INT8U Rt[2]="1\0";
INT8U e_result[4]="1\0\0\0";                //ִ�з��ؽ��,1��ʾ�ɹ�,2��֪��ԭ��,100������
INT8U LCD_user_pw[7];

struct ml_cnb           /*������cn�Ľṹ�壬����pΪ��Ӧ��������Ӻ������ָ�룬codΪ�������洢����*/
       {
          //void  (*subcn)();
          char  cod[5];
		  int  list;
       };
struct ml_stb            /*ϵͳ���st�Ľṹ�壬����qΪ��Ӧ�����Ӻ������ָ�룬codΪ����洢����*/
       {
          char (*subst)();
          char cod[3];
       };
struct ml_stb st[5];
struct intercp
	{
		char D_intercp[20];
		char K_intercp[20];
	};

struct intercp interparametre[25];


COMBAG CBAG={{"##"},{"\0"},{"ST=00;"},{"CN=0000;"},{"PW=123456;"},{"MN=12345678901234;"},{"\0"},{'0','0','0','0',0x0d,0x0a}};
INT8U cbag_cp[6]="CP=&&\0";
INT8U cbag_cptail[3]="&&\0";
INT8U semicolon[2]=";\0";
INT8U comma[2]=",\0";

const struct ml_cnb cn[40]={{/*set_overtime_resendtime,*/"1000\0",0},{/*set_warntime,*/"1001\0",1},
						  {/*collect_systemtime,*/"1011\0",2},{/*set_systemtime,*/"1012\0",3},
						  {/*collect_P_alarmlimt,*/"1021\0",4},{/*set_P_alarmlimt,*/"1022\0",5},
						  {/*collect_alarmtarget,*/"1031\0",6},{/*set_alarmtarge,*/"1032\0",7},
						  {/*collect_reporttime,*/"1041\0",8},{/*set_reporttime,*/"1042\0",9},
						  {/*collect_realdata_interval,*/"1061\0",10},{/*set_realdata_interval,*/"1062\0",11},
						  {/*set_password,*/"1072\0",12},{"9013\0",13},
						  {"9014\0",14},{"2011\0",15},
						  {/*notice_response,*/"2012\0",16},{"2021\0",17},
						  {/*notice_response,*/"2022\0",18},{"2031\0",19},
						  {"2041\0",20},{"2051\0",21},
						  {"2061\0",22},{"2071\0",23},
						  {"3011\0",24},{"3012\0",25},
						  {"3013\0",26},{"3014\0",27},
						  {"9018\0",28},{"1002\0",29},
              {"1003\0",30},{"3030\0",31},
              {"2081\0",32},{"2091\0",33},
                         };

void Delay20ms()
{
	unsigned int  i;
	unsigned int j;
	for(i=0;i<30;i++)
		for(j=0;j<3000;j++)	;	
}
/*-----------------------------------------------------------------
** ��������:    insert
** ��������:    ��һ���ַ������뵽��һ�ַ�����ָ��λ��
** �䡡��:      des:   �����봮�׵�ַ
**              src:   ���봮�׵�ַ
**              pos: ����λ��
** ����ֵ��     ��
** ������: �½���
** �ա���: 2007-08-20
**-----------------------------------------------------------------*/
void insert(INT8U *des,const void *src,INT16U pos) //reentrant
{
    INT16U lens,lent,i;       /*lens�Ǳ����봮��,lent�ǲ��봮��*/
    INT8U *p,*q,*r;         /*p�Ǳ����봮ָ��,q�ǲ���λ��ָ��,r�ǲ��봮ָ��*/
    p=des;
    q=p+pos-1;
    r=(INT8U*)src;
    lens=strlen(des);
    lent=strlen(src);
    for(p=p+lens;p>=q;p--)   /*�����봮��pos����Ӵ�����,�ճ����봮��λ��*/
        *(p+lent)=*p;
    for(i=1;i<=lent;i++)     /*�������뵽posλ����*/
    {
        *(p+1)=*r;
        p++;
        r++;		
    }
}

/*-----------------------------------------------------------------
** ��������: 	separation
** ��������: 	��cp���⣬����ؼ��ּ����Ӧ����������
** �䡡��: 		strinput: �������ݶ��׵�ַ
** ����ֵ��     ��
** ������: �½���
** �ա���: 2007-08-17
**-----------------------------------------------------------------*/
void separation(INT8U *strinput)
{

    INT8U str1[]="&&";
    INT8U *pointer1;
    INT8U *q;
    INT8U *m;
    //INT8U *w=strinput;
    INT16U k,i;
    INT8U key[5];
    INT8U dat1[25];                 /* Ӧ�ö����ȫ�ֵı���*/
                                                 /***********************/
    INT8U *sy;

    sy=strstr(strinput,str1);              /*�ҵ���һ��&���ֵ�λ�ã�*/
    for(pointer1=strinput,q=pointer1;pointer1<=sy-3&&q<=sy-3;)
    {
        if((*pointer1)!='=')pointer1++;
        else
        {
            for(m=q,k=0;m<pointer1;m++)   /* ȡ�ؼ��֣���������key[5]��*/
            {
            key[k]=*m;
            k++;
            }
            key[k]='\0';    /*�ѹؼ���ʣ��ռ�����*/
            for(q=pointer1+1;q<=sy-2&&(*(q-1))!=';';)   /* ȡ��������룬��������dat[25]�� */
            {
                if((*q)!=';')q++;
                else
               {
                    for(m=pointer1+1,i=0;m<q;m++)
                    {
                        dat1[i]=*m;
                        i++;

                    }
                    dat1[i]='\0';
                    q++;
               }
            }
            pointer1=q;
            command_store(dat1,key);
      }

    }

}

/*-----------------------------------------------------------------
** ��������: 	command_store
** ��������: 	��cp���⣬����ؼ��ּ����Ӧ�������ķ���洢
** �䡡��: 		dat: �������洢�׵�ַ
**              key: �ؼ��ִ洢�׵�ַ
** ����ֵ��     ��
** ������: �½���
** �ա���: 2007-08-18
**-----------------------------------------------------------------*/

void command_store(INT8U *dat1,INT8U *key)
{
    INT16U n,s;
    for(s=0,n=1;n!=0&&s<=5;s++)
    {
      n=strcmp(keyb[s],key);
    }
    switch(s)                                                /*��ؼ��ֶ�Ӧ��������������Ӧ������*/
    {
      case 1:    strcpy(ml_qn,dat1);
      break;

      case 2:    strcpy(ml_st,dat1);
      break;

      case 3:    strcpy(ml_cn,dat1);
      break;

      case 4:    if(strcmp(LCD_user_pw,dat1)!=0)
                      Rt[0]='3';
                 else Rt[0]='1';             /*dat�еĴ����뱾���洢��pw�������ƥ��*/
      break;

      case 5:    //strcmp(ml_mn,dat1);             /*dat�еĴ����뱾���洢��mn����ƥ��*/
      break;

      case 6:    ml_flag=dat1[0];                /*�жϲ�ִ��flag����������Ӧ�Ĳ���*/
      break;

      default:   //printf("error");
      break;

    }

}

/*-----------------------------------------------------------------
** ��������: 	request_response
** ��������: 	�ֳ�������Ӧ�����ݶε�����
** �䡡��: 		Rt:	����Ӧ�𷵻�ֵ�ַ����洢�׵�ַ
** ��  ����     ��
** ������: �½���
** �ա���: 2007-08-18
**-----------------------------------------------------------------*/
void Response(INT8U type)//0 request_response 1 notice_response 2 execution_return 4 ʵʱ���� 5 �����
{
  INT8U i;
  INT8U req1[]="QN=\0";
  INT8U req2[]=";QnRtn=\0";
  INT8U req3[]=";ExeRtn=\0";
  INT8U c[8]="Flag=0;\0";
  for(i=0;i<6;i++)
          //CBAG.cbag_PW[3+i]=LCD_user_pw[i];
    CBAG.cbag_PW[3+i]=parameter.sys_parameter.pw[i];
  for(i=0;i<14;i++)
    CBAG.cbag_MN[3+i]=parameter.sys_parameter.mn[i];
  strcat(CBAG.cbag_dynamic,cbag_cp);

  if(Symbol.ccc==0)
  {
    strcat(CBAG.cbag_dynamic,req1);
    strcat(CBAG.cbag_dynamic,ml_qn);
  }
  switch(type)
  {
    case 0:
      CBAG.cbag_ST[3]='9';
      CBAG.cbag_ST[4]='1';
      CBAG.cbag_CN[3]='9';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='1';
      CBAG.cbag_CN[6]='1';

      insert(CBAG.cbag_dynamic,c,1);
      strcat(CBAG.cbag_dynamic,req2);

      strcat(CBAG.cbag_dynamic,Rt);
      Add_Send();
    break;
    case 1:
      CBAG.cbag_ST[3]='9';
      CBAG.cbag_ST[4]='1';
      CBAG.cbag_CN[3]='9';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='1';
      CBAG.cbag_CN[6]='3';
      Add_Send();
    break;
    case 2:
      CBAG.cbag_ST[3]='9';
      CBAG.cbag_ST[4]='1';
      CBAG.cbag_CN[3]='9';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='1';
      CBAG.cbag_CN[6]='2';
      strcat(CBAG.cbag_dynamic,req3);
      strcat(CBAG.cbag_dynamic,e_result);
      Add_Send();
    break;
    case 4:
      for(i=0;i<2;i++)
      {
              CBAG.cbag_ST[3+i]=parameter.sys_parameter.st[i];
      }
      CBAG.cbag_CN[3]='2';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='1';
      CBAG.cbag_CN[6]='1';
    break;
    case 5:
      for(i=0;i<2;i++)
      {
              CBAG.cbag_ST[3+i]=parameter.sys_parameter.st[i];
      }
      CBAG.cbag_CN[3]='3';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='1';
      CBAG.cbag_CN[6]='5';
    break;
    case 8:
      for(i=0;i<2;i++)
      {
        CBAG.cbag_ST[3+i]=parameter.sys_parameter.st[i];
      }
      CBAG.cbag_CN[3]='2';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='2';
      CBAG.cbag_CN[6]='1';
    break;
    case 10:  //Сʱ��ʷ����
      for(i=0;i<2;i++)
      {
        CBAG.cbag_ST[3+i]=parameter.sys_parameter.st[i];
      }
      CBAG.cbag_CN[3]='2';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='6';
      CBAG.cbag_CN[6]='1';
    break;
    case 11:  //����ʷ����
      for(i=0;i<2;i++)
      {
        CBAG.cbag_ST[3+i]=parameter.sys_parameter.st[i];
      }
      CBAG.cbag_CN[3]='2';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='3';
      CBAG.cbag_CN[6]='1';
    break;
    case 12:
      for(i=0;i<2;i++)
      {
        CBAG.cbag_ST[3+i]=parameter.sys_parameter.st[i];
      }
      CBAG.cbag_CN[3]='2';
      CBAG.cbag_CN[4]='0';
      CBAG.cbag_CN[5]='5';
      CBAG.cbag_CN[6]='1';
    break;
    default:
      for(i=0;i<2;i++)
      {
        CBAG.cbag_ST[3+i]=parameter.sys_parameter.st[i];
      }
      //if(ccc==0)
      for(i=0;i<4;i++)
      {
        CBAG.cbag_CN[3+i]=ml_cn[i];
      }	
    break;
  }	
}
/*-----------------------------------------------------------------
** ��������:    add_head&end
** ��������:    ����������ͨѶ��
** �䡡��:      crc:          crcУ�����ַ����׵�ַ
** ����ֵ��     c_bag:        ���ɵ�ͨѶ���׵�ַ
** ������: �½���
** �ա���: 2007-08-21
**-----------------------------------------------------------------*/

//void Add_Send()
void add_send()
{
  INT16U length;
  INT8U i;
  INT8U CRC16[2];
  INT8U crc[4]="\0\0\0\0";
  strcat(CBAG.cbag_dynamic,cbag_cptail);
  length=strlen(CBAG.cbag_ST);
  CRC_Calculation(CBAG.cbag_ST,length,CRC16);
  StrToASCII(CRC16,crc,2);
  for(i=0;i<4;i++)
  {
    CBAG.cbag_crc_tail[i]=crc[i];
  }
  CBAG.cbag_length[0]=(length/1000)|0x30;         //��lengthת�����ַ�����������len[5]��
  CBAG.cbag_length[1]=((length%1000)/100)|0x30;
  CBAG.cbag_length[2]=((length%100)/10)|0x30;
  CBAG.cbag_length[3]=(length%10)|0x30;
  SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
  SendData(CBAG.cbag_crc_tail,6,COM1);
  //MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
  //CBAG.cbag_dynamic[0]='\0';
			
}
/*-----------------------------------------------------------------
** ��������: 	cp_analyze
** ��������: 	���ݶ�cp���йؼ��ּ����Ӧ�������ķ���
** �䡡��: 		strinput�� �������ݶ��׵�ַ
** ����ֵ��     ��
** ������: �½���
** �ա���: 2007-08-18
**-----------------------------------------------------------------*/
 void cp_analyze()
{
    //unsigned char xdata str2[]="CP=&&";
    INT8U *p,*m;
    INT8U key_intercp[15];         /*cp���ڹؼ��ִ������*/
    INT8U dat_intercp[20];        /*cp���ڹؼ��ֶ�Ӧ�������������*/
    INT16U i=0;
    INT16U j=0;
	  INT16U k=0;
    p=strstr(ReData.data_segme,cbag_cp);           /*cp����ʼ��ַ*/
    //q=strrchr(ReData.data_segme,'&');           /*cp�ν�����ַ*/


    for(m=p+5;*m!='&'/*m<q-1*/;m++)
    {
        if(*m!='=')                     /*ȡcp���еĹؼ���*/
        {
            key_intercp[i]=*m;
            i++;
        }
        else
        {
          key_intercp[i]='\0';        /*����ؼ������������*/
          i=0;
          for(p=m+1;*(p-1)!='&'/*p<=q-1*/&&(*(p-1))!=';'&&(*(p-1))!=',';p++)
          {
              if(*p!=';'&&*p!=','&&/*p<q-1*/*p!='&')    /*ȡ��Ӧ�Ĺؼ��ִ���*/
              {
                 dat_intercp[j]=*p;
                 j++;
              }
              else
              {
                  dat_intercp[j]='\0';
                  j=0;
                  m=p;
              }
          }
          strcpy(interparametre[k].D_intercp,dat_intercp);
          interparametre[k].D_intercp[strlen(dat_intercp)]='\0';
          strcpy(interparametre[k].K_intercp,key_intercp);
          interparametre[k].K_intercp[strlen(key_intercp)]='\0';
          k++;                                              /*��Ӵ�����Ӧ����*/

        }


    }
   for(;k<20;k++)
   {
    interparametre[k].D_intercp[0]='\0';
    interparametre[k].K_intercp[0]='\0';
   }
}

void dealdata_wjj()
{
  INT16U i,j,temp,k;
  INT8U s_time[]=";SystemTime=\0";
  INT8U l[]="-LowValue=\0";
  INT8U u[]="-UpValue=\0";
  INT8U value[21];
  INT8U pol_num[4];
  INT8U a_target[]=";AlarmTarget=\0";
  INT8U r_time[]=";ReportTime=\0";
  INT8U rt_interval[]=";RtdInterval=\0";
  INT8U ftstring[18]="\0";
  //INT8U CRC16[2];
  //INT8U datetemp=0,hourtemp=0,numtemp=0;
  //INT8U crc[5]="\0\0\0\0\0";
  long Tcurrent=0,Tend=0,Tstart=0;
  //long Tlimit=0,Tcheck=0,Tcurrent=0;
//	unsigned char xdata *pst;
  //CRC_Calculation(ReData.data_segme,strlen(ReData.data_segme),CRC16);
  //StrToASCII(CRC16,crc,2);
	//���հ�����CRCУ�飬���У����һ�£���Ϊ���ݰ���Ч
 // if(strcmp(crc,ReData.CRC)!=0)
  {
  //        Rt[0]='2'; //CRCУ�����
    //      Response(0);
  }
  //else
  {
    separation(ReData.data_segme);
    if(Rt[0]=='3')
    {
      Response(0);	
    }
    else
    {
      switch(ml_flag)
      {
        case '0':
        break;
        case '1':
          Rt[0]='1';
          Response(0);
        break;
        case '2':
        break;
        case '3':
          Rt[0]='1';
          Response(0);		
        break;		
        default:
        break;
      }
      for(i=0,j=1;i<40&&j!=0;i++)                //�ж���λ����������cn�������,��cn�������бȶ�
      j=strcmp(cn[i].cod,ml_cn);
		
      switch(i-1)  //operation();
      {
	case 0:           //���ó�ʱʱ�����ط����� 1000
          /*i=0;
          cp_analyze();
          while(strcmp("OverTime",interparametre[i].K_intercp)!=0) i++;
          j =strlen(interparametre[i].D_intercp);
          if(j>5)
          {
            e_result[0]='2';
	    Response(2);
            return;
          }
	  //strcpy(parameter.sys_parameter.overtime,interparametre[i].D_intercp);
          for(k=0;k<5;k++)
            parameter.sys_parameter.overtime[k]=interparametre[i].D_intercp[k];
          for(k=0;k<6;k++)
            LCD_index2[0][k]=0;
          for(k=0;k<5-j;k++)
            LCD_index2[0][k]='0';
          //LCD_indexs(parameter.sys_parameter.overtime,j,LCD_index2[0]+5-j);  //��ʱʱ�� 5λ
	  LCD_indexs(parameter.sys_parameter.overtime,5,LCD_index2[0]);  //��ʱʱ�� 5λ

          i=0;
	  while(strcmp("ReCount",interparametre[i].K_intercp)!=0) i++;
          j =strlen(interparametre[i].D_intercp);
          if(j>2)
          {
            e_result[0]='2';
	    Response(2);
            return;
          }
	  strcpy(parameter.sys_parameter.resendtime,interparametre[i].D_intercp);
          for(k=0;k<4;k++)
            LCD_index1[3][k]=0;
          for(k=0;k<2-j;k++)
            LCD_index1[3][k]='0';
          //LCD_indexs(parameter.sys_parameter.resendtime,j,LCD_index1[3]+2-j);	//�ط����� 2λ
	  LCD_indexs(parameter.sys_parameter.resendtime,2,LCD_index1[3]);	//�ط����� 2λ

	  Symbol.LCD_para_set=1;
          */
          e_result[0]='1';
          Response(2);
        break;
        case 1:          //���ó��ޱ���ʱ��  1001
          i=0;
          cp_analyze();
	  while(strcmp("WarnTime",interparametre[i].K_intercp)!=0)i++;
          j =strlen(interparametre[i].D_intercp);
          if(j>5)
          {
            e_result[0]='2';
	    Response(2);
            return;
          }
	  //strcpy(parameter.sys_parameter.warntime,interparametre[i].D_intercp);
          for(k=0;k<5;k++)
            parameter.sys_parameter.warntime[k]=interparametre[i].D_intercp[k];
          for(k=0;k<6;k++)
            LCD_index2[1][k]=0;
          for(k=0;k<5-j;k++)
            LCD_index2[1][k]='0';
	  //LCD_indexs(parameter.sys_parameter.warntime,j,LCD_index2[1]+5-j);  //����ʱ�� 5λ
	  LCD_indexs(parameter.sys_parameter.warntime,5,LCD_index2[1]);  //����ʱ�� 5λ

          Symbol.LCD_para_set=1;

          e_result[0]='1';
          Response(2);
        break;
        case 2:     //��ȡ�ֳ�ϵͳʱ��   1011
          Response(3);
          strcat(CBAG.cbag_dynamic,s_time);
          strcat(CBAG.cbag_dynamic,System_Time);
          Add_Send();
          e_result[0]='1';
          Response(2);
        break;
        case 3:            //�����ֳ���ʱ��  1012
          i=0;
          cp_analyze();
          while(strcmp("SystemTime",interparametre[i].K_intercp)!=0) i++;
          strcpy(System_Time1,interparametre[i].D_intercp);		
          v_Set1302();	// ����ʱ��
          e_result[0]='1';
          Response(2);
	break;
        case 4:          //��ȡ��Ⱦ�ﱨ������ֵ 1021
          cp_analyze();
          Response(3);

          for(i=0;i<8;i++)
          {
            for(j=0;j<3;j++)
            {
              pol_num[j]=ADPARA(i).po_id[j];
              //pol_num[j]=AD_parameter.AD_i[i].po_id[j];
            }
            pol_num[3]='\0';
            for(j=0;strcmp(pol_num,interparametre[j].D_intercp)!=0&&interparametre[j].D_intercp[0]!='\0';j++);
            if(interparametre[j].D_intercp[0]!='\0')
            {
              strcat(CBAG.cbag_dynamic,semicolon);
              strcat(CBAG.cbag_dynamic,interparametre[j].D_intercp);
              strcat(CBAG.cbag_dynamic,l);
              float_to_string(ADPARA(i).L_Alarmlimt.b,ftstring);
              //float_to_string(AD_parameter.AD_i[i].L_Alarmlimt.b,ftstring); //float��ת��Ϊstring
              strcat(CBAG.cbag_dynamic,ftstring);
              strcat(CBAG.cbag_dynamic,comma);
              strcat(CBAG.cbag_dynamic,interparametre[j].D_intercp);
              float_to_string(ADPARA(i).U_Alarmlimt.b,ftstring);
              //float_to_string(AD_parameter.AD_i[i].U_Alarmlimt.b,ftstring); //float��ת��Ϊstring
              strcat(CBAG.cbag_dynamic,u);
              strcat(CBAG.cbag_dynamic,ftstring);
             }
          }
          Add_Send();
          e_result[0]='1';
          Response(2);
        break;
        case 5:        //������Ⱦ�ﱨ������ֵ  1022
          cp_analyze();
          for(i=0;i<16&&interparametre[i].K_intercp[0]!='\0';i++)
          {
            j=0;
            while(interparametre[i].K_intercp[j]!='-')
            {
                    value[j]=interparametre[i].K_intercp[j];               //��ȡ��Ⱦ�����
                    j++;
            }
            value[j]='\0';
            for(k=0;k<8;k++)
            {
              INT8U x;
              for(x=0;x<3;x++)
              {
                if(ADPARA(k).po_id[x]!=value[x])
                  break;
              }
              if(x>=3)
                break;
            }
            if(k<8) //�ҵ����ϵ�
            {
              if(interparametre[i].K_intercp[j+1]=='L')  //�洢��Ⱦ���ż�����
              {
                ADPARA(k).L_Alarmlimt.b=string_to_float(interparametre[i].D_intercp);
                float_to_string(ADPARA(k).L_Alarmlimt.b,ftstring);
				//////WDTCN     = 0xA5;	//ι��
                for(j=0;j<13;j++)
                {
                  if(ftstring[j]==0x2e)//��
                  {
                    LCD_indexAD[k][4][j]=11;
                    Pre_LCD_Dot[3]|=(1<<k);
                  }
                  else if (ftstring[j]=='\0')
                  {
                    LCD_indexAD[k][4][j]=0;//12;
                    break;
                  }	
                  else LCD_indexAD[k][4][j]=(ftstring[j]&0x0f)+1;	
                }
                for(;j<13;j++)
                  LCD_indexAD[k][4][j]=0;
                LCD_indexAD[k][4][13]=0;
                //AD_parameter.AD_i[i/2].L_Alarmlimt.b=string_to_float(interparametre[i].D_intercp);	
              }
              else   //�洢����
              {
                ADPARA(k).U_Alarmlimt.b=string_to_float(interparametre[i].D_intercp);
                float_to_string(ADPARA(k).U_Alarmlimt.b,ftstring);

                for(j=0;j<13;j++)
                {
                  if(ftstring[j]==0x2e)//��
                  {
                    LCD_indexAD[k][3][j]=11;
                    Pre_LCD_Dot[2]|=(1<<k);
                  }
                  else if (ftstring[j]=='\0')
                  {
                    LCD_indexAD[k][3][j]=0;
                    break;
                  }	
                  else LCD_indexAD[k][3][j]=(ftstring[j]&0x0f)+1;	
                }
                for(;j<13;j++)
                  LCD_indexAD[k][3][j]=0;
                LCD_indexAD[k][3][13]=0;
              }

            }
	  //AD_parameter.AD_i[(i-1)/2].U_Alarmlimt.b=string_to_float(interparametre[i].D_intercp);
						
          }
          Symbol.LCD_para_set=1;
          e_result[0]='1';
          Response(2);
        break;
        case 6:              //��ȡ��λ����ַ 1031
          Response(3);
          strcat(CBAG.cbag_dynamic,a_target);	
          j=strlen(CBAG.cbag_dynamic);				
          for(i=0;i<20;i++)
          {
            CBAG.cbag_dynamic[j+i]=parameter.sys_parameter.alarmtarge[i];
          }
          CBAG.cbag_dynamic[j+20]='\0';
          Add_Send();
          e_result[0]='1';
          Response(2); 	
        break;
        case 7:           //������λ����ַ  1032
          cp_analyze();
          for(i=0;i<20;i++)
          {
            parameter.sys_parameter.alarmtarge[i]=interparametre[0].D_intercp[i];
          }

          Symbol.LCD_para_set=1;
          e_result[0]='1';
          Response(2);
        break;
        case 8:          //��ȡ�����ϱ�ʱ��  1041
          Response(3);
          strcat(CBAG.cbag_dynamic,r_time);
          j=strlen(CBAG.cbag_dynamic);
          for(i=0;i<4;i++)
          {
                  CBAG.cbag_dynamic[j+i]=parameter.sys_parameter.reporttime[i];
          }
          CBAG.cbag_dynamic[j+4]='\0';
          Add_Send();
          e_result[0]='1';
          Response(2);
        break;
        case 9:          //���������ϱ�ʱ��   1042
	  cp_analyze();
          j =strlen(interparametre[0].D_intercp);
          if(j>4)
          {
            e_result[0]='2';
	    Response(2);
            return;
          }
          //strcpy(value,interparametre[0].D_intercp);

          //value[strlen(interparametre[0].D_intercp)]='\0';
          //for(i=0;i<4&&value[i]!='\0';i++)
          for(i=0;i<4;i++)
          {
            //parameter.sys_parameter.reporttime[i]=value[i];
            parameter.sys_parameter.reporttime[i]=interparametre[0].D_intercp[i];
          }
          for(k=0;k<5;k++)
            LCD_index2[2][k]  =0;
          for(k=0;k<4-j;k++)
            LCD_index2[2][k]  ='0';
          LCD_indexs(parameter.sys_parameter.reporttime,4,LCD_index2[2]);	//�����ϱ���� 2λ
          //LCD_indexs(parameter.sys_parameter.reporttime,j,LCD_index2[2]+4-j);	//�����ϱ���� 2λ
          Symbol.LCD_para_set=1;
          e_result[0]='1';
          Response(2);
        break;
        case 10:         //��ȡʵʱ���ݼ�� 1061
          Response(3);
          strcat(CBAG.cbag_dynamic,rt_interval);
          j=strlen(CBAG.cbag_dynamic);
          for(i=0;i<3;i++)
            CBAG.cbag_dynamic[j+i]=parameter.sys_parameter.rd_interval[i];
          CBAG.cbag_dynamic[j+3]='\0';
          Add_Send();
          e_result[0]='1';
          Response(2);
        break;
        case 11:        //����ʵʱ���ݼ�� 1062
	  cp_analyze();
          j=strlen(interparametre[0].D_intercp);
          if(j>3)
          {
            e_result[0]='2';
            Response(2);
            return;
          }
          for(i=0;i<3;i++)
          {
            parameter.sys_parameter.rd_interval[i]=interparametre[0].D_intercp[i];
          }
          for(k=0;k<4;k++)
            LCD_index2[3][k]  =0;
          for(k=0;k<3-j;k++)
            LCD_index2[3][k]  ='0';
            //LCD_indexs(parameter.sys_parameter.rd_interval,j,LCD_index2[3]+3-j);//ʵʱ�ϱ���� 3λ
            LCD_indexs(parameter.sys_parameter.rd_interval,3,LCD_index2[3]);//ʵʱ�ϱ���� 3λ

          Symbol.LCD_para_set=1;
          Symbol.rd_change=1;
          e_result[0]='1';
          Response(2);
        break;
        case 12:        //���÷�������  1072
          cp_analyze();
          strcpy(LCD_user_pw,interparametre[0].D_intercp);
          for(i=0;i<6;i++)
          {
                  parameter.sys_parameter.pw[i]=interparametre[0].D_intercp[i];
          }
				
          Symbol.LCD_para_set=1;
          e_result[0]='1';
          Response(2);
					
        break;
        case 13:
        break;
        case 14:
        break;
        case 15:
          Symbol.stop_watch=1;     //ȡ��Ⱦ��ʵʱ����  2011
          collect_realdata(0,0);
          e_result[0]='1';
          Response(2);
        break;
        case 16:
          Symbol.stop_watch=0;
          Response(1);		
        break;
	case 17:  //2021 ��Ⱦ������ʩ����״̬
          //INT8U SBV;
          stop_watch_devstatus=1;
          Symbol.ccc=1;
          Response(3);
          Symbol.ccc=0;
          strcat(CBAG.cbag_dynamic,"DataTime=");
          strcat(CBAG.cbag_dynamic,System_Time);
          for(i=0;i<8;i++)
          {
            INT8U SBx[]=";SB1-RS=\0\0";
            SBx[8]=((RealData.R_D.DO>>i)&0X01)+0X30;
            SBx[3]=i+0X31;
            strcat(CBAG.cbag_dynamic,SBx);
          }
          j=strlen(CBAG.cbag_dynamic);
          CBAG.cbag_dynamic[j]='\0';
          Add_Send();
          e_result[0]='1';
          Response(2);
          //Progupdatemain();
        break;
        case 18:  //2022 ֹͣ�鿴��Ⱦ������ʩ����״̬
          stop_watch_devstatus=0;
          Response(1);
        break;
        case 19://ȡ��Ⱦ����ʷ������  2031
          cp_analyze();
          Tcurrent=(((long)current_year<<16)|((long)current_moth<<8)|((long)(current_date)));
          value[0]=(interparametre[1].D_intercp[2]&0x0f)*10+(interparametre[1].D_intercp[3]&0x0f);  //endyear
          value[1]=(interparametre[1].D_intercp[4]&0x0f)*10+(interparametre[1].D_intercp[5]&0x0f);  //endmonth
          value[2]=(interparametre[1].D_intercp[6]&0x0f)*10+(interparametre[1].D_intercp[7]&0x0f);  //enddate
          //value[3]=(interparametre[1].D_intercp[8]&0x0f)*10+(interparametre[1].D_intercp[9]&0x0f);  //endhour
          Tend=(((long)value[0]<<16)|((long)value[1]<<8)|(((long)value[2])));

          value[6]=(interparametre[0].D_intercp[2]&0x0f)*10+(interparametre[0].D_intercp[3]&0x0f);  //startyear
          value[7]=(interparametre[0].D_intercp[4]&0x0f)*10+(interparametre[0].D_intercp[5]&0x0f);  //startmonth
          value[8]=(interparametre[0].D_intercp[6]&0x0f)*10+(interparametre[0].D_intercp[7]&0x0f);  //startdate
          //value[9]=(interparametre[0].D_intercp[8]&0x0f)*10+(interparametre[0].D_intercp[9]&0x0f);  //starthour
          Tstart=(((long)value[6]<<16)|((long)value[7]<<8)|(((long)value[8])));
          if((Tcurrent<Tend)||(Tstart>Tend))
          {
            e_result[0]='2';
            Response(2);
          }
          else
          {
            ReportDaydata(value[7],value[8],value[1],value[2]);
            /*
            if(current_year==value[6])
            {
              if(current_moth==value[7])
              {
                UploadHourdata(value[7],value[8],value[9],value[1],value[2],value[3]);
              }
              else
              {
                if((current_moth-value[7])==1)
                {
                  if(current_date<value[8])
                  {
                    ReportDaydata(value[7],value[8],value[1],value[2]);
                  }
                  else
                  {
                    e_result[0]='2';
                    Response(2);
                  }
                }
                else
                {
                  e_result[0]='2';
                  Response(2);
                }
              }
            }
            else
            {
              if((current_year-value[6])==1)
              {
                if((current_moth==1)&&(value[7]==12))
                {
                  if(current_date<value[8])
                  {
                    ReportDaydata(value[7],value[8],value[1],value[2]);
                  }
                  else
                  {
                    e_result[0]='2';
                    Response(2);
                  }
                }
                else
                {
                  e_result[0]='2';
                  Response(2);
                }
              }
              else
              {
                e_result[0]='2';
                Response(2);
              }
            }*/
          }
        break;
        /*case 19:        //ȡ��Ⱦ����ʷ��������  2031

                if(Mod_checkover==1)
                {
                        Mod_method=Mod_DayD;
                        Day_data=0;
                        Mod_ontime=1;
                }
                else
                        Day_data=1;
        break;*/
        case 20:    //2041
          cp_analyze();
          Symbol.ccc=1;
          Response(3);
          Symbol.ccc=0;
          strcat(CBAG.cbag_dynamic,"DataTime=");
          strcat(CBAG.cbag_dynamic,interparametre[0].D_intercp);
          for(i=0;i<8;i++)
          {
            INT8U SBx[]=";SB1-RT=\0\0";
            SBx[8]=((RealData.R_D.DO>>i)&0X01)+0X30;
            SBx[3]=i+0X31;
            strcat(CBAG.cbag_dynamic,SBx);
          }
          j=strlen(CBAG.cbag_dynamic);
          CBAG.cbag_dynamic[j]='\0';
          Add_Send();
          e_result[0]='1';
          Response(2);

        break;
        case 21:  //2051
          cp_analyze();
          Symbol.ccc=1;
          Response(3);
          Symbol.ccc=0;
          strcat(CBAG.cbag_dynamic,"DataTime=");
          strcat(CBAG.cbag_dynamic,interparametre[0].D_intercp);
          for(i=0;i<4;i++)
          {
            INT8U a[]="-Min=\0";
            INT8U b[]="-Max=\0";
            INT8U c[]="-Avg=\0";
            INT8U d[5]="\0\0\0\0";
            for(j=0;j<3;j++)
            {
                d[j]=ADPARA(i).po_id[j];
                    //d[j]=AD_parameter.AD_i[i].po_id[j];
            }
            d[3]='\0';
            strcat(CBAG.cbag_dynamic,semicolon);	//�ӷֺ�
            strcat(CBAG.cbag_dynamic,d);
            float_to_string(RealData.R_D.FF.ADF[i],ftstring);
            strcat(CBAG.cbag_dynamic,a);
            strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����	

            strcat(CBAG.cbag_dynamic,",");
            strcat(CBAG.cbag_dynamic,d);
            strcat(CBAG.cbag_dynamic,c);
            strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����	

            strcat(CBAG.cbag_dynamic,",");
            strcat(CBAG.cbag_dynamic,d);
            strcat(CBAG.cbag_dynamic,b);
            strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����	


          }
          j=strlen(CBAG.cbag_dynamic);
          CBAG.cbag_dynamic[j]='\0';
          Add_Send();
          e_result[0]='1';
          Response(2);
          /*
                if(Mod_checkover)
                {
                        Mod_method=Mod_MinD;
                        Minute_data=0;
                        Mod_ontime=1;
                }
                else
                        Minute_data=1;*/
        break;
        case 22:    //��ȡСʱ��ʷ���� 2061
          cp_analyze();
          Tcurrent=(((long)current_year<<24)|((long)current_moth<<16)|((long)(current_date)<<8)|(current_hour));
          value[0]=(interparametre[1].D_intercp[2]&0x0f)*10+(interparametre[1].D_intercp[3]&0x0f);  //endyear
          value[1]=(interparametre[1].D_intercp[4]&0x0f)*10+(interparametre[1].D_intercp[5]&0x0f);  //endmonth
          value[2]=(interparametre[1].D_intercp[6]&0x0f)*10+(interparametre[1].D_intercp[7]&0x0f);  //enddate
          value[3]=(interparametre[1].D_intercp[8]&0x0f)*10+(interparametre[1].D_intercp[9]&0x0f);  //endhour
          Tend=(((long)value[0]<<24)|((long)value[1]<<16)|(((long)value[2])<<8)|(value[3]));

          value[6]=(interparametre[0].D_intercp[2]&0x0f)*10+(interparametre[0].D_intercp[3]&0x0f);  //startyear
          value[7]=(interparametre[0].D_intercp[4]&0x0f)*10+(interparametre[0].D_intercp[5]&0x0f);  //startmonth
          value[8]=(interparametre[0].D_intercp[6]&0x0f)*10+(interparametre[0].D_intercp[7]&0x0f);  //startdate
          value[9]=(interparametre[0].D_intercp[8]&0x0f)*10+(interparametre[0].D_intercp[9]&0x0f);  //starthour
          Tstart=(((long)value[6]<<24)|((long)value[7]<<16)|(((long)value[8])<<8)|(value[9]));

          if((Tcurrent<Tend)||(Tstart>Tend))
          {
            e_result[0]='2';
            Response(2);
          }
          else
          {
            UploadHourdata(value[7],value[8],value[9],value[1],value[2],value[3]);
            /*
            if(current_year==value[6])
            {
              if(current_moth==value[7])
              {
                UploadHourdata(value[7],value[8],value[9],value[1],value[2],value[3]);
              }
              else
              {
                if((current_moth-value[7])==1)
                {
                  if(current_date<value[8])
                  {
                    UploadHourdata(value[7],value[8],value[9],value[1],value[2],value[3]);
                  }
                  else
                  {
                    e_result[0]='2';
                    Response(2);
                  }
                }
                else
                {
                  e_result[0]='2';
                  Response(2);
                }
              }
            }
            else
            {
              if((current_year-value[6])==1)
              {
                if((current_moth==1)&&(value[7]==12))
                {
                  if(current_date<value[8])
                  {
                    UploadHourdata(value[7],value[8],value[9],value[1],value[2],value[3]);
                  }
                  else
                  {
                    e_result[0]='2';
                    Response(2);
                  }
                }
                else
                {
                  e_result[0]='2';
                  Response(2);
                }
              }
              else
              {
                e_result[0]='2';
                Response(2);
              }
            }*/
          }
	break;
	case 23:  //2071  ȡ��Ⱦ�ﱨ����¼
          cp_analyze();
          if((strcmp(System_Time,interparametre[1].D_intercp)<0)||(strcmp(interparametre[0].D_intercp,interparametre[1].D_intercp)>0))
          {
            e_result[0]='2';
            Response(2);
          }
          else
          {
            ReportAlarmRecord(interparametre[0].D_intercp,interparametre[1].D_intercp);
          }
          /*
          Tcurrent=(((long)current_year<<24)|((long)current_moth<<16)|((long)(current_date)<<8)|(current_hour));
          value[0]=(interparametre[1].D_intercp[2]&0x0f)*10+(interparametre[1].D_intercp[3]&0x0f);  //endyear
          value[1]=(interparametre[1].D_intercp[4]&0x0f)*10+(interparametre[1].D_intercp[5]&0x0f);  //endmonth
          value[2]=(interparametre[1].D_intercp[6]&0x0f)*10+(interparametre[1].D_intercp[7]&0x0f);  //enddate
          value[3]=(interparametre[1].D_intercp[8]&0x0f)*10+(interparametre[1].D_intercp[9]&0x0f);  //endhour
          value[4]=(interparametre[1].D_intercp[10]&0x0f)*10+(interparametre[1].D_intercp[11]&0x0f);  //endminute
          value[5]=(interparametre[1].D_intercp[12]&0x0f)*10+(interparametre[1].D_intercp[13]&0x0f);  //endsecond
          Tend=(((long)value[0]<<24)|((long)value[1]<<16)|(((long)value[2])<<8)|(value[3]));

          value[6]=(interparametre[0].D_intercp[2]&0x0f)*10+(interparametre[0].D_intercp[3]&0x0f);  //startyear
          value[7]=(interparametre[0].D_intercp[4]&0x0f)*10+(interparametre[0].D_intercp[5]&0x0f);  //startmonth
          value[8]=(interparametre[0].D_intercp[6]&0x0f)*10+(interparametre[0].D_intercp[7]&0x0f);  //startdate
          value[9]=(interparametre[0].D_intercp[8]&0x0f)*10+(interparametre[0].D_intercp[9]&0x0f);  //starthour
          value[9]=(interparametre[0].D_intercp[10]&0x0f)*10+(interparametre[0].D_intercp[11]&0x0f);  //startminute
          value[9]=(interparametre[0].D_intercp[12]&0x0f)*10+(interparametre[0].D_intercp[13]&0x0f);  //startsecond
          Tstart=(((long)value[6]<<24)|((long)value[7]<<16)|(((long)value[8])<<8)|(value[9]));
          */
        break;
        case 24:  //3011 У��У��
          cp_analyze();
          e_result[0]='1';
          Response(2);
          for(i=0;i<4;i++)
          {
            devcpoID[i]=interparametre[0].D_intercp[i];
          }
          devcpoID[3]=0;
          Devctr.calibrate  =1;
        break;
        case 25:  //3012 ��ʱ����
          cp_analyze();
          e_result[0]='1';
          Response(2);
          for(i=0;i<4;i++)
          {
            devcpoID[i]=interparametre[0].D_intercp[i];
          }
          devcpoID[3]=0;
          Devctr.sample =1;
        break;
        case 26:  //3013 �豸��������
          e_result[0]='1';
          Response(2);
          Devctr.operate  =1;
        break;
        case 27:  //3014 �����豸����ʱ������
          cp_analyze();
          e_result[0]='1';
          Response(2);
          for(i=0;i<4;i++)
          {
            devcpoID[i]=interparametre[0].D_intercp[i];
          }
          for(i=1;(interparametre[i].K_intercp[0]!='\0')&&(i<=24);i++)
          {
            devctime[i-1]  =(interparametre[i].D_intercp[0]&0x0f)*10+interparametre[i].D_intercp[1]&0x0f;
          }
          devctnum  =i-1;
          Devctr.settimes =1;
        break;
        case 28:              //�������������ָ�� 9018
          cp_analyze();
          for(i=0;interparametre[i].K_intercp[0]!='\0';i++)
          {
            value[0]=interparametre[i].D_intercp[0]&0x01;
            value[1]=interparametre[i].K_intercp[2]&0x0f;
            if(value[0]==0x01)                  //��ĳλ
            {
              if(value[1]==5)
              {
                Symbol.NDO56=1;
                temp=DIInput();
                if((temp&0xc000)==0x8000)//�ж�DI15��16�Ƿ�Ϊ0��1
                {
                  OldDO=RealData.R_D.DO;
                  RealData.R_D.DO|=0x20;
                  DOOutput(RealData.R_D.DO);
                  Delay20ms();
                  Delay20ms();
                  RealData.R_D.DO&=0xDF;
                  DOOutput(RealData.R_D.DO);
                  RealData.R_D.DO|=0x10;//��DO5	
                  DOOutput(RealData.R_D.DO);
                  Symbol.DO5_DI15=1;
                }
                else
                {
                  e_result[0]='2';
                  Response(2);
                }
              }
              else if(value[1]==6)
              {
                Symbol.NDO56=1;
                temp=DIInput();
                if((temp&0xc000)==0x4000)
                {
                  OldDO=RealData.R_D.DO;
                  RealData.R_D.DO|=0x20;//RealData.R_D.DO|(value[0]<<(value[1]-1));
                  DOOutput(RealData.R_D.DO);
                  Symbol.DO6_DI16=1;
                }
                else
                {
                  e_result[0]='2';
                  Response(2);
                }
              }
              else
              {
                OldDO=RealData.R_D.DO;
                RealData.R_D.DO=RealData.R_D.DO|(value[0]<<(value[1]-1));
                Symbol.NDO56=0;
              }	
            }
            else                                //��ĳλ
            {
              OldDO=RealData.R_D.DO;
              RealData.R_D.DO=RealData.R_D.DO&(~((value[0]|0x01)<<(value[1]-1)));
              Symbol.NDO56=0;
            }
          }
          if(Symbol.NDO56==0)
          {
            //NDO56=0;
            DOOutput(RealData.R_D.DO);
            e_result[0]='1';
            Response(2);

          }
          else
            Symbol.NDO56=0;
          if((Symbol.LCD_deal==0)&&(LCD_page==41))
          {
            Symbol.LCD_DO=1;
            Symbol.LCD_deal=1;
          }				
        break;
        case 29:           //������Ⱦ������(ADת������Ӧ����Ⱦ��) 1002
          cp_analyze();
          for(i=0;i<8;i++)
          {
            for(j=0;j<3;j++)
            {
              ADPARA(i).po_id[j]=interparametre[i].D_intercp[j];
              //AD_parameter.AD_i[i].po_id[j]=interparametre[i].D_intercp[j];
            }
          }

          Symbol.LCD_para_set=1;
          e_result[0]='1';
          Response(2);				
        break;	
        case 30:				
	      break;
        case 31://Զ������
          Progupdatemain();
        break;
        case 32:    //2081  ȡ��ʷʵʱ���� 2����
          cp_analyze();
          //Tcurrent=(((long)(current_date)<<16)|((long)(current_hour)<<8));
          //value[0]=(interparametre[1].D_intercp[6]&0x0f)*10+(interparametre[1].D_intercp[7]&0x0f);  //endday
          //value[1]=(interparametre[1].D_intercp[8]&0x0f)*10+(interparametre[1].D_intercp[9]&0x0f);  //endmhour
          //value[2]=(interparametre[1].D_intercp[10]&0x0f)*10+(interparametre[1].D_intercp[11]&0x0f);  //endmin
          //value[3]=(interparametre[1].D_intercp[8]&0x0f)*10+(interparametre[1].D_intercp[9]&0x0f);  //endhour
          //value[4]=(interparametre[1].D_intercp[10]&0x0f)*10+(interparametre[1].D_intercp[11]&0x0f);  //endhour
          //Tend=(((long)value[0]<<16)|((long)value[1]<<8)|(((long)value[2])));

          value[6]=(interparametre[0].D_intercp[6]&0x0f)*10+(interparametre[0].D_intercp[7]&0x0f);  //startday
          value[7]=(interparametre[0].D_intercp[8]&0x0f)*10+(interparametre[0].D_intercp[9]&0x0f);  //starthour
          //value[8]=(interparametre[0].D_intercp[10]&0x0f)*10+(interparametre[0].D_intercp[11]&0x0f);  //startmin
          //value[9]=(interparametre[0].D_intercp[8]&0x0f)*10+(interparametre[0].D_intercp[9]&0x0f);  //starthour
         // Tstart=(((long)value[6]<<8)|((long)value[7]));
          if((value[6]==current_date)||(value[6]==yestoday))
          //if((Tcurrent<Tend)||(Tstart>Tend))
          {
            UploadRealdata(value[6],value[7]); 
          }
          else
          {
            e_result[0]='2';
            Response(2);
          }
        break;
        case 33:  //2091 ȡ���м�¼ ����ʱ�䣬�ϵ�ʱ��
          runlogreport();
        break;
        default:
        break;
      }
    }
  }
  //RI0=0;
}

long mod10n(INT8U n)
{
  long a=1;
  char i;
  for(i=0;i<n;i++)
  a*=10;
  return(a);
}

void float_to_string(float tempf,INT8U *ftstring) //reentrant
{
  INT8U t1,t2,j;
  unsigned long g,m;
  INT16U h;
  g=(long)tempf;   //ȡ��������
  h=(int)((tempf-g)*100); //ȡС������

  for(t1=10;t1>0;t1--)
  {
    m=g;
    for(t2=0;t2<t1-1;t2++)
      m=m/10;
    if(m!=0)
      break;
  }
  ftstring[0]=((int)m)|0x30;        //д��������
  j=1;
  for(;t1>1;t1--,j++)
  {
    m=g;
    m=m%mod10n(t1-1);
    m=m/mod10n(t1-2);
    ftstring[j]=((int)m)|0x30;
  }
  ftstring[j]='.';
  t1=h/10;        //дС������
  t2=h%10;
  if((t1!=0)||(t2!=0))
  {	
    ftstring[j+1]=t1|0x30;
    if(t2!=0)
            ftstring[j+2]=t2|0x30;
    else
            ftstring[j+2]='\0';
    ftstring[j+3]='\0';
  }
  else
    ftstring[j]='\0';
}

float string_to_float(INT8U *tempstr) //reentrant
{
  INT8U i,j,h;
  unsigned long temp=0,temp1=0;
  float temp2=0.0,temp3=0.0;
  char *temppt;

  for(i=0;*tempstr!='.'&&*tempstr!='\0';tempstr++,i++); //����С����λ��

  temppt=tempstr;
  for(h=i;i>0;i--)                   //�ַ���ת��Ϊ������
  {
    temp1=*(tempstr-1)&0xf;
    for(j=0;j<h-i;j++)
            temp1*=10;
    tempstr--;
    temp+=temp1;
  }

  for(i=h+1,j=0;(*temppt!='\0')&&(*(temppt+1)!='\0');i++)
  {
    temp2=*(temppt+1)&0xf;
    for(j=0;j<i-h;j++)
            temp2=temp2/10;
    temp3+=temp2;
    temppt++;	
  }
  return(temp3+(float)temp);		

}
/*---------------------------------------------------------------------------------------------
                             ȡ��Ⱦ��ʵʱ����  2011
------------------------------------------------------------------------------------------------*/
void collect_realdata(INT8U hoursave,INT8U daysave)
{
  //INT8U t1,t2;
  INT8U i,j;
  INT8U cps[]="DataTime=\0";
  INT8U a[]="-Rtd=\0";
  //INT8U b[]=";DI=\0";
  //INT8U c[]=";DO=\0";
  INT8U d[10]="\0\0\0\0\0\0\0\0\0\0";
  INT8U ftstring[18]="\0";
  HISDATAFLAGS_R sig_r;
  //INT16U pageaddr=0;
  //INT8U Rdaddr[3];

  Symbol.ccc=1;
  Response(4);
  Symbol.ccc=0;

  strcat(CBAG.cbag_dynamic,cps);
  strcat(CBAG.cbag_dynamic,System_Time);

  for(i=0;i<8;i++)
  {
    if((ADPARA(i).po_id[0]=='0')&&(ADPARA(i).po_id[1]=='0')&&(ADPARA(i).po_id[2]=='0'))
      continue;
    for(j=0;j<3;j++)
    {
      d[j]=ADPARA(i).po_id[j];
      //d[j]=AD_parameter.AD_i[i].po_id[j];
    }
    d[3]='\0';
    strcat(CBAG.cbag_dynamic,semicolon);	//�ӷֺ�
    strcat(CBAG.cbag_dynamic,d);
    float_to_string(RealData.R_D.FF.ADF[i],ftstring);
    strcat(CBAG.cbag_dynamic,a);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����			
  }

  
  INT16U tempnum_ch=0;
  for(i=0;i<6;i++)
    tempnum_ch  +=Channel_Num[i];
  if(tempnum_ch!=0)
  {
    ModataUpload(1);
  }

  add_send();//�������
	//Add_Send();
  //ʵʱ���ݴ洢 ��ǩҳ��ַ10
  INT16U pageaddr;
  INT8U Rdaddr[3];
  d[0]  =((System_Time[6]&0X0F)*10+(System_Time[7]&0X0F));//��ǰ��
  d[1]  =((System_Time[8]&0X0F)*10+(System_Time[9]&0X0F));//��ǰʱ
  d[2]  =((System_Time[10]&0X0F)*10+(System_Time[11]&0X0F));//��ǰ��
  pageaddr  =10;
  Rdaddr[0]=0;
  Rdaddr[1]=0x50;
  Rdaddr[2]=0;
  FlashMemorytoBuffer(Rdaddr,FLASHBUFFER1);//read all the flag structs to buffer1
  Delay_N_mS(400);

  pageaddr  =200*2;   //current struct buffer offset address
  Rdaddr[0]=0;
  Rdaddr[1]=(INT8U)(pageaddr>>8);
  Rdaddr[2]=(INT8U)(pageaddr);
  FlashBufferRead(Rdaddr,&sig_r,sizeof(sig_r),FLASHBUFFER1);
  FlashBufferRead(Rdaddr,&sig_r,sizeof(sig_r),FLASHBUFFER1); //day data flag struct
  if(sig_r.day!=d[0])
  {
    INT16U oldaddr=0;
    pageaddr  =10;
    Rdaddr[0]=(INT8U)(pageaddr>>5);
    Rdaddr[1]=(INT8U)(pageaddr<<3);
    Rdaddr[2]=0;

    FlashBufferRead(Rdaddr,&sig_r,sizeof(sig_r),FLASHBUFFER1); //day data flag struct
    oldaddr  =sig_r.page;
    for(i=0;i<2;i++)
    {
      MemoryClear(&sig_r,sizeof(sig_r));
      pageaddr  =200*(i+1);   //buffer offset address
      Rdaddr[0]=0;
      Rdaddr[1]=((INT8U)(pageaddr>>8))|0x50;
      Rdaddr[2]=(INT8U)(pageaddr);
      FlashBufferRead(Rdaddr,&sig_r,sizeof(sig_r),FLASHBUFFER1); //hour data flag struct
      pageaddr  =200*i;   //buffer offset address
      Rdaddr[0]=0;
      Rdaddr[1]=((INT8U)(pageaddr>>8))|0x50;
      Rdaddr[2]=(INT8U)(pageaddr);
      FlashBufferWrite(Rdaddr,&sig_r,sizeof(sig_r),FLASHBUFFER1);
    }
    MemoryClear(&sig_r,sizeof(sig_r));

    //sig_d.year =(System_Time[0]&0x0f)*1000+(System_Time[1]&0x0f)*100+(System_Time[2]&0x0f)*10+(System_Time[3]&0x0f);
    sig_r.day  =d[0];
    sig_r.page  =oldaddr;

  }
  sig_r.flag[d[1]]  |=0x0001<<(d[2]/5);  //set flag
  pageaddr  =200*2;   //current struct buffer offset address
  Rdaddr[0]=0;
  Rdaddr[1]=((INT8U)(pageaddr>>8))|0x50;
  Rdaddr[2]=(INT8U)(pageaddr);

  FlashBufferWrite(Rdaddr,&sig_r,sizeof(sig_r),FLASHBUFFER1);  //write current flag struct to the buffer of the flash

  pageaddr =sig_r.page;
  pageaddr +=(d[1]*12+d[2]/5);  //page address to store
  Rdaddr[0]=0;
  Rdaddr[1]=0;
  Rdaddr[2]=0;

  Rdaddr[0]=(INT8U)(pageaddr>>5);
  Rdaddr[1]=(INT8U)(pageaddr<<3);

  FlashMemoryWrite(Rdaddr,&CBAG,sizeof(CBAG),FLASHBUFFER2);
  Rdaddr[0]=0;
  Rdaddr[1]=0x50;
  Rdaddr[2]=0;
  FlashBuffertoMemory(Rdaddr,FLASHBUFFER1); //write back flag structs to the flash memory


  MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
}
/*
void UploadHourdata(INT8U date,INT8U hour,INT8U num)
{
  INT8U i;
  INT8U Flashaddr[3];
  INT8U temp[2];
  INT16U lenth=0,addrtemp;
  INT8U datatemp[512];
  addrtemp=(date-1)*24+hour;
  Flashaddr[0]=(INT8U)(addrtemp>>8);
  Flashaddr[1]=(INT8U)(addrtemp);
  for(i=0;i<num;i++)
  {
    FlashMemoryRead(Flashaddr,temp,2);
    lenth=(((INT16U)temp[0])<<8)|temp[1];
    Flashaddr[2]=0X10;
    FlashMemoryRead(Flashaddr,datatemp,lenth);
    strcat(CBAG.cbag_dynamic,datatemp+5);
    Add_Send();//�������
    MemoryClear(datatemp,strlen(datatemp));//clear datatemp
    addrtemp=(((INT16U)Flashaddr[0])<<8)|Flashaddr[1];
    addrtemp+=0X0008; //PAGE ADDRESS ADD BY 1

    Flashaddr[0]=(INT8U)(addrtemp>>8);
    Flashaddr[1]=(INT8U)(addrtemp);
  }
}
*/
void UploadHourdata(INT8U smonth,INT8U sdate,INT8U shour,INT8U emonth,INT8U edate,INT8U ehour)
{
  INT16U pageaddr=0,i,j,k,x;
  INT8U Rdaddr[3]={0,0,0};
  HISDATAFLAGS Sighisdata;
  INT16U count=0;

  FlashMemorytoBuffer(Rdaddr,FLASHBUFFER1);
  Delay_N_mS(200);
  if(smonth<=emonth)
  {
    for(i=0;i<7;i++)
    {
      pageaddr  =120*i;
      Rdaddr[0]=0;
      Rdaddr[1]=(INT8U)(pageaddr>>8);
      Rdaddr[2]=(INT8U)(pageaddr);

      FlashBufferRead(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1); //hour data flag struct

      if((Sighisdata.month<=emonth)&&(Sighisdata.month>=smonth))
      {
        if((Sighisdata.month>smonth)&&(Sighisdata.month<emonth))
        {
          //pageaddr  =Sighisdata.page;
          for(j=0;j<31;j++)
          {
            for(k=0;k<3;k++)
            {
              if(Sighisdata.flag[j][k]!=0)
              {
                for(x=0;x<8;x++)
                {
                  if((Sighisdata.flag[j][k]>>x)&0x01)
                  {
                    pageaddr =Sighisdata.page+j*24+k*8+x;
                    Rdaddr[0]=(INT8U)(pageaddr>>5);
                    Rdaddr[1]=(INT8U)(pageaddr<<3);
                    Rdaddr[2]=0;
                    FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                    SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                    SendData(CBAG.cbag_crc_tail,6,COM1);
                    MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                    count++;
                  }
                }
              }
            }
          }
        }
        else if((Sighisdata.month==smonth)&&(Sighisdata.month<emonth))
        {
          for(j=sdate-1;j<31;j++)
          {
            for(k=0;k<3;k++)
            {
              if(Sighisdata.flag[j][k]!=0)
              {
                for(x=0;x<8;x++)
                {
                  if((Sighisdata.flag[j][k]>>x)&0x01)
                  {
                    pageaddr =Sighisdata.page+j*24+k*8+x;
                    Rdaddr[0]=(INT8U)(pageaddr>>5);
                    Rdaddr[1]=(INT8U)(pageaddr<<3);
                    Rdaddr[2]=0;
                    FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                    SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                    SendData(CBAG.cbag_crc_tail,6,COM1);
                    MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                    count++;
                  }
                }
              }
            }
          }
        }
        else if((Sighisdata.month>smonth)&&(Sighisdata.month=emonth))
        {
          for(j=0;j<edate;j++)
          {
            for(k=0;k<3;k++)
            {
              if(Sighisdata.flag[j][k]!=0)
              {
                for(x=0;x<8;x++)
                {
                  if((Sighisdata.flag[j][k]>>x)&0x01)
                  {
                    pageaddr =Sighisdata.page+j*24+k*8+x;
                    Rdaddr[0]=(INT8U)(pageaddr>>5);
                    Rdaddr[1]=(INT8U)(pageaddr<<3);
                    Rdaddr[2]=0;
                    FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                    SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                    SendData(CBAG.cbag_crc_tail,6,COM1);
                    MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                    count++;
                  }
                }
              }
            }
          }
        }
        else if((Sighisdata.month=smonth)&&(Sighisdata.month=emonth))
        {
          if(sdate==edate)
          {
            j=sdate-1;
            if((shour/8)==(ehour/8))
            {
              k=shour/8;
              for(x=((shour)%8);x<((ehour)%8)+1;x++)
              {
                if((Sighisdata.flag[j][k]>>x)&0x01)
                {
                  pageaddr =Sighisdata.page+j*24+k*8+x;
                  Rdaddr[0]=(INT8U)(pageaddr>>5);
                  Rdaddr[1]=(INT8U)(pageaddr<<3);
                  Rdaddr[2]=0;
                  FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                  SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                  SendData(CBAG.cbag_crc_tail,6,COM1);
                  MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                  count++;
                }
              }
            }
            else
            {
              for(k=shour/8;k<(ehour/8)+1;k++)
              {
                if(k==shour/8)
                {
                  for(x=((shour)%8);x<8;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
                else if(k==ehour/8)
                {
                  for(x=0;x<((ehour)%8)+1;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
                else
                {
                  for(x=0;x<8;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
              }
            }

          }
          else
          {
            for(j=sdate-1;j<edate;j++)
            {
              if(j==sdate-1)
              {
                for(k=shour/8;k<3;k++)
                {
                  if(k==shour/8)
                  {
                    for(x=(shour)%8;x<8;x++)
                    {
                      if((Sighisdata.flag[j][k]>>x)&0x01)
                      {
                        pageaddr =Sighisdata.page+j*24+k*8+x;
                        Rdaddr[0]=(INT8U)(pageaddr>>5);
                        Rdaddr[1]=(INT8U)(pageaddr<<3);
                        Rdaddr[2]=0;
                        FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                        SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                        SendData(CBAG.cbag_crc_tail,6,COM1);
                        MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                        count++;
                      }
                    }
                  }
                  else
                  {
                    for(x=0;x<8;x++)
                    {
                      if((Sighisdata.flag[j][k]>>x)&0x01)
                      {
                        pageaddr =Sighisdata.page+j*24+k*8+x;
                        Rdaddr[0]=(INT8U)(pageaddr>>5);
                        Rdaddr[1]=(INT8U)(pageaddr<<3);
                        Rdaddr[2]=0;
                        FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                        SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                        SendData(CBAG.cbag_crc_tail,6,COM1);
                        MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                        count++;
                      }
                    }
                  }
                }
              }
              else if(j==edate-1)
              {
                for(k=0;k<ehour/8+1;k++)
                {
                  if(k==ehour/8)
                  {
                    for(x=0;x<((ehour)%8)+1;x++)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                  else
                  {
                    for(x=0;x<8;x++)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }

                }
              }
              else
              {
                for(k=0;k<3;k++)
                {
                  for(x=0;x<8;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
              }
            }
          }

        }

      }
    }
  }
  else
  {
    for(i=0;i<7;i++)
    {
      pageaddr  =120*i;
      Rdaddr[0]=0;
      Rdaddr[1]=(INT8U)(pageaddr>>8);
      Rdaddr[2]=(INT8U)(pageaddr);

      FlashBufferRead(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1); //hour data flag struct
      if(Sighisdata.month<=12)
      {
        if((Sighisdata.month>smonth)||(Sighisdata.month<emonth))
        {
          for(j=0;j<31;j++)
          {
            for(k=0;k<3;k++)
            {
              if(Sighisdata.flag[j][k]!=0)
              {
                for(x=0;x<8;x++)
                {
                  if((Sighisdata.flag[j][k]>>x)&0x01)
                  {
                    pageaddr =Sighisdata.page+j*24+k*8+x;
                    Rdaddr[0]=(INT8U)(pageaddr>>5);
                    Rdaddr[1]=(INT8U)(pageaddr<<3);
                    Rdaddr[2]=0;
                    FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                    SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                    SendData(CBAG.cbag_crc_tail,6,COM1);
                    MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                    count++;
                  }
                }
              }
            }
          }
        }
        else if(Sighisdata.month==emonth)
        {
          for(j=0;j<edate;j++)
          {
            if(j==edate-1)
            {
              for(k=0;k<(ehour/8)+1;k++)
              {
                if(k==(ehour/8))
                {
                  for(x=0;x<((ehour)%8)+1;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
                else
                {
                  for(x=0;x<8;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
              }
            }
            else
            {
              for(k=0;k<3;k++)
              {
                if(Sighisdata.flag[j][k]!=0)
                {
                  for(x=0;x<8;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
              }
            }
          }
        }
        else if(Sighisdata.month==smonth)
        {
          for(j=sdate-1;j<31;j++)
          {
            if(j==sdate-1)
            {
              for(k=shour/8;k<3;k++)
              {
                if(k==shour/8)
                {
                  for(x=(shour)%8;x<8;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
                else
                {
                  for(x=0;x<8;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
              }
            }
            else
            {
              for(k=0;k<3;k++)
              {
                if(Sighisdata.flag[j][k]!=0)
                {
                  for(x=0;x<8;x++)
                  {
                    if((Sighisdata.flag[j][k]>>x)&0x01)
                    {
                      pageaddr =Sighisdata.page+j*24+k*8+x;
                      Rdaddr[0]=(INT8U)(pageaddr>>5);
                      Rdaddr[1]=(INT8U)(pageaddr<<3);
                      Rdaddr[2]=0;
                      FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
                      SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
                      SendData(CBAG.cbag_crc_tail,6,COM1);
                      MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
                      count++;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  if(count!=0)
  {
    e_result[0]='1';//success
    Response(2);
  }
  else
  {
    e_result[0]='1';//no data
    e_result[1]='0';
    e_result[2]='0';
    Response(2);
    e_result[1]=0;
    e_result[2]=0;
  }
}
/*
void UploadHourdata(INT8U smonth,INT8U sdate,INT8U shour,INT8U emonth,INT8U edate,INT8U ehour)
{
  INT8U i,j;
  INT8U flashaddr[3];
  INT8U temp[25];
  INT8U datatemp[512];
  INT16U addrtemp,count,lenth;

  if(sdate<edate)
  {
    count=0;
    for(i=sdate;i<=edate;i++)
    {
      //addrtemp=(sdate-1+i)*0x20;
      addrtemp=(i-1)*0x20;
      flashaddr[0]=0x00;
      flashaddr[1]=(INT8U)(addrtemp>>8);
      flashaddr[2]=(INT8U)addrtemp;
      FlashMemoryRead(flashaddr,temp,1);  //�����±�ǩ

      if((temp[0]==smonth))
      {
        addrtemp+=2;
        flashaddr[1]=(INT8U)(addrtemp>>8);
        flashaddr[2]=(INT8U)addrtemp;
        FlashMemoryRead(flashaddr,temp,24); //����Сʱ���ݱ�ǩ

        if(i==sdate)
        {
          for(j=shour;j<24;j++)
          {
            if(temp[j]==0x01)
            {
              addrtemp=(sdate-1)*24+j+0x10;  //ҳ��ַ
              flashaddr[0]=(INT8U)(addrtemp>>5);
              flashaddr[1]=(INT8U)(addrtemp<<3);
              flashaddr[2]=0x00;

              FlashMemoryRead(flashaddr,datatemp,2);//read the length of the hourdata
              lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];
              flashaddr[2]=0x10;
              FlashMemoryRead(flashaddr,datatemp,lenth);//��������
              datatemp[lenth]=0;
              Symbol.ccc=1;
              Response(6);
              Symbol.ccc=0;
              strcat(CBAG.cbag_dynamic,datatemp);
              Add_Send();//�������
              MemoryClear(datatemp,lenth);//clear datatemp
              count++;
            }
          }
        }
        else if(i==edate)
        {
          for(j=0;j<=ehour;j++)
          {
            if(temp[j]==0x01)
            {
              addrtemp=(edate-1)*24+j+0x10;  //ҳ��ַ
              flashaddr[0]=(INT8U)(addrtemp>>5);
              flashaddr[1]=(INT8U)(addrtemp<<3);
              flashaddr[2]=0x00;

              FlashMemoryRead(flashaddr,datatemp,2);//read the length of the hourdata
              lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];
              flashaddr[2]=0x10;
              FlashMemoryRead(flashaddr,datatemp,lenth);//��������
              datatemp[lenth]=0;
              Symbol.ccc=1;
              Response(6);
              Symbol.ccc=0;
              strcat(CBAG.cbag_dynamic,datatemp);
              Add_Send();//�������
              MemoryClear(datatemp,lenth);//clear datatemp
              count++;
            }
          }
        }
        else  //���ڿ�ʼ����������м��ĳ�죬24Сʱ���ݾ���Ч
        {
          for(j=0;j<24;j++)
          {
            if(temp[j]==0x01)
            {
              addrtemp=(i-1)*24+j+0x10;  //ҳ��ַ
              flashaddr[0]=(INT8U)(addrtemp>>5);
              flashaddr[1]=(INT8U)(addrtemp<<3);
              flashaddr[2]=0x00;

              FlashMemoryRead(flashaddr,datatemp,2);//read the length of the hourdata
              lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];
              flashaddr[2]=0x10;
              FlashMemoryRead(flashaddr,datatemp,lenth);//��������
              strcat(CBAG.cbag_dynamic,datatemp);
              datatemp[lenth]=0;
              Symbol.ccc=1;
              Response(6);
              Symbol.ccc=0;
              Add_Send();//�������
              MemoryClear(datatemp,lenth);//clear datatemp
              count++;
            }
          }
        }
      }
    }
    if(count!=0)
    {
      e_result[0]='1';//success
      Response(2);
    }
    else
    {
      e_result[0]='1';//no data
      e_result[1]='0';
      e_result[2]='0';
      Response(2);
      e_result[1]=0;
      e_result[2]=0;
    }
  }
  else if(sdate==edate)
  {
    addrtemp=(sdate-1)*0x20;
    flashaddr[0]=0x00;
    flashaddr[1]=(INT8U)(addrtemp>>8);
    flashaddr[2]=(INT8U)addrtemp;
    FlashMemoryRead(flashaddr,temp,1);  //�����±�ǩ

    count=0;
    if((temp[0]==smonth))
    {
      addrtemp+=2;
      flashaddr[1]=(INT8U)(addrtemp>>8);
      flashaddr[2]=(INT8U)addrtemp;
      FlashMemoryRead(flashaddr,temp,24); //����Сʱ���ݱ�ǩ

      count=0;
      for(i=shour;i<=ehour;i++)
      {
        if(temp[i]==0x01)
        {
          addrtemp=(sdate-1)*24+i+0x10;  //ҳ��ַ
          flashaddr[0]=(INT8U)(addrtemp>>5);
          flashaddr[1]=(INT8U)(addrtemp<<3);
          flashaddr[2]=0x00;

          FlashMemoryRead(flashaddr,datatemp,2);//read the length of the hourdata
          lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];
          flashaddr[2]=0x10;
          FlashMemoryRead(flashaddr,datatemp,lenth);//��������
          datatemp[lenth]=0;
          Symbol.ccc=1;
          Response(6);
          Symbol.ccc=0;
          strcat(CBAG.cbag_dynamic,datatemp);
          Add_Send();//�������
          MemoryClear(datatemp,lenth);//clear datatemp
          count++;
        }
      }
    }
    if(count!=0)
    {
      e_result[0]='1';//success
      Response(2);
    }
    else
    {
      e_result[0]='1';//no data
      e_result[1]='0';
      e_result[2]='0';
      Response(2);
      e_result[1]=0;
      e_result[2]=0;
    }
  }
  else
  {
    count=0;
    for(i=sdate;i<=31;i++)
    {
      addrtemp=(i-1)*0x20;
      flashaddr[0]=0x00;
      flashaddr[1]=(INT8U)(addrtemp>>8);
      flashaddr[2]=(INT8U)addrtemp;
      FlashMemoryRead(flashaddr,temp,1);  //�����±�ǩ

      if(temp[0]==smonth)
      {
        addrtemp+=2;
        flashaddr[1]=(INT8U)(addrtemp>>8);
        flashaddr[2]=(INT8U)addrtemp;
        FlashMemoryRead(flashaddr,temp,24); //����Сʱ���ݱ�ǩ

        if(i==sdate)
        {
          for(j=shour;j<24;j++)
          {
            if(temp[j]==0x01)
            {
              addrtemp=(sdate-1)*24+j+0x10;  //ҳ��ַ
              flashaddr[0]=(INT8U)(addrtemp>>5);
              flashaddr[1]=(INT8U)(addrtemp<<3);
              flashaddr[2]=0x00;

              FlashMemoryRead(flashaddr,datatemp,2);//read the length of the hourdata
              lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];
              flashaddr[2]=0x10;
              FlashMemoryRead(flashaddr,datatemp,lenth);//��������
              datatemp[lenth]=0;
              Symbol.ccc=1;
              Response(6);
              Symbol.ccc=0;
              strcat(CBAG.cbag_dynamic,datatemp);
              Add_Send();//�������
              MemoryClear(datatemp,lenth);//clear datatemp
              count++;
            }
          }
        }
        else  //���ڿ�ʼ����������м��ĳ�죬24Сʱ���ݾ���Ч
        {
          for(j=0;j<24;j++)
          {
            if(temp[j]==0x01)
            {
              addrtemp=(sdate-1)*24+j+0x10;  //ҳ��ַ
              flashaddr[0]=(INT8U)(addrtemp>>5);
              flashaddr[1]=(INT8U)(addrtemp<<3);
              flashaddr[2]=0x00;

              FlashMemoryRead(flashaddr,datatemp,2);//read the length of the hourdata
              lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];
              flashaddr[2]=0x10;
              FlashMemoryRead(flashaddr,datatemp,lenth);//��������
              datatemp[lenth]=0;
              Symbol.ccc=1;
              Response(6);
              Symbol.ccc=0;
              strcat(CBAG.cbag_dynamic,datatemp);
              Add_Send();//�������
              MemoryClear(datatemp,lenth);//clear datatemp
              count++;
            }
          }
        }
      }

    }
    for(i=1;i<=edate;i++)
    {
      addrtemp=(i-1)*0x20;
      flashaddr[0]=0x00;
      flashaddr[1]=(INT8U)(addrtemp>>8);
      flashaddr[2]=(INT8U)addrtemp;
      FlashMemoryRead(flashaddr,temp,1);  //�����±�ǩ

      if(temp[0]==emonth)
      {
        addrtemp+=2;
        flashaddr[1]=(INT8U)(addrtemp>>8);
        flashaddr[2]=(INT8U)addrtemp;
        FlashMemoryRead(flashaddr,temp,24); //����Сʱ���ݱ�ǩ

        if(i==edate)
        {
          for(j=0;j<=ehour;j++)
          {
            if(temp[j]==0x01)
            {
              addrtemp=(edate-1)*24+j+0x10;  //ҳ��ַ
              flashaddr[0]=(INT8U)(addrtemp>>5);
              flashaddr[1]=(INT8U)(addrtemp<<3);
              flashaddr[2]=0x00;

              FlashMemoryRead(flashaddr,datatemp,2);//read the length of the hourdata
              lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];
              flashaddr[2]=0x10;
              FlashMemoryRead(flashaddr,datatemp,lenth);//��������
              datatemp[lenth]=0;
              Symbol.ccc=1;
              Response(6);
              Symbol.ccc=0;
              strcat(CBAG.cbag_dynamic,datatemp);
              Add_Send();//�������
              MemoryClear(datatemp,lenth);//clear datatemp
              count++;
            }
          }
        }
        else  //���ڿ�ʼ����������м��ĳ�죬24Сʱ���ݾ���Ч
        {
          for(j=0;j<24;j++)
          {
            if(temp[j]==0x01)
            {
              addrtemp=(i-1)*24+j+0x10;  //ҳ��ַ
              flashaddr[0]=(INT8U)(addrtemp>>5);
              flashaddr[1]=(INT8U)(addrtemp<<3);
              flashaddr[2]=0x00;

              FlashMemoryRead(flashaddr,datatemp,2);//read the length of the hourdata
              lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];
              flashaddr[2]=0x10;
              FlashMemoryRead(flashaddr,datatemp,lenth);//��������
              datatemp[lenth]=0;
              Symbol.ccc=1;
              Response(6);
              Symbol.ccc=0;
              strcat(CBAG.cbag_dynamic,datatemp);
              //CBAG.cbag_dynamic[lenth+5]=0;
              Add_Send();//�������
              MemoryClear(datatemp,lenth);//clear datatemp
              count++;
            }
          }
        }
      }
    }

    if(count!=0)
    {
      e_result[0]='1';//success
      Response(2);
    }
    else
    {
      e_result[0]='1';//no data
      e_result[1]='0';
      e_result[2]='0';
      Response(2);
      e_result[1]=0;
      e_result[2]=0;
    }
  }
}
*/
/*
  ��ʷ���ݴ洢��ʼҳ��ַΪ1000.���洢30��������
*/
void ReportDaydata(INT8U smonth,INT8U sdate,INT8U emonth,INT8U edate)
{
  INT16U pageaddr=0,i,j;
  INT8U Rdaddr[3]={0,0x28,0};
  HISDATAFLAGS_D Sig_d;
  INT16U count=0;

  FlashMemorytoBuffer(Rdaddr,FLASHBUFFER1);
  Delay_N_mS(200);
  if(smonth<emonth)
  {
    for(i=0;i<13;i++)
    {
      pageaddr  =80*i;
      Rdaddr[0]=0;
      Rdaddr[1]=((INT8U)(pageaddr>>8))|0x28;
      Rdaddr[2]=(INT8U)(pageaddr);

      FlashBufferRead(Rdaddr,&Sig_d,sizeof(Sig_d),FLASHBUFFER1); //day data flag struct
      if(Sig_d.month==smonth)
      {
        for(j=sdate-1;j<31;j++)
        {
          if(Sig_d.flag[j]==0x01)
          {
            pageaddr =Sig_d.page+j;
            Rdaddr[0]=(INT8U)(pageaddr>>5);
            Rdaddr[1]=(INT8U)(pageaddr<<3);
            Rdaddr[2]=0;
            FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
            SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
            SendData(CBAG.cbag_crc_tail,6,COM1);
            count++;
          }
        }
      }
      else if(Sig_d.month==emonth)
      {
        for(j=0;j<edate;j++)
        {
          if(Sig_d.flag[j]==0x01)
          {
            pageaddr =Sig_d.page+j;
            Rdaddr[0]=(INT8U)(pageaddr>>5);
            Rdaddr[1]=(INT8U)(pageaddr<<3);
            Rdaddr[2]=0;
            FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
            SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
            SendData(CBAG.cbag_crc_tail,6,COM1);
            count++;
          }
        }
      }
      else if((Sig_d.month<emonth)&&(Sig_d.month>smonth))
      {
        for(j=0;j<31;j++)
        {
          if(Sig_d.flag[j]==0x01)
          {
            pageaddr =Sig_d.page+j;
            Rdaddr[0]=(INT8U)(pageaddr>>5);
            Rdaddr[1]=(INT8U)(pageaddr<<3);
            Rdaddr[2]=0;
            FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
            SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
            SendData(CBAG.cbag_crc_tail,6,COM1);
            count++;
          }
        }
      }
    }
  }
  else if(smonth==emonth)
  {
    for(i=0;i<13;i++)
    {
      pageaddr  =80*i;
      Rdaddr[0]=0;
      Rdaddr[1]=((INT8U)(pageaddr>>8))|0x28;
      Rdaddr[2]=(INT8U)(pageaddr);

      FlashBufferRead(Rdaddr,&Sig_d,sizeof(Sig_d),FLASHBUFFER1); //day data flag struct
      if(Sig_d.month==smonth)
      {
         for(j=sdate-1;j<edate;j++)
         {
            if(Sig_d.flag[j]==0x01)
            {
              pageaddr =Sig_d.page+j;
              Rdaddr[0]=(INT8U)(pageaddr>>5);
              Rdaddr[1]=(INT8U)(pageaddr<<3);
              Rdaddr[2]=0;
              FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
              SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
              SendData(CBAG.cbag_crc_tail,6,COM1);
              count++;
            }
         }
      }
    }
  }
  else
  {
    for(i=0;i<13;i++)
    {
      pageaddr  =80*i;
      Rdaddr[0]=0;
      Rdaddr[1]=((INT8U)(pageaddr>>8))|0x28;
      Rdaddr[2]=(INT8U)(pageaddr);

      FlashBufferRead(Rdaddr,&Sig_d,sizeof(Sig_d),FLASHBUFFER1); //day data flag struct
      if(Sig_d.month==smonth)
      {
        for(j=sdate-1;j<31;j++)
         {
            if(Sig_d.flag[j]==0x01)
            {
              pageaddr =Sig_d.page+j;
              Rdaddr[0]=(INT8U)(pageaddr>>5);
              Rdaddr[1]=(INT8U)(pageaddr<<3);
              Rdaddr[2]=0;
              FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
              SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
              SendData(CBAG.cbag_crc_tail,6,COM1);
              count++;
            }
         }
      }
      else if(Sig_d.month==emonth)
      {
        for(j=0;j<emonth;j++)
       {
          if(Sig_d.flag[j]==0x01)
          {
            pageaddr =Sig_d.page+j;
            Rdaddr[0]=(INT8U)(pageaddr>>5);
            Rdaddr[1]=(INT8U)(pageaddr<<3);
            Rdaddr[2]=0;
            FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
            SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
            SendData(CBAG.cbag_crc_tail,6,COM1);
            count++;
          }
       }
      }
      else if((Sig_d.month>smonth)||(Sig_d.month<emonth))
      {
        for(j=0;j<31;j++)
       {
          if(Sig_d.flag[j]==0x01)
          {
            pageaddr =Sig_d.page+j;
            Rdaddr[0]=(INT8U)(pageaddr>>5);
            Rdaddr[1]=(INT8U)(pageaddr<<3);
            Rdaddr[2]=0;
            FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
            SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
            SendData(CBAG.cbag_crc_tail,6,COM1);
            count++;
          }
       }
      }
    }
  }
  if(count!=0)
  {
    MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
    e_result[0]='1';//success
    Response(2);
  }
  else
  {
    e_result[0]='1';//no data
    e_result[1]='0';
    e_result[2]='0';
    Response(2);
    e_result[1]=0;
    e_result[2]=0;
  }
}


void UploadRealdata(INT8U sday,INT8U shour)
{
  INT16U pageaddr=0,i;
  INT8U Rdaddr[3]={0,0x50,0};
  HISDATAFLAGS_R Sig_r;
  INT16U count=0;

  FlashMemorytoBuffer(Rdaddr,FLASHBUFFER1);
  Delay_N_mS(200);
  if(sday==current_date)
  {
    pageaddr =2*200;
  }
  else    //yestoday
  {
    pageaddr =1*200;
  }

    Rdaddr[1]=((INT8U)(pageaddr>>8))|0x50;
    Rdaddr[2]=(INT8U)(pageaddr);
    FlashBufferRead(Rdaddr,&Sig_r,sizeof(Sig_r),FLASHBUFFER1); //real data flag struct

  if(Sig_r.day==sday)
  {
    if(Sig_r.flag[shour]!=0)
    {
      INT8U tmp[3]={0,0,0};
      for(i=0;i<12;i++)
      {
        if(Sig_r.flag[shour]&(0x0001<<i))
        {
          pageaddr =Sig_r.page+shour*12+i;
          Rdaddr[0]=(INT8U)(pageaddr>>5);
          Rdaddr[1]=(INT8U)(pageaddr<<3);
          Rdaddr[2]=0;
          FlashMemoryRead(Rdaddr,tmp,2);
          if((tmp[0]=='#')&&(tmp[1]=='#'))
          {
            FlashMemoryRead(Rdaddr,&CBAG,sizeof(CBAG));
            SendData(CBAG.cbag_head,strlen(CBAG.cbag_head),COM1);
            SendData(CBAG.cbag_crc_tail,6,COM1);
            //MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
            count++;
          }
        }
      }
    }
    else
      count =0;
  }
  else
  {
     count =0;
  }
  if(count!=0)
  {
    MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
    e_result[0]='1';//success
    Response(2);
  }
  else
  {
    e_result[0]='1';//no data
    e_result[1]='0';
    e_result[2]='0';
    Response(2);
    e_result[1]=0;
    e_result[2]=0;
  }
}
/*
void ReportDaydata(INT8U smonth,INT8U sdate,INT8U emonth,INT8U edate)
{
  INT8U dataddr[3]={0,0,0};
  INT16U pageaddr;
  INT16U i,lenth,count=0;
  INT8U datatemp[512];

  if(sdate>edate)
  {
    for(i=sdate;i<=31;i++)
    {
      pageaddr  =1000;
      dataddr[0]  =(INT8U)(pageaddr>>5);
      dataddr[1]  =(INT8U)(pageaddr<<3);
      dataddr[2]  =i;
      FlashMemoryRead(dataddr,datatemp,1);
      if((datatemp[0]>=smonth)&&(datatemp[0]<=emonth))
      {
        pageaddr  =1000+i;
        dataddr[0]  =(INT8U)(pageaddr>>5);
        dataddr[1]  =(INT8U)(pageaddr<<3);
        dataddr[2]  =0x00;

        FlashMemoryRead(dataddr,datatemp,2);
        lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];

        dataddr[2]=0x10;
        FlashMemoryRead(dataddr,datatemp,lenth);//��������
        datatemp[lenth]=0;
        Symbol.ccc=1;
        Response(6);
        Symbol.ccc=0;
        strcat(CBAG.cbag_dynamic,datatemp);
        //CBAG.cbag_dynamic[lenth+5]=0;
        Add_Send();//�������
        MemoryClear(datatemp,lenth);//clear datatemp
        count++;
      }
    }
    for(i=1;i<=edate;i++)
    {
      pageaddr  =1000;
      dataddr[0]  =(INT8U)(pageaddr>>5);
      dataddr[1]  =(INT8U)(pageaddr<<3);
      dataddr[2]  =i;
      FlashMemoryRead(dataddr,datatemp,1);
      if((datatemp[0]>=smonth)&&(datatemp[0]<=emonth))
      {
        pageaddr  =1000+i;
        dataddr[0]  =(INT8U)(pageaddr>>5);
        dataddr[1]  =(INT8U)(pageaddr<<3);
        dataddr[2]  =0x00;

        FlashMemoryRead(dataddr,datatemp,2);
        lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];

        dataddr[2]=0x10;
        FlashMemoryRead(dataddr,datatemp,lenth);//��������
        datatemp[lenth]=0;
        Symbol.ccc=1;
        Response(6);
        Symbol.ccc=0;
        strcat(CBAG.cbag_dynamic,datatemp);
        //CBAG.cbag_dynamic[lenth+5]=0;
        Add_Send();//�������
        MemoryClear(datatemp,lenth);//clear datatemp
        count++;
      }
    }

  }
  else if(sdate<edate)
  {
    for(i=sdate;i<=edate;i++)
    {
      pageaddr  =1000;
      dataddr[0]  =(INT8U)(pageaddr>>5);
      dataddr[1]  =(INT8U)(pageaddr<<3);
      dataddr[2]  =i;
      FlashMemoryRead(dataddr,datatemp,1);
      if((datatemp[0]>=smonth)&&(datatemp[0]<=emonth))
      {
        pageaddr  =1000+i;
        dataddr[0]  =(INT8U)(pageaddr>>5);
        dataddr[1]  =(INT8U)(pageaddr<<3);
        dataddr[2]  =0x00;

        FlashMemoryRead(dataddr,datatemp,2);
        lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];

        dataddr[2]=0x10;
        FlashMemoryRead(dataddr,datatemp,lenth);//��������
        datatemp[lenth]=0;
        Symbol.ccc=1;
        Response(6);
        Symbol.ccc=0;
        strcat(CBAG.cbag_dynamic,datatemp);
        //CBAG.cbag_dynamic[lenth+5]=0;
        Add_Send();//�������
        MemoryClear(datatemp,lenth);//clear datatemp
        count++;
      }
    }
  }
  else if(sdate==edate)
  {
    pageaddr  =1000;
    dataddr[0]  =(INT8U)(pageaddr>>5);
    dataddr[1]  =(INT8U)(pageaddr<<3);
    dataddr[2]  =sdate;
    FlashMemoryRead(dataddr,datatemp,1);
    if((datatemp[0]>=smonth)&&(datatemp[0]<=emonth))
    {
      pageaddr  =1000+sdate;
      dataddr[0]  =(INT8U)(pageaddr>>5);
      dataddr[1]  =(INT8U)(pageaddr<<3);
      dataddr[2]  =0x00;

      FlashMemoryRead(dataddr,datatemp,2);
      lenth=(((INT16U)datatemp[0])<<8)|datatemp[1];

      dataddr[2]=0x10;
      FlashMemoryRead(dataddr,datatemp,lenth);//��������
      datatemp[lenth]=0;
      Symbol.ccc=1;
      Response(6);
      Symbol.ccc=0;
      strcat(CBAG.cbag_dynamic,datatemp);
      //CBAG.cbag_dynamic[lenth+5]=0;
      Add_Send();//�������
      MemoryClear(datatemp,lenth);//clear datatemp
      count++;
    }
  }
  if(count!=0)
  {
    e_result[0]='1';//success
    Response(2);
  }
  else
  {
    e_result[0]='1';//no data
    e_result[1]='0';
    e_result[2]='0';
    Response(2);
    e_result[1]=0;
    e_result[2]=0;
  }
}
*/
void ReportAlarm(void)
{
  INT8U counttemp;
  INT8U lens[3];
  INT8U CRC16[2];
  INT16U len;

  for(counttemp=0;counttemp<8;counttemp++)
  {
    if((RealData.R_D.FF.ADF[counttemp]>ADPARA(counttemp).U_Alarmlimt.b)||(RealData.R_D.FF.ADF[counttemp]<ADPARA(counttemp).L_Alarmlimt.b))
    {
      INT16U disa,length;
      INT8U tempstr[4];
      INT8U flashaddr[3];
      INT16U pageaddr;
      INT8U ftstring[18]="\0";
      struct _TEMP
        {
          INT8U head[2];
          INT8U len[4];
          INT8U QN[20];
          INT8U ST[6];
          INT8U CN[8];
          INT8U PW[10];
          INT8U MN[18];
          INT8U FLAG[8];
          INT8U CPhead[29];
          INT8U comma;
          INT8U data[32];
          INT8U CRC[4];
          INT8U tail[2];
        };
      struct _TEMP alarmtemp={
        .head={'#','#'},
        .QN="QN=",
        .ST=";ST=",
        .CN={';','C','N','=','2','0','7','2'},
        .PW=";PW=",
        .MN=";MN=",
        .FLAG={';','F','l','a','g','=','0',';'},
        .CPhead="CP=&&AlarmTime=",
        .comma=';',
        .data="-Ala=,AlarmType=1&&\0",
        .tail={0x0d,0x0a}
      };

      for(disa=0;disa<14;disa++)
      {
        alarmtemp.QN[3+disa]=System_Time[disa];
        alarmtemp.CPhead[15+disa]=System_Time[disa];
      }
      for(disa=0;disa<3;disa++)
        alarmtemp.QN[3+14+disa]='0';
      for(disa=0;disa<2;disa++)
        alarmtemp.ST[4+disa]=parameter.sys_parameter.st[disa];
      for(disa=0;disa<6;disa++)
        alarmtemp.PW[4+disa]=parameter.sys_parameter.pw[disa];
      for(disa=0;disa<14;disa++)
        alarmtemp.MN[4+disa]=parameter.sys_parameter.mn[disa];
      float_to_string(RealData.R_D.FF.ADF[counttemp],ftstring);
      insert(alarmtemp.data,ftstring,6);
      for(disa=0;disa<3;disa++)
        tempstr[disa]=ADPARA(counttemp).po_id[disa];
      tempstr[3]=0;
      insert(alarmtemp.data,tempstr,1);

      disa=100+strlen(alarmtemp.data);

      length=strlen(alarmtemp.QN);
      CRC_Calculation(alarmtemp.QN,length,CRC16);
      StrToASCII(CRC16,tempstr,2);
      for(disa=0;disa<4;disa++)
      {
        alarmtemp.CRC[disa]=tempstr[disa];
      }
      alarmtemp.len[0]=(length/1000)|0x30;         //��lengthת�����ַ�����������len[5]��
      alarmtemp.len[1]=((length%1000)/100)|0x30;
      alarmtemp.len[2]=((length%100)/10)|0x30;
      alarmtemp.len[3]=(length%10)|0x30;
      SendData(alarmtemp.head,strlen(alarmtemp.head),COM1);
      SendData(alarmtemp.CRC,6,COM1);

      if(Alarmtimes>=10)
        Alarmtimes=0;
      pageaddr  =2000;
      flashaddr[0]  =(INT8U)(pageaddr>>5);
      flashaddr[1]  =(INT8U)(pageaddr<<3);
      flashaddr[2]  =Alarmtimes*0x10;
      FlashMemoryWrite(flashaddr,System_Time,14,FLASHBUFFER2);//д�뱨��ʱ���ǩ

      pageaddr  =2000+Alarmtimes+1;
      flashaddr[0]  =(INT8U)(pageaddr>>5);
      flashaddr[1]  =(INT8U)(pageaddr<<3);
      flashaddr[2]  =0x00;

      len =strlen(&alarmtemp.CPhead[15])-14;
      lens[0]=(INT8U)(len>>8);
      lens[1]=(INT8U)len;
      FlashBufferWrite(flashaddr,lens,2,FLASHBUFFER1);
      flashaddr[2]  =0x10;
      FlashMemoryWrite(flashaddr,&alarmtemp.CPhead[15],len,FLASHBUFFER1);

      Alarmtimes++;
    }
  }
}

void ReportAlarmRecord(INT8U *start,INT8U *end)
{
  INT8U flashaddr[3];
  INT8U i;
  INT16U pageaddr,len,count=0;
  INT8U lens[3];
  INT8U datatemp[200];


  for(i=0;i<10;i++)
  {
    pageaddr  =2000;
    flashaddr[0]  =(INT8U)(pageaddr>>5);
    flashaddr[1]  =(INT8U)(pageaddr<<3);
    flashaddr[2]  =i*0x10;
    FlashMemoryRead(flashaddr,datatemp,14);
    datatemp[14]  =0;
    if((strcmp(start,datatemp)<=0)&&(strcmp(end,datatemp)>=0))
    {
      pageaddr  =2000+i+1;
      flashaddr[0]  =(INT8U)(pageaddr>>5);
      flashaddr[1]  =(INT8U)(pageaddr<<3);
      flashaddr[2]  =0x00;
      FlashMemoryRead(flashaddr,lens,2);
      len =(lens[0]<<8)|lens[1];

      flashaddr[2]  =0x10;
      MemoryClear(datatemp,200);//clear datatemp
      FlashMemoryRead(flashaddr,datatemp,len);
      //datatemp[len] =0;

      Symbol.ccc=1;
      Response(6);
      Symbol.ccc=0;

      strcat(CBAG.cbag_dynamic,"DataTime=");
      strcat(CBAG.cbag_dynamic,datatemp);
      //CBAG.cbag_dynamic[lenth+5]=0;
      Add_Send();//�������

      count++;
    }
  }
  if(count!=0)
  {
    e_result[0]='1';//success
    Response(2);
  }
  else
  {
    e_result[0]='1';//no data
    e_result[1]='0';
    e_result[2]='0';
    Response(2);
    e_result[1]=0;
    e_result[2]=0;
  }
}

void Device_Status(void)
{
  INT16U i,j;
  //INT8U SBx[]=";SB1-RS=\0\0";
  Symbol.ccc=1;
  Response(8);
  Symbol.ccc=0;
  strcat(CBAG.cbag_dynamic,"DataTime=");
  strcat(CBAG.cbag_dynamic,System_Time);
  for(i=0;i<8;i++)
  {
    INT8U SBx[]=";SB1-RS=\0\0";
    SBx[8]=((RealData.R_D.DO>>i)&0X01)+0X30;
    SBx[3]=i+0X31;
    strcat(CBAG.cbag_dynamic,SBx);
  }
  j=strlen(CBAG.cbag_dynamic);
  CBAG.cbag_dynamic[j]='\0';
  Add_Send();
}

void Reporthisdata_M(void)
{
  INT8U cps[]="DataTime=\0";
  INT8U min[]="-Min=\0";
  INT8U avg[]="-Avg=\0";
  INT8U max[]="-Max=\0";
  INT8U ftstring[18]="\0";
  INT8U timetmp[16];
  INT8U i,j;
  INT8U d[10]="\0";
  //INT16U pageaddr=0;
  //INT8U Rdaddr[3];
  //HISDATAFLAGS Sighisdata;

  //MemoryClear(&Sighisdata,sizeof(Sighisdata));
  Symbol.ccc  =1;
  Response(12);
  Symbol.ccc=0;
  strcat(CBAG.cbag_dynamic,cps);
  MemoryClear(timetmp,16);
  MemoryCopy(System_Time,timetmp,12);
  timetmp[13]=0x30;
  timetmp[12]=0x30;
  //timetmp[11]=0x30;
  //timetmp[10]=0x30;
  strcat(CBAG.cbag_dynamic,timetmp);
  for(i=0;i<8;i++)
  {
    if((ADPARA(i).po_id[0]=='0')&&(ADPARA(i).po_id[1]=='0')&&(ADPARA(i).po_id[2]=='0'))
      continue;
    if(local_mma_M[i].num==0)
          local_mma_M[i].avg =0;
    else
      local_mma_M[i].avg  =local_mma_M[i].sum/local_mma_M[i].num;
    //local_mma_D[i].avg +=local_mma_H[i].avg;  //����Сʱ�ۼ�ֵ
    //local_mma_D[i].num++;
    for(j=0;j<3;j++)
    {
      d[j]=ADPARA(i).po_id[j];
      //d[j]=AD_parameter.AD_i[i].po_id[j];
    }
    d[3]='\0';
      
    strcat(CBAG.cbag_dynamic,semicolon);	//�ӷֺ�

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_M[i].min,ftstring);
    strcat(CBAG.cbag_dynamic,min);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		
    strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_M[i].avg,ftstring);
    strcat(CBAG.cbag_dynamic,avg);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		
    strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_M[i].max,ftstring);
    strcat(CBAG.cbag_dynamic,max);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		

    if((!strcmp(d,"B01")))
    {
      INT8U cou[]="-Cou=\0";
      float coutmp;
      strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���
      strcat(CBAG.cbag_dynamic,d);
      /*������Ҫ���ˮ����ʵʱֵ��λΪL/s,�ۼ�ֵ��λΪm3*/
      coutmp =local_mma_M[i].sum/1000; //L/s ��λת��L->m3
      //local_mma_D[i].sum +=coutmp;
      //coutmp =local_mma_H[i].avg;  //m3/h
      //coutmp =b01h_cou;
      //local_mma_H[i].avg
      float_to_string(coutmp,ftstring);
      strcat(CBAG.cbag_dynamic,cou);
      strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		

      //Min_B01_avg[current_hour] =coutmp;
    }
    b01h_cou =0;
    
  }
  INT16U tempnum_ch=0;
  for(i=0;i<6;i++)
    tempnum_ch  +=Channel_Num[i];
  if(tempnum_ch!=0)
  {
    ModataUpload(Mod_MinD);
    //MemoryClear(dev_mma_H,sizeof(dev_mma_H));
  }
  //strcat(CBAG.cbag_dynamic,datatemp);
  //CBAG.cbag_dynamic[lenth+5]=0;
  add_send();//�������

  MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
  MemoryClear(local_mma_M,sizeof(local_mma_M));
  MemoryClear(dev_mma_M,sizeof(dev_mma_M));
}
void Reporthisdata_H(void)
{
  INT8U cps[]="DataTime=\0";
  INT8U min[]="-Min=\0";
  INT8U avg[]="-Avg=\0";
  INT8U max[]="-Max=\0";
  INT8U ftstring[18]="\0";
  INT8U timetmp[16];
  INT8U i,j;
  INT8U d[10]="\0";
  INT16U pageaddr=0;
  INT8U Rdaddr[3];
  HISDATAFLAGS Sighisdata;

  MemoryClear(&Sighisdata,sizeof(Sighisdata));
  Symbol.ccc  =1;
  Response(10);
  Symbol.ccc=0;
  strcat(CBAG.cbag_dynamic,cps);
  MemoryClear(timetmp,16);
  MemoryCopy(System_Time,timetmp,10);
  timetmp[13]=0x30;
  timetmp[12]=0x30;
  timetmp[11]=0x30;
  timetmp[10]=0x30;
  strcat(CBAG.cbag_dynamic,timetmp);
  for(i=0;i<8;i++)
  {
    if((ADPARA(i).po_id[0]=='0')&&(ADPARA(i).po_id[1]=='0')&&(ADPARA(i).po_id[2]=='0'))
      continue;
    if(local_mma_H[i].num==0)
          local_mma_H[i].avg =0;
    else
      local_mma_H[i].avg  =local_mma_H[i].sum/local_mma_H[i].num;
    local_mma_D[i].avg +=local_mma_H[i].avg;  //����Сʱ�ۼ�ֵ
    local_mma_D[i].num++;
    for(j=0;j<3;j++)
    {
      d[j]=ADPARA(i).po_id[j];
      //d[j]=AD_parameter.AD_i[i].po_id[j];
    }
    d[3]='\0';
      
    strcat(CBAG.cbag_dynamic,semicolon);	//�ӷֺ�

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_H[i].min,ftstring);
    strcat(CBAG.cbag_dynamic,min);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		
    strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_H[i].avg,ftstring);
    strcat(CBAG.cbag_dynamic,avg);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		
    strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_H[i].max,ftstring);
    strcat(CBAG.cbag_dynamic,max);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		

    if((!strcmp(d,"B01")))
    {
      INT8U cou[]="-Cou=\0";
      float coutmp;
      strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���
      strcat(CBAG.cbag_dynamic,d);
      /*������Ҫ���ˮ����ʵʱֵ��λΪL/s,�ۼ�ֵ��λΪm3*/
      coutmp =local_mma_H[i].sum/1000; //L/s ��λת��L->m3
      local_mma_D[i].sum +=coutmp;
      //coutmp =local_mma_H[i].avg;  //m3/h
      //coutmp =b01h_cou;
      //local_mma_H[i].avg
      float_to_string(coutmp,ftstring);
      strcat(CBAG.cbag_dynamic,cou);
      strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		

      Hour_B01_avg[current_hour] =coutmp;
    }
    b01h_cou =0;
    
  }
  INT16U tempnum_ch=0;
  for(i=0;i<6;i++)
    tempnum_ch  +=Channel_Num[i];
  if(tempnum_ch!=0)
  {
    ModataUpload(Mod_HourD);
    //MemoryClear(dev_mma_H,sizeof(dev_mma_H));
  }
  //strcat(CBAG.cbag_dynamic,datatemp);
  //CBAG.cbag_dynamic[lenth+5]=0;
  add_send();//�������

  /**********************************************************************
                               ��ǩ��ʽ
  ��ǩҳ������Flash�ĵ�һҳ�У�ҳ��ַ0x00��ÿ32���ֽ�Ϊһ������ÿ����һ��
  �ֽ����ڴ��浱ǰ�±�ǩ���ӵ������ֽ���ƫ�Ƶ�ַΪ ��ǰʱ��0x02��ΪСʱ
  ���ݱ�ǩ��0x01��ʾ��ǰСʱ�����ݣ�����Ϊ0x00
  ����ѯ��ʷ����ʱ�����Ȳ�ѯ��ǩҳ������Ҫ��ʱ�䷶Χ�Ƿ�������
  **********************************************************************/
  //FlashMemoryRead(Rdaddr,Sighisdata,sizeof(Sighisdata));  //hour data flag struct

  d[0]  =((System_Time[4]&0X0F)*10+(System_Time[5]&0X0F));//��ǰ��
  d[1]  =((System_Time[6]&0X0F)*10+(System_Time[7]&0X0F));//��ǰ��
  d[2]  =(System_Time[8]&0X0F)*10+(System_Time[9]&0X0F);   //��ǰʱ

  Rdaddr[0]=0;
  Rdaddr[1]=0;
  Rdaddr[2]=0;
  FlashMemorytoBuffer(Rdaddr,FLASHBUFFER1);//read all the flag structs to buffer1
  Delay_N_mS(400);

  pageaddr  =120*6;   //current struct buffer offset address
  Rdaddr[0]=0;
  Rdaddr[1]=(INT8U)(pageaddr>>8);
  Rdaddr[2]=(INT8U)(pageaddr);
  FlashBufferRead(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1); //hour data flag struct
  FlashBufferRead(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1);
  
  if(Sighisdata.month!=d[0])//a new month is coming
  {
    INT16U oldaddr=0;
    Rdaddr[0]=0;
    Rdaddr[1]=0;
    Rdaddr[2]=0;
    //FlashMemorytoBuffer(Rdaddr,FLASHBUFFER1);//read all the flag structs to buffer1
    //Delay_N_mS(200);
    FlashBufferRead(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1); //hour data flag struct
    oldaddr  =Sighisdata.page;
    //����������µ����ݱ�־�ṹ��ǰ�ƣ����ǵ���ɵ�
    for(i=0;i<6;i++)
    {
      MemoryClear(&Sighisdata,sizeof(Sighisdata));
      pageaddr  =120*(i+1);   //buffer offset address
      Rdaddr[0]=0;
      Rdaddr[1]=(INT8U)(pageaddr>>8);
      Rdaddr[2]=(INT8U)(pageaddr);
      FlashBufferRead(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1); //hour data flag struct
      pageaddr  =120*i;   //buffer offset address
      Rdaddr[0]=0;
      Rdaddr[1]=(INT8U)(pageaddr>>8);
      Rdaddr[2]=(INT8U)(pageaddr);
      FlashBufferWrite(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1);
    }
    MemoryClear(&Sighisdata,sizeof(Sighisdata));

    Sighisdata.year =(System_Time[0]&0x0f)*1000+(System_Time[1]&0x0f)*100+(System_Time[2]&0x0f)*10+(System_Time[3]&0x0f);
    Sighisdata.month  =d[0];
    Sighisdata.page  =oldaddr;
    //Sighisdata.flag[d[1]-1][(d[2]-1)>>3]  =Sighisdata.flag[d[1]-1][(d[2]-1)>>3]|(0x01<<((d[2]-1)%8));//set current hour flags
  }
  Sighisdata.flag[d[1]-1][(d[2])>>3]  =Sighisdata.flag[d[1]-1][(d[2])>>3]|(0x01<<((d[2])%8));//set current hour flags
  pageaddr  =120*6;   //current struct buffer offset address
  Rdaddr[0]=0;
  Rdaddr[1]=(INT8U)(pageaddr>>8);
  Rdaddr[2]=(INT8U)(pageaddr);
  FlashBufferWrite(Rdaddr,&Sighisdata,sizeof(Sighisdata),FLASHBUFFER1);  //write current flag struct to the buffer of the flash

  pageaddr =Sighisdata.page;
  pageaddr +=(d[1]-1)*24+d[2];  //page address to store
  Rdaddr[0]=0;
  Rdaddr[1]=0;
  Rdaddr[2]=0;

  Rdaddr[0]=(INT8U)(pageaddr>>5);
  Rdaddr[1]=(INT8U)(pageaddr<<3);

  FlashMemoryWrite(Rdaddr,&CBAG,sizeof(CBAG),FLASHBUFFER2);
  Rdaddr[0]=0;
  Rdaddr[1]=0;
  Rdaddr[2]=0;
  FlashBuffertoMemory(Rdaddr,FLASHBUFFER1); //write back flag structs to the flash memory

  MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
  MemoryClear(local_mma_H,sizeof(local_mma_H));
  MemoryClear(dev_mma_H,sizeof(dev_mma_H));
}

void Reporthisdata_D(void)
{
  INT8U cps[]="DataTime=\0";
  INT8U min[]="-Min=\0";
  INT8U avg[]="-Avg=\0";
  INT8U max[]="-Max=\0";
  INT8U ftstring[18]="\0";
  INT8U timetmp[16];
  INT16U i,j;
  INT8U d[10]="\0";
  INT16U pageaddr=0;
  INT8U Rdaddr[3];
  HISDATAFLAGS_D sig_d;

  Symbol.ccc  =1;
  Response(11);
  Symbol.ccc=0;
  strcat(CBAG.cbag_dynamic,cps);
  MemoryClear(timetmp,16);
  MemoryCopy(System_Time,timetmp,8);
  timetmp[13]=0x30;
  timetmp[12]=0x30;
  timetmp[11]=0x30;
  timetmp[10]=0x30;
  timetmp[9]=0x30;
  timetmp[8]=0x30;
  strcat(CBAG.cbag_dynamic,timetmp);
  for(i=0;i<8;i++)
  {
    if((ADPARA(i).po_id[0]=='0')&&(ADPARA(i).po_id[1]=='0')&&(ADPARA(i).po_id[2]=='0'))
      continue;
    if(local_mma_D[i].num==0)
      local_mma_D[i].avg =0;
    else
      local_mma_D[i].avg  =local_mma_D[i].avg/local_mma_D[i].num;
    for(j=0;j<3;j++)
    {
      d[j]=ADPARA(i).po_id[j];
      //d[j]=AD_parameter.AD_i[i].po_id[j];
    }
    d[3]='\0';
    strcat(CBAG.cbag_dynamic,semicolon);	//�ӷֺ�

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_D[i].min,ftstring);
    strcat(CBAG.cbag_dynamic,min);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		
    strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_D[i].avg,ftstring);
    strcat(CBAG.cbag_dynamic,avg);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		
    strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���

    strcat(CBAG.cbag_dynamic,d);
    float_to_string(local_mma_D[i].max,ftstring);
    strcat(CBAG.cbag_dynamic,max);
    strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����	

    if(!strcmp(d,"B01"))
    {
      INT8U cou[]="-Cou=\0";
      float coutmp;
      strcat(CBAG.cbag_dynamic,comma);	//�Ӷ���
      strcat(CBAG.cbag_dynamic,d);
      //coutmp =b01d_cou;
      /*������Ҫ���ˮ����ʵʱֵ��λΪL/s,�ۼ�ֵ��λΪm3*/
      coutmp =local_mma_D[i].sum;
      float_to_string(coutmp,ftstring);
      strcat(CBAG.cbag_dynamic,cou);
      strcat(CBAG.cbag_dynamic,ftstring);    //������Ⱦ��ʵʱ����		
    }
    b01d_cou =0;
  }
  INT16U tempnum_ch=0;
  for(i=0;i<6;i++)
    tempnum_ch  +=Channel_Num[i];
  if(tempnum_ch!=0)
  {
    ModataUpload(Mod_DayD);
  }
  add_send();//�������

  /*
      ��ʷ���ݴ洢 ��ǩ��ʶҳ��ַΪ5
  */
  d[0]  =((System_Time[4]&0X0F)*10+(System_Time[5]&0X0F));//��ǰ��
  d[1]  =((System_Time[6]&0X0F)*10+(System_Time[7]&0X0F));//��ǰ��
  pageaddr  =5;
  Rdaddr[0]=0;
  Rdaddr[1]=0x28;
  Rdaddr[2]=0;
  FlashMemorytoBuffer(Rdaddr,FLASHBUFFER1);//read all the flag structs to buffer1
  Delay_N_mS(200);

  pageaddr  =80*12;   //current struct buffer offset address
  Rdaddr[0]=0;
  Rdaddr[1]=(INT8U)(pageaddr>>8);
  Rdaddr[2]=(INT8U)(pageaddr);
  FlashBufferRead(Rdaddr,&sig_d,sizeof(sig_d),FLASHBUFFER1);
  FlashBufferRead(Rdaddr,&sig_d,sizeof(sig_d),FLASHBUFFER1); //day data flag struct
  if(sig_d.month!=d[0])
  {
    INT16U oldaddr=0;
    pageaddr =5;
    Rdaddr[0]=(INT8U)(pageaddr>>5);
    Rdaddr[1]=(INT8U)(pageaddr<<3);
    Rdaddr[2]=0;

    FlashBufferRead(Rdaddr,&sig_d,sizeof(sig_d),FLASHBUFFER1); //day data flag struct
    oldaddr  =sig_d.page;
    for(i=0;i<12;i++)
    {
      MemoryClear(&sig_d,sizeof(sig_d));
      pageaddr  =80*(i+1);   //buffer offset address
      Rdaddr[0]=0;
      Rdaddr[1]=((INT8U)(pageaddr>>8))|0x28;
      Rdaddr[2]=(INT8U)(pageaddr);
      FlashBufferRead(Rdaddr,&sig_d,sizeof(sig_d),FLASHBUFFER1); //hour data flag struct
      pageaddr  =80*i;   //buffer offset address
      Rdaddr[0]=0;
      Rdaddr[1]=((INT8U)(pageaddr>>8))|0x28;
      Rdaddr[2]=(INT8U)(pageaddr);
      FlashBufferWrite(Rdaddr,&sig_d,sizeof(sig_d),FLASHBUFFER1);
    }
    MemoryClear(&sig_d,sizeof(sig_d));

    sig_d.year =(System_Time[0]&0x0f)*1000+(System_Time[1]&0x0f)*100+(System_Time[2]&0x0f)*10+(System_Time[3]&0x0f);
    sig_d.month  =d[0];
    sig_d.page  =oldaddr;

  }
  sig_d.flag[d[1]-1]=0x01;  //set flag
  pageaddr  =80*12;   //current struct buffer offset address
  Rdaddr[0]=0;
  Rdaddr[1]=((INT8U)(pageaddr>>8))|0x28;
  Rdaddr[2]=(INT8U)(pageaddr);

  FlashBufferWrite(Rdaddr,&sig_d,sizeof(sig_d),FLASHBUFFER1);  //write current flag struct to the buffer of the flash

  pageaddr =sig_d.page;
  pageaddr +=(d[1]-1);  //page address to store
  Rdaddr[0]=0;
  Rdaddr[1]=0;
  Rdaddr[2]=0;

  Rdaddr[0]=(INT8U)(pageaddr>>5);
  Rdaddr[1]=(INT8U)(pageaddr<<3);

  FlashMemoryWrite(Rdaddr,&CBAG,sizeof(CBAG),FLASHBUFFER2);
  Rdaddr[0]=0;
  Rdaddr[1]=0x28;
  Rdaddr[2]=0;
  FlashBuffertoMemory(Rdaddr,FLASHBUFFER1); //write back flag structs to the flash memory

  MemoryClear(CBAG.cbag_dynamic,sizeof(CBAG.cbag_dynamic));
  MemoryClear(local_mma_D,sizeof(local_mma_D));
  MemoryClear(dev_mma_D,sizeof(dev_mma_D));
}

