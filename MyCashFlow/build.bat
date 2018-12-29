@echo off
call setup
call build_config Debug
if errorlevel 1 pause
call build_config DebugTest
if errorlevel 1 pause
call build_config Release
if errorlevel 1 pause
call build_config ReleaseTest
if errorlevel 1 pause
call build_bin
if errorlevel 1 pause
