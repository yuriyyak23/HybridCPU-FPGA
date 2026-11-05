$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Admission\Memory'; $p=Join-Path $r 'MemoryDomainRuntime.AddressSpaceGeneration.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include <memory>')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include <memory>`r`n#include <string>")}; $anchor='enum class MemoryDomainSourceCaptureDecision'; $def=@"
class MemoryDomainDescriptor;
class MemoryDomainSourceCaptureResult;
struct MemoryDomainSourceBindResult final {
    MemoryDomainSourceBindDecision Decision;
    std::shared_ptr<MemoryDomainDescriptor> Descriptor;
    unsigned long long AddressSpaceGeneration;
    std::wstring Reason;
    bool isBound() const noexcept { return Decision == MemoryDomainSourceBindDecision::Bound && Descriptor != nullptr && AddressSpaceGeneration != 0; }
};

"@; if(-not $s.Contains('struct MemoryDomainSourceBindResult final')) {$s=$s.Replace($anchor,$def+$anchor)}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'YAKSys_Hybrid_CPU.Core.MemoryDomainRuntime.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('struct MemoryDomainSourceCaptureResult final')) {$s += @"

namespace YAKSys_Hybrid_CPU::Core {
struct MemoryDomainSourceCaptureResult final {
    MemoryDomainSourceCaptureDecision Decision;
    std::shared_ptr<MemoryDomainRuntime::SourceCapture> Capture;
    std::wstring Reason;
    bool isCaptured() const noexcept { return Decision == MemoryDomainSourceCaptureDecision::Captured && Capture != nullptr; }
};
}
"@ }; [IO.File]::WriteAllText($p,$s)
