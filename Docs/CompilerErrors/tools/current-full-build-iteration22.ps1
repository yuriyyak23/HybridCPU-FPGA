$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$log = Join-Path $root 'CompilerErrors\current-full-build-iteration22.log'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.sln') /m /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo /fl "/flp:logfile=$log;verbosity=normal"
$exitCode = $LASTEXITCODE
"MSBUILD_EXIT=$exitCode" | Tee-Object -FilePath (Join-Path $root 'CompilerErrors\current-full-build-iteration22.console.log')
exit $exitCode
