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
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MP_wait 	5000
#define BP_wait 	10000

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

/*========== Wi-Fi setting ==========*/
char *ESP12_NoEcho = "ATE0\r\n";
char *ESP12_SetIPR = "AT+IPR=115200\r\n";
char *ESP12_CloseTCP = "AT+CIPCLOSE\r\n";
char *ESP12_CloseWiFi = "AT+CWQAP\r\n";
char *ESP12_WiFiMode = "AT+CWMODE=1\r\n";
char *ESP12_Connected = "AT+CWJAP=\"usb_lab_2.4G\",\"usblabwifi\"\r\n";
char *ESP12_Maskset = "AT+CIPSTA=\"192.168.1.22\",\"192.168.1.1\",\"255.255.255.0\"\r\n";
char *ESP12_DHCPSet = "AT+CWDHCP=1,1\r\n";
char *ESP12_TCPConnect = "AT+CIPSTART=\"TCP\",\"192.168.1.23\",21\r\n";         
char *ESP12_TransimitMode = "AT+CIPMODE=0\r\n";

char *ESP12_Transimit_first_connect = "AT+CIPSEND=9\r\n";
char *ESP12_Transimit_first_data = "02A0000\r\n";

/*========== IPS use ==========*/
int IPS_flag = 0;

char *IPS_WiFimode = "AT+CWMODE=3\r\n";
char *IPS_ShareWiFi = "AT+CWSAP=\"ESP_MP\",\"00000000\",1,4\r\n";
char *IPS_Connect = "AT+CWJAP=\"ESP_MP\",\"00000000\"\r\n";
char *IPS_RSSI = "AT+CWJAP?\r\n";




/* USER CODE END Variables */
osThreadId TaskStateLEDHandle;
osThreadId TaskInitHandle;
osThreadId TaskIPSHandle;
osThreadId TaskUartHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTaskStateLED(void const * argument);
void StartTaskInit(void const * argument);
void StartTaskIPS(void const * argument);
void StartTaskUart(void const * argument);

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
  osThreadDef(TaskInit, StartTaskInit, osPriorityRealtime, 0, 1024);
  TaskInitHandle = osThreadCreate(osThread(TaskInit), NULL);

  /* definition and creation of TaskIPS */
  osThreadDef(TaskIPS, StartTaskIPS, osPriorityHigh, 0, 1024);
  TaskIPSHandle = osThreadCreate(osThread(TaskIPS), NULL);

  /* definition and creation of TaskUart */
  osThreadDef(TaskUart, StartTaskUart, osPriorityNormal, 0, 1024);
  TaskUartHandle = osThreadCreate(osThread(TaskUart), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	vTaskSuspend(TaskIPSHandle);
	vTaskSuspend(TaskUartHandle);
	
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
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
		osDelay(300);
		
		HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
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
	HAL_UART_Transmit_IT(&huart2,(uint8_t*)"Start\r\n", 7);
	osDelay(200);
	
	HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_NoEcho,strlen(ESP12_NoEcho));
	osDelay(500);
	
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
	
	HAL_UART_Transmit_IT(&huart2,(uint8_t*)"finish\r\n", 8);
	osDelay(500);
	
	HAL_UART_Receive_IT(&huart1, &data1, 1);
	
	vTaskDelete(NULL); // Remove hole Task!!
	
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTaskInit */
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
		
		
		vTaskSuspend(TaskIPSHandle);
    osDelay(1);
  }
  /* USER CODE END StartTaskIPS */
}

