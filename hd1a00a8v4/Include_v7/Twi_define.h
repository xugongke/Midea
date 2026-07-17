/*******************************SH79F326*************************************************************************/
#ifndef _TWI_DEFINE_H 
#define _TWI_DEFINE_H	
//工作模式宏定义
//#define M_MODE_Send	     // Master Transmit Mode
#define M_MODE_Receive     //	Master Receive Mode
//#define S_MODE_Send	     // Slave Transmit Mode
//#define S_MODE_Receive	 // Slave Receive Mode




#define Send_ACK	TWICON |= 0x04;
#define Send_NACK	TWICON &= 0xFB;
#define SLAVE_Address  0x66				// Slave address

#endif