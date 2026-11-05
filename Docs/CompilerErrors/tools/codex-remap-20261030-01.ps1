$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$dir=Join-Path $root 'CompilerErrors'
$log=Join-Path $dir 'codex-remap-20261030-01.log'
@('CloseToHSL\\Core\Runtime\Events\Injection\InterruptRemapPolicy.cpp') | ForEach-Object {
 Add-Content $log "SOURCE=$_"
 & 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$_" "/p:IntDir=$dir\codex-remap-obj\" /m:1 /v:minimal /nologo *>> $log
 Add-Content $log "MSBUILD_EXIT=$LASTEXITCODE"
}
