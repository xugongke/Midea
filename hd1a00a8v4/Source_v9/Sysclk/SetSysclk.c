#include "SH79F326.h"
#include "intrins.h"
#include "api_ext.h"
#include "cpu.h"
#include "sysclk_define.h"
/*****************************************************************************************************
 *  Function Name: void SetSystemClk()
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   设置系统时钟 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
void SetClk()
 {
 
#ifdef 	HIGH_FREQUENCE 
	  CLKCON =0x08;
	  Delay();
	  CLKCON|=0x04;
#endif
#ifdef 	LOW_FREQUENCE  
	  CLKCON &=0xFB;	  //设置FS为0，选择OSC1CLK为系统时钟
	  CLKCON &=0xF7;      //关闭高频时钟OSC2CLK
	  CLKCON &=0x9F;	  //关闭分频
#endif
 }
