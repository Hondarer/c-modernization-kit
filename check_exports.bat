@echo off
call Add-MinGW-Path.cmd >nul 2>&1
call set-vsbt-env-x64.bat >nul 2>&1
dumpbin /EXPORTS prod\calc\lib\calc.dll
