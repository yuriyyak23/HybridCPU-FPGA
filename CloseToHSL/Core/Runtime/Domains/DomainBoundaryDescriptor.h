#pragma once
#include <memory>

// Description: Neutral domain-boundary view for runtime-owned execution, memory, and I/O descriptor admission.
namespace YAKSys_Hybrid_CPU::Core
{

class DomainRuntimeContext;
struct DomainBoundaryDescriptor {
    const bool RequiresExecutionDomain, RequiresMemoryDomain, RequiresIoDomain;
    DomainBoundaryDescriptor(bool execution, bool memory, bool io)
        : RequiresExecutionDomain(execution), RequiresMemoryDomain(memory), RequiresIoDomain(io) {}
    static const std::shared_ptr<DomainBoundaryDescriptor> ExecutionOnly;
    static const std::shared_ptr<DomainBoundaryDescriptor> FullDomainRuntime;
    bool IsSatisfiedBy(std::shared_ptr<DomainRuntimeContext> context) const;
};
inline const std::shared_ptr<DomainBoundaryDescriptor> DomainBoundaryDescriptor::ExecutionOnly = std::make_shared<DomainBoundaryDescriptor>(true, false, false);
inline const std::shared_ptr<DomainBoundaryDescriptor> DomainBoundaryDescriptor::FullDomainRuntime = std::make_shared<DomainBoundaryDescriptor>(true, true, true);
}