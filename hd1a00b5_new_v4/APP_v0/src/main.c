#include "stm8s.h"
#include "gpio_cfg.h"
#include "key.h"
#include "para.h"

#define WDG_ENABLE  1
#define SAMPLE_NUMS  5  /**/
u8 SecMos2; /*MOS2测试时间*/

volatile unsigned char CommTimeout; //总线通讯超时计数
u8 MCtrl;       /*主显示板控制:  bit[0]:1:交流加热; 0:交流不加热   bit[1~7]暂时不使用*/
u8 CurState;    /*反馈给主显示板当前状态  
                bit[0]:1:交流加热; 0:交流不加热;
                bit[1]:直流加热状态
                bit[2]:交流电是否存在
                bit[3]:直流电是否存在
                bit[4]:直流继电器闭合控制异常(启动加热) 关闭异常   
                bit[5]:直流继电器断开控制异常(停止加热) 关闭异常
                bit[6]:1:光伏存在,但是直流DCDC输出12v电压不存在
                bit[7]:干烧错误 发生后, 需要断电才能恢复  复用做光伏电源正负接反指示
                */
s8 ACTempHigh=60; /*交流温度阈值上限*/
u8 mos2_start; //第2路MOS是否启动加热  1:启动  0:未启动
u8 VolStart;    /*启动电压*/
u8 VolThres;    /*光伏板电压阈值*/
u8 VolThresHigh;/*光伏板电压阈值*/

u8 VOLChoose;   /*当前光伏板的种类72V或36V*/
u8 InputVol;    /*当前光伏板实际电压值*/ 
u8 InputVol2;   //测试接反电压值
s8 TempHigh = 75; /*温度高阈值, 高于该阈值时停止加热*/
s8 TempLow = 65;  /*温度低阈值, 低于该阈值时启动加热*/

s8 Temperature; /*2个50KNTC的温度值, 分压电阻采用10K 单位:℃*/

#define ERR_TEMP  (s8)-19

#define VOL_72V 1
#define VOL_36V 0
void UartSendTest(void);
u8 StartHeater(void);
u8 VOLChoose_mac = VOL_72V; /*电压选择: 0:36V光伏: 1:72V光伏*/
/*停止加热*/
//u8 1:成功  0:失败
u8 StopHeater(void);
#if WDG_ENABLE
/*******************************************************************************
**函数名称：void IWDG_Init()
**功能描述：初始化独立看门狗
**入口参数：无
**输出：无
*******************************************************************************/
void IWDG_Init()
{
    IWDG_SetReload(0xff);      //重载寄存器写入255
    //先写键值 ，先写0XCC , 后写0X55
    IWDG_Enable();
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    IWDG_SetPrescaler(IWDG_Prescaler_256);   //64KHZ / 256 = 1.024S
}

/*******************************************************************************
**函数名称：void FeedTheIWDog(void)
**功能描述：向独立看门狗喂狗，防止系统复位
**入口参数：无
**输出：无
*******************************************************************************/
void FeedTheIWDog()
{
    IWDG_ReloadCounter();
}
#endif

u8 FSM;     /*状态机变量, 用于指示当前系统状态*/

u32 StateStartTime;  /*状态切换开始的时间 单位:ms*/

/*******************************************************************************
**函数名称：void TIM2_Init()
**功能描述：定时器2参数初始化
**入口参数：无
**输出：无
*******************************************************************************/
void TIM2_Init()
{
    TIM2_TimeBaseInit(TIM2_PRESCALER_16 ,    //定时器2预分频数为 16 分频，即定时器时钟 = 系统时钟/16 = 1MHz
        65535/*50000-1*/        //设置50毫秒时间自动重载 1000
        );
    TIM2_ITConfig(TIM2_IT_UPDATE , ENABLE); //使能定时器2更新中断
    TIM2_ARRPreloadConfig(ENABLE);          //允许自动重载
    TIM2_Cmd(ENABLE);        //启动定时器2开始计数
}


