#include "SH79F326.h"
#include "intrins.h"
#include "cpu.h"

void Delay()
{
	UCHAR i=0x0a;
	while(i--);
}


 /*****************************************************************************************************
 *  Function Name: select_bank0
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   切换到寄存器bank0 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
//#if 0
void select_bank0()
{
      INSCON=0x00;
}
//#endif
 /*****************************************************************************************************
 *  Function Name: select_bank1
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   切换到寄存器bank0 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
//#if 0
void select_bank1()
{
     INSCON=0x40;
}
//#endif
 /*****************************************************************************************************
 *  Function Name: power_pd
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   系统进入PD模式 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/
#if 0
void power_pd()
{
	 SUSLO=0x55;
	 PCON=0x02;
	 _nop_();
	 _nop_();
	 _nop_();
}

 /*****************************************************************************************************
 *  Function Name: power_idle
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         None
 *  Output:        None
 *  Description:   系统进入IDLE模式 
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/

 void power_idle()
{
	 SUSLO=0x55;
	 PCON=0x01;
	 _nop_();
	 _nop_();
	 _nop_();
}
#endif