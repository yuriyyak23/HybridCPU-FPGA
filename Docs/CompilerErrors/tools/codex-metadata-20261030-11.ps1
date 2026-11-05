$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$log = Join-Path $root 'CompilerErrors\codex-metadata-20261030-11.log'
$intDir = Join-Path $root 'CompilerErrors\codex-execution-isolated-obj\'
$folder = 'CloseToHSL\\Core\Execution\ExternalAccelerators\Capabilities'
Get-ChildItem (Join-Path $root $folder) -Filter '*MetadataCapabilityProvider.cpp' | ForEach-Object {
 $source = Join-Path $folder $_.Name
 Add-Content -LiteralPath $log -Value "`nSOURCE=$source"
 & $msbuild (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=$intDir" /m:1 /v:minimal /nologo *>> $log
 Add-Content -LiteralPath $log -Value "MSBUILD_EXIT=$LASTEXITCODE"
}