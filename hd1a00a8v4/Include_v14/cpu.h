/*
*********************************************************************************************************
*                                         SH88F6161 Demo code 
*
*                                        (c) Copyright 2014-11-21
*                                           All Rights Reserved
*
* File    : CPU.H
* By      : JACK
* Version : V0.0
*********************************************************************************************************
*/
#ifndef _CPU_H 
#define _CPU_H

/**********************************************************************************************************
*                    定义与编译器无关的数据类型
**********************************************************************************************************/
typedef unsigned short              u16;    /*!<u16*/
typedef unsigned char               u8;     /*!<u8*/
 typedef unsigned  long       u32;    /*!<s32*/
typedef signed long                 s32;    /*!<s32*/
typedef signed short                s16;    /*!<s16*/
typedef signed char                 s8;     /*!<s8*/
typedef unsigned char  UCHAR;                   /* 无符号8位整型变量   */
typedef unsigned int   UINT16;                  /* 无符号16位整型变量 */
typedef signed   int   INT16;                  /* 有符号16位整型变量  */

typedef volatile unsigned long       vu32;   /*!<vu32*/
typedef volatile unsigned short     vu16;   /*!<vu16*/
typedef volatile unsigned char      vu8;    /*!<vu8*/

typedef volatile signed long         vs32;   /*!<vs32*/
typedef volatile signed short       vs16;   /*!<vs16*/
typedef volatile signed char        vs8;    /*!<vs8*/

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0x0
#endif

#ifndef NULL
#define NULL  0
#endif

#ifndef ON
#define ON  1
#endif

#ifndef OFF
#define OFF  0
#endif

/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
#endif