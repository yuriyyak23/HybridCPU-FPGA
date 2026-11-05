$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls\VirtualizationOperandSnapshot.h'
$s=[IO.File]::ReadAllText($p)
$anchor="`t/// <summary>`r`n`t/// Immutable one-attempt operand identity"
if (-not $s.Contains($anchor)) { $anchor="`t/// <summary>`n`t/// Immutable one-attempt operand identity" }
$insert=@"
    class VirtualizationOperandSnapshot;

    struct VirtualizationOperandCaptureResult final
    {
        const VirtualizationOperandCaptureDecision Decision;
        const std::shared_ptr<VirtualizationOperandSnapshot> Snapshot;
        const std::wstring Reason;

        VirtualizationOperandCaptureResult(VirtualizationOperandCaptureDecision decision,
            std::shared_ptr<VirtualizationOperandSnapshot> snapshot, std::wstring reason)
            : Decision(decision), Snapshot(std::move(snapshot)), Reason(std::move(reason)) {}
        bool isCaptured() const noexcept
        {
            return Decision == VirtualizationOperandCaptureDecision::Captured && Snapshot != nullptr;
        }
    };

    struct VirtualizationOperandValidationResult final
    {
        const VirtualizationOperandValidationDecision Decision;
        const std::wstring Reason;

        VirtualizationOperandValidationResult(VirtualizationOperandValidationDecision decision, std::wstring reason)
            : Decision(decision), Reason(std::move(reason)) {}
        bool isValidForE2Input() const noexcept
        {
            return Decision == VirtualizationOperandValidationDecision::ValidForE2Input;
        }
    };

"@
if (-not $s.Contains('struct VirtualizationOperandCaptureResult final')) { if (-not $s.Contains($anchor)) { throw 'Anchor missing' }; $s=$s.Replace($anchor,$insert+$anchor) }
if (-not $s.Contains('#include <utility>')) { $s=$s.Replace('#include <memory>',"#include <memory>`r`n#include <utility>") }
[IO.File]::WriteAllText($p,$s)
