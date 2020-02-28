#include"iocc2530.h"


extern unsigned char rx_data[20];
extern unsigned char tx_data[20];
extern unsigned char len;
extern unsigned char flag;


void uart_init(void)
{
   CLKCONCMD &= 0x80;   //时钟速度设置为32MHz 
   PERCFG=0X00;
   P0SEL=0X3C;
   U0CSR|=0X80;
   U0BAUD=59;
   U0GCR=10;
   U0UCR|=0X80;
   U0CSR|=0X40;//允许接收
   URX0IF=0;
   UTX0IF=0;
   URX0IE=1;   //使能中断
   EA=1;
}
#pragma vector=URX0_VECTOR
__interrupt void uart(void)
{
   URX0IF=0;
   rx_data[len]=U0DBUF;
   len++;
   flag=1;
   
}
void uart_write(unsigned char *p ,unsigned char len)
{ 
  unsigned char i=0;
  for(i=0;i<len;i++)
  {
     U0DBUF=*p;
     while(!UTX0IF);
     UTX0IF=0;
     p++;
  }
}
void delay(unsigned int time)
{  
   unsigned int i,j; 
    for(i=0;i<time;i++)
    {
      for(j=0;j<240;j++)
      {
        ;
        ;
        ;
            
	      
      }
    }
}