REM @ECHO OFF
REM /*********************************************************************************************************//**
REM * @file    _ProjectConfigScript.bat
REM * @version $Rev:: 5766         $
REM * @date    $Date:: 2022-03-09 #$
REM * @brief   Delete unsupport project related files.
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
SETLOCAL ENABLEDELAYEDEXPANSION

SET IC_NAME_FILE=..\..\..\project_template\Script\_ht32_ic_name.ini
SET PROJ_CONFIG_INI_FILE=_ProjectConfig.ini

IF NOT EXIST %PROJ_CONFIG_INI_FILE% GOTO NO_PROJ_CONFIG_INI_FILE

REM SET PROJ_DEVICE=0 (Default delete)
FOR /F "tokens=1 delims=," %%i IN (%IC_NAME_FILE%) DO (
  SET REM_LINE=%%i
  SET REM_LINE2=!REM_LINE:~0,3!
  SET SER_NAME2=!REM_LINE:~3,1!
  IF "!REM_LINE2!" == "REM" (
    SET DUM=""
  ) else if "%%i"=="\n" (
    SET DUM=""
  ) else if "!SER_NAME2!"=="*" (
    SET DUM=""
  ) else (
    SET PROJ_%%i=0
  )
)

REM Read from PROJ_CONFIG_INI_FILE ini file and set PROJ_DEVICE=1 (Keep project)
FOR /F "tokens=1,2,3,4 delims=, " %%i IN (%PROJ_CONFIG_INI_FILE%) DO (
  IF NOT "%%i" == "REM" (
  IF NOT "%%i" == "rem" (
    IF NOT "%%i" == "" (
      SET PROJ_%%i=1
    )
  )
  )
)

FOR /F "tokens=1 delims=," %%i IN (%IC_NAME_FILE%) DO (
  SET REM_LINE=%%i
  SET REM_LINE2=!REM_LINE:~0,3!
  SET SER_NAME2=!REM_LINE:~3,1!
  SET SER_NAME1=!REM_LINE:~2,1!
  SET PROJ_NAME=PROJ_%%i
  IF "!REM_LINE2!" == "REM" (
    SET DUM=""
  ) else if "%%i"=="\n" (
    SET DUM=""
  ) else if "!SER_NAME2!"=="*" (
    SET DUM=""
  ) else if "!SER_NAME1!"=="*" (
    SET DUM=""
  ) else (
    CALL :DELP %%PROJ_NAME%% %%i
  )
)

:NO_PROJ_CONFIG_INI_FILE
GOTO :EOF

:DELP
IF !%1!==0 (
REM echo DEL %2
del /S Project_%2*.* %2.mk 1> nul 2>&1
)
exit /b
