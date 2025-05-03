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

#include "epaper.h"
#include "RC522.h"


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
char *ESP12_Transimit_first_data = "02C";

char ESP12_marge_data[256];


/*------------------ Message for communication ------------------*/
/*------------------ Format : 03C:weight:RFID- ------------------*/
char message_Start[5] = "03C:";
char message_conjunction[1] = ":";
char message_End[1] = "-";

char message_CRLF[2] = "\r\n";

/*==========Epaper==========*/
char epaper_dis[100];


/*==========Light singal==========*/
int Cabinet_light_flag = 0;


/*==========DIP SW setting==========*/
char DIP_message[7];

int DIP_Pin_state1 = 0;
int DIP_Pin_state2 = 0;
int DIP_Pin_state3 = 0;
int DIP_Pin_state4 = 0;

/*==========E-scale count number==========*/
float item_weight = 100;
int item_total_num = 0;


/* USER CODE END Variables */
osThreadId TaskStateLEDHandle;
osThreadId TaskInitHandle;
osThreadId TaskWiFiHandle;
osThreadId TaskUart2Handle;
osThreadId TaskLightSingalHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void DIP_read(void);
uint8_t readCard(uint8_t *readUid,void(*funCallBack)(void));
void epaper(char in_str[]);


/* USER CODE END FunctionPrototypes */

void StartTaskStateLED(void const * argument);
void StartTaskInit(void const * argument);
void StartTaskWiFi(void const * argument);
void StartTaskUart2(void const * argument);
void StartTaskLightSingal(void const * argument);

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

  /* definition and creation of TaskUart2 */
  osThreadDef(TaskUart2, StartTaskUart2, osPriorityBelowNormal, 0, 1024);
  TaskUart2Handle = osThreadCreate(osThread(TaskUart2), NULL);

  /* definition and creation of TaskLightSingal */
  osThreadDef(TaskLightSingal, StartTaskLightSingal, osPriorityLow, 0, 128);
  TaskLightSingalHandle = osThreadCreate(osThread(TaskLightSingal), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	vTaskSuspend(TaskWiFiHandle);
	vTaskSuspend(TaskUart2Handle);
	
	
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
//    HAL_GPIO_WritePin(state_LED_GPIO_Port, state_LED_Pin, GPIO_PIN_SET);
//		osDelay(300);
//		HAL_GPIO_WritePin(state_LED_GPIO_Port, state_LED_Pin, GPIO_PIN_RESET);
//		osDelay(300);
		
		HAL_GPIO_WritePin(LED_SEG_GPIO_Port, LED_SEG_Pin, GPIO_PIN_SET);
		osDelay(400);
		HAL_GPIO_WritePin(LED_SEG_GPIO_Port, LED_SEG_Pin, GPIO_PIN_RESET);
		osDelay(400);
		
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
	HAL_UART_Receive_IT(&huart2, &data2, 1);
	
	MFRC_Init();
  PCD_Reset();
	
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
    int count1 = 0, count2 = 0, count3 = 0 ,temp = 0, NAME_length = 0, AMOUNT_length = 0, mes_length = 0;
		
		char message[20] = "\0";
		char NAME[50] = "\0";
		char AMOUNT[50] = "\0";
		
		if(ESP_LEN > 20)
		{
			if(strstr(ESP_Temp, "IPD"))
			{
				while(1) // find first $
				{
					if(ESP_Temp[count1++] == '$') break;
				}

				while(1) // find second $
				{
					if(ESP_Temp[count3++] == '$')
					{
						temp++;
					}
					
					if(temp == 2) break;
				}
				
				while(1) // find End -
				{
					if(ESP_Temp[count2++] == '-') break;
				}
				
				for(int i = count1; i < count3 - 1; i++)
				{
						NAME[NAME_length++] = ESP_Temp[i];
				}
				
				for(int i = count3; i < count2 - 1; i++)
				{
						AMOUNT[AMOUNT_length++] = ESP_Temp[i];
				}
				
				strncat(epaper_dis, "\n\n", 2);
				strncat(epaper_dis, NAME, NAME_length);
				strncat(epaper_dis, "\n\n\n", 3);
				strncat(epaper_dis, AMOUNT, AMOUNT_length);
				
				HAL_UART_Transmit_IT(&huart1, (uint8_t*)epaper_dis, strlen(epaper_dis));
				osDelay(100);
				
				epaper(epaper_dis);
				
//				HAL_UART_Transmit_IT(&huart1, (uint8_t*)epaper_dis, strlen(epaper_dis));
//				osDelay(100);
				
				memset(ESP_Temp, '\0', 128);
				memset(epaper_dis, '\0', 100);
				ESP_LEN = 0;
				
				vTaskSuspend(TaskWiFiHandle);
				
			}
		}
		else
		{
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
			}
			
			if(strstr(message, "LEDON"))
			{
//				HAL_UART_Transmit_IT(&huart2, (uint8_t*)"LEDON", 5);
//				osDelay(100);
				
				Cabinet_light_flag = 1;
			}
			else if(strstr(message, "LEDOF"))
			{
//				HAL_UART_Transmit_IT(&huart2, (uint8_t*)"LEDOF", 5);
//				osDelay(100);
				
				Cabinet_light_flag = 0;
			}
		}
  }
  /* USER CODE END StartTaskWiFi */
}

