$ErrorActionPreference = 'Continue'
$source = 'CloseToHSL\\Core\Runtime\Domains\SecureCompute\Descriptors\Nested\YAKSys_Hybrid_CPU.Core.SecureChildDomainIntentDescriptor.cpp'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$project = 'H:\HybridCPU_FPGA.vcxproj'
$stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
$log = "H:\CompilerErrors\focused-secure-child-domain-$stamp.log"
"START $(Get-Date -Format o) SOURCE=$source" | Set-Content -LiteralPath $log
& $msbuild $project /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=H:\CompilerErrors\secure-child-domain-$stamp-obj\" /m:1 /v:minimal /nologo *>> $log
$code = $LASTEXITCODE
"MSBUILD_EXIT=$code" | Add-Content -LiteralPath $log
Write-Output "MSBUILD_EXIT=$code LOG=$log"
exit $code
