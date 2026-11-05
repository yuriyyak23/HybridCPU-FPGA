$ErrorActionPreference = 'Continue'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$solution = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\DPCPPConsoleApplication1.sln'
$log = Join-Path $PSScriptRoot 'codex-full-build-20261029-18.log'
& $msbuild $solution /t:Build /p:Configuration=Debug /p:Platform=x64 /m:1 /v:minimal /nologo *> $log
$code = $LASTEXITCODE
Add-Content -LiteralPath $log -Value "MSBUILD_EXIT=$code"
Get-Content -LiteralPath $log -Tail 50
exit $code
