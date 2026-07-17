#include "SH79F326.h"
#include "intrins.h"
#include "cpu.h"
#include "Twi_define.h"
#include "api_ext.h"
void TwiInit()
{
//=====System Init==========
	CLKCON = 0x08;
	Delay();
	CLKCON |= 0x04;						// RC24M
//======TWI LCM==========
	select_bank1();
    TWICR = 0X45;		  				// SCK = P4.2  SDA = P4.3
	select_bank0();
//=======TWI Init==========
	TWITOUT = 0x02;			 			// Enable the internal pull-up resistor of TWI
	TWIBR = 28;			   			    // Set the baud rate, corresponding SCK clock
	TWISTA = 0x04;						// SCK clock setting, prohibit bus timeout
//=======Configure Slave Address and Response Signal=============
#ifdef S_MODE_Send
	TWIADR = (SLAVE_Address<<1)&0xFE;	// Configure slave address, prohibit response to general address
	TWIAMR = 0X00;						// Address is not blocked
	Send_ACK;							// ACK signal
#endif
#ifdef S_MODE_Receive
	TWIADR = (SLAVE_Address<<1)&0xFE;	// Configure slave address, prohibit response to general address
	TWIAMR = 0X00;						// Address is not blocked
	Send_ACK;							// ACK signal
#endif	
	TWTFREE = 0xFF;						// SCK high level timeout Count
	TWICON |= 0x40;						// Enable TWI
//======Master Send Start Signal=========
#ifdef M_MODE_Send
	TWICON |= 0x20;
#endif
#ifdef M_MODE_Receive
	TWICON |= 0x20;
#endif
//=========TWI Interrupt=====
	IEN1 |= 0x01;						// Enable TWI Interrupt
	EA = 1;
}