@ECHO OFF
REM /*********************************************************************************************************//**
REM * @file    afterbuild.bat
REM * @version $Rev:: 5149         $
REM * @date    $Date:: 2021-01-13 #$
REM * @brief   After build script.
REM *************************************************************************************************************
REM * @attention
REM *
REM * Firmware Disclaimer Information
REM *
REM * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
REM *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
REM *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
REM *    other intellectual property laws.
REM *
REM * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
REM *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
REM *    other than HOLTEK and the customer.
REM *
REM * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
REM *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
REM *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
REM *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
REM *
REM * <h2><center>Copyright (C) Holtek Semiconductor Inc. All rights reserved</center></h2>
REM ************************************************************************************************************/

ECHO creating binary file...
arm-none-eabi-objcopy.exe -O binary %1.elf  %1.bin

ECHO creating disassemble/text file...

REM Three kind of way to create disassemble/text file, refer to "objcopy.txt" for more information
%2\ARM\ARMCC\bin\fromelf --text -c -o %1.text %1.elf
rem arm-none-eabi-objdump.exe -D "%1.elf"  > "%1.text"
rem arm-none-eabi-objdump.exe -S "%1.elf"  > "%1.text"

REM ADD_YOUR_CODE_HERE
