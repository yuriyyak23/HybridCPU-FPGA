#pragma once
#include "StealabilityPolicy.h"
#include "BranchHint.h"
#include "LocalityHint.h"
#include "ThermalHint.h"
#include "../Scheduling/SlotPlacementMetadata.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core { struct MicroOpAdmissionMetadata; }
namespace YAKSys_Hybrid_CPU::Core
{
    class SlotMetadata final
    {
    public:
        unsigned char SchemaVersion = 4;
        BranchHint BranchHint = static_cast<YAKSys_Hybrid_CPU::Core::BranchHint>(0);
        StealabilityPolicy StealabilityPolicy = static_cast<YAKSys_Hybrid_CPU::Core::StealabilityPolicy>(0);
        unsigned char DonorVtHint = 0xFF;
        LocalityHint LocalityHint = static_cast<YAKSys_Hybrid_CPU::Core::LocalityHint>(0);
        unsigned char PreferredVt = 0xFF;
        ThermalHint ThermalHint = static_cast<YAKSys_Hybrid_CPU::Core::ThermalHint>(0);
        std::shared_ptr<MicroOpAdmissionMetadata> AdmissionMetadata;
        static const std::shared_ptr<SlotMetadata> Default;
        static std::shared_ptr<SlotMetadata> getDefault();
        static const std::shared_ptr<SlotMetadata> NotStealable;
    };
}
