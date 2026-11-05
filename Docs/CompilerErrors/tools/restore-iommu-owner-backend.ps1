$r='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL'
$p=Join-Path $r 'Core\Execution\DmaStreamCompute\DmaStreamComputeOwnerBinding.h'; if(-not (Test-Path $p)) { @"
#pragma once
namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute {
struct DmaStreamComputeOwnerBinding final {
    unsigned short OwnerVirtualThreadId = 0;
    unsigned int OwnerContextId = 0, OwnerCoreId = 0, OwnerPodId = 0;
    unsigned long long OwnerDomainTag = 0;
    unsigned int DeviceId = 0;
};
}
"@ | Set-Content -LiteralPath $p -Encoding UTF8 }
$p=Join-Path $r 'Core\Execution\DmaStreamCompute\DmaStreamComputeDescriptor.h'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "DmaStreamComputeOwnerBinding.h"')) {$s=$s.Replace('#pragma once',"#pragma once`r`n#include `"DmaStreamComputeOwnerBinding.h`"")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Core\Runtime\Domains\Descriptors\IoDomain\IoVirtualizationBlock.cpp'; $s=[IO.File]::ReadAllText($p); if(-not $s.Contains('#include "../../../../../Memory/MMU/IoVirtualizationHostBackend.h"')) {$s=$s.Replace('#include "IoVirtualizationBlock.h"',"#include `"IoVirtualizationBlock.h`"`r`n#include `"../../../../../Memory/MMU/IoVirtualizationHostBackend.h`"`r`n#include `"../../../../Execution/DmaStreamCompute/DmaStreamComputeOwnerBinding.h`"")}; [IO.File]::WriteAllText($p,$s)
$p=Join-Path $r 'Memory\MMU\IoVirtualizationHostBackend.h'; $s=[IO.File]::ReadAllText($p).Replace('static inline const std::shared_ptr<IoVirtualizationHostBackend> Instance = std::make_shared<IoVirtualizationHostBackend>();','static const std::shared_ptr<IoVirtualizationHostBackend> Instance;'); if(-not $s.Contains('IoVirtualizationHostBackend::Instance =')) {$s += "`r`ninline const std::shared_ptr<HybridCPU_ISE::CloseToHSL::Memory::MMU::IoVirtualizationHostBackend> HybridCPU_ISE::CloseToHSL::Memory::MMU::IoVirtualizationHostBackend::Instance = std::shared_ptr<IoVirtualizationHostBackend>(new IoVirtualizationHostBackend());`r`n"}; [IO.File]::WriteAllText($p,$s)
