$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$dir=Join-Path $root 'CompilerErrors'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:ForceImportAfterCppTargets=$dir\codex-matmul-verify-20261029.targets" "/p:IntDir=$dir\codex-matmul-verify-obj\" /m:1 /v:minimal /nologo *> "$dir\codex-matmul-verify-20261029.log"
$code=$LASTEXITCODE
Add-Content "$dir\codex-matmul-verify-20261029.log" "MSBUILD_EXIT=$code"
exit $code
