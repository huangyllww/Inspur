@REM ## @file  
@REM #
@REM # Copyright (c) 2006 - 2021, Byosoft Corporation.<BR> 
@REM # All rights reserved.This software and associated documentation (if any)
@REM # is furnished under a license and may only be used or copied in 
@REM # accordance with the terms of the license. Except as permitted by such
@REM # license, no part of this software or documentation may be reproduced, 
@REM # stored in a retrieval system, or transmitted in any form or by any 
@REM # means without the express written consent of Byosoft Corporation.
@REM # 
@REM # File Name:
@REM #   make.bat  
@REM # 
@REM # Abstract: 
@REM #   build command batch file to complete the build process.
@REM #
@REM # Revision History:
@REM #

@echo off

set TEST_BIOS_VERSION=0
set UART_SELECTION=
set MY_BIOS_TARGET_NAME=
set MY_BOARD_ID=
set BOARD_REV=1
set OEM_ID=ABC
set BOARD_ID=ABCDEFG
set PSP_PREBUILD_BIN=1
set PLATFORM_PACKAGE=ProjectPkg
set OUTPUT_FD_FILE=CRB2018.fd
set BIOS_SIGN_KEY_PATH=ProjectPkg\SignKey

call BuildNum.bat
set BIOS_MAJOR_VER=%BUILD_NUMBER:~2,2%
set BIOS_MINOR_VER=00

REM ----------------------------------------------------------------------------
set SKIP_AUTOGEN=
if /I "%1" == "d" (
  SET TARGET=DEBUG
) else if /I "%1" == "dd" (
  SET TARGET=DEBUG
  set SKIP_AUTOGEN=TRUE
) else if /I "%1" == "r" (
  SET TARGET=RELEASE
) else if /I "%1" == "clean" (
  goto DoBuildClean 
) else (
  echo Default debug mode
  SET TARGET=DEBUG
)

@if "%TARGET%" == "DEBUG" (
  @SET  BUILD_TYPE=D
) else (
  @SET  BUILD_TYPE=R
)

set TOOL_CHAIN_TAG=VS2008x86
if /I "%2" == "2008" (
  set TOOL_CHAIN_TAG=VS2008
) else if /I "%2" == "2008x" (
  set TOOL_CHAIN_TAG=VS2008x86
) else if /I "%2" == "2010x" (
  set TOOL_CHAIN_TAG=VS2010x86   
) else if /I "%2" == "2013" (
  set TOOL_CHAIN_TAG=VS2013
) else if /I "%2" == "2013x" (
  set TOOL_CHAIN_TAG=VS2013x86
) else (
  echo Default TOOL_CHAIN_TAG = %TOOL_CHAIN_TAG%  
)


REM ----------------------------------------------------------------------------
if defined WORKSPACE goto SetWorkSpaceOK

set WORKSPACE=%CD%
set EFI_SOURCE=
set EDK_SOURCE=
set ECP_SOURCE=

REM Y:\ => Y:\.
if "%WORKSPACE:~-1,1%" EQU "\" set WORKSPACE=%WORKSPACE%.

set IASL_PREFIX=%WORKSPACE%\ByoToolPkg\Bin\
set EDK_TOOLS_PATH=%WORKSPACE%\BaseTools

set BYO_TOOL_PATH=%WORKSPACE%\ByoToolPkg\Bin
set PATH=%BYO_TOOL_PATH%;%BYO_TOOL_PATH%\FCE;%PATH%

:SetWorkSpaceOK
call %EDK_TOOLS_PATH%\toolsetup.bat

set OUTPUT_DIR=%WORKSPACE%\Build\%PLATFORM_PACKAGE%\%TARGET%_%TOOL_CHAIN_TAG%
if not exist %OUTPUT_DIR%\X64 (
  mkdir %OUTPUT_DIR%\X64
)

REM it seems only "%WORKSPACE%/build" folder is auto included to compile.
REM so copy it from top folder to *build*
echo BIOS Customer :%3

copy /y %WORKSPACE%\Token.h %WORKSPACE%\Build\Token.h

if /I "%3" == "TENCENT" (
  copy /y %WORKSPACE%\Tencent\BCID.h %WORKSPACE%\Build\BCID.h
  copy /y %WORKSPACE%\Tencent\BCID.h %WORKSPACE%\BCID.h
  if exist %WORKSPACE%\Tencent\OemSetTokenTencent.cmd call %WORKSPACE%\Tencent\OemSetTokenTencent.cmd
  ) else if /I "%3" == "INSPURST" (
  copy /y %WORKSPACE%\InspurST\BCID.h %WORKSPACE%\Build\BCID.h
  copy /y %WORKSPACE%\InspurST\BCID.h %WORKSPACE%\BCID.h
  if exist %WORKSPACE%\InspurST\OemSet_InspurST.cmd (
    call %WORKSPACE%\InspurST\OemSet_InspurST.cmd
  )  
) else (
  copy /y %WORKSPACE%\NormalBCID.h %WORKSPACE%\Build\BCID.h
  copy /y %WORKSPACE%\NormalBCID.h %WORKSPACE%\BCID.h
  if exist %WORKSPACE%\OemSet.cmd call %WORKSPACE%\OemSet.cmd
)
echo MY_BOARD_ID :%MY_BOARD_ID%.
if not "%MY_BOARD_ID%" EQU "" set BOARD_ID=%MY_BOARD_ID%



