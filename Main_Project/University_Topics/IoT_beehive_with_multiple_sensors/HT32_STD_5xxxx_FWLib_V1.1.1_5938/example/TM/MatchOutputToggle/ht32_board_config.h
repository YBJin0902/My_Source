/*********************************************************************************************************//**
 * @file    TM/MatchOutputToggle/ht32_board_config.h
 * @version $Rev:: 5805         $
 * @date    $Date:: 2022-04-12 #$
 * @brief   The header file of board configuration.
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
/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __HT32_BOARD_CONFIG_H
#define __HT32_BOARD_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Settings ------------------------------------------------------------------------------------------------*/
#if defined(USE_HT32F52230_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F52241_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F52253_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F52341_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F52352_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   C
  #define _HTCFG_COMP_CH1_GPIOX                   C
  #define _HTCFG_COMP_CH2_GPIOX                   C
  #define _HTCFG_COMP_CH3_GPIOX                   C
  #define _HTCFG_COMP_CH0_GPION                   4
  #define _HTCFG_COMP_CH1_GPION                   5
  #define _HTCFG_COMP_CH2_GPION                   8
  #define _HTCFG_COMP_CH3_GPION                   9

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F0008_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F50230_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F50241_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F52354_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F0006_DVB)
  #define _HTCFG_COMP_CH0_GPIOX                   C
  #define _HTCFG_COMP_CH1_GPIOX                   C
  #define _HTCFG_COMP_CH2_GPIOX                   C
  #define _HTCFG_COMP_CH3_GPIOX                   C
  #define _HTCFG_COMP_CH0_GPION                   4
  #define _HTCFG_COMP_CH1_GPION                   5
  #define _HTCFG_COMP_CH2_GPION                   8
  #define _HTCFG_COMP_CH3_GPION                   9

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F57341_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   C
  #define _HTCFG_COMP_CH1_GPIOX                   C
  #define _HTCFG_COMP_CH2_GPIOX                   C
  #define _HTCFG_COMP_CH3_GPIOX                   C
  #define _HTCFG_COMP_CH0_GPION                   10
  #define _HTCFG_COMP_CH1_GPION                   11
  #define _HTCFG_COMP_CH2_GPION                   12
  #define _HTCFG_COMP_CH3_GPION                   13

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F57352_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   C
  #define _HTCFG_COMP_CH1_GPIOX                   C
  #define _HTCFG_COMP_CH2_GPIOX                   C
  #define _HTCFG_COMP_CH3_GPIOX                   C
  #define _HTCFG_COMP_CH0_GPION                   10
  #define _HTCFG_COMP_CH1_GPION                   11
  #define _HTCFG_COMP_CH2_GPION                   12
  #define _HTCFG_COMP_CH3_GPION                   13

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F50343_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F52367_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_BM53A367A_DVB)
  #define _HTCFG_COMP_CH0_GPIOX                   A               // D27
  #define _HTCFG_COMP_CH1_GPIOX                   A               // D28
  #define _HTCFG_COMP_CH2_GPIOX                   A               // D26
  #define _HTCFG_COMP_CH3_GPIOX                   A               // D29
  #define _HTCFG_COMP_CH0_GPION                   4
  #define _HTCFG_COMP_CH1_GPION                   5
  #define _HTCFG_COMP_CH2_GPION                   6
  #define _HTCFG_COMP_CH3_GPION                   7

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F61141_SK)  //PRELIMINARY_NOT_TEST
  #define _HTCFG_COMP_CH0_GPIOX                   B
  #define _HTCFG_COMP_CH1_GPIOX                   B
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   1
  #define _HTCFG_COMP_CH1_GPION                   0
  #define _HTCFG_COMP_CH2_GPION                   15
  #define _HTCFG_COMP_CH3_GPION                   14

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F65240_DVB)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   C
  #define _HTCFG_COMP_CH2_GPIOX                   C
  #define _HTCFG_COMP_CH3_GPIOX                   C
  #define _HTCFG_COMP_CH0_GPION                   7
  #define _HTCFG_COMP_CH1_GPION                   4
  #define _HTCFG_COMP_CH2_GPION                   5
  #define _HTCFG_COMP_CH3_GPION                   6

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F65240_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   C
  #define _HTCFG_COMP_CH2_GPIOX                   C
  #define _HTCFG_COMP_CH3_GPIOX                   C
  #define _HTCFG_COMP_CH0_GPION                   7
  #define _HTCFG_COMP_CH1_GPION                   4
  #define _HTCFG_COMP_CH2_GPION                   5
  #define _HTCFG_COMP_CH3_GPION                   6

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F65232_SK)  //PRELIMINARY_NOT_TEST
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   C
  #define _HTCFG_COMP_CH2_GPIOX                   C
  #define _HTCFG_COMP_CH3_GPIOX                   C
  #define _HTCFG_COMP_CH0_GPION                   7
  #define _HTCFG_COMP_CH1_GPION                   4
  #define _HTCFG_COMP_CH2_GPION                   5
  #define _HTCFG_COMP_CH3_GPION                   6

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F61355_SK) || defined(USE_HT32F61356_SK) || defined(USE_HT32F61357_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   C
  #define _HTCFG_COMP_CH1_GPIOX                   C
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   4
  #define _HTCFG_COMP_CH1_GPION                   5
  #define _HTCFG_COMP_CH2_GPION                   6
  #define _HTCFG_COMP_CH3_GPION                   7

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F54241_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#if defined(USE_HT32F54253_SK)
  #define _HTCFG_COMP_CH0_GPIOX                   A
  #define _HTCFG_COMP_CH1_GPIOX                   A
  #define _HTCFG_COMP_CH2_GPIOX                   A
  #define _HTCFG_COMP_CH3_GPIOX                   A
  #define _HTCFG_COMP_CH0_GPION                   0
  #define _HTCFG_COMP_CH1_GPION                   1
  #define _HTCFG_COMP_CH2_GPION                   2
  #define _HTCFG_COMP_CH3_GPION                   3

  #define HTCFG_COMP_IPN                          GPTM0
#endif

#define HTCFG_COMP_CH0_GPIO_ID                    STRCAT2(GPIO_P,         _HTCFG_COMP_CH0_GPIOX)
#define HTCFG_COMP_CH1_GPIO_ID                    STRCAT2(GPIO_P,         _HTCFG_COMP_CH1_GPIOX)
#define HTCFG_COMP_CH2_GPIO_ID                    STRCAT2(GPIO_P,         _HTCFG_COMP_CH2_GPIOX)
#define HTCFG_COMP_CH3_GPIO_ID                    STRCAT2(GPIO_P,         _HTCFG_COMP_CH3_GPIOX)

#define HTCFG_COMP_CH0_AFIO_PIN                   STRCAT2(AFIO_PIN_,      _HTCFG_COMP_CH0_GPION)
#define HTCFG_COMP_CH1_AFIO_PIN                   STRCAT2(AFIO_PIN_,      _HTCFG_COMP_CH1_GPION)
#define HTCFG_COMP_CH2_AFIO_PIN                   STRCAT2(AFIO_PIN_,      _HTCFG_COMP_CH2_GPION)
#define HTCFG_COMP_CH3_AFIO_PIN                   STRCAT2(AFIO_PIN_,      _HTCFG_COMP_CH3_GPION)

#define HTCFG_COMP_PORT                           STRCAT2(HT_,             HTCFG_COMP_IPN)

#if (LIBCFG_GPTM_GIRQ == 1)
  #define HTCFG_COMP_IRQn                         STRCAT2(HTCFG_COMP_IPN,  _G_IRQn)
  #define HTCFG_COMP_IRQHandler                   STRCAT2(HTCFG_COMP_IPN,  _G_IRQHandler)
#else
  #define HTCFG_COMP_IRQn                         STRCAT2(HTCFG_COMP_IPN,  _IRQn)
  #define HTCFG_COMP_IRQHandler                   STRCAT2(HTCFG_COMP_IPN,  _IRQHandler)
#endif


#ifdef __cplusplus
}
#endif

#endif
