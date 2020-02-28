#include <ioCC2530.h>
#include "type.h"
#include "hal_lcd.h"

void main(void)
{   
          HalLcd_HW_Init(); //≥ı ºªØLCD      
          HalLcd_HW_WriteLine(1,"nanjing gebi dianzi");
          HalLcd_HW_WriteLine(3,"gebidianzi.taobao.com");
          HalLcd_HW_WriteLine(5,"CC2530BOARD");
}