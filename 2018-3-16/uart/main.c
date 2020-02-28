#include "adc.h"
#include "uart.h"




unsigned char rx_data[20]="aaaaaaaaaaaaaaaaaaa";
unsigned char tx_data[20];
unsigned char len=0;
unsigned char flag=0;



void uart_init(void);
void uart_write(unsigned char *p ,unsigned char len);
void delay(unsigned int time);



void main(void)
{
   uart_init();
   adc_init();
   
   
   P1SEL&=~0X01;
   P1DIR|=0X01;
   P1_0=1;
   while(1)
   {
      if(flag==1)
      {
        delay(1000);
        P1_0=~P1_0;
        uart_write( rx_data ,len);
        
        
        len=0;
        flag=0;
        
      }
      delay(1000);
      //P1_0=~P1_0;
      delay(1000);
      adc_print();
      uart_write(tx_data,9);
   }
}
