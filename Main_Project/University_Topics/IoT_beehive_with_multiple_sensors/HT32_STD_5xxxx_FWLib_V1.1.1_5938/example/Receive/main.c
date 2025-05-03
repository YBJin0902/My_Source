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
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

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
//void mqtt_send(u8 *tempdata, u8 *len);
void gpsinfo(const int lentemp2 , const char *tempt2);
void gpshead(void);
void gpscheck(void);
void gpsint_to_u8(int te);

/* Global variables ----------------------------------------------------------------------------------------*/
uc8  *gURTx_Ptr;
vu32 gURTx_Length = 0;
u8  *gURRx_Ptr;
vu32 gURRx_Length = 0;

uc8  *gURTx_Ptr1;
vu32 gURTx_Length1 = 0;
u8  *gURRx_Ptr1;
vu32 gURRx_Length1 = 0;

u8 rxgps[256];
u8 gpsTempData[128];
char ctemp[128];


int address = 0, ans = 0;

u32 fsr_11 = 0, fsr_21 = 0, fsr_31 = 0, tem1 = 0, times1 = 0, hum1 = 0;
u32 fsr_12 = 0, fsr_22 = 0, fsr_32 = 0, tem2 = 0, times2 = 0, hum2 = 0;
u8 all_len1 = 0, all_len2 = 0, all_len = 0;

extern u32 OK_flag;	
extern u32 OK_flag_lora;
extern u32 RCV_flag_lora;
extern u32 add_flag_1;
extern u32 add_flag_2;

extern u8 lora_data;
extern u8 rx_data;

extern u32 RXRD;		
extern u32 lora_RXRD;
extern u32 gps_RXRD;

extern u32 RXRD_Index;
extern u32 lora_Index;
extern u32 gps_Index;

extern u8 RXRD_BUF[100];
extern u8 lora_BUF[100];
extern u8 gps_BUF[100];

extern int BFTM_FLAG;

u8 Box1_Data[128];
u8 Box2_Data[128];
u8 len_data[128];
u8 len_i2c_Data[128];
char fsr1temp[128];
char fsr2temp[128];
char fsr3temp[128];
char temtemp[128];
char humtemp[128];
char timestemp[128];
char datatemp[512];
vu32 gIsTxFinished = FALSE;
vu32 gIsTxFinished1 = FALSE;
vu32 delaytime;
int time_flag = 0;

void  delay(u32 ntime)
{
	/* Enable the SysTick Counter */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_ENABLE);

	delaytime = ntime;

	while(delaytime != 0);

	/* Disable SysTick Counter */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_DISABLE);
	/* Clear SysTick Counter */
	SYSTICK_CounterCmd(SYSTICK_COUNTER_CLEAR);
}

void TimingDelay(void)
{
  if(delaytime != 0)
  {
    delaytime--;
  }
}
/* LoRa ----------------------------------------------------------------------------------------------------*/

u8 LoRa_AddressSet[]={"AT+ADDRESS=43220\r\n"};
u8 LoRa_NetworkIDSet[]={"AT+NETWORKID=5\r\n"};
u8 LoRa_ParameterSet[]={"AT+PARAMETER=10,7,1,7\r\n"};
u8 LoRa_Box1[]={"AT+SEND=43221,2,ok\r\n"};
u8 LoRa_Box2[]={"AT+SEND=43222,2,ok\r\n"};

/* NB-Iot --------------------------------------------------------------------------------------------------*/

u8 sim7020e_test[]={"AT\r\n"};
u8 sim7020e_run[]={"AT+CGREG?\r\n"};
u8 sim7020e_ACS[]={"AT+CREVHEX=0\r\n"}; 
u8 sim7020e_con[] = {"AT+CMQNEW=\"www.coolerd.net\",\"1883\",12000,100\r\n"};
u8 sim7020e_connect_client[] = {"AT+CMQCON=0,4,\"beebox\",600,0,0\r\n"};
u8 sim7020e_send[] = {"AT+CMQPUB=0,\"beebox\",1,0,0,5,\"Start\"\r\n"};
u8 sim7020e_mqtt_close[] = {"AT+CMQDISCON=0""\r\n"};
u8 sim7020e_mqtt_dissub[]= {"AT+CMQUNSUB=0,\"beebox\"\r\n"};
char My_mqtt_SendData[16];

u8 mqtt_send_at[] = {"AT+CMQPUB=0,"};
u8 mqtt_send_add[] = {"\"beebox\""};
u8 mqtt_send_first[] = {",1,0,0,"};
u8 mqtt_send_box1[] = {"08d2f192"};
u8 mqtt_send_box2[] = {"fe06715c"};
u8 mqtt_send_end[] = {"\r\n"};


/* Private variables ---------------------------------------------------------------------------------------*/
uc8 gHelloString[] = "Hello, this is USART Tx/Rx interrupt example. Please enter 5 characters...\r\n";
u8 gTx_Buffer[128];
u8 gRx_Buffer[128];
/* ---------------------------------------------------------------------------------------------------------*/
/*-----------------am2320 variables-------------------------------------------------------------------------*/
#define I2C_MASTER_ADDRESS  0x0A
#define I2C_AM2320_ADDRESS  0x5C
#define BufferSize          16
#define ClockSpeed          10000