extern u16 tick_count;
/*最大10s*/
u32 GetTimeMsU32(void)
{
    u16 us;
    u32 last_tick;
    last_tick = tick_count;
    us = (((uint16_t)TIM2->CNTRH << 8) | (uint16_t)(TIM2->CNTRL));
    while (last_tick != tick_count)
    {
        /*刚刚发生一次定时器溢出*/
        last_tick = tick_count;
        us = (((uint16_t)TIM2->CNTRH << 8) | (uint16_t)(TIM2->CNTRL));
    }

    last_tick <<= 16;
    last_tick = last_tick | (u32)us;
    last_tick = last_tick / 1000; /*转换为ms*/

    return last_tick;
}

u16 GetTimeus(void)
{
    u16 us;
    us = (((uint16_t)TIM2->CNTRH << 8) | (uint16_t)(TIM2->CNTRL));

    return us;
}

void mDelaymS(u32 ms )// 延时毫秒
{
    u32 i;
    i = GetTimeMsU32();
    while ((GetTimeMsU32() - i) < ms) {
     //UartSendTest();
#if WDG_ENABLE
        FeedTheIWDog();
#endif
    }
}


u32 SamplesAdd;
/*******************************************************************************
**函数名称：void Get_ADC_AnyChannal_Value(u16 *AD_Value , u16 SamplesTimes)
**功能描述：读取ADC完成多次模数转换结果
**入口参数：u16 *AD_Value  存放多冷采样的平均值
u16 SamplesTimes  采样次数
**输出：无
*******************************************************************************/
void Get_ADC_AnyChannal_Value(u16 *AD_Value, u16 SamplesTimes)
{
    u32 SamplesAdd , count , i;
    SamplesAdd = 0;

    for(i = 0; i < SamplesTimes; i++ )
    {
        ADC1_Cmd(ENABLE);     //启动ADC
        while(ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET);  //等待转换结束
        ADC1_ClearFlag(ADC1_FLAG_EOC);  //清除转换结束标志
        count = ADC1_GetConversionValue();  //读取转换数据
        SamplesAdd += count;
    }
    *AD_Value = (SamplesAdd / SamplesTimes);
}

/*0:继电器是闭合状态,加热管加热
1: 继电器是断开状态, 直流加热管停止加热
*/
u16 RelayADC;
u16 RelayADC2;
#define STATE_CLOSE       0 /*继电器闭合，加热管加热*/
#define STATE_DISCONNECT  1 /*继电器断开，加热管停止加热*/
/*直流加热继电器状态*/
u8 GetRelayState(void)
{
    u32 cal;
    /*获取当前继电器状态*/
    u16 adc_value;
    /*获取当前电压*/
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_2, ADC1_ALIGN_RIGHT);
    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    /*200K  10K电阻分压*/
    //Vx=ADC*5V/1024  V输入=210K*VX/10K
    //V输入=ADC*5V*21/1024
    cal = adc_value;
    cal = 105*cal;
    cal = cal >> 10;
    RelayADC = cal;

    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_3, ADC1_ALIGN_RIGHT);
    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    /*200K  10K电阻分压*/
    //Vx=ADC*5V/1024  V输入=210K*VX/10K
    //V输入=ADC*5V*21/1024
    cal = adc_value;
    cal = 105*cal;
    cal = cal >> 10;
    RelayADC2 = cal;  //光耦独立驱动加热管

    if (RelayADC < 6) {
        /*继电器是闭合状态*/
        return STATE_CLOSE;
    }
    else {
        return STATE_DISCONNECT; /*继电器是断开状态, 加热管停止加热*/
    }
}
u8 AcVol;
/***************************************************************************************************
*\Function      GetACDectect
*\Description   检测交流电是否存在
*\Parameter     
*\Return        u8
*\Note          
*\Log           2020.05.11    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
void GetACDectect(void)
{
#if 0
    u32 cal;
    u16 adc_value;
    /*获取当前电压*/
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_6, ADC1_ALIGN_RIGHT);

    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    /*200K  10K电阻分压*/
    //Vx=ADC*5V/1024  V输入=210K*VX/10K
    //V输入=ADC*5V*21/1024
    cal = adc_value;
    cal = 105*cal;
    cal = cal >> 10;
    AcVol = cal;
