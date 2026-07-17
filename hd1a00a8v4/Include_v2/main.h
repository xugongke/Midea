#ifndef _MAIN_H
#define _MAIN_H
#include "cpu.h"


#define UART1_SEND_EN       0   /*是否使能UART1发送功能   0:不使能 1:使能    量产时关闭*/

#define ACPOWER_ON_DIS_EN   1  /*是否使能市电上电显示   1:使能   0:不使能*/
#define AC_DOWN_TEMP        5  /*交流加热启动下限温度与上限温度差值*/

#define WORK_MODE_ADDR      2 /*工作模式存储地址*/
#define MIX_MODE_TEMP_ADDR  1  /*混动模式下温度存储地址 温度范围30~60*/
#define AC_MODE_TEMP_ADDR   0  /*纯市电模式下温度存储地址 温度范围30~75*/

/*p0.2 LED_POWER*/
#define LED_POWER_ON P0_2 = 0
#define LED_POWER_OFF P0_2 = 1
#define LED_POWER_INIT() P0CR |= 0x04;P0PCR |= 0x04


#define HEAT_LOW_TEMP  30   /*加热最低温度 ℃*/

//#define YIN_CHUAN_VER   1 /*是否为银川版本程序*/

#define TICKS_ONE_SECOND (u16)(1000)
//#define SLEEP_TIME (u16)(5*60)  /*息屏时间*/
extern u8 SendBuf1[6]; //uart0 send buf
extern u8 SendLen1; //要发送总的字节数 
extern u8 SentLen1; //已经发送的字节数
extern u8 SendBuf[8]; //uart0 send buf
extern u8 SendLen; //要发送总的字节数 
extern u8 SentLen; //已经发送的字节数
extern u8 RecvBuf[8]; //uart0 接收字节缓冲区
extern u8 RecvLen; //接收长度

extern const u8 BCD_decode_tab[0x10];
void main();
void  isr_timer(void);
int display(unsigned char * buf);
extern vu8 Ctrlbits; /*按位表示继电器等的控制*/
extern vu8 FangdongFlag; //防冻标记
extern vu8 UserOpen;



#define LED_AC      0X03  /*DC INDEX:2*/
#define LED_DC      0X0c  /*AC INDEX:2*/
#define LED_MD_AC   0X30  /*市电模式  INDEX:2*/
#define LED_MD_DC   0XC0  /*光伏模式  INDEX:2*/

//#define LED_H       0X30  /*H  INDEX:5*/


#define LED_DEGREE  0x03  /*℃ index:3*/
#define LED_MD_HT   0X30  /*高温杀菌 index:3*/


u8 EEPROM_ByteRead(u8 Addr);
void EEPROM_ByteWrite(u8 ADDR,u8 da);

#define HEAT_ON()   RunStateFlag |= HEATING_FLAG      /*启动加热*/
#define HEAT_OFF()  RunStateFlag &= ~HEATING_FLAG; FangdongFlag = 0;  UserOpen = 0    /*停止加热*/

#define KEY_MODE    (u8)(1 << (3))  /*模式选择, 从0开始*/
#define KEY_UP      (u8)(1 << (1))  /*上调, 从0开始 */
#define KEY_DOWN    (u8)(1 << (2))  /*下调, 从0开始 */
#define KEY_SWTICH  (u8)(1 << (0))  /* 开关, 从0开始 */
//#define KEY_MODE    (u8)(1 << (4))  /*  从0开始*/

//#define KEY_BOOAST  (u8)(1 << (5))  /*速热模式*/

#define KEY_ALL    (u8)(KEY_UP | KEY_DOWN | KEY_SWTICH |KEY_MODE)

//#define KEY_VALUE_SET       2   /*预约设置按钮键值*/

#define KEY_VALUE_MODE      3   /*Mode模式选择按钮*/
#define KEY_VALUE_DOWN      4   /*下调按钮键值*/
#define KEY_VALUE_UP        5   /*上调按钮键值*/
#define KEY_VALUE_SWTICH    6   /*开关按钮键值*/
//#define KEY_VALUE_SW_LONG   7   /*开关按钮长按*/  
#define KEY_VALUE_HT        1   /*高温杀菌模式*/


//#define KEY_VALUE_DC_TEMP  8     /*设置直流温度上下限*/

 extern u8 key_valid; /*有效按键编号*/

 extern unsigned char sm1616_data_0[16];

 extern vs8 TempCur ; /*当前水箱温度*/

