$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$p=Join-Path $r 'Pipeline\AbstractBundle.h'; $s=[IO.File]::ReadAllText($p); $m='namespace YAKSys_Hybrid_CPU::Core::Pipeline'
if(-not $s.Contains($m) -or $s.Contains('class InstructionIR;')) {throw 'Unexpected AbstractBundle.h state'}
$s=$s.Replace($m,"namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps { class InstructionIR; }`r`n`r`n"+$m); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Decoder\DecodedInstructionBundle.h'; $s=[IO.File]::ReadAllText($p); $m='#include "CanonicalDecodedContracts.h"'
if(-not $s.Contains($m)){throw 'Expected include not found'}
$s=$s.Replace($m,"namespace YAKSys_Hybrid_CPU::Core { class BundleMetadata; }`r`nnamespace YAKSys_Hybrid_CPU::Core::Decoder { class DecodedInstruction; class CanonicalBundle; }")
$s=$s.Replace('slots = const std::vector<DecodedInstruction>()','slots = {}').Replace('std::optional<std::shared_ptr<BundleMetadata>> &bundleMetadata = std::nullopt','const std::optional<std::shared_ptr<BundleMetadata>> &bundleMetadata = std::nullopt')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Decoder\DecodedInstructionBundle.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "DecodedInstructionBundle.h"',"#include `"DecodedInstructionBundle.h`"`r`n#include `"CanonicalDecodedContracts.h`"")
$s=$s.Replace('std::optional<std::shared_ptr<BundleMetadata>> &bundleMetadata','const std::optional<std::shared_ptr<BundleMetadata>> &bundleMetadata')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
