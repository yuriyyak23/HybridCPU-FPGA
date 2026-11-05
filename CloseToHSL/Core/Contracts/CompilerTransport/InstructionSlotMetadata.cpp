#include "InstructionSlotMetadata.h"
namespace HybridCPU_ISE::Arch {
std::shared_ptr<InstructionSlotMetadata> InstructionSlotMetadata::getDefault() { static const auto value = std::make_shared<InstructionSlotMetadata>(); return value; }
const std::shared_ptr<InstructionSlotMetadata> InstructionSlotMetadata::Default = InstructionSlotMetadata::getDefault();
}
