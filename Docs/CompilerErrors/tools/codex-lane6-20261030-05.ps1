$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$dir=Join-Path $root 'CompilerErrors'
$log=Join-Path $dir 'codex-lane6-20261030-05.log'
$files=Get-ChildItem (Join-Path $root 'CloseToHSL\\Core\Runtime\Lanes\Lane6') -Filter '*.cpp' -Recurse | Where-Object { $_.Name -notin @('Lane6QueueRuntime.cpp','Lane6StateBlock.cpp','Lane6HostOwnedEvidenceStore.cpp') }

foreach ($file in $files) {
 $source=$file.FullName.Substring($root.Length+1)
 Add-Content $log "SOURCE=$source"
 & 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=$dir\codex-lane6-remaining-obj\" /m:1 /v:minimal /nologo *>> $log
 Add-Content $log "MSBUILD_EXIT=$LASTEXITCODE"
}
