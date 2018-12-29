@echo off
echo Running "%1" Unit Tests:
set errlev=0
if /%1/ == // goto help
pushd %1Test
mycashflow.exe
if errorlevel 1 goto failed
popd
exit /B 0
:failed
popd
exit /B %errorlevel%
:help
