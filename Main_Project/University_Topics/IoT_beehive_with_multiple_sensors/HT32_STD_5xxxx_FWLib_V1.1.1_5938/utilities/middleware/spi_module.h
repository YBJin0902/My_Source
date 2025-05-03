/*********************************************************************************************************//**
 * @file    spi_module.h
 * @version $Rev:: 568          $
 * @date    $Date:: 2022-05-31 #$
 * @brief   The header file of spi api (interrupt mode with buffer).
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
#ifndef __SPI_MODULE_H
#define __SPI_MODULE_H

/** @addtogroup HT32_Series_Peripheral_Examples HT32 Peripheral Examples
  * @{
  */

/** @addtogroup SPI SPI
  * @{
  */

/** @addtogroup SPI_API SPI API
  * @{
  */

/* Includes ------------------------------------------------------------------------------------------------*/
/* If spi_module_config.h is missing, 
   1. please copy tilities/middleware/spi_module_config_templet.h to the folder of main program
   2. and change the file name to spi_module_config.h. */
#include "spi_module_config.h"

/* Exported types ------------------------------------------------------------------------------------------*/
/**
 * @brief SPI IAP TX/RX buffer
 */
typedef struct
{
  u32 uTxRxLength;
  u8 *puTxBuffer;
  u8 *puRxBuffer;
  bool uIsTxBufferFix;
  bool uIsRxBufferFix;
} SPIAPI_BufferTypeDef;

/**
 * @brief SPI API status
 */
typedef enum
{
  SPI_API_FINISH                       = 0,
  SPI_API_BUSBUSY                      = 1,
  SPI_API_SLAVE_ABORT                  = 2,
  SPI_API_SLAVE_RX_UNEXPECTED_DATA     = 4,
} SPIAPI_Status_Enum;

/* Exported constants --------------------------------------------------------------------------------------*/
#define   SPI_API_CH0    0
#define   SPI_API_CH1    1
#define   SPI_API_CH2    2
#define   SPI_API_CH3    3

#define SPI_API_CH0_IRQHandler            STRCAT2(HTCFG_SPI_CH0, _IRQHandler)
#ifdef HTCFG_SPIM_CH1
#define SPI_API_CH1_IRQHandler            STRCAT2(HTCFG_SPI_CH1, _IRQHandler)
#endif
#ifdef HTCFG_SPIM_CH2
#define SPI_API_CH2_IRQHandler            STRCAT2(HTCFG_SPI_CH2, _IRQHandler)
#endif
#ifdef HTCFG_SPIM_CH3
#define SPI_API_CH3_IRQHandler            STRCAT2(HTCFG_SPI_CH3, _IRQHandler)
#endif

