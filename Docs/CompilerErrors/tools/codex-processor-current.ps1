$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'; $dir=Join-Path $root 'CompilerErrors'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:ForceImportAfterCppTargets=$dir\codex-processor-current.targets" "/p:IntDir=$dir\codex-processor-current-obj\" /m:1 /v:minimal /nologo *> "$dir\codex-processor-current.log"
$code=$LASTEXITCODE; Add-Content "$dir\codex-processor-current.log" "MSBUILD_EXIT=$code"; exit $code
