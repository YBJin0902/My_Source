#ifndef __USART_H
#define __USART_H

#include "ht32f5xxxx_usart.h"
//#include "sys.h"

#define USART_GPIO_GROUP (GPIO_PA)
#define USART_TX_PIN (GPIO_PIN_4) 
#define USART_RX_PIN (GPIO_PIN_5)
#define USART_AFIO_MODE (AFIO_FUN_USART_UART)

#define COM1_PORT (HT_USART1)
#define COM1_IRQn (USART0_IRQn)

void	USART_Configuration(void);

void COM1_IRQHandler(void);

void Usart_Sendbyte(HT_USART_TypeDef* USARTx, u8 data);

void Usart_SendArray(HT_USART_TypeDef* USARTx, u8 *array, u8 num);
	
void Usart_Sender(HT_USART_TypeDef* USARTx, uint8_t *str);

void USART_Tx(const char* TxBuffer, u32 length);
	

#endif