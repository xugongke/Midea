/***************************************************************************************************
* Copyright (C), 2016-2023, Sinowealth Tech. Co., Ltd.
* File name:    main.c                                                
* Author:       Cathy.Feng
* Version:      V0.0 
* Date:         2016-07-19                                            
* Description:  DEMO FOR 79F326                                                                                                                                                                   
* Others:                                                                 
* Function List:                                                    
1.  MAIN                            //Main route                           

* History:                                                          

1. Date:
Author:
Modification:

*****************************************************************************************************/
/****************************Include Files Start****************************************************/
#include "SH79F326.h"
#include "api_ext.h"
#include "cpu.h"
#include "myiic.h"
#include "string.h"
#include "main.h"
#include "intrins.h"
#include "tk.h"

vu8 Ctrlbits; /*按位表示继电器等的控制*/

//#define SLEEP_EN 1 /*是否使能息屏功能*/

vs8 TempHeatUp;    /*交流加热上限*/
vs8 TempHeatDown;  /*交流加热下限*/

vs8 DCTempHeatUp; /*直流加热上限*/

vs8 Errcnt;  /*防干烧滤波*/

vu16 sec_timeout; //模式下超时时间
vu32 heat_start_time;  /*定时开起始时间*/
vu32 heat_off_time;    /*定时关结束时间*/
vu32 detect_start_time;/*防干烧起始时间点*/
vu32 temp32;
vs8  start_temp;       /*防干烧起始温度*/
u16 time_sleep; /*sleep 定时器*/

vu8 dis_hour;  /*要显示的时钟 小时*/
vu8 dis_min;   /*要显示的时钟 分钟*/

u16 temp_refresh;
vs8 TempCur ; /*当前水箱温度*/
vs8 TempLast ; /*当前水箱温度*/
vu8 FangdongFlag; //防冻标记

u8 fast_cnt; //用于尽快更新同步串口信息  小于4时快速同步  

u8 last_data_buf;
u8 cur_key;
vu16 key_tick;
vu16 key_tick_long;
u8 key_down;

/****************************Global Variables Start*************************************************/
const u8 BCD_decode_tab[0x10] = { 0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X6F, 
    0X77, 0X7C, 0X58, 0X5E, 0X79, 0X71 };//BCD   最低位A..GH
/****************************Global Variables End**************************************************/

/****************************Funtion Quotation Start*************************************************/ 
#define  FEED_WDT() RSTSTAT=001; 
//读或者写RSTSTAT寄存器重新复位计数器

void WDTInit(void)
{
    RSTSTAT = 001; //1024ms溢出 
}
void timer5_init()
{  
    _push_(INSCON);  //系统时钟12MHZ
    Select_Bank1();
    //配置定时器的初值为0xff00,溢出时间为0x100个系统时钟
    T5CON = 0; //直接使用12M系统时钟 不分频
#if 0
    //48000-1 4ms一个中断
    //0xffff-bb80 = 447f
    TL5 = 0x7f;		 
    TH5 = 0x44;
#else
    //12000 1ms一个中断
    //0xffff-12000=D11F
    TL5 = 0x1f-2;		 
    TH5 = 0xD1;
#endif
    T5CON |= 0x02;  //启动定时器5
    _pop_(INSCON);
    IEN0 |= 0x88;     //EA, ET5 使能对应中断
}

u8 SendBuf[8]; //uart0 send buf
u8 SendLen; //要发送总的字节数 
u8 SentLen; //已经发送的字节数

u8 RecvBuf[8]; //uart0 接收字节缓冲区
u8 RecvLen; //接收长度


