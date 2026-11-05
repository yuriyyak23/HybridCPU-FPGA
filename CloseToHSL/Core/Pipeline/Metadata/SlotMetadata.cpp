#include "SlotMetadata.h"
namespace YAKSys_Hybrid_CPU::Core {
std::shared_ptr<SlotMetadata> SlotMetadata::getDefault() { static const auto value = std::make_shared<SlotMetadata>(); return value; }
const std::shared_ptr<SlotMetadata> SlotMetadata::Default = SlotMetadata::getDefault();
const std::shared_ptr<SlotMetadata> SlotMetadata::NotStealable = [] { auto value=std::make_shared<SlotMetadata>(); value->StealabilityPolicy=static_cast<Core::StealabilityPolicy>(1); return value; }();
}