u8 i=1;
uint8_t  data[4];
uint8_t  Data[4];
float T_H[2];
char i2cloradata[30];
u8 Tem_tempdata[30] = {'\0'};
u8 Hum_tempdata[30] = {'\0'};
int i2c_Tem_len = 0;
int i2c_Hum_len = 0;
/*----------------------------------------------------------------------------------------------------------*/
/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/

/*
void Usart_Sendbyte_gps(HT_USART_TypeDef* USARTx, u8 data)
{
	USART_SendData(USARTx, data);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);		
}
*/

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
		Usart_Sendbyte(USARTx, str[i]);
	}
}

/*-------------------------------------------------------------------------------------------------------------*/

void Usart_Sendbyte_nb(HT_USART_TypeDef* USARTx, u8 data)
{
	USART_SendData(USARTx, data);
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);		
}

void Usart_SendStr_nb(HT_USART_TypeDef* USARTx, uint8_t *str)
{
	uint8_t i;
	for(i = 0;str[i] != '\0';i++)
	{
		Usart_Sendbyte_nb(USARTx, str[i]);
	}
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);
}

/* Delay ----------------------------------------------------------------------------------------------------*/
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


int GET_Flag_Rx(u32 flag)
{	
		u32 uartflag = RXRD;

		if(flag == 1 && uartflag ==1)    
		{
			RXRD = 0;
			OK_flag = 0;
			RXRD_Index = 0;
			memset(RXRD_BUF, '\0', 100);			
			return 1;
		}
		return 0;
}
void LoRa_set()
{	
	Usart_SendStr(HTCFG_UART_PORT, LoRa_AddressSet);
	delay_ms(300);
	
	Usart_SendStr(HTCFG_UART_PORT, LoRa_NetworkIDSet);
	delay_ms(300);
	
	Usart_SendStr(HTCFG_UART_PORT, LoRa_ParameterSet);
	delay_ms(300);
	
}
void len_count (int len)
{
	char temp[100];
	int nn = 0;
	memset(len_data, '\0', 100);
	
	sprintf(temp,"%d",len); //int te
	size_t length = strlen(temp);
	
	for(nn = 0; nn<length; nn++)
	{
		len_data[nn] = temp[nn]; //u8 TempData[tsize]
	}
	
}
void mqtt_send_all(u8 *tempdata1, u8 *tempdata2, u8 *len)
{
	int tt;
	
	Usart_SendStr_nb(COM2_PORT, mqtt_send_at);
	//printf("%s", mqtt_send_at);
	Usart_SendStr_nb(COM2_PORT, mqtt_send_add);
	//printf("%s", mqtt_send_add);
	Usart_SendStr_nb(COM2_PORT, mqtt_send_first);
	//printf("%s", mqtt_send_first);
	Usart_SendStr_nb(COM2_PORT, len);//strlen
	Usart_Sendbyte_nb(COM2_PORT, ',');
	//printf("%s", len);
	
	Usart_SendStr_nb(COM2_PORT, mqtt_send_box1); //7
	
	Usart_Sendbyte_nb(COM2_PORT, '-');//1

	for(tt = 0; tt < all_len1; tt++)//19
	{
		Usart_Sendbyte_nb(COM2_PORT, Box1_Data[tt]);
	}
	
	Usart_Sendbyte_nb(COM2_PORT, '-');//1
	
	Usart_SendStr_nb(COM2_PORT, tempdata1);//2
	
	Usart_Sendbyte_nb(COM2_PORT, '-');
	
	Usart_SendStr_nb(COM2_PORT, tempdata2);//2
	
	Usart_Sendbyte_nb(COM2_PORT, '%');//1
	
	Usart_SendStr_nb(COM2_PORT, mqtt_send_box2);//7
	//printf("%s", mqtt_send_box2);
	
	Usart_Sendbyte_nb(COM2_PORT, '-');//1
	
	for(tt = 0; tt < all_len2; tt++)//19
	{
		Usart_Sendbyte_nb(COM2_PORT, Box2_Data[tt]);
	}
	
	Usart_Sendbyte_nb(COM2_PORT, '-');//1
	
	Usart_SendStr_nb(COM2_PORT, tempdata1);//2
	
	Usart_Sendbyte_nb(COM2_PORT, '-');//1
	
	Usart_SendStr_nb(COM2_PORT, tempdata2);//2
	
	Usart_SendStr_nb(COM2_PORT, mqtt_send_end);
}

void NB_IOT_SEND_ALL(void)
{
	memset(RXRD_BUF,'\0', 100); //Clear nbiot String
	all_len = all_len1 + all_len2 + ( i2c_Tem_len + i2c_Hum_len ) * 2 + 21;
	
	len_count(all_len);
	mqtt_send_all(Hum_tempdata, Tem_tempdata, len_data);
	
	delay_ms(5000);
	memset(RXRD_BUF, '\0', 100); //Clear nbiot String
	
	memset(Box1_Data, '\0', 100);
	all_len1 = 0;
	memset(Box2_Data, '\0', 100);
	all_len2 = 0;
}

