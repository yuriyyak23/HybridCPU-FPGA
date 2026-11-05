$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$dir=Join-Path $root 'CompilerErrors'
$log=Join-Path $dir 'codex-lane6-20261030-01.log'
@('CloseToHSL\\Core\Runtime\Lanes\Lane6\Lane6QueueRuntime.cpp','CloseToHSL\\Core\Runtime\Lanes\Lane6\Lane6StateBlock.cpp','CloseToHSL\\Core\Runtime\Lanes\Lane6\HostOwnedEvidence\Lane6HostOwnedEvidenceStore.cpp') | ForEach-Object {
 Add-Content $log "SOURCE=$_"
 & 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$_" "/p:IntDir=$dir\codex-lane6-20261030-obj\" /m:1 /v:minimal /nologo *>> $log
 Add-Content $log "MSBUILD_EXIT=$LASTEXITCODE"
}
