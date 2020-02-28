
/*描述：显示加速度计和陀螺仪的6位原始数据
****************************************/
#include "ioCC2530.h"
#include "iic.h"
#include "OnBoard.h"
#include "hal_types.h"
 

/*
  引脚定义
*/
#define SDA P0_4
#define SCL P0_5




 

 
 
 
/****************us延时函数 32M晶振 单周期MCU********************/
void delay_us(unsigned int u)
{
   u=4*u;
   MicroWait(u);
}
 
 
/*****************************MPU6050*******************************/
void WriteSDA_0(void) 
{
    P0SEL&=~0x10;
    P0DIR|=0x10;
    SDA=0;
}
 
 
void WriteSDA_1(void) 
{
    P0SEL&=~0x10;
    P0DIR|=0x10;
    SDA=1;
}
    
void WriteSCL_0(void)  
{
    P0SEL&=~0x20;
    P0DIR|=0x20;
    SCL=0;
}
 
 
void WriteSCL_1(void)  
{
    P0SEL&=~0x20;
    P0DIR|=0x20;
    SCL=1;
}

 
void ReadSDA(void)//sda输入,=0
{
    P0SEL&=~0x10;P0DIR&=~0x10;
}

void SDA_OUT(void)//sda输出,=1
{
    P0SEL&=~0x10;
    P0DIR|=0x10;
}
 
//void Init_IO(void)
//{
//  P0DIR|=0xc0;
//  P0SEL&=0x3f;
//}
// 
/**************************************
起始信号
**************************************/
void OLED_Start(void)  
{
    WriteSDA_1();
    WriteSCL_1();
    delay_us(5);
    WriteSDA_0();
    delay_us(5);
    WriteSCL_0();
    delay_us(5);
}
 
 
/**************************************
停止信号
**************************************/
void OLED_Stop()  
{    
   WriteSDA_0();
   WriteSCL_1();
   delay_us(5);
   WriteSDA_1();
}
 
 
//I2C发送应答信号
//入口参数:ack (0:ACK 1:NAK)
void OLED_SendACK(unsigned char ack)
{
    
    SDA=ack;                     //写应答信号
    WriteSCL_1();                   //拉高时钟线
    delay_us(5);//Delay5us();    //延时
    WriteSCL_0();                   //拉低时钟线
    delay_us(5);//Delay5us();    //延时
}
 
 
/**************************************
发送应答信号
**************************************/
unsigned char OLED_RecvACK()
{
    ReadSDA();
    WriteSCL_1();                       //拉高时钟线
    delay_us(5);//Delay5us();    //延时
    CY=SDA;                      //读应答信号
    WriteSCL_0();                       //拉低时钟线
    delay_us(5);//Delay5us();    //延时
    P0DIR|=0x00;
    return CY;
}
 
 
/**************************************
向IIC总线发送一个字节数据
**************************************/
void OLED_SendByte(unsigned char data)
{
    unsigned char i;
    SDA_OUT();
    
    for(i=0;i<8;i++)         //8位计数器
    {       
        if(data&0x80)
        {WriteSDA_1();}
        else 
        {WriteSDA_0();}
        
        data<<=1;
        WriteSCL_1();            //拉高时钟线
        delay_us(5);          //延时
        WriteSCL_0();            //拉低时钟线
        delay_us(5);          //延时
    }
    OLED_RecvACK();
}
 
 
/**************************************
从IIC总线接收一个字节数据
**************************************/
unsigned char OLED_RecvByte()
{
    unsigned char i;
    unsigned char dat = 0;
    WriteSDA_1();                  //使能内部上拉,准备读取数据,
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;
        SCL = 1;                //拉高时钟线
        delay_us(5);                 //延时
        dat |= SDA;             //读数据               
        SCL = 0;                //拉低时钟线
         delay_us(5);                 //延时
    }
    return dat;
 
}
