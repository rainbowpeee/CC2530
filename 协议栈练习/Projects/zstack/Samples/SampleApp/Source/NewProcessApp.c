#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "ZDApp.h"

#include "NewProcessApp.h"

#include "OnBoard.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"//添加uart.h文件
// 此列表应填写特定于应用程序的群集ID.

//#define SEND_DATA_EVENT  0X01//添加定时时事件



const cId_t NewProcessAPP_ClusterList[NEWPROCESSAPP_MAX_CLUSTERS] =
{
  NEWPROCESSAPP_PERIODIC_CLUSTERID
};
//用来描述一个ZigBee设备节点，称为简单设备描述符

const SimpleDescriptionFormat_t NewProcessAPP_SimpleDesc =
{
  NEWPROCESSAPP_ENDPOINT,              //  int Endpoint;
  NEWPROCESSAPP_PROFID,                //  uint16 AppProfId[2];
  NEWPROCESSAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  NEWPROCESSAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  NEWPROCESSAPP_FLAGS,                 //  int   AppFlags:4;
  NEWPROCESSAPP_MAX_CLUSTERS,          //  uint8  AppNumInClusters;
  (cId_t *)NewProcessAPP_ClusterList,  //  uint8 *pAppInClusterList;
  0,           
  (cId_t *)NULL    
};

endPointDesc_t NewProcessAPP_epDesc;  //节点描述符
uint8 NewProcessAPP_TaskID;           //任务优先级
uint8 NewProcessAPP_TransID;          //数据发送序列号
unsigned char uartbuf[128];           //串口发送缓冲区
//******************************************************************************
void NewProcessAPP_MessageMSGCB( afIncomingMSGPacket_t *pckt );//声明消息处理函数
void NewProcessAPP_SendTheMessage( void );                     //数据发送函数
void rxCB(uint8 port,uint8 event);                             //串口发送函数
void NewProcessAPP_key();
//******************************************************************************


void NewProcessApp_Init( uint8 task_id )//任务初始化函数
{
  halUARTCfg_t uartConfig;                                   //*
  NewProcessAPP_TaskID            = task_id;                  //初始化任务优先级
  NewProcessAPP_TransID           = 0;                        //将发送数据包序号初始化为0
  
  NewProcessAPP_epDesc.endPoint   = NEWPROCESSAPP_ENDPOINT;   //对节点描述符进行初始化
  NewProcessAPP_epDesc.task_id    = &NewProcessAPP_TaskID;
  NewProcessAPP_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&NewProcessAPP_SimpleDesc;
  NewProcessAPP_epDesc.latencyReq = noLatencyReqs;

  //使用afRegister函数将节点描述符进行注册才可以使用OSAL服务
  afRegister( &NewProcessAPP_epDesc );
  //*********************uart配置表********************//  
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.callBackFunc         = rxCB;//无需回调函数
  HalUARTOpen(0,&uartConfig);
}


uint16 NewProcessApp_ProcessEvent(uint8 task_id,uint16 events)
{

  afIncomingMSGPacket_t *MSGpkt;//定义一个指向接收消息结构体的指针MSGpkt
   
   if( events & SYS_EVENT_MSG)
   {   
/*使用osal_msg_receive函数从消息队列上接收消息，该消息中包含接收到无线数据包
准确说是包含了指向接收到的无线数据包的指针）*******************************/
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(NewProcessAPP_TaskID);
      while( MSGpkt)
      {
        switch(MSGpkt->hdr.event)
        {
           case KEY_CHANGE:
	
           case AF_INCOMING_MSG_CMD: //对接收到的消息进行判断,如果是无线信号则调用下列函数处理
	NewProcessAPP_MessageMSGCB(MSGpkt);//声明消息处理函数
	break;
        default:    //当所有case x:都不执行时执行default：
          break;
        }
        osal_msg_deallocate((uint8 *)MSGpkt);//接收消息处理完成，释放消息所占用存储空间
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( NewProcessAPP_TaskID );
        //处理完成后，再使用osal_msg_receive函数从消息队列上接收消息
      }
      return (events ^ SYS_EVENT_MSG);
      
      
   }
//   if(events & SEND_DATA_EVENT)  //*
//      {
//      NewProcessAPP_SendTheMessage();//**声明消息处理函数
//      osal_start_timerEx(NewProcessAPP_TaskID,SEND_DATA_EVENT,100);
//      return (events ^ SEND_DATA_EVENT);//*
//      }
   return 0;
}

void NewProcessAPP_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
   unsigned char buffer[5] = "   ";
   switch( pkt->clusterId)
   { 
     case NEWPROCESSAPP_PERIODIC_CLUSTERID:
       osal_memcpy(buffer,pkt->cmd.Data,5);//将收到的数据拷贝到buffer中
       HalUARTWrite(0,buffer,5);
       HalLedBlink( HAL_LED_1, 1, 50, 50 );
       break;
  default:
    break;
   }
}
void rxCB(uint8 port,uint8 event)//*串口回调函数
{
  
  if ((event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL | HAL_UART_RX_TIMEOUT)))
   {
   //HalUARTWrite(0,"接收一次",8);
   HalLedBlink( HAL_LED_2, 1, 50, 50 );
   NewProcessAPP_SendTheMessage();
 
  }
}
void NewProcessAPP_SendTheMessage(void)//无线发送函数
{  
  
   unsigned char theMessageData[5] = "     ";
   HalUARTRead(0,theMessageData,5);
   HalLedBlink(HAL_LED_2,3,50,50);
   
   afAddrType_t my_DstAddr;
   my_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;//初始化为广播
   my_DstAddr.endPoint = NEWPROCESSAPP_ENDPOINT;//初始化端口号
   my_DstAddr.addr.shortAddr = 0xFFFF;//发送数据
   
   AF_DataRequest(&my_DstAddr,//包含了目的节点的网络地址及发送数据格式，如广播，单播，多播
	     &NewProcessAPP_epDesc,//指定节点的发送端口号
	     NEWPROCESSAPP_PERIODIC_CLUSTERID,//命令号
	     5,//发送数据大小
	     theMessageData,//数据缓存
	     &NewProcessAPP_TransID,//指向发送指针，每发送一次加一
	     AF_DISCV_ROUTE, 
	     AF_DEFAULT_RADIUS);
   
                  HalLedBlink(HAL_LED_2,3,50,50);
                 //HalLedBlink(HAL_LED_2,0,50,500);
	   
    
}






