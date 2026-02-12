# Windows 開発環境セットアップスクリプト (PowerShell)
# MinGW PATH と VSBT 環境変数を現在のセッションに設定します

param(
    [switch]$startvscode,
    [switch]$usage
)

# ---- プラットフォームチェック ----

if (-not $IsWindows -and $PSVersionTable.PSEdition -ne 'Desktop') {
    Write-Host "Error: This script is for Windows only."
    exit 1
}

# ***** 可変値の定義 START *****

# 以下の内容は、環境に応じてカスタマイズを行ってください。

# このスクリプトのディレクトリを基準に相対指定を行いたい場合は以下を活かす
#$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

# Git for Windows ディレクトリ
# 例:             "C:\Program Files\Git"
#                 "C:\ProgramData\devbin-win\bin\git"
$gitProgramPath = "C:\Program Files\Git"

# VS DIA SDK ディレクトリ
# 例:         "C:\Program Files\Microsoft Visual Studio\18\Professional\DIA SDK"
#             "C:\ProgramData\devbin-win\bin\vsbt\DIA SDK"
$diaSDKPath = "C:\Program Files\Microsoft Visual Studio\18\Professional\DIA SDK"

# MSVC ツールセット ディレクトリ
# 例:              "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Tools\MSVC"
#                  "C:\ProgramData\devbin-win\bin\vsbt\VC\Tools\MSVC"
$msvcToolSetPath = "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Tools\MSVC"

# MSVC ツールセット バージョン
# インストールされているバージョンを確認するには、$msvcToolSetPath 配下を確認してください
# 例:                 "14.50.35717"
#                     "14.44.35207"
$msvcToolSetVersion = "14.50.35717"

# Windows SDK ディレクトリ
# 例:             "C:\Program Files (x86)\Windows Kits\10"
#                 "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10"
$windowsSDKPath = "C:\Program Files (x86)\Windows Kits\10"

# Windows SDK バージョン
# インストールされているバージョンを確認するには、$windowsSDKPath\Lib 配下を確認してください
# 例:                "10.0.26100.0"
$windowsSDKVersion = "10.0.26100.0"

# ***** 可変値の定義 END *****

if ($usage) {
    Write-Host ""
    Write-Host "使い方:"
    Write-Host "  . .\Set-Windows-Env.ps1 [-startvscode] [-usage]"
    Write-Host ""
    Write-Host "オプション:"
    Write-Host "  (なし)         MinGW PATH と VSBT 環境変数を現在のセッションに設定します"
    Write-Host "  -startvscode   環境変数の設定後に VS Code (code .) を起動します"
    Write-Host "  -usage         このヘルプを表示します"
    Write-Host ""
    Write-Host "注意:"
    Write-Host "  現在の PowerShell セッションに環境変数を反映するには、ドットソース (先頭の .) が必要です。"
    Write-Host "  -startvscode のみが目的の場合、ドットソースは不要です。"
    Write-Host "  (VS Code はスクリプトの子プロセスとして起動するため、環境変数を継承します)"
    Write-Host ""
    Write-Host "例:"
    Write-Host "  . .\Set-Windows-Env.ps1              # 現在のセッションに環境変数を設定"
    Write-Host "  .\Set-Windows-Env.ps1 -startvscode   # VS Code を正しい環境変数で起動"
    Write-Host ""
    return
}

# ---- MinGW 環境変数の設定 ----

$mingwPath = Join-Path $gitProgramPath "mingw64\bin"
$usrPath   = Join-Path $gitProgramPath "usr\bin"

if (-not (Test-Path $mingwPath)) {
    Write-Host "Error: MinGW path not found: $mingwPath"
    Write-Host "Please ensure Git is properly installed"
    exit 1
}

if (-not (Test-Path $usrPath)) {
    Write-Host "Error: usr/bin path not found: $usrPath"
    Write-Host "Please ensure Git is properly installed"
    exit 1
}

$mingwChanged = $false

