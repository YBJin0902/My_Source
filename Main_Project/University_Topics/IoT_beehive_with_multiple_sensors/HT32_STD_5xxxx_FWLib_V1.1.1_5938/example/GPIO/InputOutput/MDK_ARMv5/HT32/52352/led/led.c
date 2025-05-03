#include "led.h"

void led_init(void)
{
	CKCU_PeripClockConfig_TypeDef LC = {{0}}; 
//name "CKCU_PeripClockConfig_TypeDef " to other !
	LC.Bit.PC = 1;
	
	CKCU_PeripClockConfig(LC, ENABLE);
	
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_15, GPIO_DIR_OUT);
	
	led_off();
	
}



void led_on(void)
{
	GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_15, RESET);
	
}



void led_off(void)
{
	GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_15, SET);
	
}