void NB_Iotset()
{
//	Usart_SendStr_nb(COM2_PORT, sim7020e_mqtt_close);
//	delay_ms(500);
	//while(!GET_Flag_Rx(OK_flag));
	
	Usart_SendStr_nb(COM2_PORT, sim7020e_test);
	delay_ms(500);
	while(!GET_Flag_Rx(OK_flag));
	
	Usart_SendStr_nb(COM2_PORT, sim7020e_run);
	delay_ms(500);
	while(!GET_Flag_Rx(OK_flag));
	
	Usart_SendStr_nb(COM2_PORT, sim7020e_ACS);
	delay_ms(500);
	while(!GET_Flag_Rx(OK_flag));

	Usart_SendStr_nb(COM2_PORT, sim7020e_con);
	delay_ms(700);
	//while(!GET_Flag_Rx(OK_flag));
	
	Usart_SendStr_nb(COM2_PORT, sim7020e_connect_client);
	delay_ms(700);
	//while(!GET_Flag_Rx(OK_flag));

	memset(RXRD_BUF, '\0', 100); //Clear nbiot String

}

int Find_ADDRESS()
{
	int temp = 0, count = 0;
	int address = 0;
	
	while(temp < 1)
	{
		if(lora_BUF[count++] == ',')
		{
			temp++;
		}			
	}

	address = ((int)lora_BUF[count - 6] - 48) * 10000 + 
						((int)lora_BUF[count - 5] - 48) * 1000 + 
						((int)lora_BUF[count - 4] - 48) * 100 + 
						((int)lora_BUF[count - 3] - 48) * 10 + 
						((int)lora_BUF[count - 2] - 48);
									
	return address;
						
}


void Find_DATA_1()
{
	int temp = 0, count = 0, temp_1 = 0, count_1 = 0, ans = 0, 
			m_1 = 0, m_2 = 0, m_3 = 0, m_4 = 0 ,m_5 = 0 ,m_6 = 0, 
			t_1 = 0, t_2 = 0, t_3 = 0, t_4 = 0 ,t_5 = 0 ,t_6 = 0, 
			x1 = 0, x2 = 0, x3 = 0, x4 = 0, x5 = 0, x6 = 0,
			j;
	
	//reset
	all_len1 = 0;	
	memset(Box1_Data, '\0', 100);
	//end
	while(temp < 3)
	{
		if(lora_BUF[count++] == ',')
		{
			temp++;
		}
			
	}
	//head
	while(temp_1 < 2) 
	{
		if(lora_BUF[count_1++] == ',')
		{
			temp_1++;
		}
			
	}
	// times
	while(t_1 < 1)
	{
		if(lora_BUF[m_1++] == '-')
		{
			t_1++;
		}
	}
	//hum
	while(t_2 < 2)
	{
		if(lora_BUF[m_2++] == '-')
		{
			t_2++;
		}
	}
	//tem
	while(t_3 < 3)
	{
		if(lora_BUF[m_3++] == '-')
		{
			t_3++;
		}
	}
	//fsr1
	while(t_4 < 4)
	{
		if(lora_BUF[m_4++] == '-')
		{
			t_4++;
		}
	}
	//fsr2
	while(t_5 < 5)
	{
		if(lora_BUF[m_5++] == '-')
		{
			t_5++;
		}
	}
	//fsr3
	while(t_6 < 6)
	{
		if(lora_BUF[m_6++] == '-')
		{
			t_6++;
		}
	}
	
	x1 = m_1 - count_1 - 1;
	x2 = m_2 - m_1 - 1;
	x3 = m_3 - m_2 - 1;
	x4 = m_4 - m_3 - 1;
	x5 = m_5 - m_4 - 1;
	x6 = m_6 - m_5 - 1;		
	
	//tem
	for(j = count_1; j <= m_1 - 2; j++)
	{		
		Box1_Data[all_len1] = lora_BUF[j];
		all_len1++;
	}
	Box1_Data[all_len1] = '-';
	all_len1++;
	
	//hum
	for(j = m_1; j <= m_2 - 2; j++)
	{
		Box1_Data[all_len1] = lora_BUF[j];
		all_len1++;
	}
	Box1_Data[all_len1] = '-';
	all_len1++;
	
	//times
	for(j = m_2; j <= m_3 - 2; j++)
	{
		Box1_Data[all_len1] = lora_BUF[j];
		all_len1++;
	}
	Box1_Data[all_len1] = '-';
	all_len1++;
	
	//fsr1
	for(j = m_3; j <= m_4 - 2; j++)
	{
		Box1_Data[all_len1] = lora_BUF[j];
		all_len1++;
	}
	Box1_Data[all_len1] = '-';
	all_len1++;
	
	//fsr2
	for(j = m_4; j <= m_5 - 2; j++)
	{
		Box1_Data[all_len1] = lora_BUF[j];
		all_len1++;
	}
	Box1_Data[all_len1] = '-';
	all_len1++;
	
	//fsr3
	for(j = m_5; j <= m_6 - 2; j++)
	{
		Box1_Data[all_len1] = lora_BUF[j];
		all_len1++;
	}
	memset(lora_BUF, '\0', 100); //Clear lora String
}


