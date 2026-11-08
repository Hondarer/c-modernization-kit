# GitHub Actions で release asset を取得する。
# releases/download の直リンクは 503 や 504 を返すことがある。
# gh は API の asset から同じ配布物を取得する。
param(
    [Parameter(Mandatory = $true)][string]$Repo,
    [Parameter(Mandatory = $true)][string]$Tag,
    [Parameter(Mandatory = $true)][string]$Name,
    [Parameter(Mandatory = $true)][string]$OutFile
)

$ErrorActionPreference = "Stop"
$PSNativeCommandUseErrorActionPreference = $false

$directory = Split-Path -Parent $OutFile
if (-not [string]::IsNullOrEmpty($directory) -and -not (Test-Path -LiteralPath $directory)) {
    New-Item -ItemType Directory -Force -Path $directory | Out-Null
}

$downloaded = $false
for ($attempt = 1; $attempt -le 3; $attempt++) {
    if (Test-Path -LiteralPath $OutFile) {
        Remove-Item -LiteralPath $OutFile -Force
    }
    & gh release download $Tag --repo $Repo --pattern $Name --output $OutFile --clobber
    if ($LASTEXITCODE -eq 0 -and (Test-Path -LiteralPath $OutFile) -and ((Get-Item -LiteralPath $OutFile).Length -gt 0)) {
        $downloaded = $true
        break
    }
    Write-Host "download attempt $attempt failed: $Repo $Tag $Name"
    if ($attempt -lt 3) {
        Start-Sleep -Seconds (5 * $attempt)
    }
}

if (-not $downloaded) {
    throw "GitHub release download failed: $Repo $Tag $Name"
}

$global:LASTEXITCODE = 0