/* USER CODE BEGIN Header_StartTaskUart2 */
/**
* @brief Function implementing the TaskUart2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskUart2 */
void StartTaskUart2(void const * argument)
{
  /* USER CODE BEGIN StartTaskUart2 */
  /* Infinite loop */
  for(;;)
  {
//		HAL_UART_Transmit_IT(&huart1,(uint8_t*)Data_Temp, strlen(Data_Temp));
//		osDelay(200);
		
		char message_Len[20] = "AT+CIPSEND=";
		char SEND_LEN[5] = "\0";
		int Full_str_len = 0, count = 0, mes_Length = 0;
		
		uint8_t readUid[5];
		
		char temp[20]={'\0'};
		int ID_Length = 0;
		
		char message_RFID[20];
		char message_ALL[100];
		
		float now_weight = 0;
		
		if(!readCard(readUid,NULL))
		{
			memset(message_RFID, 0, strlen(message_RFID));	
			sprintf(temp, "%d", readUid[0]);
			strncat(message_RFID, temp, strlen(temp));
			
			memset(temp, 0, strlen(temp));
			sprintf(temp, "%d", readUid[1]);
			strncat(message_RFID, temp, strlen(temp));
			
			memset(temp, 0, strlen(temp));
			sprintf(temp, "%d", readUid[2]);
			strncat(message_RFID, temp, strlen(temp));
			
			memset(temp, 0, strlen(temp));
			sprintf(temp, "%d", readUid[3]);
			strncat(message_RFID, temp, strlen(temp));
			
			ID_Length = strlen(message_RFID);
			
			memset(temp, 0, strlen(temp));			
			memset(readUid, 0, 5);
		}
		
		while(1)
		{
			if(Data_Temp[count++] == '+') break;
		}
		
		for(int i = count; i < turn_LEN; i++)
		{
			if(Data_Temp[i] == ' ')
			{
				message_ALL[mes_Length] = '0';
			}
			else
			{
				message_ALL[mes_Length] = Data_Temp[i];
			}
			
			mes_Length++;
			
			//message_ALL[mes_Length++] = Data_Temp[i];
		}
		
		now_weight = atof(message_ALL);
		
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)message_ALL, strlen(message_ALL));
		osDelay(200);
		
		
		Full_str_len = 5 + mes_Length + ID_Length;
		sprintf(SEND_LEN, "%d", Full_str_len);
		
		strncat(message_Len, SEND_LEN, strlen(SEND_LEN));
		
		//< AT+CIPSEND="message_Len"\r\n
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_Len, strlen(message_Len));
		osDelay(200);
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_CRLF, strlen(message_CRLF));
		osDelay(200);
		
		/*------------------------------------ combine hole message ---------------------------------------*/
		strncat(message_ALL, message_conjunction, 1);
		strncat(message_ALL, message_RFID, ID_Length);
		strncat(message_ALL, message_End, 1);
		/*------------------------------------ combine hole message ---------------------------------------*/
		
		//< 03C:
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_Start, strlen(message_Start));
		osDelay(200);
		
		//< Weight:RFID-
		HAL_UART_Transmit_IT(&huart1,(uint8_t*)message_ALL, strlen(message_ALL));
		osDelay(200);
		
		memset(message_ALL, '\0', 100);
		memset(message_RFID, '\0', 20);
		
		memset(Data_Temp, '\0', 20);
		memset(SEND_LEN, '\0', 5);
		
		turn_LEN = 0; 
		Full_str_len = 0;
		
		
		/*------------------------------------ calculate amount of item by weight ---------------------------------------*/
		char item_name[17] = "NAME:Consumables";
		char item_amount[11] = "AMOUNT:";
		char now_item_amount[3];
		
		item_total_num = now_weight / item_weight;
		
		sprintf(now_item_amount, "%d", item_total_num);
		
		strncat(item_amount, now_item_amount, strlen(now_item_amount));
		
		strncat(epaper_dis, "\n\n", 2);
		strncat(epaper_dis, item_name, strlen(item_name));
		strncat(epaper_dis, "\n\n\n", 3);
		strncat(epaper_dis, item_amount, strlen(item_amount));
		
		HAL_UART_Transmit_IT(&huart1, (uint8_t*)epaper_dis, strlen(epaper_dis));
		osDelay(100);
		
		epaper(epaper_dis);
		
		/*------------------------------------ calculate amount of item by weight ---------------------------------------*/
    //osDelay(100);
		vTaskSuspend(TaskUart2Handle);
  }
  /* USER CODE END StartTaskUart2 */
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
		switch(Cabinet_light_flag)
		{
			case 0:
				HAL_GPIO_WritePin(LED_SEG_GPIO_Port, LED_SEG_Pin, GPIO_PIN_SET);
				break;
			
			case 1:
				HAL_GPIO_WritePin(LED_SEG_GPIO_Port, LED_SEG_Pin, GPIO_PIN_RESET);
				break;
			
		}
  }
  /* USER CODE END StartTaskLightSingal */
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
			//ST,NT,+  0.995kg\r\n
			memcpy(Data_Temp, buffer2, data_len2 - 2);
			turn_LEN = data_len2 - 2;
			
			memset(buffer2, '\0', data_len2);
			data_len2 = 0;
			
			xTaskResumeFromISR(TaskUart2Handle);
		}
		
		HAL_UART_Receive_IT(&huart2, &data2, 1);
	}
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
	
	//HAL_UART_Transmit_IT(&huart2, (uint8_t*)DIP_message, strlen(DIP_message));
	//osDelay(300);
	
	memset(temp, 0, strlen(temp));
	HAL_Delay(500);
}

uint8_t readCard(uint8_t *readUid,void(*funCallBack)(void))
{
	uint8_t Temp[5];
	if (PCD_Request(0x52, Temp) == 0)
	{
		if (PCD_Anticoll(readUid) == 0)
		{
			if(funCallBack != NULL)
				funCallBack();
			return 0;
		}	
	}
	return 1;
}

void epaper(char in_str[])
{
	EPD_2IN9_V2_Init();
	EPD_2IN9_V2_Clear();
	epaper_resetXY();
	
	epaper_strdisplay(in_str);
	
	epaper_refresh();
	EPD_2IN9_V2_Sleep();
	//memset(in_str, '\0', 100);
}

/* USER CODE END Application */

