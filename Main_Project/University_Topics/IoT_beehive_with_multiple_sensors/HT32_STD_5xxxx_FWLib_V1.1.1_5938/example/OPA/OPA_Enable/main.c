/*********************************************************************************************************//**
 * @file    OPA/OPA_Enable/main.c
 * @version $Rev:: 5074         $
 * @date    $Date:: 2020-11-08 #$
 * @brief   Main program.
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

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup OPA_Examples OPA
  * @{
  */

/** @addtogroup OPA_Enable
  * @{
  */


/* Private function prototypes -----------------------------------------------------------------------------*/
void OPA_Configuration(void);

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
int main(void)
{
  OPA_Configuration();

  while (1)
  {
  }
}

/*********************************************************************************************************//**
  * @brief  OPA configuration.
  * @retval None
  ***********************************************************************************************************/
void OPA_Configuration(void)
{
  { /* Enable peripheral clock                                                                              */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};
    CKCUClock.Bit.OPA = 1;
    CKCUClock.Bit.AFIO = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Configure AFIO mode as OPA function                                                                    */
  AFIO_GPxConfig(GPIO_PB, GPIO_PIN_5, AFIO_FUN_CMP_OPA);
  AFIO_GPxConfig(GPIO_PC, GPIO_PIN_1, AFIO_FUN_CMP_OPA);
  AFIO_GPxConfig(GPIO_PC, GPIO_PIN_2, AFIO_FUN_CMP_OPA);

  AFIO_GPxConfig(GPIO_PC, GPIO_PIN_3, AFIO_FUN_CMP_OPA);
  AFIO_GPxConfig(GPIO_PB, GPIO_PIN_6, AFIO_FUN_CMP_OPA);
  AFIO_GPxConfig(GPIO_PB, GPIO_PIN_7, AFIO_FUN_CMP_OPA);

  /* Enable OPA                                                                                             */
  OPA_Cmd(HT_OPA0, ENABLE);
  #if defined(USE_HT32F65230_40)
  OPA_Cmd(HT_OPA1, ENABLE);
  #endif
}

#if (HT32_LIB_DEBUG == 1)
/*********************************************************************************************************//**
  * @brief  Report both the error name of the source file and the source line number.
  * @param  filename: pointer to the source file name.
  * @param  uline: error line source number.
  * @retval None
  ***********************************************************************************************************/
void assert_error(u8* filename, u32 uline)
{
  /*
     This function is called by IP library that the invalid parameters has been passed to the library API.
     Debug message can be added here.
     Example: printf("Parameter Error: file %s on line %d\r\n", filename, uline);
  */

  while (1)
  {
  }
}
#endif


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
