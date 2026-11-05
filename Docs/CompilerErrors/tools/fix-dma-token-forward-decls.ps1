$ErrorActionPreference='Stop'
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute\DmaStreamComputeToken.h'
$s=[IO.File]::ReadAllText($p); $m='namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { class DmaStreamComputeTelemetryCounters; }'
if(-not $s.Contains($m)){throw 'Expected forward declaration missing'}
$decl='namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { class DmaStreamComputeTelemetryCounters; class DmaStreamComputeDescriptor; class DmaStreamComputeFaultRecord; class DmaStreamComputeTokenLifecycleEvidence; class DmaStreamComputeTokenAdmissionResult; class DmaStreamComputeCommitResult; class DmaStreamComputeOwnerGuardDecision; class DmaStreamComputeMemoryRange; }'
$s=$s.Replace($m,$decl); [IO.File]::WriteAllText($p,$s,[Text.UTF8Encoding]::new($false))
