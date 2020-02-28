#include<iocc2530.h>

#define d3 (P1_0)
#define d4 (P1_1)
#define d5 (P1_3)
#define d6 (P1_4)
#define sw1 (P1_2)

unsigned char flag=0;
unsigned char on_off=0;

void main()
{  
  P1SEL &=0X1F;
  P1DIR|=0X1B;
  P1DIR &=~0X04;
  T1CTL =0X0C;
  T1CC0H =0X7A;
  T1CC0L =0X12;
  T1IE=1;
  T1OVFIM=1;
  T1CTL|=0X03;
  EA=1;
  
  
  d3=0;
  d4=0;
  d5=0;
  d6=0;
  
   while(1)
   {
     switch (flag)
     {
     case 0:d4=1;break;
     case 1:d3=1;break;
     case 2:d6=1;break;
     case 3:d5=1;break;
     case 4:d5=0;break;
     case 5:d6=0;break;
     case 6:d3=0;break;
     case 7:d4=0;break;
     case 8:break;
     
     
     }
   }
}
#pragma vector = T1_VECTOR
__interrupt void t1_int(void)
{
   T1STAT &=~0X02;
   if(sw1==0)
   {
     flag++;
      if(flag>8)
      {flag=0;}
   }
   
   
//   if(on_off==0)
//   {
//      flag++;
//      if(flag>8)
//      {flag=0;}
//   }
}