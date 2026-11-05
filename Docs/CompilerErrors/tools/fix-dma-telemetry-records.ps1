$p = 'C:\Users\Yuriy\Desktop\DPCPPConsoleApplication1\2\CloseToHSL\Core\Execution\DmaStreamCompute\DmaStreamComputeTelemetry.h'
$s = [IO.File]::ReadAllText($p)
$begin = $s.IndexOf('//'+"`t"+'public sealed record DmaStreamComputeTelemetrySnapshot')
$end = $s.IndexOf('class DmaStreamComputeTelemetryCounters final', $begin)
if ($begin -lt 0 -or $end -lt 0) { throw 'Telemetry record boundary not found' }
$record = $s.Substring($begin, $end - $begin)
$typeMap = @{ long='long long'; ulong='unsigned long long'; byte='unsigned char'; ushort='unsigned short'; bool='bool'; 'System.Nullable<string>'='std::wstring' }
$fields = foreach ($match in [regex]::Matches($record, '(?m)^//\s*public\s+(\S+)\s+(\w+)\s*\r?$')) {
    $type = $match.Groups[1].Value
    if ($typeMap.ContainsKey($type)) { $type = $typeMap[$type] }
    "`t`t$type $($match.Groups[2].Value){};"
}
$definitions = @'
	struct DmaStreamComputePressurePolicy final {
		unsigned char RequiredDmaCredits;
		unsigned short RequiredSrfCredits;
		unsigned char RequiredMemoryCredits;
		unsigned short OutstandingTokenCap;
		DmaStreamComputePressurePolicy(unsigned char dma, unsigned short srf, unsigned char memory, unsigned short cap)
			: RequiredDmaCredits(dma ? dma : 1), RequiredSrfCredits(srf ? srf : 1), RequiredMemoryCredits(memory ? memory : 1), OutstandingTokenCap(cap ? cap : 1) {}
		static const std::shared_ptr<DmaStreamComputePressurePolicy> Default;
	};
	inline const std::shared_ptr<DmaStreamComputePressurePolicy> DmaStreamComputePressurePolicy::Default = std::make_shared<DmaStreamComputePressurePolicy>(1, 1, 1, 16);
	struct DmaStreamComputePressureSnapshot final {
		bool Lane6Available;
		unsigned char DmaCreditsAvailable;
		unsigned short SrfCreditsAvailable;
		unsigned char MemorySubsystemCreditsAvailable;
		unsigned short OutstandingTokens;
		static std::shared_ptr<DmaStreamComputePressureSnapshot> Permissive(const std::shared_ptr<DmaStreamComputePressurePolicy>& policy) {
			return std::make_shared<DmaStreamComputePressureSnapshot>(true, policy->RequiredDmaCredits, policy->RequiredSrfCredits, policy->RequiredMemoryCredits, 0);
		}
		static std::shared_ptr<DmaStreamComputePressureSnapshot> FromHardwareOccupancy(const YAKSys_Hybrid_CPU::Core::Memory::HardwareOccupancySnapshot128& snapshot, bool lane6Available = true, unsigned char dma = 1, unsigned short srf = 1, unsigned short outstanding = 0) {
			return std::make_shared<DmaStreamComputePressureSnapshot>(lane6Available, dma, srf, snapshot.getMemoryIssueBudget(), outstanding);
		}
	};
	struct DmaStreamComputeTelemetrySnapshot final {
'@
$definitions += "`r`n" + ($fields -join "`r`n") + "`r`n`t};`r`n`tstruct DmaStreamComputeReplayEvidenceComparison;`r`n`r`n`t"
$s = $s.Insert($end, $definitions)
$s = $s.Replace('#include "DmaStreamComputeDescriptor.h"', "#include `"DmaStreamComputeDescriptor.h`"`r`n#include `"../../Memory/IHardwareOccupancyInput.h`"")
[IO.File]::WriteAllText($p, $s)
