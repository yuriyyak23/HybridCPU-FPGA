$ErrorActionPreference = 'Continue'
$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$sln = Join-Path $root 'DPCPPConsoleApplication1.sln'
$log = Join-Path $root 'CompilerErrors\codex-full-build-20261030-101.log'
& $msbuild $sln /m /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo 2>&1 | Tee-Object -FilePath $log
$code = $LASTEXITCODE
"MSBUILD_EXIT=$code" | Tee-Object -FilePath $log -Append
exit $code

