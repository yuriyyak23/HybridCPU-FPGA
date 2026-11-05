$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$mappedRoot = (& "$env:SystemRoot\System32\subst.exe" | Select-String '^H:\\: => (.+)$' | ForEach-Object { $_.Matches[0].Groups[1].Value.TrimEnd('\') })
if (-not $mappedRoot -or [System.IO.Path]::GetFullPath($mappedRoot) -ne [System.IO.Path]::GetFullPath($root)) {
    throw "H: must be mapped to the project workspace '$root' before using this short-path build script."
}

$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$solution = 'H:\HybridCPU_FPGA.sln'
$stamp = [DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss-fff')
$log = Join-Path $PSScriptRoot "solution-build-Debug-x64-shortpath-$stamp.log"
while (Test-Path -LiteralPath $log) {
    $stamp = [DateTime]::UtcNow.ToString('yyyyMMdd-HHmmss-fff') + '-' + [guid]::NewGuid().ToString('N').Substring(0, 8)
    $log = Join-Path $PSScriptRoot "solution-build-Debug-x64-shortpath-$stamp.log"
}
& $msbuild $solution /m /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo *> $log
$exitCode = $LASTEXITCODE
Add-Content -LiteralPath $log -Value "`nMSBUILD_EXIT_CODE=$exitCode"
Write-Output "MSBUILD_EXIT_CODE=$exitCode LOG=$log"
exit $exitCode
