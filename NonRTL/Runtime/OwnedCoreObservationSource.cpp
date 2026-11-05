#include "OwnedCoreObservationSource.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::NonRTL::Runtime
{

	OwnedCoreObservationSource::OwnedCoreObservationSource(std::shared_ptr<Processor::CPU_Core> core)
		: core(core)
	{
		ArgumentNullException::ThrowIfNull(core);
		if (core->getCoreID() != 0)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Only runner core zero is supported.", nameof(core));
			throw std::invalid_argument("Only runner core zero is supported.");
		}
	}

	::HybridCPU_ISE::Machine::MachineStateSourceProvenance OwnedCoreObservationSource::getSourceProvenance() const
	{
		return MachineStateSourceProvenance::LiveCore;
	}

	int OwnedCoreObservationSource::GetCoreCount()
	{
		EnsureOwner();
		return 1;
	}

	std::optional<std::shared_ptr<OwnedCoreRetireObservation>> OwnedCoreObservationSource::ReadOriginalRetire(Guid segmentId, unsigned long long entryAddress)
	{
		EnsureOwner();
		unsigned long long pc;
		unsigned long long sequence;
		if (!core->TryReadLastRetiredBundle(pc, sequence))
		{
			return std::nullopt;
		}
		return std::make_shared<OwnedCoreRetireObservation>(OwnedCoreRetireObservation{segmentId, entryAddress, pc, sequence});
	}

	std::shared_ptr<CpuCoreDiagnosticSnapshot> OwnedCoreObservationSource::GetCoreSnapshot(int coreId)
	{
		EnsureOwner();
		if (coreId != 0 || core->getCoreID() != 0)
		{
			throw ArgumentOutOfRangeException(L"coreId", coreId, L"This binding supports only the runner's core zero.");
		}
		return CpuCoreDiagnosticSnapshot::Capture(core);
	}

	void OwnedCoreObservationSource::dispose()
	{
		EnsureOwnerThread();
		closed = true;
	}

	void OwnedCoreObservationSource::EnsureOwnerThread()
	{
		if (std::this_thread::get_id() != owner)
		{
			throw InvalidOperationException(L"Core observation must execute on the bound CPU-owner thread.");
		}
	}

	void OwnedCoreObservationSource::EnsureOwner()
	{
		EnsureOwnerThread();
		if (closed)
		{
			throw InvalidOperationException(L"Core observation source is closed.");
		}
	}

	int OwnedCoreObservationSource::GetPodCount()
	{
		return Unavailable<int>(L"GetPodCount");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public PodController? GetPod(int podIndex)
	std::shared_ptr<PodController> OwnedCoreObservationSource::GetPod(int podIndex)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: return Unavailable<PodController?>(nameof(GetPod));
		return Unavailable<std::shared_ptr<PodController>>(L"GetPod");
	}

	std::vector<unsigned char> OwnedCoreObservationSource::ReadMemory(unsigned long long address, int length)
	{
		return Unavailable<std::vector<unsigned char>>(L"ReadMemory");
	}

	long long OwnedCoreObservationSource::GetTotalMemorySize()
	{
		return Unavailable<long long>(L"GetTotalMemorySize");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemorySubsystem? GetMemorySubsystem()
	std::shared_ptr<MemorySubsystem> OwnedCoreObservationSource::GetMemorySubsystem()
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: return Unavailable<MemorySubsystem?>(nameof(GetMemorySubsystem));
		return Unavailable<std::shared_ptr<MemorySubsystem>>(L"GetMemorySubsystem");
	}

	std::shared_ptr<PerformanceReport> OwnedCoreObservationSource::GetPerformanceReport()
	{
		return Unavailable<std::shared_ptr<PerformanceReport>>(L"GetPerformanceReport");
	}

	ReplayPhaseMetrics OwnedCoreObservationSource::GetReplayPhaseMetrics(int coreId)
	{
		return Unavailable<ReplayPhaseMetrics>(L"GetReplayPhaseMetrics");
	}

	SchedulerPhaseMetrics OwnedCoreObservationSource::GetSchedulerPhaseMetrics(int coreId)
	{
		return Unavailable<SchedulerPhaseMetrics>(L"GetSchedulerPhaseMetrics");
	}

	std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Diagnostics::TypedSlotTelemetryProfile>> OwnedCoreObservationSource::GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash)
	{
		return Unavailable<std::optional<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Diagnostics::TypedSlotTelemetryProfile>>>(L"GetTypedSlotTelemetryProfile");
	}

	std::wstring OwnedCoreObservationSource::GetReplayToken()
	{
		return Unavailable<std::wstring>(L"GetReplayToken");
	}
}