#else 
    AcVol = 0;
    if (AC_DECTECT_ST != 0){
        AcVol = 21; //交流电存在
    }
#endif
    if (AcVol > 20){
        /*交流电存在*/
        CurState |= 0x04; 
    }
    else {
        /*交流电不存在*/
        CurState &= ~0x04;
        MCtrl &= ~0x01;
    }

    if (((MCtrl & 0x1) != 0) && (Temperature < ACTempHigh) && (CommTimeout < 15)){ /*交流加热 */
        /*启动交流加热*/
        CurState |= 0x01;
        AC_RELAY_ON;
    }
    else {
        CurState &= ~0x01; //交流加热指示
        MCtrl &= ~0x01;
        AC_RELAY_OFF;
    } 

    if ((CurState & 0x08) != 0){ // 直流电压
        if ((GetRelayState() == STATE_CLOSE) /*&& (Temperature < TempHigh)*/){
            CurState |= 0x02;//直流加热
        }
        else {
            CurState &= ~0x02;
        }
    }
    else {
        /*直流电压不存在*/
        if (ResetRelayFlag != 0){
            StartHeaterRAW();
        }
       
        CurState &= ~0x02;
    }
}

u8 DCErrcnt;
/***************************************************************************************************
*\Function      GetInputVol
*\Description   光伏电压
*\Parameter     
*\Return        u16 输出电压值
*\Note          
*\Log           2019.03.13    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
u8 GetInputVol(void)
{
    u32 cal;
    u16 adc_value;
    /*获取当前光伏电压*/
    VOL_100V_IN;
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_4, ADC1_ALIGN_RIGHT);

    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    /*200K  10K电阻分压*/
    //Vx=ADC*5V/1024  V输入=210K*VX/10K
    //V输入=ADC*5V*21/1024
    cal = adc_value;
    //cal = 105*cal;
    cal = 110*cal;
    cal = cal >> 10;
    InputVol = cal;
    VOL_100V_TEST; //切换到测试电路

    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);

    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);
    /*200K  10K电阻分压*/
    //Vx=ADC*5V/1024  V输入=210K*VX/10K
    //V输入=ADC*5V*21/1024
    cal = adc_value;
    //cal = 105*cal;
    cal = 110*cal;
    cal = cal >> 10;
    InputVol2 = cal;
    if ((InputVol2+10) < InputVol){
        DCErrcnt++;
        if (DCErrcnt > 4){
           CurState |= 0x80;//电源电压输入过低，则可能电源接反
        }
    }
    else {
        DCErrcnt = 0;
        CurState &= (~0x80);
    }
    if (InputVol < VolStart){
        DCErrcnt = 0;
        /*电压值过低, 直流电不存在*/
        CurState &= ~0x08;
        CurState &= ~0x02; //直流加热不存在
    }
    else { //直流电存在
         CurState |= 0x08;
    }
    /*获取当前光伏电压*/
    VOL_100V_IN;
   
    return InputVol;
}

