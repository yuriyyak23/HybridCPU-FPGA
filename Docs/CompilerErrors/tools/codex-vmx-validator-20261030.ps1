$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$dir=Join-Path $root 'CompilerErrors'
$source='CloseToHSL\\Core\Execution\DmaStreamCompute\VmxDmaDescriptorValidator.cpp'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=$dir\codex-vmx-validator-20261030-obj\" /m:1 /v:minimal /nologo *> "$dir\codex-vmx-validator-20261030.log"
Add-Content "$dir\codex-vmx-validator-20261030.log" "MSBUILD_EXIT=$LASTEXITCODE"
