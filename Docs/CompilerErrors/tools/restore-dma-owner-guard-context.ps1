$root='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core';$dest=Join-Path $root 'Execution\DmaStreamCompute\DmaStreamComputeOwnerGuardContext.h';$descriptor=Join-Path $root 'Execution\DmaStreamCompute\DmaStreamComputeDescriptor.h';$verifier=Join-Path $root 'Pipeline\Safety\YAKSys_Hybrid_CPU.Core.SafetyVerifier.h'
if(Test-Path $dest){throw 'File exists'}
$body=@"
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
"@
$ds=[IO.File]::ReadAllText($descriptor);$ds=$ds.Replace('#pragma once',"#pragma once`r`n#include `"DmaStreamComputeOwnerGuardContext.h`"");$vs=[IO.File]::ReadAllText($verifier);$vs=$vs.Replace('class DmaStreamComputeOwnerGuardContext;','struct DmaStreamComputeOwnerGuardContext;')
[IO.File]::WriteAllText($dest,$body,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($descriptor,$ds,[Text.UTF8Encoding]::new($false));[IO.File]::WriteAllText($verifier,$vs,[Text.UTF8Encoding]::new($false))
