#include "BundleMetadata.h"
namespace YAKSys_Hybrid_CPU::Core {
std::shared_ptr<BundleMetadata> BundleMetadata::GetDefault() { static const auto value = std::make_shared<BundleMetadata>(); return value; }
std::shared_ptr<SlotMetadata> BundleMetadata::GetSlotMetadata(int slotIndex) const { if(!SlotMetadata || slotIndex<0 || static_cast<size_t>(slotIndex)>=SlotMetadata->size() || !(*SlotMetadata)[slotIndex]) return Core::SlotMetadata::getDefault(); return (*SlotMetadata)[slotIndex]; }
std::shared_ptr<MicroOpAdmissionMetadata> BundleMetadata::GetAdmissionMetadata(int slotIndex) const { return GetSlotMetadata(slotIndex)->AdmissionMetadata; }
std::shared_ptr<BundleMetadata> BundleMetadata::CreateAllStealable() { auto m=std::make_shared<BundleMetadata>(); m->SlotMetadata=std::make_shared<std::vector<std::shared_ptr<Core::SlotMetadata>>>(BundleSlotCount, Core::SlotMetadata::getDefault()); return m; }
std::shared_ptr<BundleMetadata> BundleMetadata::CreateFspFence() { auto m=std::make_shared<BundleMetadata>(); m->FspBoundary=true; m->SlotMetadata=std::make_shared<std::vector<std::shared_ptr<Core::SlotMetadata>>>(BundleSlotCount, Core::SlotMetadata::NotStealable); return m; }
}
