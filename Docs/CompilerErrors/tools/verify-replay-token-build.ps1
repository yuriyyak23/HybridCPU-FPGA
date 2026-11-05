$msbuild='C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe'
$solution=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'DPCPPConsoleApplication1.sln'
& $msbuild $solution /t:ClCompile /p:Configuration=Debug /p:Platform=x64 '/p:SelectedFiles=2\CloseToHSL\Core\Pipeline\MicroOps\Replay\ReplayToken.cpp' /m:1 /v:quiet /nologo
exit $LASTEXITCODE
