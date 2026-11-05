$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$source = 'CloseToHSL\\Core\Execution\DmaStreamCompute\YAKSys_Hybrid_CPU.Core.Execution.DmaStreamCompute.DmaStreamComputeDescriptorParser.cpp'
$log = Join-Path $root 'CompilerErrors\codex-dma-parser-20261030-02.log'
& $msbuild (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=$root\CompilerErrors\codex-dma-isolated-obj\" /m:1 /v:minimal /nologo *> $log
Add-Content -LiteralPath $log -Value "MSBUILD_EXIT=$LASTEXITCODE"