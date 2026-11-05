$root = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Architecture\Registers\Retire'
$h = Join-Path $root 'Rf08RetireEffectIdentityContracts.h'
$c = Join-Path $root 'Rf08RetireEffectIdentityContracts.cpp'
$s = [IO.File]::ReadAllText($h)
$anchor = 'namespace YAKSys_Hybrid_CPU::Core::Registers::Retire { class RetireEffectIdentityContractViolationException; }'
$decl = 'namespace YAKSys_Hybrid_CPU::Core::Decoder { class SourceOperationProvenance; }'
if (-not $s.Contains($decl)) { if (-not $s.Contains($anchor)) { throw 'Forward declaration anchor missing' }; $s = $s.Replace($anchor, "$anchor`r`n$decl") }
$s = $s.Replace('std::optional<int> &architecturalRegisterId', 'std::optional<int> architecturalRegisterId')
$s = [regex]::Replace($s, '(?s)\r?\n\s*protected:\s*std::shared_ptr<RetireEffectIdentityContractViolationException> shared_from_this\(\)\s*\{\s*return std::static_pointer_cast<RetireEffectIdentityContractViolationException>\(InvalidOperationException::shared_from_this\(\)\);\s*\}', '')
[IO.File]::WriteAllText($h, $s)
$s = [IO.File]::ReadAllText($c).Replace('std::optional<int> &architecturalRegisterId', 'std::optional<int> architecturalRegisterId')
[IO.File]::WriteAllText($c, $s)