#if SPICFG_PDMA_MODE
  #if SPICFG_CH0_PDMA_ENABLE
    #if SPICFG_CH0_PDMA
    #define SPI_API_CH0_RX_PDMA_CHANNEL                PDMA_SPI1_RX
    #define SPI_API_CH0_TX_PDMA_CHANNEL                PDMA_SPI1_TX
    #define SPI_API_CH0_RX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI1_RX * 6 * 4))
    #define SPI_API_CH0_TX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI1_TX * 6 * 4))
      #if (__CORTEX_M == 0)
        #if (SPI_API_CH0_RX_PDMA_CHANNEL < 2)
        #define SPI_API_RX_PDMA_INT0                       PDMACH0_1_IRQn
        #define SPI_API_TX_PDMA_INT0                       PDMACH0_1_IRQn
        #else
        #define SPI_API_RX_PDMA_INT0                       PDMACH2_5_IRQn
        #define SPI_API_TX_PDMA_INT0                       PDMACH2_5_IRQn
        #endif
      #endif
      #if (__CORTEX_M == 3)
      #define SPI_API_RX_PDMA_INT0                       PDMACH4_IRQn
      #define SPI_API_TX_PDMA_INT0                       PDMACH5_IRQn
      #endif
    #else
    #define SPI_API_CH0_RX_PDMA_CHANNEL                PDMA_SPI0_RX
    #define SPI_API_CH0_TX_PDMA_CHANNEL                PDMA_SPI0_TX
    #define SPI_API_CH0_RX_PDMA                        (HT_PDMACH_TypeDef *)(HT_PDMA_BASE + PDMA_SPI0_RX * 6 * 4)
    #define SPI_API_CH0_TX_PDMA                        (HT_PDMACH_TypeDef *)(HT_PDMA_BASE + PDMA_SPI0_TX * 6 * 4)
      #if (__CORTEX_M == 0)
        #if (SPI_API_CH0_RX_PDMA_CHANNEL < 2)
        #define SPI_API_RX_PDMA_INT0                       PDMACH0_1_IRQn
        #define SPI_API_TX_PDMA_INT0                       PDMACH0_1_IRQn
        #else
        #define SPI_API_RX_PDMA_INT0                       PDMACH2_5_IRQn
        #define SPI_API_TX_PDMA_INT0                       PDMACH2_5_IRQn
        #endif
      #endif
      #if (__CORTEX_M == 3)
      #define SPI_API_RX_PDMA_INT0                       PDMACH0_IRQn
      #define SPI_API_TX_PDMA_INT0                       PDMACH1_IRQn
      #endif
    #endif
  #endif
  #if SPICFG_CH1_PDMA_ENABLE
    #if SPICFG_CH1_PDMA
    #define SPI_API_CH1_RX_PDMA_CHANNEL                PDMA_SPI1_RX
    #define SPI_API_CH1_TX_PDMA_CHANNEL                PDMA_SPI1_TX
    #define SPI_API_CH1_RX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI1_RX * 6 * 4))
    #define SPI_API_CH1_TX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI1_TX * 6 * 4))
      #if (__CORTEX_M == 0)
        #if (SPI_API_CH1_RX_PDMA_CHANNEL < 2)
        #define SPI_API_RX_PDMA_INT1                   PDMACH0_1_IRQn
        #define SPI_API_TX_PDMA_INT1                   PDMACH0_1_IRQn
        #else
        #define SPI_API_RX_PDMA_INT1                   PDMACH2_5_IRQn
        #define SPI_API_TX_PDMA_INT1                   PDMACH2_5_IRQn
        #endif
      #endif
      #if (__CORTEX_M == 3)
      #define SPI_API_RX_PDMA_INT1                     PDMACH4_IRQn
      #define SPI_API_TX_PDMA_INT1                     PDMACH5_IRQn
      #endif
    #else
    #define SPI_API_CH1_RX_PDMA_CHANNEL                PDMA_SPI0_RX
    #define SPI_API_CH1_TX_PDMA_CHANNEL                PDMA_SPI0_TX
    #define SPI_API_CH1_RX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI0_RX * 6 * 4))
    #define SPI_API_CH1_TX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI0_TX * 6 * 4))
      #if (__CORTEX_M == 0)
        #if (SPI_API_CH1_RX_PDMA_CHANNEL < 2)
        #define SPI_API_RX_PDMA_INT1                   PDMACH0_1_IRQn
        #define SPI_API_TX_PDMA_INT1                   PDMACH0_1_IRQn
        #else
        #define SPI_API_RX_PDMA_INT1                   PDMACH2_5_IRQn
        #define SPI_API_TX_PDMA_INT1                   PDMACH2_5_IRQn
        #endif
      #endif
      #if (__CORTEX_M == 3)
      #define SPI_API_RX_PDMA_INT1                     PDMACH0_IRQn
      #define SPI_API_TX_PDMA_INT1                     PDMACH1_IRQn
      #endif
    #endif
  #endif
  #if SPICFG_CH2_PDMA_ENABLE
    #if SPICFG_CH2_PDMA
    #define SPI_API_CH2_RX_PDMA_CHANNEL                PDMA_SPI1_RX
    #define SPI_API_CH2_TX_PDMA_CHANNEL                PDMA_SPI1_TX
    #define SPI_API_CH2_RX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI1_RX * 6 * 4))
    #define SPI_API_CH2_TX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI1_TX * 6 * 4))
      #if (__CORTEX_M == 0)
        #if (SPI_API_CH2_RX_PDMA_CHANNEL < 2)
        #define SPI_API_RX_PDMA_INT2                   PDMACH0_1_IRQn
        #define SPI_API_TX_PDMA_INT2                   PDMACH0_1_IRQn
        #else
        #define SPI_API_RX_PDMA_INT2                   PDMACH2_5_IRQn
        #define SPI_API_TX_PDMA_INT2                   PDMACH2_5_IRQn
        #endif
      #endif
      #if (__CORTEX_M == 3)
      #define SPI_API_RX_PDMA_INT2                     PDMACH4_IRQn
      #define SPI_API_TX_PDMA_INT2                     PDMACH5_IRQn
      #endif
    #else
    #define SPI_API_CH2_RX_PDMA_CHANNEL                PDMA_SPI0_RX
    #define SPI_API_CH2_TX_PDMA_CHANNEL                PDMA_SPI0_TX
    #define SPI_API_CH2_RX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI0_RX * 6 * 4))
    #define SPI_API_CH2_TX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI0_TX * 6 * 4))
      #if (__CORTEX_M == 0)
        #if (SPI_API_CH2_RX_PDMA_CHANNEL < 2)
        #define SPI_API_RX_PDMA_INT2                   PDMACH0_1_IRQn
        #define SPI_API_TX_PDMA_INT2                   PDMACH0_1_IRQn
        #else
        #define SPI_API_RX_PDMA_INT2                   PDMACH2_5_IRQn
        #define SPI_API_TX_PDMA_INT2                   PDMACH2_5_IRQn
        #endif
      #endif
      #if (__CORTEX_M == 3)
      #define SPI_API_RX_PDMA_INT2                     PDMACH0_IRQn
      #define SPI_API_TX_PDMA_INT2                     PDMACH1_IRQn
      #endif
    #endif
  #endif
  #if SPICFG_CH3_PDMA_ENABLE
    #if SPICFG_CH3_PDMA
    #define SPI_API_CH3_RX_PDMA_CHANNEL                PDMA_SPI1_RX
    #define SPI_API_CH3_TX_PDMA_CHANNEL                PDMA_SPI1_TX
    #define SPI_API_CH3_RX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI1_RX * 6 * 4))
    #define SPI_API_CH3_TX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI1_TX * 6 * 4))
      #if (__CORTEX_M == 0)
        #if (SPI_API_CH3_RX_PDMA_CHANNEL < 2)
        #define SPI_API_RX_PDMA_INT3                   PDMACH0_1_IRQn
        #define SPI_API_TX_PDMA_INT3                   PDMACH0_1_IRQn
        #else
        #define SPI_API_RX_PDMA_INT3                   PDMACH2_5_IRQn
        #define SPI_API_TX_PDMA_INT3                   PDMACH2_5_IRQn
        #endif
      #endif
      #if (__CORTEX_M == 3)
      #define SPI_API_RX_PDMA_INT3                     PDMACH4_IRQn
      #define SPI_API_TX_PDMA_INT3                     PDMACH5_IRQn
      #endif
    #else
    #define SPI_API_CH3_RX_PDMA_CHANNEL                PDMA_SPI0_RX
    #define SPI_API_CH3_TX_PDMA_CHANNEL                PDMA_SPI0_TX
    #define SPI_API_CH3_RX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI0_RX * 6 * 4))
    #define SPI_API_CH3_TX_PDMA                        ((HT_PDMACH_TypeDef*)(HT_PDMA_BASE + PDMA_SPI0_TX * 6 * 4))
      #if (__CORTEX_M == 0)
        #if (SPI_API_CH3_RX_PDMA_CHANNEL < 2)
        #define SPI_API_RX_PDMA_INT3                   PDMACH0_1_IRQn
        #define SPI_API_TX_PDMA_INT3                   PDMACH0_1_IRQn
        #else
        #define SPI_API_RX_PDMA_INT3                   PDMACH2_5_IRQn
        #define SPI_API_TX_PDMA_INT3                   PDMACH2_5_IRQn
        #endif
      #endif
      #if (__CORTEX_M == 3)
      #define SPI_API_RX_PDMA_INT3                     PDMACH0_IRQn
      #define SPI_API_TX_PDMA_INT3                     PDMACH1_IRQn
      #endif
    #endif
  #endif
#endif

/* Exported functions --------------------------------------------------------------------------------------*/
void SPIAPI_Init(u32 CH);
SPIAPI_Status_Enum SPIAPI_GetStatus(u32 CH);
bool SPIAPI_WriteRead(u32 CH, SPIAPI_BufferTypeDef *pSPIAPIBuffer);
u32 SPIAPI_GetReadBufferIndex(u32 CH);
u32 SPIAPI_GetWriteBufferIndex(u32 CH);
void SPIAPI_ClearStatus(u32 CH);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif
