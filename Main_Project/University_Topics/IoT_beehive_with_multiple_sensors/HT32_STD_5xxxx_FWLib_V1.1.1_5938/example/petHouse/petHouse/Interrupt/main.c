/*********************************************************************************************************//**
 * @file    USART/Interrupt/main.c
 * @version $Rev:: 5073         $
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
#include "ht32_board_config.h"
#include "string.h"
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


/* Private function prototypes -----------------------------------------------------------------------------*/
void UxART_Configuration(void);
void UxART_TxTest(void);
void UxART_RxTest(void);


/* Global variables ----------------------------------------------------------------------------------------*/
uc8  *gURTx_Ptr;
vu32 gURTx_Length = 0;
u8  *gURRx_Ptr;
vu32 gURRx_Length = 0;

vu32 gIsTxFinished = FALSE;

vu32 DelayTime;
void Delay(u32 nTime)
{
	/* Enable the SysTick Counter */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);

	DelayTime = nTime;

	while(DelayTime != 0);

	/* Disable SysTick Counter */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
	/* Clear SysTick Counter */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
}

void TimingDelay(void)
{
  if(DelayTime != 0)
  {
    DelayTime--;
  }
}

/*LoRa*/
u8 LoRa_AddressSet[]={"AT+ADDRESS=43207\r\n"};
u8 LoRa_NetworkIDSet[]={"AT+NETWORKID=10\r\n"};
u8 LoRa_ParameterSet[]={"AT+PARAMETER=10,7,1,7\r\n"};

extern u8 LoRadata;
extern char LoRa_BUF[200];
extern u32 LoRa_Index;

int DistantFlag = 0;
/*LoRa*/

/*WiFi*/
char	*Wifi_Account = "Account";
char	*Wifi_Password = "password";

char	*MQTT_IP = "www.coolerd.net";
char	*MQTT_Port = "1883";

u8 mqtt_message[200]={0};
u8 mqtt_msg[200]={0};
u8 mqttLen;
u8 t;

u8 ESP12S_mqtt_send[] = {"AT+CIPSEND="};

char  *MQTT_Msg = "test";
char  *MQTT_Topic = "MQTTtest";
char  *msg[100];

u8 ESP12S_control_msg[14];
/*WiFi*/

/*RS485*/
extern u8 RXdata;
extern u32 RX_Index;
extern char RX_BUF[200];

char VALUE[200];
char value[200];
char *comma = ",";

char  *temp = "00";

char *DOGHOUSE_CODENAME = "testcode";

int temperature;


/*RS485*/

/*Solar*/
extern u8 SOLARdata;
extern u32 SOLAR_Index;
extern char SOLAR_BUF[200];

char VALUE[200];
char value[200];
//char *comma = ",";

//char *DOGHOUSE_CODENAME = "testcode";
/*Solar*/

/* Private variables ---------------------------------------------------------------------------------------*/
uc8 gHelloString[] = "Hello, this is USART Tx/Rx interrupt example. Please enter 5 characters...\r\n";
u8 gTx_Buffer[128];
u8 gRx_Buffer[128];

/*LoRa*/
char buf[200];
char *substr = NULL;
int count = 0;
int GPIOFLAG = 0, val;
short NEW = 0,OLD = 0;
/*LoRa*/

/*WiFi*/
extern u32 OK_flag;
extern u32 RXRD;
extern u32 RXRD_Index;
extern u8 RXRD_BUF[100];
/*Wifi*/

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
void delay_us(uint32_t delay_us)
{
  volatile unsigned int num;
  volatile unsigned int t;

  for (num = 0; num < delay_us; num++)
  {
    t = 11;
    while (t != 0)
    {
      t--;
    }
  }
}

void delay_ms(uint16_t delay_ms)
{
  volatile unsigned int num;
  for (num = 0; num < delay_ms; num++)
  {
    delay_us(1000);
  }
}

void Usart_Sendbyte(HT_USART_TypeDef* USARTx, u8 data)
{
	USART_SendData(USARTx, data);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);		
}

void Usart_SendStr(HT_USART_TypeDef* USARTx, uint8_t *str)
{
	uint8_t i;
	for(i = 0;str[i] != '\0';i++)
	{
		Usart_Sendbyte(USARTx,str[i]);
	}
}

void Usart_Send_0xXX(HT_USART_TypeDef* USARTx, u8 data)
{
	USART_SendData(USARTx,data);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);		
}

