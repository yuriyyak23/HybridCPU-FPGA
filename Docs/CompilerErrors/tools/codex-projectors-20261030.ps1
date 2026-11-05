$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\CompilerErrors'
$root=Split-Path $dir
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:ForceImportAfterCppTargets=$dir\codex-projectors-20261030.targets" "/p:IntDir=$dir\codex-projectors-20261030-obj\" /m:1 /v:minimal /nologo *> "$dir\codex-projectors-20261030.log"
Add-Content "$dir\codex-projectors-20261030.log" "MSBUILD_EXIT=$LASTEXITCODE"
