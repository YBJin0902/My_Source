/*********************************************************************************************************//**
 * @file    lora_send/Interrupt/main.c
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

extern int BFTM_FLAG;
extern u32 lora_RXRD;
extern u32 lora_Index;
extern u8 lora_BUF[100];
int add = 0;

vu32 gIsTxFinished = FALSE;

vu32 delaytime;

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
//u8 LoRa_OK[]={"AT\r\n"};
u8 LoRa_AddressSet[]={"AT+ADDRESS=43222\r\n"};
u8 LoRa_NetworkIDSet[]={"AT+NETWORKID=5\r\n"};
u8 LoRa_ParameterSet[]={"AT+PARAMETER=10,7,1,7\r\n"};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

u8 LoRa_Send_Data[]={"AT+SEND=43220,50,10-10-2-100-100-100-\r\n"};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* LoRa ----------------------------------------------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------------------------------------*/
uc8 gHelloString[] = "Hello, this is USART Tx/Rx interrupt example. Please enter 5 characters...\r\n";
u8 gTx_Buffer[128];
u8 gRx_Buffer[128];

/* Global functions ----------------------------------------------------------------------------------------*/
/*********************************************************************************************************//**
  * @brief  Main program.
  * @retval None
  ***********************************************************************************************************/
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
void LoRa_set()
{
	Usart_SendStr(HTCFG_UART_PORT,LoRa_AddressSet);
	delay_ms(300);
	
	Usart_SendStr(HTCFG_UART_PORT,LoRa_NetworkIDSet);
	delay_ms(300);
	
	Usart_SendStr(HTCFG_UART_PORT,LoRa_ParameterSet);
	delay_ms(300);
	
}

int Find_ADDRESS()
{
	int temp = 0, count = 0, address = 0;
	
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

void lora_out(void)
{
	add = Find_ADDRESS();
	
	if(add == 43220)
	{
		Usart_SendStr(HTCFG_UART_PORT, LoRa_Send_Data);
		delay_ms(500);
		add = 0;
		memset(lora_BUF, '\0', 100); //Clear lora String
		lora_Index = 0;
	}
}

void lora_send_data(void)
{
	Usart_SendStr(HTCFG_UART_PORT, LoRa_Send_Data);
	delay_ms(500);
}

int main(void)
{
  HT32F_DVB_LEDInit(HT_LED1);
  HT32F_DVB_LEDInit(HT_LED2);
  UxART_Configuration();
	
	LoRa_set();
	delay_ms(300);
	memset(lora_BUF, '\0', 100); //Clear lora String  
	
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.BFTM0 = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  
  NVIC_EnableIRQ(BFTM0_IRQn);
	
	BFTM_SetCompare(HT_BFTM0, SystemCoreClock);
  BFTM_SetCounter(HT_BFTM0, 0);
  BFTM_IntConfig(HT_BFTM0, ENABLE);
  BFTM_EnaCmd(HT_BFTM0, ENABLE);	
	
	while(1);
	
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
  }

  /* Enable UxART interrupt of NVIC                                                                         */
  NVIC_EnableIRQ(HTCFG_UART_IRQn);

  /* Enable UxART Rx interrupt                                                                              */
  USART_IntConfig(HTCFG_UART_PORT, USART_INT_RXDR, ENABLE);

  /* Enable UxART Tx and Rx function                                                                        */
  USART_TxCmd(HTCFG_UART_PORT, ENABLE);
  USART_RxCmd(HTCFG_UART_PORT, ENABLE);
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
