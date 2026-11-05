#pragma once

#include "../../../Machine/IIseMachineStateSource.h"
#include "../../../Machine/MachineStateSourceProvenance.h"
#include "../../../Machine/IseObservationService.h"
#include "../../../CloseToHSL/Processor/Core/YAKSys_Hybrid_CPU.PodController.h"
#include "../../../CloseToHSL/Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../../../NonRTL/Processor/Performance/YAKSys_Hybrid_CPU.PerformanceReport.h"
#include "../../../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../../../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <any>
#include <optional>
#include <memory>

using namespace HybridCPU_ISE::Machine;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::Legacy
{
	enum class LegacyMemoryReadPolicy
	{
		FailClosed = 0,
		ZeroPaddedCompatibility = 1
	};

	/// <summary>
	/// Explicit legacy bridge over the process-global Processor.* runtime surface.
	/// Host code must opt into this contour deliberately and provide the shared sync root
	/// when constructing <see cref="IseObservationService"/>.
	/// </summary>
	class LegacyProcessorMachineStateSource final : public IIseMachineStateSource
	{
	private:
		static inline const std::any SharedSyncRoot = std::any();

		const LegacyMemoryReadPolicy _memoryReadPolicy;

	public:
		static std::any getSharedSyncRoot();

		LegacyProcessorMachineStateSource(LegacyMemoryReadPolicy memoryReadPolicy);

		MachineStateSourceProvenance getSourceProvenance() const override;

		int GetCoreCount() override;

		std::shared_ptr<CpuCoreDiagnosticSnapshot> GetCoreSnapshot(int coreId) override;

		int GetPodCount() override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public PodController? GetPod(int podIndex)
		std::shared_ptr<PodController> GetPod(int podIndex) override;

		std::vector<unsigned char> ReadMemory(unsigned long long address, int length) override;

	private:
		std::vector<unsigned char> HandleOutOfRangeRead(unsigned long long address, int length, long long memoryLength, std::vector<unsigned char> &buffer);

	public:
		long long GetTotalMemorySize() override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemorySubsystem? GetMemorySubsystem()
		std::shared_ptr<MemorySubsystem> GetMemorySubsystem() override;

		std::shared_ptr<PerformanceReport> GetPerformanceReport() override;

		ReplayPhaseMetrics GetReplayPhaseMetrics(int coreId) override;

		SchedulerPhaseMetrics GetSchedulerPhaseMetrics(int coreId) override;

		std::optional<std::shared_ptr<TypedSlotTelemetryProfile>> GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash) override;

		std::wstring GetReplayToken() override;
	};
}
