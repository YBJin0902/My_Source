#include "ht32.h"
#include "ht32_board_config.h"
#include "string.h"

void CLOCKconfig(void);
void initGPIO(void);
void Laser(void);
void I2C_Configuration(void);
void I2C_Down(void);
void I2C_Energize(void);
void I2C_Gauge(void);
void I2C_Data(void);
void I2C_Wake(void);
void I2C_Write(void);
void I2C_Read(void);
void BH1750(void);
void AM2320(void);
void i2cfloat_to_u8 (float i2cf);
void i2cint_to_u8 (int i2ci);

#define I2C_MASTER_ADDRESS     0x0A
#define I2C_BH1750_ADDRESS     0x23	//BH1750 ADD CONECT GND
#define I2C_AM2320_ADDRESS		 0x5C //AM2320 7BIT ADDRESS
#define BufferSize             16
#define ClockSpeed             100000

