@echo off
echo CLean Build Dir...
echo %CD%
echo .

if exist *.bin del *.bin
if exist BCID.h del BCID.h

@rem set cosole mode.
if exist  "%CD%\Build" (
    echo Remove Build ...
    rd /S /Q "%CD%\Build"    
)  
if exist  "%CD%\Conf" (
    echo Remove Conf ...
    rd /S /Q "%CD%\Conf"    
)  

exit
