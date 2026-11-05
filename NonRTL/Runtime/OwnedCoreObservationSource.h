#pragma once

#include "../../Machine/IIseMachineStateSource.h"
#include "../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../Machine/MachineStateSourceProvenance.h"
#include "../../Machine/IseObservationService.h"
#include "../../Machine/NullMachineStateSource.h"
#include "../../CloseToHSL/Processor/Core/YAKSys_Hybrid_CPU.PodController.h"
#include "../../CloseToHSL/Memory/Subsystem/YAKSys_Hybrid_CPU.Memory.MemorySubsystem.h"
#include "../Processor/Performance/YAKSys_Hybrid_CPU.PerformanceReport.h"
#include "../../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.h"
#include "../../CloseToHSL/Core/Pipeline/Certificates/ReplayPhaseSubstrate.Implementations.h"
#include "../../CloseToHSL/Core/Diagnostics/TypedSlotTelemetryProfile.h"
#include "../../Guid.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>
#include <thread>
#include "../../tangible_disposable_interface.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::NonRTL::Runtime
{

	struct OwnedCoreRetireObservation
	{
		Guid SegmentId;
		unsigned long long EntryAddress{};
		unsigned long long BundlePc{};
		unsigned long long Sequence{};
	};

	/// <summary>
	/// Observation of one existing core on its execution-owner thread, between cycles.
	/// The host must not execute this core concurrently or transfer ownership while bound.
	/// This source neither executes nor stops the CPU and never reads global Processor state.
	/// </summary>
	class OwnedCoreObservationSource final : public ::HybridCPU_ISE::Machine::IIseMachineStateSource, public Disposable
	{
	private:
		const std::shared_ptr<Processor::CPU_Core> core;
		const std::thread::id owner = std::this_thread::get_id();
		bool closed = false;

	public:
		OwnedCoreObservationSource(std::shared_ptr<Processor::CPU_Core> core);

		::HybridCPU_ISE::Machine::MachineStateSourceProvenance getSourceProvenance() const override;

		int GetCoreCount() override;

		std::optional<std::shared_ptr<OwnedCoreRetireObservation>> ReadOriginalRetire(Guid segmentId, unsigned long long entryAddress);

		std::shared_ptr<CpuCoreDiagnosticSnapshot> GetCoreSnapshot(int coreId) override;

		void dispose() override;

	private:
		void EnsureOwnerThread();

		void EnsureOwner();

		template<typename T>
		static T Unavailable(const std::wstring &operation)
		{
			throw MachineStateSourceUnavailableException(L"OwnedCoreObservationSource", operation);
		}

	public:
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
		std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Diagnostics::TypedSlotTelemetryProfile>> GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash) override;
		std::wstring GetReplayToken() override;
	};

}
