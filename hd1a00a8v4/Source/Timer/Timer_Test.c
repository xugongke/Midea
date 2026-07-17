#include "api_ext.h"
#include "SH79F326.h"
#include "timer_define.h"
void Timer_Test()
{
  P0CR = 0x80;	//P0.7 as output
  CLKCON =0x08;
  Delay();
  CLKCON|=0x04;
  #ifdef TIMER3
  init_timer3();
  #endif
  #ifdef TIMER4
  init_timer4();
  #endif
  #ifdef TIMER5
  init_timer5();
  #endif
  while(1);

}