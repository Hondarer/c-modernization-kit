$ErrorActionPreference = 'Stop'
$scriptPath = Join-Path $PSScriptRoot '..\bin_internal\Start-VSCode-With-Env.ps1'
$tokens = $null
$errors = $null
$ast = [System.Management.Automation.Language.Parser]::ParseFile($scriptPath, [ref]$tokens, [ref]$errors)
if ($errors.Count) { throw "Cannot parse $scriptPath" }
$function = $ast.Find({ param($node) $node -is [System.Management.Automation.Language.FunctionDefinitionAst] -and $node.Name -eq 'Find-GitBinaryPaths' }, $true)
if (-not $function) { throw 'Find-GitBinaryPaths not found' }
Invoke-Expression $function.Extent.Text

$root = Join-Path ([System.IO.Path]::GetTempPath()) ([System.Guid]::NewGuid().ToString())
try {
    New-Item -ItemType Directory -Path $root | Out-Null
    if (Find-GitBinaryPaths $root) { throw 'Empty installation must be rejected' }
    foreach ($dir in @('usr\bin', 'mingw64\bin', 'ucrt64\bin')) {
        New-Item -ItemType Directory -Path (Join-Path $root $dir) -Force | Out-Null
    }
    New-Item -ItemType File -Path (Join-Path $root 'mingw64\bin\git.exe') | Out-Null
    if (Find-GitBinaryPaths $root) { throw 'Installation without bash must be rejected' }
    New-Item -ItemType File -Path (Join-Path $root 'usr\bin\bash.exe') | Out-Null
    $paths = Find-GitBinaryPaths $root
    if ($paths.Bin -ne (Join-Path $root 'mingw64\bin')) { throw 'Legacy layout was not selected' }
    New-Item -ItemType File -Path (Join-Path $root 'ucrt64\bin\git.exe') | Out-Null
    $paths = Find-GitBinaryPaths $root
    if ($paths.Bin -ne (Join-Path $root 'ucrt64\bin')) { throw 'UCRT layout was not selected' }
    Remove-Item -LiteralPath (Join-Path $root 'mingw64\bin\git.exe')
    $paths = Find-GitBinaryPaths $root
    if ($paths.Bin -ne (Join-Path $root 'ucrt64\bin') -or $paths.Usr -ne (Join-Path $root 'usr\bin')) { throw 'UCRT layout was not resolved' }
    Write-Host 'Git layout checks passed.'
} finally {
    if (Test-Path -LiteralPath $root) { Remove-Item -LiteralPath $root -Recurse -Force }
}
