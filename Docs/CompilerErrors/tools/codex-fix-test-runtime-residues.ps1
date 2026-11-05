$ErrorActionPreference = 'Stop'
$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1'
$file = Join-Path $root 'CloseToHSL\\Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.cpp'
$content = [IO.File]::ReadAllText($file)
$required = @(
  'return temp_var106;',
  'getRuntime()->getDecode()->Decode.Clear();',
  'getRuntime()->getExecution()->Execute.Clear();',
  'getRuntime()->getMemoryPipeline()->Memory.Clear();',
  'getRuntime()->getMemoryPipeline()->WriteBack.Clear();',
  'return getRuntime()->getDecode()->Decode.MicroOp;',
  'return getRuntime()->getAdmission()->Handoff;',
  'execute.SetLane(0, std::move(lane));'
)
foreach ($needle in $required) { if (-not $content.Contains($needle)) { throw "Expected translated code not found; refusing to overwrite user edits: $needle" } }
$forbidden = @()
foreach ($needle in $forbidden) { if ($content.Contains($needle)) { throw "Legacy residue remains; refusing to mark fix as applied: $needle" } }
Write-Output 'Processor test/runtime translation block is present and validated; no file changes were needed.'
