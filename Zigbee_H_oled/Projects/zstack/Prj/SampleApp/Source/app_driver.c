#include "OSAL.h"
#include "ZGlobals.h"
#include "OnBoard.h"
#include "AF.h"
#include "hal_uart.h"

#include "app_driver.h"
#include <stdio.h>
#include "app_sample.h"
#include "OLED.h"
//static byte key_rspTaskID = 0;
//static uint16 key_event = 0;
//static uint16 key_time = 0;


//char Set_t[2];
//unsigned char read_key_data = 0;
//unsigned char read_key_state = 0;



//void keyInit( byte task, uint16 event, uint16 time )
//{
//  key_rspTaskID = task;
//  key_event = event;
//  key_time = time;
//  
//  //P0中断优先级最高
//  //  IP0 |= (1<<5);
//  //  IP1 |= (1<<5);
//  
//  // P06
// P0SEL &= ~0x40;       // 0:GPIO
// P0DIR &= ~0x40;       // 0:input    1:output
//  P0INP |= 0x40;        // 0:上下拉    1:三态
//  P0IEN |= 0X40;        // P00 设置为中断方式
//  PICTL |= 0x40;        // 下降沿触发  
//
//  // P07
// P0SEL &= ~0x80;       // 0:GPIO
// P0DIR &= ~0x80;       // 0:input    1:output
//  P0INP |= 0x80;        // 0:上下拉    1:三态
//  P0IEN |= 0X80;        // P00 设置为中断方式
//  PICTL |= 0x80;        // 下降沿触发  
//
//  // P03
// P0SEL &= ~0x08;       // 0:GPIO
// P0DIR &= ~0x08;       // 0:input    1:output
//  P0INP |= 0x08;        // 0:上下拉    1:三态
//  P0IEN |= 0X08;        // P00 设置为中断方式
//  PICTL |= 0x08;        // 下降沿触发  
//  
//
//  
//  IEN1 |= 0X20;         // 允许P0口中断;
//  P0IFG = 0x00;         // 初始化中断标志位
//  EA = 1; 
//}

//uint8 readKeyPress(void)
//{
//  uint8 key = 0;
//  
//  if(P0_7==0)
//  {
//    key |= 0x07;
//  }
//  else
//  {
//    key &= ~0x07;
//  }
//  
//  if(P0_3==0)
//  {
//    key |= 0x03;
//  }
//  else
//  {
//    key &= ~0x03;
//  }
//  if(P0_6==0)
//  {
//    key |= 0x06;
//  }
//  else
//  {
//    key &= ~0x06;
//  }
//  
//  
//  return key;
//}

 /**************************************************************************************************
* @fn      halKeyPort0Isr
*
* @brief   Port0 ISR
*
* @param
*
* @return
**************************************************************************************************/
//HAL_ISR_FUNCTION( halKeyPort0Isr, P0INT_VECTOR )
//{
//  HAL_ENTER_ISR()
////  P0SEL |= 0x0C;        //串口IO处理
//
//  if(key_event){
//      read_key_data = readKeyPress();
//    
//    if(read_key_data == 0x03)
//    {
//      read_key_state++;
//      read_key_state = read_key_state % 3;
//    }
//    if(1){
//      if(read_key_data == 0x06){
//        Set_t[0]++;
//      }
//      if(read_key_data == 0x07){
//        Set_t[0]--;
//      }
//      if(Set_t[0]>99){
//        Set_t[0] = 0;
//      }
//      else if(Set_t[0] < 0){
//        Set_t[0] = 99;
//      }
//      OLED_ShowNum(36,6,Set_t[0],2,16);//显示ASCII字符的码值 
//    }
//    if(1){
//      if(read_key_data == 0x06){
//        Set_t[1]++;
//      }
//      if(read_key_data == 0x07){
//        Set_t[1]--;
//      }
//      if(Set_t[1]>99){
//        Set_t[1] = 0;
//      }
//      else if(Set_t[1]<0){
//        Set_t[1] = 99;
//      }
//      OLED_ShowNum(80,6,Set_t[1],2,16);//显示ASCII字符的码值 
//    }
//    SampleApp_SendPeriodicMessage(Set_t,2);
//  
//  }
//  
//  //清中断标志
//  P0IFG = 0;             
//  P0IF = 0;           
//  HAL_EXIT_ISR();
//}


void uartInit( uint8 buad, halUARTCBack_t serialCallBack )
{
  halUARTCfg_t uartConfig;

  // configure UART
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = buad;
  uartConfig.flowControl          = FALSE;
  uartConfig.flowControlThreshold = 64;
  uartConfig.rx.maxBufSize        = 255;
  uartConfig.tx.maxBufSize        = 255;
  uartConfig.idleTimeout          = 6;
  uartConfig.intEnable            = TRUE;
  uartConfig.callBackFunc         = serialCallBack;

  // start UART
  // Note: Assumes no issue opening UART port.
  (void)HalUARTOpen( 0, &uartConfig );

  return;
}


void printDebugString( char *str )
{
  HalUARTWrite(0,str,osal_strlen(str)); 
}

void printDebugStringNum( char *str, uint16 num )
{
  char buf[15];
  
  HalUARTWrite(0,str,osal_strlen(str)); 
  sprintf(buf,"%d - %x\n",num,num);
  HalUARTWrite(0,buf,osal_strlen(buf)); 
}

void printResString( char *str )
{
  HalUARTWrite(0,str,osal_strlen(str)); 
}

void printInt( unsigned int PrintInt )
{
  unsigned char PrintChar[2];
//  PrintChar[0]  = PrintInt/10000;
//  PrintChar[1]  = PrintInt/1000%10;
//  PrintChar[2]  = PrintInt/100%10;
//  PrintChar[3]  = PrintInt/10%10;
//  PrintChar[4]  = PrintInt%10;

  PrintChar[0] = (PrintInt>>8) & 0x00ff;
  PrintChar[1] = PrintInt & 0x00ff;
  HalUARTWrite(0,PrintChar,2); 
}

void printchar( unsigned char Printchar )
{
  unsigned char PrintChar[1];
  PrintChar[0] = Printchar;
  HalUARTWrite(0,PrintChar,1); 
}




