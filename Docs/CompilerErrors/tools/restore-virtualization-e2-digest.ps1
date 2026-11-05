$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Pipeline\Safety';$h=Join-Path $dir 'SafetyVerifier.VirtualizationOperationAdmission.h';$c=Join-Path $dir 'SafetyVerifier.VirtualizationOperationAdmission.cpp';$s=[IO.File]::ReadAllText($h)
$start=$s.IndexOf("`tprivate:`r`n`t`tstatic void WriteText",$s.IndexOf('class VirtualizationE2Digest'));$end=$s.IndexOf("`r`n`t};",$start);if($start -lt 0 -or $end -lt 0){throw 'Helper declarations missing'};$s=$s.Remove($start,$end-$start)
$cpp=@"
#include "SafetyVerifier.VirtualizationOperationAdmission.h"
#include "../../Decoder/Sha256Digest.h"
#define NOMINMAX
#include <Windows.h>
#include <climits>
#include <limits>
#include <stdexcept>
#include <string>

namespace YAKSys_Hybrid_CPU::Core {
namespace {
void WriteInteger(std::vector<unsigned char>& out, unsigned long long value, int bytes) {
    for (int i = bytes - 1; i >= 0; --i) out.push_back(static_cast<unsigned char>(value >> (i * 8)));
}
void WriteText(std::vector<unsigned char>& out, const std::wstring& value) {
    if (value.size() > INT_MAX) throw std::length_error("UTF-16 input too long");
    int n = value.empty() ? 0 : WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    if (!value.empty() && !n) throw std::runtime_error("UTF-8 conversion failed");
    std::string bytes(static_cast<std::size_t>(n), '\0');
    if (n && !WideCharToMultiByte(CP_UTF8, 0, value.data(), static_cast<int>(value.size()), bytes.data(), n, nullptr, nullptr)) throw std::runtime_error("UTF-8 conversion failed");
    WriteInteger(out, static_cast<unsigned int>(bytes.size()), 4);
    out.insert(out.end(), bytes.begin(), bytes.end());
}
void WriteHex(std::vector<unsigned char>& out, const std::wstring& text) {
    if (text.size() % 2 != 0) throw std::invalid_argument("Odd hex length");
    auto digit=[](wchar_t c)->unsigned char { if(c>=L'0' && c<=L'9') return static_cast<unsigned char>(c-L'0'); if(c>=L'a' && c<=L'f') return static_cast<unsigned char>(c-L'a'+10); if(c>=L'A' && c<=L'F') return static_cast<unsigned char>(c-L'A'+10); throw std::invalid_argument("Invalid hex"); };
    for (std::size_t i=0; i<text.size(); i+=2) out.push_back(static_cast<unsigned char>((digit(text[i]) << 4) | digit(text[i+1])));
}
}
std::wstring VirtualizationE2Digest::Compute(unsigned long long sequence, unsigned long long attemptId, unsigned long long e1Generation, int vt, int ownerContext, unsigned long long domain, unsigned long long bundle, unsigned long long replay, const std::wstring& decisionId, const std::wstring& specDigest, const std::wstring& acceptanceDigest, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch, const std::wstring& operationNamespace, const std::wstring& operationId, unsigned short leaf, const std::wstring& ownerPolicyDigest, const std::wstring& operandDigest, unsigned long long grantIdentity, unsigned long long grantGeneration, unsigned long long rootEpoch, unsigned long long restoreGeneration) {
    auto bytes = Envelope;
    WriteInteger(bytes,sequence,8); WriteInteger(bytes,attemptId,8); WriteInteger(bytes,e1Generation,8);
    WriteInteger(bytes,static_cast<unsigned int>(vt),4); WriteInteger(bytes,static_cast<unsigned int>(ownerContext),4);
    WriteInteger(bytes,domain,8); WriteInteger(bytes,bundle,8); WriteInteger(bytes,replay,8);
    WriteText(bytes,decisionId); WriteHex(bytes,specDigest); WriteHex(bytes,acceptanceDigest);
    WriteInteger(bytes,ownerId,8); WriteInteger(bytes,ownerPolicyVersion,4); WriteInteger(bytes,ownerEpoch,4);
    WriteText(bytes,operationNamespace); WriteText(bytes,operationId); WriteInteger(bytes,leaf,2);
    WriteHex(bytes,ownerPolicyDigest); WriteHex(bytes,operandDigest);
    WriteInteger(bytes,grantIdentity,8); WriteInteger(bytes,grantGeneration,8); WriteInteger(bytes,rootEpoch,8); WriteInteger(bytes,restoreGeneration,8);
    return Decoder::Sha256Hex(bytes);
}
}
"@
[IO.File]::WriteAllText($h,$s,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($c,$cpp,[Text.UTF8Encoding]::new($false))
