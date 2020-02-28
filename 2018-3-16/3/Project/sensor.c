#include "hal_defs.h"
#include "hal_cc8051.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "hal_board.h"
#include "hal_led.h"
#include "hal_rf.h"
#include "basic_rf.h"
#include "hal_uart.h" 
#include "UART_PRINT.h" 
#include "TIMER.h"
#include "get_adc.h"
#include "sh10.h"
#include <string.h>

#define MAX_SEND_BUF_LEN  128  
#define MAX_RECV_BUF_LEN  128
static uint8 pTxData[MAX_SEND_BUF_LEN]; //定义无线发送缓冲区的大小
static uint8 pRxData[MAX_RECV_BUF_LEN]; //定义无线接收缓冲区的大小
uint16 pTxlen = 0;
uint16 pRxlen = 0;
#define MAX_UART_SEND_BUF_LEN  128
#define MAX_UART_RECV_BUF_LEN  128
uint8 uTxData[MAX_UART_SEND_BUF_LEN]; //定义串口发送缓冲区的大小
uint8 uRxData[MAX_UART_RECV_BUF_LEN]; //定义串口接收缓冲区的大小
uint16 uTxlen = 0;
uint16 uRxlen = 0;


/*****点对点通讯地址设置******/
#define RF_CHANNEL                13           // 频道 11~26
#define PAN_ID                    0x0013      //网络id 
#define MY_ADDR                   0x4444      // 本机模块地址
#define SEND_ADDR                 0x2222     //发送地址
/**************************************************/
static basicRfCfg_t basicRfConfig;
uint8   APP_SEND_DATA_FLAG;
uint16 count=0;
void sen_sensor(void);
/******************************************/

/**************************************************/
// 无线RF初始化
void ConfigRf_Init(void)
{
    basicRfConfig.panId       =   PAN_ID;        //zigbee的ID号设置
    basicRfConfig.channel     =   RF_CHANNEL;    //zigbee的频道设置
    basicRfConfig.myAddr      =  MY_ADDR;   //设置本机地址
    basicRfConfig.ackRequest  =   TRUE;          //应答信号
    while(basicRfInit(&basicRfConfig) == FAILED); //检测zigbee的参数是否配置成功
    basicRfReceiveOn();                // 打开RF
}
/********************MAIN START************************/
void main(void)
{
    uint8  status;
    halBoardInit();  //模块相关资源的初始化
    ConfigRf_Init(); //无线收发参数的配置初始化 
    Timer4_Init();    //定时器初始化
    Timer4_On();      //打开定时器
    P1SEL &=~0X04;
    P1DIR &=~0X04;
   
  while(1)
  { 
    
    
    if( GetSendDataFlag()==1)
    {
      
        
        sen_sensor();
        status = basicRfSendPacket(SEND_ADDR,pTxData,11);
        halLedToggle(1);

        Timer4_On();

    }
      
       
       
    }
   
  
   
}
void sen_sensor(void)
{  
  uint16 adc=0;
  uint16 chk=0;
  uint8 i=0;
  adc=get_adc();
  count++;
  
  pTxData[0]=0xff;

  pTxData[1]=0xfd;
  
  pTxData[2]=0x00;
  pTxData[3]=0x04;
  
  pTxData[4]=0x36;
  pTxData[5]=0x39;
  pTxData[6]=0x00;
  pTxData[7]=0x00;
  
  pTxData[8]=count&0xff;
  pTxData[9]=count>>8;
  
  for(i=0;i++;i<10)
  {
    chk=chk+pTxData[i];
  }
  if(count==0xffff)
  {count=0;}
  pTxData[10]=count&0xff;
  
   
}