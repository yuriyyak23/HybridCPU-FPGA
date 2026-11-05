$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\CompilerErrors'
$root=Split-Path $dir
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:ForceImportAfterCppTargets=$dir\codex-focused-20261030-98.targets" "/p:IntDir=$dir\codex-focused-20261030-98-obj\" /m:1 /v:minimal /nologo *> "$dir\codex-focused-20261030-98.log"
Add-Content "$dir\codex-focused-20261030-98.log" "MSBUILD_EXIT=$LASTEXITCODE"