u8 Retrycnt;
u8 ResetRelayFlag; //是否需要复位继电器状态 1:需要复位
/***************************************************************************************************
*\Function      OpenHeaterRAW
*\Description   启动直流加热功能,不进行任何状态维护
*\Parameter     
*\Return        u8 1:启动成功  0:启动失败
*\Note          1)用于直流电源不正常时复位继电器状态
*\Log           2019.02.26    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
u8 StartHeaterRAW(void)
{
    if (ResetRelayFlag == 0){
        return 0;
    }
    if ((CurState & 0x30) != 0) /*继电器控制异常则不进行相关的控制*/{
        return 0;
    }
    ResetRelayFlag = 0;
    /*首先短接MOS管, 降低继电器两端电压*/
    RELAY_CTRL2_START();
    //MOS_CTRL2_START();
    mDelaymS(100);
    /*继电器复位常闭状态启动加热*/
    RELAY_CLOSE();/*复位继电器*/
    mDelaymS(100*1);    /*继电器断开大约需要100ms @todo 实际产品中再增加一倍时间， 防止产品老化后反应时间不足*/
    //RELAY_CTRL2_STOP(); /*关闭MOS管*/
  //  MOS_CTRL2_STOP();
    RELAY_IDLE();
    
    return 0;
}

/***************************************************************************************************
*\Function      OpenHeater
*\Description   启动直流加热功能
*\Parameter     
*\Return        u8 1:启动成功  0:启动失败
*\Note          
*\Log           2019.02.26    Ver 1.0    骆兆松
*               创建函数。
***************************************************************************************************/
u8 StartHeater(void)
{
    if ((GetRelayState() == STATE_CLOSE) || ((CurState & 0x08) == 0)) {/*继电器闭合状态*/
        DEBUG_LED1_ON();
        CurState &= ~(0x10);//启动加热成功
        return 1; /*操作成功*/
    }
    if ((CurState & 0x30) != 0) /*继电器控制异常则不进行相关的控制*/{
        if ((CurState & 0x08) != 0){ // 直流电压
            StopHeater();
        }
        return 1;
    }
    ResetRelayFlag = 0;
    CurState |= 0x02;
    /*首先短接MOS管, 降低继电器两端电压*/
    RELAY_CTRL2_START();
    //MOS_CTRL2_START();
    mDelaymS(100);
    Retrycnt = 0;
    while (Retrycnt < 50)
    {
        if (GetRelayState() == STATE_CLOSE) {
            break;
        }
        Retrycnt++;
        mDelaymS(10);
    }
    /*继电器复位常闭状态启动加热*/
    RELAY_CLOSE();/*复位继电器*/
    mDelaymS(150*1);    /*继电器断开大约需要100ms @todo 实际产品中再增加一倍时间， 防止产品老化后反应时间不足*/
    //RELAY_CTRL2_STOP(); /*关闭MOS管*/
   // MOS_CTRL2_STOP();
    RELAY_IDLE();

    mDelaymS(400);
    if (GetRelayState() == STATE_CLOSE) {/*继电器闭合状态*/
        DEBUG_LED1_ON();
        CurState &= ~(0x10);//启动加热成功
        return 1; /*操作成功*/
    }
    else {
        /*继电器操作失败*/
        //CurState |= (0x10);//启动加热失败
        return 0;
    }
}

/*停止加热*/
//u8 1:成功  0:失败
u8 StopHeater(void)
{
    Retrycnt = 0;
    ResetRelayFlag = 1; //标记需要复位继电器状态
    if ((GetRelayState() == STATE_DISCONNECT) || ((CurState & 0x08) == 0) || (GF_12V_DECTECT_PIN == 0)) {/*继电器断开状态*/
        DEBUG_LED1_OFF();
        CurState &= ~(0x20);//停止加热成功
        return 1; /*操作成功*/
    }
    CurState &= ~0x02;
    /*首先短接MOS管, 对继电器分流*/
    RELAY_CTRL2_START();
    //MOS_CTRL2_START();
    mDelaymS(100);
    while (Retrycnt < 50)
    {
        if (GetRelayState() == STATE_CLOSE){
            break;
        }
        Retrycnt++;
        mDelaymS(10);
    }
    RELAY_DISCONNECT();/*断开继电器*/
    mDelaymS(150*1);      /*继电器断开大约需要100ms @todo 实际产品中再增加一倍时间， 防止产品老化后反应时间不足*/
    RELAY_CTRL2_STOP(); /*关闭MOS管*/
    MOS_CTRL2_STOP();
    RELAY_IDLE();
    mDelaymS(400);
    if (GetRelayState() == STATE_DISCONNECT) {/*继电器断开状态*/
        DEBUG_LED1_OFF();
        CurState &= ~(0x20);//停止加热成功
        return 1; /*操作成功*/
    }
    else {
        InputVol = GetInputVol();InputVol = GetInputVol();
        /*停止加热报警*/
        if (((CurState & 0x08) == 0) || (GF_12V_DECTECT_PIN == 0)){ 
            CurState &= ~(0x20);//停止加热成功
            return 1; /*操作成功*/ 
        }
        else {
            //CurState |= (0x20);//停止加热失败
            return 0;
        }
    }
}

