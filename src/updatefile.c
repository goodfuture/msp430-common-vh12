#include <msp430xG46x.h>
#include "update.h"

#define CONTINUOUS_MREAD      0X03
#define BUFFER1_READ          0XD1
#define BUFFER2_READ          0XD3
#define BUFFER1_WRITE         0x84
#define BUFFER2_WRITE         0x87
#define MEMORY_WRITE_THBUFF1  0X82
#define MEMORY_WRITE_THBUFF2  0X85
#define BUFFER1_TO_MEMORY     0x83
#define BUFFER2_TO_MEMORY     0x86
#define MEMORY_TO_BUFFER1     0x53
#define MEMORY_TO_BUFFER2     0x55

#define FLASH_ENABLE  P4OUT &=~BIT0/*P5OUT &=~BIT2*/
#define FLASH_DISABLE P4OUT |=BIT0/*P5OUT |=BIT2*/

#define FLASHBUFFER1       0X00
#define FLASHBUFFER2       0X01

void FlashMemoryRead(INT8U *addr,void *array,INT16U len)
{
  INT8U i;
  INT8U *Dest;

  Dest  =(INT8U *)array;
  FLASH_ENABLE;
  /*send command*/
  while(!(IFG2&UTXIFG1));
  U1TXBUF =CONTINUOUS_MREAD;
  /*send address 3 bytes*/
  for(i=0;i<3;i++)
  {
    while(!(IFG2&UTXIFG1));
    U1TXBUF =*addr++;
  }
  /*read a dummy byte*/
  Delay_N_mS(20);
  while(!(IFG2&UTXIFG1));
    U1TXBUF =0;
    while(!(IFG2&URXIFG1));
    *Dest=U1RXBUF;
  /*read data from flash*/
  while(len--)
  {
    while(!(IFG2&UTXIFG1));
    U1TXBUF =0;
    while(!(IFG2&URXIFG1));
    *Dest++=U1RXBUF;
  }
  Delay_N_mS(20);
  FLASH_DISABLE;
}

void FlashMemoryWrite(INT8U *addr,void *array,INT16U len,INT8U buff)
{
  INT8U i;
  INT8U *Dest;

  Dest  =(INT8U *)array;
  FLASH_ENABLE;
  /*send command*/
  //while((IFG2&UTXIFG1)==0);
  if(buff==FLASHBUFFER1)
    U1TXBUF =MEMORY_WRITE_THBUFF1;
  else
    U1TXBUF =MEMORY_WRITE_THBUFF2;
  /*send address 3 bytes*/
  for(i=0;i<3;i++)
  {
    while((IFG2&UTXIFG1)==0);
    U1TXBUF =*addr++;
  }
  /*write data to flash memory*/
  Delay_N_mS(20);
  while(len--)
  {
    while((IFG2&UTXIFG1)==0);
    U1TXBUF =*Dest++;
  }
  while(!(TXEPT&U1TCTL));
  //Delay_N_mS(10);
  FLASH_DISABLE;
}

#ifndef NULL
#define NULL 0
#endif

#define nop() asm("NOP")
typedef  int (INIT_FUNC)(void);

/* Table of CRC values for high-order byte */
INT8U auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;


/* Table of CRC values for low-order byte */
INT8U auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
0x43, 0x83, 0x41, 0x81, 0x80, 0x40
} ;
/*
int SCLK_init(void)
{
   volatile INT16U i;
  _BIS_SR(OSCOFF);   //turn off DCO and FLL+
   SCFQCTL=4;
  SCFQCTL   &=~SCFQ_M; //enable modulation
  nop();
  SCFI0   |=FN_8+FLLD_1;  //DCO range fDCOCLK =   1.4-12MHz
  //nop();
  FLL_CTL0    |= XCAP18PF;  //XIN Cap = XOUT Cap = 18pf
  nop();
 // FLL_CTL1    |=FLL_DIV_8;   // ALCK/8
  nop();
  FLL_CTL1    &=~XT2OFF; //turn on xt2  fxt2=8M
  nop();
  //FLL_CTL1

  do  //wait for stability
  {
    IFG1  &=~OFIFG;   //clear oscillator fault flag
    for(i=0;i<5000;i++);
  }while((IFG1&OFIFG));
  nop();
  FLL_CTL1    |=SELM1;
  FLL_CTL1    |=SELM_DCO+SELS;   //select XT2 as the MCLK and SMCLK
  nop();
  return(0);
}*/