void Find_DATA_2()
{
	int temp = 0, count = 0, temp_1 = 0, count_1 = 0, ans = 0, 
			m_1 = 0, m_2 = 0, m_3 = 0, m_4 = 0 ,m_5 = 0 ,m_6 = 0, 
			t_1 = 0, t_2 = 0, t_3 = 0, t_4 = 0 ,t_5 = 0 ,t_6 = 0, 
			x1 = 0, x2 = 0, x3 = 0, x4 = 0, x5 = 0, x6 = 0,
			j;
	
	//reset
	all_len2 = 0;	
	memset(Box2_Data, '\0', 100);
	//end
	while(temp < 3)
	{
		if(lora_BUF[count++] == ',')
		{
			temp++;
		}
			
	}
	//head
	while(temp_1 < 2) 
	{
		if(lora_BUF[count_1++] == ',')
		{
			temp_1++;
		}
	}
	//hum
	while(t_1 < 1)
	{
		if(lora_BUF[m_1++] == '-')
		{
			t_1++;
		}
	}
	//tem
	while(t_2 < 2)
	{
		if(lora_BUF[m_2++] == '-')
		{
			t_2++;
		}
	}
	//times
	while(t_3 < 3)
	{
		if(lora_BUF[m_3++] == '-')
		{
			t_3++;
		}
	}
	//fsr1
	while(t_4 < 4)
	{
		if(lora_BUF[m_4++] == '-')
		{
			t_4++;
		}
	}
	//fsr2
	while(t_5 < 5)
	{
		if(lora_BUF[m_5++] == '-')
		{
			t_5++;
		}
	}
	//fsr3
	while(t_6 < 6)
	{
		if(lora_BUF[m_6++] == '-')
		{
			t_6++;
		}
	}
	
	//count len
	x1 = m_1 - count_1 - 1;
	x2 = m_2 - m_1 - 1;
	x3 = m_3 - m_2 - 1;
	x4 = m_4 - m_3 - 1;
	x5 = m_5 - m_4 - 1;
	x6 = m_6 - m_5 - 1;
	
	//tem
	for(j = count_1; j <= m_1 - 2; j++)
	{		
		Box2_Data[all_len2] = lora_BUF[j];
		all_len2++;
	}
	Box2_Data[all_len2] = '-';
	all_len2++;
	
	//hum
	for(j = m_1; j <= m_2 - 2; j++)
	{
		Box2_Data[all_len2] = lora_BUF[j];
		all_len2++;
	}
	Box2_Data[all_len2] = '-';
	all_len2++;
	
	//times
	for(j = m_2; j <= m_3 - 2; j++)
	{
		Box2_Data[all_len2] = lora_BUF[j];
		all_len2++;
	}
	Box2_Data[all_len2] = '-';
	all_len2++;
	
	//fsr1
	for(j = m_3; j <= m_4 - 2; j++)
	{
		Box2_Data[all_len2] = lora_BUF[j];
		all_len2++;
	}
	Box2_Data[all_len2] = '-';
	all_len2++;
	
	//fsr2
	for(j = m_4; j <= m_5 - 2; j++)
	{
		Box2_Data[all_len2] = lora_BUF[j];
		all_len2++;
	}
	Box2_Data[all_len2] = '-';
	all_len2++;
	
	//fsr3
	for(j = m_5; j <= m_6 - 2; j++)
	{
		Box2_Data[all_len2] = lora_BUF[j];
		all_len2++;
	}
	memset(lora_BUF, '\0', 100); //Clear lora String
}
void LoRa_box(void)
{
	 int temp = 0, add = 0;
	 int i;
	 
	 add = Find_ADDRESS();

	 if(add == 43221)
	 {
		Find_DATA_1();
		temp = 1; 
	 }
	 else if(add == 43222)
	 {
		Find_DATA_2(); 
		temp = 2;
	 }
	 /*
	 if(temp == 2 || temp == 1)
	 {
			memset(lora_BUF, '\0', 100); //Clear lora String
			lora_Index = 0;
			add = 0;
	 }
	 */
	memset(lora_BUF, '\0', 100); //Clear lora String
	lora_Index = 0;
	add = 0;

}

void LoRa_send_Box1(void)
{
	Usart_SendStr(HTCFG_UART_PORT,  LoRa_Box1);
	delay_ms(300);
	memset(lora_BUF, '\0', 100); //Clear lora String
}

void LoRa_send_Box2(void)
{
	Usart_SendStr(HTCFG_UART_PORT,  LoRa_Box2);
	delay_ms(300);
	memset(lora_BUF, '\0', 100); //Clear lora String
}

