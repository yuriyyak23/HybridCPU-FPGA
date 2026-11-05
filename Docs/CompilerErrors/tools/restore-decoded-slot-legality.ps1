$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$h=Join-Path $r 'Pipeline\Metadata\BundleWidth.h'
if(Test-Path -LiteralPath $h){throw 'BundleWidth header already exists'}
[IO.File]::WriteAllText($h,"#pragma once`r`nnamespace YAKSys_Hybrid_CPU::Core { inline constexpr int BundleSlotCount = 8; }`r`n",[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Legality\DecodedSlotLegality.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "../Decoder/HazardEffectKind.h"',"#include `"../Decoder/HazardEffectKind.h`"`r`n#include `"../Pipeline/Metadata/BundleWidth.h`"")
$s=$s.Replace('BundleMetadata::BundleSlotCount','BundleSlotCount')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Legality\DecodedSlotLegality.cpp'; $s=[IO.File]::ReadAllText($p)
$start=$s.IndexOf("`tDecodedSlotLegality::DecodedSlotLegality("); $end=$s.IndexOf("`tint DecodedSlotLegality::getSlotIndex()",$start)
if($start -lt 0 -or $end -lt 0){throw 'Constructor block missing'}
$replacement=@(
'    DecodedSlotLegality::DecodedSlotLegality(int slotIndex, bool isOccupied, YAKSys_Hybrid_CPU::Core::SlotClass slotClass, unsigned char hardRejectPeerMask, unsigned char needsRuntimeCheckPeerMask, HazardEffectKind dominantEffectKind)',
'        : SlotIndex(slotIndex), IsOccupied(isOccupied), SlotClass(isOccupied ? slotClass : YAKSys_Hybrid_CPU::Core::SlotClass::Unclassified),',
'          HardRejectPeerMask(isOccupied ? hardRejectPeerMask : 0), NeedsRuntimeCheckPeerMask(isOccupied ? needsRuntimeCheckPeerMask : 0), DominantEffectKind(dominantEffectKind)',
'    {',
'        ValidateSlotIndex(slotIndex);',
'        ValidatePeerMask(L"hardRejectPeerMask", slotIndex, hardRejectPeerMask);',
'        ValidatePeerMask(L"needsRuntimeCheckPeerMask", slotIndex, needsRuntimeCheckPeerMask);',
'    }',
'') -join "`r`n"
$s=$s.Substring(0,$start)+$replacement+$s.Substring($end)
$s=$s.Replace('return new(slotIndex, isOccupied: false);','return DecodedSlotLegality(slotIndex, false);').Replace('BundleMetadata::BundleSlotCount','BundleSlotCount')
$s=[regex]::Replace($s,'throw ArgumentOutOfRangeException\(L"slotIndex", slotIndex, std::format\([^;]+;','throw std::out_of_range("Slot index is outside bundle width.");')
$s=[regex]::Replace($s,'throw ArgumentOutOfRangeException\(paramName, peerMask, std::format\([^;]+;','throw std::out_of_range("Peer mask sets bits outside bundle width.");')
$s=$s.Replace('throw ArgumentOutOfRangeException(paramName, peerMask, L"Peer mask must not include the slot itself.");','throw std::out_of_range("Peer mask must not include the slot itself.");')
$s=$s.Replace('#include "DecodedSlotLegality.h"',"#include `"DecodedSlotLegality.h`"`r`n#include <stdexcept>")
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
