#pragma once

#include "MachineStateSourceProvenance.h"
#include "IseObservationService.h"
#include "../CloseToHSL/Processor/Core/YAKSys_Hybrid_CPU.PodController.h"
#include "../CloseToHSL/Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../NonRTL/Processor/Performance/YAKSys_Hybrid_CPU.PerformanceReport.h"
#include "../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "../CloseToHSL/Core/Diagnostics/TypedSlotTelemetryProfile.h"
#include <string>
#include <vector>
#include <optional>
#include <memory>

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::Machine
{
	class IIseMachineStateSource
	{
	public:
		virtual MachineStateSourceProvenance getSourceProvenance() const = 0;
		virtual int GetCoreCount() = 0;
		virtual std::shared_ptr<CpuCoreDiagnosticSnapshot> GetCoreSnapshot(int coreId) = 0;
		virtual int GetPodCount() = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: PodController? GetPod(int podIndex);
		virtual std::shared_ptr<PodController> GetPod(int podIndex) = 0;
		virtual std::vector<unsigned char> ReadMemory(unsigned long long address, int length) = 0;
		virtual long long GetTotalMemorySize() = 0;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: MemorySubsystem? GetMemorySubsystem();
		virtual std::shared_ptr<MemorySubsystem> GetMemorySubsystem() = 0;
		virtual std::shared_ptr<PerformanceReport> GetPerformanceReport() = 0;
		virtual ReplayPhaseMetrics GetReplayPhaseMetrics(int coreId) = 0;
		virtual SchedulerPhaseMetrics GetSchedulerPhaseMetrics(int coreId) = 0;
		virtual std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Diagnostics::TypedSlotTelemetryProfile>> GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash) = 0;
		virtual std::wstring GetReplayToken() = 0;
	};
}
