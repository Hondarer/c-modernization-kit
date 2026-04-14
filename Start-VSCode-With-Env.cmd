@echo off

REM Get current script directory
set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

powershell.exe -ExecutionPolicy Bypass -File "%SCRIPT_DIR%\bin\Start-VSCode-With-Env.ps1" %*
set "EXIT_CODE=%ERRORLEVEL%"

echo.
powershell.exe -NoProfile -Command ^
  "$s = 5; while ($s -gt 0) { Write-Host -NoNewline \"`rThis window will close in $s seconds...\"; Start-Sleep -Seconds 1; $s-- }; Write-Host"
exit %EXIT_CODE%
