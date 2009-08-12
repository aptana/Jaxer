@echo off

rem
rem This batch file is part of the Windows Jaxer distribution.
rem It launches JaxerManager, but does not by itself launch any web server.
rem
rem
if not defined JAXER_WEB_PORT set JAXER_WEB_PORT=4327
if not defined JAXER_COMMAND_PORT set JAXER_COMMAND_PORT=4328
if not defined JAXER_LOG_OUTPUT set JAXER_LOG_OUTPUT=%~dp0\..\logs\jaxer.log
if not defined JAXER_TEMP set JAXER_TEMP=%~dp0\..\tmp
if not defined JAXER_CFG set JAXER_CFG=%~dp0\..\local_jaxer\conf\JaxerManager.cfg

if "%~1"=="" goto start_jaxer
if "%~1"=="start" goto start_jaxer
if "%~1"=="stop" goto stop_jaxer
if "%~1"=="exit" goto stop_jaxer
goto show_usage

:start_jaxer
	title Start Jaxer
	if defined NO_JAXER_SPLASH goto start_jaxermanager
:show_jaxermanager_only
	echo  _____________________________________________________
	echo ^|                                                     ^|
	echo ^| Starting JaxerManager (web %JAXER_WEB_PORT%, command %JAXER_COMMAND_PORT%) ...  ^|
	echo ^|_____________________________________________________^|
	echo.
:start_jaxermanager
	start /b /wait Jaxer -reg -tempdir "%JAXER_TEMP%" > nul 2>&1
	start /b JaxerManager --configfile="%JAXER_CFG%" --webport=%JAXER_WEB_PORT% --commandport=%JAXER_COMMAND_PORT% --cfg:tempdir="%JAXER_TEMP%" --log:output="%JAXER_LOG_OUTPUT%" 1> nul
:ready
	if not "%~1"=="" exit
	if defined NO_JAXER_SPLASH goto wait_for_user
	echo  _____________________________________
	echo ^|                                     ^|
	echo ^| Hit any key to stop and exit        ^|
	echo ^|_____________________________________^|
	echo.
:wait_for_user
	pause > nul

:stop_jaxer
	start /b tellJaxerManager -p %JAXER_COMMAND_PORT% stop 1> nul
	if "%~1"=="start" goto done
	if "%~1"=="stop" goto done
	exit

:show_usage
	title Usage
	echo.
	echo usage: StartJaxer [start^|stop]

:done
