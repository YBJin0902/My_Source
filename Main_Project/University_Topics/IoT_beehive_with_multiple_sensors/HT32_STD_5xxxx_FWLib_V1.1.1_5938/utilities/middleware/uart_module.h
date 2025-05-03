/*********************************************************************************************************//**
 * @file    uart_module.h
 * @version $Rev:: 564          $
 * @date    $Date:: 2022-05-26 #$
 * @brief   The header file of UART Module (interrupt mode with buffer).
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/
// <<< Use Configuration Wizard in Context Menu >>>

/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __UART_MODULE_H
#define __UART_MODULE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Settings ------------------------------------------------------------------------------------------------*/
//<q0> Enable Debug mode
// <i> Enable Debug mode which output debug message to retarget.
#define UART_MODULE_DEBUG_MODE                (0)

#ifndef HTCFG_UARTM_CH0
  /* !!! NOTICE !!!
     Default Setting which will be override by configuration outside (such as "ht32_board_config.h"),
     shall be modified according to the device you are using.
  */
  #define HTCFG_UARTM_CH0                 USART0
  #define HTCFG_UARTM0_TX_GPIO_PORT       A
  #define HTCFG_UARTM0_TX_GPIO_PIN        4
  #define HTCFG_UARTM0_RX_GPIO_PORT       A
  #define HTCFG_UARTM0_RX_GPIO_PIN        5
  #define HTCFG_UARTM0_TX_BUFFER_SIZE     128
  #define HTCFG_UARTM0_RX_BUFFER_SIZE     128
#endif

#ifndef UART_MODULE_FIFO_LEVEL
#define UART_MODULE_FIFO_LEVEL                (USART_RXTL_06)
#endif

/* Exported constants --------------------------------------------------------------------------------------*/
#define   UARTM_CH0                           (0)
#define   UARTM_CH1                           (1)
#define   UARTM_CH2                           (2)
#define   UARTM_CH3                           (3)

#define UARTM_CH0_IRQHandler                  STRCAT2(HTCFG_UARTM_CH0, _IRQHandler)
#ifdef HTCFG_UARTM_CH1
#define UARTM_CH1_IRQHandler                  STRCAT2(HTCFG_UARTM_CH1, _IRQHandler)
#endif
#ifdef HTCFG_UARTM_CH2
#define UARTM_CH2_IRQHandler                  STRCAT2(HTCFG_UARTM_CH2, _IRQHandler)
#endif
#ifdef HTCFG_UARTM_CH3
#define UARTM_CH3_IRQHandler                  STRCAT2(HTCFG_UARTM_CH3, _IRQHandler)
#endif

/* Exported functions --------------------------------------------------------------------------------------*/
void UARTM_Init(u32 CH, USART_InitTypeDef *pUART_Init, u32 uRxTimeOutValue);

u32 UARTM_WriteByte(u32 CH, u8 uData);
u32 UARTM_Write(u32 CH, u8 *pBuffer, u32 uLength);

u32 UARTM_ReadByte(u32 CH, u8 *pData);
u32 UARTM_Read(u32 CH, u8 *pBuffer, u32 uLength);

u32 UARTM_GetReadBufferLength(u32 CH);
u32 UARTM_GetWriteBufferLength(u32 CH);

u8 UARTM_IsTxFinished(u32 CH);

void UARTM_DiscardReadBuffer(u32 CH);

#ifdef __cplusplus
}
#endif

#endif