extern vu8 RefreshLED;  /*刷新LED*/
#define FSM_IDLE  0     /*空闲状态 开关关闭  待机状态*/
#define FSM_OPEN  1     /*开关状态打开  该状态下可以响应设置按钮*/
#define FSM_ERR   2     /*故障状态*/
#define FSM_SLEEP 3     /*休眠模式 在没有光伏情况下, 若是长时间没有按钮动作, 则考虑将屏幕熄灭*/

#define MODE_SHUT       0 /*关机状态*/
#define MODE_AC         1 /*速热模式(市电加热)*/
#define MODE_GF         2 /*纯光伏模式*/
#define MODE_MIX        3 /*混动模式*/
#define MODE_HIGH_TEMP  4 /*高温杀菌模式*/
#define MODE_OPEN       5 /*刚开机状态 中间转换状态*/

extern u8 fast_cnt; //用于尽快更新同步串口信息  小于4时快速同步
extern vs8 DCTempHeatUp; /*直流加热上限*/
extern vs8 TempHeatUp;    /*交流加热上限*/
extern vs8 TempHeatDown;  /*交流加热下限*/
#define MAX_SETTING_INDEX  (2+1)  /*最大索引号 0作为无效索引*/
extern u8 SetingIndex;  /*设置索引号*/
/*临时测试用变量 开始*/
extern u8 updown_flag;  /*用于指示设置过程中, 是否用户按下过上调或下调按钮*/
extern u8 code_index;
extern vu16 time_boot; /*秒时间戳*/
extern vu16 tick;  /*系统tick 单位:1/256s*/
extern vu32 second ; /*当前时钟*/
extern vu32 heat_start_time;  /*定时开起始时间*/
extern vu32 heat_off_time;    /*定时关结束时间*/
extern vu8 time_sec; /*定时秒数 专用于显示*/
extern vu8 dis_hour;  /*要显示的时钟 小时*/
extern vu8 dis_min;   /*要显示的时钟 分钟*/
extern vu8 revert;
extern vu8 time_dis;
#define SWITCH_FLAG     (1<<0)  /*开关状态 0:关闭   1:打开*/
#define TIMING_ON_FLAG  (1<<1)  /*定时开关 0:关闭   1:打开*/
#define TIMING_OFF_FLAG (1<<2)  /*定时开关 0:关闭   1:打开*/
#define HEATING_FLAG    (1<<3)  /*当前正在加热状态 0:空闲 1:正在加热*/
#define KEEPING_FLAG    (1<<4)  /*保温状态 0:空闲  1:正在保温状态*/
#define FAULT_FLAG      (1<<5)  /*故障状态  0: 正常  1:故障*/
#define FREEZE_FLAG     (1<<6)  /*防结冰  0:非自动  1:自动*/
//#define GF_FLAG       (1<<7)  /*光伏是否加热*/
 extern vu8 ErrCode;   /*错误编号*/
 extern vu8 RunStateFlag;   /*运行状态*/
 extern vu8 RunState;   /*状态机*/
 extern vu8 Mode;   /*模式*/
 extern bit done_flag; //加热完成标记
 extern bit ht_done_flag; //高温杀菌加热完成标记
 extern bit time_on_flag; // 预约到时标记
 extern const u8 BCD_decode_tab[0x10];
 extern bit ACHeatFlag; //交流市电指示
 extern bit  GFHeatFlag ; /*光伏加热是否有效  0:无效  1:有效*/
 extern bit GFFlag; /*光伏是否存在*/
 extern vu8  JLFlag ; /*交流是否有效  0:无效  1:有效*/

 extern vu8 Set_hour; /*要设置的时钟 小时*/ 
 //extern vu8 Set_min;  /*要设置的时钟 分钟*/

#define HEAT_DECTECT   60   /*防干烧检测周期 单位: s*/
#define HEAT_TEMP_SUB  15 /*判断干烧温度差值 ℃*/

 extern vu32 detect_start_time;   /*防干烧起始时间点*/
 extern vs8   start_temp;    /*防干烧起始温度*/
 extern u8 key_down; //按钮长时间被按下
 extern vu8 dis_index;   /*进度条闪烁索引*/

 extern u8 IrKey;  /*红外接收到的键值*/
 /*根据存储状态切换模式*/
 void ModeExchange(void);
 /***************************************************************************************************
 *\Function      DisInitState
 *\Description   故障显示
 ***************************************************************************************************/
 void DisErrState(void);

 /***************************************************************************************************
 *\Function      DisInitState
 *\Description   待机状态  
 ***************************************************************************************************/
 void DisInitState(void);

 /***************************************************************************************************
 *\Function      DisInitState
 *\Description   开机状态
 ***************************************************************************************************/
 void DisOpenState(void);

 void KeyProcess(u8 key);

#define DOWN 2
#define UP   1

#endif /*_MAIN_H*/
