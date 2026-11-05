$ErrorActionPreference='Stop'
$p=Join-Path (Split-Path -Parent (Split-Path -Parent $PSScriptRoot)) 'Frontend\\Cache\SemanticBundleCache.cpp'
$s=Get-Content $p -Raw
$s=$s.Replace('#include <chrono>', "#include <chrono>`r`n#include <windows.h>`r`n#include <bcrypt.h>`r`n#include <string>`r`n#pragma comment(lib, `"bcrypt.lib`")")
$marker='    // The JSON byte count must match System.Text.Json; implementation is a separate porting task.'
if (!$s.Contains($marker)) { throw 'JSON insertion marker missing' }
$code=@"
    static void AppendJsonString(std::string& out, const std::wstring& text) {
        constexpr char hex[] = "0123456789ABCDEF"; out.push_back('"');
        for (wchar_t ch : text) {
            unsigned int c = static_cast<unsigned int>(ch);
            switch (c) {
                case '\\': out += "\\\\"; continue;
                case 8: out += "\\b"; continue; case 9: out += "\\t"; continue;
                case 10: out += "\\n"; continue; case 12: out += "\\f"; continue; case 13: out += "\\r"; continue;
                default: break;
            }
            if (c < 32 || c > 126 || c == '"' || c == '&' || c == '\'' || c == '+' || c == '<' || c == '>' || c == '`') {
                out += "\\u"; for (int shift = 12; shift >= 0; shift -= 4) out.push_back(hex[(c >> shift) & 15]);
            } else out.push_back(static_cast<char>(c));
        }
        out.push_back('"');
    }
    static std::string BindingJson(const HotBinding& b) {
        std::string out = "{\\"Structural\\":"; AppendJsonString(out,b.Structural);
        out += ",\\"Certificate\\":"; AppendJsonString(out,b.Certificate);
        out += ",\\"Owner\\":"; AppendJsonString(out,b.Owner);
        out += ",\\"Boundary\\":"; AppendJsonString(out,b.Boundary);
        out += ",\\"Serializing\\":" + std::to_string(b.Serializing) + ",\\"Phase\\":" + std::to_string(b.Phase) + ",\\"Cursor\\":" + std::to_string(b.Cursor) + "}";
        return out;
    }
    int BindingJsonSize(const HotBinding& b) { return static_cast<int>(BindingJson(b).size()); }
    static std::string TagJson(const LookupTag& tag) {
        const auto& c = *tag.Context;
        std::string out = "{\\"Pc\\":" + std::to_string(tag.Pc) + ",\\"VirtualThread\\":" + std::to_string(tag.VirtualThread) + ",\\"Context\\":{\\"ManifestVersion\\":";
        AppendJsonString(out,c.ManifestVersion); out += ",\\"ManifestHash\\":"; AppendJsonString(out,c.ManifestHash);
        out += ",\\"Extensions\\":"; AppendJsonString(out,c.Extensions); out += ",\\"DecoderEpoch\\":"; AppendJsonString(out,c.DecoderEpoch);
        out += ",\\"DecoderVersion\\":"; AppendJsonString(out,c.DecoderVersion); out += ",\\"Privilege\\":"; AppendJsonString(out,c.Privilege);
        out += ",\\"Domain\\":"; AppendJsonString(out,c.Domain); out += ",\\"AddressSpace\\":"; AppendJsonString(out,c.AddressSpace);
        out += ",\\"VectorConfiguration\\":"; AppendJsonString(out,c.VectorConfiguration);
        out += ",\\"ExecutableEpoch\\":" + std::to_string(c.ExecutableEpoch) + ",\\"CodeEpoch\\":" + std::to_string(c.CodeEpoch) + ",\\"ReplayEligible\\":" + (c.ReplayEligible ? "true" : "false");
        out += ",\\"ContextIncarnation\\":"; AppendJsonString(out,c.ContextIncarnation); out += ",\\"SidebandIdentity\\":"; AppendJsonString(out,c.SidebandIdentity);
        out += ",\\"IsBound\\":"; out += c.isBound() ? "true" : "false";
        out += "},\\"Freshness\\":{\\"Incarnation\\":" + std::to_string(tag.Freshness.Incarnation) + ",\\"Generation\\":" + std::to_string(tag.Freshness.Generation) + "}}";
        return out;
    }
"@
# PowerShell here-string uses backtick escape; output must contain C++ escaped quotes and slashes.
$code=$code.Replace('\\"','\"').Replace('\\\\','\\').Replace('\\b','\b').Replace('\\t','\t').Replace('\\n','\n').Replace('\\f','\f').Replace('\\r','\r').Replace('\\u','\u')
$s=$s.Replace($marker,$code)
$s=$s.Replace('return JsonSerializer::SerializeToUtf8Bytes(tag)->Length;', 'return static_cast<int>(TagJson(*tag).size());')
$a=$s.IndexOf("`tstd::vector<unsigned char> SemanticBundleCache::EntryIntegrity("); $b=$s.IndexOf('    int SemanticBundleCache::Victim(', $a)
if ($a -lt 0 -or $b -lt 0) { throw 'Integrity method boundaries missing' }
$body=@"
    std::vector<unsigned char> SemanticBundleCache::EntryIntegrity(std::shared_ptr<LookupTag> tag, std::span<const unsigned char> payload) {
        BCRYPT_ALG_HANDLE algorithm = nullptr; BCRYPT_HASH_HANDLE hash = nullptr;
        if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) throw std::runtime_error("SHA-256 provider unavailable");
        struct Cleanup { BCRYPT_ALG_HANDLE& algorithm; BCRYPT_HASH_HANDLE& hash; ~Cleanup() { if (hash) BCryptDestroyHash(hash); BCryptCloseAlgorithmProvider(algorithm, 0); } } cleanup{algorithm, hash};
        if (BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) < 0) throw std::runtime_error("SHA-256 initialization failed");
        auto json = TagJson(*tag);
        if (BCryptHashData(hash, reinterpret_cast<PUCHAR>(json.data()), static_cast<ULONG>(json.size()), 0) < 0 ||
            BCryptHashData(hash, const_cast<PUCHAR>(payload.data()), static_cast<ULONG>(payload.size()), 0) < 0) throw std::runtime_error("SHA-256 update failed");
        std::vector<unsigned char> digest(32);
        if (BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0) < 0) throw std::runtime_error("SHA-256 finalization failed");
        return digest;
    }

"@
$s=$s.Substring(0,$a)+$body+$s.Substring($b)
Set-Content $p $s -Encoding UTF8 -NoNewline
