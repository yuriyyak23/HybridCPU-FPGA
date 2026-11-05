#include "../../Frontend/Replay/HotCompatibility.h"
#include <iostream>
namespace HybridCpu::ShadowFrontend {
    static void AppendJsonString(std::string& out, const std::wstring& text) {
        constexpr char hex[] = "0123456789ABCDEF"; out.push_back('"');
        for (wchar_t ch : text) {
            unsigned int c = static_cast<unsigned int>(ch);
            if (c == 92) { out.push_back(92); out.push_back(92); continue; }
            char esc = 0;
            switch (c) { case 8: esc = 'b'; break; case 9: esc = 't'; break; case 10: esc = 'n'; break; case 12: esc = 'f'; break; case 13: esc = 'r'; break; }
            if (esc) { out.push_back(92); out.push_back(esc); continue; }
            if (c < 32 || c > 126 || c == 34 || c == 38 || c == 39 || c == 43 || c == 60 || c == 62 || c == 96) {
                out.push_back(92); out.push_back('u'); for (int shift = 12; shift >= 0; shift -= 4) out.push_back(hex[(c >> shift) & 15]);
            } else out.push_back(static_cast<char>(c));
        }
        out.push_back('"');
    }
    static std::string BindingJson(const HotBinding& b) {
        std::string out = "{\"Structural\":"; AppendJsonString(out,b.Structural);
        out += ",\"Certificate\":"; AppendJsonString(out,b.Certificate);
        out += ",\"Owner\":"; AppendJsonString(out,b.Owner);
        out += ",\"Boundary\":"; AppendJsonString(out,b.Boundary);
        out += ",\"Serializing\":" + std::to_string(b.Serializing) + ",\"Phase\":" + std::to_string(b.Phase) + ",\"Cursor\":" + std::to_string(b.Cursor) + "}";
        return out;
    }
    int BindingJsonSize(const HotBinding& b) { return static_cast<int>(BindingJson(b).size()); }
    static std::string TagJson(const LookupTag& tag) {
        const auto& c = *tag.Context;
        std::string out = "{\"Pc\":" + std::to_string(tag.Pc) + ",\"VirtualThread\":" + std::to_string(tag.VirtualThread) + ",\"Context\":{\"ManifestVersion\":";
        AppendJsonString(out,c.ManifestVersion); out += ",\"ManifestHash\":"; AppendJsonString(out,c.ManifestHash);
        out += ",\"Extensions\":"; AppendJsonString(out,c.Extensions); out += ",\"DecoderEpoch\":"; AppendJsonString(out,c.DecoderEpoch);
        out += ",\"DecoderVersion\":"; AppendJsonString(out,c.DecoderVersion); out += ",\"Privilege\":"; AppendJsonString(out,c.Privilege);
        out += ",\"Domain\":"; AppendJsonString(out,c.Domain); out += ",\"AddressSpace\":"; AppendJsonString(out,c.AddressSpace);
        out += ",\"VectorConfiguration\":"; AppendJsonString(out,c.VectorConfiguration);
        out += ",\"ExecutableEpoch\":" + std::to_string(c.ExecutableEpoch) + ",\"CodeEpoch\":" + std::to_string(c.CodeEpoch) + ",\"ReplayEligible\":" + (c.ReplayEligible ? "true" : "false");
        out += ",\"ContextIncarnation\":"; AppendJsonString(out,c.ContextIncarnation); out += ",\"SidebandIdentity\":"; AppendJsonString(out,c.SidebandIdentity);
        out += ",\"IsBound\":"; out += c.isBound() ? "true" : "false";
        out += "},\"Freshness\":{\"Incarnation\":" + std::to_string(tag.Freshness.Incarnation) + ",\"Generation\":" + std::to_string(tag.Freshness.Generation) + "}}";
        return out;
    }
}
int main() {
 using namespace HybridCpu::ShadowFrontend;
 DecodeContext c; c.ManifestVersion=L"A+&\"Ж"; c.ManifestHash=L"x"; c.Extensions=L"x"; c.DecoderEpoch=L"x"; c.DecoderVersion=L"x"; c.Privilege=L"x"; c.Domain=L"x"; c.AddressSpace=L"x"; c.VectorConfiguration=L"x"; c.ContextIncarnation=L"x"; c.SidebandIdentity=L"x"; c.ReplayEligible=true;
 LookupTag tag{42,2,std::make_shared<DecodeContext>(c),FreshnessToken{3,4}};
 HotBinding binding{L"s+",L"c",L"o",L"b",1,2,3};
 std::cout << TagJson(tag) << "\n" << BindingJson(binding) << "\n";
}
