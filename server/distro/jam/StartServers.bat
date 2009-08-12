@echo off

rem
rem This batch file is part of the Windows portable Jaxer package.
rem It launches JaxerManager and, if present, the Apache web server.
rem If you create a Windows shortcut to this file, be sure the shortcut
rem has its "Start in" value set to the folder containing this file.
rem
rem
set JAXER_APACHE_PORT=8081
rem Note: MySQL is neither packaged nor started by default
set JAXER_MYSQL_PORT=3306
set JAXER_WEB_PORT=4327
set JAXER_COMMAND_PORT=4328
set NO_JAXER_SPLASH=1
set JAXER_LOG_OUTPUT=%~dp0\logs\jaxer.log
set JAXER_TEMP=%~dp0\tmp
set ANCHOR=%~dps0
cd "%ANCHOR%"

if "%~1"=="" goto start_all
if "%~1"=="start" goto start_all
if "%~1"=="stop" goto stop_all
goto show_usage

:start_all
    if not exist "%JAXER_TEMP%" mkdir "%JAXER_TEMP%"
    if not exist public mkdir public
    if exist public\favicon.ico goto redirect
    if exist jaxer\default_public\favicon.ico copy jaxer\default_public\favicon.ico public\favicon.ico 1> nul
:redirect
    if exist public\index.html.redirect goto logs
    if exist jaxer\default_public\index.html.redirect copy jaxer\default_public\index.html.redirect public\index.html.redirect 1> nul
:logs
    if not exist logs mkdir logs
    if exist local_jaxer goto local_jaxer_conf
    mkdir local_jaxer
    if exist jaxer\default_local_jaxer xcopy jaxer\default_local_jaxer local_jaxer /e 1> nul
:local_jaxer_conf
    if exist local_jaxer\conf goto local_jaxer_data
    mkdir local_jaxer\conf
    if exist jaxer\default_local_jaxer\conf xcopy jaxer\default_local_jaxer\conf local_jaxer\conf /e 1> nul
:local_jaxer_data
    if exist local_jaxer\data goto local_jaxer_conf_config_js
    mkdir local_jaxer\data
    if exist jaxer\default_local_jaxer\data xcopy jaxer\default_local_jaxer\data local_jaxer\data /e 1> nul
:local_jaxer_conf_config_js
    if exist local_jaxer\conf\config.js goto configlog
    if exist jaxer\default_local_jaxer\conf\config.js copy jaxer\default_local_jaxer\conf\config.js local_jaxer\conf\config.js 1> nul
:configlog
    if exist local_jaxer\conf\configLog.js goto configapps
    if exist jaxer\default_local_jaxer\conf\configLog.js copy jaxer\default_local_jaxer\conf\configLog.js local_jaxer\conf\configLog.js 1> nul
:configapps
    if exist local_jaxer\conf\configApps.js goto cfg
    if exist jaxer\default_local_jaxer\conf\configApps.js copy jaxer\default_local_jaxer\conf\configApps.js local_jaxer\conf\configApps.js 1> nul
:cfg
    if exist local_jaxer\conf\JaxerManager.cfg goto extensions
    if exist jaxer\default_local_jaxer\conf\JaxerManager.cfg copy jaxer\default_local_jaxer\conf\JaxerManager.cfg local_jaxer\conf\JaxerManager.cfg 1> nul
:extensions
    if exist local_jaxer\extensions goto mysql
    mkdir local_jaxer\extensions
    if exist jaxer\default_local_jaxer\extensions xcopy jaxer\default_local_jaxer\extensions local_jaxer\extensions /e 1> nul
:mysql
    if not exist mysql\bin\mysqld.exe goto start_jaxermanager
:init_mysql
    if exist data\mysql goto start_mysql
    echo  ___________________________________________
    echo ^|                                           ^|
    echo ^| This is the first use of this MySQL...    ^|
    echo ^|                                           ^|
    echo ^| Creating the data directory               ^|
    echo ^|___________________________________________^|
    echo.
    mkdir data\mysql
    xcopy mysql\data_blank data /s
:start_mysql
    echo  _____________________________________
    echo ^|                                     ^|
    echo ^| Starting MySQL (port %JAXER_MYSQL_PORT%) ...      ^|
    echo ^|_____________________________________^|
    echo.
    start /b mysql\bin\mysqld --no-defaults --port=%JAXER_MYSQL_PORT% --standalone --console --datadir="%ANCHOR%/data" --log-error=logs/mysql_error.log 1> nul
:start_jaxermanager
    echo  _____________________________________________________
    echo ^|                                                     ^|
    echo ^| Starting JaxerManager (web %JAXER_WEB_PORT%, command %JAXER_COMMAND_PORT%) ...  ^|
    echo ^|_____________________________________________________^|
    echo.
    start /d jaxer /b StartJaxer.bat start 1> nul
:start_apache
    if not exist Apache22\bin\httpd.exe goto ready
    echo  ______________________________________________
    echo ^|                                              ^|
    echo ^| Starting Apache (web %JAXER_WEB_PORT%) ...               ^|
    echo ^|______________________________________________^|
    echo.
    if not exist Apache22\logs mkdir Apache22\logs
    start /b Apache22\bin\httpd
    echo  ^*^*^* BROWSE TO HERE: ^-^-^>  http://localhost:%JAXER_APACHE_PORT%/aptana/
    echo.
:ready
    if not "%~1"=="" goto done
    echo  _____________________________________
    echo ^|                                     ^|
    echo ^| Hit any key to stop and exit        ^|
    echo ^|_____________________________________^|
    echo.
    pause > nul
:stop_all
    if not exist mysql\bin\mysqladmin.exe goto stop_apache
    mysql\bin\mysqladmin --port=%JAXER_MYSQL_PORT% -u root shutdown
:stop_apache
    if not exist Apache22\bin\httpd.exe goto stop_jaxer
    taskkill /f /im httpd.exe
:stop_jaxer
    start /d jaxer /b StartJaxer.bat exit
    if not "%~1"=="" goto done
    exit
:show_usage
    echo.
    echo Usage: %~n0 [start^|stop]
:done