int SCLK_init(void)
{
  volatile INT16U i;
  _BIS_SR(SCG0+OSCOFF+SCG1);   //turn off DCO and FLL+
  //SCFQCTL   &=~SCFQ_M; //enable modulation
  nop();
  //SCFI0   |=FN_4+FLLD_1;  //DCO range fDCOCLK =   1.4-12MHz
  //nop();
  //FLL_CTL0    |= XCAP18PF;  //XIN Cap = XOUT Cap = 18pf
  nop();
 // FLL_CTL1    |=FLL_DIV_8;   // ALCK/8
  nop();
  FLL_CTL1    &=~XT2OFF; //turn on xt2  fxt2=8M

  do  //wait for stability
  {
    IFG1  &=~OFIFG;   //clear oscillator fault flag
    for(i=0;i<5000;i++);
  }while((IFG1&OFIFG));
  nop();
  FLL_CTL1    |=SELM1;
  FLL_CTL1    |=SELM_XT2+SELS;   //select XT2 as the MCLK and SMCLK
  nop();
  return(0);
}

int IO_init(void)
{
  //unused io pins  should be configured as io function and output mode
#ifdef MAINBOARD
  //P1
  P1DIR |=BIT0+BIT1+BIT2+BIT3+BIT4+BIT5;  //p1.0,1.2,1.3,1.5,1.6 are output
//  P1IE  |=BIT6;            //enable p1.1,p1.4 interrupt
  P1IES |=0x40;                 //interrupt when high-to-low
  P1IFG &=0x00;                 //clear interrupt flags
  //P2
  P2DIR |=BIT0+BIT1+BIT4+BIT5+BIT6;
//  P2IE  |=BIT2;
  P2IES |=0x04;
  P2IFG &=0x00;
  //P3  p3.3 unused
  P3SEL |=BIT1+BIT2;                 //p3 are used as io pins p3.1,p3.2 I2C
  P3DIR |=BIT0+BIT5+BIT6;
  //P4  P4.2 unused
  P4SEL |=BIT3+BIT4+BIT5+BIT6+BIT7; //P4.3,4,5 are used as SPI,uart
  P4DIR |=BIT2;      //P4.2,3,5,6 are output
  //P5  P5.0,1,2,3,4 unused
  P5DIR |=BIT2+BIT3;
  //P9
  P9DIR |=BIT4+BIT5;
  //P10
  P10DIR |=0xff;
#else
  //p2
  P2DIR |=BIT2+BIT3+BIT4+BIT5+BIT6;
  P2IE  |=BIT0+BIT1;
  P2IES |=0x03;
  P2IFG &=0x00;
  //P3
  P3DIR |=BIT4+BIT5+BIT6+BIT7;
  //p4
  P4SEL |=BIT3+BIT4+BIT5+BIT6+BIT7; //P4.3,4,5 are used as SPI,uart
  P4DIR |=BIT0+BIT1+BIT2;
  //P5
  P5DIR |=BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;
  //p6
  P6SEL|=0xff;  //used as analog input
  //P7
  P7DIR |=BIT4+BIT5;
  //P8
  P8DIR |=BIT4+BIT5+BIT6+BIT7;
  //p9
  P9DIR |=0xff;
  P10DIR |=BIT0+BIT1+BIT6;
#endif  //end MAINBOARD

  return(0);
}

int UART_init(void)
{
  //SPI 8bit master
  U1CTL |=SWRST;
  nop();
  U1CTL |=CHAR+SYNC+MM; //8-bit master spi mode
  U1TCTL  |=SSEL1+STC+CKPH;    //baud rate generated by SMCLK(8M)
  U1BR0 =0X02;//0x04;        //BAUD RATE =SMCLK/8   1Mhz?
  U1BR1 =0x00;
  U1MCTL  =0x00;
  ME2 |=USPIE1;
  U1CTL &=~SWRST;
  IFG2  &=~(UTXIFG1+URXIFG1); //clear interrupt flags

  //UART 8N1
  UCA0CTL1  |= UCSWRST;
  nop();
  UCA0CTL1  |=UCSSEL1;  //baud rate generated by SMCLK(8M)
  //UCA0CTL0  |=UCMSB;    //MSB first, uart mode, 8N1

  /*baud rate=9600 (BR0,BR1)=(0x41,0x03)  UCBRSX=0X02     **
  **baud rate=57600  (BR0,BR1)=(0X8A,0X00)  UCBRSX=0X07   **
  **baud rate=1M    (BR0,BR1)=(0X08,0X00)   UCBRSX=0X00   **
  */
  UCA0BR0 =0x71;  //N=SMCLK/BAUD  UCA0BR0+UCA0BR1*256=INT(N)
  UCA0BR1 =0x02;
  UCA0MCTL  =0x00;      //modulation  UCBRSX=round((N-int(N)*8))
//
  IFG2  &=~UCA0RXIFG;
  UCA0CTL1  &= ~UCSWRST;
  IE2 |=UCA0RXIE;     //enable receive interrupt
  //I2C   7-bit address master mode
  UCB0CTL1  |=UCSWRST;
  UCB0CTL0  |=UCMODE_3+UCSYNC+UCMST; //I2C master sync mode
  UCB0CTL1  |=UCSSEL_2;  //clock source SMCLK
  UCB0I2CSA =0x50;    //slave address is 0x50 AT24C256
  UCB0BR0   =0x14;//0xa0;//100K//0x14;        //fclk=SMCLK/20=400K
  UCB0BR1   =0x00;
  UCB0CTL1  &=~UCSWRST;
  IFG2  &=~UCB0RXIFG;
  //UCB0I2CSA =0X50;
  return(0);

}

