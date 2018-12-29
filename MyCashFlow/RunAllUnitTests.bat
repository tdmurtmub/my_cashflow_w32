call rundebunittests
if errorlevel 1 exit /B %errorlevel%
call runrelunittests
if errorlevel 1 exit /B %errorlevel%
