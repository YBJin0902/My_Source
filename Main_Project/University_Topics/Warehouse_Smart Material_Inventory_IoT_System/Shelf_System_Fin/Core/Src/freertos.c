/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"
#include "gpio.h"
#include "string.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/*------------------------------------------------------------------------------
UART1 : ESP-12s

		ESP-12s SET :
			Baud -> 115200
			Wi-Fi mode -> station mode
			IP mode -> DHCP
			Server IP -> 140.130.4.49:8764 (gjt)
								-> 192.168.1.31:8124 (jian)

UART2 : Barcode

		Barcode SET :
			Baud -> 115200

------------------------------------------------------------------------------*/

uint8_t data1;
uint8_t buffer1[128];
int data_len1 = 0; 

char ESP_Temp[128] = "\0";
int ESP_LEN = 0;

/*==========Wi-Fi setting==========*/
char *ESP12_SetIPR = "AT+IPR=115200\r\n";
char *ESP12_CloseTCP = "AT+CIPCLOSE\r\n";
char *ESP12_CloseWiFi = "AT+CWQAP\r\n";
char *ESP12_WiFiMode = "AT+CWMODE=1\r\n";
char *ESP12_Connected = "AT+CWJAP=\"usb_lab_2.4G\",\"usblabwifi\"\r\n";
char *ESP12_Maskset = "AT+CIPSTA=\"192.168.1.22\",\"192.168.1.1\",\"255.255.255.0\"\r\n";
char *ESP12_DHCPSet = "AT+CWDHCP=1,1\r\n";
char *ESP12_TCPConnect = "AT+CIPSTART=\"TCP\",\"192.168.1.27\",8764\r\n";         
char *ESP12_TransimitMode = "AT+CIPMODE=0\r\n";

char *ESP12_Transimit_first_connect = "AT+CIPSEND=9\r\n";
char *ESP12_Transimit_first_data = "02L";

char ESP12_marge_data[256];

/*------------------ Message for communication ------------------*/
char message_Start[5] = "03L:";
char message_Center[20];
char message_End[1] = "-";

char message_CRLF[2] = "\r\n";
int Shelf_light_flag = 0;

/*==========DIP SW setting==========*/
char DIP_message[7];

int DIP_Pin_state1 = 0;
int DIP_Pin_state2 = 0;
int DIP_Pin_state3 = 0;
int DIP_Pin_state4 = 0;


/* USER CODE END Variables */
osThreadId TaskStateLEDHandle;
osThreadId TaskInitHandle;
osThreadId TaskWiFiHandle;
osThreadId TaskLightSingalHandle;
osThreadId TaskIPSHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void DIP_read(void);


/* USER CODE END FunctionPrototypes */

