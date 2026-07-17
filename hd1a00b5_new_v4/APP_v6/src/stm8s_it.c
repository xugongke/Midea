/**
  ******************************************************************************
  * @file     stm8s_it.c
  * @author   MCD Application Team
  * @version  V2.2.0
  * @date     30-September-2014
  * @brief    Main Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8s_it.h"
#include "gpio_cfg.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/

#ifdef _COSMIC_
/**
  * @brief  Dummy interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(NonHandledInterrupt, 25)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*_COSMIC_*/

/**
  * @brief  TRAP interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER_TRAP(TRAP_IRQHandler)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief  Top Level Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TLI_IRQHandler, 0)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief  Auto Wake Up Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(AWU_IRQHandler, 1)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief  Clock Controller Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(CLK_IRQHandler, 2)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief  External Interrupt PORTA Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTA_IRQHandler, 3)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief  External Interrupt PORTB Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief  External Interrupt PORTC Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief  External Interrupt PORTD Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief  External Interrupt PORTE Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#ifdef STM8S903
/**
  * @brief  External Interrupt PORTF Interrupt routine
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(EXTI_PORTF_IRQHandler, 8)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*STM8S903*/

#if defined (STM8S208) || defined (STM8AF52Ax)
/**
  * @brief CAN RX Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(CAN_RX_IRQHandler, 8)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @brief  CAN TX Interrupt routine
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(CAN_TX_IRQHandler, 9)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*STM8S208 || STM8AF52Ax */

/**
  * @brief  SPI Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(SPI_IRQHandler, 10)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
 u32 tick_10ms;
 u32 sec_time;
/**
  * @brief  Timer1 Update/Overflow/Trigger/Break Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    if(TIM1_GetITStatus(TIM1_IT_UPDATE) == SET )
    {
        TIM1_ClearFlag(TIM1_FLAG_UPDATE);   //清除中断标志
        tick_10ms++;        //对毫秒数记录
        if((tick_10ms % ONE_SECOND_TICK) == 0){
           sec_time++;
        }
    }
}

/**
  * @brief  Timer1 Capture/Compare Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM1_CAP_COM_IRQHandler, 12)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#ifdef STM8S903
/**
  * @brief  Timer5 Update/Overflow/Break/Trigger Interrupt routine
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM5_UPD_OVF_BRK_TRG_IRQHandler, 13)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
/**
  * @brief  Timer5 Capture/Compare Interrupt routine
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM5_CAP_COM_IRQHandler, 14)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#else /*STM8S208, STM8S207, STM8S105 or STM8S103 or STM8AF62Ax or STM8AF52Ax or STM8AF626x */
/**
  * @brief  Timer2 Update/Overflow/Break Interrupt routine
  * @param  None
  * @retval None
  */
 u32 tick_count;

 extern u8 adcflag;
 INTERRUPT_HANDLER(TIM2_UPD_OVF_BRK_IRQHandler, 13)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
     if(TIM2_GetITStatus(TIM2_IT_UPDATE) == SET )
     {
       TIM2_ClearFlag(TIM2_FLAG_UPDATE);   //清除中断标志
       tick_count++;        //对毫秒数记录
       //if(tick_count % 20 == 0)
       {
      //   ms_count = 0;
       //  GPIO_WriteReverse(GPIOC , GPIO_PIN_2);
       }
     }
 }

/**
  * @brief  Timer2 Capture/Compare Interrupt routine
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM2_CAP_COM_IRQHandler, 14)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*STM8S903*/

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S105) || \
    defined(STM8S005) ||  defined (STM8AF62Ax) || defined (STM8AF52Ax) || defined (STM8AF626x)
/**
  * @brief Timer3 Update/Overflow/Break Interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    
     //if(TIM3_GetITStatus(TIM3_IT_UPDATE) == SET )
     {

     }
     //else
     {
         /**/
     }
 }

