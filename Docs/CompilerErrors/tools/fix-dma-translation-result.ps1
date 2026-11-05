$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\IO\Dma\DmaTranslationResult.h'; if(Test-Path -LiteralPath $p){throw 'File exists'}
$source=@"
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
"@
[IO.File]::WriteAllText($p,$source)
$p='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Virtualization\Compatibility\Adapters\IO\YAKSys_Hybrid_CPU.Memory.IOMMU.cpp'; $s=[IO.File]::ReadAllText($p).Replace('#include "../../../../Runtime/IO/Dma/DmaDomainBinding.h"','#include "../../../../Runtime/IO/Dma/DmaDomainBinding.h"'+"`r`n"+'#include "../../../../Runtime/IO/Dma/DmaTranslationResult.h"').Replace('iotlbHit: true','true').Replace('iotlbHit: false','false'); [IO.File]::WriteAllText($p,$s)