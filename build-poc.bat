@echo off
setlocal

REM 環境設定（重要: この順序を維持すること）
REM Environment setup (Important: maintain this order)
call Add-MinGW-Path.cmd
call set-vsbt-env-x64.bat

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

REM add コマンドのビルド
REM Build add command
echo.
echo 3/3 Building add command...
cd ..\..\src\add
make -f Makefile.Windows-poc clean
if errorlevel 1 (
    echo ERROR add command clean failed
    exit /b 1
)
make -f Makefile.Windows-poc
if errorlevel 1 (
    echo ERROR add command build failed
    exit /b 1
)
echo SUCCESS add command built successfully

echo.
echo ========================================
echo Build completed successfully
echo ========================================
echo.

endlocal
