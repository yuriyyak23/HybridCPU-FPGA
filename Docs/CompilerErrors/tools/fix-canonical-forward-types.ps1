$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder\CanonicalDecodedContracts.h'
$s=[IO.File]::ReadAllText($p)
$decl="namespace YAKSys_Hybrid_CPU::Core { class BundleMetadata; }`r`nnamespace YAKSys_Hybrid_CPU::Core::Decoder { class DecodedInstruction; }`r`n"
if (-not $s.Contains('class BundleMetadata;')) { $s=$s.Replace('using namespace HybridCPU_ISE::Arch;', $decl+'using namespace HybridCPU_ISE::Arch;') }
[IO.File]::WriteAllText($p,$s)