/*
void gpscheck(void)
{
	u32 Len;
	char *tempt2;
	char ugps[] = "$GPRMC";
	u8 ps[10];
	bool rxget = true;
		//UARTM_WriteByte(UARTM_CH0, 'B');
		

	while(1){

		Usart_Sendbyte_gps(HTCFG_UART_PORT,'A');
		printf("gps\n");
		if(rxget){
			while (gps_RXRD == RESET);
			Usart_Sendbyte_gps(COM3_PORT,'A');
			rxgps[0] = USART_ReceiveData(COM3_PORT);
			Usart_Sendbyte_gps(COM3_PORT,rxgps[0]);
			
			if(rxgps[0] == '$')gpshead();
			if(rxgps[3] == 'R'){
				for(Len = 6 ;Len < 80;Len++){
					while (USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDNE) == RESET);
					rxgps[Len] = USART_ReceiveData(COM3_PORT);
					Usart_Sendbyte_gps(COM3_PORT,rxgps[Len]);
				}
			}
			if((tempt2 = strstr((char*)rxgps,ugps))){
				//printf("tempt2 = %c\n",tempt2[count]);
				int lentemp2 = strlen(tempt2);
				//printf("lentemp2 = %d\n",lentemp2);
				gpsinfo(lentemp2,tempt2);
				//printf("132\n");
				//UARTM_Write(UARTM_CH0, Test, 5);		
				memset(rxgps,'\0',255);		
				rxget = false;				
			}
		}
		if(!rxget)break;
	}
}

void gpsinfo(const int lentemp2 , const char *tempt2)
{
	int count , cma = 0 , templen[20] = {0};
	for(count = 0;count<lentemp2;count++){
		bool ngpsdata = true;
		if(tempt2[count] == ','){
			templen[cma] = count;
			//printf("templen[%d] = %d\n",cma,count);
			cma++;
			switch(cma){
				case 2:{
					int lencma,itempT,utc = 8;
					char ctempT[20];
					printf("time:");
					for(lencma = templen[0]+1;lencma < templen[1];lencma++){
						ctempT[lencma - (templen[0]+1)] = tempt2[lencma];
					}
					itempT = atoi(ctempT) + (utc * 10000);
					gpsint_to_u8(itempT);
					Usart_SendStr(COM3_PORT, gpsTempData);
					//printf("%d",itempT);
					printf("\n");
					}break;
				case 3:{
					char ctempT[20];
					ctempT[0] = tempt2[templen[1]+1];
					//printf("%c",ctempT[0]);
					if(ctempT[0] != 'A')ngpsdata = false;
				}break;
				case 4:{	//N
					int lencma,itempN,fcount = 1,icount = 0;
					char ctempN[20];
					float ftempN;
					printf("%c:",tempt2[templen[3]+1]);
					for(lencma = templen[2]+1;lencma < templen[3];lencma++){
						ctempN[lencma - (templen[2]+1)] = tempt2[lencma];
						if(tempt2[lencma] == '.' || fcount > 9){
							fcount *= 10;
						}
					}
					ftempN = atof(ctempN);
					itempN = ftempN / 100;
					ftempN = ((((ftempN/100) - itempN)/60)*100)+itempN;// ftempN = 2342.12541,itempN = 23
					itempN = ftempN * fcount * 10;
					gpsint_to_u8(itempN);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[0]);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[1]);
					if(gpsTempData[icount]){
						Usart_Sendbyte_gps(COM3_PORT,'.');
						for(icount = 2;gpsTempData[icount]!=NULL;icount++){
						Usart_Sendbyte_gps(COM3_PORT,gpsTempData[icount]);
						}
					}
					
					printf("\n");
					}break;
				case 6:{	//E
					int lencma,itempE,fcount = 1,icount = 0;
					char ctempE[20];
					float ftempE;
					printf("%c:",tempt2[templen[5]+1]);
					for(lencma = templen[4]+1;lencma < templen[5];lencma++){
						ctempE[lencma - (templen[4]+1)] = tempt2[lencma];
						if(tempt2[lencma] == '.' || fcount > 9){
							fcount *= 10;
						}
					}
					ftempE = atof(ctempE);
					itempE = ftempE / 100;
					ftempE = ((((ftempE/100) - itempE)/60)*100)+itempE;
					itempE = ftempE * fcount*10;
					gpsint_to_u8(itempE);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[0]);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[1]);
					Usart_Sendbyte_gps(COM3_PORT,gpsTempData[2]);
					if(gpsTempData[icount]){
						Usart_Sendbyte_gps(COM3_PORT,'.');
						for(icount = 3;gpsTempData[icount]!=NULL;icount++){
							Usart_Sendbyte_gps(COM3_PORT,gpsTempData[icount]);
						}
					}
					//printf("%f",ftempE);
					printf("\n");
					}break;
				default:break;
			}
		}
		if(ngpsdata == 0)break;
	}
		printf("break\n");
}

void gpshead()
{
	int n;
	for(n = 1 ;n < 6;n++){
				while (USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDNE) == RESET);
				rxgps[n] = USART_ReceiveData(COM3_PORT);
				//Usart_Sendbyte_gps(COM3_PORT,rxgps[n]);
			}
	//Usart_SendStr(COM3_PORT, rxgps);
	//Usart_Sendbyte_gps(COM3_PORT,'\n');
}

void gpsint_to_u8 (int te)
{
	memset(gpsTempData,'\0',128);
	int nn = 0;
	sprintf(ctemp,"%d",te); //int te
	size_t length = strlen(ctemp);
	for(nn = 0;nn<length;nn++)gpsTempData[nn] = ctemp[nn]; //u8 gpsTempData[tsize]
}

*/


/*----------AM2320------------------------------------------------------------------------------------------*/
void CLOCKconfig(void)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	
	CKCUClock.Bit.EXTI 							= 1;
  HTCFG_I2C_MASTER_CLK(CKCUClock) = 1;
  CKCUClock.Bit.AFIO              = 1;
	CKCUClock.Bit.PA             	  = 1;
	CKCUClock.Bit.PD 								= 1;
	CKCUClock.Bit.PB 								= 1;
	
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	CKCU_SetADCPrescaler(CKCU_ADCPRE_DIV32);
}

