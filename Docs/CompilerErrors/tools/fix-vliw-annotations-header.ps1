$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Contracts\CompilerTransport'
$p=Join-Path $r 'VliwBundleAnnotations.h'; $s=[IO.File]::ReadAllText($p)
$m='static inline const std::shared_ptr<VliwBundleAnnotations> Empty = std::make_shared<VliwBundleAnnotations>(Array::Empty<std::shared_ptr<InstructionSlotMetadata>>(), YAKSys_Hybrid_CPU::Core::BundleMetadata::Default);'
if(-not $s.Contains($m)){throw 'Expected Empty declaration missing'}
$s=$s.Replace($m,'static const std::shared_ptr<VliwBundleAnnotations> Empty;')
$s=$s.Replace('namespace HybridCPU_ISE::Arch',"namespace YAKSys_Hybrid_CPU::Core { class BundleMetadata; }`r`nnamespace HybridCPU_ISE::Arch { class InstructionSlotMetadata; }`r`n`r`nnamespace HybridCPU_ISE::Arch")
$s=$s.Replace('std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata','const std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'VliwBundleAnnotations.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata','const std::vector<std::shared_ptr<InstructionSlotMetadata>> &slotMetadata')
$m='namespace HybridCPU_ISE::Arch'; $idx=$s.IndexOf($m); $brace=$s.IndexOf('{',$idx)
if($idx -lt 0 -or $brace -lt 0){throw 'Expected namespace missing'}
$def="`r`n    const std::shared_ptr<VliwBundleAnnotations> VliwBundleAnnotations::Empty = std::make_shared<VliwBundleAnnotations>(std::vector<std::shared_ptr<InstructionSlotMetadata>>{}, YAKSys_Hybrid_CPU::Core::BundleMetadata::Default);`r`n"
$s=$s.Insert($brace+1,$def)
$s=$s.Replace('#include "VliwBundleAnnotations.h"',"#include `"VliwBundleAnnotations.h`"`r`n#include `"../../Pipeline/Metadata/BundleMetadata.h`"`r`n#include `"InstructionSlotMetadata.h`"")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