void led_init(void)
{
    CKCU_PeripClockConfig_TypeDef LEDClock = {{ 0 }};
    LEDClock.Bit.PA = 1;
    LEDClock.Bit.PB = 1;
    LEDClock.Bit.PC = 1;   
    LEDClock.Bit.PD = 1;

    LEDClock.Bit.AFIO = 1;
    CKCU_PeripClockConfig(LEDClock, ENABLE);

    GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_6,  GPIO_DIR_OUT);
		GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_14,  GPIO_DIR_OUT);
		//GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_15,  GPIO_DIR_OUT);
		
		//GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_0,  GPIO_DIR_OUT);
		//GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_8,  GPIO_DIR_OUT);
		GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_10,  GPIO_DIR_OUT);
		GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_11,  GPIO_DIR_OUT);
		GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_12,  GPIO_DIR_OUT);
		
		//GPIO_DirectionConfig(HT_GPIOD, GPIO_PIN_1,  GPIO_DIR_OUT);
		//GPIO_DirectionConfig(HT_GPIOD, GPIO_PIN_2,  GPIO_DIR_OUT);
		
}

/*CONVERT*/
void hex2str(u8 hi, char str[])
{
	
	long int temp = 0;
	temp = ((hi & 0xf0) >> 4);
	if (temp >= 0x0A)
		str[0] = temp + '7';
	else
		str[0] = temp + '0'; //+'0' to ascii
	temp = (hi & 0x0f);
	if (temp >= 0x0A)
		str[1] = temp + '7';
	else
		str[1] = temp + '0';
	
	//printf("%c",hi);
}

/*CONVERT*/

/*LoRa*/
int my_atoi(const char *str)
{
	int value = 0;
	int flag = 1;
	while (*str == ' ')
	{
		str++;
	}
	if (*str == '-')
	{
    flag = 0;
    str++;
	}
	else if (*str == '+')
	{
    flag = 1;
    str++;
	}
	else if (*str >= '9' || *str <= '0') 
	{
		return 0;    
	}
	while (*str != '\0' && *str <= '9' && *str >= '0')
	{
		value = value * 10 + *str - '0';
    str++;
	}
	if (flag == 0)
	{
		value = -value;
	}
	return value;
}

/*LoRa*/

/*WiFi*/
int GET_Flag_Rx(u32 flag)
{	
		u32 uartflag = RXRD;
		if(flag == 1 && uartflag ==1)
		{
			RXRD = 0;
			OK_flag = 0;
			RXRD_Index = 0;
			memset(RXRD_BUF, 0, 100);
			return 1;
		}
		return 0;
}

void UART_SEND_AT(char *str)
{
	u8 i;
	for(i = 0; i < strlen(str); i++)
	{
		USART_SendData(COM1_PORT, str[i]);
		while (USART_GetFlagStatus(COM1_PORT, USART_FLAG_TXDE) == RESET);
	}
}

void wifiConnect()
{
	UART_SEND_AT("AT+CWMODE=1\r\n");
	while(!GET_Flag_Rx(OK_flag));/*--1--*/
	//delay_ms(1000);
	UART_SEND_AT("AT+CWJAP_DEF=\"");
	UART_SEND_AT(Wifi_Account);
	UART_SEND_AT("\",\"");
	UART_SEND_AT(Wifi_Password);
	UART_SEND_AT("\"\r\n");
	while(!GET_Flag_Rx(OK_flag));/*--2--*/
	//delay_ms(1000);
}

void tcpLinkCreat()
{
	UART_SEND_AT("AT+CIPSTART=\"TCP\",\"");
	UART_SEND_AT(MQTT_IP);						
	UART_SEND_AT("\",");
	UART_SEND_AT(MQTT_Port);
	UART_SEND_AT("\r\n");
	while(!GET_Flag_Rx(OK_flag));/*--3--*/
	//delay_ms(1000);
}

