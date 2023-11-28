@mode con cols=100 lines=9999
@echo off
taskkill /F /FI "WINDOWTITLE eq Building, %~s0*
echo Building, %~s0
echo.
title Building, %~s0


REM =========================================
cd /D"%~dp0"
cd ../
set MAP_DRV_NO=Z
if exist %MAP_DRV_NO%: subst %MAP_DRV_NO%: /d 
subst %MAP_DRV_NO%: .
cd /d %MAP_DRV_NO%:

call make r 2008x Tencent
exit    