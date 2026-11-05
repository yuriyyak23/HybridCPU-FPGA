$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\ExternalAccelerators\Capabilities\AcceleratorResourceModel.cpp'
$s = [IO.File]::ReadAllText($p)
$s = $s.Replace('#include "AcceleratorResourceModel.h"', "#include `"AcceleratorResourceModel.h`"`r`n#include <limits>`r`n#include <stdexcept>")
$signature = 'AcceleratorResourceModel::AcceleratorResourceModel(unsigned int baseLatencyCycles, unsigned int cyclesPerElement, unsigned int maxQueueOccupancy, unsigned long long scratchBytes, unsigned long long memoryBandwidthBytesPerCycle)'
$s = $s.Replace($signature, $signature + "`r`n`t`t: BaseLatencyCycles(baseLatencyCycles), CyclesPerElement(cyclesPerElement), MaxQueueOccupancy(maxQueueOccupancy), ScratchBytes(scratchBytes), MemoryBandwidthBytesPerCycle(memoryBandwidthBytesPerCycle)")
foreach ($line in @('BaseLatencyCycles = baseLatencyCycles;', 'CyclesPerElement = cyclesPerElement;', 'MaxQueueOccupancy = maxQueueOccupancy;', 'ScratchBytes = scratchBytes;', 'MemoryBandwidthBytesPerCycle = memoryBandwidthBytesPerCycle;')) { $s = $s.Replace($line, '') }
$s = $s.Replace('return getBaseLatencyCycles() + (elementCount * getCyclesPerElement());', "if (getCyclesPerElement() != 0 && elementCount > (std::numeric_limits<unsigned long long>::max() - getBaseLatencyCycles()) / getCyclesPerElement())`r`n`t`t`t`tthrow std::overflow_error(`"Accelerator latency exceeds unsigned 64-bit range`");`r`n`t`t`treturn getBaseLatencyCycles() + (elementCount * getCyclesPerElement());")
[IO.File]::WriteAllText($p, $s)