void StartTaskStateLED(void const * argument);
void StartTaskInit(void const * argument);
void StartTaskWiFi(void const * argument);
void StartTaskLightSingal(void const * argument);
void StartTaskIPS(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of TaskStateLED */
  osThreadDef(TaskStateLED, StartTaskStateLED, osPriorityIdle, 0, 128);
  TaskStateLEDHandle = osThreadCreate(osThread(TaskStateLED), NULL);

  /* definition and creation of TaskInit */
  osThreadDef(TaskInit, StartTaskInit, osPriorityHigh, 0, 512);
  TaskInitHandle = osThreadCreate(osThread(TaskInit), NULL);

  /* definition and creation of TaskWiFi */
  osThreadDef(TaskWiFi, StartTaskWiFi, osPriorityNormal, 0, 1024);
  TaskWiFiHandle = osThreadCreate(osThread(TaskWiFi), NULL);

  /* definition and creation of TaskLightSingal */
  osThreadDef(TaskLightSingal, StartTaskLightSingal, osPriorityLow, 0, 512);
  TaskLightSingalHandle = osThreadCreate(osThread(TaskLightSingal), NULL);

  /* definition and creation of TaskIPS */
  osThreadDef(TaskIPS, StartTaskIPS, osPriorityAboveNormal, 0, 1024);
  TaskIPSHandle = osThreadCreate(osThread(TaskIPS), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	vTaskSuspend(TaskWiFiHandle);
	vTaskSuspend(TaskIPSHandle);
	
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartTaskStateLED */
/**
  * @brief  Function implementing the TaskStateLED thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskStateLED */
void StartTaskStateLED(void const * argument)
{
  /* USER CODE BEGIN StartTaskStateLED */
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_WritePin(state_LED_GPIO_Port, state_LED_Pin, GPIO_PIN_SET);
		osDelay(300);
		HAL_GPIO_WritePin(state_LED_GPIO_Port, state_LED_Pin, GPIO_PIN_RESET);
		osDelay(300);
		
  }
  /* USER CODE END StartTaskStateLED */
}

/* USER CODE BEGIN Header_StartTaskInit */
/**
* @brief Function implementing the TaskInit thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskInit */
void StartTaskInit(void const * argument)
{
  /* USER CODE BEGIN StartTaskInit */
	DIP_read();
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_SetIPR,strlen(ESP12_SetIPR));
	osDelay(500);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_CloseTCP,strlen(ESP12_CloseTCP));
	osDelay(500);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_CloseWiFi,strlen(ESP12_CloseWiFi));
	osDelay(500);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_WiFiMode,strlen(ESP12_WiFiMode));
	osDelay(500);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_Connected,strlen(ESP12_Connected));
	osDelay(3000);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_DHCPSet,strlen(ESP12_DHCPSet));
	osDelay(500);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_TCPConnect,strlen(ESP12_TCPConnect));
	osDelay(3500);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_TransimitMode,strlen(ESP12_TransimitMode));
	osDelay(500);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_Transimit_first_connect,strlen(ESP12_Transimit_first_connect));
	osDelay(500);
	
	HAL_UART_Transmit_IT(&huart1, (uint8_t*)ESP12_Transimit_first_data, strlen(ESP12_Transimit_first_data));
	osDelay(100);
	
	HAL_UART_Transmit_IT(&huart1, (uint8_t*)DIP_message, strlen(DIP_message));
	osDelay(100);
	
	HAL_UART_Transmit_IT(&huart1, (uint8_t*)message_End, strlen(message_End));
	osDelay(100);
	
	HAL_UART_Receive_IT(&huart1, &data1, 1);
	
	vTaskDelete(NULL); // Remove hole Task!!
	
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskInit */
}

/* USER CODE BEGIN Header_StartTaskWiFi */
/**
* @brief Function implementing the TaskWiFi thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskWiFi */
void StartTaskWiFi(void const * argument)
{
  /* USER CODE BEGIN StartTaskWiFi */
  /* Infinite loop */
  for(;;)
  {
//		HAL_UART_Transmit_IT(&huart2, (uint8_t*)ESP_Temp, ESP_LEN);
//		osDelay(300);
		
		int count1 = 0, count2 = 0, mes_length = 0;
		
		char message[20] = "\0";
		
		if(strstr(ESP_Temp, "IPD"))
		{
			while(1) // find first $
			{
				if(ESP_Temp[count1++] == '$') break;
			}

			while(1) // find END
			{
				if(ESP_Temp[count2++] == '-') break;
			}
			
			for(int i = count1; i < count2 - 1; i++)
			{
					message[mes_length++] = ESP_Temp[i];
			}
			
			
			if(strstr(message, "REDON"))
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t*)"REDON", 5);
				osDelay(100);
				
				Shelf_light_flag = 1;
			}
			else if(strstr(message, "REDOF"))
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t*)"REDOF", 5);
				osDelay(100);
				
				Shelf_light_flag = 2;
			}
			else if(strstr(message, "YELON"))
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t*)"YELON", 5);
				osDelay(100);
				
				Shelf_light_flag = 3;
			}
			else if(strstr(message, "YELOF"))
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t*)"YELOF", 5);
				osDelay(100);
				
				Shelf_light_flag = 4;
			}
			else if(strstr(message, "GRNON"))
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t*)"GRNON", 5);
				osDelay(100);
				
				Shelf_light_flag = 5;
			}
			else if(strstr(message, "GRNOF"))
			{
				HAL_UART_Transmit_IT(&huart2, (uint8_t*)"GRNOF", 5);
				osDelay(100);
				
				Shelf_light_flag = 6;
			}
		}
		
		memset(ESP_Temp, '\0', ESP_LEN);
		ESP_LEN = 0;
		
		vTaskSuspend(TaskWiFiHandle);
  }
  /* USER CODE END StartTaskWiFi */
}

