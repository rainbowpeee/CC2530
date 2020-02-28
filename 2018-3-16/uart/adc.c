#include "adc.h"

extern unsigned char tx_data[20];

void adc_init(void)
{
   APCFG|=1;
   P0SEL|=0X01;
   P0DIR&=~0X01;
}
unsigned int ged_adc(void)
{
   unsigned long value;
   ADCIF=0;
   ADCCON3=(0X80|0X10|0X00);
   while(!ADCIF);
   value=ADCH;
   value=value<<8;
   value|=ADCL;
   value=(value*330);
   value=value>>15;
   return (unsigned int)value;
   
     
}
void adc_print(void)
{ 
  unsigned int value;
    value=ged_adc();
    
    tx_data[0]='l';
    tx_data[1]='i';
    tx_data[2]='g';
    tx_data[3]='h';
    tx_data[4]='t';
    tx_data[5]='=';
    tx_data[6]=value/100+'0';
    tx_data[7]=value/10%10+'0';
    tx_data[8]=value%10+'0';
   
	 
}