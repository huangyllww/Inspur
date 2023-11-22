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
REM Customize the HPCB output binary and input data files
REM ============================================================================

REM ----------------------------------------------------------------------------
REM First thing, setup HpcbToolV2.exe
REM ----------------------------------------------------------------------------
IF NOT DEFINED WORK_SPACE SET WORK_SPACE=%CD%\..\..\..\..\..\Hygon4
IF NOT DEFINED TOOL_DIR SET TOOL_DIR=%WORK_SPACE%\HgpiModulePkg\HYGONTools\HpcbTool

REM ----------------------------------------------------------------------------
REM Target HPCB binary will be put in RELEASE_DIR
REM ----------------------------------------------------------------------------
SET HPCB_BIN_FILE_BASE_NAME=HPCB

REM ----------------------------------------------------------------------------
REM Reset variable then start adding HPCB data source C files (base name only)
REM ----------------------------------------------------------------------------
SET HPCB_DATA_PREPROCESS_OUTPUT=1
SET HPCB_DATA_DIR=HpcbData
SET HPCB_DATA_TYPE_FILE_LIST=


REM Default file list for all instances
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1701_Type_ExtDefParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1701_Type_ExtParams
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1702_Type_ExtDefParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1702_Type_ExtParams
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1703_Type_ExtDefParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1703_Type_ExtParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_ConsoleOutControl
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_DdrPostPackageRepair
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_DimmInfoSpd
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_ErrorOutControl
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_ExtDefParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_ExtParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_ExtVoltageControl
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsoOverride
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsOdtPatUDIMMDdr
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsCadBusUDIMMDdr 
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsDataBusUDIMMDdr 
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsMaxFreqUDIMMDdr
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsStretchFreqUDIMMDdr
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsCadBusRDIMMDdr
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsDataBusRDIMMDdr
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsOdtPatRDIMMDdr
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsMaxFreqRDIMMDdr
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsStretchFreqRDIMMDdr
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsCadBusLRDIMMDdr
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsDataBusLRDIMMDdr
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsOdtPatLRDIMMDdr
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsMaxFreqLRDIMMDdr
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_PsStretchFreqLRDIMMDdr
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_SpdInfo
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_SpdInfo_udimm
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1705_Type_ExtDefParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1705_Type_ExtParams
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1706_Type_ExtDefParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1706_Type_ExtParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1707_Type_CbsDefParams
SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1707_Type_CbsParams

REM ----------------------------------------------------------------------------
REM BoardIdGettingMethod applies to multiple HPCB only, please remove it if
REM only 1 HPCB instance is needed
REM ----------------------------------------------------------------------------
REM SET HPCB_DATA_TYPE_FILE_LIST=%HPCB_DATA_TYPE_FILE_LIST% HpcbData_SAT_GID_0x1704_Type_BoardIdGettingMethod

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
	SET HPCB_TOOL_NAME=HpcbToolV2.exe
	IF NOT DEFINED TOOL_DIR ECHO
	IF NOT EXIST %TOOL_DIR%\%HPCB_TOOL_NAME%  GOTO ERROR_TOOLDIR&& EXIT /B 10
	IF EXIST %TOOL_DIR%\%HPCB_TOOL_NAME%  ECHO Found %HPCB_TOOL_NAME% at "%TOOL_DIR%".
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

SET BUILD_DIR=%WORK_SPACE%\Build\Hpcb\HyEx
SET RELEASE_DIR=%CD%\Release
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
@ECHO Building HPCB data ...
REM ============================================================================
CALL :SETUP_OUTPUT_DIR

REM ============================================================================
@echo Prepare parameter for data files from HPCB_DATA_TYPE_FILE_LIST
REM ============================================================================
CALL :PREPARE_DATA_FILE_LIST_IN_TOOL_PARAM_FORM

CALL :BUILD_ALL_C_SOURCES
IF ERRORLEVEL 1 GOTO ERR_END    

REM ============================================================================
@ECHO.
@ECHO Prepare HPCB data for building ...
REM ============================================================================

REM ============================================================================
@ECHO.
@ECHO HPCB binary generation ...
REM ============================================================================
SET HPCB_BIN_FILE=%RELEASE_DIR%\%HPCB_BIN_FILE_BASE_NAME%.bin
REM File extension .txt will be added
SET HPCB_EXE_LOG=HpcbBuild_ExeLog
CALL :RUN_HPCB_TOOL_WITH_EXE_LOG
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
	@ECHO       For build HPCB data and tool
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
REM Get current working director and set variable HPCB_WORK_DIR for later use
REM ----------------------------------------------------------------------------
:GET_WORK_DIR
	SET HPCB_WORK_DIR=%CD%
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
	SET HPCB_DATA_TYPE_FILE_LIST_TOOL_PARAM=
	for %%D IN ( %HPCB_DATA_TYPE_FILE_LIST%) DO (
		CALL :APPEND_DATA_FILE_LIST_IN_TOOL_PARAM_FORM %%D
	)
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM Append data file list in tool parameter form
REM ----------------------------------------------------------------------------
:APPEND_DATA_FILE_LIST_IN_TOOL_PARAM_FORM
	REM Specify the build data file extension
	SET HPCB_DATA_OUTPUT_FILE_EXT=bin
	SET HPCB_DATA_TYPE_FILE_LIST_TOOL_PARAM=%HPCB_DATA_TYPE_FILE_LIST_TOOL_PARAM% -I %1.%HPCB_DATA_OUTPUT_FILE_EXT%
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
  GOTO :EOF
