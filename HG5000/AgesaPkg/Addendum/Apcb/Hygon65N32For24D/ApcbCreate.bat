@ECHO OFF
REM -------------------------------------------------------------------
REM Set execution environment base line
SETLOCAL
SETLOCAL EnableDelayedExpansion
IF ERRORLEVEL 1 ECHO Failed to turn on EnableDelayedExpansion! Build system might not work properly!&&PAUSE
VERIFY ON
REM -------------------------------------------------------------------
REM ============================================================================
REM ============================================================================
REM ============================================================================
REM Start of customization section
REM ============================================================================
REM ============================================================================
REM ============================================================================

REM ============================================================================
REM Customize the APCB output binary and input data files
REM ============================================================================

REM ----------------------------------------------------------------------------
REM First thing, setup ApcbToolV2.exe
REM ----------------------------------------------------------------------------
IF NOT DEFINED WORK_SPACE SET WORK_SPACE=%CD%\..\..\..\..
IF NOT DEFINED TOOL_DIR SET TOOL_DIR=%WORK_SPACE%\AgesaModulePkg\AMDTools\ApcbToolV2\Tools

REM ----------------------------------------------------------------------------
REM Target APCB binary will be put in RELEASE_DIR
REM ----------------------------------------------------------------------------
SET APCB_BIN_FILE_BASE_NAME=APCB_DN_D4

REM ----------------------------------------------------------------------------
REM Reset variable then start adding APCB data source C files (base name only)
REM ----------------------------------------------------------------------------
SET APCB_DATA_PREPROCESS_OUTPUT=1
SET APCB_DATA_DIR=ApcbData
SET APCB_DATA_TYPE_FILE_LIST=
REM AGT output APCB XGMI TXEQ settings
SET AGT_DATA_PATH=%APCB_DATA_DIR%\AGT_Data
SET AGT_DATA_INSTANCES=0
SET AGT_XGMI_TXEQ_FILE_LIST=

REM Default file list for all instances
SET AGT_XGMI_TXEQ_FILE_LIST_DFLT=xgmi_apcb_settings_8.h xgmi_apcb_settings_9.h xgmi_apcb_settings_10.h
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1701_Type_ExtDefParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1701_Type_ExtParams
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1702_Type_ExtDefParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1702_Type_ExtParams
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1703_Type_ExtDefParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1703_Type_ExtParams
REM XGMI TXEQ SETTING Instance 0
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1703_Type_xGMITxEq
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_ConsoleOutControl
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_DdrPostPackageRepair
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_DimmInfoSpd
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_ErrorOutControl
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_ExtDefParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_ExtParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_ExtVoltageControl
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsoOverride
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsOdtPatUDIMMDdr4
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsCadBusUDIMMDdr4 
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsDataBusUDIMMDdr4 
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsMaxFreqUDIMMDdr4
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsStretchFreqUDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsCadBusRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsDataBusRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsOdtPatRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsMaxFreqRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsStretchFreqRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsCadBusLRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsDataBusLRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsOdtPatLRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsMaxFreqLRDIMMDdr4
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_PsStretchFreqLRDIMMDdr4
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_SpdInfo
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1705_Type_ExtDefParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1705_Type_ExtParams
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1706_Type_ExtDefParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1706_Type_ExtParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1707_Type_CbsDefParams
SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1707_Type_CbsParams

REM ----------------------------------------------------------------------------
REM BoardIdGettingMethod applies to multiple APCB only, please remove it if
REM only 1 APCB instance is needed
REM ----------------------------------------------------------------------------
REM SET APCB_DATA_TYPE_FILE_LIST=%APCB_DATA_TYPE_FILE_LIST% ApcbData_ZP_GID_0x1704_Type_BoardIdGettingMethod

GOTO START_MAIN_FLOW

REM ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
REM End of customization section
REM ============================================================================
REM ============================================================================
REM ============================================================================


REM ============================================================================
REM Environment Checks
REM ============================================================================
:ENV_CHECK
	SET APCB_TOOL_NAME=ApcbToolV2.exe
	IF NOT DEFINED TOOL_DIR ECHO
	IF NOT EXIST %TOOL_DIR%\%APCB_TOOL_NAME%  GOTO ERROR_TOOLDIR&& EXIT /B 10
	IF EXIST %TOOL_DIR%\%APCB_TOOL_NAME%  ECHO Found %APCB_TOOL_NAME% at "%TOOL_DIR%".
	GOTO :EOF

REM ============================================================================
REM ============================================================================
REM ============================================================================
REM Start of main flow
REM ============================================================================
REM ============================================================================
REM ============================================================================
:START_MAIN_FLOW
CALL :DISP_HEADER
REM cls
CALL :ENV_CHECK
CALL :GET_WORK_DIR
CALL :GET_SYS_TIME
IF /I "%1" == "--HELP" CALL :DISP_HELP && GOTO END
IF /I "%1" == "-H"     CALL :DISP_HELP && GOTO END
IF /I "%1" == ""       CALL :DISP_HELP && GOTO END

