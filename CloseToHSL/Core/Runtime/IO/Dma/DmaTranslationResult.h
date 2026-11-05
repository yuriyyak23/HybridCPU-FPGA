#pragma once
#include "DmaDomainBinding.h"
#include "../../../../Memory/MMU/IotlbTag.h"
#include <memory>
namespace YAKSys_Hybrid_CPU::Memory {
struct DmaTranslationResult final {
    bool Succeeded = false, IotlbHit = false;
    unsigned long long IoVirtualAddress = 0, HostPhysicalAddress = 0, AccessSize = 0;
    IOMMUAccessPermissions Permissions = IOMMUAccessPermissions::None;
    std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::MMU::IotlbTag> Tag;
    DmaFault Fault;
    static std::shared_ptr<DmaTranslationResult> Success(bool hit, unsigned long long address, unsigned long long physical, unsigned long long size, IOMMUAccessPermissions permissions, std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::MMU::IotlbTag> tag) {
        return std::make_shared<DmaTranslationResult>(DmaTranslationResult{true, hit, address, physical, size, permissions, std::move(tag), {}});
    }
    static std::shared_ptr<DmaTranslationResult> Fail(unsigned long long address, unsigned long long size, IOMMUAccessPermissions permissions, DmaFault fault) {
        return std::make_shared<DmaTranslationResult>(DmaTranslationResult{false, false, address, 0, size, permissions, nullptr, std::move(fault)});
    }
};
}