foreach ($pathToAdd in @($mingwPath, $usrPath)) {
    $pathExists = $env:PATH -split ';' | Where-Object { $_ -eq $pathToAdd }
    if (-not $pathExists) {
        $env:PATH = "$pathToAdd;$env:PATH"
        $mingwChanged = $true
    }
}

if ($mingwChanged) {
    Write-Host "MinGW PATH addition completed."
} else {
    Write-Host "MinGW PATH already set."
}

# ---- VSBT 環境変数の設定 ----

$msvcBin    = Join-Path $msvcToolSetPath "$msvcToolSetVersion\bin\Hostx64\x64"
$sdkBin     = Join-Path $windowsSDKPath "bin\$windowsSDKVersion\x64"
$sdkUcrtBin = Join-Path $windowsSDKPath "bin\$windowsSDKVersion\x64\ucrt"
$diaBin     = Join-Path $diaSDKPath "bin"

$msvcInclude      = Join-Path $msvcToolSetPath "$msvcToolSetVersion\include"
$sdkUcrtInclude   = Join-Path $windowsSDKPath "Include\$windowsSDKVersion\ucrt"
$sdkSharedInclude = Join-Path $windowsSDKPath "Include\$windowsSDKVersion\shared"
$sdkUmInclude     = Join-Path $windowsSDKPath "Include\$windowsSDKVersion\um"
$sdkWinrtInclude  = Join-Path $windowsSDKPath "Include\$windowsSDKVersion\winrt"
$sdkCppWinrtInclude = Join-Path $windowsSDKPath "Include\$windowsSDKVersion\cppwinrt"
$diaInclude       = Join-Path $diaSDKPath "include"

$msvcLib    = Join-Path $msvcToolSetPath "$msvcToolSetVersion\lib\x64"
$sdkUcrtLib = Join-Path $windowsSDKPath "Lib\$windowsSDKVersion\ucrt\x64"
$sdkUmLib   = Join-Path $windowsSDKPath "Lib\$windowsSDKVersion\um\x64"
$diaLib     = Join-Path $diaSDKPath "lib"

$vsbtDirs = @(
    $msvcBin, $sdkBin, $sdkUcrtBin, $diaBin,
    $msvcInclude, $sdkUcrtInclude, $sdkSharedInclude, $sdkUmInclude,
    $sdkWinrtInclude, $sdkCppWinrtInclude, $diaInclude,
    $msvcLib, $sdkUcrtLib, $sdkUmLib, $diaLib
)

foreach ($dir in $vsbtDirs) {
    if (-not (Test-Path $dir)) {
        Write-Host "Error: VSBT path not found: $dir"
        exit 1
    }
}

$env:VSCMD_ARG_HOST_ARCH = "x64"
$env:VSCMD_ARG_TGT_ARCH  = "x64"
$env:VCToolsVersion      = $msvcToolSetVersion
$env:WindowsSDKVersion   = $windowsSDKVersion
$env:VCToolsInstallDir   = Join-Path $msvcToolSetPath $msvcToolSetVersion
$env:WindowsSdkBinPath   = Join-Path $windowsSDKPath "bin"

$vsbtChanged = $false

foreach ($pathToAdd in @($msvcBin, $sdkBin, $sdkUcrtBin, $diaBin)) {
    $pathExists = $env:PATH -split ';' | Where-Object { $_ -eq $pathToAdd }
    if (-not $pathExists) {
        $env:PATH = "$pathToAdd;$env:PATH"
        $vsbtChanged = $true
    }
}

$env:INCLUDE = "$msvcInclude;$sdkUcrtInclude;$sdkSharedInclude;$sdkUmInclude;$sdkWinrtInclude;$sdkCppWinrtInclude;$diaInclude"
$env:LIB     = "$msvcLib;$sdkUcrtLib;$sdkUmLib;$diaLib"

if ($vsbtChanged) {
    Write-Host "VSBT PATH addition completed."
} else {
    Write-Host "VSBT PATH already set."
}

# ---- VS Code 起動 ----

if ($startvscode) {
    Write-Host "Starting VS Code with configured environment..."
    code .
}
