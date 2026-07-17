#include "api_ext.h"
#include "SH79F326.h"
#include"cpu.h"
#include"timer_define.h"
#include "intrins.h"
/*****************************************************************************************************
 *  Function Name: init_timer3
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-16
 *  Input:         None
 *  Output:        None
 *  Description:   初始化定时器工作在相应的模式下
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
void init_timer3()
{
  IEN0 |= 0xA0;     //打开定时器3中断

  _push_(INSCON);
  Select_Bank1();
  T3CON = 0x02;	  //外部32.768kHz/128kHz为时钟源，1分频
  //配置定时器的初值为0xfffe,溢出时间为2个32.768kHz时钟
  TL3 = 0xfe;		 
  TH3 = 0xff;
  T3CON |=0x04;  //启动定时器3

  _pop_(INSCON);
}