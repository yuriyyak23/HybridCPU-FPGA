$ErrorActionPreference = 'Continue'
$project = 'H:\HybridCPU_FPGA.vcxproj'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$log = 'H:\CompilerErrors\focused-safety-governance-20261101.log'
$intDir = 'H:\CompilerErrors\focused-safety-governance-obj\'
$files = @(
  'CloseToHSL\\Core\Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.cpp',
  'CloseToHSL\\Core\Runtime\Events\Hypercalls\Governance\Phase38VirtualizationDecisionAcceptanceV2.cpp',
  'CloseToHSL\\Core\Runtime\Events\Hypercalls\Governance\Phase38VirtualizationDecisionSpecV2.cpp'
)
"START $(Get-Date -Format o)" | Set-Content -LiteralPath $log
foreach ($file in $files) {
  "SOURCE=$file" | Add-Content -LiteralPath $log
  & $msbuild $project /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$file" "/p:IntDir=$intDir" /m:1 /v:minimal /nologo *>> $log
  "MSBUILD_EXIT=$LASTEXITCODE" | Add-Content -LiteralPath $log
}
"END $(Get-Date -Format o)" | Add-Content -LiteralPath $log
