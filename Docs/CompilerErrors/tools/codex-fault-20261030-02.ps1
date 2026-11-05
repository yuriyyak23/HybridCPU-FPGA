$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$intDir = Join-Path $root 'CompilerErrors\fault-obj-02\'
$targets = Join-Path $root 'CompilerErrors\tools\codex-fault-select.targets'
$log = Join-Path $root 'CompilerErrors\codex-fault-20261030-02.log'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:ForceImportAfterCppTargets=$targets" "/p:IntDir=$intDir" /m:1 /v:minimal /nologo *> $log
$code = $LASTEXITCODE
Add-Content -Path $log -Value "MSBUILD_EXIT=$code"
exit $code



