@mode con cols=100 lines=
@echo off
taskkill /F /FI "WINDOWTITLE eq Building, %~s0*
echo Building, %~s0
echo.
title Building, %~s0

REM =========================================
cd /D"%~dp0"
set MAP_DRV_NO=Z
if exist %MAP_DRV_NO%: subst %MAP_DRV_NO%: /d 
subst %MAP_DRV_NO%: .
cd /d %MAP_DRV_NO%:

call make d 2008x Inspur
exit  
    