/**
  * @brief  Timer3 Capture/Compare Interrupt routine
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM3_CAP_COM_IRQHandler, 16)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#endif /*STM8S208, STM8S207 or STM8S105 or STM8AF62Ax or STM8AF52Ax or STM8AF626x */

#if defined (STM8S208) || defined(STM8S207) || defined(STM8S007) || defined(STM8S103) || \
    defined(STM8S003) ||  defined (STM8AF62Ax) || defined (STM8AF52Ax) || defined (STM8S903)
#define RX_DISABLE UART1->CR2 &= (uint8_t)(~UART1_CR2_REN);  
#define RX_ENABLE  UART1->CR2 |= (uint8_t)UART1_CR2_REN 

 extern u8 UartRecvBuf[8]; /*UART接收缓冲区*/
 extern u8 UartRecvLen;

 extern u16 UartRecvTime; //uart最好一个字节接收时间 单位: us
 extern u8 UartSendBuf[8]; /*UART接收缓冲区*/
 extern u8 UartSendLen; //要发送的数据长度
 extern u8 UartSendOff; //发送的偏移值
 u8 tmp_rd;
 u16 GetTimeus(void);
/**
  * @brief  UART1 TX Interrupt routine
  * @param  None
  * @retval None
  */
 u8 uart_sr;
 INTERRUPT_HANDLER(UART1_TX_IRQHandler, 17)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    uart_sr = UART1->SR;
      if ((uart_sr & UART1_FLAG_TC)!= 0) {
        if (UartSendOff == UartSendLen)
        {
            RX_ENABLE;
            UART1_ITConfig(UART1_IT_RXNE, ENABLE);  //使能接收中断
            UART1_ITConfig(UART1_IT_TC, DISABLE); // 发送完成中断
            UART1_RX_PIN;
            /*清除溢出标记*/
            if ((UART1_GetFlagStatus(UART1_FLAG_OR) == SET))
            {
                tmp_rd = ((uint8_t)UART1->DR);
            }
        }
        else {
      
            if (UartSendOff < UartSendLen)
            {
                (UART1->DR) = UartSendBuf[UartSendOff];
                UartSendOff++;
            }
        }
    }
    
}

 u8 CheckValue;
/**
  * @brief  UART1 RX Interrupt routine
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART1_RX_IRQHandler, 18)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
    u8 i;
    /*清除溢出标记*/
    uart_sr = UART1->SR;
    if ((uart_sr&UART1_FLAG_OR) != 0)
    {
        tmp_rd = ((uint8_t)UART1->DR);
        return;
    }
    if ((uart_sr &UART1_FLAG_RXNE) != 0)
    {
        UartRecvTime = GetTimeus();
        UartRecvBuf[UartRecvLen] = ((uint8_t)UART1->DR);
        if (UartRecvLen == 4){
#if 0
            memcpy(UartSendBuf, UartRecvBuf, UartRecvLen+1);
            UartSendLen = UartRecvLen+1;
            UartRecvLen = 0;
            UartSendLen = 5;
            UartSendOff = 1;
            UART1_TX_PIN;
            RX_DISABLE;
            UART1_SendData8(UartSendBuf[0]);
            UART1_ITConfig(UART1_IT_RXNE, DISABLE);  //禁能能接收中断
            UART1_ITConfig(UART1_IT_TC, ENABLE); // 发送完成中断
#else
            CheckValue = 0;
            for (i = 0; i < UartRecvLen; i++){
                CheckValue += UartRecvBuf[i];
            }
            CheckValue = ~CheckValue;
            if (CheckValue == UartRecvBuf[4]){
                CommTimeout = 0;
                if ((s8)(UartRecvBuf[0]) >= 30){
                    ACTempHigh = (s8)(UartRecvBuf[0]); //交流温度阈值
                    MCtrl = UartRecvBuf[2];
                }
                if ((s8)(UartRecvBuf[1]) >= 30){
                    TempHigh = (s8)(UartRecvBuf[1]); //直流温度阈值
                    TempLow = TempHigh - 3; //直流温度阈值
                }
                UartSendBuf[0] = ~((u8)Temperature);//ACTempHigh; 当前温度备份
                UartSendBuf[1] = TempHigh;
                UartSendBuf[2] = CurState;
                UartSendBuf[3] = (u8)Temperature;
                CheckValue = 0;
                for (i = 0; i < 4; i++)
                {
                    CheckValue += UartSendBuf[i];
                }
                CheckValue = ~CheckValue;
                UartSendBuf[4] = CheckValue;
                UartSendLen = 5;
                UartSendOff = 1;
                UART1_TX_PIN;
                RX_DISABLE;
                UART1_SendData8(UartSendBuf[0]);
                UART1_ITConfig(UART1_IT_RXNE, DISABLE);  //禁能能接收中断
                UART1_ITConfig(UART1_IT_TC, ENABLE); // 发送完成中断
            }
            UartRecvLen = 0;
#endif
        }
        else {
            UartRecvLen++;
        }
    }
    
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);  //使能接收中断
    UART1_Cmd(ENABLE);//使能
}
#endif /*STM8S105*/

