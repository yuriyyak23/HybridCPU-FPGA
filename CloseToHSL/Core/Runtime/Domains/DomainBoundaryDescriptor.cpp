#include "DomainBoundaryDescriptor.h"
#include "Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h"
#include "Descriptors/MemoryDomain/YAKSys_Hybrid_CPU.Core.MemoryDomainDescriptor.h"
#include "Descriptors/IoDomain/YAKSys_Hybrid_CPU.Core.IoDomainDescriptor.h"
namespace YAKSys_Hybrid_CPU::Core {
bool DomainBoundaryDescriptor::IsSatisfiedBy(std::shared_ptr<DomainRuntimeContext> context) const {
    if (!context) return false;
    if (RequiresExecutionDomain && (!context->getExecution() || !context->getExecution()->isAuthoritativeExecutionStateOwner())) return false;
    if (RequiresMemoryDomain && (!context->getMemory() || !context->getMemory()->isAuthoritativeMemoryStateOwner())) return false;
    if (RequiresIoDomain && (!context->getIo() || !context->getIo()->isAuthoritativeIoStateOwner())) return false;
    return true;
}
}