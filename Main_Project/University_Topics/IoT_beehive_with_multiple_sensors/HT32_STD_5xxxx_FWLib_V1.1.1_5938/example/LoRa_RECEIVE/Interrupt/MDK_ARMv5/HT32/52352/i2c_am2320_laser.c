#include "i2c_am2320_laser.h"

u8 i=1;
u32 count=0;
int count2 = 0;
int pwmtimer = 0;
uint8_t  data[4];
uint8_t  Data[4];
float Lux[2];
float T_H[2];
u8 i2ct_h;
char i2cLoRa_Send[]={"AT+SEND=43220,6,"};
char i2cloradata[30];


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

void AM2320(void)
{
	I2C_Wake();
	delay_ms(10);
	I2C_Write();
	I2C_Read();
	printf("Humidity:");
	i2cfloat_to_u8(T_H[0]);
	printf("Temperature:");
	i2cfloat_to_u8(T_H[1]);
	delay_ms(2000);
}

void i2cfloat_to_u8 (float i2cf){
	char i2ctemp[30];
	u8 i2ctempdata[30] = {'\0'};
	int nn = 0;
	int tempn = 0;
	memset(i2cloradata,0,30);
	sprintf(i2ctemp,"%f",i2cf); //int te
	//strcat(i2cloradata,i2cLoRa_Send);
	//strcat(i2cloradata,i2ctemp);
	size_t length = strlen(i2cloradata);
	for(nn = 0;nn<length;nn++){
		i2ctempdata[nn] = i2cloradata[nn]; //u8 TempData[tsize]
		tempn = nn;
	}
	i2ctempdata[tempn+1] = '\r';
	i2ctempdata[tempn+2] = '\n';
	//printf("w = %s",i2ctempdata);
	//Usart_SendStr(HTCFG_UART_PORT,i2ctempdata);
	//mqtt_send(i2ctempdata, length);
	//i2ct_h = i2ctempdata;
	delay_ms(300);
}

/*void i2cint_to_u8 (int i2ci){
	char i2ctemp[30];
	u8 i2ctempdata[30] = {'\0'};
	int nn = 0;
	int tempn = 0;
	memset(i2cloradata,0,30);
	sprintf(i2ctemp,"%d",i2ci); //int te
	strcat(i2cloradata,i2cLoRa_Send);
	strcat(i2cloradata,i2ctemp);
	size_t length = strlen(i2cloradata);
	for(nn = 0;nn<length;nn++){
		i2ctempdata[nn] = i2cloradata[nn]; //u8 TempData[tsize]
		tempn = nn;
	}
	i2ctempdata[tempn+1] = '\r';
	i2ctempdata[tempn+2] = '\n';
	//printf("w = %s",i2ctempdata);
	//Usart_SendStr_nb(COM2_PORT,i2ctempdata);
	delay_ms(300);
}*/