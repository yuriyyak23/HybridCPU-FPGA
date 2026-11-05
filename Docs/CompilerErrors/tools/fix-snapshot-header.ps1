$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core'
$p=Join-Path $root 'Pipeline\Certificates\ReplayPhaseSubstrate.h'
$s=[IO.File]::ReadAllText($p).Replace('using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;', '')
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'Runtime\Events\Hypercalls\Governance\VirtualizationOperationOwnerSnapshot.h'
$s=[IO.File]::ReadAllText($p)
$anchor='enum class VirtualizationOperationOwnerSnapshotLoadDecision : unsigned char'
$decl="class VirtualizationDecisionSpecV2;`r`n    class VirtualizationDecisionOwnerMapEntryV2;`r`n    class VirtualizationDecisionAcceptanceRecordV2;`r`n    class AcceptedVirtualizationDecision;`r`n    class HypercallRuntimeOwnerAllocation;`r`n    class VirtualizationOperationOwnerSnapshotLoadResult;`r`n`r`n    "
if (-not $s.Contains('class VirtualizationDecisionSpecV2;')) { if (-not $s.Contains($anchor)) { throw 'Anchor missing' }; $s=$s.Replace($anchor,$decl+$anchor) }
$s=$s.Replace('ImmutableArray<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>>','std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>>')
$s=$s.Replace('static inline const std::vector<unsigned char> Envelope = Encoding::ASCII->GetBytes(L"HCPUO1\0");',"static inline const std::vector<unsigned char> Envelope = {'H', 'C', 'P', 'U', 'O', '1', 0};")
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'Runtime\Events\Hypercalls\Governance\VirtualizationOperationOwnerSnapshot.cpp'
$s=[IO.File]::ReadAllText($p).Replace('ImmutableArray<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>>','std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>>')
[IO.File]::WriteAllText($p,$s)
