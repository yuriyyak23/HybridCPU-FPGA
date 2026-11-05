$path=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'CloseToHSL\\Core\Decoder\CanonicalDecodedContracts.h'
$text=[IO.File]::ReadAllText($path)
foreach($line in @('using namespace YAKSys_Hybrid_CPU::Arch;', 'using namespace YAKSys_Hybrid_CPU::Arch::Generated;', 'using namespace YAKSys_Hybrid_CPU::Core::Pipeline;', 'using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;')) { $text=$text.Replace($line,'') }
[IO.File]::WriteAllText($path,$text,[Text.UTF8Encoding]::new($false))