SET BUILD_DIR=%WORK_SPACE%\Build\Apcb
SET RELEASE_DIR=%BUILD_DIR%\Release
SET LOG_DIR=%BUILD_DIR%\Log

:PARSE_PARAM
IF /I "%1" == "CLEAN"  CALL :CLEAN_OUTPUT
REM Force to use parameter to continue the build
IF /I "%1" == "BUILD"  GOTO START_BUILDING
IF /I "%1" == ""       GOTO END
REM Parse the next parameter
SHIFT
GOTO PARSE_PARAM
GOTO END

REM ============================================================================
REM Start building if input proper parameter
REM ============================================================================
:START_BUILDING

REM ============================================================================
@ECHO.
@ECHO Building APCB data ...
REM ============================================================================
CALL :SETUP_OUTPUT_DIR
CALL :PROCESS_XGMI_AGT_FILE

REM ============================================================================
@echo Prepare parameter for data files from APCB_DATA_TYPE_FILE_LIST
REM ============================================================================
CALL :PREPARE_DATA_FILE_LIST_IN_TOOL_PARAM_FORM

CALL :BUILD_ALL_C_SOURCES
IF ERRORLEVEL 1 GOTO ERR_END    

REM ============================================================================
@ECHO.
@ECHO Prepare APCB data for building ...
REM ============================================================================

REM ============================================================================
@ECHO.
@ECHO APCB binary generation ...
REM ============================================================================
SET APCB_BIN_FILE=%RELEASE_DIR%\%APCB_BIN_FILE_BASE_NAME%.bin
REM File extension .txt will be added
SET APCB_EXE_LOG=ApcbBuild_ExeLog
CALL :RUN_APCB_TOOL_WITH_EXE_LOG
IF ERRORLEVEL 1 GOTO ERR_END

GOTO END
REM ============================================================================
REM ============================================================================
REM ============================================================================
REM End of main flow
REM ============================================================================
REM ============================================================================
REM ============================================================================






REM ============================================================================
REM Local functions
REM ============================================================================

REM ----------------------------------------------------------------------------
REM Display header
REM ----------------------------------------------------------------------------
:DISP_HEADER
	SET BAT_TOOL_NAME=%~n0
	@ECHO.
	@ECHO **************************
	@ECHO  Start %BAT_TOOL_NAME%
	@ECHO **************************
	@ECHO.
	GOTO :EOF


REM ----------------------------------------------------------------------------
REM Display help info
REM ----------------------------------------------------------------------------
:DISP_HELP
	@ECHO.
	@ECHO %~n0 help info
	@ECHO.
	@ECHO USAGE: 
	@ECHO       For clean up output directories (Release, Build and Log directoies)
	@ECHO		%~n0  CLEAN    
	@ECHO.
	@ECHO       For build APCB data and tool
	@ECHO		%~n0  BUILD    
	@ECHO.
	@ECHO       You may combine both
	@ECHO		%~n0  CLEAN BUILD    
	@ECHO.
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM This is a "function" to do common Error tasks
REM ----------------------------------------------------------------------------
:GOT_ERROR
  ECHO:**** BUILD FAILED ****
  EXIT /B 1

REM ----------------------------------------------------------------------------
REM Get current working director and set variable APCB_WORK_DIR for later use
REM ----------------------------------------------------------------------------
:GET_WORK_DIR
	SET APCB_WORK_DIR=%CD%
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM Setup output folder
REM ----------------------------------------------------------------------------
:SETUP_OUTPUT_DIR
  REM --------------------------------------------------------------------
  IF NOT EXIST %BUILD_DIR% md %BUILD_DIR%
  IF NOT EXIST %RELEASE_DIR% md %RELEASE_DIR%
  IF NOT EXIST %LOG_DIR% md %LOG_DIR%
  GOTO :EOF
