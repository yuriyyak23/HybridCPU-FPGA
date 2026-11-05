#pragma once

#include "IIseMachineStateSource.h"
#include "MachineStateSourceProvenance.h"
#include "IseObservationService.h"
#include "../CloseToHSL/Processor/Core/YAKSys_Hybrid_CPU.PodController.h"
#include "../CloseToHSL/Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../NonRTL/Processor/Performance/YAKSys_Hybrid_CPU.PerformanceReport.h"
#include "../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include <string>
#include <vector>
#include <optional>
#include <format>
#include <memory>
#include "../tangible_string_helper.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::Machine
{
	class MachineStateSourceUnavailableException final : public InvalidOperationException
	{
	private:
		const std::wstring SourceKind;
		const std::wstring Operation;

	public:
		std::wstring getSourceKind() const;
		std::wstring getOperation() const;

		MachineStateSourceUnavailableException(const std::wstring &sourceKind, const std::wstring &operation);

	};

	class NullMachineStateSource final : public IIseMachineStateSource
	{
	private:
		static inline const std::shared_ptr<NullMachineStateSource> Instance = std::make_shared<NullMachineStateSource>();

		static inline const std::wstring SourceKind = L"Null";

	public:
		static std::shared_ptr<NullMachineStateSource> getInstance();

		MachineStateSourceProvenance getSourceProvenance() const override;

		int GetCoreCount() override;

		std::shared_ptr<CpuCoreDiagnosticSnapshot> GetCoreSnapshot(int coreId) override;

		int GetPodCount() override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public PodController? GetPod(int podIndex)
		std::shared_ptr<PodController> GetPod(int podIndex) override;

		std::vector<unsigned char> ReadMemory(unsigned long long address, int length) override;

		long long GetTotalMemorySize() override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemorySubsystem? GetMemorySubsystem()
		std::shared_ptr<MemorySubsystem> GetMemorySubsystem() override;

		std::shared_ptr<PerformanceReport> GetPerformanceReport() override;

		ReplayPhaseMetrics GetReplayPhaseMetrics(int coreId) override;

		SchedulerPhaseMetrics GetSchedulerPhaseMetrics(int coreId) override;

		std::optional<std::shared_ptr<TypedSlotTelemetryProfile>> GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash) override;

		std::wstring GetReplayToken() override;

	private:
		template<typename T>
		static T ThrowUnavailable(const std::wstring &operation)
		{
			throw MachineStateSourceUnavailableException(SourceKind, operation);
		}
	};
}
