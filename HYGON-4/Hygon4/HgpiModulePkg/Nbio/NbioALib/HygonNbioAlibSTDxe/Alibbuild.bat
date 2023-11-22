REM #
REM #Alib build batch file
REM #c:\asl\asl.exe /Fo=.\alibv2.aml  alibv2.asl
REM #

REM # remove build folder
rmdir /S /Q BUILD

REM #
REM #set HGPI_ROOT=EFI_SOURCE\HGPI
REM #
set HGPI_ROOT=%CD%\..\..\..\..

set path=c:\ASL;%path%
nmake -f AlibMake.mak
