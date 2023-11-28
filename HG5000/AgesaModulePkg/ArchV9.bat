@echo off
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Br -p Am4 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Rv -p Am4 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Sm|Br" -p Am4 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Rv|Sm|Br" -p Am4 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o "Rv|Sm" -p Am4 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Sm -p Am4 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Rv -p Fp5 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Zp -p Sp3 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Zp -p Sp4 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
perl "%CD%\AMDTools\ArchV9\ArchV9.pl" -o Tr -p Sp3r2 -i "%CD%\AMDTools\ArchV9\Config" -e "%CD%"
@echo on