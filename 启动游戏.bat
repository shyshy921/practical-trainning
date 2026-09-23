@echo off
title Interstellar Colonizer - JAVARTS4.0
echo ================================================
echo   Interstellar Colonizer (JAVARTS4.0 Final)
echo   Starting game...
echo   First launch compiles the project, please wait.
echo ================================================
cd /d "%~dp0JavaRTS"
call "%~dp0JavaRTS\mvnw.cmd" javafx:run
echo.
echo Game exited.
pause
