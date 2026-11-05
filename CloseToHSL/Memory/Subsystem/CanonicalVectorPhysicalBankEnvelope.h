#pragma once
#include "MemoryBankGeometryGeneration.h"
#include "PhysicalMemoryBankIndex.h"
#include <span>
#include <vector>
#include <string>
namespace YAKSys_Hybrid_CPU::Memory {
class CanonicalVectorPhysicalBankEnvelope final {
    MemoryBankGeometryGeneration generation_;
    std::vector<PhysicalMemoryBankIndex> indexes_;
public:
    CanonicalVectorPhysicalBankEnvelope() = default;
    CanonicalVectorPhysicalBankEnvelope(MemoryBankGeometryGeneration generation, std::span<const PhysicalMemoryBankIndex> indexes);
    MemoryBankGeometryGeneration getGeneration() const { return generation_; }
    int getCount() const { return static_cast<int>(indexes_.size()); }
    unsigned long long getElementCount() const { return indexes_.size(); }
    std::span<const PhysicalMemoryBankIndex> getSourceBankIndexes() const { return indexes_; }
    bool isWellFormed() const { return AreComponentsRepresentable(generation_, indexes_); }
    static bool AreComponentsRepresentable(MemoryBankGeometryGeneration generation, std::span<const PhysicalMemoryBankIndex> indexes);
    static CanonicalVectorPhysicalBankEnvelope Create(MemoryBankGeometryGeneration generation, std::span<const PhysicalMemoryBankIndex> indexes);
    static bool TryCreate(MemoryBankGeometryGeneration generation, std::span<const PhysicalMemoryBankIndex> indexes, CanonicalVectorPhysicalBankEnvelope& envelope);
    PhysicalMemoryBankIndex GetSourceBankIndex(int index) const;
    std::vector<PhysicalMemoryBankIndex> CopySourceBankIndexes() const { return indexes_; }
    std::wstring ToString() const;
};
}
