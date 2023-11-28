@echo off

echo ByoCore2018
echo.
echo BuildDebug
echo BuildRelease

if defined VS90COMNTOOLS goto SetVs90toolOK
echo set VS90COMNTOOLS
set VS90COMNTOOLS=C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\Tools\
:SetVs90toolOK

cmd /k