u8 SendBuf1[6]; //uart0 send buf
u8 SendLen1; //要发送总的字节数 
u8 SentLen1; //已经发送的字节数
#if UART1_SEND_EN
void Uart1Init(void){
    //配置Uart工作在模式1  8-N-1 
    SendLen1 = 0;
    SentLen1 = 0;
    //RecvLen1 = 0;
    select_bank1();
    UART1CR = 0x32; //配置引脚 TX P4.1 RX P4.0  
    
    SCON1=0x40;  //模式1  禁止接收 使能停止位检查
#if 0
    /*配置波特率参数，波特率9600*/
    /* 计算公式：(int)X=FSY/(16*波特率) ;  SBRT=32768-X  ;   SFINE=（FSY/波特率）-16*X   FSY=12M*/
    //x=12000000/(16*9600)=78.125   SBRT=32768-78=32690  SFINE=12000000/9600-16*78=1250-1248=2
    SBRTH1=0xfF;
    SBRTL1=0xB2;
    SFINE1=0x02;
    //REN1 = 1; //使能接收 
#else 
    /*配置波特率参数，波特率115200*/
    /* 计算公式：(int)X=FSY/(16*波特率) ;  SBRT=32768-X  ;   SFINE=（FSY/波特率）-16*X   FSY=12M*/
    //x=12000000/(16*115200)=6.51   SBRT=32768-6=32762=7ffa  SFINE=12000000/115200-16*6=1250-1248=8
    SBRTH1=0xfF;
    SBRTL1=0xfa;
    SFINE1=0x08;
#endif
    TI1 = 0;
    select_bank0();
    P4PCR |= 0X2; //使能TX上拉
    IEN2 |= 0x01;// 使能UART1中断
}

void uart1_send(u8 len)
{
    SendBuf1[0] = 0xa0;
    SendBuf1[1] = gWrCurKey>>8;
    SendBuf1[2] = (u8)gWrCurKey;
    SendBuf1[3] = 0x50;
    SendBuf1[4] = 0x51;
    SendBuf1[5] = 0x55;

    _push_(INSCON);  
    select_bank1();
    REN1 = 0; //关闭接收功能
    TI1 = 0;
    RI1 = 0; //手动清除一下中断标志 防止系统不处理中断
    SentLen1 = 1;
    SendLen1 = len;
    //select_bank1();
    //UART0CR = 0x45; //配置引脚 RX0 P2.7  TX0 P2.6
    //UART0CR = 0x10; //配置引脚 RX0 P0.0  TX0 P0.1
    // select_bank0();

    SBUF1 = SendBuf1[0];
    //启动发送
    _pop_(INSCON);  
}
#endif

void Uart0Init(void){
    //CLKCON =0x08;
    //Delay();
    // CLKCON|=0x04;
    //配置Uart工作在模式1  8-N-1 
    SendLen = 0;
    SentLen = 0;
    RecvLen = 0;

    select_bank1();
    //UART0CR = 0x45; //配置引脚 RX0 P2.7  TX0 P2.6
    UART0CR = 0x10; //配置引脚 RX0 P0.0  TX0 P0.1

    //UART0CR = 0x60; //配置引脚 RX0 P0.0  TX0 P5.2
    select_bank0();
    P0CR &= (~0x03);
    P0PCR |= 0X03;

    SCON=0x50;  //模式1  使能停止位检查

    /*配置波特率参数，波特率9600*/
    /* 计算公式：(int)X=FSY/(16*波特率) ;  SBRT=32768-X  ;   SFINE=（FSY/波特率）-16*X   FSY=12M*/
    //x=12000000/(16*9600)=78.125   SBRT=32768-78=32690  SFINE=12000000/9600-16*78=1250-1248=2

    SBRTH=0xfF;
    SBRTL=0xB2;
    SFINE=0x02;
    REN = 1; //使能接收 

    IEN0 |= 0x10;// 使能UART0中断
}



void uart0_send(u8 len)
{
    //配置
    //关闭接收功能 @todo
    REN = 0; //关闭接收功能
    TI = 0;
    RI = 0; //手动清除一下中断标志 防止系统不处理中断
    SentLen = 1;
    SendLen = len;
    //select_bank1();
    //UART0CR = 0x45; //配置引脚 RX0 P2.7  TX0 P2.6
    //UART0CR = 0x10; //配置引脚 RX0 P0.0  TX0 P0.1
    // select_bank0();

    SBUF = SendBuf[0];
    //启动发送
}

