#include <ioCC2530.h>
#include "type.h"
#include "hal_lcd.h"

__code const uint8 ascii_table_5x7[][5];

/**************************************************************************************************
 * @fn      HalLcd_HW_WaitUs
 *
 * @brief   wait for x us. @ 32MHz MCU clock it takes 32 "nop"s for 1 us delay.
 *
 * @param   x us. range[0-65536]
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WaitUs(uint16 microSecs)
{
  while(microSecs--)
  {
    /* 32 NOPs == 1 usecs */
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
    asm("nop"); asm("nop");
  }
}

uint8 getStrlen(char *p)
{
    int len=0;
    while(*p!='\0')
    {
        p++;
        len++;
    }
    return len;
}

/**************************************************************************************************
 * @fn      halLcd_ConfigIO
 *
 * @brief   Configure IO lines needed for LCD control.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
static void halLcd_ConfigIO(void)
{
    #define cs1    P1_5	
    #define rs     P1_6	
    #define sclk   P1_2     
    #define sid    P0_0
        P0SEL &= 0xFE;           //让P00为普通IO口，
        P0DIR |= 0x01;           //让P00为输出
        P1SEL &= 0x9b;           //让P12，P15，P16为普通IO口
        P1DIR |= 0x6C;           //把P12，P15，P16 设置为输出
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Control
 *
 * @brief   Write 1 command to the LCD
 *
 * @param   uint8 cmd - command to be written to the LCD
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Control(uint8 cmd)
{
  	unsigned char j;

	cs1 = 0;
        rs = 0;
	
	for (j = 0; j < 8; j++)
	      {
		sclk=0;
                  if(cmd&0x80)  sid = 1;
                else sid = 0;
                HalLcd_HW_WaitUs(1);
		sclk = 1;
		HalLcd_HW_WaitUs(1);
		cmd = cmd << 1;
	      }
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Write
 *
 * @brief   Write 1 byte to the LCD
 *
 * @param   uint8 data - data to be written to the LCD
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Write(uint8 data)
{
  	char j;

	cs1 = 0;
	rs = 1;
	
	for (j = 0; j < 8; j++)
	{
		sclk = 0;
                  if(data&0x80)  sid = 1;
                else sid = 0;
		sclk = 1;
		data = data << 1;
	}
}


/**************************************************************************************************
 * @fn      HalLcd_HW_Clear
 *
 * @brief   Clear the HW LCD
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Clear(void)
{
    	int i,j;
	for (i = 0; i < 9; i++)
	{
          cs1 = 0;
		HalLcd_HW_Control(0xb0+i);
		HalLcd_HW_Control(0x10);
		HalLcd_HW_Control(0x00);
		for (j = 0; j < 132; j++)
		{
			HalLcd_HW_Write(0x00);
		}
	}
}

/**************************************************************************************************
 * @fn      HalLcd_HW_Init
 *
 * @brief   Initilize HW LCD Driver.
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_Init(void)
{       
    HalLcd_HW_WaitUs(20);
    halLcd_ConfigIO();
        
	HalLcd_HW_Control(0xe2);          //软复位
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0x2c);          //升压步聚1
    HalLcd_HW_WaitUs(15);
	HalLcd_HW_Control(0x2e);          //升压步聚2
    HalLcd_HW_WaitUs(15);
	HalLcd_HW_Control(0x2f);          //升压步聚
    HalLcd_HW_WaitUs(15);
	HalLcd_HW_Control(0x23);          //粗调对比度，可设置范围20～27
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0x81);          //微调对比度
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0x1f); 
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0xa1);
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0xc8);
    HalLcd_HW_WaitUs(15);
	HalLcd_HW_Control(0xa0);          //行扫描顺序：从上到下
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0x40);          //升压电路设置指令代码
    HalLcd_HW_WaitUs(15);
        HalLcd_HW_Control(0xaf);          //开显示 
        HalLcd_HW_Clear(); 
}

/*
作用    设置LCD 文本显示的其实行和列
参数1   line,范围:0~7,即能够显示的行为1~8行，也就是lcd手册里提到的page
参数2   col,范围:0~127,即lcd的总列数，显示的起始位置可以设置到每一列
*/
void set_ddram_line_col(uint8 line,uint8 col)
{

  uint8 page,coll,coll_l,coll_h;
  page = line;
  coll = col;
  coll_h = coll>>4;
  coll_l = coll&0x0f;
  HalLcd_HW_Control(0xB0+page);
  HalLcd_HW_WaitUs(15); // 15 us
  HalLcd_HW_Control(0x10+coll_h);
  HalLcd_HW_WaitUs(15); // 15 us
  HalLcd_HW_Control(0x00+coll_l);
  HalLcd_HW_WaitUs(15); // 15 us
}

