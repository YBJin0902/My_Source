#include "ht32.h"
#include "led.h"

static void delay(u32 nCount)
{
	vu32 i;
	
	for(i = 0; i < 10000 * nCount; i++)
	{
		
	}
	
}

int main(void)
{
	led_init();
	
	while(1)
	{
		led_on();
		delay(500);
		led_off();
		delay(500);
	}
	
	return 0;
	
}