void SystemInit(void)
{
    //OP_0SC :Internal RC(24MHz)   （0000）  因此下面控制位都无效
    //fsys = foscs/2  24/2=12MHZ
    IEN0 = 0; //禁止所有中断
    CLKCON =0x20;
    FEED_WDT();
    //CLKCON =0x08;
    Delay();
    Delay();
    WDTInit();
    bus_Init();
    timer5_init();
    Uart0Init();
#if UART1_SEND_EN
    Uart1Init();
#endif
    LED_POWER_INIT();
    //LED_POWER_ON;
    LED_POWER_OFF;
    // P0CR |= 0x80; //p0.7 output
    //memset(sm1616_data_0, 0X00, sizeof(sm1616_data_0));
    //display(sm1616_data_0);
    InitKey();
    
}
/****************************Function Quotation End**************************************************/

u8 UartStReg;
u16 test_cnt;
u8 test_a;
UINT16 KeyValue;
u16 time_s;
u8 tick_s;
bit  FirstPower;

vs8 DCTempHeatUp; /*直流加热上限*/


vu8 ErrCode;   /*错误编号*/
vu8 RunStateFlag;   /*运行状态*/

vu8 UserOpen; /*自动模式下是否用户点击开关启动加热*/
vu8 RunState;   /*状态机*/
vu8 Mode;    /*运行模式*/
vu8 CloseCnt;

u16 long_press;
vu16 refresh_tick;
vu8 revert;
vu8 time_dis; /*是否显示时间*/
vu8 RefreshLED;     /*刷新LED*/
u8 key_valid; /*有效按键编号*/
u8 open_cnt; /*自动打开计数*/
bit done_close;//发生过用户主动关闭
extern volatile bit recok;

bit done_flag; //加热完成标记
bit ht_done_flag; //高温杀菌加热完成
bit time_on_flag; //预约到时标记
bit ACHeatFlag; //交流市电指示
bit  GFHeatFlag ; /*光伏加热是否有效  0:无效  1:有效*/
bit GFFlag; /*光伏是否存在*/
vu8  LastJLFlag ; /*交流是否有效  0:无效  1:有效*/
vu8  JLFlag ; /*交流是否有效  0:无效  1:有效*/

u8 LastSt; /*错误之前状态*/
u8 check;
u8 err_com; 
u8 err_temp;
u8 err_byte;
u8 RecvLen;
//u8 RecvBuf[5];
//u8 SendBuf[5];
u8 testdata = 0;
u8 code_index;
u8 SetingIndex;  /*设置索引号   10:设置直流温度*/
u8 updown_flag;  /*用于指示设置过程中, 是否用户按下过上调或下调按钮*/
void USER_PROGRAM_INITIAL()  
{ 
    fast_cnt = 0;
    ACHeatFlag = 0; //交流市电指示
    GFHeatFlag = 0; /*光伏加热是否有效  0:无效  1:有效*/
    GFFlag = 0; /*光伏是否存在*/
    LastJLFlag =0; /*交流是否有效  0:无效  1:有效*/
    JLFlag = 0; /*交流是否有效  0:无效  1:有效*/
    open_cnt = 0;
    done_close = 0;
    TempCur = 0;
    TempLast = 0;
    ErrCode = 0;
    err_com = 0;
    CloseCnt = 0;
    time_on_flag = 0;
    FirstPower = 0;
    RunState = FSM_IDLE;
    Mode = MODE_SHUT;
    SetingIndex = 0;
    FangdongFlag = 0;
    /*
    全局变量初始化, 编译器存在BUG，会造成非零全局变量的初始化值为0
    */
    second = 0;//(unsigned long)12*3600; //默认时间中午12点
    heat_start_time = second;
    heat_off_time = second + 3600;
    RefreshLED = 0;/*上电刷新一遍LED矩阵*/

    /*全局变量初始化  结束*/
    //     TempHeatUp = EEPROM_ByteRead(01);
    //     if ((TempHeatUp < HEAT_LOW_TEMP ) || (TempHeatUp > 75)){
    //         TempHeatUp = 75;
    //         EEPROM_ByteWrite(01, TempHeatUp); //地址1存储预约模式下设置温度
    //     }
    /*地址0 交流加热模式温度*/
    TempHeatUp = EEPROM_ByteRead(00);
    if ((TempHeatUp < HEAT_LOW_TEMP ) || (TempHeatUp > 75)){
        TempHeatUp = 75;
        EEPROM_ByteWrite(00, TempHeatUp);
    }
    TempHeatUp = EEPROM_ByteRead(MIX_MODE_TEMP_ADDR);
    if ((TempHeatUp < HEAT_LOW_TEMP ) || (TempHeatUp > 60)){
        TempHeatUp = 50;
        EEPROM_ByteWrite(MIX_MODE_TEMP_ADDR, TempHeatUp); //混动模式默认50℃
    }
    DCTempHeatUp = 75;

    TempHeatDown = TempHeatUp - AC_DOWN_TEMP;

    tick =0;
    refresh_tick = 0;
}

