$ErrorActionPreference = 'Continue'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$project = 'H:\HybridCPU_FPGA.vcxproj'
$targets = @(
    'CloseToHSL\\Core\Pipeline\Safety\SafetyVerifier.VirtualizationAdmission.cpp',
    'CloseToHSL\\Core\Runtime\IO\Dma\DmaAuthorityService.cpp',
    'CloseToHSL\\Core\Runtime\IO\Dma\YAKSys_Hybrid_CPU.Core.DmaAuthorityService.cpp',
    'CloseToHSL\\Core\Runtime\Migration\Checkpoint\DomainHypercallLifecycleGate.cpp',
    'CloseToHSL\\Core\Runtime\Migration\Checkpoint\DomainHypercallDrainLifecycleOwner.cpp',
    'CloseToHSL\\Core\Runtime\Completion\Records\DomainHypercallRetireOwner.cpp',
    'CloseToHSL\\Core\Runtime\Events\Hypercalls\DisabledVirtualizationRepositoryOwnerReviewWorkflow.cpp',
    'CloseToHSL\\Core\Runtime\Events\Hypercalls\DomainHypercallExactRuntimeProfile.cpp'
)
$failed = 0
foreach ($source in $targets) {
    $name = [IO.Path]::GetFileNameWithoutExtension($source)
    $log = "H:\CompilerErrors\verify-attached-$name-final-20261101.log"
    "START $(Get-Date -Format o) SOURCE=$source" | Set-Content -LiteralPath $log
    & $msbuild $project /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" '/p:IntDir=H:\CompilerErrors\verify-attached-final-obj\' /m:1 /v:minimal /nologo *>> $log
    $code = $LASTEXITCODE
    "MSBUILD_EXIT=$code" | Add-Content -LiteralPath $log
    if ($code -ne 0) { $failed++; Write-Output "FAILED $source EXIT=$code LOG=$log" }
    else { Write-Output "PASSED $source LOG=$log" }
}
"FOCUSED_FAILURES=$failed"
if ($failed -ne 0) { exit 1 }
