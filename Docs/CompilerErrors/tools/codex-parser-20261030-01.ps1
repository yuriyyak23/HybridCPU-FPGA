$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$source = Join-Path $root 'CloseToHSL\\Core\Execution\ExternalAccelerators\Descriptors\AcceleratorDescriptorParser.cpp'
$intDir = Join-Path $root 'CompilerErrors\parser-obj-01\'
$log = Join-Path $root 'CompilerErrors\codex-parser-20261030-01.log'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=$intDir" /m:1 /v:minimal /nologo *> $log
$code = $LASTEXITCODE
Add-Content -Path $log -Value "MSBUILD_EXIT=$code"
exit $code
