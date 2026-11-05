$base='C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL'
$id=Join-Path $base 'Memory\Timing\MemoryRequestId.h'
if(Test-Path -LiteralPath $id){throw 'MemoryRequestId.h already exists'}
$text=@"
#pragma once

namespace HybridCPU_ISE::CloseToHSL::Memory::Timing {
    // Identity is local to the memory controller; zero is invalid.
    struct MemoryRequestId final {
        unsigned long long Value = 0;
        explicit constexpr MemoryRequestId(unsigned long long value = 0) noexcept : Value(value) {}
        constexpr bool getIsValid() const noexcept { return Value != 0; }
        friend constexpr bool operator==(MemoryRequestId, MemoryRequestId) = default;
    };
}
"@
[IO.File]::WriteAllText($id,$text)
$p=Join-Path $base 'Memory\Timing\MemoryCycleController.h'; $s=[IO.File]::ReadAllText($p).Replace('#include "MemoryAdmissionTypes.h"','#include "MemoryAdmissionTypes.h"'+"`r`n"+'#include "MemoryRequestId.h"'); [IO.File]::WriteAllText($p,$s)
$p=Join-Path $base 'Core\Architecture\Common\YAKSys_Hybrid_CPU.Processor.h'; $s=[IO.File]::ReadAllText($p).Replace('#include "../../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"','#include "../../../Memory/Registers/YAKSys_Hybrid_CPU.Memory.StreamRegisterFile.h"'+"`r`n"+'#include "../../../Memory/Timing/MemoryRequestId.h"').Replace('unsigned char[]? PendingReadBuffer;', 'std::shared_ptr<std::vector<unsigned char>> PendingReadBuffer;').Replace('namespace YAKSys_Hybrid_CPU { class CPU_Core; class ShadowFrontendCpuSnapshot; }','namespace YAKSys_Hybrid_CPU { class CPU_Core; class ShadowFrontendCpuSnapshot; class TimeSpan; }'); [IO.File]::WriteAllText($p,$s)