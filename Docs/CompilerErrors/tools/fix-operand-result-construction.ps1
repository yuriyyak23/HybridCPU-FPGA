$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls'
$p=Join-Path $root 'VirtualizationOperandSnapshot.cpp'
$s=[IO.File]::ReadAllText($p)
$s=$s.Replace('return new(VirtualizationOperandCaptureDecision::Captured, snapshot, L"Canonical Rs1 value captured once after E1 validation.");','return std::make_shared<VirtualizationOperandCaptureResult>(VirtualizationOperandCaptureDecision::Captured, snapshot, L"Canonical Rs1 value captured once after E1 validation.");')
$s=$s.Replace('return new(VirtualizationOperandValidationDecision::ValidForE2Input, L"Operand snapshot is immutable, current and eligible only as future E2 input.");','return std::make_shared<VirtualizationOperandValidationResult>(VirtualizationOperandValidationDecision::ValidForE2Input, L"Operand snapshot is immutable, current and eligible only as future E2 input.");')
$s=$s.Replace('return new(decision, nullptr, reason);','return std::make_shared<VirtualizationOperandCaptureResult>(decision, nullptr, reason);')
$s=$s.Replace('return new(decision, reason);','return std::make_shared<VirtualizationOperandValidationResult>(decision, reason);')
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'VirtualizationOperandSnapshot.h'
$s=[IO.File]::ReadAllText($p).Replace('std::mutex _gate;', 'mutable std::mutex _gate;')
[IO.File]::WriteAllText($p,$s)
