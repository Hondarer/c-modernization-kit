<#
.SYNOPSIS
    Windows 開発環境セットアップスクリプト

.DESCRIPTION
    MinGW PATH と VSBT 環境変数を現在のセッションに設定し、VS Code を起動します。
    このテンプレートを任意のユーザーフォルダにコピーし、カスタマイズして利用してください。

.PARAMETER EnvOnly
    環境変数の設定のみを行い、VS Code を起動しません。

.PARAMETER Help
    このヘルプを表示します。

.EXAMPLE
    .\Start-VSCode-With-Env.ps1
    環境変数を設定し、VS Code を新しいウィンドウで起動します。

.EXAMPLE
    . .\Start-VSCode-With-Env.ps1 -EnvOnly
    現在の PowerShell セッションに環境変数のみを設定します。
    セッションに環境変数を反映するには、ドットソース (先頭の .) が必要です。

.EXAMPLE
    powershell.exe -ExecutionPolicy Bypass -File .\Start-VSCode-With-Env.ps1
    実行権限エラー (PSSecurityException) が発生する場合は、このように実行してください。

.NOTES
    VS Code の起動のみが目的の場合、ドットソースは不要です。
    VS Code はスクリプトの子プロセスとして起動するため、環境変数を継承します。
#>

# ---- パラメータ宣言 ----

param(
    [switch]$EnvOnly,
    [switch]$Help
)

# ---- このスクリプトのファイル名とディレクトリ ----

$scriptPath = $MyInvocation.MyCommand.Path
$scriptDir = Split-Path -Parent $scriptPath

# ---- ユーザー設定値 START ----------------------------------------------------------------------------

# 以下の内容は、環境に応じてカスタマイズを行ってください

# Git for Windows ディレクトリ
# 例:                 "C:\Program Files\Git"
#                     "C:\ProgramData\devbin-win\bin\git"
$gitProgramPath     = "C:\Program Files\Git"

# VS Debug Interface Access SDK ディレクトリ
# 例:                 "C:\Program Files\Microsoft Visual Studio\2022\Professional\DIA SDK"
#                     "C:\Program Files\Microsoft Visual Studio\18\Professional\DIA SDK"
#                     "C:\ProgramData\devbin-win\bin\vsbt\DIA SDK"
$diaSDKPath         = "C:\Program Files\Microsoft Visual Studio\18\Professional\DIA SDK"

# MSVC ツールセット ディレクトリ
# 例:                 "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC"
#                     "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Tools\MSVC"
#                     "C:\ProgramData\devbin-win\bin\vsbt\VC\Tools\MSVC"
$msvcToolSetPath    = "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Tools\MSVC"

# MSVC ツールセット バージョン
# インストールされているバージョンを確認するには、$msvcToolSetPath 配下を確認してください
# 例:                 "14.50.35717"
#                     "14.44.35207"
$msvcToolSetVersion = "14.50.35717"

# Windows SDK ディレクトリ
# 例:                 "C:\Program Files (x86)\Windows Kits\10"
#                     "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10"
$windowsSDKPath     = "C:\Program Files (x86)\Windows Kits\10"

# Windows SDK バージョン
# インストールされているバージョンを確認するには、$windowsSDKPath\Lib 配下を確認してください
# 例:                 "10.0.26100.0"
$windowsSDKVersion  = "10.0.26100.0"

# VS Code 起動対象のレポジトリのパス
# このスクリプトを個人フォルダにコピーして利用する場合に変更してください
$vscodeTargetPath   = Join-Path $scriptDir "."

# ---- ユーザー設定値 END ------------------------------------------------------------------------------

# ---- プラットフォームチェック ----

if (-not $IsWindows -and $PSVersionTable.PSEdition -ne 'Desktop') {
    Write-Host "Error: This script is for Windows only."
    exit 1
}

# ---- ヘルプ表示 ----

if ($Help) {
    Get-Help $scriptPath -Detailed
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

# ---- 環境変数設定のみの場合は、ここで終了 ----

if ($EnvOnly) {
    return
}

# ---- GNU Make 存在チェック ----

$makeCmd = Get-Command make.exe -ErrorAction SilentlyContinue
if (-not $makeCmd) {
    Write-Host "Error: make.exe not found in PATH."
    Write-Host "Please ensure GNU Make is installed and its path is configured."
    exit 1
}
$makeVersionOutput = & make.exe --version 2>&1 | Select-Object -First 1
if ($makeVersionOutput -notmatch "^GNU Make") {
    Write-Host "Error: make.exe is not GNU Make."
    Write-Host "Found: $makeVersionOutput"
    Write-Host "Path: $($makeCmd.Source)"
    exit 1
}
#Write-Host "GNU Make found: $($makeCmd.Source) ($makeVersionOutput)"

# ---- VS Code 起動 ----

if (-not (Test-Path $vscodeTargetPath)) {
    Write-Host "Error: VS Code target path not found: $vscodeTargetPath"
    exit 1
}
Write-Host "Starting new VS Code window with configured environment..."
code --new-window "$vscodeTargetPath"
