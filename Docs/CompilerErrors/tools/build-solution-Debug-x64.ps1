$ErrorActionPreference = 'Continue'
$root = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
& $msbuild (Join-Path $root 'HybridCPU_FPGA.sln') /m /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
exit $LASTEXITCODE
