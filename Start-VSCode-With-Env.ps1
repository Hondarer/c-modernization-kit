<#
.SYNOPSIS
    Windows 開発環境セットアップスクリプト

.DESCRIPTION
    MinGW PATH と VSBT 環境変数を現在のセッションに設定し、VS Code を起動します。
    このテンプレートを任意のユーザーフォルダにコピーし、カスタマイズして利用してください。

    各設定値が空文字 ("") の場合、候補ディレクトリを自動走査して検出します。
    明示的にパスを設定した場合は、その値が優先されます。

.PARAMETER EnvOnly
    環境変数の設定のみを行い、VS Code を起動しません。

.PARAMETER Help
    このヘルプを表示します。

.EXAMPLE
    .\Start-VSCode-With-Env.ps1
    候補ディレクトリを自動走査し、環境変数を設定して VS Code を起動します。

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

    自動走査の対象:
    - Git for Windows: "C:\Program Files\Git", "C:\ProgramData\devbin-win\bin\git"
    - Visual Studio: "C:\Program Files\Microsoft Visual Studio\*\*", "C:\ProgramData\devbin-win\bin\vsbt"
    - Windows SDK: "C:\Program Files (x86)\Windows Kits\10", "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10"
    - MSVC / Windows SDK のバージョンは、最新のものが自動選択されます。
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
# 空文字 ("") を設定すると、候補ディレクトリの走査による自動検出を行います

# Git for Windows ディレクトリ
# 例:                 "C:\Program Files\Git"
#                     "C:\ProgramData\devbin-win\bin\git"
$gitProgramPath     = ""

# VS Debug Interface Access SDK ディレクトリ
# 例:                 "C:\Program Files\Microsoft Visual Studio\2022\Professional\DIA SDK"
#                     "C:\Program Files\Microsoft Visual Studio\18\Professional\DIA SDK"
#                     "C:\ProgramData\devbin-win\bin\vsbt\DIA SDK"
$diaSDKPath         = ""

# MSVC ツールセット ディレクトリ
# 例:                 "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Tools\MSVC"
#                     "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Tools\MSVC"
#                     "C:\ProgramData\devbin-win\bin\vsbt\VC\Tools\MSVC"
$msvcToolSetPath    = ""

# MSVC ツールセット バージョン
# インストールされているバージョンを確認するには、$msvcToolSetPath 配下を確認してください
# 例:                 "14.50.35717"
#                     "14.44.35207"
$msvcToolSetVersion = ""

# Windows SDK ディレクトリ
# 例:                 "C:\Program Files (x86)\Windows Kits\10"
#                     "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10"
$windowsSDKPath     = ""

# Windows SDK バージョン
# インストールされているバージョンを確認するには、$windowsSDKPath\Lib 配下を確認してください
# 例:                 "10.0.26100.0"
$windowsSDKVersion  = ""

# VS Code 起動対象のレポジトリのパス
# このスクリプトを個人フォルダにコピーして利用する場合に変更してください
$vscodeTargetPath   = Join-Path $scriptDir "."

# ---- ユーザー設定値 END ------------------------------------------------------------------------------

# ---- プラットフォームチェック ----

if (-not $IsWindows -and $PSVersionTable.PSEdition -ne 'Desktop') {
    Write-Host "Error: This script is for Windows only."
    exit 1
}

if ([System.Environment]::Is64BitOperatingSystem -eq $false) {
    Write-Host "Error: This script requires a 64-bit operating system."
    exit 1
}

# ---- ヘルプ表示 ----

if ($Help) {
    Get-Help $scriptPath -Detailed
    return
}

# ---- 候補ディレクトリ走査用ヘルパー関数 ----

# 候補パスのリストから最初に存在するパスを返す
function Find-FirstValidPath {
    param([string[]]$Candidates)
    foreach ($path in $Candidates) {
        if (Test-Path $path) { return $path }
    }
    return $null
}

# ディレクトリ内のバージョン番号ディレクトリから最新のものを返す
function Find-LatestVersionDirectory {
    param([string]$BasePath)
    if (-not (Test-Path $BasePath)) { return $null }
    $dirs = Get-ChildItem $BasePath -Directory | Where-Object { $_.Name -match '^\d+\.' }
    if (-not $dirs) { return $null }
    $sorted = $dirs | Sort-Object {
        try { [version]$_.Name } catch { [version]"0.0" }
    } -Descending
    return ($sorted | Select-Object -First 1).Name
}

# ---- 候補ディレクトリの走査 ----