/**
  * @brief  I2C Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(I2C_IRQHandler, 19)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

#if defined(STM8S105) || defined(STM8S005) ||  defined (STM8AF626x)
/**
  * @brief  UART2 TX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART2_TX_IRQHandler, 20)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
 
  }

/**
  * @brief  UART2 RX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART2_RX_IRQHandler, 21)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
  }
#endif /* STM8S105*/

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
/**
  * @brief  UART3 TX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART3_TX_IRQHandler, 20)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
  }

/**
  * @brief  UART3 RX interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(UART3_RX_IRQHandler, 21)
{
    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
  }
#endif /*STM8S208 or STM8S207 or STM8AF52Ax or STM8AF62Ax */

#if defined(STM8S207) || defined(STM8S007) || defined(STM8S208) || defined (STM8AF52Ax) || defined (STM8AF62Ax)
/**
  * @brief  ADC2 interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(ADC2_IRQHandler, 22)
{

    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    return;

}
#else /*STM8S105, STM8S103 or STM8S903 or STM8AF626x */
/**
  * @brief  ADC1 interrupt routine.
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(ADC1_IRQHandler, 22)
{

    /* In order to detect unexpected events during development,
       it is recommended to set a breakpoint on the following instruction.
    */
    return;

}
#endif /*STM8S208 or STM8S207 or STM8AF52Ax or STM8AF62Ax */

#ifdef STM8S903
/**
  * @brief  Timer6 Update/Overflow/Trigger Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(TIM6_UPD_OVF_TRG_IRQHandler, 23)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}
#else /*STM8S208, STM8S207, STM8S105 or STM8S103 or STM8AF62Ax or STM8AF52Ax or STM8AF626x */
/**
  * @brief  Timer4 Update/Overflow Interrupt routine
  * @param  None
  * @retval None
  */
 INTERRUPT_HANDLER(TIM4_UPD_OVF_IRQHandler, 23)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.

  */
    if(TIM4_GetITStatus(TIM4_IT_UPDATE) == SET )
    {
        TIM4_ClearFlag(TIM4_FLAG_UPDATE);   //清除中断标志
       // tick_10ms++;        //对毫秒数记录
        //if(tick_count % 20 == 0)
        {
            //   ms_count = 0;
            //  GPIO_WriteReverse(GPIOC , GPIO_PIN_2);
        }
    }
}
#endif /*STM8S903*/

/**
  * @brief  Eeprom EEC Interrupt routine
  * @param  None
  * @retval None
  */
INTERRUPT_HANDLER(EEPROM_EEC_IRQHandler, 24)
{
  /* In order to detect unexpected events during development,
     it is recommended to set a breakpoint on the following instruction.
  */
}

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