u16 mqtt_connect_message(u8 *mqtt_message,char *client_id,char *username,char *password)
{
			u16 client_id_length = strlen(client_id);
			u16 username_length = strlen(username);
			u16 password_length = strlen(password);
			u16 packetLen;
			u16 i,baseIndex;

			packetLen = 12 + 2 + client_id_length;
			if(username_length > 0)
				packetLen = packetLen + 2 + username_length;
			if(password_length > 0)
				packetLen = packetLen+ 2 + password_length;

			mqtt_message[0] = 16;				//0x10 // MQTT Message Type CONNECT
			mqtt_message[1] = packetLen - 2;
			baseIndex = 2;
			if(packetLen > 127)
			{
				mqtt_message[2] = 1;			//packetLen/127;    
				baseIndex = 3;
			}
			mqtt_message[baseIndex++] = 0;		// Protocol Name Length MSB    
			mqtt_message[baseIndex++] = 4;		// Protocol Name Length LSB    
			mqtt_message[baseIndex++] = 77;		// ASCII Code for M    
			mqtt_message[baseIndex++] = 81;		// ASCII Code for Q    
			mqtt_message[baseIndex++] = 84;		// ASCII Code for T    
			mqtt_message[baseIndex++] = 84;		// ASCII Code for T    
			mqtt_message[baseIndex++] = 4;		// MQTT Protocol version = 4    
			mqtt_message[baseIndex++] = 2;		// conn flags 
			mqtt_message[baseIndex++] = 60;		// Keep-alive Time Length MSB    
			mqtt_message[baseIndex++] = 60;		// Keep-alive Time Length LSB    
			mqtt_message[baseIndex++] = (0xff00&client_id_length)>>8;// Client ID length MSB    
			mqtt_message[baseIndex++] = 0xff&client_id_length;	// Client ID length LSB    

			// Client ID
			for(i = 0; i < client_id_length; i++)
			{
				mqtt_message[baseIndex + i] = client_id[i];    
			}
			baseIndex = baseIndex+client_id_length;
				
			if(username_length > 0)
			{
				//username    
				mqtt_message[baseIndex++] = (0xff00&username_length)>>8;//username length MSB    
				mqtt_message[baseIndex++] = 0xff&username_length;	//username length LSB    
				for(i = 0; i < username_length ; i++)
				{
					mqtt_message[baseIndex + i] = username[i];    
				}
				baseIndex = baseIndex + username_length;
			}
				
			if(password_length > 0)
			{
				//password    
				mqtt_message[baseIndex++] = (0xff00&password_length)>>8;//password length MSB    
				mqtt_message[baseIndex++] = 0xff&password_length;	//password length LSB    
				for(i = 0; i < password_length ; i++)
				{
					mqtt_message[baseIndex + i] = password[i];    
				}
				baseIndex += password_length; 
			}	

			return baseIndex;    
}

void UART_SEND_NUM(u8 num)
{
			USART_SendData(COM1_PORT, num);
}

void Usart_Send(HT_USART_TypeDef* USARTx, uint8_t data)
{
	USART_SendData(USARTx,data);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);
}

void mqttServerConnect()
{
	mqttLen = mqtt_connect_message(mqtt_msg,"", "", "");
	Usart_SendStr(COM1_PORT,ESP12S_mqtt_send);
	if(mqttLen >= 10)
	{
		UART_SEND_NUM(mqttLen / 10 + '0');
	}
	UART_SEND_NUM(mqttLen % 10 + '0');
	printf("\r\n");
	
	while(!GET_Flag_Rx(OK_flag));/*--4--*/
	//delay_ms(1000);

	for(t = 0; t < mqttLen; t++)
	{
		Usart_Send(COM1_PORT,mqtt_msg[t]); 			
	}
	delay_ms(3000);
}

u16 mqtt_publish_message(u8 *mqtt_message, char * topic, char * message, u8 qos)
{  
			u16 topic_length = strlen(topic);    
			u16 message_length = strlen(message);  
			u16 i,index=0;	
			static u16 id=0;

			mqtt_message[index++] = 48;	//0x30 // MQTT Message Type PUBLISH    
			if(qos)
				mqtt_message[index++] = 2 + topic_length + 2 + message_length;
			else
				mqtt_message[index++] = 2 + topic_length + message_length;   // Remaining length    
			mqtt_message[index++] = (0xff00&topic_length)>>8;
			mqtt_message[index++] = 0xff&topic_length;
				
			// Topic    
			for(i = 0; i < topic_length; i++)
			{
				mqtt_message[index + i] = topic[i];
			}
			index += topic_length;	
			if(qos)
			{
				mqtt_message[index++] = (0xff00&id)>>8;
				mqtt_message[index++] = 0xff&id;
				id++;
			}

			// Message
			for(i = 0; i < message_length; i++)
			{
				mqtt_message[index + i] = message[i];
			}
			index += message_length;	
			return index;
}

