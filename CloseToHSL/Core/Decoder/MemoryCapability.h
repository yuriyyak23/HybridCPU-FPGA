#pragma once
#include <algorithm>
#include <limits>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core::Decoder {
enum class MemoryCapabilityKind : unsigned char { NoMemory=0, Load=1, Store=2, Atomic=3 };
enum class MemoryAccessDirection : unsigned char { None=0, Read=1, Write=2, ReadWrite=3 };
struct MemoryBankId {
    static constexpr int BankCount=16;
    int Value;
    explicit MemoryBankId(int value) : Value(value) { if(value < 0 || value >= BankCount) throw std::out_of_range("Memory bank must be in [0, 15]"); }
    bool operator==(const MemoryBankId &) const = default;
};
struct FrozenMemoryRange {
    unsigned long long Address, Length;
    FrozenMemoryRange(unsigned long long address, unsigned long long length) : Address(address), Length(length) {
        if (length == 0 || length - 1 > (std::numeric_limits<unsigned long long>::max)() - address) throw std::out_of_range("Invalid frozen memory range");
    }
    unsigned long long getLastAddress() const { return Address + Length - 1; }
    bool operator==(const FrozenMemoryRange &) const = default;
};
class MemoryCapability final {
    MemoryCapability(MemoryCapabilityKind kind, MemoryAccessDirection direction, std::optional<MemoryBankId> bank, std::vector<FrozenMemoryRange> footprint)
        : Kind(kind), Direction(direction), Bank(bank), Footprint(std::move(footprint)) {}
public:
    const MemoryCapabilityKind Kind;
    const MemoryAccessDirection Direction;
    const std::optional<MemoryBankId> Bank;
    const std::vector<FrozenMemoryRange> Footprint;
    static const std::shared_ptr<MemoryCapability> None;
    static std::shared_ptr<MemoryCapability> Create(MemoryCapabilityKind kind, std::optional<std::vector<FrozenMemoryRange>> footprint = std::nullopt,
        std::optional<MemoryBankId> bank = std::nullopt, std::optional<MemoryAccessDirection> direction = std::nullopt) {
        MemoryAccessDirection expected;
        switch(kind) { case MemoryCapabilityKind::NoMemory: expected=MemoryAccessDirection::None; break; case MemoryCapabilityKind::Load: expected=MemoryAccessDirection::Read; break; case MemoryCapabilityKind::Store: expected=MemoryAccessDirection::Write; break; case MemoryCapabilityKind::Atomic: expected=MemoryAccessDirection::ReadWrite; break; default: throw std::out_of_range("Unknown memory capability kind"); }
        if (kind == MemoryCapabilityKind::NoMemory && ((footprint && !footprint->empty()) || bank)) throw std::invalid_argument("NoMemory cannot carry a footprint or bank");
        if (kind != MemoryCapabilityKind::NoMemory && (!footprint || footprint->empty() || !bank)) throw std::invalid_argument("Memory capability requires a footprint and bank");
        if (direction && *direction != expected) throw std::invalid_argument("Memory capability direction mismatch");
        auto ranges=footprint.value_or(std::vector<FrozenMemoryRange>{});
        std::sort(ranges.begin(),ranges.end(),[](const auto &a,const auto &b){return a.Address < b.Address || (a.Address == b.Address && a.Length < b.Length);});
        for(std::size_t i=1;i<ranges.size();++i) if(ranges[i].Address <= ranges[i-1].getLastAddress()) throw std::invalid_argument("Memory ranges overlap");
        return std::shared_ptr<MemoryCapability>(new MemoryCapability(kind,expected,bank,std::move(ranges)));
    }
};
inline const std::shared_ptr<MemoryCapability> MemoryCapability::None=MemoryCapability::Create(MemoryCapabilityKind::NoMemory);
}