//显示缓存
//u8 number[8];
//u8 adcflag;
//u16 ADCData1;

//u32 vol;
//u32 us_time;
//u32 us_time_last;


//u16 ms_count , ADC_count;
//u8 SMGtimes , SMGBit;

//u32 KeyDetectTime;      /*上次按键检测时间*/
//u32 DisplayFlushTime;   //上次显示刷新时间
//u8 last_ctrl_key;            //控制按钮
//u8 ctrl_key;            //控制按钮
//u8 key_valid;           //用于标记按键检测是否有效  0:表示无效; 1表示有效
//u8 cfg_key;             //参数配置按钮


u16 AdcValue[4];

#define ADC_TOTAL_NUM 4

/*******************************************************************************
**函数名称：void ADC_SCAN_Init()
**功能描述：初始化ADC,单次扫描模式，一次扫描所有的6个通道
**入口参数：无
**输出：无
*******************************************************************************/
void ADC_SCAN_Init(void)
{
    //设置PB->6 为输入 悬空输入  中断禁止
    GPIO_Init(GPIOC , GPIO_PIN_4 , GPIO_MODE_IN_FL_NO_IT); //AN2
    GPIO_Init(GPIOD , GPIO_PIN_2 , GPIO_MODE_IN_FL_NO_IT); //AN3
    GPIO_Init(GPIOD , GPIO_PIN_3 , GPIO_MODE_IN_FL_NO_IT); //AN4
//     GPIO_Init(GPIOD , GPIO_PIN_5 , GPIO_MODE_IN_FL_NO_IT); //AN5
     GPIO_Init(GPIOD , GPIO_PIN_6 , GPIO_MODE_IN_FL_NO_IT); //AN6

    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE ,  //单次转换模式
        ADC1_CHANNEL_4,                 //配置为通道 10 采样
        ADC1_PRESSEL_FCPU_D10,              //采样速率为16M / 18
        ADC1_EXTTRIG_TIM ,                  //外部触发
        DISABLE ,                           //不使能外部触发
        ADC1_ALIGN_RIGHT ,                  //采样数据右对齐
        ADC1_SCHMITTTRIG_ALL ,              //施密特
        ENABLE                             //不使能施密特
        );
     ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_ALL, ENABLE);
    ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNEL2, DISABLE);
    ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNEL3, DISABLE);
    ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNEL4, DISABLE);
    ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNEL6, DISABLE);

    ADC1_ScanModeCmd(ENABLE);     //使能SCAN扫描模式
    ADC1_Cmd(ENABLE);             //启动ADC
}

