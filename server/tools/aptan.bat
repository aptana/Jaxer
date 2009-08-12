::
:: aptan.bat: An aptan.py launcher script.
:: version: $Revision: 3205 $
:: os: win
:: env: %Path% must contain python.
::
@echo off
setlocal
set PARENT=%~dp0

python "%PARENT%aptan.py" %*

endlocal
exit /b %ERRORLEVEL%
