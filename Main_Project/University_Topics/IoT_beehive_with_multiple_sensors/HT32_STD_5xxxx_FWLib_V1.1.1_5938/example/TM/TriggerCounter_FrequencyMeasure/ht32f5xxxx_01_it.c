/*********************************************************************************************************//**
 * @file    TM/TriggerCounter_FrequencyMeasure/ht32f5xxxx_01_it.c
 * @version $Rev:: 5832         $
 * @date    $Date:: 2022-04-25 #$
 * @brief   This file provides all interrupt service routine.
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

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32.h"
#include "ht32_board.h"
#include "ht32_board_config.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup TM_Examples TM
  * @{
  */

/** @addtogroup TriggerCounter_FrequencyMeasure
  * @{
  */


/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
 * @brief   This function handles NMI exception.
 * @retval  None
 ************************************************************************************************************/
void NMI_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles Hard Fault exception.
 * @retval  None
 ************************************************************************************************************/
void HardFault_Handler(void)
{
  #if 1

  static vu32 gIsContinue = 0;
  /*--------------------------------------------------------------------------------------------------------*/
  /* For development FW, MCU run into the while loop when the hardfault occurred.                           */
  /* 1. Stack Checking                                                                                      */
  /*    When a hard fault exception occurs, MCU push following register into the stack (main or process     */
  /*    stack). Confirm R13(SP) value in the Register Window and typing it to the Memory Windows, you can   */
  /*    check following register, especially the PC value (indicate the last instruction before hard fault).*/
  /*    SP + 0x00    0x04    0x08    0x0C    0x10    0x14    0x18    0x1C                                   */
  /*           R0      R1      R2      R3     R12      LR      PC    xPSR                                   */
  while (gIsContinue == 0)
  {
  }
  /* 2. Step Out to Find the Clue                                                                           */
  /*    Change the variable "gIsContinue" to any other value than zero in a Local or Watch Window, then     */
  /*    step out the HardFault_Handler to reach the first instruction after the instruction which caused    */
  /*    the hard fault.                                                                                     */
  /*--------------------------------------------------------------------------------------------------------*/

  #else

  /*--------------------------------------------------------------------------------------------------------*/
  /* For production FW, you shall consider to reboot the system when hardfault occurred.                    */
  /*--------------------------------------------------------------------------------------------------------*/
  NVIC_SystemReset();

  #endif
}

/*********************************************************************************************************//**
 * @brief   This function handles SVCall exception.
 * @retval  None
 ************************************************************************************************************/
void SVC_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles PendSVC exception.
 * @retval  None
 ************************************************************************************************************/
void PendSV_Handler(void)
{
}
 
extern u32 OverflowCounter;
u32 gCAP_Counter;
u32 gCompensate_Counter;
/*********************************************************************************************************//**
 * @brief   This function handles SysTick Handler.
 * @retval  None
 ************************************************************************************************************/
void SysTick_Handler(void)
{
  /* DISABLE TM channel capture timer and 100ms Counter                                                     */
  TM_Cmd(HTCFG_CAP_PORT, DISABLE);
  SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);

  /* Show the Value, OverflowCounter and CAP Timer Counter                                                  */
  printf("==============================\r\n");
  printf("OverflowCounter   = %05d\r\n", (OverflowCounter));
  printf("CAP Timer Counter = %05d\r\n", (TM_GetCounter(HTCFG_CAP_PORT)));

  /* Counter Overflow will being at 0, so OverflowCounter need multiply 65536                               */
  gCAP_Counter = ((OverflowCounter)*65536)+(TM_GetCounter(HTCFG_CAP_PORT));

  /* Every 10K count will be one less, so compensate back                                                   */
  gCompensate_Counter = (gCAP_Counter/10000);

  /* Show the Value, Freq_divd_by_10 and Input Signal Freq                                                  */
  printf("Freq_divd_by_10   = %05d\r\n", (gCAP_Counter+gCompensate_Counter));
  printf("Input Signal Freq = %05d Hz\r\n", (gCAP_Counter+gCompensate_Counter)*10);

  /* Clear related counter value and all interrupt flags                                                    */
  OverflowCounter = 0;
  TM_SetCounter(HTCFG_CAP_PORT, 0);
  TM_ClearIntPendingBit(HTCFG_CAP_PORT, TM_INT_TEV | TM_INT_UEV);

  /* Enable TM Channel Trigger Event interrupt                                                              */
  TM_IntConfig(HTCFG_CAP_PORT, TM_INT_TEV, ENABLE);

  /* ENABLE TM channel capture timer                                                                        */
  TM_Cmd(HTCFG_CAP_PORT, ENABLE);
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