extern const u16 TempTable[141];
/*获取水箱温度*/
s8 TempLast;
u8 temp_filter_cnt;
u8 OverErr;
u8 ErrOverCycle;
#define  ERR_RETRY_CNT  1
s8 GetTemperature(void)
{
    s16 i;
    s16 high = 100;
    s16 low = 0;
    u16 adc_value;
    /*获取当前温度*/
#if WDG_ENABLE
    FeedTheIWDog();
#endif
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_6, ADC1_ALIGN_RIGHT);
    Get_ADC_AnyChannal_Value(&adc_value, SAMPLE_NUMS);

    if (adc_value < (TempTable[140])){ /*电阻值过低，温度过高, 可能传感器短路，返回温度120*/
        OverErr++;
        if (OverErr > ERR_RETRY_CNT){
            OverErr = 0;
            ErrOverCycle++;
        }
        if (ErrOverCycle > 1){
            Temperature = 120;
        }
        return Temperature;
    }
    if (adc_value > TempTable[0]) /*电阻值过高, 温度过低, 可能传感器断路, 返回温度: -20*/
    {
        OverErr++;
        if (OverErr > ERR_RETRY_CNT){
            ErrOverCycle++;
            OverErr = 0;
        }
        if (ErrOverCycle > 1){
            Temperature = -20;
        }
        return Temperature;
    }
    OverErr=0;
    ErrOverCycle = 0;
    i = 50;
    high = 140;
    low  = 0;
    while (high > (low+2)){
        if (adc_value < TempTable[i]){
            low = i;
        }
        else if (adc_value == TempTable[i]){
            break;
        }
        else {
            high = i;
        }
        i = (high+low)/2;
    }
  
    i = i - 20;/*当前实际温度*/
    //i++;//补偿一度
    if (TempLast == 0){
        TempLast = i;
        Temperature = i;
    }

    temp_filter_cnt++;
#if 1
    i = (i + TempLast)/2;
    //i = (i + Temperature)/2;
    if (temp_filter_cnt > 2)
    {
        Temperature = i;//+1;
        temp_filter_cnt = 0;
    }
    else {
        TempLast = i;
    }
#else
    Temperature = i;
#endif
    
    return Temperature; /*暂时使用第2个NTC作为温度参考*/
}

u8 VOLChooseGet(void)
{
    //if (GPIO_READ_INPUT_PIN(VOL_CHOOSE_PORT, VOL_CHOOSE_PIN)) {
    if (VOLChoose_mac == VOL_72V) {
        /*为1表示选择72V*/
        VOLChoose = 72;
        VolThres = 26;
        VolStart = 25;
        VolThresHigh = 28;
    }
    else {
        /*为0选择36V*/
        VOLChoose = 36;
        VolThres = 18;
        VolStart = 16;
        VolThresHigh = 21;
    }

    return 0;
}

void DelaySecond(u16 sec)
{
    if (sec == 0) {
        return;
    }
    while (sec--)
    {
        InputVol = GetInputVol();
        GetTemperature();
        if ( CommTimeout < 20)
        {
            CommTimeout++;
        }
        GetACDectect();
        mDelaymS(400);
        GetACDectect();
        mDelaymS(500);
        GetACDectect();
        if ((CurState & 0x02) != 0) //直流加热状态
        {
            SecMos2++;
            if (GetRelayState() == STATE_CLOSE) {/*继电器闭合状态*/
                DEBUG_LED1_ON();
                CurState &= ~(0x10);//启动加热成功
#if 0
                /*测试代码*/
                 SecMos2 %= 10;
                if (SecMos2 == 0){
                    if (mos2_start == 0){ /*到时间进行一次反转*/
                        MOS_CTRL2_START();
                    }
                    else {
                        MOS_CTRL2_STOP();
                    }
                }
#else 
                SecMos2 %= 240;
                if ((SecMos2 == 0) || (InputVol > (VolHighPower+15))){
                    if ((mos2_start == 0)){
                        if ((InputVol > VolHighPower)){ //高于阈值电压启动第二路MOS加热
                            MOS_CTRL2_START();
                        }
                    }
                }

                if (InputVol < (VolHighPower - 20)){ //电压降低后 关闭第二路加热
                    MOS_CTRL2_STOP();
                    SecMos2 = 0;
                }
#endif
            }
            else {
                /*继电器操作失败*/
                //CurState |= (0x10);//启动加热失败
            }
        }
        else {
            SecMos2 = 0;
        }
        if ((CurState & 0x10) != 0) /*继电器闭合控制异常*/{
            if ((CurState & 0x08) != 0){ // 直流电压
                StopHeater();
            }
        }
        else  if ((CurState & 0x20) != 0) {/*继电器断开控制异常*/
            //if ((CurState & 0x08) != 0){ // 直流电压
            //    CloseHeater();
            //}
        }
        if ((Temperature < ERR_TEMP) || (Temperature >= TempHigh)){ /*传感器异常或温度过高都停止直流加热*/
            if ((CurState & 0x08) != 0){ // 直流电压
                StopHeater();
            }
            CurState &= ~0x01; //交流加热指示
            MCtrl &= ~0x01;
            AC_RELAY_OFF;//停止交流加热
        }
    }
}
#if 0
void TestFunc(void)
{
    while (1)
    {
        OpenHeater();
        DelaySecond(2);
        CloseHeater();
        DelaySecond(2);
    }
}

