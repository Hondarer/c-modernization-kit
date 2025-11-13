@echo off
setlocal

REM 環境設定（重要: この順序を維持すること）
REM Environment setup (Important: maintain this order)
call Add-MinGW-Path.cmd
call Add-VSBT-Env-x64.cmd

REM ワークスペースフォルダの設定
REM Set workspace folder
set WORKSPACE_FOLDER=%CD%

REM libcalcbase のビルド
REM Build libcalcbase
echo.
echo 1/3 Building libcalcbase...
cd prod\calc\libsrc\calcbase
make -f Makefile.Windows-poc clean
if errorlevel 1 (
    echo ERROR libcalcbase clean failed
    exit /b 1
)
make -f Makefile.Windows-poc
if errorlevel 1 (
    echo ERROR libcalcbase build failed
    exit /b 1
)
echo SUCCESS libcalcbase built successfully

REM libcalc のビルド
REM Build libcalc
echo.
echo 2/3 Building libcalc...
cd ..\calc
make -f Makefile.Windows-poc clean
if errorlevel 1 (
    echo ERROR libcalc clean failed
    exit /b 1
)
make -f Makefile.Windows-poc
if errorlevel 1 (
    echo ERROR libcalc build failed
    exit /b 1
)
echo SUCCESS libcalc built successfully
dumpbin /NOLOGO /EXPORTS %WORKSPACE_FOLDER%\prod\calc\lib\calc.dll | findstr /C:"Dump of file" /C:"calc.dll" /C:"add" /C:"calcHandler"

REM コマンドのビルド
REM Build command
echo.
echo 3/3 Building add command...
cd ..\..\src
make clean
if errorlevel 1 (
    echo ERROR command clean failed
    exit /b 1
)
make
if errorlevel 1 (
    echo ERROR command build failed
    exit /b 1
)
echo SUCCESS command built successfully
dumpbin /NOLOGO /IMPORTS add\add.exe | findstr /C:"Dump of file" /C:"calc.dll" /C:"add" /C:"calcHandler"
dumpbin /NOLOGO /IMPORTS calc\calc.exe | findstr /C:"Dump of file" /C:"calc.dll" /C:"add" /C:"calcHandler"
dumpbin /NOLOGO /IMPORTS shared-and-static-add\shared-and-static-add.exe | findstr /C:"Dump of file" /C:"calc.dll" /C:"add" /C:"calcHandler"

echo.
echo ========================================
echo Build completed successfully
echo ========================================
echo.

endlocal
