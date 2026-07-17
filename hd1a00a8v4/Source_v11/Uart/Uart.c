#include "api_ext.h"
#include "SH79F326.h"
#include"cpu.h"

void init_uart()
{
  CLKCON =0x08;
  Delay();
  CLKCON|=0x04;
// 配置Uart工作在模式1
  select_bank1();
  UART0CR = 0x45;
  select_bank0();
  SCON=0x50; 
   /*配置波特率参数，波特率9600*/
   /* 计算公式：(int)X=FSY/(16*波特率) ;  SBRT=32768-X  ;   SFINE=（FSY/波特率）-16*X   FSY=8M*/
  SBRTH=0xff;
  SBRTL=0xcc;
  SFINE=0x01;

}
