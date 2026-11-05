$ErrorActionPreference = 'Continue'
$solution = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\DPCPPConsoleApplication1.sln'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
& $msbuild $solution /t:Build /p:Configuration=Debug /p:Platform=x64 /m:1 /v:minimal /nologo
exit $LASTEXITCODE
