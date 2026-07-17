/***************************************************************************************************
*                    (c) Copyright 1992-2017 Embedded Products Research Center
*                                       All Rights Reserved
*
*\File          board.h
*\Description   
*\Note          
*\Log           2017.11.09    Ver 1.0    骆兆松
*               创建文件。
***************************************************************************************************/
#ifndef _BOARD_H
#define _BOARD_H
#include "stm8s.h"
#define GetTimeusu16() (((uint16_t)TIM2->CNTRH << 8) | (uint16_t)(TIM2->CNTRL))
u32 GetTimeMsU32(void);

//#define TIME_DEBUG 

/*系统工作的状态机定义*/
#define FSM_IDLE            0   /*空闲状态*/
#define FSM_START_STAGE1    1   /*启动状态1, 在该状态下不进行过载处理*/
//#define FSM_START_STAGE2    2   /*启动状态2, 在该状态下进行过载处理*/
//#define FSM_DIANDONG_STAGE1 3   /*点动状态1, 在该状态下不进行过载处理，暂时定为5s*/
//#define FSM_DIANDONG_STAGE2 4   /*点动状态2, 在该状态下进行过载处理,与启动状态下的处理一样*/
//#define FSM_STOP_WAITING    5   /*停止等待，正转或反转状态下，切换为空闲状态或其他状态之前，先进入该状态，等待5s后进入空闲状态*/
//#define FSM_PARA_SET_STAGE  6   /*参数配置状态*/
//#define FSM_OVERLOAD_ST     7   /*进入过载状态*/
//#define FSM_UNBALANCE_ST    8   /*进入不平衡状态*/
#define FSM_WAIT_ALL_KEY_UP 15   /*等待所有的控制按钮都释放*/
#endif /*_BOARD_H*/