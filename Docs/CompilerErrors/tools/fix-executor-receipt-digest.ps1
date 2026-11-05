$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Events\Hypercalls'
$p=Join-Path $root 'DomainHypercallRuntimeExecutor.h'; $s=[IO.File]::ReadAllText($p).Replace('std::shared_ptr<Stream> stream','std::vector<unsigned char> &stream'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $root 'DomainHypercallRuntimeExecutor.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "DomainHypercallRuntimeExecutor.h"',"#include `"DomainHypercallRuntimeExecutor.h`"`r`n#include `"../../../Decoder/Sha256Digest.h`"`r`n#include <Windows.h>`r`n#include <stdexcept>`r`n#include <limits>")
$pat='(?s)(std::wstring DomainHypercallExecutionReceiptDigest::Compute\([^\r\n]+\)\s*\{).*?(?=\r?\n\})'
$rep=@"
`$1
        if (virtualThreadId < 0) throw std::overflow_error("virtualThreadId");
        std::vector<unsigned char> stream = Envelope;
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
        WriteU64(stream, sequence); appendHex(e2Digest); WriteU64(stream, attemptId);
        WriteU32(stream, static_cast<unsigned int>(virtualThreadId)); WriteU64(stream, domainTag);
        WriteText(stream, decisionId); WriteU64(stream, ownerId); WriteU32(stream, ownerPolicyVersion);
        WriteU32(stream, ownerEpoch); WriteText(stream, operationNamespace); WriteText(stream, operationId);
        WriteU16(stream, numericLeaf); WriteU64(stream, restoreGeneration);
        stream.push_back(static_cast<unsigned char>(effectClass)); stream.push_back(static_cast<unsigned char>(resultAbi));
        appendHex(effectDigest); appendHex(resultDigest);
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(stream);
    }
    std::wstring DomainHypercallExecutionReceiptDigest::HashConstant(const std::wstring &value)
    {
        std::vector<unsigned char> bytes;
        for (wchar_t c : value) bytes.push_back(c <= 127 ? static_cast<unsigned char>(c) : static_cast<unsigned char>('?'));
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(bytes);
    }
    void DomainHypercallExecutionReceiptDigest::WriteText(std::vector<unsigned char> &stream, const std::wstring &value)
    {
        if (value.size() > static_cast<size_t>((std::numeric_limits<int>::max)())) throw std::overflow_error("text length");
        int size = value.empty() ? 0 : WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
        if (!value.empty() && size == 0) throw std::invalid_argument("UTF-8 conversion failed");
        WriteU32(stream, static_cast<unsigned int>(size));
        auto start = stream.size(); stream.resize(start + size);
        if (size && !WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), reinterpret_cast<char*>(stream.data() + start), size, nullptr, nullptr)) throw std::invalid_argument("UTF-8 conversion failed");
    }
    void DomainHypercallExecutionReceiptDigest::WriteU16(std::vector<unsigned char> &stream, unsigned short value)
    { for (int shift = 8; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
    void DomainHypercallExecutionReceiptDigest::WriteU32(std::vector<unsigned char> &stream, unsigned int value)
    { for (int shift = 24; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
    void DomainHypercallExecutionReceiptDigest::WriteU64(std::vector<unsigned char> &stream, unsigned long long value)
    { for (int shift = 56; shift >= 0; shift -= 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
"@
$t=[regex]::Replace($s,$pat,$rep); if($t -eq $s){throw 'digest block not found'}; [IO.File]::WriteAllText($p,$t)
