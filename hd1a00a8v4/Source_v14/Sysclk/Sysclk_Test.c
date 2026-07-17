#include "api_ext.h"
#include "SH79F326.h"
#include"cpu.h"
#ifdef Enable_sysclk_test
/*****************************************************************************************************
 *  Function Name: SysclkTest()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:    
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
void Sysclk_Test()
{
 
  P0CR |=0x80;    //配置Port端口的bit7为输出模式 
  P0_7 = 0;      //控制P0_7pin输出低电平
  SetClk(); 
  while(1)	//控制P0_0pin翻转测试系统clk
  {
   P0_7=~P0_7;

  }

}
#endif