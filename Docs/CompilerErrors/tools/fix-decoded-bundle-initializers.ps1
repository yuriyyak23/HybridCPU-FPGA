$ErrorActionPreference='Stop'
$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Decoder'
$p=Join-Path $r 'DecodedBundleDescriptor.h'; $s=[IO.File]::ReadAllText($p)
$m='SlotPlacementMetadata {RequiredSlotClass = SlotClass::Unclassified, PinningKind = SlotPinningKind::HardPinned, PinnedLaneId = 0, DomainTag = 0}'
if(-not $s.Contains($m)){throw 'Expected placement initializer missing'}
$s=$s.Replace($m,'SlotPlacementMetadata {.RequiredSlotClass = SlotClass::Unclassified, .PinningKind = SlotPinningKind::HardPinned, .PinnedLaneId = 0, .DomainTag = 0}')
$s=$s.Replace('std::optional<bool> &isVectorOpOverride = std::nullopt','const std::optional<bool> &isVectorOpOverride = std::nullopt')
$m='DecodedBundleDependencySummary() : _systemBarrierConflictMask(0), _pinnedLaneConflictMask(0)'
if(-not $s.Contains($m)){throw 'Expected default constructor missing'}
$s=$s.Replace($m,'DecodedBundleDependencySummary() : ReadRegisterMask(0), WriteRegisterMask(0), AggregateResourceMask(), RawDependencyMask(0), WawDependencyMask(0), WarDependencyMask(0), ControlConflictMask(0), MemoryConflictMask(0), ScalarClusterEligibleMask(0), _systemBarrierConflictMask(0), _pinnedLaneConflictMask(0)')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
$p=Join-Path $r 'DecodedBundleDescriptor.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('std::optional<bool> &isVectorOpOverride','const std::optional<bool> &isVectorOpOverride')
[IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
