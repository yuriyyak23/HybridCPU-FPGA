#pragma once
namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute {
struct DmaStreamComputeOwnerBinding final {
    unsigned short OwnerVirtualThreadId = 0;
    unsigned int OwnerContextId = 0, OwnerCoreId = 0, OwnerPodId = 0;
    unsigned long long OwnerDomainTag = 0;
    unsigned int DeviceId = 0;
    bool operator==(const DmaStreamComputeOwnerBinding&) const = default;
};
}