void initGPIO(void)
{
	AFIO_GPxConfig(HTCFG_I2C_MASTER_SCL_GPIO_ID, HTCFG_I2C_MASTER_SCL_AFIO_PIN, AFIO_MODE_1);
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SDA_GPIO_ID, HTCFG_I2C_MASTER_SDA_AFIO_PIN, AFIO_MODE_1);

	GPIO_DirectionConfig(HT_GPIOA,HTCFG_I2C_MASTER_SCL_AFIO_PIN,GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOA,HTCFG_I2C_MASTER_SDA_AFIO_PIN,GPIO_DIR_OUT);


	GPIO_WriteOutBits(HT_GPIOA,HTCFG_I2C_MASTER_SCL_AFIO_PIN,RESET);
	GPIO_WriteOutBits(HT_GPIOA,HTCFG_I2C_MASTER_SDA_AFIO_PIN,RESET);
	GPIO_WriteOutBits(HT_GPIOA,HTCFG_I2C_MASTER_SCL_AFIO_PIN,SET);
	GPIO_WriteOutBits(HT_GPIOA,HTCFG_I2C_MASTER_SDA_AFIO_PIN,SET);
	
	AFIO_GPxConfig(HTCFG_I2C_MASTER_SCL_GPIO_ID, HTCFG_I2C_MASTER_SCL_AFIO_PIN, AFIO_MODE_DEFAULT);
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SDA_GPIO_ID, HTCFG_I2C_MASTER_SDA_AFIO_PIN, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOA,GPIO_PIN_0,GPIO_DIR_OUT);
	GPIO_DirectionConfig(HT_GPIOA,GPIO_PIN_1,GPIO_DIR_OUT);
	GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_0,SET);
	GPIO_WriteOutBits(HT_GPIOA,GPIO_PIN_1,SET);
}

void I2C_Configuration(void)
{
  /* Configure GPIO to I2C mode for Master                                                                  */
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SCL_GPIO_ID, HTCFG_I2C_MASTER_SCL_AFIO_PIN, AFIO_FUN_I2C);
  AFIO_GPxConfig(HTCFG_I2C_MASTER_SDA_GPIO_ID, HTCFG_I2C_MASTER_SDA_AFIO_PIN, AFIO_FUN_I2C);
  
  { /* I2C Master configuration                                                                             */
    I2C_InitTypeDef  I2C_InitStructure;

    I2C_InitStructure.I2C_GeneralCall = DISABLE;
    I2C_InitStructure.I2C_AddressingMode = I2C_ADDRESSING_7BIT;
    I2C_InitStructure.I2C_Acknowledge = DISABLE;
    I2C_InitStructure.I2C_OwnAddress = I2C_MASTER_ADDRESS;
    I2C_InitStructure.I2C_Speed = ClockSpeed;
    I2C_InitStructure.I2C_SpeedOffset = 0;
    I2C_Init(HTCFG_I2C_MASTER_PORT, &I2C_InitStructure);
		I2C_IntConfig(HTCFG_I2C_MASTER_PORT, I2C_INT_STA | I2C_INT_ADRS | I2C_INT_RXDNE | I2C_INT_TXDE, ENABLE);
  }
  /* Enable I2C                                                                                            */
  I2C_Cmd(HTCFG_I2C_MASTER_PORT, ENABLE);
}

void I2C_Wake(void)
{
	I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2C_AM2320_ADDRESS, I2C_MASTER_WRITE);
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_SEND_START));
	//while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TRANSMITTER_MODE));
	
	delay_ms(1);
	
	I2C_GenerateSTOP(HTCFG_I2C_MASTER_PORT);	
}

void I2C_Write(void)
{
	I2C_ClearFlag(HTCFG_I2C_MASTER_PORT,I2C_FLAG_RXNACK);
	I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2C_AM2320_ADDRESS, I2C_MASTER_WRITE);
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_SEND_START));
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TRANSMITTER_MODE));
	
	I2C_SendData(HTCFG_I2C_MASTER_PORT,0x03);
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TX_EMPTY));
	I2C_SendData(HTCFG_I2C_MASTER_PORT,0x00);
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TX_EMPTY));
	I2C_SendData(HTCFG_I2C_MASTER_PORT,0x04);
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_TX_EMPTY));
	
	I2C_GenerateSTOP(HTCFG_I2C_MASTER_PORT);
	delay_ms(2);
	I2C_AckCmd(HTCFG_I2C_MASTER_PORT,ENABLE);
}

void I2C_Read(void)
{
	I2C_TargetAddressConfig(HTCFG_I2C_MASTER_PORT, I2C_AM2320_ADDRESS, I2C_MASTER_READ);
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RECEIVER_MODE));
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RX_NOT_EMPTY));
	
	I2C_ReceiveData(HTCFG_I2C_MASTER_PORT);
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RX_NOT_EMPTY));
	I2C_ReceiveData(HTCFG_I2C_MASTER_PORT);
	while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RX_NOT_EMPTY));
	
	for(i=0;i<4;i++)
	{
		while (!I2C_CheckStatus(HTCFG_I2C_MASTER_PORT, I2C_MASTER_RX_NOT_EMPTY));
		Data[i] = I2C_ReceiveData(HTCFG_I2C_MASTER_PORT);
		
		if(i==2)I2C_AckCmd(HTCFG_I2C_MASTER_PORT,DISABLE);
	}
	
	T_H[0] = ((Data[0]<<8)|Data[1])/10;
	T_H[1] = ((Data[2]<<8)|Data[3])/10;
	
	I2C_GenerateSTOP(HTCFG_I2C_MASTER_PORT);
	while (I2C_ReadRegister(HTCFG_I2C_MASTER_PORT, I2C_REGISTER_SR)&0x80000);
}
void change_Tem_float_to_u8(float i2cf)
{
	char i2ctemp[30];
	memset(Tem_tempdata, '\0', 30);
	int nn = 0;
	i2c_Tem_len = 0;
	
	sprintf(i2ctemp, "%.0f", i2cf); //int te

	size_t length = strlen(i2ctemp);
	
	for(nn = 0; nn<length; nn++)
	{
		Tem_tempdata[nn] = i2ctemp[nn]; //u8 TempData[tsize]
		//printf("%c", Tem_tempdata[nn]);
	}
	
	i2c_Tem_len = nn;
	delay_ms(300);
}

