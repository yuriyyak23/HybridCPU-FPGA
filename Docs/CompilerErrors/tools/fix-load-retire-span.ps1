$dir = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Memory'
foreach ($file in @('MicroOp.LoadStore.h','MicroOp.LoadStore.cpp')) {
  $path = Join-Path $dir $file
  $s = [IO.File]::ReadAllText($path)
  $old = 'Span<RetireRecord> retireRecords'
  if (-not $s.Contains($old)) { throw "Expected signature not found: $file" }
  $s = $s.Replace($old, 'std::span<RetireRecord> retireRecords')
  if ($file -eq 'MicroOp.LoadStore.h') { $s = $s.Replace('#pragma once', "#pragma once`r`n#include <span>") }
  [IO.File]::WriteAllText($path, $s, [Text.UTF8Encoding]::new($false))
}
