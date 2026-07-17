#include "intrins.h"
#include "SH79F326.h"
#include"pca_define.h"
#include"timer_define.h"
#include"twi_define.h"
#include "cpu.h"
#include "api_ext.h"
#include "main.h"
UINT16 Val;

unsigned char state;
unsigned char Send_Len = 0;				// Length of Transmiting Data
unsigned char Receive_Len = 0;			// Length of Receiving Data    
unsigned code Send_buf[8]={0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80};	// Transmit data
unsigned char Receive_buf[8];	// Receive data

vu16 time_boot; /*秒时间戳*/
vu8 time_sec; /*定时秒数 专用于显示*/
unsigned short time_cnt;
vu16 tick;  /*单位:1/128s*/
u8 time_flash; //高温杀菌模式下闪烁5秒用
vu32 second = (unsigned long)12*3600; /*当前时钟*/

#if 0
/*-------------------------------------------------------
FunctionName  : INT_TouchKey
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_TouchKey(void) interrupt 1
{ 
	_push_(INSCON);

	_pop_(INSCON);       
}
#endif
/*-------------------------------------------------------
FunctionName  : INT_TIMER5
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_TIMER5(void) interrupt 3
{ 
	_push_(INSCON);
   	//Select_Bank0();
	//P0_2 = ~P0_2;
    tick++;
    time_cnt++;
    //if(time_cnt == 128)   //1S LED8灯闪烁一次
    if(time_cnt >= TICKS_ONE_SECOND)   //1S LED8灯闪烁一次
    {
        time_sec++;
        time_boot++;
        second++;
        time_cnt=0;
        if (second >= ((unsigned long)24*(unsigned long)3600)){
            second = 0;
        }
        
        if (time_flash < 10){
            time_flash++;
        }
    }
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_EUART0
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_EUART0(void) interrupt 4
{ 
	_push_(INSCON);
    Select_Bank0();
    if (RI != 0){
	
         RecvBuf[RecvLen]=SBUF;
         RecvLen = (RecvLen+1)%8;
		 RI = 0;
    }

    if (TI != 0){
        //发送完成后
        TI = 0;
        if (SentLen < SendLen){
			SentLen %= 8;
            SBUF = SendBuf[SentLen];
            SentLen++;
        }
        else {
            //SentLen = SendLen;
            //发送已经完成
            if (SendLen != 0){
                SendLen = 0;
				RecvLen = 0;
                REN = 1; //使能接收功能
            }
        }
        RI=0; //清除可能得接收中断
    }
   
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_TIMER3
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/

void INT_TIMER3(void) interrupt 5
{ 
	_push_(INSCON);
	//Select_Bank0();
	//P0_7 = ~P0_7;
	_pop_(INSCON);
    
}

/*-------------------------------------------------------
FunctionName  : INT_ADC
Description   : 
Input         : 无
OutPut        : 无
Author        : jet
Date          : 2014.11.21
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
UINT16 Adc_Val;
void INT_ADC(void) interrupt 6
{ 
	_push_(INSCON);
    select_bank0();

	ADCON1 &=0xbf; 
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_TWI
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
#ifdef TWI_TEST
void INT_TWI(void) interrupt 7
{ 
	_push_(INSCON);
	TWICON &= 0xDF;	
	state = TWISTA & 0xf8;
	switch(state)
	{
//===============主机发送模式========
#ifdef M_MODE_Send
		case 0x08:	
			TWIDAT = (SLAVE_Address<<1)&0xFE;        // Send device address and write command
			break;
		case 0x18:
		case 0x20:                                   // Receive the ACK/NACK signal and send the first data
			TWIDAT = Send_buf[0];
			break;
		case 0x28:
		case 0x30:                                   // Receive the ACK/NACK signal, send the remaining 7 data, and then send the stop signal
			if(Send_Len<7)
			{
				TWIDAT = Send_buf[Send_Len+1] ;		
				Send_Len ++;
			}
			else
			{	
				TWICON |= 0x10;
			}
			break;
		/****Other modes****/
		case 0xF8:
	        STO = 1;
			break;	
		case 0x00:
			STO = 1;
			break;	
		default:
			STO = 1;
			break;		
#endif
//===============主机接收模式========
#ifdef M_MODE_Receive
		case 0x08:	
			TWIDAT = (SLAVE_Address<<1)|0x01;		// Send device address and read command
			break;
		case 0x40:									// Receive ACK signal, prepare to receive data and send ACK response
			Send_ACK;
			break;
		case 0x48:									// Receive NACK signal, end communication
			STO = 1;
			break;
		case 0x50:									// Receive data, return ACK, after receiving the last data, return NACK
			if(Receive_Len<7)
			{
				Receive_buf[Receive_Len] = TWIDAT;
				Receive_Len++;
				Send_ACK;
			}
			else
			{
				 Receive_buf[Receive_Len] = TWIDAT;
				 Receive_Len = 0;
				 Send_NACK;
			}
			break;
		case 0x58:								  	// The NACK signal has been sent back and end communication
			STO = 1;
			break;
	/****Other modes****/
		case 0xF8:
			STO = 1;
			break;	
		case 0x00:
			STO = 1;
			break;	
		default:
			STO = 1;
			break;
