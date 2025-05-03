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

uint8_t data2;
uint8_t buffer2[128];
int data_len2 = 0; 

char Data_Temp[20] = "\0";
int turn_LEN = 0;

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
char *ESP12_Transimit_first_data = "02A00001-";

char ESP12_marge_data[256];


/*------------------ Message for communication ------------------*/
char message_Start[5] = "03A:";
char message_Center[20];
char message_End[1] = "-";

char message_CRLF[2] = "\r\n";


/* USER CODE END Variables */
osThreadId TaskStateLEDHandle;
osThreadId TaskInitHandle;
osThreadId TaskWiFiHandle;
osThreadId TaskBarcodeHandle;
osThreadId TaskIPSHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
	

/* USER CODE END FunctionPrototypes */

void StartTaskStateLED(void const * argument);
void StartTaskInit(void const * argument);
void StartTaskWiFi(void const * argument);
void StartTaskBarcode(void const * argument);
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

  /* definition and creation of TaskBarcode */
  osThreadDef(TaskBarcode, StartTaskBarcode, osPriorityBelowNormal, 0, 1024);
  TaskBarcodeHandle = osThreadCreate(osThread(TaskBarcode), NULL);

  /* definition and creation of TaskIPS */
  osThreadDef(TaskIPS, StartTaskIPS, osPriorityAboveNormal, 0, 1024);
  TaskIPSHandle = osThreadCreate(osThread(TaskIPS), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	vTaskSuspend(TaskWiFiHandle);
	vTaskSuspend(TaskBarcodeHandle);
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
		//HAL_GPIO_TogglePin(state_LED_GPIO_Port, state_LED_Pin);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		osDelay(300);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
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
	
	HAL_UART_Receive_IT(&huart1, &data1, 1);
	HAL_UART_Receive_IT(&huart2, &data2, 1);
	
	vTaskDelete(NULL); // Remove hole Task!!
  /* Infinite loop */
  for(;;)
  {
		
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
		memset(ESP_Temp, '\0', 128);
		ESP_LEN = 0;
		
		vTaskSuspend(TaskWiFiHandle);
  }
  /* USER CODE END StartTaskWiFi */
}

/* USER CODE BEGIN Header_StartTaskBarcode */
/**
* @brief Function implementing the TaskBarcode thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskBarcode */
void StartTaskBarcode(void const * argument)
{
  /* USER CODE BEGIN StartTaskBarcode */
	
  /* Infinite loop */
  for(;;)
  {
		char message_Len[20] = "AT+CIPSEND=";
		char SEND_LEN[5] = "\0";
		int Full_str_len = 0;
		
		strncat(message_Center, Data_Temp, turn_LEN);
		
		Full_str_len = 5 + turn_LEN;
		sprintf(SEND_LEN, "%d", Full_str_len);
		
		strncat(message_Len, SEND_LEN, strlen(SEND_LEN));
		
		//< AT+CIPSEND="message_Len"\r\n
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_Len, strlen(message_Len));
		osDelay(200);
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_CRLF, strlen(message_CRLF));
		osDelay(200);
		
		//< 03A:
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_Start, strlen(message_Start));
		osDelay(500);
		
		//< QR-code
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_Center, strlen(message_Center));
		osDelay(500);
		
		//< -
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_End, strlen(message_End));
		osDelay(500);
		
		memset(message_Center, '\0', 20);
		memset(Data_Temp, '\0', 20);
		memset(SEND_LEN, '\0', 5);
		turn_LEN = 0; 
		Full_str_len = 0;
		
    //osDelay(100);
		vTaskSuspend(TaskBarcodeHandle);
  }
  /* USER CODE END StartTaskBarcode */
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
	if(UartHandle->Instance == USART2)
	{
		buffer2[data_len2] = data2;
		data_len2++;                          			
		
		if(buffer2[data_len2 - 1] == '\n')
		{
			//HAL_UART_Transmit_IT(&huart1, (uint8_t*)"UART2\r\n", 7);
			memcpy(Data_Temp, buffer2, data_len2 - 2);
			turn_LEN = data_len2 - 2;
			
			memset(buffer2, '\0', data_len2);
			data_len2 = 0;
			
			xTaskResumeFromISR(TaskBarcodeHandle);
		}
		
		HAL_UART_Receive_IT(&huart2, &data2, 1);
	}
}

/* USER CODE END Application */

