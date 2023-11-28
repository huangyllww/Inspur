@echo off

REM MY_BOARD_ID only support 7 chars.
REM if MY_BOARD_ID is EMPTY, BOARD_ID will be set as ZhaoXin CRB rule name.

echo **************** OEM_SET ****************

REM HG7000S-V01.bin
REM just follow BuildNum.bat
REM set BIOS_MAJOR_VER=02


REM Puti_BIOS_1.0.00_Standard_20200526.bin
REM 
REM X.Y.ZZ
REM   X - 1 : EVT
REM     - 2 : DVT
REM     - 3 : PVT
REM     - 4 : MP

set MY_BOARD_ID=CS5260H
set OEM_ID=ISP
set BIOS_MAJOR_VER=2
set BIOS_CPUID=3
set BIOS_MINOR_VER=8
set CUSTOMER_ID=Standard
set MY_BIOS_TARGET_BASE_NAME=%MY_BOARD_ID%_BIOS_%BIOS_MAJOR_VER%.%BIOS_CPUID%.%BIOS_MINOR_VER%_%CUSTOMER_ID%



REM UART_SELECTION
REM   0 - CPU UART 0
REM   1 - BMC UART 1
REM   2 - CPU UART 1
set UART_SELECTION=2

REM 0 - unused
set TEST_BIOS_VERSION=0

set PSP_PREBUILD_BIN=0




set TARGET_NAME_POSTFIX=
if "%TEST_BIOS_VERSION%" == "0" (
  set TARGET_NAME_POSTFIX=D
) else (
  set TARGET_NAME_POSTFIX=D%TEST_BIOS_VERSION%
)

if "%BUILD_TYPE%" == "R" (
  set TARGET_NAME_POSTFIX=
)


echo %MY_BIOS_TARGET_BASE_NAME%


echo **************** OEM_SET_END ****************