#endif
//===============从机发送模式========
#ifdef S_MODE_Send
		case 0xA8:		   					// Receive its own device address and read command
			Send_ACK;						
			TWIDAT = Send_buf[0];
		 	break;
		case 0xB8:							// Data has been transmitted, ACK response is received, continue to send data until the last data
			if(Send_Len<6)
			{
				TWIDAT = Send_buf[Send_Len+1];
				Send_Len++;
				Send_ACK;
			}
			else
			{
				TWIDAT = Send_buf[Send_Len+1];	  	// Send the last data, slave AA=0 as sign of the last data
				Send_Len = 0;
				Send_NACK;
			}		
			break;
		case  0xC0:							// A NACK response has been received, indicating that the communication is over and the bus is released
			Send_ACK;						// Prepare the next TWI slave communication response
			break;
		case 0xC8:							// The last data is ready to be sent, but the master still responds ACK signal, ending the communication
			Send_ACK;						// Prepare next TWI slave communication response
			break;
        /****Other modes****/
		case 0xF8:
			Send_ACK;
			break;	
		case 0x00:							// Abnormal, release the bus, prepare for the next TWI slave communication response
		    STO = 1;
			Send_ACK;
			break;	
		default:							// Abnormal, release the bus, prepare for the next TWI slave communication response
		    STO = 1;
			Send_ACK;
			break;		
#endif
//===============从机接收模式========
#ifdef S_MODE_Receive
		case 0x60:            // Receive its own device address and write command, prepare to receive data and ACK response
			Send_ACK;													  
			break;
		case 0x80:            // Data has been received, ACK has been responded; if it is not the last data received, ACK will always be returned, otherwise NACK will be returned
		    Receive_buf[Receive_Len] = TWIDAT;	
			if(Receive_Len<6)
			{				
				Send_ACK;
			}
			else              // When receiving the 7th data, prepare to respond a NACK for the 8th data
			{							
				Send_NACK;						
			}
			Receive_Len ++;			
			break;
		case 0x88:						  // Receive the last piece of data, NACK has been returned, release the bus; continue the next slave ACK response
			Receive_buf[Receive_Len] = TWIDAT;
			Send_ACK;
			break;
       /****Other modes****/
		case 0xF8:
	        Send_ACK;
			break;	
		case 0x00:
		    STO = 1;
			Send_ACK;
			break;	
		default:
		    STO = 1;
			Send_ACK;
			break;		
#endif 
	}	
	TWICON &= 0xF7;						// Clearn TINT			 
    _pop_(INSCON);   
}
#endif

/*-------------------------------------------------------
FunctionName  : INT_LED
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_LED(void) interrupt 8
{ 
	_push_(INSCON);
	select_bank0();
   
   	IEN1 &=0xFD;
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_TIMER4
Description   : 
Input         : 无
OutPut        : 无
Author        : jet
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_TIMER4(void) interrupt 9
{ 
      _push_(INSCON);
	Select_Bank0();
	P0_7 = ~P0_7;
	_pop_(INSCON);
}

/*-------------------------------------------------------
FunctionName  : INT_INT4
Description   : 
Input         : 无
OutPut        : 无
Author        : jet
Date          : 2014.11.21
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_INT4(void) interrupt 10
{ 
	_push_(INSCON);
   
   
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_INT3
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_INT3(void) interrupt 11
{ 
	_push_(INSCON);
   
   
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_INT2
Description   : 
Input         : 无
OutPut        : 无
Author        : jet
Date          : 2014.11.21
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_INT2(void) interrupt 12
{
	_push_(INSCON);
   
   
	_pop_(INSCON);      
}

/*-------------------------------------------------------
FunctionName  : INT_LPD
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_LPD(void) interrupt 13
{ 
	_push_(INSCON);
   	Select_Bank0();
	LPDCON &= 0xef;
	P0_7 = ~P0_7;
   
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_SCM
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_SCM(void) interrupt 14
{ 
	_push_(INSCON);
   
   
	_pop_(INSCON);       
}
									 #if 0
/*-------------------------------------------------------
FunctionName  : INT_EUART1
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_EUART1(void) interrupt 15
{ 
	_push_(INSCON);
   
    Select_Bank1();
    if (RI1 != 0){
       // RecvBuf[RecvLen]=SBUF;
       // RecvLen = (RecvLen+1)%8;
        RI1 = 0;
    }

    if (TI1 != 0){
        //发送完成后
        TI1 = 0;
        if (SentLen1 < SendLen1){
            SentLen1 %= 8;
            SBUF1 = SendBuf1[SentLen1];
            SentLen1++;
        }
        else {
            //SentLen = SendLen;
            //发送已经完成
            if (SendLen1 != 0){
                SendLen1 = 0;//                RecvLen1 = 0;
                //REN1 = 1; //使能接收功能
            }
        }
        //RI1=0; //清除可能得接收中断
    }

	_pop_(INSCON);       
}
								#endif
/*-------------------------------------------------------
FunctionName  : INT_CRC
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_CRC(void) interrupt 17
{ 
	_push_(INSCON);
   	Select_Bank0();
	CRCCON &= 0xbf;
	CRCCON |= 0x80;
   
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_PWM0
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_PWM0(void) interrupt 18
{ 
	_push_(INSCON);
   
   
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_PCA0
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_PCA0(void) interrupt 20
{ 
	_push_(INSCON);
   	Select_Bank1();
	P0CF = 0x0;
   
	_pop_(INSCON);       
}

/*-------------------------------------------------------
FunctionName  : INT_PWM1
Description   : 
Input         : 无
OutPut        : 无
Author        : Cathy.Feng
Date          : 2016.07.19
Other         : 注意事项、修改记录、函数版本等
--------------------------------------------------------*/
void INT_PWM1(void) interrupt 21
{ 
	_push_(INSCON);
   
   
	_pop_(INSCON);       
}

