#include "SH79F326.h"
#include "intrins.h"
#include "cpu.h"
UCHAR ssp_flag;	 //防程序跑飞导致对flash的误操作的校验位
/*****************************************************************************************************
 *  Function Name: UCHAR EEPromByteRead(UCHAR nAddrH,UCHAR nAddrL)  
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-16
 *  Input:         nAddrH, nAddrL
 *  Output:        None
 *  Return:        nTemp
 *  Description:   从指定地址处读取1个byte返回
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
UCHAR EEPromByteRead(UCHAR nAddrH,UCHAR nAddrL)  				// 字节读,耗时2个时钟 
{
	UCHAR nTemp = 0 ;
	UINT16 nInt = 0 ;
//=====关闭总中断======
    _push_(IEN0);
    IEN0 &=0x7F;//关总中断	
//======访问EEPROM区===========
	FLASHCON = TRUE ;
//=====读取相应地址数据========
	nInt = (nAddrH << 8) + nAddrL;
	nTemp = *(( volatile UCHAR code  *) (nInt)) ;
//=======切回Flash区=========
	FLASHCON = FALSE ;
//====操作结束，恢复总中断=====
    _pop_(IEN0);
	return (nTemp) ;
}


//	一个扇区512个字节
/*****************************************************************************************************
 *  Function Name: void EEPromByteProgram(UCHAR nAddrH,UCHAR nAddrL, UCHAR nData)   
 *  Created By:    Cathy.feng
 *  Created Date:  2016-07-16
 *  Input:         nAddrH, nAddrL，nData
 *  Output:        None
 *  Return:        
 *  Description:   向指定地址处写入1个byte
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
void EEPromByteProgram(UCHAR nAddrH,UCHAR nAddrL, UCHAR nData)  // 字节编程，如果系统时钟=8M，耗时74 = 0x4a 个时钟
{
//=====SSP操作,关闭总中断======
 	_push_(IEN0);
	IEN0 &=	0x7F;
//======访问EEPROM区========
	FLASHCON = TRUE ;

	XPAGE = nAddrH ;			 			//0x00开始
	IB_OFFSET = nAddrL ;
	IB_DATA = nData ;

	IB_CON1	 = 0x6E ;
	IB_CON2	 = 0x05 ;
	IB_CON3	 = 0x0A ;
	IB_CON4	 = 0x09 ;
//=====增加flag判断，加强抗干扰能力======
	if(ssp_flag!=0x5A)
		goto Error;
	IB_CON5	 = 0x06 ;
	_nop_() ;				
	_nop_() ;
	_nop_() ;
	_nop_() ;	
Error:
	ssp_flag = 0;
	IB_CON1 = 0x00;
	IB_CON2 = 0x00;
	IB_CON3 = 0x00;
	IB_CON4 = 0x00;
	IB_CON5 = 0x00;
//=========切回Flash区======
	FLASHCON = FALSE ;
//=====SSP操作结束,恢复总中断======
 	_pop_(IEN0);
}
/*****************************************************************************************************
 *  Function Name: void EEPromSectorErase(UCHAR nAddrH)   
 *  Created By:    Cathy.Feng
 *  Created Date:  2016-07-19
 *  Input:         nAddrH
 *  Output:        None
 *  Return:        
 *  Description:   
 *  Modified By:   
 *  Modified Date:
 *  History:
 *  Others:                        
 *****************************************************************************************************/ 
void EEPromSectorErase(UCHAR nAddrH)// 扇区擦除，如果系统时钟=8M，耗时28000 = 0x6d60 个时钟 
{											// 总用时21ms,21000us/0.75 = 28000
//=====SSP操作,关闭总中断======
 	_push_(IEN0);
	IEN0 &=	0x7F;//关总中断	 
//======访问EEPROM区========
	FLASHCON = TRUE ;
	
	XPAGE = nAddrH ;			 			//0x00开始

	IB_CON1	 = 0xE6 ;	
	IB_CON2	 = 0x05 ;
	IB_CON3	 = 0x0A ;
	IB_CON4	 = 0x09 ;
//=====增加flag判断，加强抗干扰能力======
	if(ssp_flag!=0xA5)
		goto Error;
	IB_CON5	 = 0x06 ;
	_nop_() ;				
	_nop_() ;
	_nop_() ;
	_nop_() ;	
Error:
	ssp_flag = 0;
	IB_CON1 = 0x00;
	IB_CON2 = 0x00;
	IB_CON3 = 0x00;
	IB_CON4 = 0x00;
	IB_CON5 = 0x00;
//=========切回Flash区======
	FLASHCON = FALSE ;
//=====操作结束,恢复总中断======
 	_pop_(IEN0);
}
