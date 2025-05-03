#ifndef __GPS_H
#define __GPS_H
#include "ht32f5xxxx_01.h"
#include "ht32.h"
#include "ht32_board_config.h"
#include "ht32_board.h"
#include "string.h"
#include "stdlib.h"

void gpsinfo(const int lentemp2 , const char *tempt2);
void gpshead(void);
void gpscheck(void);
void gpsint_to_u8(int te);
#endif
