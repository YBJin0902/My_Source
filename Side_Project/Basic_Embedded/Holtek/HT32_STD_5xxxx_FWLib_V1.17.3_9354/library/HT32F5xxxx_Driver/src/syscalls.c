/*********************************************************************************************************//**
 * @file    syscalls.c
 * @version $Rev:: 9354         $
 * @date    $Date:: 2025-07-30 #$
 * @brief   Implementation of system call related functions.
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
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ht32f5xxxx_conf.h"

/** @addtogroup HT32_Peripheral_Driver HT32 Peripheral Driver
  * @{
  */

/** @defgroup SYSCALLS System call functions
  * @brief System call functions for GNU toolchain
  * @{
  */


/* Global variables ----------------------------------------------------------------------------------------*/
/** @defgroup SYSCALLS_Global_Variable System call global variables
  * @{
  */
#undef errno
extern int errno;
extern int  _end;
/**
  * @}
  */

/* Private macro -------------------------------------------------------------------------------------------*/
/** @defgroup SCANF_Private_Macro scanf private macros
  * @{
  */
#define INT32_MAX      (2147483647)
#define INT32_MIN      (-2147483648)
#define ABS_INT32_MIN  (INT32_MAX + 1U)
#define UINT32_MAX     (0xFFFFFFFF)
/**
  * @}
  */

/* Global functions ----------------------------------------------------------------------------------------*/
/** @defgroup SYSCALLS_Exported_Functions System call exported functions
  * @{
  */
caddr_t _sbrk(int incr)
{
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL)
  {
    heap = (unsigned char *)&_end;
  }
  prev_heap = heap;

  heap += incr;

  return (caddr_t) prev_heap;
}

int link(char *old, char *new)
{
  return -1;
}

int _close(int fd)
{
  return -1;
}

int _fstat(int fd, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int _isatty(int fd)
{
  return 1;
}

int _lseek(int fd, int ptr, int dir)
{
  return 0;
}

__attribute__((weak)) unsigned int SERIAL_GetChar(void)
{
  /* !!! NOTICE !!!
     Your environment did not implement SERIAL function for retarget.
     Please refer to the HT32 Firmware Library and use ht32_retarget.c / ht32_serial.c
  */
  while (1);
}

__attribute__((weak)) int _read(int fd, char *ptr, int len)
{
  int i;
  (void)fd;

  for (i = 0; i < len; i++)
  {
    *ptr = SERIAL_GetChar();
    if (*ptr == 13)
    {
      i++;
      break;
    }
    ptr++;
  }

  return i;
}

__attribute__((weak)) unsigned int SERIAL_PutChar(unsigned int ch)
{
  /* !!! NOTICE !!!
     Your environment did not implement SERIAL function for retarget.
     Please refer to the HT32 Firmware Library and use ht32_retarget.c / ht32_serial.c
  */
  while (1);
}

__attribute__((weak)) int _write(int fd, char *ptr, int len)
{
  unsigned int i;
  (void)fd;

  for (i = len; i != 0; i--)
  {
    SERIAL_PutChar(*ptr++);
  }

  return len;
}

void abort(void)
{
  /* Abort called                                                                                           */
  while (1);
}

#if (SCANF_USE_CLIB == 0)
/*********************************************************************************************************//**
 * @brief Input function.
 * @param f: Format string.
 * @retval Number of assigned input items.
 * @note - This implementation supports "%d" & "%x".
 *       - Literal character matching is NOT supported.
 *       - Using unsupported formats will cause an infinite loop.
 ************************************************************************************************************/
signed int scanf(const char *f, ...)
{
  va_list args;
  va_start(args, f);
  int assigned = 0;

  while (*f)
  {
    if (*f == '%')
    {
      f++;
      if (*f == 'd')  // decimal
      {
        int val = 0;
        #if (HT32_LIB_LITE == 0)
        bool overflow = false;
        #endif
        bool neg = false;
        char ch;

        // Skip whitespace or newlines
        do { ch = (char)SERIAL_GetChar(); } while (ch == ' ' || ch == '\r' || ch == '\n');

        if (ch == '-')
        {
          neg = true;
          ch = (char)SERIAL_GetChar();
        }

        while (ch >= '0' && ch <= '9')
        {
          int digit = ch - '0';

          #if (HT32_LIB_LITE == 0)
          // Check overflow
          if (((neg == false) && ((uint32_t)val > (INT32_MAX - digit) / 10)) || \
              ((neg == true)  && ((uint32_t)val > (ABS_INT32_MIN - digit) / 10)))
          {
            overflow = true;
          }
          else
          #endif
          {
            val = val * 10 + digit;
          }

          ch = (char)SERIAL_GetChar();
        }

        #if (HT32_LIB_LITE == 0)
        if (overflow)
        {
          if (neg) val = INT32_MIN;
          else val = INT32_MAX;
        }
        else
        #endif
        {
          if (neg) val = -val;
        }

        int *int_ptr = va_arg(args, int *);
        *int_ptr = val;
        assigned++;
      }
      else if (*f == 'x')  // hexadecimal
      {
        unsigned int val = 0;
        #if (HT32_LIB_LITE == 0)
        bool overflow = false;
        #endif
        char ch;

        // Skip whitespace or newlines
        do { ch = (char)SERIAL_GetChar(); } while (ch == ' ' || ch == '\r' || ch == '\n');

        #if (HT32_LIB_LITE == 0)
        // Support "0x" & "0X" input
        if (ch == '0')
        {
          ch = (char)SERIAL_GetChar();
          if (ch == 'x' || ch == 'X')
          {
            ch = (char)SERIAL_GetChar();
          }
        }
        #endif

        while ((ch >= '0' && ch <= '9') ||
               (ch >= 'a' && ch <= 'f') ||
               (ch >= 'A' && ch <= 'F'))
        {
          uint8_t digit;
          if (ch >= '0' && ch <= '9')
            digit = (ch - '0');
          else if (ch >= 'a' && ch <= 'f')
            digit = (ch - 'a' + 10);
          else if (ch >= 'A' && ch <= 'F')
            digit = (ch - 'A' + 10);

          #if (HT32_LIB_LITE == 0)
          // Check overflow
          if (val > (UINT32_MAX - digit) / 16)
          {
            overflow = true;
          }
          else
          #endif
          {
            val = val * 16 + digit;
          }

          ch = (char)SERIAL_GetChar();
        }

        #if (HT32_LIB_LITE == 0)
        if (overflow) val = UINT32_MAX;
        #endif

        unsigned int *uint_ptr = va_arg(args, unsigned int *);
        *uint_ptr = val;
        assigned++;
      }
      else
      {
        #if (HT32_LIB_LITE == 0)
        /* !!! NOTICE !!!
           Unexpected format specifier encountered.
           Entering infinite loop for debugging purposes.
        */
        while(1)
        {
        }
        #endif
      }
    }
    else
    {
      #if (HT32_LIB_LITE == 0)
      /* !!! NOTICE !!!
         Literal character matching not supported.
         Entering infinite loop for debugging purposes.
      */
      while(1)
      {
      }
      #endif
    }

    f++;
  }

  va_end(args);
  return assigned;
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
