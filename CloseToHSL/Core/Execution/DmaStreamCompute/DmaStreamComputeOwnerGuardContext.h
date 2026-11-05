#pragma once
namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute {
struct DmaStreamComputeOwnerGuardContext final {
    unsigned short OwnerVirtualThreadId = 0;
    unsigned int OwnerContextId = 0, OwnerCoreId = 0, OwnerPodId = 0;
    unsigned long long OwnerDomainTag = 0, ActiveDomainCertificate = 0;
    unsigned int DeviceId = 0;
    DmaStreamComputeOwnerGuardContext() = default;
    DmaStreamComputeOwnerGuardContext(unsigned short vt, unsigned int context, unsigned int core,
        unsigned int pod, unsigned long long domain, unsigned long long active, unsigned int device = 6)
        : OwnerVirtualThreadId(vt), OwnerContextId(context), OwnerCoreId(core), OwnerPodId(pod),
          OwnerDomainTag(domain), ActiveDomainCertificate(active), DeviceId(device) {}
    bool operator==(const DmaStreamComputeOwnerGuardContext&) const = default;
};
}