#pragma once
#include <algorithm>
#include <cwctype>
#include <stdexcept>
#include <string>
#include <utility>
#include <memory>
#include <span>
#include <sstream>
#include <iomanip>
#include "../../../NonRTL/Arch/Generated/GeneratedIsaShadowCatalog.g.h"
#include "Sha256Digest.h"

namespace YAKSys_Hybrid_CPU::Core::Decoder {
inline std::wstring RequireBindingIdentity(std::wstring value) {
    auto whitespace=[](wchar_t c){return std::iswspace(c)!=0;};
    auto first=std::find_if_not(value.begin(),value.end(),whitespace);
    if(first==value.end()) throw std::invalid_argument("Binding identity must not be empty or whitespace");
    auto last=std::find_if_not(value.rbegin(),value.rend(),whitespace).base();
    return std::wstring(first,last);
}
class MaterializerId {
public:
    const std::wstring Value;
    explicit MaterializerId(std::wstring value):Value(RequireBindingIdentity(std::move(value))){}
    std::wstring ToString() const {return Value;}
    bool operator==(const MaterializerId &) const = default;
};
class RuntimeExecutionProviderId {
public:
    const std::wstring Value;
    explicit RuntimeExecutionProviderId(std::wstring value):Value(RequireBindingIdentity(std::move(value))){}
    std::wstring ToString() const {return Value;}
    bool operator==(const RuntimeExecutionProviderId &) const = default;
};
class LatencyModelId {
public:
    const std::wstring Value;
    explicit LatencyModelId(std::wstring value):Value(RequireBindingIdentity(std::move(value))){}
    std::wstring ToString() const {return Value;}
    bool operator==(const LatencyModelId &) const = default;
};
class RuntimeExecutionProviderBinding final {
public:
    const RuntimeExecutionProviderId Id;
    const std::wstring PayloadSchema;
    RuntimeExecutionProviderBinding(RuntimeExecutionProviderId id, std::wstring schema)
        :Id(std::move(id)),PayloadSchema(RequireBindingIdentity(std::move(schema))){}
};
class GeneratedStaticBinding final {
    static void AppendJsonString(std::ostringstream& out, const std::wstring& value) {
        out << '"';
        for (wchar_t c : value) {
            unsigned int u = static_cast<unsigned short>(c);
            if (u >= 0x20 && u <= 0x7e && c != L'"' && c != L'\\' && c != L'<' && c != L'>' && c != L'&' && c != L'\'') out << static_cast<char>(c);
            else if (c == L'\\') out << "\\\\";
            else if (c == L'\b') out << "\\b";
            else if (c == L'\f') out << "\\f";
            else if (c == L'\n') out << "\\n";
            else if (c == L'\r') out << "\\r";
            else if (c == L'\t') out << "\\t";
            else out << "\\u" << std::uppercase << std::hex << std::setw(4) << std::setfill('0') << u << std::dec << std::nouppercase;
        }
        out << '"';
    }
    static std::wstring ComputeDescriptorFingerprint(const YAKSys_Hybrid_CPU::Arch::Generated::GeneratedIsaDescriptor& d) {
        std::ostringstream o;
        o << "{\"Opcode\":" << d.Opcode << ",\"Mnemonic\":"; AppendJsonString(o,d.Mnemonic);
        o << ",\"Aliases\":["; bool first = true; for (const auto& alias : d.Aliases) { if (!first) o << ','; AppendJsonString(o,alias); first=false; }
        o << "],\"OpcodeCategory\":" << static_cast<int>(d.OpcodeCategory) << ",\"InstructionFlags\":" << static_cast<int>(d.InstructionFlags)
          << ",\"ExecutionLatency\":" << static_cast<unsigned>(d.ExecutionLatency) << ",\"MemoryBandwidth\":" << static_cast<unsigned>(d.MemoryBandwidth)
          << ",\"EncodingForm\":"; AppendJsonString(o,d.EncodingForm);
        o << ",\"OperandSchema\":"; AppendJsonString(o,d.OperandSchema);
        o << ",\"StaticClass\":" << static_cast<unsigned>(d.StaticClass) << ",\"SlotConstraints\":"; AppendJsonString(o,d.SlotConstraints);
        o << ",\"Serialization\":" << static_cast<unsigned>(d.Serialization) << ",\"Privilege\":"; AppendJsonString(o,d.Privilege);
        o << ",\"Extension\":"; AppendJsonString(o,d.Extension);
        o << ",\"ProviderId\":"; AppendJsonString(o,d.ProviderId);
        o << ",\"MaterializerId\":"; AppendJsonString(o,d.MaterializerId);
        o << ",\"StaticEffectContract\":"; AppendJsonString(o,d.StaticEffectContract);
        o << ",\"LatencyModelId\":"; AppendJsonString(o,d.LatencyModelId); o << '}';
        const auto json=o.str(); return Sha256Hex(std::span<const unsigned char>(reinterpret_cast<const unsigned char*>(json.data()),json.size()));
    }
public:
    const unsigned int Opcode;
    const Decoder::MaterializerId MaterializerId;
    const Decoder::RuntimeExecutionProviderId RuntimeExecutionProviderId;
    const Decoder::LatencyModelId LatencyModelId;
    const std::wstring CatalogVersion, CatalogSha256, DescriptorFingerprint;
    GeneratedStaticBinding(unsigned int opcode, Decoder::MaterializerId materializer, Decoder::RuntimeExecutionProviderId provider,
                           Decoder::LatencyModelId latency, std::wstring version, std::wstring catalogHash, std::wstring descriptorHash)
        : Opcode(opcode), MaterializerId(std::move(materializer)), RuntimeExecutionProviderId(std::move(provider)),
          LatencyModelId(std::move(latency)), CatalogVersion(std::move(version)), CatalogSha256(std::move(catalogHash)), DescriptorFingerprint(std::move(descriptorHash)) {}
    bool operator==(const GeneratedStaticBinding&) const = default;
    static std::shared_ptr<GeneratedStaticBinding> FromDescriptor(const YAKSys_Hybrid_CPU::Arch::Generated::GeneratedIsaDescriptor& descriptor) {
        using YAKSys_Hybrid_CPU::Arch::Generated::GeneratedIsaCatalog;
        return std::make_shared<GeneratedStaticBinding>(descriptor.Opcode, Decoder::MaterializerId(descriptor.MaterializerId),
            Decoder::RuntimeExecutionProviderId(descriptor.ProviderId), Decoder::LatencyModelId(descriptor.LatencyModelId),
            GeneratedIsaCatalog::CatalogVersion, GeneratedIsaCatalog::CatalogSha256, ComputeDescriptorFingerprint(descriptor));
    }
    static std::shared_ptr<GeneratedStaticBinding> FromDescriptor(const std::shared_ptr<YAKSys_Hybrid_CPU::Arch::Generated::GeneratedIsaDescriptor>& descriptor) {
        if (!descriptor) throw std::invalid_argument("descriptor is null");
        return FromDescriptor(*descriptor);
    }
    static bool TryFromOpcode(unsigned int opcode, std::shared_ptr<GeneratedStaticBinding>& binding) {
        std::shared_ptr<YAKSys_Hybrid_CPU::Arch::Generated::GeneratedIsaDescriptor> descriptor;
        if (!YAKSys_Hybrid_CPU::Arch::Generated::GeneratedIsaCatalog::TryGetDescriptor(opcode, descriptor)) { binding.reset(); return false; }
        binding=FromDescriptor(descriptor); return true;
    }
};
}
