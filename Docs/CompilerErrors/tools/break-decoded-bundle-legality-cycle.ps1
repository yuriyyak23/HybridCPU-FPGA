$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$p=Join-Path $r 'Decoder\DecodedBundleDescriptor.h'; $s=[IO.File]::ReadAllText($p); $m='#include "../Legality/BundleLegalityDescriptor.h"'
if(-not $s.Contains($m)){throw 'Expected include missing'}
$s=$s.Replace($m,'namespace YAKSys_Hybrid_CPU::Core::Legality { class BundleLegalityDescriptor; }')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Decoder\DecodedBundleDescriptor.cpp'; $s=[IO.File]::ReadAllText($p); $m='#include "DecodedBundleDescriptor.h"'
if(-not $s.Contains($m)){throw 'Expected source include missing'}
$s=$s.Replace($m,$m+"`r`n"+'#include "../Legality/BundleLegalityDescriptor.h"')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Legality\BundleLegalityDescriptor.h'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('BundleMetadata::BundleSlotCount','BundleSlotCount').Replace('std::optional<DecodedBundleDependencySummary> &dependencySummary = std::nullopt','const std::optional<DecodedBundleDependencySummary> &dependencySummary = std::nullopt')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'Legality\BundleLegalityDescriptor.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('std::optional<DecodedBundleDependencySummary> &dependencySummary','const std::optional<DecodedBundleDependencySummary> &dependencySummary')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
