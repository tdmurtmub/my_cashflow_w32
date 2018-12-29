@if "%1" == "" goto help
@call setup
devenv mycashflow.sln /build %1
@exit /B
:help
@echo Syntax: %0 configuration

