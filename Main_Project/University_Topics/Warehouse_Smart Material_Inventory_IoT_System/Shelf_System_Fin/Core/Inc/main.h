/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DIP1_Pin GPIO_PIN_15
#define DIP1_GPIO_Port GPIOA
#define DIP2_Pin GPIO_PIN_3
#define DIP2_GPIO_Port GPIOB
#define DIP3_Pin GPIO_PIN_4
#define DIP3_GPIO_Port GPIOB
#define DIP4_Pin GPIO_PIN_5
#define DIP4_GPIO_Port GPIOB
#define Light_RED_Pin GPIO_PIN_6
#define Light_RED_GPIO_Port GPIOB
#define Light_YEL_Pin GPIO_PIN_7
#define Light_YEL_GPIO_Port GPIOB
#define Light_GRN_Pin GPIO_PIN_8
#define Light_GRN_GPIO_Port GPIOB
#define state_LED_Pin GPIO_PIN_9
#define state_LED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