REM ----------------------------------------------------------------------------
REM Clean up output folder
REM ----------------------------------------------------------------------------
:CLEAN_OUTPUT
	for %%O IN (%BUILD_DIR% %LOG_DIR% %RELEASE_DIR%) DO (
		IF EXIST  %%O ECHO Deleting directory "%%O"...
		IF EXIST  %%O rd /s /q %%O
	)
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM Prepare data file list in tool parameter form
REM ----------------------------------------------------------------------------
:PREPARE_DATA_FILE_LIST_IN_TOOL_PARAM_FORM
	SET APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM=
	for %%D IN ( %APCB_DATA_TYPE_FILE_LIST%) DO (
		CALL :APPEND_DATA_FILE_LIST_IN_TOOL_PARAM_FORM %%D
	)
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM Append data file list in tool parameter form
REM ----------------------------------------------------------------------------
:APPEND_DATA_FILE_LIST_IN_TOOL_PARAM_FORM
	REM Specify the build data file extension
	SET APCB_DATA_OUTPUT_FILE_EXT=bin
	SET APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM=%APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM% -I %1.%APCB_DATA_OUTPUT_FILE_EXT%
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM Get system time
REM 
REM OUTPUT: Environment varibles SysTime, SysTimeHour, SysTimeMin and SysDate
REM ----------------------------------------------------------------------------
: GET_SYS_TIME
	REM Get the system date and time
	FOR /F "tokens=1-4 delims=/ " %%a IN ("%date%") DO (
	  SET SysDate=%%a%%b%%c
	)
	FOR /F "tokens=1-4 delims=:. " %%a IN ("%time%") DO (
	  SET SysTimeHour=%%a
	  SET SysTimeMin=%%b%%c
	)
	set SysTime=%SysTimeHour%%SysTimeMin%
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM Process AGT tool output files for XGMI TX EQ settings
REM ----------------------------------------------------------------------------
:PROCESS_XGMI_AGT_FILE
  SET APCB_PLAT_C_FLAGS=
  FOR %%A IN (%AGT_DATA_INSTANCES%) DO (
    @ECHO Preprocessing AGT data instance %%A ...
    CALL :PROCESS_XGMI_AGT_INSTANCE_FILE %%A
    @ECHO.
  )
  GOTO :EOF
REM ----------------------------------------------------------------------------
:PROCESS_XGMI_AGT_INSTANCE_FILE
  SET AGT_XGMI_DATA_INSTANCE_ID=%1
  SET AGT_XGMI_INSTANCE_PATH=%AGT_DATA_PATH%\Instance%AGT_XGMI_DATA_INSTANCE_ID%
  SET AGT_XGMI_DATA_FILE_LIST_BAT=%AGT_XGMI_INSTANCE_PATH%\AGT_xGMITxEq_FileList.bat
  SET AGT_XGMI_COMBINED_FILE_DIR=%BUILD_DIR%
  SET AGT_XGMI_COMBINED_FILE=%AGT_XGMI_COMBINED_FILE_DIR%\AGT_xGMITxEq_Inst_%AGT_XGMI_DATA_INSTANCE_ID%.H
  SET AGT_XGMI_TXEQ_FILE_LIST=%AGT_XGMI_TXEQ_FILE_LIST_DFLT%
  REM Apply customized file list
  IF EXIST %AGT_XGMI_DATA_FILE_LIST_BAT%  ECHO Applying XGMI data file list for Instance %AGT_XGMI_DATA_INSTANCE_ID%&&CALL %AGT_XGMI_DATA_FILE_LIST_BAT%
  ECHO AGT XGMI data file list: %AGT_XGMI_TXEQ_FILE_LIST%
  ECHO Generating combined AGT XGMI tuning output file "%AGT_XGMI_COMBINED_FILE%"...
  ECHO // > %AGT_XGMI_COMBINED_FILE%
  ECHO // Auto-generated file combining AGT XGMI tuning output files >> %AGT_XGMI_COMBINED_FILE%
  ECHO // >> %AGT_XGMI_COMBINED_FILE%
  FOR %%X IN (%AGT_XGMI_TXEQ_FILE_LIST%) DO (
    SET AGT_XGMI_FILE_INPUT=%AGT_XGMI_INSTANCE_PATH%\%%X
    ECHO. >> %AGT_XGMI_COMBINED_FILE%
    IF EXIST !AGT_XGMI_FILE_INPUT! (
      ECHO // >> %AGT_XGMI_COMBINED_FILE%
      ECHO // Source file: !AGT_XGMI_FILE_INPUT! >> %AGT_XGMI_COMBINED_FILE%
      ECHO // >> %AGT_XGMI_COMBINED_FILE%
      TYPE !AGT_XGMI_FILE_INPUT! >> %AGT_XGMI_COMBINED_FILE%
      SET APCB_PLAT_C_FLAGS=/D AGT_DATA_FILE_INSTANCE%AGT_XGMI_DATA_INSTANCE_ID%_EXIST=1 %APCB_PLAT_C_FLAGS%
    ) ELSE (
      ECHO // >> %AGT_XGMI_COMBINED_FILE%
      ECHO // ERROR: AGT XGMI tuning file "%%X" does not exist under folder "!AGT_XGMI_INSTANCE_PATH!". >> %AGT_XGMI_COMBINED_FILE%
      ECHO // ERROR: Please check path "%CD%\!AGT_XGMI_INSTANCE_PATH!". >> %AGT_XGMI_COMBINED_FILE%
      ECHO // >> %AGT_XGMI_COMBINED_FILE%
      ECHO ERROR: AGT XGMI tuning file "%%X"does not exist under folder "!AGT_XGMI_INSTANCE_PATH!".
      ECHO ERROR: Please check path "%CD%\!AGT_XGMI_INSTANCE_PATH!".
      PAUSE
      EXIT 1 /B
    )
  )
  SET APCB_PLAT_C_FLAGS=%APCB_PLAT_C_FLAGS% /I %AGT_XGMI_COMBINED_FILE_DIR%
  GOTO :EOF
