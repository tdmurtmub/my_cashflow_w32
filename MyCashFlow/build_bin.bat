@echo off
md "bin"
xcopy release\MyCashflow.exe .\bin /D
if errorlevel 1 pause
xcopy release\MyCashflow.map .\bin /D
if errorlevel 1 pause
"\Program Files\PellesC\Bin\pomake.exe" %1 %2 %3 /F installer.ppj
if errorlevel 1 pause
