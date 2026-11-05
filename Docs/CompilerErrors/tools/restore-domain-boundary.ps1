$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains'
$p=Join-Path $dir 'DomainBoundaryDescriptor.h'; $s=[IO.File]::ReadAllText($p); $s=$s.Replace('#pragma once',"#pragma once`r`n#include <memory>")
$i=$s.IndexOf("// C# TO C++ CONVERTER TASK: C# 'records'"); if($i -lt 0){throw 'anchor missing'}
$s=$s.Substring(0,$i)+@'
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
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $dir 'DomainBoundaryDescriptor.cpp'; $s=@'
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
'@
[IO.File]::WriteAllText($p,$s)
$p=Join-Path $dir 'Authority\YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.h"', "#include `"YAKSys_Hybrid_CPU.Core.DomainRuntimeAuthority.h`"`r`n#include `"../DomainBoundaryDescriptor.h`"`r`n#include `"../Descriptors/ExecutionDomain/YAKSys_Hybrid_CPU.Core.ExecutionDomainDescriptor.h`""); [IO.File]::WriteAllText($p,$s)