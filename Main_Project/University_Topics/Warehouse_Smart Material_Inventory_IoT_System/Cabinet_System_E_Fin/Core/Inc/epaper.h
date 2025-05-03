//#ifndef __EPAPER_H_
//#define __EPAPER_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>
#include <stdio.h>
//#include "fonts.h"
#include "main.h"
//#include "epaper.c"

/*#include "spi.h"
#include "usart.h"
#include "gpio.h"*/

/*byte define*/
#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

/*pin define*/
#define EPD_RST_PIN     RST_GPIO_Port, RST_Pin
#define EPD_DC_PIN      DC_GPIO_Port, DC_Pin
#define EPD_PWR_PIN     PWR_GPIO_Port, PWR_Pin
#define EPD_CS_PIN      CS_GPIO_Port, CS_Pin
#define EPD_Busy_PIN    Busy_GPIO_Port, Busy_Pin

/*epaper size*/
#define EPD_2IN9_V2_WIDTH       128
#define EPD_2IN9_V2_HEIGHT      296

/*function define*/
#define DEV_Digital_Write(_pin, _value) HAL_GPIO_WritePin(_pin, _value == 0? GPIO_PIN_RESET:GPIO_PIN_SET)
#define DEV_Digital_Read(_pin) HAL_GPIO_ReadPin(_pin)
#define DEV_Delay_ms(__xms) HAL_Delay(__xms);

/*function*/
void DEV_SPI_WriteByte(UBYTE value);
int DEV_Module_Init(void);
void DEV_Module_Exit(void);
void EPD_2IN9_V2_Reset(void);//function :	Software reset

void EPD_2IN9_V2_SendCommand(UBYTE Reg);//send command Reg=command register
void EPD_2IN9_V2_SendData(UBYTE Data);//send data Data=Write data
void EPD_2IN9_V2_ReadBusy(void);//busypin==0
void EPD_2IN9_V2_LUT(UBYTE *lut);
void EPD_2IN9_V2_LUT_by_host(UBYTE *lut);
void EPD_2IN9_V2_TurnOnDisplay(void);//display
void EPD_2IN9_V2_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend);//Setting the display window
void EPD_2IN9_V2_SetCursor(UWORD Xstart, UWORD Ystart);//Set Cursor
void EPD_2IN9_V2_Init(void);//Initialize the e-paper register
void EPD_2IN9_V2_Clear(void);//Clear screen
void EPD_2IN9_V2_Display(UBYTE *Image);/*Sends the image buffer in RAM to e-Paper and displays*/
void EPD_2IN9_V2_Display_Base(UBYTE *Image);/*Sends the image buffer in RAM to e-Paper and displays*/
void EPD_2IN9_V2_TurnOnDisplay_Partial(void);//turn display
void EPD_2IN9_V2_Display_Partial(UBYTE *Image);//write display content
void EPD_2IN9_V2_Sleep(void);//Enter sleep mode
void EPD_2IN9_V2_GenshinStartKleePlayer(void);
/*painting*/
void epaper_partdisplay(int, int, char);
int bbyte(int);

void epaper_strdisplay(char*);
void show_data(char*);

int get_displayX(void);
int get_displayY(void);
void epaper_resetXY(void);
void nextline(void);
void epaper_intdisplay(int);
void epaper_refresh(void);
extern int displayX,displayY,displayDataType,displayDataValue;
extern char displayData;

extern int A_[5], B_[5], C_[5], D_[5], E_[5], F_[5], G_[5], H_[5], I_[5], J_[5], K_[5], L_[5], M_[5], N_[5], O_[5], P_[5], Q_[5], R_[5], S_[5], T_[5], U_[5], V_[5], W_[5], X_[5], Y_[5], Z_[5];
extern int a_[5], b_[5], c_[5], d_[5], e_[5], f_[5], g_[5], h_[5], i_[5], j_[5], k_[5], l_[5], m_[5], n_[5], o_[5], p_[5], q_[5], r_[5], s_[5], t_[5], u_[5], v_[5], w_[5], x_[5], y_[5], z_[5];
extern int _0[5], _1[5], _2[5], _3[5], _4[5], _5[5], _6[5], _7[5], _8[5], _9[5];
extern int _colon[5], _[5];
