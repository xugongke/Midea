#include	<intrins.h>	
#include	<ABSACC.h>	
#include 	<SH79F326.h>
#include 	"tk.h"	


//===============================================================================================
//								中颖 SH79F326 触摸库 v1
//							-----------------------------
//	触摸按键测试程序							
//	
//===============================================================================================

#ifdef	TEST_TK_DATA

//#define	TK_PC_UART0
#define	TK_PC_UART1			//用的是SWE口，又可以仿真又可以发数据给电脑

//===============================================================================================
//								UART0 发触摸数据到电脑	
//===============================================================================================

#ifdef	TK_PC_UART0

#pragma message "P0.1 UART0 send touch data to PC"


unsigned char	xdata	gBUartSendFg;
bit		gbUartTran;
unsigned char 	xdata	gBUartLen;
unsigned char 	xdata	gBUartTotalByte;
unsigned char 	xdata	gBUartBuf[17*8+3];


void UARTRxD_Interrupt(void) interrupt 4
{
	_push_(INSCON);
	INSCON = 0;
	_push_(FLASHCON);
	FLASHCON = 0;
	//-----------------------------------------------------------	
    if(RI)
	{
		RI= 0;		
	}
	if(TI)
	{		
		TI = 0;
		if(gbUartTran)
		{
			if(gBUartLen <gBUartTotalByte	)
			{
				SBUF	=	gBUartBuf[gBUartLen];				
				gBUartLen++;
			}
			else
			{
				gbUartTran	    = 0;				
			}	
		}
	}
	
	//-----------------------------------------------------------
	INSCON = 0;
	_pop_(FLASHCON);
	_pop_(INSCON);
}



void	UART_INIT(void)
{	
	INSCON |= 0x40;   
	UART0CR = 0x10	+	0x00;			//TXD -- P0.1	RXD -- P0.0
	INSCON &= 0xBF;

	P0_1= 1;			   //TXD
	P0CR |= b1;

	P0_0= 1;			   //RXD
	P0PCR |= b0;
	P0CR &= (~b0);

	
     
	SFINE	=	8;							//12M/(8*16+7) = 100000bps
	SBRTL	=	(32768-7)&0xff;
	SBRTH	=	(((32768-7)>>8)&0xff) | 0x80;
	SCON = 0x40;
	REN = 0;			//只发送不接收
	

	
	ES0 = 1;
}


//一次性发送模式
extern	unsigned char	getTKStable(unsigned char idx);
extern	unsigned int		xdata	gWNoise_out,gWNoise2_out,gWNoise3_out;

void UART_Txd_TKDEBUG(void)
{		
	unsigned char i,j,k,x;
	unsigned int sample,base;


	if(gbUartTran == 0)
	{
		x = 0;
		gBUartBuf[x++] = 0x69;

		gBUartBuf[x++] = ((GetTKNum())<<3)+1;
	//	gBUartBuf[x++] = ((GetTKNum()+1)<<3)+1;		//增加一个假通道，发送时间
		for(k = 0;k<GetTKNum();k++)		
	//	for(k = 0;k<(GetTKNum()+1);k++)				//增加一个假通道，发送时间
		{		
			if(k==GetTKNum())
			{
				sample = base = getTKCxt();			//增加一个假通道，发送时间
                          //	sample = base = getTKResult();
			}	
			else
			{
				sample = GetTKSample(k);// 触摸原始数据
				base   = GetTKBase(k);   //触摸基准值	
			
				if(base==0)
				{
					sample=0;
				}	
			}

			gBUartBuf[x++] = 0x02;
			gBUartBuf[x++] = k;		
			gBUartBuf[x++] = (base>>8)&0xff;	
			gBUartBuf[x++] = (base)&0xff;
					
			gBUartBuf[x++] = 0x12;
			gBUartBuf[x++] = k;		
			gBUartBuf[x++] = (sample>>8)&0xff;		
			gBUartBuf[x++] = (sample)&0xff;
		}
					
		for(i=0,j=0;i<x;i++)
		{
			j+= gBUartBuf[i];
		}
		gBUartBuf[x++] = j;
		gBUartLen = 0;
		gBUartTotalByte = x;
					
		gbUartTran = 1;

	
		TI = 1;				
		
	}
}
#endif

//===============================================================================================
//								UART1 发触摸数据到电脑	
//===============================================================================================

#ifdef	TK_PC_UART1