REM ----------------------------------------------------------------------------
REM Build all C source files in APCB_DATA_TYPE_FILE_LIST
REM ----------------------------------------------------------------------------
:BUILD_ALL_C_SOURCES
  SET C_FLAGS=/nologo %APCB_PLAT_C_FLAGS%
  SET C_INCLUDE_PATH_FLAG=/I Include /I ..\Inc /I %WORK_SPACE%\Build

  ECHO Compile C_FLAG=%C_FLAGS%
	REM --------------------------------------------------------------------
	REM Build APCB data files
	REM --------------------------------------------------------------------
	for %%F IN (%APCB_DATA_TYPE_FILE_LIST%) DO (
    IF %APCB_DATA_PREPROCESS_OUTPUT% == 1 (
      @ECHO Preprocessing ...
      cl %C_FLAGS% %APCB_DATA_DIR%\%%F.c %C_INCLUDE_PATH_FLAG% /EP > %BUILD_DIR%\%%F.cod
    )
		@ECHO Compiling ...
		cl %C_FLAGS% %APCB_DATA_DIR%\%%F.c  %C_INCLUDE_PATH_FLAG% /Fo%BUILD_DIR%\%%F.obj /Fe%BUILD_DIR%\%%F.%APCB_DATA_OUTPUT_FILE_EXT% /Fm%BUILD_DIR%\%%F.map
		IF ERRORLEVEL 1 GOTO ERR_END
		@ECHO.
	)
	GOTO :EOF


REM ----------------------------------------------------------------------------
REM Run APCB Tool and generate log
REM 
REM INPUT:
REM        BUILD_DIR
REM        APCB_BIN_FILE
REM        APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM
REM        APCB_EXE_LOG
REM OUTPUT:
REM        APCB_BIN_FILE - APCB binary file specified
REM        APCB_EXE_LOG  - Execution log specified (create a new file)
REM ----------------------------------------------------------------------------
:RUN_APCB_TOOL_WITH_EXE_LOG
	SET APCB_EXE_LOG_INPUT=%APCB_EXE_LOG%
	REM --------------------------------------------------------------------
	SET APCB_EXE_LOG=%LOG_DIR%\%APCB_EXE_LOG_INPUT%.txt
	@ECHO Run APCB tool with log ...      (Log:%APCB_EXE_LOG%)
	PUSHD %BUILD_DIR%
	CALL :APCB_TOOL
	POPD
	REM --------------------------------------------------------------------
	REM Restor variable
	SET APCB_EXE_LOG=%APCB_EXE_LOG_INPUT%
	SET APCB_EXE_LOG_INPUT=
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM Run APCB Generator tool V2 by ApcbToolV2
REM 
REM INPUT:
REM        BUILD_DIR
REM        APCB_BIN_FILE
REM        APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM
REM        APCB_EXE_LOG
REM OUTPUT:
REM        APCB_BIN_FILE - APCB binary file specified
REM        APCB_EXE_LOG  - Execution log specified (create a new file)
REM ----------------------------------------------------------------------------
:APCB_TOOL
	%TOOL_DIR%\%APCB_TOOL_NAME% --outputfile %APCB_BIN_FILE% %APCB_DATA_TYPE_FILE_LIST_TOOL_PARAM% > %APCB_EXE_LOG% || GOTO ERR_END
	GOTO :EOF
	

REM ============================================================================
:ERROR_TOOLDIR
	ECHO.
	ECHO.
	ECHO ***********************************************************************
	ECHO  ERROR : %APCB_TOOL_NAME% cannot be found at "%TOOL_DIR%" (TOOL_DIR)
    SET TOOL_DIR
	ECHO.
	ECHO  Environment variable TOOL_DIR should be set properly.
	ECHO  %APCB_TOOL_NAME% should be found under folder of "AgesaModulePkg\AMDTools\ApcbToolV2\Tools".
	ECHO ***********************************************************************
	ECHO.
	ECHO.
	PAUSE
	GOTO :EOF

REM ============================================================================
:ERR_END
	@ECHO.
	@ECHO **************************
	@ECHO  !!! Exit with error. !!!
	@ECHO ************************** 
	EXIT /B 1
	GOTO :EOF

:END
	@ECHO.
	@ECHO **************************
	@ECHO  %BAT_TOOL_NAME% completed!
	@ECHO **************************
	@ECHO.