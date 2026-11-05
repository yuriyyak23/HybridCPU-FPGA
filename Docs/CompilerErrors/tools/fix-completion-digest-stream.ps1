$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Completion\Records'; $p=Join-Path $r 'DomainHypercallCompletionOwner.h'; $s=[IO.File]::ReadAllText($p).Replace('std::shared_ptr<MemoryStream> stream','const std::vector<unsigned char> &stream').Replace('std::shared_ptr<Stream> stream','std::vector<unsigned char> &stream'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'DomainHypercallCompletionOwner.cpp'; $s=[IO.File]::ReadAllText($p).Replace('#include "DomainHypercallCompletionOwner.h"',"#include `"DomainHypercallCompletionOwner.h`"`r`n#include `"../../../Decoder/Sha256Digest.h`"`r`n#include <Windows.h>`r`n#include <stdexcept>`r`n#include <limits>")
$pat='(?s)(std::wstring DomainHypercallCompletionDigest::ComputeRecord\(std::shared_ptr<CompletionRecord> record\)\s*\{).*?(?=\r?\n\})'
$rep=@"
`$1
        std::vector<unsigned char> stream = RecordEnvelope;
        stream.push_back(static_cast<unsigned char>(record->getRecordClass()));
        WriteU32(stream, record->getReasonCode()); WriteU64(stream, record->getQualification());
        WriteU64(stream, record->getFaultAddress()); WriteU64(stream, record->getFaultAux());
        return Hash(stream);
    }
    std::wstring DomainHypercallCompletionDigest::ComputeToken(unsigned long long attemptId, int virtualThreadId, unsigned long long domainTag, const std::wstring &decisionId, unsigned long long ownerId, unsigned long long executionSequence, unsigned long long completionSequence, const std::wstring &e3Digest, const std::wstring &effectDigest, const std::wstring &completionDigest, unsigned long long restoreGeneration)
    {
        if (virtualThreadId < 0) throw std::overflow_error("virtualThreadId");
        std::vector<unsigned char> stream = TokenEnvelope;
        WriteU64(stream, attemptId); WriteU32(stream, static_cast<unsigned int>(virtualThreadId));
        WriteU64(stream, domainTag); WriteText(stream, decisionId); WriteU64(stream, ownerId);
        WriteU64(stream, executionSequence); WriteU64(stream, completionSequence);
        auto appendHex = [&](const std::wstring& hex) {
            if (hex.size() % 2) throw std::invalid_argument("Invalid hexadecimal digest");
            auto digit = [](wchar_t c) -> unsigned char {
                if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
                if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
                if (c >= L'A' && c <= L'F') return static_cast<unsigned char>(c - L'A' + 10);
                throw std::invalid_argument("Invalid hexadecimal digest");
            };
            for (size_t i = 0; i < hex.size(); i += 2) stream.push_back(static_cast<unsigned char>((digit(hex[i]) << 4) | digit(hex[i + 1])));
        };
        appendHex(e3Digest); appendHex(effectDigest); appendHex(completionDigest);
        stream.push_back(static_cast<unsigned char>(EvidenceVisibilityClass::HostOwnedRuntimeEvidence));
        stream.push_back(static_cast<unsigned char>(TrapCompletionMigrationClass::HostOwnedNonMigratable));
        WriteU64(stream, restoreGeneration); return Hash(stream);
    }
    std::wstring DomainHypercallCompletionDigest::Hash(const std::vector<unsigned char>& stream)
    { return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(stream); }
    void DomainHypercallCompletionDigest::WriteText(std::vector<unsigned char>& stream, const std::wstring& value)
    {
        if (value.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::overflow_error("text length");
        int count = value.empty() ? 0 : WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
        if (!value.empty() && !count) throw std::invalid_argument("UTF-8 conversion failed");
        WriteU32(stream, static_cast<unsigned int>(count)); auto start = stream.size(); stream.resize(start + count);
        if (count && !WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), reinterpret_cast<char*>(stream.data() + start), count, nullptr, nullptr)) throw std::invalid_argument("UTF-8 conversion failed");
    }
    void DomainHypercallCompletionDigest::WriteU32(std::vector<unsigned char>& stream, unsigned int value)
    { for (int shift = 24; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
    void DomainHypercallCompletionDigest::WriteU64(std::vector<unsigned char>& stream, unsigned long long value)
    { for (int shift = 56; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
"@
$t=[regex]::Replace($s,$pat,$rep); if($t -eq $s){throw 'digest block not found'}; [IO.File]::WriteAllText($p,$t)