#pragma message "SWE P4.0 UART1 send touch data to PC"

	unsigned char		gBUartSendFgAA;
bit		gbUartTranAA;
unsigned char 	xdata	gBUartLenAA;
unsigned char 	xdata	gBUartTotalByteAA;
unsigned char 	xdata	gBUartBufAA[17*8+3];


void UARTRxD_Interrupt(void) interrupt 15
{
	//---------------------------------------------------------------------------
	//所有中断前面加这4句话
	_push_(INSCON);                           //inscon -- bank0/1
	INSCON  = 0;															//flashcon -- bank0
	_push_(FLASHCON);                         //
	FLASHCON = 0;
	//---------------------------------------------------------------------------

	INSCON |= 0x40;
    if(RI1)
	{
		RI1= 0;		
	}
	if(TI1)
	{		
		TI1 = 0;
		if(gbUartTranAA)
		{
			if(gBUartLenAA <gBUartTotalByteAA	)
			{
				SBUF1	=	gBUartBufAA[gBUartLenAA];				
				gBUartLenAA++;
			}
			else
			{
				gbUartTranAA	    = 0;				
			}	
		}
	}
	INSCON &= 0xBF;

	//---------------------------------------------------------------------------
	//所有中断后面加这3句话
	INSCON = 0;																//flashcon is bank0
	_pop_(FLASHCON);                          // 
	_pop_(INSCON);                            //inscon -- bank0/1
}



void	UART_INIT(void)
{	
	INSCON |= 0x40;   
	UART1CR = 0x20	+	0x03;			//TXD1 -- P4.0	RXD1 -- P4.1
	INSCON &= 0xBF;

// 	P4_0= 1;			   //TXD
// 	P4CR |= bit0;
// 	
// 	P4_1= 1;			   //RXD
// 	P4PCR |= bit1;
// 	P4CR &= (~bit1);

	    P4PCR |= 0X2; //使能TX上拉

    INSCON |= 0x40;   
	SFINE1	=	8;							//12M/(7*16+8) = 100000bps
	SBRTL1	=	(32768-7)&0xff;
	SBRTH1	=	(((32768-7)>>8)&0xff) | 0x80;
	SCON1 = 0x40;
	REN1 = 0;			//只发送不接收
	INSCON &= 0xBF;

	
	IEN2 |= 0x01;
}


//一次性发送模式
extern	unsigned char	getTKStable(unsigned char idx);
extern	unsigned int		xdata	gWNoise_out,gWNoise2_out,gWNoise3_out;

unsigned int PC_TIME;
void UART_Txd_TKDEBUG(void)
{		
	unsigned char i,j,k,x;
	unsigned int sample,base;


	if(gbUartTranAA == 0)
	{
		x = 0;
		gBUartBufAA[x++] = 0x69;

	//	gBUartBufAA[x++] = ((GetTKNum())<<3)+1;		//1.正常模式
		gBUartBufAA[x++] = ((GetTKNum()+1)<<3)+1;		//2.增加一个假通道，发送时间


	//	for(k = 0;k<GetTKNum();k++)					//1.正常模式
		for(k = 0;k<(GetTKNum()+1);k++)				//2.增加一个假通道，发送时间
		{		
			if(k==GetTKNum())
			{
			//	sample = base = getTKCxt();		
						sample = base = PC_TIME;	
						PC_TIME++;
			}	
			else
			{
				sample = GetTKSample(k);// 触摸原始数据
				base   = GetTKBase(k);   //触摸基准值	
		
			//	sample = GetTKRaw(k);
			//	base = GetTKRaw(k);
				if(base==0)
				{
					sample=0;
				}	
			}

			gBUartBufAA[x++] = 0x02;
			gBUartBufAA[x++] = k;		
			gBUartBufAA[x++] = (base>>8)&0xff;	
			gBUartBufAA[x++] = (base)&0xff;
					
			gBUartBufAA[x++] = 0x12;
			gBUartBufAA[x++] = k;		
			gBUartBufAA[x++] = (sample>>8)&0xff;		
			gBUartBufAA[x++] = (sample)&0xff;
		}
					
		for(i=0,j=0;i<x;i++)
		{
			j+= gBUartBufAA[i];
		}
		gBUartBufAA[x++] = j;
		gBUartLenAA = 0;
		gBUartTotalByteAA = x;
					
		gbUartTranAA = 1;

		INSCON |= 0x40;
		TI1 = 1;				
		INSCON &= 0xBF;
	}
}

#endif	

#endif	