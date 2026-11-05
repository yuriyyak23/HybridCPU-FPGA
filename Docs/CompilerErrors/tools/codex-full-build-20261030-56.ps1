$solution = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\DPCPPConsoleApplication1.sln'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$log = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\CompilerErrors\codex-full-build-20261030-56.log'
& $msbuild $solution /t:Build /p:Configuration=Debug /p:Platform=x64 /m:1 /v:minimal /nologo 2>&1 | Tee-Object -FilePath $log
$buildExit = $LASTEXITCODE
Add-Content -LiteralPath $log -Value "MSBUILD_EXIT=$buildExit"
exit $buildExit
