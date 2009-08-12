@echo off
rem
rem If you create a shortcut to this file, be sure the shortcut
rem has its "Start in" value set to the folder containing this file.
rem

set JAXER_APACHE_PORT=8081
set JAXER_MYSQL_PORT=4417
set ANCHOR=%CD%
cd %ANCHOR%

if "%~1"=="" goto start_jaxer
if "%~1"=="start" goto start_jaxer
if "%~1"=="stop" goto stop_jaxer
goto show_usage

:start_jaxer
    title Start Apache+MySQL
    if exist mysql\data goto data_exists
    echo  ___________________________________________
    echo ^|                                           ^|
    echo ^| This is the first use of this MySQL...    ^|
    echo ^|                                           ^|
    echo ^| Creating the data directory               ^|
    echo ^|___________________________________________^|
    echo.
    mkdir mysql\data
    xcopy mysql\data_blank mysql\data /s
:data_exists
    echo  ___________________________________
    echo ^|                                   ^|
    echo ^| Starting Apache and MySQL...      ^|
    echo ^|___________________________________^|
    echo.
    echo  ^*^*^* BROWSE TO HERE: ^-^-^>  http://localhost:%JAXER_APACHE_PORT%/aptana/
    echo.
    echo ________________________________
    echo.
    echo Logging messages follow:
    echo.
    start /b Apache2\bin\httpd
    start /b mysql\bin\mysqld --no-defaults --port=%JAXER_MYSQL_PORT% --standalone --console --log-error=logs/mysql_error.log > nul
    if not "%~1"=="" goto done
    echo  ___________________________________
    echo ^|                                   ^|
    echo ^| Hit any key to stop them and exit ^|
    echo ^|___________________________________^|
    echo.
    pause > nul
    goto done

:stop_jaxer
    title Stop Apache+MySQL
    mysql\bin\mysqladmin --port=%JAXER_MYSQL_PORT% -u root shutdown
    taskkill /f /im httpd.exe /im AptanaServer.exe
    goto done

:show_usage
    title Usage
    echo.
    echo usage: jaxer [start^|stop]

:done
    echo.
    echo exiting.
