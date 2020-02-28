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
static uint8 pRxData[MAX_RECV_BUF_LEN]; //定义无线发送缓冲区的大小
#define MAX_UART_SEND_BUF_LEN  128
#define MAX_UART_RECV_BUF_LEN  128
uint8 uTxData[MAX_UART_SEND_BUF_LEN]; //定义串口发送缓冲区的大小
uint8 uRxData[MAX_UART_RECV_BUF_LEN]; //定义串口接收缓冲区的大小
uint16 uTxlen = 0;
uint16 uRxlen = 0;



/*****点对点通讯地址设置******/
#define RF_CHANNEL                20           // 频道 11~26
#define PAN_ID                    0x3040      //网络id 
#define MY_ADDR                   0X0003     // 本机模块地址
#define SEND_ADDR                 0x1234     //发送地址
//#define yyyy_ADDR                 0X0003     //监听地址
/**************************************************/
static basicRfCfg_t basicRfConfig;
uint8   APP_SEND_DATA_FLAG;
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

/********************MAIN************************/
void main(void)    
{   uint8 flag=0;
    uint8 status,len; 
    halBoardInit();  //模块相关资源的初始化
    ConfigRf_Init(); //无线收发参数的配置初始化 
    halLedSet(1);
    halLedSet(2);
    Timer4_Init();
    Timer4_On();
    while(1)
    {  
      status = basicRfPacketIsReady();
      len = halUartRxLen();
      if(status==1)
      {
         len = basicRfReceive(pRxData,MAX_UART_SEND_BUF_LEN,NULL);
         halUartWrite(pRxData, len);
         status=0;
         len=0;
      }
    
      if(len!=0)
      {
        
         len = halUartRead(uRxData, len);
         //halUartWrite(uRxData, len);
         //status = basicRfSendPacket(SEND_ADDR,uRxData,len);
         len=0;
      }
    }
}
