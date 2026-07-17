#include "SH79F326.h"
#include "intrins.h"
#include "cpu.h"
#include "api_ext.h"
void PWM_Test()
{
  CLKCON &= 0x9F;	 //系统时钟不分频
  init_pwm0();
  init_pwm1();
  while(1);


}