/*可靠性测试*/
void ReliabilityTest(void)
{
    while (1) {
       GetTemperature();
        OpenHeater();
        mDelaymS(2200);
        CloseHeater();
        mDelaymS(2200);
    }
}

void ReliabilityTestLong1(void)
{
    while (1) {
        CloseHeater();
        DelaySecond(1*60);
        if (GetTemperature() < 90) {
            OpenHeater();
        }
        DelaySecond(20*60);

    }
}
void ReliabilityTestLong(void)
{
    while (1) {
        CloseHeater();
        DelaySecond(10*60);
        if (GetTemperature() < 75) {
            OpenHeater();  /*高于75℃以后不允许加热*/
        }
        DelaySecond(15*60);
    }
}

void MosTest(void)
{
    while (1) {
        /*首先短接MOS管, 降低继电器两端电压*/
        DEBUG_LED1_ON();
        RELAY_CTRL2_START();
        DelaySecond(2);
        DEBUG_LED1_OFF();
        RELAY_CTRL2_STOP(); /*关闭MOS管*/
        DelaySecond(3);
    }
}
#endif
u8 UartRecvBuf[8]; /*UART接收缓冲区*/
u8 UartRecvLen;
u16 UartRecvTime; //uart最好一个字节接收时间 单位: ms
u16 time_sub; //临时变量

u8 SendTest = 0;
u8 UartSendBuf[8]; /*UART接收缓冲区*/
u8 UartSendLen; //要发送的数据长度
u8 UartSendOff; //发送的偏移值
#include "stm8s_uart1.h"

UART1_TypeDef *UartRegs;
void uart1Init(void)
{
    UART1_DeInit();
    UART1_Init(9600, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE); /*@TODO尝试不同模式*/
    UART1_HalfDuplexCmd(ENABLE);//使能单总线半双工模式
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);  //使能接收中断
    UART1_ITConfig(UART1_IT_TC, DISABLE); // 发送完成中断
    UART1_RX_PIN;
    UART1_Cmd(ENABLE);//使能
}

void UartSendTest(void)
{
    time_sub = GetTimeus() - UartRecvTime;
    if ((UartRecvLen != 0) && (time_sub > 8000)){
        UartRecvLen = 0;
    }
    if (UartSendLen == UartSendOff) {
        if (SendTest == 1) {
            UartSendBuf[0] = 0xa1;
            UartSendBuf[1] = 0xa2;
            UartSendBuf[2] = 0xa5;
            UartSendBuf[3] = 0xaa;
            UartSendBuf[4] = 0x5a;
            UartSendLen = 5;
            UartSendOff = 1;
            UART1_TX_PIN;
            UART1_SendData8(UartSendBuf[0]);
            UART1_ITConfig(UART1_IT_RXNE, DISABLE);  //禁能能接收中断
            UART1_ITConfig(UART1_IT_TC, ENABLE); // 发送完成中断
            // SendTest  = 0;
        }
        else {

        }
    }
}

