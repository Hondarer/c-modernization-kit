@echo off
call Add-MinGW-Path.cmd
call set-vsbt-env-x64.bat
set WORKSPACE_FOLDER=D:\Users\tetsuo\Local\repos\doxygen-sample

echo Building libcalc...
cd prod\calc\libsrc\calc
make -f Makefile.Windows-poc clean
if errorlevel 1 (
    echo libcalc clean failed
    exit /b 1
)
make -f Makefile.Windows-poc
if errorlevel 1 (
    echo libcalc build failed
    exit /b 1
)

echo Building add command...
cd ..\..\src\add
make -f Makefile.Windows-poc clean
if errorlevel 1 (
    echo add command clean failed
    exit /b 1
)
make -f Makefile.Windows-poc
if errorlevel 1 (
    echo add command build failed
    exit /b 1
)

echo Build completed successfully