/*
作用     显示一个字节的字符，该字符大小为宽5个点，高7个点
参数1    page,范围0~7,共8行
参数2    column,范围0~127
参数3    text,要显示的字符，该值为ascii码
*/
void DisplayByte_5x7(uint8 page,uint8 column,char text)
{
	int j,k;

	if((text>=0x20)&&(text<0x7e)){/*需要显示的文字*/
		j=text-0x20;/*寻址，通过字符的ascii码找到点阵库中的改字符的位置*/
		set_ddram_line_col(page,column);
		for(k=0;k<5;k++)
		{
			HalLcd_HW_Write(ascii_table_5x7[j][k]);/*显示5x7的ASCII字到LCD上，y为页地址，x为列地址，最后为数据*/
		}
		//第六列写入0，即清除上一次留下来的数据
		HalLcd_HW_Write(0x00);
		column+=6;
	}else if(text==0x00){/*不需要显示，清空指定位置*/
		set_ddram_line_col(page,column);
		for(k=0;k<5;k++){
			HalLcd_HW_Write(0x00); //清空指定的字符位置
		}
	}

}

/**************************************************************************************************
作用    向指定的行和列写入一个字符
参数1   line，范围1~8,带显示的行,注意这里的范围是1~8,而不是0~7,目的是兼容上层的代码
参数2   col，范围1~LCD_MAX_LINE_LENGTH,待显示的列,注意，这里将128等分成LCD_MAX_LINE_LENGTH个区域，每个区域显示一个字符
参数3   text，需要显示的ascii字符
 **************************************************************************************************/
/**************************************************************************************************
 * @fn      HalLcd_HW_WriteChar
 *
 * @brief   Write one char to the display
 *
 * @param   uint8 line - line number that the char will be displayed
 *          uint8 col - colum where the char will be displayed
 *
 * @return  None
 **************************************************************************************************/
void HalLcd_HW_WriteChar(uint8 line, uint8 col, char text)
{
  //吧这个函数内容屏蔽掉，添加自己的
  /*if (col < HAL_LCD_MAX_CHARS)
  {
    SET_DDRAM_ADDR((line - 1) * HAL_LCD_MAX_CHARS + col);
    HalLcd_HW_Write(text);
  }
  else
  {
    return;
  }*/
  
  //以下是我加的
  	uint8 column = 1+col*6;
	uint8 page = line-1;
	if(col > 21)/*超出部分不显示*/
		return;
	DisplayByte_5x7(page,column,text);
}
/**************************************************************************************************
作用    向指定行指定列写入一串字符串
参数1   line。范围1~8
参数2   pText。待显示的字符串
 **************************************************************************************************/
void HalLcd_HW_WriteLine(uint8 line, const char *pText)
{

     uint8 count; 
     uint8 totalLength = getStrlen( (char *)pText );
     for (count=0; count<totalLength; count++)
     {
        HalLcd_HW_WriteChar(line,count,*pText);
        pText++;
     }  
            /* Write blank spaces to rest of the line */
  for(count=totalLength; count<21;count++)
  {
    HalLcd_HW_WriteChar(line, count, 0x00);
  }
}