extern  u8 uart_sr;
u8 AbNormalFlag;
u16 NormalCnt;
/* 主函数 */
int main(void)
{
    u8 i;
    disableInterrupts();     //先禁止系统总中断
    //内部时钟为1分频 = 16Mhz
    CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);
#if WDG_ENABLE
    IWDG_Init();
#endif
    GpioInit(); //初始化所有引脚
    TIM2_Init();
    enableInterrupts(); //使能系统总中断
    
    
    mDelaymS(200);
 //   KeyDetectTime = GetTimeMsU32();
    ADC_SCAN_Init(); //@todo 测试
    DEBUG_LED1_OFF();
    DEBUG_LED1_ON();
    
    VOLChooseGet(); /*获取当前光伏板电压值*/
    UartRegs = UART1;
    FSM = FSM_WAIT_ALL_KEY_UP;
#if WDG_ENABLE
    FeedTheIWDog();
#endif
    //MosTest();
    //TestFunc();
//   ReliabilityTest();
    //ReliabilityTestLong();
    //ReliabilityTestLong1();
 // CloseHeater();
   
    InputVol = GetInputVol();
    InputVol = GetInputVol();
    uart1Init();
    Temperature = 0;
    GetTemperature();
    DelaySecond(3); /*延时10s, 防止电压不稳定状态*/
    //Temperature = 0;
    while (1)
    {
        DelaySecond(1);
        GetTemperature();
        if (GetRelayState() == STATE_DISCONNECT){
            DEBUG_LED1_OFF();
        }
        else {
            DEBUG_LED1_ON();
        }
        InputVol = GetInputVol();
        if (InputVol < VolThres) {
            /*当前电压过低不进行控制*/
            DelaySecond(5);
            InputVol = GetInputVol();
            if (InputVol >= VolThres) {
                continue;  
            }
            do {
                if (GetRelayState() == STATE_DISCONNECT) {
                    /*当前继电器断开, 则闭合*/
                    if ((InputVol > VolStart) && (Temperature < TempHigh) && (Temperature > ERR_TEMP)) {
                        StartHeater();
                    }
                }
                InputVol = GetInputVol();
                if (InputVol < VolThresHigh) {
                    DelaySecond(1*60);
                    InputVol = GetInputVol();
                    if (InputVol < VolThresHigh)
                        continue;
                    else 
                        break;
                }
                else {
                    break;
                }
            } while (1);
            continue;
        }

        if (AbNormalFlag == 1) {
            mDelaymS(500);
            DEBUG_LED1_EVERT();
            NormalCnt++;
            if (NormalCnt >= 60) {
                NormalCnt = 0;
            }
            else 
                continue;
        }
        //GetRelayState();//温度传感器异常 则不进行加热
        if ((GetTemperature() <= TempLow)&&(Temperature > ERR_TEMP)) {
            DelaySecond(60);
            if (GetTemperature() > TempLow) {
                continue;
            }
            /*当前温度低于阈值, 且不处于加热状态, 则启动加热*/
            if (GetRelayState() == STATE_DISCONNECT)
            {
                i = 0;
                do 
                {
                    if ((StartHeater() == 1)) {
                        break;
                    }
                    else {
                        DelaySecond(2*60);
                        i++;
                    }

                } while (i < 3);
                if (i == 3) {
                    AbNormalFlag = 1;
                }
                else {
                    AbNormalFlag = 0;
                }
            }
        }
        else if (GetTemperature() >= TempHigh) 
        {
            /*当前温度高于75℃*/
            if (GetRelayState() == STATE_CLOSE)
            {
                DelaySecond(15);
                i = 0;
                do
                {
                    /*停止加热*/
                    if (StopHeater() == 1) {
                        break;
                    }
                    else {
                        DelaySecond(2*60);
                        i++;
                    }
                } while (i < 3);
                if (i == 3) {
                    AbNormalFlag = 1;
                }
                else {
                    AbNormalFlag = 0;
                }
            }
        }
    }
}
