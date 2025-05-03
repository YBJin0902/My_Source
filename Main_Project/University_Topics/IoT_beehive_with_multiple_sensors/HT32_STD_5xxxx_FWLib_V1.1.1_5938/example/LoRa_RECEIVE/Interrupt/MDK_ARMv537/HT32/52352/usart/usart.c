#include "usart.h"
#include "ht32f5xxxx_gpio.h"


void USART_Configuration(void) //Basic set
{
	USART_InitTypeDef USART_InitStructure;
	
	AFIO_GPxConfig(USART_GPIO_GROUP, USART_TX_PIN, AFIO_FUN_USART_UART);
	AFIO_GPxConfig(USART_GPIO_GROUP, USART_TX_PIN, AFIO_FUN_USART_UART);
	
	//set
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
	USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
	USART_InitStructure.USART_Parity = USART_PARITY_NO;
	USART_InitStructure.USART_Mode = USART_MODE_NORMAL;	
	USART_Init(COM1_PORT, &USART_InitStructure);
	
	
	//let com1_port can do sand and resive
	USART_TxCmd(COM1_PORT, ENABLE);
	USART_RxCmd(COM1_PORT, ENABLE);
	
	
	//stop contect!!
	NVIC_EnableIRQ(COM1_IRQn);
	
	USART_IntConfig(COM1_PORT, USART_FLAG_RXDR, ENABLE);
	USART_IntConfig(COM1_PORT, USART_FLAG_TXDE, ENABLE);
	
	//set FIFO Level
	USART_RXTLConfig(COM1_PORT, USART_RXTL_04);
	
}

void COM1_IRQHandler(void) //receive diss contect
{
	u8 data;
	
	if(USART_GetFlagStatus(COM1_PORT, USART_FLAG_RXDR))
	{
		data = USART_ReceiveData(COM1_PORT);
		printf("data = %c\n", data);
	}

}

void USART_Tx(const char* TxBuffer, u32 length) //FIFO
{
	int i;
	
	for(i = 0; i < length; i++)
	{
		while( !USART_GetFlagStatus(COM1_PORT, USART_FLAG_TXC));
		USART_SendData(COM1_PORT, TxBuffer[i]);
	}

}

void Usart_Sendbyte(HT_USART_TypeDef* USARTx, u8 data) //send string
{
	USART_SendData(USARTx, data);
	while(USART_GetFlagStatus(USARTx, USART_FLAG_TXDE) == RESET);
}

void Usart_SendArray(HT_USART_TypeDef* USARTx, u8 *array, u8 num) //send int
{
	u8 i;
	for(i = 0; i < num; i++)
	{
		Usart_Sendbyte(USARTx, *array);
		array++;
	}
}

void Usart_SendStr(HT_USART_TypeDef* USARTx, uint8_t *str) //send char
{
	uint8_t i;
	for(i = 0; str[i] != '\0'; i++)
	{
		Usart_Sendbyte(USARTx, str[i]);
	}
}