int DMA_init(void)
{
  return(0);
}

int ADC_init(void)
{
  ADC12CTL0|=SHT10+MSC+REF2_5V+REFON+ADC12ON;
  ADC12CTL1|=SHP+ADC12DIV_3+CONSEQ_3;
  ADC12MCTL0|=SREF_1+INCH_0;
  ADC12MCTL1|=SREF_1+INCH_0;
  ADC12MCTL2|=SREF_1+INCH_1;
  ADC12MCTL3|=SREF_1+INCH_1;
  ADC12MCTL4|=SREF_1+INCH_2;
  ADC12MCTL5|=SREF_1+INCH_2;
  ADC12MCTL6|=SREF_1+INCH_3;
  ADC12MCTL7|=SREF_1+INCH_3;
  ADC12MCTL8|=SREF_1+INCH_4;
  ADC12MCTL9|=SREF_1+INCH_4;
  ADC12MCTL10|=SREF_1+INCH_5;
  ADC12MCTL11|=SREF_1+INCH_5;
  ADC12MCTL12|=SREF_1+INCH_6;
  ADC12MCTL13|=SREF_1+INCH_6;
  ADC12MCTL14|=SREF_1+INCH_7;
  ADC12MCTL15|=SREF_1+INCH_7+EOS;
  ADC12IE =0X8000;
  ADC12CTL0|=ENC+ADC12SC;
  return(0);
}

int TIME_init(void)
{
  //Watch Dog  used as interval timer
  WDTCTL  =WDT_ARST_1000;  //interval=32768/32768=1
  IE1 |=WDTIE;
  IFG1  &=~WDTIFG;
  return(0);
}

int DAC_init(void)
{
  return(0);
}

INIT_FUNC  *pinit_func_sequence[]={
              SCLK_init,
              IO_init,
              //TIME_init,
              UART_init,
              DMA_init,
              ADC_init,
              DAC_init,
              NULL
};
/*
int CPU_init(void)
{
  INIT_FUNC **pinit_func;
  _BIS_SR(GIE);   //GIE =0 disable interrupt
  nop();
  WDTCTL  =WDTPW+WDTHOLD; //turn off watchdog
  nop();
  for(pinit_func=pinit_func_sequence;*pinit_func==NULL;pinit_func++)
  {
    (*pinit_func)();
  }
  return(0);
}*/
int CPU_init(void)
{
  //_BIS_SR(GIE);   //GIE =0 disable interrupt
  nop();
  WDTCTL  =WDTPW+WDTHOLD; //turn off watchdog
  nop();
  SCLK_init();
  IO_init();
  UART_init();
  ADC_init();
  //TIME_init();
  return(0);
}

void MemoryClear(INT8U *pdes,INT16U size)
{
    while (size > 0) {
        *pdes++ = (INT8U)0;
        size--;
    }
}
void MemoryCopy(INT8U *src,INT8U *des,INT16U len)
{
  while(len--)
  {
    *des++  =*src++;
  }
}

void CRC_Calculation(INT8U *src,INT16U len,INT8U *des)
{
  //unsigned char i;
	INT16U j;
	INT16U uIndex ;	/* will index into CRC lookup table*/

	//cc.CRC_HL=0xffff;
  *des  =0xff;
  *(des+1)  =0xff;
			
	for(j=0;j<len;j++)
	{
		uIndex = *des ^ *src++ ; /* calculate the CRC */
		(*des) = *(des+1) ^ auchCRCHi[uIndex] ;
		*(des+1) = auchCRCLo[uIndex] ;
	}
	/*		
	for(j=0;j<2;j++)
	{
		i=(cc.CRC[j]>>4)&0xf;
		if(i<=9)
			crc[2*j]=i|0x30;
		else
			crc[2*j]=i+0x37;
		i=cc.CRC[j]&0xf;
		if(i<=9)
			crc[2*j+1]=i|0x30;
		else
			crc[2*j+1]=i+0x37;
	}*/
}
INT8U StrToASCII(INT8U *src,INT8U *des,INT16U len)
{
  INT16U j;
  INT8U i;
  for(j=0;j<len;j++)
	{
		i=((*src)>>4)&0xf;
		if(i<=9)
			*des=i|0x30;
		else
			*des=i+0x37;
		i=(*src)&0xf;
		if(i<=9)
			*(des+1)=i|0x30;
		else
			*(des+1)=i+0x37;
    src++;
    des+=2;
	}
  return(0);
}

void Delay_N_mS( unsigned int n_milisecond)  /* n mS delay */
{
   unsigned char i;
   while(n_milisecond--)
  {
           i=17;
           while(i--);
  }
}
