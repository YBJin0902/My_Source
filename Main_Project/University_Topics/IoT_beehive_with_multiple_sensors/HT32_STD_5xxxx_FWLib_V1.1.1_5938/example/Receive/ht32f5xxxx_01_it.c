/*********************************************************************************************************//**
 * @file    USART/Interrupt/ht32f5xxxx_01_it.c
 * @version $Rev:: 4679         $
 * @date    $Date:: 2020-03-12 #$
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
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup USART_Examples USART
  * @{
  */

/** @addtogroup Interrupt
  * @{
  */
	
extern void TimingDelay(void);
extern void LoRa_box(void);
/*
extern void LoRa_send_Box1(void);
extern void LoRa_send_Box2(void);
*/
extern void Find_DATA_1(void);
extern void Find_DATA_1(void);
extern void NB_IOT_SEND_ALL(void);
extern void NB_Iotset(void);
extern void AM2320(void);
u8 rx_data;

u8 gps_data;

char RXRD_BUF[100];

char gps_BUF[100];

char *s;

u32 OK_flag;

u32 RXRD;

u32 gps_RXRD;

u32 RXRD_Index = 0;

u32 gps_Index = 0;


u8 lora_data;
char lora_BUF[100];
u32 lora_RXRD;
u32 lora_Index = 0;



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

/*********************************************************************************************************//**
 * @brief   This function handles SysTick Handler.
 * @retval  None
 ************************************************************************************************************/
void SysTick_Handler(void)
{
	TimingDelay();
}

/*********************************************************************************************************//**
 * @brief   This function handles BFTM interrupt.
 * @retval  None
 ************************************************************************************************************/

int BFTM_FLAG = 0;

void BFTM0_IRQHandler(void)// every 1 sec
{		
	if(BFTM_GetFlagStatus(HT_BFTM0) != RESET)
	{	
		//printf("%d ", BFTM_FLAG);
		BFTM_FLAG++;
	
		if(BFTM_FLAG == 1800) // do nb-iot and clean all buffer
		{
			//printf("do NB-Iot sent\r\n");
			NB_IOT_SEND_ALL();
			
			//Clear lora String
			memset(lora_BUF, '\0', 100); 
			strcpy(lora_BUF," ");
			lora_Index = 0;
			lora_data = 0;
			lora_RXRD = 0;		
			
			//Clear NB-IOT String
			memset(RXRD_BUF, '\0', 100); 
			strcpy(RXRD_BUF," ");
			RXRD_Index = 0;
			rx_data = 0;
			RXRD = 0;			
			
			//counter clean
			BFTM_FLAG = 0;
		}
		else if(BFTM_FLAG == 30) // do AM2320 get tem and hem data
		{
			AM2320();
		}
		else // do lora receive data and transform to char string
		{
			LoRa_box();
		}
		
		BFTM_ClearFlag(HT_BFTM0);	
	}
}

#define DEBUG_IO          (0)
/*********************************************************************************************************//**
 * @brief   This function handles UxARTn interrupt.
 * @retval  None
 ************************************************************************************************************/

void HTCFG_UART_IRQHandler(void)
{
  /* Rx: Move data from UART to buffer                                                                      */
  if (USART_GetFlagStatus(HTCFG_UART_PORT, USART_FLAG_RXDR) != RESET)
  {				
		lora_RXRD = USART_GetFlagStatus(HTCFG_UART_PORT, USART_FLAG_RXDR);
		
		lora_data = USART_ReceiveData(HTCFG_UART_PORT);
		
		lora_BUF[lora_Index++] = lora_data;
		
    #if 1 //if lora_index full then clear
    if (lora_Index == 128)
    {
      lora_Index = 0;
    }
    #endif
  }
}
/*---------------------------------------------------------------------------------------------------------*/
void COM2_IRQHandler(void)
{
  /* Rx: Move data from UART to buffer       	*/
	if (USART_GetFlagStatus(COM2_PORT, USART_FLAG_RXDR) != RESET)
  {
		RXRD = USART_GetFlagStatus(COM2_PORT, USART_FLAG_RXDR);
		rx_data = USART_ReceiveData(COM2_PORT);									
		RXRD_BUF[RXRD_Index++] = rx_data;															
		
		s = strstr(RXRD_BUF, "OK\r\n");																		
	
		if(s != NULL)
		{
				OK_flag = 1;
		}
		else
		{
				OK_flag = 0;
		}
		
		#if 1
    if (RXRD_Index == 128)
    {
      RXRD_Index = 0; // Rx Buffer full
    }
    #endif
  }
}
/*---------------------------------------------------------------------------------------------------------*/
void COM3_IRQHandler(void)
{
  /* Rx: Move data from UART to buffer       	*/
	if (USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDR))
  {
		gps_RXRD = USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDR);
		gps_data = USART_ReceiveData(COM3_PORT);	
		gps_BUF[gps_Index++] = gps_data;
		
		#if 0
    if (gps_Index == 128)
    {
      while (1) {}; // Rx Buffer full
    }
    #endif
  }
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