REM ----------------------------------------------------------------------------
REM Build all C source files in HPCB_DATA_TYPE_FILE_LIST
REM ----------------------------------------------------------------------------
:BUILD_ALL_C_SOURCES
  SET C_FLAGS=/nologo %HPCB_PLAT_C_FLAGS%
  SET C_INCLUDE_PATH_FLAG=/I Include /I ..\Inc /I %WORK_SPACE%\Build

  ECHO Compile C_FLAG=%C_FLAGS%
	REM --------------------------------------------------------------------
	REM Build HPCB data files
	REM --------------------------------------------------------------------
	for %%F IN (%HPCB_DATA_TYPE_FILE_LIST%) DO (
    IF %HPCB_DATA_PREPROCESS_OUTPUT% == 1 (
      @ECHO Preprocessing ...
      cl %C_FLAGS% %HPCB_DATA_DIR%\%%F.c %C_INCLUDE_PATH_FLAG% /EP > %BUILD_DIR%\%%F.cod
    )
		@ECHO Compiling ...
		cl %C_FLAGS% %HPCB_DATA_DIR%\%%F.c  %C_INCLUDE_PATH_FLAG% /Fo%BUILD_DIR%\%%F.obj /Fe%BUILD_DIR%\%%F.%HPCB_DATA_OUTPUT_FILE_EXT% /Fm%BUILD_DIR%\%%F.map
		IF ERRORLEVEL 1 GOTO ERR_END
		@ECHO.
	)
	GOTO :EOF


REM ----------------------------------------------------------------------------
REM Run HPCB Tool and generate log
REM 
REM INPUT:
REM        BUILD_DIR
REM        HPCB_BIN_FILE
REM        HPCB_DATA_TYPE_FILE_LIST_TOOL_PARAM
REM        HPCB_EXE_LOG
REM OUTPUT:
REM        HPCB_BIN_FILE - HPCB binary file specified
REM        HPCB_EXE_LOG  - Execution log specified (create a new file)
REM ----------------------------------------------------------------------------
:RUN_HPCB_TOOL_WITH_EXE_LOG
	SET HPCB_EXE_LOG_INPUT=%HPCB_EXE_LOG%
	REM --------------------------------------------------------------------
	SET HPCB_EXE_LOG=%LOG_DIR%\%HPCB_EXE_LOG_INPUT%.txt
	@ECHO Run HPCB tool with log ...      (Log:%HPCB_EXE_LOG%)
	PUSHD %BUILD_DIR%
	CALL :HPCB_TOOL
	POPD
	REM --------------------------------------------------------------------
	REM Restor variable
	SET HPCB_EXE_LOG=%HPCB_EXE_LOG_INPUT%
	SET HPCB_EXE_LOG_INPUT=
	GOTO :EOF

REM ----------------------------------------------------------------------------
REM Run HPCB Generator tool V2 by HpcbToolV2
REM 
REM INPUT:
REM        BUILD_DIR
REM        HPCB_BIN_FILE
REM        HPCB_DATA_TYPE_FILE_LIST_TOOL_PARAM
REM        HPCB_EXE_LOG
REM OUTPUT:
REM        HPCB_BIN_FILE - HPCB binary file specified
REM        HPCB_EXE_LOG  - Execution log specified (create a new file)
REM ----------------------------------------------------------------------------
:HPCB_TOOL
	%TOOL_DIR%\%HPCB_TOOL_NAME% --outputfile %HPCB_BIN_FILE% %HPCB_DATA_TYPE_FILE_LIST_TOOL_PARAM% > %HPCB_EXE_LOG% || GOTO ERR_END
	GOTO :EOF
	

REM ============================================================================
:ERROR_TOOLDIR
	ECHO.
	ECHO.
	ECHO ***********************************************************************
	ECHO  ERROR : %HPCB_TOOL_NAME% cannot be found at "%TOOL_DIR%" (TOOL_DIR)
    SET TOOL_DIR
	ECHO.
	ECHO  Environment variable TOOL_DIR should be set properly.
	ECHO  %HPCB_TOOL_NAME% should be found under folder of "HgpiModulePkg\HYGONTools\HpcbTool".
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