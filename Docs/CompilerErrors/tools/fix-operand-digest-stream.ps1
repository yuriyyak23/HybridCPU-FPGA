$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls'
$h=Join-Path $root 'VirtualizationOperandSnapshot.h'; $s=[IO.File]::ReadAllText($h); $s=$s.Replace('std::shared_ptr<Stream> stream','std::vector<unsigned char> &stream'); [IO.File]::WriteAllText($h,$s)
$p=Join-Path $root 'VirtualizationOperandSnapshot.cpp'; $s=[IO.File]::ReadAllText($p)
if(-not $s.Contains('#include "../../../Decoder/Sha256Digest.h"')) { $s=$s.Replace('#include "VirtualizationOperandSnapshot.h"',"#include `"VirtualizationOperandSnapshot.h`"`r`n#include `"../../../Decoder/Sha256Digest.h`"`r`n#include <stdexcept>") }
$pattern='(?s)(std::wstring VirtualizationOperandSnapshotDigest::Compute\([^\n]+\)\s*\{).*?(?=\s*// C# TO C\+\+ CONVERTER WARNING: Nullable reference types have no equivalent in C\+\+:\s*\r?\n// ORIGINAL LINE: internal VirtualizationOperandCaptureResult)'
$body=@"
`$1
        std::vector<unsigned char> bytes = Envelope;
        WriteUInt64(bytes, attemptId); WriteUInt64(bytes, issuerGeneration);
        WriteUInt32(bytes, static_cast<unsigned int>(virtualThreadId));
        WriteUInt32(bytes, static_cast<unsigned int>(ownerContextId));
        WriteUInt64(bytes, domainTag); bytes.push_back(rs1Selector); WriteUInt64(bytes, rs1Value);
        bytes.push_back(rs2Selector); WriteUInt64(bytes, rs2Value); bytes.push_back(rdSelector);
        WriteUInt32(bytes, static_cast<unsigned int>(sourceSlotId));
        WriteUInt32(bytes, static_cast<unsigned int>(workingSlotId));
        WriteUInt64(bytes, bundleIdentity); WriteUInt64(bytes, replayEpoch);
        WriteUInt64(bytes, carrierIdentityDigest); WriteUInt64(bytes, restoreGeneration);
        WriteUInt64(bytes, captureSequence);
        if (ownerPolicyDigest.size() % 2 != 0) throw std::invalid_argument("Invalid hexadecimal digest");
        auto digit = [](wchar_t c) -> unsigned char {
            if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
            if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
            if (c >= L'A' && c <= L'F') return static_cast<unsigned char>(c - L'A' + 10);
            throw std::invalid_argument("Invalid hexadecimal digest");
        };
        for (size_t i = 0; i < ownerPolicyDigest.size(); i += 2)
            bytes.push_back(static_cast<unsigned char>((digit(ownerPolicyDigest[i]) << 4) | digit(ownerPolicyDigest[i + 1])));
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(bytes);
    }
    void VirtualizationOperandSnapshotDigest::WriteUInt32(std::vector<unsigned char> &stream, unsigned int value)
    {
        for (int shift = 24; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift));
    }
    void VirtualizationOperandSnapshotDigest::WriteUInt64(std::vector<unsigned char> &stream, unsigned long long value)
    {
        for (int shift = 56; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift));
    }
"@
$t=[regex]::Replace($s,$pattern,$body); if($t -eq $s){throw 'Digest block not found'}; [IO.File]::WriteAllText($p,$t)
