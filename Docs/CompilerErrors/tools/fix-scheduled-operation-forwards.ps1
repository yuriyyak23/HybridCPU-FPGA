$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$files = @('Execution\Rf07ExecutionOutcomeContracts.h', 'Architecture\Registers\Retire\Rf08RetireEffectIdentityContracts.h', 'Pipeline\Scheduling\PostStageBIssuedAttempt.h')
$decl = 'namespace YAKSys_Hybrid_CPU::Core::Decoder { class ScheduledOperation; }'
foreach ($file in $files) {
  $path = Join-Path $root $file
  $text = [IO.File]::ReadAllText($path)
  if ($text.Contains($decl)) { throw "Already modified: $file" }
  $marker = '// C# TO C++ CONVERTER NOTE: Forward class declarations:'
  if (-not $text.Contains($marker)) { throw "Missing marker: $file" }
  $text = $text.Replace($marker, $decl + "`r`n" + $marker)
  [IO.File]::WriteAllText($path, $text, [Text.UTF8Encoding]::new($false))
}
