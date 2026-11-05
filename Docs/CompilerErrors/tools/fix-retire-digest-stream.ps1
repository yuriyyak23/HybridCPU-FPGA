$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records'; $p=Join-Path $r 'DomainHypercallRetireOwner.h'; $s=[IO.File]::ReadAllText($p).Replace('std::shared_ptr<Stream> stream','std::vector<unsigned char> &stream'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'DomainHypercallRetireOwner.cpp'; $s=[IO.File]::ReadAllText($p).Replace('#include "DomainHypercallRetireOwner.h"',"#include `"DomainHypercallRetireOwner.h`"`r`n#include `"../../../Decoder/Sha256Digest.h`"`r`n#include <stdexcept>`r`n#include <limits>")
$pat='(?s)(std::wstring DomainHypercallRetireOwner::ComputeDigest\([^\r\n]+\)\s*\{).*?(?=\s*std::shared_ptr<DomainHypercallRetireResult> DomainHypercallRetireOwner::Deny)'
$rep=@"
`$1
        if (eligibility->VirtualThreadId < 0 || eligibility->SourceSlotId < 0 || eligibility->WorkingSlotId < 0 || eligibility->PhysicalLaneId < 0) throw std::overflow_error("negative retire identity");
        std::vector<unsigned char> stream = Envelope;
        WriteU64(stream, sequence); WriteU64(stream, e5->getAttemptId());
        WriteU32(stream, static_cast<unsigned int>(eligibility->VirtualThreadId)); WriteU64(stream, eligibility->DomainTag);
        WriteU32(stream, static_cast<unsigned int>(eligibility->SourceSlotId)); WriteU32(stream, static_cast<unsigned int>(eligibility->WorkingSlotId));
        WriteU64(stream, eligibility->WorkingBundleSequence); WriteU64(stream, eligibility->OperationAttempt);
        WriteU32(stream, static_cast<unsigned int>(eligibility->PhysicalLaneId));
        WriteU64(stream, eligibility->RetireWindowIdentity); WriteU64(stream, eligibility->OrderEpoch); WriteU64(stream, e5->getRestoreGeneration());
        const auto hex = e5->getTokenDigest();
        if (hex.size() % 2 || hex.size() / 2 > (std::numeric_limits<unsigned int>::max)()) throw std::invalid_argument("Invalid hexadecimal digest");
        auto digit = [](wchar_t c) -> unsigned char {
            if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
            if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
            if (c >= L'A' && c <= L'F') return static_cast<unsigned char>(c - L'A' + 10);
            throw std::invalid_argument("Invalid hexadecimal digest");
        };
        WriteU32(stream, static_cast<unsigned int>(hex.size() / 2));
        for (size_t i = 0; i < hex.size(); i += 2) stream.push_back(static_cast<unsigned char>((digit(hex[i]) << 4) | digit(hex[i + 1])));
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(stream);
    }
    void DomainHypercallRetireOwner::WriteU32(std::vector<unsigned char>& stream, unsigned int value)
    { for (int shift = 0; shift <= 24; shift += 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
    void DomainHypercallRetireOwner::WriteU64(std::vector<unsigned char>& stream, unsigned long long value)
    { for (int shift = 0; shift <= 56; shift += 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }

"@
$t=[regex]::Replace($s,$pat,$rep); if($t -eq $s){throw 'digest block missing'}; [IO.File]::WriteAllText($p,$t)
