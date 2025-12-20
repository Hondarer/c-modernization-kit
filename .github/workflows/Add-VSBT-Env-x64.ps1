# VSBT PATH 動的追加スクリプト (PowerShell) - GitHub Actions 専用
# MSVC と Windows SDK を GitHub Actions の環境変数に追加します

# GitHub Actions 環境の Visual Studio インストールパスを取得
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsInstallPath = & $vswhere -latest -property installationPath
$vsbtBase = $vsInstallPath

$msvcBin = Join-Path $vsbtBase "VC\Tools\MSVC\14.44.35207\bin\Hostx64\x64"
$sdkBin = "${env:ProgramFiles(x86)}\Windows Kits\10\bin\10.0.26100.0\x64"
$sdkUcrtBin = "${env:ProgramFiles(x86)}\Windows Kits\10\bin\10.0.26100.0\x64\ucrt"
$diaBin = Join-Path $vsbtBase "DIA SDK\bin"

$msvcInclude = Join-Path $vsbtBase "VC\Tools\MSVC\14.44.35207\include"
$sdkUcrtInclude = "${env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.26100.0\ucrt"
$sdkSharedInclude = "${env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.26100.0\shared"
$sdkUmInclude = "${env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.26100.0\um"
$sdkWinrtInclude = "${env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.26100.0\winrt"
$sdkCppWinrtInclude = "${env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.26100.0\cppwinrt"
$diaInclude = Join-Path $vsbtBase "DIA SDK\include"

$msvcLib = Join-Path $vsbtBase "VC\Tools\MSVC\14.44.35207\lib\x64"
$sdkUcrtLib = "${env:ProgramFiles(x86)}\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64"
$sdkUmLib = "${env:ProgramFiles(x86)}\Windows Kits\10\Lib\10.0.26100.0\um\x64"
$diaLib = Join-Path $vsbtBase "DIA SDK\lib"

# MSVC パスの存在確認
if (-not (Test-Path $msvcBin)) {
    Write-Host "Error: MSVC path not found: $msvcBin"
    exit 1
}

# Windows SDK パスの存在確認
if (-not (Test-Path $sdkBin)) {
    Write-Host "Error: Windows SDK bin path not found: $sdkBin"
    exit 1
}

# GitHub Actions の環境変数に PATH を追加
$pathsToAdd = @($diaBin, $sdkUcrtBin, $sdkBin, $msvcBin)

foreach ($pathToAdd in $pathsToAdd) {
    Write-Host "Adding to PATH: $pathToAdd"
    Add-Content -Path $env:GITHUB_PATH -Value $pathToAdd
}

# GitHub Actions の環境変数に INCLUDE, LIB などを追加
$includeValue = "$msvcInclude;$sdkUcrtInclude;$sdkSharedInclude;$sdkUmInclude;$sdkWinrtInclude;$sdkCppWinrtInclude;$diaInclude"
$libValue = "$msvcLib;$sdkUcrtLib;$sdkUmLib;$diaLib"

Write-Host "Setting environment variables for GitHub Actions..."
Add-Content -Path $env:GITHUB_ENV -Value "VSCMD_ARG_HOST_ARCH=x64"
Add-Content -Path $env:GITHUB_ENV -Value "VSCMD_ARG_TGT_ARCH=x64"
Add-Content -Path $env:GITHUB_ENV -Value "VCToolsVersion=14.44.35207"
Add-Content -Path $env:GITHUB_ENV -Value "WindowsSDKVersion=10.0.26100.0"
Add-Content -Path $env:GITHUB_ENV -Value "VCToolsInstallDir=$(Join-Path $vsbtBase 'VC\Tools\MSVC\14.44.35207')"
Add-Content -Path $env:GITHUB_ENV -Value "WindowsSdkBinPath=${env:ProgramFiles(x86)}\Windows Kits\10\bin"
Add-Content -Path $env:GITHUB_ENV -Value "INCLUDE=$includeValue"
Add-Content -Path $env:GITHUB_ENV -Value "LIB=$libValue"

Write-Host "VSBT environment setup completed."
