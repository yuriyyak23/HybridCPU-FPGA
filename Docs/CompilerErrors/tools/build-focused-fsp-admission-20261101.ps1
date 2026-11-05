$ErrorActionPreference = 'Continue'
$project = 'H:\HybridCPU_FPGA.vcxproj'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$log = 'H:\CompilerErrors\focused-fsp-admission-20261101.log'
$intDir = 'H:\CompilerErrors\focused-fsp-admission-obj\'
$files = @('CloseToHSL\\Core\Pipeline\Scheduling\SlotLegality\SlotAssignment.cpp','CloseToHSL\\Core\Pipeline\Scheduling\Fsp\FspAdmissionPolicy.cpp')
"START $(Get-Date -Format o)" | Set-Content -LiteralPath $log
foreach ($source in $files) { "SOURCE=$source" | Add-Content -LiteralPath $log; & $msbuild $project /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=$intDir" /m:1 /v:minimal /nologo *>> $log; "MSBUILD_EXIT=$LASTEXITCODE" | Add-Content -LiteralPath $log }
"END $(Get-Date -Format o)" | Add-Content -LiteralPath $log
