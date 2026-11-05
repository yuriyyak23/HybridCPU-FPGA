$ErrorActionPreference = 'Stop'
$solution = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA\HybridCPU_FPGA.sln'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$log = 'C:\Users\Yuriy\Desktop\HybridCPU_FPGA\CompilerErrors\solution-load-build-Debug-x64-20261030.log'
if (Get-Process devenv,MSBuild -ErrorAction SilentlyContinue) { throw 'Close Visual Studio and MSBuild before running this diagnostic build.' }
"Command: `"$msbuild`" `"$solution`" /m /nologo /p:Configuration=Debug /p:Platform=x64 /v:minimal /flp:logfile=`"$log`";verbosity=normal`"" | Set-Content -LiteralPath ($log + '.command.txt') -Encoding UTF8
& $msbuild $solution /m /nologo /p:Configuration=Debug /p:Platform=x64 /v:minimal "/flp:logfile=$log;verbosity=normal"
$code = $LASTEXITCODE
"MSBuild exit code: $code" | Add-Content -LiteralPath ($log + '.command.txt') -Encoding UTF8
exit $code
