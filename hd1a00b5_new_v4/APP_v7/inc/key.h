/***************************************************************************************************
*                    (c) Copyright 1992-2017 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          key.h
*\Description   
*\Note          
*\Log           2017.11.13    Ver 1.0    骆兆松
*               创建文件。
***************************************************************************************************/
#ifndef _KEY_H
#define _KEY_H
#include "board.h"
#include "gpio_cfg.h"

#define KEY_INVALID       0   /*没有按键按下*/
#define KEY_STOP_DOWN     1   /*停止按钮按下，该按钮优先级最高*/
#define KEY_QIDONG_DOWN   2   /*启动按钮按下，启动与点动按钮优先级相同, 当有按键同时按下时，则返回无效关键*/
#define KEY_DIANDONG_DOWN 3   /*点动按钮按下*/
#define KEY_ALL_UP        4   /*所有按钮都没有被按下*/

#define KEY_CFG_DIS      6    /*配置功能:界面按键*/
#define KEY_CFG_UP       7    /*配置功能:上翻按键*/
#define KEY_CFG_DOWN     8    /*配置功能:下翻按键*/
#define KEY_CFG_CON      9    /*配置功能:确认按键*/

#define KEY_STATE_DETECTING                                                2
#define KEY_STATE_DOWN                                                     1
#define KEY_STATE_UP                                                       0

/***************************************************************************************************
*\Function      KeyDetect
*\Description   按键检测,该函数中仅检测对外的停止、启动和点动按钮
*\Parameter     
*\Return        u8
*\Note          
*\Log           2017.11.13    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
u8 KeyDetect(void);

#endif /*_KEY_H*/
