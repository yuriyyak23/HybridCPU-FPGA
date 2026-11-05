$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$log = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\CompilerErrors\current-full-build-iteration13.log'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\DPCPPConsoleApplication1.sln' /m /t:Build /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo /fl "/flp:logfile=$log;verbosity=normal"
exit $LASTEXITCODE