REM ----------------------------------------------------------------------------
if /I "%SKIP_AUTOGEN%" == "TRUE" (
  goto READY_TO_BUILD
)


REM AutoGen Files
set AUTOGEN_TARGET_FILE=%PLATFORM_PACKAGE%\BiosId.bin
echo Generate %AUTOGEN_TARGET_FILE%
Win32Tool.exe -biosid %BOARD_ID% %BOARD_REV% %OEM_ID% %BUILD_TYPE% %BIOS_MAJOR_VER%%BIOS_MINOR_VER% %AUTOGEN_TARGET_FILE% %AUTOGEN_TARGET_FILE%.sec
if "%ERRORLEVEL%" NEQ "0" goto :ERROR

Win32Tool.exe -bit %PLATFORM_PACKAGE%\BiosId.bin bat > %OUTPUT_DIR%\BiosBuildTime.bat
call %OUTPUT_DIR%\BiosBuildTime.bat
set MY_BIOS_TARGET_NAME=%MY_BIOS_TARGET_BASE_NAME%_%BIOS_BUILD_DATE%%TARGET_NAME_POSTFIX%



set AUTOGEN_TARGET_FILE=%PLATFORM_PACKAGE%\PlatformAutoGen.dsc
@echo # autogen file, do not edit ...                  > %AUTOGEN_TARGET_FILE%

if /I "%3" == "TENCENT" (
  echo DEFINE BC_NORMAL  = FALSE  >> %AUTOGEN_TARGET_FILE%
  echo DEFINE BC_TENCENT  = TRUE  >> %AUTOGEN_TARGET_FILE%
  echo DEFINE BC_INSPUR_ST  = FALSE  >> %AUTOGEN_TARGET_FILE%
)else if /I "%3" == "INSPURST" (
  echo DEFINE BC_NORMAL  = FALSE  >> %AUTOGEN_TARGET_FILE%
  echo DEFINE BC_TENCENT  = FALSE  >> %AUTOGEN_TARGET_FILE%
  echo DEFINE BC_INSPUR_ST  = TRUE  >> %AUTOGEN_TARGET_FILE%
) else (
  echo DEFINE BC_NORMAL  = TRUE  >> %AUTOGEN_TARGET_FILE%
  echo DEFINE BC_TENCENT  = FALSE  >> %AUTOGEN_TARGET_FILE%
  echo DEFINE BC_INSPUR_ST  = FALSE  >> %AUTOGEN_TARGET_FILE%
)

if "%UART_SELECTION%" neq ""     echo DEFINE UART_SELECTION = %UART_SELECTION% >> %AUTOGEN_TARGET_FILE%
echo DEFINE TEST_BIOS_VERSION  = %TEST_BIOS_VERSION%  >> %AUTOGEN_TARGET_FILE%
echo DEFINE BIOS_SIGN_KEY_PATH = %BIOS_SIGN_KEY_PATH% >> %AUTOGEN_TARGET_FILE%

echo "Update Conf\target.txt"
@findstr /V "ACTIVE_PLATFORM TARGET TARGET_ARCH TOOL_CHAIN_TAG BUILD_RULE_CONF MAX_CONCURRENT_THREAD_NUMBER" Conf\target.txt > %OUTPUT_DIR%\target.txt
@echo ACTIVE_PLATFORM = %PLATFORM_PACKAGE%/%PLATFORM_PACKAGE%.dsc >> %OUTPUT_DIR%\target.txt
@echo TARGET          = %TARGET%                                  >> %OUTPUT_DIR%\target.txt
@echo TARGET_ARCH     = IA32 X64                                  >> %OUTPUT_DIR%\target.txt
@echo TOOL_CHAIN_TAG  = %TOOL_CHAIN_TAG%                          >> %OUTPUT_DIR%\target.txt
@echo BUILD_RULE_CONF = Conf/build_rule.txt                       >> %OUTPUT_DIR%\target.txt
@echo MAX_CONCURRENT_THREAD_NUMBER = %NUMBER_OF_PROCESSORS%       >> %OUTPUT_DIR%\target.txt
@move /Y %OUTPUT_DIR%\target.txt Conf > NUL

set BIOS_TARGET_NAME=%BOARD_ID%_%BUILD_TYPE%%BIOS_MAJOR_VER%_%BIOS_BUILD_DATE_Y2%.bin
if not "%MY_BIOS_TARGET_NAME%" EQU "" set BIOS_TARGET_NAME=%MY_BIOS_TARGET_NAME%.bin
echo %BIOS_TARGET_NAME%
echo DEFINE BIOS_FILE_NAME = %MY_BIOS_TARGET_NAME% >> %AUTOGEN_TARGET_FILE%