# Visual Studio インストールの候補パスを構築
# VS2022 以前: 年ベースのディレクトリ名 (2017, 2019, 2022)
# VS2026 以降: 内部メジャーバージョン番号 (18, 19, ...)
# 新しいバージョンを優先するため、内部バージョン番号 (< 2000) を年ベース (>= 2000) より先にソートする
$vsBaseCandidates = @()
$vsProgramFiles = "C:\Program Files\Microsoft Visual Studio"
if (Test-Path $vsProgramFiles) {
    $vsVersionDirs = Get-ChildItem $vsProgramFiles -Directory |
        Where-Object { $_.Name -match '^\d+$' } |
        Sort-Object {
            $v = [int]$_.Name
            if ($v -lt 2000) { 10000 + $v } else { $v }
        } -Descending
    foreach ($versionDir in $vsVersionDirs) {
        $vsEditions = Get-ChildItem $versionDir.FullName -Directory
        foreach ($edition in $vsEditions) {
            $vsBaseCandidates += $edition.FullName
        }
    }
}
$vsBaseCandidates += "C:\ProgramData\devbin-win\bin\vsbt"

# Git for Windows の自動検出
if (-not $gitProgramPath) {
    $gitCandidates = @(
        "C:\Program Files\Git",
        "C:\ProgramData\devbin-win\bin\git"
    )
    $gitProgramPath = Find-FirstValidPath $gitCandidates
    if ($gitProgramPath) {
        Write-Host "Auto-detected Git: $gitProgramPath"
    } else {
        Write-Host "Error: Git for Windows not found in candidate directories:"
        $gitCandidates | ForEach-Object { Write-Host "  - $_" }
        exit 1
    }
}

# DIA SDK の自動検出
if (-not $diaSDKPath) {
    $diaCandidates = $vsBaseCandidates | ForEach-Object { Join-Path $_ "DIA SDK" }
    $diaSDKPath = Find-FirstValidPath $diaCandidates
    if ($diaSDKPath) {
        Write-Host "Auto-detected DIA SDK: $diaSDKPath"
    } else {
        Write-Host "Error: DIA SDK not found in candidate directories:"
        $diaCandidates | ForEach-Object { Write-Host "  - $_" }
        exit 1
    }
}

# MSVC ツールセットの自動検出
if (-not $msvcToolSetPath) {
    $msvcCandidates = $vsBaseCandidates | ForEach-Object { Join-Path $_ "VC\Tools\MSVC" }
    $msvcToolSetPath = Find-FirstValidPath $msvcCandidates
    if ($msvcToolSetPath) {
        Write-Host "Auto-detected MSVC ToolSet: $msvcToolSetPath"
    } else {
        Write-Host "Error: MSVC ToolSet not found in candidate directories:"
        $msvcCandidates | ForEach-Object { Write-Host "  - $_" }
        exit 1
    }
}

# MSVC ツールセットバージョンの自動検出
if (-not $msvcToolSetVersion) {
    $msvcToolSetVersion = Find-LatestVersionDirectory $msvcToolSetPath
    if ($msvcToolSetVersion) {
        Write-Host "Auto-detected MSVC version: $msvcToolSetVersion"
    } else {
        Write-Host "Error: No MSVC ToolSet version found in: $msvcToolSetPath"
        exit 1
    }
}

# Windows SDK の自動検出
if (-not $windowsSDKPath) {
    $sdkCandidates = @(
        "C:\Program Files (x86)\Windows Kits\10",
        "C:\ProgramData\devbin-win\bin\vsbt\Windows Kits\10"
    )
    $windowsSDKPath = Find-FirstValidPath $sdkCandidates
    if ($windowsSDKPath) {
        Write-Host "Auto-detected Windows SDK: $windowsSDKPath"
    } else {
        Write-Host "Error: Windows SDK not found in candidate directories:"
        $sdkCandidates | ForEach-Object { Write-Host "  - $_" }
        exit 1
    }
}

# Windows SDK バージョンの自動検出
if (-not $windowsSDKVersion) {
    $sdkLibPath = Join-Path $windowsSDKPath "Lib"
    $windowsSDKVersion = Find-LatestVersionDirectory $sdkLibPath
    if ($windowsSDKVersion) {
        Write-Host "Auto-detected Windows SDK version: $windowsSDKVersion"
    } else {
        Write-Host "Error: No Windows SDK version found in: $sdkLibPath"
        exit 1
    }
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

# 既存の VS Code インスタンスが存在する場合、新しいウィンドウは既存プロセスの環境変数を
# 継承するため、このスクリプトで設定した環境変数が反映されない (as-designed)。
# https://github.com/microsoft/vscode/issues/236981
$codeProcess = Get-Process -Name "Code" -ErrorAction SilentlyContinue
if ($codeProcess) {
    Write-Host "Error: VS Code is already running."
    Write-Host "Environment variables set by this script will not be inherited by the existing instance."
    Write-Host "Please close all VS Code windows and run this script again."
    exit 1
}

if (-not (Test-Path $vscodeTargetPath)) {
    Write-Host "Error: VS Code target path not found: $vscodeTargetPath"
    exit 1
}
Write-Host "Starting new VS Code window with configured environment..."
code --new-window "$vscodeTargetPath"
