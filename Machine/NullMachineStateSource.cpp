#include "NullMachineStateSource.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::Machine
{

	std::wstring MachineStateSourceUnavailableException::getSourceKind() const
	{
		return SourceKind;
	}

	std::wstring MachineStateSourceUnavailableException::getOperation() const
	{
		return Operation;
	}

	MachineStateSourceUnavailableException::MachineStateSourceUnavailableException(const std::wstring &sourceKind, const std::wstring &operation)
		: InvalidOperationException(std::format(L"{} machine-state source cannot provide {}; observation data is unavailable.",
			StringHelper::isEmptyOrWhiteSpace(sourceKind) ? std::wstring(L"Unknown") : sourceKind,
			StringHelper::isEmptyOrWhiteSpace(operation) ? std::wstring(L"UnknownObservationOperation") : operation)),
		  SourceKind(StringHelper::isEmptyOrWhiteSpace(sourceKind) ? L"Unknown" : sourceKind),
		  Operation(StringHelper::isEmptyOrWhiteSpace(operation) ? L"UnknownObservationOperation" : operation)
	{
	}

	std::shared_ptr<NullMachineStateSource> NullMachineStateSource::getInstance()
	{
		return Instance;
	}

	MachineStateSourceProvenance NullMachineStateSource::getSourceProvenance() const
	{
		return MachineStateSourceProvenance::Null;
	}

	int NullMachineStateSource::GetCoreCount()
	{
		return ThrowUnavailable<int>(L"GetCoreCount");
	}

	std::shared_ptr<CpuCoreDiagnosticSnapshot> NullMachineStateSource::GetCoreSnapshot(int coreId)
	{
		return ThrowUnavailable<std::shared_ptr<CpuCoreDiagnosticSnapshot>>(L"GetCoreSnapshot");
	}

	int NullMachineStateSource::GetPodCount()
	{
		return ThrowUnavailable<int>(L"GetPodCount");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public PodController? GetPod(int podIndex)
	std::shared_ptr<PodController> NullMachineStateSource::GetPod(int podIndex)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: return ThrowUnavailable<PodController?>(nameof(GetPod));
		return ThrowUnavailable<std::shared_ptr<PodController>>(L"GetPod");
	}

	std::vector<unsigned char> NullMachineStateSource::ReadMemory(unsigned long long address, int length)
	{
		return ThrowUnavailable<std::vector<unsigned char>>(L"ReadMemory");
	}

	long long NullMachineStateSource::GetTotalMemorySize()
	{
		return ThrowUnavailable<long long>(L"GetTotalMemorySize");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemorySubsystem? GetMemorySubsystem()
	std::shared_ptr<MemorySubsystem> NullMachineStateSource::GetMemorySubsystem()
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: return ThrowUnavailable<MemorySubsystem?>(nameof(GetMemorySubsystem));
		return ThrowUnavailable<std::shared_ptr<MemorySubsystem>>(L"GetMemorySubsystem");
	}

	std::shared_ptr<PerformanceReport> NullMachineStateSource::GetPerformanceReport()
	{
		return ThrowUnavailable<std::shared_ptr<PerformanceReport>>(L"GetPerformanceReport");
	}

	ReplayPhaseMetrics NullMachineStateSource::GetReplayPhaseMetrics(int coreId)
	{
		return ThrowUnavailable<ReplayPhaseMetrics>(L"GetReplayPhaseMetrics");
	}

	SchedulerPhaseMetrics NullMachineStateSource::GetSchedulerPhaseMetrics(int coreId)
	{
		return ThrowUnavailable<SchedulerPhaseMetrics>(L"GetSchedulerPhaseMetrics");
	}

	std::optional<std::shared_ptr<TypedSlotTelemetryProfile>> NullMachineStateSource::GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash)
	{
		return ThrowUnavailable<std::optional<std::shared_ptr<TypedSlotTelemetryProfile>>>(L"GetTypedSlotTelemetryProfile");
	}

	std::wstring NullMachineStateSource::GetReplayToken()
	{
		return ThrowUnavailable<std::wstring>(L"GetReplayToken");
	}
}
