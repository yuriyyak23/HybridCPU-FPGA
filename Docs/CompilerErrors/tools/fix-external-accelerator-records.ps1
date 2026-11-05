# Idempotent validation for C# records restored as C++ declarations.
$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$checks = @{
  'CloseToHSL\\Core\Execution\ExternalAccelerators\Tokens\AcceleratorTokenStatusWord.h' = 'struct AcceleratorTokenStatusWord final'
  'CloseToHSL\\Core\Execution\ExternalAccelerators\Tokens\AcceleratorTokenResults.h' = 'class AcceleratorTokenLookupResult final'
  'CloseToHSL\\Core\Execution\ExternalAccelerators\Commit\AcceleratorCommitModel.h' = 'class AcceleratorCommitResult final'
  'CloseToHSL\\Core\Execution\ExternalAccelerators\Fences\AcceleratorFenceModel.h' = 'class AcceleratorFenceResult final'
  'CloseToHSL\\Core\Execution\ExternalAccelerators\ExternalAcceleratorRuntime.h' = 'class ExternalAcceleratorRuntimeCommandResult final'
  'CloseToHSL\\Core\Execution\ExternalAccelerators\Backends\ExternalAcceleratorBackends.h' = 'class AcceleratorBackendResult;'
}
foreach ($relative in $checks.Keys) {
  $path = Join-Path $root $relative
  if (-not (Select-String -LiteralPath $path -SimpleMatch $checks[$relative] -Quiet)) { throw "Restored declaration missing: $path" }
}
Write-Host 'Restored accelerator declaration checks passed.'
