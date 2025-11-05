@echo off
call Add-MinGW-Path.cmd >nul 2>&1
call set-vsbt-env-x64.bat >nul 2>&1
dumpbin /IMPORTS prod\calc\src\add\add.exe | findstr /C:"calc.dll" /C:"add" /C:"calcHandler"