__code const uint8 ascii_table_5x7[][5] = {
  {0x00,0x00,0x00,0x00,0x00},//space
  {0x00,0x00,0x4f,0x00,0x00},//!
  {0x00,0x07,0x00,0x07,0x00},//"
  {0x14,0x7f,0x14,0x7f,0x14},//#
  {0x24,0x2a,0x7f,0x2a,0x12},//$
  {0x23,0x13,0x08,0x64,0x62},//%
  {0x36,0x49,0x55,0x22,0x50},//&
  {0x00,0x05,0x07,0x00,0x00},//]
  {0x00,0x1c,0x22,0x41,0x00},//(
  {0x00,0x41,0x22,0x1c,0x00},//)
  {0x14,0x08,0x3e,0x08,0x14},//*
  {0x08,0x08,0x3e,0x08,0x08},//+
  {0x00,0x50,0x30,0x00,0x00},//,
  {0x08,0x08,0x08,0x08,0x08},//-
  {0x00,0x60,0x60,0x00,0x00},//.
  {0x20,0x10,0x08,0x04,0x02},///
  {0x3e,0x51,0x49,0x45,0x3e},//0
  {0x00,0x42,0x7f,0x40,0x00},//1
  {0x42,0x61,0x51,0x49,0x46},//2
  {0x21,0x41,0x45,0x4b,0x31},//3
  {0x18,0x14,0x12,0x7f,0x10},//4
  {0x27,0x45,0x45,0x45,0x39},//5
  {0x3c,0x4a,0x49,0x49,0x30},//6
  {0x01,0x71,0x09,0x05,0x03},//7
  {0x36,0x49,0x49,0x49,0x36},//8
  {0x06,0x49,0x49,0x29,0x1e},//9
  {0x00,0x36,0x36,0x00,0x00},//:
  {0x00,0x56,0x36,0x00,0x00},//;
  {0x08,0x14,0x22,0x41,0x00},//<
  {0x14,0x14,0x14,0x14,0x14},//=
  {0x00,0x41,0x22,0x14,0x08},//>
  {0x02,0x01,0x51,0x09,0x06},//?
  {0x32,0x49,0x79,0x41,0x3e},//@
  {0x7e,0x11,0x11,0x11,0x7e},//A
  {0x7f,0x49,0x49,0x49,0x36},//B
  {0x3e,0x41,0x41,0x41,0x22},//C
  {0x7f,0x41,0x41,0x22,0x1c},//D
  {0x7f,0x49,0x49,0x49,0x41},//E
  {0x7f,0x09,0x09,0x09,0x01},//F
  {0x3e,0x41,0x49,0x49,0x7a},//G
  {0x7f,0x08,0x08,0x08,0x7f},//H
  {0x00,0x41,0x7f,0x41,0x00},//I
  {0x20,0x40,0x41,0x3f,0x01},//J
  {0x7f,0x08,0x14,0x22,0x41},//K
  {0x7f,0x40,0x40,0x40,0x40},//L
  {0x7f,0x02,0x0c,0x02,0x7f},//M
  {0x7f,0x04,0x08,0x10,0x7f},//N
  {0x3e,0x41,0x41,0x41,0x3e},//O
  {0x7f,0x09,0x09,0x09,0x06},//P
  {0x3e,0x41,0x51,0x21,0x5e},//Q
  {0x7f,0x09,0x19,0x29,0x46},//R
  {0x46,0x49,0x49,0x49,0x31},//S
  {0x01,0x01,0x7f,0x01,0x01},//T
  {0x3f,0x40,0x40,0x40,0x3f},//U
  {0x1f,0x20,0x40,0x20,0x1f},//V
  {0x3f,0x40,0x38,0x40,0x3f},//W
  {0x63,0x14,0x08,0x14,0x63},//X
  {0x07,0x08,0x70,0x08,0x07},//Y
  {0x61,0x51,0x49,0x45,0x43},//Z
  {0x00,0x7f,0x41,0x41,0x00},//[
  {0x02,0x04,0x08,0x10,0x20},// 斜杠
  {0x00,0x41,0x41,0x7f,0x00},//]
  {0x04,0x02,0x01,0x02,0x04},//^
  {0x40,0x40,0x40,0x40,0x40},//_
  {0x01,0x02,0x04,0x00,0x00},//`
  {0x20,0x54,0x54,0x54,0x78},//a
  {0x7f,0x48,0x48,0x48,0x30},//b
  {0x38,0x44,0x44,0x44,0x44},//c
  {0x30,0x48,0x48,0x48,0x7f},//d
  {0x38,0x54,0x54,0x54,0x58},//e
  {0x00,0x08,0x7e,0x09,0x02},//f
  {0x48,0x54,0x54,0x54,0x3c},//g
  {0x7f,0x08,0x08,0x08,0x70},//h
  {0x00,0x00,0x7a,0x00,0x00},//i
  {0x20,0x40,0x40,0x3d,0x00},//j
  {0x7f,0x20,0x28,0x44,0x00},//k
  {0x00,0x41,0x7f,0x40,0x00},//l
  {0x7c,0x04,0x38,0x04,0x7c},//m
  {0x7c,0x08,0x04,0x04,0x78},//n
  {0x38,0x44,0x44,0x44,0x38},//o
  {0x7c,0x14,0x14,0x14,0x08},//p
  {0x08,0x14,0x14,0x14,0x7c},//q
  {0x7c,0x08,0x04,0x04,0x08},//r
  {0x48,0x54,0x54,0x54,0x24},//s
  {0x04,0x04,0x3f,0x44,0x24},//t
  {0x3c,0x40,0x40,0x40,0x3c},//u
  {0x1c,0x20,0x40,0x20,0x1c},//v
  {0x3c,0x40,0x30,0x40,0x3c},//w
  {0x44,0x28,0x10,0x28,0x44},//x
  {0x04,0x48,0x30,0x08,0x04},//y
  {0x44,0x64,0x54,0x4c,0x44},//z
  {0x08,0x36,0x41,0x41,0x00},//{
  {0x00,0x00,0x77,0x00,0x00},//|
  {0x00,0x41,0x41,0x36,0x08},//}
  {0x04,0x02,0x02,0x02,0x01},//~
};