void change_Hum_float_to_u8(float i2cf)
{
	char i2ctemp[30];
	memset(Hum_tempdata, '\0', 30);
	int nn = 0;
	i2c_Hum_len = 0;
	
	sprintf(i2ctemp, "%.0f", i2cf); //int te

	size_t length = strlen(i2ctemp);
	
	for(nn = 0; nn<length; nn++)
	{
		Hum_tempdata[nn] = i2ctemp[nn]; //u8 TempData[tsize]
		//printf("%c", Hum_tempdata[nn]);
	}
	
	i2c_Hum_len = nn;
	delay_ms(300);
}

void AM2320(void)
{
	I2C_Wake();
	delay_ms(10);
	I2C_Write();
	I2C_Read();
	//printf("Humidity: %f\n",T_H[0]);
	change_Hum_float_to_u8(T_H[0]); //hum
	//printf("Temperature: %f\n",T_H[1]);
	change_Tem_float_to_u8(T_H[1]); //tem
	delay_ms(2000);
}


void gpsinfo(const int lentemp2 , const char *tempt2)
{
	int count , cma = 0 , templen[20] = {0};
	for(count = 0;count<lentemp2;count++){
		bool ngpsdata = true;
		if(tempt2[count] == ','){
			templen[cma] = count;
			//printf("templen[%d] = %d\n",cma,count);
			cma++;
			switch(cma){
				case 2:{
					int lencma,itempT,utc = 8;
					char ctempT[20];
					printf("time:");
					for(lencma = templen[0]+1;lencma < templen[1];lencma++){
						ctempT[lencma - (templen[0]+1)] = tempt2[lencma];
					}
					itempT = atoi(ctempT) + (utc * 10000);
					gpsint_to_u8(itempT);
					Usart_SendStr(COM3_PORT, gpsTempData);
					//printf("%d",itempT);
					printf("\n");
					}break;
				case 3:{
					char ctempT[20];
					ctempT[0] = tempt2[templen[1]+1];
					//printf("%c",ctempT[0]);
					if(ctempT[0] != 'A')ngpsdata = false;
				}break;
				case 4:{	//N
					int lencma,itempN,fcount = 1,icount = 0;
					char ctempN[20];
					float ftempN;
					printf("%c:",tempt2[templen[3]+1]);
					for(lencma = templen[2]+1;lencma < templen[3];lencma++){
						ctempN[lencma - (templen[2]+1)] = tempt2[lencma];
						if(tempt2[lencma] == '.' || fcount > 9){
							fcount *= 10;
						}
					}
					ftempN = atof(ctempN);
					itempN = ftempN / 100;
					ftempN = ((((ftempN/100) - itempN)/60)*100)+itempN;// ftempN = 2342.12541,itempN = 23
					itempN = ftempN * fcount * 10;
					gpsint_to_u8(itempN);
					Usart_Sendbyte(COM3_PORT,gpsTempData[0]);
					Usart_Sendbyte(COM3_PORT,gpsTempData[1]);
					if(gpsTempData[icount]){
						Usart_Sendbyte(COM3_PORT,'.');
						for(icount = 2;gpsTempData[icount]!=NULL;icount++){
						Usart_Sendbyte(COM3_PORT,gpsTempData[icount]);
						}
					}
					
					printf("\n");
					}break;
				case 6:{	//E
					int lencma,itempE,fcount = 1,icount = 0;
					char ctempE[20];
					float ftempE;
					printf("%c:",tempt2[templen[5]+1]);
					for(lencma = templen[4]+1;lencma < templen[5];lencma++){
						ctempE[lencma - (templen[4]+1)] = tempt2[lencma];
						if(tempt2[lencma] == '.' || fcount > 9){
							fcount *= 10;
						}
					}
					ftempE = atof(ctempE);
					itempE = ftempE / 100;
					ftempE = ((((ftempE/100) - itempE)/60)*100)+itempE;
					itempE = ftempE * fcount*10;
					gpsint_to_u8(itempE);
					Usart_Sendbyte(COM3_PORT,gpsTempData[0]);
					Usart_Sendbyte(COM3_PORT,gpsTempData[1]);
					Usart_Sendbyte(COM3_PORT,gpsTempData[2]);
					if(gpsTempData[icount]){
						Usart_Sendbyte(COM3_PORT,'.');
						for(icount = 3;gpsTempData[icount]!=NULL;icount++){
							Usart_Sendbyte(COM3_PORT,gpsTempData[icount]);
						}
					}
					//printf("%f",ftempE);
					printf("\n");
					}break;
				default:break;
			}
		}
		if(ngpsdata == 0)break;
	}
		printf("break\n");
}