/* USER CODE BEGIN Header_StartTaskLightSingal */
/**
* @brief Function implementing the TaskLightSingal thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskLightSingal */
void StartTaskLightSingal(void const * argument)
{
  /* USER CODE BEGIN StartTaskLightSingal */
  /* Infinite loop */
  for(;;)
  {
		osDelay(200);
		
    switch(Shelf_light_flag)
		{
			case 1:
				HAL_GPIO_WritePin(Light_RED_GPIO_Port, Light_RED_Pin, GPIO_PIN_SET);
				break;
			
			case 2:
				HAL_GPIO_WritePin(Light_RED_GPIO_Port, Light_RED_Pin, GPIO_PIN_RESET);
				break;
			
			case 3:
				HAL_GPIO_WritePin(Light_YEL_GPIO_Port, Light_YEL_Pin, GPIO_PIN_SET);
				break;
			
			case 4:
				HAL_GPIO_WritePin(Light_YEL_GPIO_Port, Light_YEL_Pin, GPIO_PIN_RESET);
				break;
			
			case 5:
				HAL_GPIO_WritePin(Light_GRN_GPIO_Port, Light_GRN_Pin, GPIO_PIN_SET);
				break;
			
			case 6:
				HAL_GPIO_WritePin(Light_GRN_GPIO_Port, Light_GRN_Pin, GPIO_PIN_RESET);
				break;
			
		}
  }
  /* USER CODE END StartTaskLightSingal */
}

/* USER CODE BEGIN Header_StartTaskIPS */
/**
* @brief Function implementing the TaskIPS thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskIPS */
void StartTaskIPS(void const * argument)
{
  /* USER CODE BEGIN StartTaskIPS */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskIPS */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	/*------------------------- USART1 -------------------------*/
	if(UartHandle->Instance == USART1)
	{
		buffer1[data_len1] = data1;
		data_len1++;                          			
		
		if(buffer1[data_len1 - 1] == '\n')
		{
			memcpy(ESP_Temp, buffer1, data_len1 - 2);
			ESP_LEN = data_len1 - 2;
			
			memset(buffer1, '\0', data_len1);
			data_len1 = 0;
			
			xTaskResumeFromISR(TaskWiFiHandle);
		}
		
		HAL_UART_Receive_IT(&huart1, &data1, 1);
	}
	
	/*------------------------- USART2 -------------------------*/

}

void DIP_read(void)
{
	char DIP_temp[5];
	char temp[10] = {'\0'}; // storage DIP pin state
	
	if(HAL_GPIO_ReadPin(DIP1_GPIO_Port, DIP1_Pin) == GPIO_PIN_RESET)
	{
		DIP_Pin_state1 = 1;
	}
	else 
	{
		DIP_Pin_state1 = 0;
	}
	
	if(HAL_GPIO_ReadPin(DIP2_GPIO_Port, DIP2_Pin) == GPIO_PIN_RESET)
	{
		DIP_Pin_state2 = 1;
	}
	else 
	{
		DIP_Pin_state2 = 0;
	}
	
	if(HAL_GPIO_ReadPin(DIP3_GPIO_Port, DIP3_Pin) == GPIO_PIN_RESET)
	{
		DIP_Pin_state3 = 1;
	}
	else 
	{
		DIP_Pin_state3 = 0;
	}
	
	if(HAL_GPIO_ReadPin(DIP4_GPIO_Port, DIP4_Pin) == GPIO_PIN_RESET)
	{
		DIP_Pin_state4 = 1;
	}
	else 
	{
		DIP_Pin_state4 = 0;
	}
	
	sprintf(temp, "%d", DIP_Pin_state1);
	strncat(DIP_temp, temp, strlen(temp));
	memset(temp, 0, strlen(temp));
	
	sprintf(temp, "%d", DIP_Pin_state2);
	strncat(DIP_temp, temp, strlen(temp));
	memset(temp, 0, strlen(temp));
	
	sprintf(temp, "%d", DIP_Pin_state3);
	strncat(DIP_temp, temp, strlen(temp));
	memset(temp, 0, strlen(temp));
	
	sprintf(temp, "%d", DIP_Pin_state4);
	strncat(DIP_temp, temp, strlen(temp));
	memset(temp, 0, strlen(temp));
	
	int DIP_Length = 0;

	DIP_Length = strlen(DIP_temp);
	//All_length = 7 - DIP_Length;
	
	for(int i = 0; i < 5 - DIP_Length; i++)
	{
		DIP_message[i] = '0';
	}
	
	strncat(DIP_message, DIP_temp, strlen(DIP_temp));
	
	DIP_message[0] = 'l';
	
	//HAL_UART_Transmit_IT(&huart2, (uint8_t*)DIP_message, strlen(DIP_message));
	//osDelay(300);
	
	memset(temp, 0, strlen(temp));
	HAL_Delay(500);
}

/* USER CODE END Application */

