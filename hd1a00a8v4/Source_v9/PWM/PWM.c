#include "SH79F326.h"
#include "intrins.h"
#include "cpu.h"
#include "api_ext.h"
#include "intrins.h"
void init_pwm0()
{
	Select_Bank1();
	PWMCR &= 0XF0;
	Select_Bank0();
	PWM0PL = 0xff;
	PWM0PH = 0xf;
	PWM0DL = 0xff;
	PWM0DH = 0x7;
	PWM0CON = 0x81;

}

void init_pwm1()
{
	_push_(INSCON);
	Select_Bank1();
	PWMCR |= 0X40;
	PWM1PL = 0xff;
	PWM1PH = 0xf;
	PWM1DL = 0xff;
	PWM1DH = 0x7;
	PWM1CON = 0xc1;
	_pop_(INSCON);
}