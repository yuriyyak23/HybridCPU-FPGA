$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$log = Join-Path $root 'CompilerErrors\progress-build-20261029.log'
$console = Join-Path $root 'CompilerErrors\progress-build-20261029.console.log'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.sln') /m /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo /fl "/flp:logfile=$log;verbosity=normal" *> $console
$code = $LASTEXITCODE
Add-Content -Path $console -Value "MSBUILD_EXIT=$code"
exit $code