void mqttMsgPublish(char *topic, char *msg)
{
			mqttLen = mqtt_publish_message(mqtt_msg, topic, msg, 0);
			Usart_SendStr(COM1_PORT,ESP12S_mqtt_send);
			if(mqttLen >= 10)
			{
					UART_SEND_NUM(mqttLen / 10 + '0');
			}
			UART_SEND_NUM(mqttLen % 10 + '0');
			printf("\r\n");
			delay_ms(1000);/*--5--*/
	
			for(t = 0; t < mqttLen; t++)
			{
					Usart_Send(COM1_PORT,mqtt_msg[t]);
			}
			delay_ms(3000);
}

void mqttConnectStart()
{
	//Wifi_Account = "A01-MegaHouse";
	//Wifi_Password = "USBLab603";
				
	Wifi_Account = "MegaHouse";
	Wifi_Password = "USBLab603";
	
	//Wifi_Account = "DESKTOP-EFQDB79 2141";
	//Wifi_Password = "8y95;4U1";
	
	//Wifi_Account = "maker";
	//Wifi_Password = "056315000";
	
	MQTT_IP = "www.coolerd.net";
	MQTT_Port = "1883";

	wifiConnect();
	delay_ms(1000);
	tcpLinkCreat();
	delay_ms(1000);
	mqttServerConnect();
	delay_ms(1000);
	//MQTT_Msg = "DATA";
	//mqttMsgPublish(MQTT_Topic, MQTT_Msg);
	//while(!GET_Flag_Rx(OK_flag));
}

/*WiFi*/

/*RS485*/
void RS485_Send_01()			//CO2(ppm),TVOC(ug/m3),CH2O(ug/m3),PM2.5(ug/m3),Humidity(RH),temperature(Celsius),PM10(ug/m3)(X)
{
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x03);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x07);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x04);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x08);
}

void RS485_Send_02_1()		//Wind speed(X)
{
	Usart_Send_0xXX(HTCFG_UART_PORT,0x02);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x03);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xF4);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xC4);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x37);
}

void RS485_Send_02_2()		//Wind force
{
	Usart_Send_0xXX(HTCFG_UART_PORT,0x02);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x03);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xF5);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x95);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xF7);
}

void RS485_Send_02_3()		//Wind direction
{
	Usart_Send_0xXX(HTCFG_UART_PORT,0x02);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x03);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xF6);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x65);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xF7);
}

void RS485_Send_02_4()		//Wind direction(degree)(X)
{
	Usart_Send_0xXX(HTCFG_UART_PORT,0x02);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x03);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xF7);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x34);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x37);
}

void RS485_Send_02_5()		//Rainfall
{
	Usart_Send_0xXX(HTCFG_UART_PORT,0x02);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x03);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x02);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xD4);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x41);
}

void RS485_Send_03()			//anion(/cm3)
{
	Usart_Send_0xXX(HTCFG_UART_PORT,0x03);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x03);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x2A);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x00);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x01);
	Usart_Send_0xXX(HTCFG_UART_PORT,0xA4);
	Usart_Send_0xXX(HTCFG_UART_PORT,0x20);
}

void RxReSet()
{
	RXdata = 0;
	RX_Index = 0;
	memset(RX_BUF, 0, 200);
}

