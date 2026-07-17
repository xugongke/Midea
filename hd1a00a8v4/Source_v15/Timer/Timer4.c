#include "api_ext.h"
#include "SH79F326.h"
#include"cpu.h"
#include"timer_define.h"
#include "intrins.h"
/*****************************************************************************************************
 *  Function Name: init_timer4
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
void init_timer4()
{
  IEN0 |= 0x80;     //EA
  IEN1 |= 0x04;		//ET4,中断使能
  _push_(INSCON);
  Select_Bank1();
  T4CON &=0xb2;	   //make sure T4CON bit6,3,2,0 = 0
  //配置定时器的初值为0x0,溢出时间为0x10000个系统时钟
  TL4 = 0x0;		 
  TH4 = 0x0;
  T4CON |=0x02;  //启动定时器4
  _pop_(INSCON);
}