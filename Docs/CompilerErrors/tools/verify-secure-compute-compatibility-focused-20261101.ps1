$ErrorActionPreference = 'Continue'
$msbuild = 'C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\amd64\MSBuild.exe'
$project = 'H:\HybridCPU_FPGA.vcxproj'
$targets = @(
    'CloseToHSL\\Core\Virtualization\SecureCompute\Compatibility\Frontend\YAKSys_Hybrid_CPU.Core.SecureComputeCompatibilityBoundary.cpp',
    'CloseToHSL\\Core\Virtualization\SecureCompute\Compatibility\Projection\YAKSys_Hybrid_CPU.Core.SecureComputeCompatibilityBoundaryMatrixPolicy.cpp'
)
$failed = 0
foreach ($source in $targets) {
    $name = [IO.Path]::GetFileNameWithoutExtension($source)
    $stamp = Get-Date -Format 'yyyyMMdd-HHmmss-fff'
    $log = "H:\CompilerErrors\verify-secure-compat-$name-$stamp.log"
    "START $(Get-Date -Format o) SOURCE=$source" | Set-Content -LiteralPath $log
    & $msbuild $project /t:ClCompile /p:Configuration=Debug /p:Platform=x64 "/p:SelectedFiles=$source" "/p:IntDir=H:\CompilerErrors\verify-secure-compat-$stamp-obj\" /m:1 /v:minimal /nologo *>> $log
    $code = $LASTEXITCODE
    "MSBUILD_EXIT=$code" | Add-Content -LiteralPath $log
    if ($code -ne 0) { $failed++; Write-Output "FAILED $source EXIT=$code LOG=$log" }
    else { Write-Output "PASSED $source LOG=$log" }
}
"FOCUSED_FAILURES=$failed"
if ($failed -ne 0) { exit 1 }