void RS485_All()
{
	RS485_Send_01();
	delay_ms(1000);
	hex2str(RX_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[4],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	
	hex2str(RX_BUF[5],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[6],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	
	hex2str(RX_BUF[7],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[8],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	
	hex2str(RX_BUF[9],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[10],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	
	hex2str(RX_BUF[11],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[12],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	
	hex2str(RX_BUF[13],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[14],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	
	if(DistantFlag == 0)
	{
		//if((RX_BUF[13] * 256 + RX_BUF[14]) >= 27157)//26 degree    365
		//if((RX_BUF[13] * 256 + RX_BUF[14]) >= 26796)//25 degree
		if((RX_BUF[13] * 256 + RX_BUF[14]) >= 26133)//23 degree
		//if((RX_BUF[13] * 256 + RX_BUF[14]) >= 25678)//22 degree
		//if((RX_BUF[13] * 256 + RX_BUF[14]) >= 25313)//21 degree
		{
			GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_10,SET);
			GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_11,SET);
			GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_12,SET);
		}
		else
		{
			GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_10,RESET);
			GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_11,RESET);
			GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_12,RESET);
		}
	}
	else
	{
		GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_10,RESET);
		GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_11,RESET);
		GPIO_WriteOutBits(HT_GPIOC,GPIO_PIN_12,RESET);
	}
	
	RxReSet();
	
	RS485_Send_02_2();
	delay_ms(1000);
	hex2str(RX_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[4],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	RxReSet();
		
	RS485_Send_02_3();
	delay_ms(1000);
	hex2str(RX_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[4],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	RxReSet();
			
	RS485_Send_02_5();
	delay_ms(1000);
	hex2str(RX_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[4],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	RxReSet();
	
	RS485_Send_03();
	delay_ms(1000);
	hex2str(RX_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(RX_BUF[4],VALUE);
	strcat(value, VALUE);
	
	if((RX_BUF[3] * 256 + RX_BUF[4]) <= 300)
	{
		GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_14,SET);
	}
	else
	{
		GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_14,RESET);
	}
	strcat(value, comma);
	strcat(value, temp);
	strcat(value, comma);
	strcat(value, temp);
	RxReSet();
	
}

/*RS485*/

/*Solar*/
void soc(void)//4.5
{
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x85);
	Usart_Send_0xXX(COM3_PORT, 0xF6);
}

void read_Battery_voltage(void)//4.6
{
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0xD4);
	Usart_Send_0xXX(COM3_PORT, 0x36);
}

void read_Load_voltage(void)//4.8.1
{
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x04);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x45);
	Usart_Send_0xXX(COM3_PORT, 0xF6);
}

void read_Load_current(void)//4.8.2
{
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x05);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x14);
	Usart_Send_0xXX(COM3_PORT, 0x36);
}

void read_Attached_power(void)//4.8.3
{
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x06);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0xE4);
	Usart_Send_0xXX(COM3_PORT, 0x36);
}

void read_Solar_panel_voltage(void)//4.9.1
{
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x07);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0xB5);
	Usart_Send_0xXX(COM3_PORT, 0xF6);
}

void read_Solar_panel_charging_current(void)//4.9.2
{
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x08);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x85);
	Usart_Send_0xXX(COM3_PORT, 0xF5);
}

void read_Solar_panel_charging_power(void)//4.9.3
{
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0x01);
	Usart_Send_0xXX(COM3_PORT, 0x09);
	Usart_Send_0xXX(COM3_PORT, 0x00);
	Usart_Send_0xXX(COM3_PORT, 0x03);
	Usart_Send_0xXX(COM3_PORT, 0xD4);
	Usart_Send_0xXX(COM3_PORT, 0x35);
}

void SolarReSet()
{
	SOLARdata = 0;
	SOLAR_Index = 0;
	memset(SOLAR_BUF, 0, 200);
}