void gpshead()
{
	int n;
	for(n = 1 ;n < 6;n++)
	{
		while (USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDNE) == RESET);
		rxgps[n] = USART_ReceiveData(COM3_PORT);
		Usart_Sendbyte(COM3_PORT,rxgps[n]);
	}
	Usart_SendStr(COM3_PORT, rxgps);
	Usart_Sendbyte(COM3_PORT,'\n');
}

void gpsint_to_u8 (int te)
{
	memset(gpsTempData,'\0',128);
	int nn = 0;
	sprintf(ctemp,"%d",te); //int te
	size_t length = strlen(ctemp);
	for(nn = 0;nn<length;nn++)gpsTempData[nn] = ctemp[nn]; //u8 gpsTempData[tsize]
}

void gpscheck(void)
{
	u32 Len, count;
	char *tempt2;
	char ugps[] = "$GPRMC";
	u8 ps[10];
	bool rxget = true;
		//UARTM_WriteByte(UARTM_CH0, 'B');
	while(1)
	{
		Usart_Sendbyte(HTCFG_UART_PORT,'A');
		printf("gps\n");
		if(rxget)
		{
			//while (gps_RXRD == RESET);
			//Usart_Sendbyte(COM3_PORT,'A');
			rxgps[0] = USART_ReceiveData(COM3_PORT);
			Usart_Sendbyte(COM3_PORT,rxgps[0]);
			
			if(rxgps[0] == '$')gpshead();
			if(rxgps[3] == 'R')
			{
				for(Len = 6 ;Len < 80;Len++)
				{
					while (USART_GetFlagStatus(COM3_PORT, USART_FLAG_RXDNE) == RESET);
					rxgps[Len] = USART_ReceiveData(COM3_PORT);
					Usart_Sendbyte(COM3_PORT,rxgps[Len]);
				}
			}
			if((tempt2 = strstr((char*)rxgps,ugps)))
			{
				printf("tempt2 = %c\n",tempt2[count]);
				int lentemp2 = strlen(tempt2);
				printf("lentemp2 = %d\n",lentemp2);
				gpsinfo(lentemp2,tempt2);
				printf("132\n");
				//UARTM_Write(UARTM_CH0, Test, 5);		
				memset(rxgps,'\0',255);		
				rxget = false;			
			}
		}
		if(!rxget)break;
	}
}


/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);

  UxART_Configuration();
	
	delay_ms(10);
	CLOCKconfig();
	initGPIO();
	//printf("\rSTART\n");
	I2C_Configuration(); 
	delay_ms(10);
	//printf("\r%0x\n",HTCFG_I2C_MASTER_PORT->SR);
	delay_ms(3000);
	
	//gpscheck();
	
	LoRa_set();	
	delay_ms(300);
	NB_Iotset();
	delay_ms(500);
	
	//Clear lora String
	memset(lora_BUF, '\0', 100); 
	lora_Index = 0;
	lora_data = 0;
	lora_RXRD = 0;		
	
	//Clear NB-IOT String
	memset(RXRD_BUF, '\0', 100); 
	RXRD_Index = 0;
	rx_data = 0;
	RXRD = 0;		
		
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	
	CKCUClock.Bit.BFTM0 = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  NVIC_EnableIRQ(BFTM0_IRQn);
	BFTM_SetCompare(HT_BFTM0, SystemCoreClock);
  BFTM_SetCounter(HT_BFTM0, 0); 
  BFTM_IntConfig(HT_BFTM0, ENABLE);
  BFTM_EnaCmd(HT_BFTM0, ENABLE);
	
	/*---------------------------------------------------------------------------------------------*/	
	while(1);
	/*--------------------------------------------------------------------------------------------*/
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
  AFIO_GPxConfig(COM1_TX_GPIO_ID, COM1_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(COM1_RX_GPIO_ID, COM1_RX_AFIO_PIN, AFIO_FUN_USART_UART);
	
	AFIO_GPxConfig(COM2_TX_GPIO_ID, COM2_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(COM2_RX_GPIO_ID, COM2_RX_AFIO_PIN, AFIO_FUN_USART_UART);

	AFIO_GPxConfig(COM3_TX_GPIO_ID, COM3_TX_AFIO_PIN, AFIO_FUN_USART_UART);
  AFIO_GPxConfig(COM3_RX_GPIO_ID, COM3_RX_AFIO_PIN, AFIO_FUN_USART_UART);

  {
    /* UxART configured as follow:
          - BaudRate = 9600 baud
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
    USART_Init(COM1_PORT, &USART_InitStructure);
		
		USART_InitStructure.USART_BaudRate = 9600;
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
  NVIC_EnableIRQ(COM1_IRQn);
	NVIC_EnableIRQ(COM2_IRQn);
	NVIC_EnableIRQ(COM3_IRQn);
	
  /* Enable UxART Rx interrupt                                                                              */
  USART_IntConfig(COM1_PORT, USART_INT_RXDR, ENABLE);
	USART_IntConfig(COM2_PORT, USART_INT_RXDR, ENABLE);
	USART_IntConfig(COM3_PORT, USART_INT_RXDR, ENABLE);
	
  /* Enable UxART Tx and Rx function                                                                        */
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
  USART_IntConfig(COM1_PORT, USART_INT_TXDE | USART_INT_TXC, ENABLE);

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
    USART_IntConfig(COM1_PORT, USART_INT_TXDE | USART_INT_TXC, ENABLE);
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