/* USER CODE BEGIN Header_StartTaskUart */
/**
* @brief Function implementing the TaskUart thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUart */
void StartTaskUart(void const * argument)
{
  /* USER CODE BEGIN StartTaskUart */
  /* Infinite loop */
  for(;;)
  {
//		HAL_UART_Transmit_IT(&huart2,(uint8_t*)ESP_Temp, strlen(ESP_Temp));
//		osDelay(1000);
		
		int count1 = 0, count2 = 0, temp1 = 0, temp2 = 0, mes_length = 0;
		
		char message[4] = "\0";
		
		switch(IPS_flag)
		{
			//MP mode
			case 1:
				HAL_UART_Transmit_IT(&huart2,(uint8_t*)"MP mode\r\n", 9);
				osDelay(500);
				
				//Change Wi-Fi mode to STA+AP mode
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)IPS_WiFimode, strlen(IPS_WiFimode));
				osDelay(500);
				
				//Share device's own Wi-Fi
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)IPS_ShareWiFi, strlen(IPS_ShareWiFi));
				osDelay(500);
			
				osDelay(MP_wait);
				
				break;
			
			//BP mode
			case 2:
				//osDelay(BP_wait);
			
				HAL_UART_Transmit_IT(&huart2,(uint8_t*)"BP mode\r\n", 9);
				osDelay(500);
			
				HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_CloseTCP,strlen(ESP12_CloseTCP));
				osDelay(500);
				
				//Connect MP shared Wi-Fi
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)IPS_Connect, strlen(IPS_Connect));
				osDelay(2500);
				
				//Get RSSI
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)IPS_RSSI, strlen(IPS_RSSI));

				break;
			
			//END
			case 3:
				HAL_UART_Transmit_IT(&huart2,(uint8_t*)"END\r\n", 5);
				osDelay(500);
				
				HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_WiFiMode,strlen(ESP12_WiFiMode));
				osDelay(500);
			
				break;
			
			//RSSI
			case 4:
				
				HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_Connected,strlen(ESP12_Connected));
				osDelay(3000);
				
				HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_DHCPSet,strlen(ESP12_DHCPSet));
				osDelay(500);
				
				HAL_UART_Transmit_IT(&huart1,(uint8_t*)ESP12_TCPConnect,strlen(ESP12_TCPConnect));
				osDelay(3500);
				
				HAL_UART_Transmit_IT(&huart2,(uint8_t*)"RSSI : ", 7);
				osDelay(500);
				
				while(1)
				{
					if(temp1 == 3) break;
					
					if(ESP_Temp[count1++] == ',')
					{
						temp1++;
					}
				}
				
				while(1)
				{
					if(temp2 == 4) break;
					
					if(ESP_Temp[count2++] == ',')
					{
						temp2++;
					}
				}
				
				for(int i = count1; i < count2 - 1; i++)
				{
					message[mes_length++] = ESP_Temp[i];
				}
				
				HAL_UART_Transmit_IT(&huart2, (uint8_t*)message, mes_length);
				osDelay(500);
				
				int RSSI_len = 0;
				char SNED_LEN[5] = "\0";
				
				char IPS_SendLen[20] = "AT+CIPSEND=";
				char CRLF[2] = "\r\n";
				char IPS_Transimit_format[10] = "04A/";
				
				RSSI_len = mes_length + 4 + 1;
				
				sprintf(SNED_LEN, "%d", RSSI_len);
				strncat(IPS_SendLen, SNED_LEN, strlen(SNED_LEN));
				
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)IPS_SendLen, strlen(IPS_SendLen));
				osDelay(100);
				
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)CRLF, strlen(CRLF));
				osDelay(300);
				
				strncat(IPS_Transimit_format, message, strlen(message));
				
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)IPS_Transimit_format, strlen(IPS_Transimit_format));
				osDelay(100);
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)"/", 1);
			
				break;
		}
		
		memset(ESP_Temp, '\0', ESP_LEN);
		ESP_LEN = 0;
		
		memset(message, '\0', mes_length);
		mes_length = 0;
		
		IPS_flag = 0;
		
		vTaskSuspend(TaskUartHandle);
    osDelay(1);
  }
  /* USER CODE END StartTaskUart */
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
			if(buffer1[0] == '+')
			{
				memcpy(ESP_Temp, buffer1, data_len1 - 2);
				ESP_LEN = data_len1 - 2;
				
				if(buffer1[data_len1 - 3] == '@')
				{
					IPS_flag = 2;
				}
				else if(buffer1[1] == 'C' && buffer1[2] == 'W' && buffer1[3] == 'J' && buffer1[4] == 'A' && buffer1[5] == 'P')
				{
					IPS_flag = 4;
				}
				
				xTaskResumeFromISR(TaskUartHandle);
			}
			
			memset(buffer1, '\0', data_len1);
			data_len1 = 0;
		}
		
		HAL_UART_Receive_IT(&huart1, &data1, 1);
	}
}
/* USER CODE END Application */