echo DEFINE BIOS_VERSION_NAME = %BIOS_MAJOR_VER%.%BIOS_CPUID%.%BIOS_MINOR_VER%  >> %AUTOGEN_TARGET_FILE%


REM ------------------------------ OVERRIDE ------------------------------------







REM ------------------------------- BUILD --------------------------------------
:READY_TO_BUILD
echo Start building ...
@build -j %OUTPUT_DIR%\Build.log
@if "%ERRORLEVEL%" NEQ "0" goto :ERROR



REM --------------------------------- FCE --------------------------------------
echo [FCE]
pushd %OUTPUT_DIR%\FV
if exist *.bin del *.bin

FCE.exe read -i %OUTPUT_FD_FILE% 0006 005C 0078 0030 0030 0030 0031 > FdVar.txt
@if "%ERRORLEVEL%" NEQ "0" (
  echo create FdVar.txt failed!
  goto FCE_Exit
)

FCE.exe update -i %OUTPUT_FD_FILE% -s FdVar.txt -g 004AE66F-F074-4398-B47F-F73BA682C7BE -o bios.fd
@if "%ERRORLEVEL%" NEQ "0" (
  echo FCE update failed!
  goto FCE_Exit
)

popd

if /I "%PSP_PREBUILD_BIN%" == "1" (
  echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  echo PSP FW generation is skipped, use pre build binary instead.
  echo if you want to re-build it, set PSP_PREBUILD_BIN to 0 in make.cmd.
  echo ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  goto AfterPspGen
)

echo.
echo build PSP.

REM bb - Build Bios image with psp entry file embedded
set PSPTOOL=%WORKSPACE%\AgesaModulePkg\AMDTools\NewPspKit\PspDirectoryTool\BuildPspDirectory.exe

if exist %OUTPUT_DIR%\FV\bios.o.fd del %OUTPUT_DIR%\FV\bios.o.fd
ren %OUTPUT_DIR%\FV\bios.fd bios.o.fd

%PSPTOOL% -o %OUTPUT_DIR%\FV bb %OUTPUT_DIR%\FV\bios.o.fd  %WORKSPACE%\PlatformPkg\psp\PspDataTemplateCombo.xml bios.fd1  >  %OUTPUT_DIR%\psp.log 2>&1
%PSPTOOL% -o %OUTPUT_DIR%\FV bb %OUTPUT_DIR%\FV\bios.fd1 %WORKSPACE%\PlatformPkg\psp\PspDataTemplateSoc0.xml bios.fd2    >>  %OUTPUT_DIR%\psp.log 2>&1
%PSPTOOL% -o %OUTPUT_DIR%\FV bb %OUTPUT_DIR%\FV\bios.fd2 %WORKSPACE%\PlatformPkg\psp\PspDataTemplateSoc1.xml bios.fd     >>  %OUTPUT_DIR%\psp.log 2>&1
del %OUTPUT_DIR%\FV\bios.o.fd
del %OUTPUT_DIR%\FV\bios.fd1
del %OUTPUT_DIR%\FV\bios.fd2

:AfterPspGen
set BIOS_SIGNTOOL_PATH=%WORKSPACE%\ByoModulePkg\BiosSign
%BIOS_SIGNTOOL_PATH%\ByoBiosSign.exe -nologo -signfv %OUTPUT_DIR%\FV\bios.fd %WORKSPACE%\%BIOS_SIGN_KEY_PATH%\pvk.pem %WORKSPACE%\%BIOS_SIGN_KEY_PATH%\pub.der

if exist %WORKSPACE%\*.bin del /Q %WORKSPACE%\*.bin
copy /b /y %OUTPUT_DIR%\FV\bios.fd %WORKSPACE%\%BIOS_TARGET_NAME%

REM ----------------- SIGN ----------------------------------------------------------------------------------------------------
echo [SIGN begin]
set BIOS_SIGNTOOL_PATH=%WORKSPACE%\ByoModulePkg\BiosSign
set KEY_PATH=%WORKSPACE%\ByoModulePkg\BiosSign
%BIOS_SIGNTOOL_PATH%\SignBios.exe %BIOS_TARGET_NAME% %KEY_PATH%\M2I_pri_Zhufeng.bin %KEY_PATH%\M2I_pub_Zhufeng.bin %WORKSPACE%\%BIOS_TARGET_NAME%
echo [SIGN end]
REM  ----------------------------------------------------------------------------------------------------------------------------------------


echo -----------------------------------------------------
Win32Tool.exe -cs %BIOS_TARGET_NAME% -stf
echo -----------------------------------------------------



REM ------------------------------- SUCCESS ------------------------------------
echo The EDK2 BIOS build has been completed.
goto :EOF


REM ------------------------------- CLEAN --------------------------------------
:DoBuildClean
@echo off
@if exist Build rmdir /Q /S Build
@if exist Conf  rmdir /Q /S Conf
@echo.
@echo clean OK.
@echo.

goto :EOF

:FCE_Exit
:ERROR
echo Build Error occurred!
pause