/*根据存储状态切换模式*/
void ModeExchange(void)
{
     Mode = EEPROM_ByteRead(WORK_MODE_ADDR);
    /*读取工作模式*/
     /*市电加热模式记忆使能,则自动切换到对应模式*/
     if ((Mode == 0) || (Mode > MODE_MIX)){
         Mode = MODE_MIX;
         EEPROM_ByteWrite(WORK_MODE_ADDR, Mode);
     }
     done_flag = 0;

    switch (Mode)
    {
    case MODE_AC: //纯市电加热模式
        TempHeatUp = EEPROM_ByteRead(AC_MODE_TEMP_ADDR);
        if ((TempHeatUp < HEAT_LOW_TEMP) || (TempHeatUp > 75)){
            TempHeatUp = 60;
            EEPROM_ByteWrite(AC_MODE_TEMP_ADDR, TempHeatUp);
        }
        TempHeatDown = TempHeatUp-AC_DOWN_TEMP;
        break;
    case MODE_GF: //纯光伏加热模式
        //不使用市电加热
        break;
    case MODE_MIX: //混动模式
        TempHeatUp = EEPROM_ByteRead(MIX_MODE_TEMP_ADDR);
        if ((TempHeatUp < HEAT_LOW_TEMP) || (TempHeatUp > 60)){
            TempHeatUp = 50;
            EEPROM_ByteWrite(MIX_MODE_TEMP_ADDR, TempHeatUp);
        }
        TempHeatDown = TempHeatUp-AC_DOWN_TEMP;
        break;
    default:
        break;
    }
}
u16 scnt;
u16 rcnt;
//此函数为用户程序，
void USER_PROGRAM()
{ 
    u8 i;
    
#if 0
    //@TODO test CODE
    GFFlag = 1;
    if (FirstPower != 0)
    {

        if (RefreshLED !=  0){
            State = FSM_OPEN;
            Mode = MODE_OPEN;
            DisOpenState();
            sm1616_data_0[0] = BCD_decode_tab[TempHeatUp/10];
            sm1616_data_0[1] = BCD_decode_tab[TempHeatUp%10];
            display(sm1616_data_0);
            LED_D1=1;
            LED_D2=1;
            if (key_valid != 0)
            {
                KeyProcess(key_valid);
            }
            key_valid = 0;
        }
        //         if (RefreshLED == 1){
        //             DisOpenState();
        //             display(sm1616_data_0);
        //         }

        if ((tick - refresh_tick) > (TICKS_ONE_SECOND/2)){
            RefreshLED = 1;
            refresh_tick = tick;
        }
        return;
    }
    else{
        if ((tick < (u16)(TICKS_ONE_SECOND*3)) && (FirstPower == 0)){
            CloseCnt = 1;
        }
        else {
            FirstPower = 1;
        }
        State = FSM_OPEN;
    }
    //end
#else
    //GFFlag = 1;
    //       if (key_valid)
    //       {
    //           KeyProcess(key_valid);
    //       }
    //     State = FSM_OPEN;
    //     Mode = MODE_OPEN;
#endif

    /*刷新LCD*/
    if (RefreshLED == 1){
        if ((tick < (u16)(TICKS_ONE_SECOND*3+TICKS_ONE_SECOND/4)) && (FirstPower == 0)){
            CloseCnt = 1;
        }
        else {
            FirstPower = 1;
        }

        /*延迟3秒屏幕全亮*/
        if (FirstPower == 0) {
            if ((GFFlag != 0) || (JLFlag != 0)){
                /*只有光伏电且电压低则不点亮*/
                memset(sm1616_data_0, 0xff, sizeof(sm1616_data_0));
                LED_POWER_ON;
                display(sm1616_data_0);
            }
            else {
                memset(sm1616_data_0, 0x00, sizeof(sm1616_data_0));
                display(sm1616_data_0);
                LED_POWER_OFF;
            }
        }
        else {
            if ((GFFlag == 0) && (JLFlag == 0)){
                RunState = FSM_IDLE; /*光伏和市电都不存在则不显示*/
            }
#if 1
            switch (RunState)
            {
            case FSM_IDLE:
                Mode = MODE_SHUT;
                DisInitState();
                break;
            case FSM_OPEN:
                DisOpenState();
                break;
            case FSM_ERR:
                Mode = MODE_SHUT;

                DisErrState();
                break;
            default:

                break;
            }

            display(sm1616_data_0);
            if (RunState == FSM_IDLE){
                LED_POWER_OFF;
            }
            else {
                LED_POWER_ON;
            }
#endif
        }
        RefreshLED = 0;
    }

    if ((tick < (TICKS_ONE_SECOND/4)) && (FirstPower == 0)){ /*开机等待*/
        return;
    }

    switch (RunState)
    {
    case FSM_IDLE: //待机状态
        if (GFFlag == 0){ /*没有光伏*/
        }
        DCTempHeatUp = 75;
        if (FirstPower == 1){
            DisInitState();
        }
        if (key_valid == KEY_VALUE_SWTICH){
            //TempHeatUp = EEPROM_ByteRead(AC_MODE_TEMP_ADDR);
            RunState = FSM_OPEN;
            Mode = MODE_OPEN;
			ModeExchange();
            done_flag = 0;
            LastJLFlag = JLFlag;
            SetingIndex = 0;
            RunStateFlag &= ~KEEPING_FLAG;
            DisOpenState();
            key_valid = 0;
            //break;
            //return;
        }
        break;
    case FSM_OPEN:
        UserOpen = 0;
        /*非高温杀菌模式下, 直流加热温度为75度  高温杀菌为80度*/
        if (Mode != MODE_HIGH_TEMP){
            DCTempHeatUp = 75;
        }

        if (key_valid == KEY_VALUE_SWTICH){
            RunState = FSM_IDLE;
            done_close = 1;
            Mode = MODE_SHUT;
            RefreshLED = 1;
            DisInitState();
            key_valid = 0;

            //return;//break;
        }
        else if (key_valid != 0){
            RefreshLED = 1;
            KeyProcess(key_valid);
            key_valid = 0;
        }

        if (SetingIndex == 0) {
            time_sec = 0;
        }
        if (time_sec > 3){
            time_sec = 0; 
            if (SetingIndex == 1){
                SetingIndex = 0; //温度设置完
                updown_flag = 0;
                if (Mode == MODE_AC)
                {
                    if (EEPROM_ByteRead(AC_MODE_TEMP_ADDR) != TempHeatUp){  /*存储纯电模式设置温度*/
                        EEPROM_ByteWrite(AC_MODE_TEMP_ADDR, TempHeatUp);
                    }
                }
                else {
                    if (EEPROM_ByteRead(MIX_MODE_TEMP_ADDR) != TempHeatUp){  /*存储混合模式设置温度*/
                        EEPROM_ByteWrite(MIX_MODE_TEMP_ADDR, TempHeatUp);
                    }
                }
                break;
            }

        }
        DisOpenState();
        break;
    case FSM_ERR:
        /*故障*/
        DisErrState();
        if (ErrCode == 5){
            /*通讯故障，检查是否通讯已恢复*/
            if (err_com < 10) {
                ErrCode = 0;
                RunState = LastSt; //恢复之前状态
                RunStateFlag &= ~FAULT_FLAG; 
            }
        }
        break;
    default:
        RunState = FSM_IDLE;
        break;
    }
    if (TempCur > 91) { //大于等于95℃ E4错误
        RunState = FSM_ERR;
        RunStateFlag |= FAULT_FLAG;
        HEAT_OFF();
        ErrCode = 3;
        DisErrState();
    }
    /*故障检查  传感器短路和断路故障*/
    if ((TempCur <= -10) || (TempCur > 119)){
        RunState = FSM_ERR;
        RunStateFlag |= FAULT_FLAG;
        HEAT_OFF();
        ErrCode = 4;
        DisErrState();
    }
    /*处理显示界面*/
    if (((tick - refresh_tick) > (TICKS_ONE_SECOND/2) && (fast_cnt >= 6))
        || ((tick - refresh_tick) > (TICKS_ONE_SECOND/5) && (fast_cnt < 6))
        ){
        if (fast_cnt < 6){
            fast_cnt++;
        }
        RefreshLED = 1;
        refresh_tick = tick;
        revert++;
        if (revert > 1){
            revert = 0;
        }
        /*只有光伏条件下自动打开显示屏*/
        if (RunState == FSM_IDLE){
#if ACPOWER_ON_DIS_EN
            if ((JLFlag == 1) ||  (GFFlag != 0)){ /*光伏或者市电存在则开机就显示*/
#else 
            if ((JLFlag == 0) && (GFFlag != 0)){ /*没有光伏*/
#endif
                if (revert == 0){
                    open_cnt++;
                }
                if (open_cnt > 2){
                    open_cnt = 0;
                    if (done_close == 0)
                    {
                         key_valid = KEY_VALUE_SWTICH;//只有光伏存在情况下 自动开机显示温度
                    }
                }
            }
            else{
                open_cnt = 0;
            }
        }

        SendBuf[0] = TempHeatUp;
//#if YIN_CHUAN_VER
 //       SendBuf[1] = TempHeatUp;//银川版本程序使用交直流一体温度
//#else
        SendBuf[1] = DCTempHeatUp;//
//#endif
        SendBuf[2] = 0;
        if ((RunStateFlag & HEATING_FLAG) != 0){ //交流加热
            SendBuf[2] |= 0x01;
        }

        SendBuf[3] = 0;
        check = 0;
        for (i = 0; i < 4; i++)
        {
            check = check + SendBuf[i];
        }
        check = ~check;
        SendBuf[4] = check;
        testdata += 4;
        memset(RecvBuf, 0x00,  5);
        //UartSend(SendBuf, 5);
        uart0_send(5);
#if UART1_SEND_EN
        uart1_send(5);
#endif
        scnt++;
        //RecvLen = UartSimuRecv(RecvBuf, 5, 4);

        if (err_com > 30)
        {
            if (FSM_ERR != RunState){
                LastSt = RunState; /*记录一下错误之前状态，异常消除后自动恢复*/
            }
            RunState = FSM_ERR;
            RunStateFlag |= FAULT_FLAG;
            HEAT_OFF();
            ErrCode = 5; //E5 通讯错误
            DisErrState();
        }
        else {
            err_com++;
        }
        
        if (TempCur >= TempHeatUp){ //任何情况下, 温度超过预设值都停止加热
            HEAT_OFF();
            UserOpen = 0;
        }
        /*加热逻辑控制*/
        switch (Mode)
        {
        case MODE_OPEN://中间过渡状态
            ModeExchange();
            break;
        case MODE_AC: //纯电模式
        case MODE_MIX:
            if (TempCur < TempHeatUp){
               // done_flag = 0;
                if (done_flag == 0){
                    if (JLFlag == 1){
                        HEAT_ON(); //启动加热
                    }
                }
                else {
                    /*完成一次加热后，则等待温度低于下限再启动加热*/
                     if (TempCur < TempHeatDown){
                         if (JLFlag == 1){
                             HEAT_ON(); //启动加热
                         }
                    }
                }
            }
            else {
                done_flag = 1;
               // Mode = MODE_AC; //改为保留当前模式不动
                HEAT_OFF();
            }
            break;
        case MODE_GF:
            /*纯光伏模式 不使用市电*/
            HEAT_OFF();
            break;
        case MODE_HIGH_TEMP: /*高温杀菌模式*/
            if (ht_done_flag == 0){
                sec_timeout = time_boot;
            }
            if (TempCur < TempHeatDown){
                done_flag = 0;
                HEAT_ON(); //启动加热
            }
            else if (TempCur >= TempHeatUp){
                done_flag = 1;
                ht_done_flag = 1;
                HEAT_OFF();
            }

            if ((u16)(time_boot - sec_timeout) > (u16)300){
                /*保温5min后退出该模式*/
                HEAT_OFF();
                Mode = MODE_OPEN;
            }
            break;

        default:
            HEAT_OFF();
            ModeExchange();
            break;
        }

        if (JLFlag == 0){
            HEAT_OFF();
        }
        //RunStateFlag |= FREEZE_FLAG;
        if ((JLFlag == 0) || (TempCur > 10)){
            RunStateFlag &= ~FREEZE_FLAG;
        }
    }

    if (RecvLen >= 5)
    {
        check = 0;
        for (i = 0; i < 4; i++) {
            check += RecvBuf[i];
        }
        check = ~check;

        if ((check == RecvBuf[4]) && (RecvBuf[3] == (u8)(~RecvBuf[0]))){ //校验合法
            err_com = 0;

            rcnt++;
            if (((s8)RecvBuf[3] < -10)||((s8)RecvBuf[3] > 94)){
                err_temp++;
                if (err_temp > 2){
                    err_temp=5;
                    TempCur = (s8)RecvBuf[3];
                }
            }
            else {
                err_temp=0;
                TempCur = (s8)RecvBuf[3];
            }
            if ((RecvBuf[2] & 0x04) != 0){ //交流电是否存在
                JLFlag = 1;
            }
            else {
                JLFlag = 0;
            }
            if ((RecvBuf[2] & 0x08) != 0){ //直流电是否存在
                GFFlag = 1;
            }
            else {
                GFFlag = 0;
            }
            if (RecvBuf[2]&0x01 != 0){
                /*交流加热指示*/
                ACHeatFlag = 1;
            }
            else {
                ACHeatFlag = 0;
            }
            if ((RecvBuf[2] & 0x02) != 0){ //直流是否加热
                GFHeatFlag = 1;
            }
            else {
                GFHeatFlag = 0;
            }
            if ((RecvBuf[2] & 0x10) != 0){
                /*光伏电压电压高于120v*/
                RunState = FSM_ERR;
                RunStateFlag |= FAULT_FLAG;
                HEAT_OFF();
                ErrCode = 6;
                DisErrState();
            }
            else {
                /*电源恢复后自动恢复*/
                if (ErrCode == 6){
                    //                         ErrCode = 0;
                    //                         State = FSM_IDLE;
                    //                         RunStateFlag = 0;
                }
            }
            if ((RecvBuf[2] & 0x20) != 0){
                /*直流继电器断开控制错误*/
                //                     State = FSM_ERR;
                //                     RunStateFlag |= FAULT_FLAG;
                //                     HEAT_OFF();
                //                     ErrCode = 8;
            }
            if ((RecvBuf[2] & 0x40) != 0){
                /*干烧错误*/
                RunState = FSM_ERR;
                RunStateFlag |= FAULT_FLAG;
                HEAT_OFF();
                ErrCode = 2;
                DisErrState();
            }
            if ((RecvBuf[2] & 0x80) != 0){
                /*光伏正负极接反*/
                RunState = FSM_ERR;
                RunStateFlag |= FAULT_FLAG;
                HEAT_OFF();
                ErrCode = 9;
                DisErrState();
            }
            else {
                /*电源恢复后自动恢复*/
                if (ErrCode == 9){
                    ErrCode = 0;
                    RunState = FSM_IDLE;
                    RunStateFlag = 0;
                }
            }
        }
        RecvLen = 0;
    }

   
}

void KeyDetectProc(){
    cur_key = (u8)gWrCurKey;
    //cur_key &= KEY_ALL;
    /*LED灯显示处理*/
    if((last_data_buf != 0) && (cur_key==0x00))
    {
        /*有效按键检测*/
        switch (last_data_buf)
        {
        case KEY_UP:
            if (key_down == 0){
                key_valid = KEY_VALUE_UP;
            }
            break;
        case KEY_DOWN:
            if (key_down == 0){
                key_valid = KEY_VALUE_DOWN;
            }
            break;
        case KEY_MODE:
            key_valid = KEY_VALUE_MODE;
            break;
        case KEY_SWTICH:
            key_valid = KEY_VALUE_SWTICH;
            break;
        default:
            break;
        }

        key_tick = tick;
        key_down = 0;
    }
    else if((last_data_buf != 0) && (last_data_buf==cur_key)){
        if ((key_down == 0)&&(tick - key_tick) > TICKS_ONE_SECOND){
            /*大于1秒则认为开始长按*/
            key_down = 1;
        }

        if (key_down == 1){
            if ((tick - key_tick) > (TICKS_ONE_SECOND/4)){ //每秒响应3次按键
                key_tick = tick;
                switch (last_data_buf)
                {
                case KEY_UP:
                    key_valid = KEY_VALUE_UP;
                    break;
                case KEY_DOWN:
                    key_valid = KEY_VALUE_DOWN;
                    break;
                default:
                    break;
                }
            }
        }
    }
    else {
        key_tick = tick;
        key_down = 0;
    }

    if (cur_key == (KEY_UP | KEY_DOWN)){
        if ((tick - key_tick_long) >= (u16)(TICKS_ONE_SECOND*3)){
            if (long_press == 0){
                long_press++;
                key_valid = KEY_VALUE_HT;//高温杀菌模式
            }
        }
    }
    else {
        long_press = 0;
        key_tick_long = tick;
    }

    last_data_buf = cur_key;
    /*蜂鸣器*/
}

void main()
{
    SystemInit();
    USER_PROGRAM_INITIAL(); //初始化全局变量
    //sm1616_data_0[0] = BCD_decode_tab[8];
    //sm1616_data_0[1] = BCD_decode_tab[6];
    /// display(sm1616_data_0); 
    // EEPROM_ByteWrite(0, 0xa8);
    // tick_s = EEPROM_ByteRead(0);

    while (1){
        if (tick_s != (u8)tick){
            tick_s = tick;
            ScanTKPress();
        }
        FEED_WDT();
        //在扫描按键间隙处理其他事务
#if 0
        KeyValue = gWrCurKey;
        if (SendLen <= SentLen)
        {   
            if (tick < (time_s+TICKS_ONE_SECOND/4)){
                continue;
            }
            time_s = tick;
            SentLen=0;
            SendBuf[0] = 0xa0;
            SendBuf[1] = KeyValue>>8;
            SendBuf[2] = (u8)KeyValue;
            SendBuf[3] = 0x50;
            SendBuf[4] = 0x51;
            SendBuf[5] = 0x55;

            uart0_send(5);
        }
#else 
        KeyDetectProc();
        USER_PROGRAM();
#endif
    }
#ifdef Enable_uart_test
    Uart_Test();
#endif
#ifdef Enable_sysclk_test

    Sysclk_Test();

#endif
#ifdef Timer_test
    Timer_Test();
#endif

#ifdef PCA0_Test
    Pca0_Test();
#endif
#ifdef LCD_TEST
    Lcd_Test();

#endif
#ifdef LED_TEST
    Led_Test();
#endif
#ifdef TWI_TEST
    Twi_Test();
#endif
#ifdef SPI_TEST
    Spi_Test();
#endif
#ifdef ADC_TEST
    Adc_Test();

#endif
#ifdef EEPROM_TEST
    //Eeprom_Test();
#endif
#ifdef PWM_TEST
    PWM_Test();
#endif
#ifdef LPD_TEST
    LPD_Test();
#endif
#ifdef CRC_TEST
    CRC_Test();
#endif

    while(1);
}