void Solar_All()
{
	soc();
	delay_ms(1000);
	hex2str(SOLAR_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(SOLAR_BUF[4],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	SolarReSet();
	
	read_Battery_voltage();
	delay_ms(1000);
	hex2str(SOLAR_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(SOLAR_BUF[4],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	SolarReSet();
	
	read_Load_voltage();
	delay_ms(1000);
	hex2str(SOLAR_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(SOLAR_BUF[4],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	SolarReSet();
	
	read_Load_current();
	delay_ms(1000);
	hex2str(SOLAR_BUF[5],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(SOLAR_BUF[6],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	SolarReSet();
	
	read_Attached_power();
	delay_ms(1000);
	hex2str(SOLAR_BUF[7],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(SOLAR_BUF[8],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	SolarReSet();
	
	read_Solar_panel_voltage();
	delay_ms(1000);
	hex2str(SOLAR_BUF[3],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(SOLAR_BUF[4],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	SolarReSet();
	
	read_Solar_panel_charging_current();
	delay_ms(1000);
	hex2str(SOLAR_BUF[5],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(SOLAR_BUF[6],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	SolarReSet();
	
	read_Solar_panel_charging_power();
	delay_ms(1000);
	hex2str(SOLAR_BUF[7],VALUE);
	strcat(value, VALUE);
	strcat(value, comma);
	hex2str(SOLAR_BUF[8],VALUE);
	strcat(value, VALUE);
	SolarReSet();
	
}
/*Solar*/

/**/
void LoRa_output()
{
	char distant[10];
		
	strcpy(buf, LoRa_BUF);
	//printf("original string: %s\n", buf);
		
	substr = strtok(buf, ",");
	do
	{
		//printf("#%d sub string: %s\n",count++, substr);
		count++;
		substr = strtok(NULL, ",");
		if(count == 3)
		{
			strcpy(distant,substr);
		}
	}	while (substr);

	//printf("%s\n",distant);
	val = my_atoi(distant);
	NEW = val;
	if(!(NEW - OLD <= -3||NEW - OLD >= 3))
	{
		if(NEW <= -65)
		{
			GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_6,SET);
			DistantFlag = 1;
			GPIOFLAG = 1;
		}
		else
		{
			GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_6,RESET);
			DistantFlag = 0;
			GPIOFLAG = 0;
		}		
	}
	OLD = NEW ;	
	//printf("%d\n",GPIOFLAG);

	//Reset
	count = 0;
	LoRadata = 0;
	LoRa_Index = 0;
	memset(LoRa_BUF,0,200);

}

void WiFi_output()
{
	strcat(value, DOGHOUSE_CODENAME);
	strcat(value, comma);
	RS485_All();
	delay_ms(1000);	
			
	MQTT_Msg = value;
	MQTT_Topic = "MQTTtest";
	delay_ms(1000);	
	mqttMsgPublish(MQTT_Topic, MQTT_Msg);
	while(!GET_Flag_Rx(OK_flag));
	delay_ms(1000);
		
	//printf("%s",value);
	memset(value, 0, 200);
			
	strcat(value, DOGHOUSE_CODENAME);
	strcat(value, comma);
	Solar_All();
	delay_ms(1000);
	
	MQTT_Msg = value;
	MQTT_Topic = "MQTTsun";
	delay_ms(1000);
	mqttMsgPublish(MQTT_Topic, MQTT_Msg);
	while(!GET_Flag_Rx(OK_flag));
	delay_ms(1000);
		
	//printf("%s",value);
	memset(value, 0, 200);
}
/**/
int main(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  
	HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);

  gURRx_Ptr = gRx_Buffer;

  UxART_Configuration();

  //UxART_TxTest();
	
	led_init();
	delay_ms(1000);
	
	/*WiFi*/
	
	mqttConnectStart();
	delay_ms(1000);
	
	CKCUClock.Bit.BFTM0 = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	NVIC_EnableIRQ(BFTM0_IRQn);
	
	BFTM_SetCompare(HT_BFTM0, SystemCoreClock);
  BFTM_SetCounter(HT_BFTM0, 0);
  BFTM_IntConfig(HT_BFTM0, ENABLE);
  BFTM_EnaCmd(HT_BFTM0, ENABLE);	
  while (1)
  {
    //UxART_RxTest();
		
		/*WiFi*/
		WiFi_output();
		delay_ms(1000);
  }
}

/*************************************************************************************************************
  * @brief  Configure the UxART
  * @retval None
  ***********************************************************************************************************/
void UxART_Configuration(void)
{
  #if 0 // Use following function to configure the IP clock speed.
  // The UxART IP clock speed must be faster 16x then the baudrate.
  CKCU_SetPeripPrescaler(CKCU_PCLK_UxARTn, CKCU_APBCLKPRE_DIV2);
  #endif

  { /* Enable peripheral clock of AFIO, UxART                                                               */
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    CKCUClock.Bit.AFIO                   = 1;
		CKCUClock.Bit.PA              		   = 1;
		CKCUClock.Bit.PB              		   = 1;
		CKCUClock.Bit.USART0								 = 1;
		CKCUClock.Bit.USART1								 = 1;
		CKCUClock.Bit.UART0								 = 1;
		CKCUClock.Bit.UART1								 = 1;
    //CKCUClock.Bit.HTCFG_UART_RX_GPIO_CLK = 1;
    //CKCUClock.Bit.HTCFG_UART_IPN         = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
  }

  /* Turn on UxART Rx internal pull up resistor to prevent unknow state                                     */
  //GPIO_PullResistorConfig(HTCFG_UART_RX_GPIO_PORT, HTCFG_UART_RX_GPIO_PIN, GPIO_PR_UP);

  /* Config AFIO mode as UxART function.                                                                    */
  AFIO_GPxConfig(HTCFG_UART_TX_GPIO_ID, HTCFG_UART_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(HTCFG_UART_RX_GPIO_ID, HTCFG_UART_RX_AFIO_PIN, AFIO_FUN_USART_UART);
	
	AFIO_GPxConfig(COM1_TX_GPIO_ID, COM1_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(COM1_RX_GPIO_ID, COM1_RX_AFIO_PIN, AFIO_FUN_USART_UART);
	
	AFIO_GPxConfig(COM2_TX_GPIO_ID, COM2_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(COM2_RX_GPIO_ID, COM2_RX_AFIO_PIN, AFIO_FUN_USART_UART);
	
	AFIO_GPxConfig(COM3_TX_GPIO_ID, COM3_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(COM3_RX_GPIO_ID, COM3_RX_AFIO_PIN, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 115200 baud
          - Word Length = 8 Bits
          - One Stop Bit
          - None parity bit
    */

    /* !!! NOTICE !!!
       Notice that the local variable (structure) did not have an initial value.
       Please confirm that there are no missing members in the parameter settings below in this function.
    */
    USART_InitTypeDef USART_InitStructure = {0};
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(HTCFG_UART_PORT, &USART_InitStructure);
		
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(COM1_PORT, &USART_InitStructure);
		
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(COM2_PORT, &USART_InitStructure);
		
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
    USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
    USART_InitStructure.USART_Parity = USART_PARITY_NO;
    USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
    USART_Init(COM3_PORT, &USART_InitStructure);
  }

  /* Enable UxART interrupt of NVIC                                                                         */
  NVIC_EnableIRQ(HTCFG_UART_IRQn);
	NVIC_EnableIRQ(COM1_IRQn);
	NVIC_EnableIRQ(COM2_IRQn);
	NVIC_EnableIRQ(COM3_IRQn);

  /* Enable UxART Rx interrupt                                                                              */
  USART_IntConfig(HTCFG_UART_PORT, USART_INT_RXDR, ENABLE);
	USART_IntConfig(COM1_PORT, USART_INT_RXDR, ENABLE);
	USART_IntConfig(COM2_PORT, USART_INT_RXDR, ENABLE);
	USART_IntConfig(COM3_PORT, USART_INT_RXDR, ENABLE);

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HTCFG_UART_PORT, ENABLE);
  USART_RxCmd(HTCFG_UART_PORT, ENABLE);
	
	USART_TxCmd(COM1_PORT, ENABLE);
  USART_RxCmd(COM1_PORT, ENABLE);
	
	USART_TxCmd(COM2_PORT, ENABLE);
  USART_RxCmd(COM2_PORT, ENABLE);
	
	USART_TxCmd(COM3_PORT, ENABLE);
  USART_RxCmd(COM3_PORT, ENABLE);
}

/*********************************************************************************************************//**
  * @brief  UxART Tx Test.
  * @retval None
  ***********************************************************************************************************/
void UxART_TxTest(void)
{
  gIsTxFinished = FALSE;
  gURTx_Ptr = (u8 *)gHelloString;
  gURTx_Length = sizeof(gHelloString) - 1;
  USART_IntConfig(HTCFG_UART_PORT, USART_INT_TXDE | USART_INT_TXC, ENABLE);

  while (gURTx_Length != 0);      // Latest byte move to UxART shift register, but the transmission may be on going.
  while (gIsTxFinished == FALSE); // Set by TXC interrupt, transmission is finished.
}

/*********************************************************************************************************//**
  * @brief  UxART Rx Test.
  * @retval None
  ***********************************************************************************************************/
void UxART_RxTest(void)
{
  u32 i;
  u32 uLength;

  /* Waiting for receive 5 data                                                                             */
  if (gURRx_Length >= 5)
  {
    // Process Rx data by gRx_Buffer[] and gURRx_Length here
    // .....

    uLength = gURRx_Length;
    for (i = 0; i < uLength; i++)
    {
      gTx_Buffer[i] = gRx_Buffer[i];
    }

    #if 1 // Loop back Rx data to Tx for test
    gIsTxFinished = FALSE;
    gURTx_Ptr = gTx_Buffer;
    gURTx_Length = uLength;
    USART_IntConfig(HTCFG_UART_PORT, USART_INT_TXDE | USART_INT_TXC, ENABLE);
    #endif

    gURRx_Length = 0;
  }
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
