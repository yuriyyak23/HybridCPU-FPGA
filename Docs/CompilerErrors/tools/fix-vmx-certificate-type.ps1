$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\MicroOps\Types'
$old = 'SafetyVerifier::VirtualizationAdmissionCertificate'
$new = 'VirtualizationAdmissionCertificate'
foreach ($file in @('MicroOp.IO.h', 'MicroOp.IO.cpp')) {
  $path = Join-Path $root $file
  $text = [IO.File]::ReadAllText($path)
  if (-not $text.Contains($old)) { throw "Expected type not found: $file" }
  $text = $text.Replace($old, $new)
  if ($file -eq 'MicroOp.IO.h') {
    $marker = 'namespace YAKSys_Hybrid_CPU::Core' + "`r`n" + '{'
    if (-not $text.Contains($marker)) { throw 'Expected namespace not found' }
    $text = $text.Replace($marker, $marker + "`r`n`tclass VirtualizationAdmissionCertificate;")
  }
  [IO.File]::WriteAllText($path, $text, [Text.UTF8Encoding]::new($false))
}
