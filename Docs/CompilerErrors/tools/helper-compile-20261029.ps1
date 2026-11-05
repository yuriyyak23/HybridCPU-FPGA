$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$log = Join-Path $root 'CompilerErrors\helper-compile-20261029.log'
$console = Join-Path $root 'CompilerErrors\helper-compile-20261029.console.log'
$source = Join-Path $root 'CloseToHSL\\Core\Pipeline\Metadata\ReadRangeMetadataHelper.cpp'
$intDir = Join-Path $root 'CompilerErrors\helper-obj\'
& 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe' (Join-Path $root 'DPCPPConsoleApplication1.vcxproj') /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=$intDir" /v:minimal /nologo /fl "/flp:logfile=$log;verbosity=normal" *> $console
$code = $LASTEXITCODE
Add-Content -Path $console -Value "MSBUILD_EXIT=$code"
exit $code
