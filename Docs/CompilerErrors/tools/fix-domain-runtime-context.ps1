$dir='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Runtime\Domains\Services'
$p=Join-Path $dir 'YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h'; $s=[IO.File]::ReadAllText($p)
$s=[regex]::Replace($s,'(?m)^#include "(?:\.\./Descriptors/|\.\./\.\./Capabilities/|\.\./SecureCompute/).*"\r?\n','')
$s=$s.Replace('class DomainRuntimeContext final',"class ExecutionDomainDescriptor;`r`n`tclass MemoryDomainDescriptor;`r`n`tclass IoDomainDescriptor;`r`n`tclass CapabilityDescriptorSet;`r`n`tclass SecureComputeDomainDescriptor;`r`n`tclass DomainRuntimeContext final"); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $dir 'YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.cpp'; $s=[IO.File]::ReadAllText($p)
$s=$s.Replace('#include "YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"',"#include `"YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h`"`r`n#include `"../../Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h`"")
$pat='(DomainRuntimeContext::DomainRuntimeContext\(std::shared_ptr<ExecutionDomainDescriptor> execution, std::shared_ptr<MemoryDomainDescriptor> memory, std::shared_ptr<IoDomainDescriptor> io, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<SecureComputeDomainDescriptor> secureCompute, unsigned long long domainTag, unsigned long long addressSpaceTag\))\s*\{\s*Execution = execution;\s*Memory = memory;\s*Io = io;\s*Capabilities = capabilities;\s*SecureCompute = secureCompute;\s*DomainTag = domainTag;\s*AddressSpaceTag = addressSpaceTag;\s*\}'
if(-not [regex]::IsMatch($s,$pat)){throw 'constructor not found'}
$s=[regex]::Replace($s,$pat,'$1 : Execution(execution), Memory(memory), Io(io), Capabilities(capabilities), SecureCompute(secureCompute), DomainTag(domainTag), AddressSpaceTag(addressSpaceTag) {}')
$s=$s.Replace('return new(getExecution(), getMemory(), getIo(),', 'return std::make_shared<DomainRuntimeContext>(getExecution(), getMemory(), getIo(),'); [IO.File]::WriteAllText($p,$s)
