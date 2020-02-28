#include "OSAL.h"
#include "ZGlobals.h"
#include "AF.h"
#include "ZDApp.h"

#include "NewProcessApp.h"

#include "OnBoard.h"
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "get_adc.h"
#include "iocc2530.h"
// This list should be filled with Application specific Cluster IDs.

//#define SEND_DATA_EVENT  0X01//添加定时时事件
#define relay (P2_0) //继电器定义
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
devStates_t NewProcessAPP_NwkState;   //***//保存节点状态的变量


//******************************************************************************
void NewProcessAPP_MessageMSGCB( afIncomingMSGPacket_t *pckt );//声明消息处理函数
//void NewProcessAPP_SendTheMessage( void );                     //数据发送函数
void relay_scan(uint8 *buffer);
//******************************************************************************


void NewProcessApp_Init( uint8 task_id )//任务初始化函数
{
  halUARTCfg_t uartConfig;   
  //hal_adc_Init();                                 //adc采集初始化
  NewProcessAPP_TaskID           = task_id;       //初始化任务优先级
  NewProcessAPP_NwkState         = DEV_INIT;      //**将设备状态初始化为DEV_INIT，表示节点没连接到网络
  NewProcessAPP_TransID          = 0;             //将发送数据包序号初始化为0
  
  NewProcessAPP_epDesc.endPoint  = NEWPROCESSAPP_ENDPOINT;//对节点描述符进行初始化
  NewProcessAPP_epDesc.task_id   = &NewProcessAPP_TaskID;
  NewProcessAPP_epDesc.simpleDesc
            = (SimpleDescriptionFormat_t *)&NewProcessAPP_SimpleDesc;
  NewProcessAPP_epDesc.latencyReq = noLatencyReqs;

  //使用afRegister函数将节点描述符进行注册才可以使用OSAL服务
  afRegister( &NewProcessAPP_epDesc );
   //*********************uart配置表********************//  
  uartConfig.configured           = TRUE;
  uartConfig.baudRate             = HAL_UART_BR_115200;
  uartConfig.flowControl          = FALSE;
  uartConfig.callBackFunc         = NULL;//无需回调函数
  HalUARTOpen(0,&uartConfig);
  P2SEL &=~0X01;
  P2DIR |=0X01;
  relay=0;
}

uint16 NewProcessApp_ProcessEvent(uint8 task_id,uint16 events)//消息处理函数
{

  afIncomingMSGPacket_t *MSGpkt;//定义一个指向接收消息结构体的指针MSGpkt
   
   if( events & SYS_EVENT_MSG)
   {   //使用osal_msg_receive函数从消息队列上接收消息，该消息中包含接收到无线数据包
       //（准确说是包含了指向接收到的无线数据包的指针）
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(NewProcessAPP_TaskID);
      while( MSGpkt)
      {
        switch(MSGpkt->hdr.event)
        {
           case ZDO_STATE_CHANGE: //**对接收到的消息进行判断,如果是则调用下列函数处理
	NewProcessAPP_NwkState = (devStates_t)(MSGpkt->hdr.status);//**读取节点设备类型
	if(NewProcessAPP_NwkState == DEV_END_DEVICE)
	{ //**对节点设备类型进行判断，如果是终端节点DEV_END_DEVICE执行下一步 
	//   osal_set_event(NewProcessAPP_TaskID,SEND_DATA_EVENT);//*
	   
	}
	
	break;
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
//   {
//      NewProcessAPP_SendTheMessage();//**声明消息处理函数
//      osal_start_timerEx(NewProcessAPP_TaskID,SEND_DATA_EVENT,2000);
//      return (events ^ SEND_DATA_EVENT);//*
//   }
   return 0;
}
//void NewProcessAPP_SendTheMessage(void)//无线发送函数
//{  
//   uint8 state=0;       //状态变量
//   uint16 chk=0XFF;
//   unsigned char theMessageData[5] = "     ";
//   state=get_swsensor();
//   
//   if(state==1)
//   {
//     theMessageData[3]=0XFF;
//   }
//   else if(state==0)
//   {
//     theMessageData[3]=0X00;
//   }
//   
//      theMessageData[0]=0XFE;
//      theMessageData[1]=0X0E;
//      theMessageData[2]=0X02;
//      chk=0X0E+0X02+theMessageData[3];
//      theMessageData[4]=(chk<<8)>>8;
//      
//   afAddrType_t my_DstAddr;
//   my_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;//初始化为单播
//   my_DstAddr.endPoint = NEWPROCESSAPP_ENDPOINT;//初始化端口号
//   my_DstAddr.addr.shortAddr = 0x0000;//协调器地址，向协调器发送数据
//   
//   AF_DataRequest(&my_DstAddr,//包含了目的节点的网络地址及发送数据格式，如广播，单播，多播
//	     &NewProcessAPP_epDesc,//指定节点的发送端口号
//	     NEWPROCESSAPP_PERIODIC_CLUSTERID,//命令号
//	     5,//发送数据大小
//	     theMessageData,//数据缓存
//	     &NewProcessAPP_TransID,//指向发送指针，每发送一次加一
//	     AF_DISCV_ROUTE, 
//	     AF_DEFAULT_RADIUS);
//                  HalLedBlink(HAL_LED_1,3,50,50);
//                 //HalLedBlink(HAL_LED_2,0,50,500);
//	     
//}

void NewProcessAPP_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
   unsigned char buffer[6] = "   ";
   //uint16 chk;
   switch( pkt->clusterId)
   { 
     case NEWPROCESSAPP_PERIODIC_CLUSTERID:
       osal_memcpy(buffer,pkt->cmd.Data,5);//将收到的数据拷贝到buffer中
       relay_scan(buffer);
       break;
         
  default:
    break;
   }
}
void relay_scan(uint8 *buffer)
{
    HalUARTWrite(0,buffer,5);
#if defined (RELAY1 )   
    if((buffer[0]==0XFE)&&(buffer[1]==0X0E)&&(buffer[2]==0X03)&&(buffer[3]==0X11)&&(buffer[4]==0x22))
      {
        HalLedBlink( HAL_LED_1, 1, 50, 50 );
        //HalUARTWrite(0,"yes",3);
        relay=1;
      }
    if((buffer[0]==0XFE)&&(buffer[1]==0X0E)&&(buffer[2]==0X03)&&(buffer[3]==0X22)&&(buffer[4]==0x33))
      {
        HalLedBlink( HAL_LED_2, 1, 50, 50 );
        //HalUARTWrite(0,"no",2);
        relay=0;
      }
#endif
      if((buffer[0]==0XFE)&&(buffer[1]==0X0E)&&(buffer[2]==0X04)&&(buffer[3]==0X11)&&(buffer[4]==0x23))
      {
        HalLedBlink( HAL_LED_1, 1, 50, 50 );
        //HalUARTWrite(0,"yes",3);
        relay=1;
      }
    if((buffer[0]==0XFE)&&(buffer[1]==0X0E)&&(buffer[2]==0X04)&&(buffer[3]==0X22)&&(buffer[4]==0x34))
      {
        HalLedBlink( HAL_LED_2, 1, 50, 50 );
        //HalUARTWrite(0,"no",2);
        relay=0;
      }
}