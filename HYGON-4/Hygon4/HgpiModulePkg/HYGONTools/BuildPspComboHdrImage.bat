@set InputBiosFile=%1
@set PspDirectoryCfg=%2
@set FINAL_BIOS=%3

if /i "%USE_PYTHON_SCRIPT%" == "TRUE" (
  if exist "%PYTHON_PATH%\python.exe" (
    @set BUILDPSPDIRECTORY=%PYTHON_PATH%\python.exe BuildPspDirectory.py
  )else if exist "%PYTHON_PATH%\bin\python.exe" (
    @set BUILDPSPDIRECTORY=%PYTHON_PATH%\bin\python.exe BuildPspDirectory.py
  )else (
    @echo !!!PYTHON_PATH system variable is NOT set correctly!!
    goto ERROR
  )
)else (
  @set BUILDPSPDIRECTORY=BuildPspDirectory.exe
)

IF "%TEMP_PATH%" == "" (
  @SET TEMP_PATH=%PSPKIT_PATH%\Temp
)

::Related PATH
@SET PSP_DIR_TOOL_PATH=%PSPKIT_PATH%\PspDirectoryTool

::Create Temporary folder, and copy all files to it
@echo rd    %TEMP_PATH% /S /Q
IF EXIST %TEMP_PATH% rd    %TEMP_PATH% /S /Q
@echo mkdir %TEMP_PATH%
mkdir %TEMP_PATH%
@echo copy  %PSP_DIR_TOOL_PATH%\*.*  %TEMP_PATH% /Y
copy  %PSP_DIR_TOOL_PATH%\*.*  %TEMP_PATH% /Y
@echo copy  %PSPKIT_PATH%\*.*       %TEMP_PATH% /Y
copy  %PSPKIT_PATH%\*.*       %TEMP_PATH% /Y

pushd %TEMP_PATH%
%BUILDPSPDIRECTORY% bb %InputBiosFile% %PspDirectoryCfg% %FINAL_BIOS%
copy Output\%FINAL_BIOS% ..\
popd

:ERROR
:EXIT
