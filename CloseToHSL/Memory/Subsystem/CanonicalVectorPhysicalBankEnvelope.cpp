#include "CanonicalVectorPhysicalBankEnvelope.h"
#include <stdexcept>
namespace YAKSys_Hybrid_CPU::Memory {
CanonicalVectorPhysicalBankEnvelope::CanonicalVectorPhysicalBankEnvelope(MemoryBankGeometryGeneration generation, std::span<const PhysicalMemoryBankIndex> indexes) : generation_(generation) {
    if (!generation.isIssued()) throw std::out_of_range("generation");
    if (indexes.empty()) throw std::invalid_argument("sourceBankIndexes");
    for (auto index : indexes) if (!PhysicalMemoryBankIndex::IsRepresentable(index.getValue())) throw std::out_of_range("sourceBankIndexes");
    indexes_.assign(indexes.begin(), indexes.end());
}
bool CanonicalVectorPhysicalBankEnvelope::AreComponentsRepresentable(MemoryBankGeometryGeneration generation, std::span<const PhysicalMemoryBankIndex> indexes) {
    if (!generation.isIssued() || indexes.empty()) return false;
    for (auto index : indexes) if (!PhysicalMemoryBankIndex::IsRepresentable(index.getValue())) return false;
    return true;
}
CanonicalVectorPhysicalBankEnvelope CanonicalVectorPhysicalBankEnvelope::Create(MemoryBankGeometryGeneration generation, std::span<const PhysicalMemoryBankIndex> indexes) { return {generation, indexes}; }
bool CanonicalVectorPhysicalBankEnvelope::TryCreate(MemoryBankGeometryGeneration generation, std::span<const PhysicalMemoryBankIndex> indexes, CanonicalVectorPhysicalBankEnvelope& envelope) {
    if (!AreComponentsRepresentable(generation, indexes)) { envelope = {}; return false; }
    envelope = CanonicalVectorPhysicalBankEnvelope(generation, indexes); return true;
}
PhysicalMemoryBankIndex CanonicalVectorPhysicalBankEnvelope::GetSourceBankIndex(int index) const {
    if (indexes_.empty()) throw std::logic_error("Malformed envelope has no source-bank indexes.");
    return indexes_.at(static_cast<std::size_t>(index));
}
std::wstring CanonicalVectorPhysicalBankEnvelope::ToString() const {
    return isWellFormed() ? L"canonical-vector-physical-bank-envelope(generation=" + std::to_wstring(generation_.getValue()) + L", elements=" + std::to_wstring(getCount()) + L")" : L"malformed-canonical-vector-physical-bank-envelope";
}
}
