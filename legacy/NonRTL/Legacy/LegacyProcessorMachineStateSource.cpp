#include "LegacyProcessorMachineStateSource.h"
#include "../../../CloseToHSL/Core/Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "LegacyMachineStateReadException.h"
#include "../../../CloseToHSL/Core/Diagnostics/TelemetryExporter.h"

using namespace HybridCPU_ISE::Machine;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::Legacy
{

	std::any LegacyProcessorMachineStateSource::getSharedSyncRoot()
	{
		return SharedSyncRoot;
	}

	LegacyProcessorMachineStateSource::LegacyProcessorMachineStateSource(LegacyMemoryReadPolicy memoryReadPolicy) : _memoryReadPolicy(memoryReadPolicy)
	{
	}

	MachineStateSourceProvenance LegacyProcessorMachineStateSource::getSourceProvenance() const
	{
		return MachineStateSourceProvenance::LegacyGlobal;
	}

	int LegacyProcessorMachineStateSource::GetCoreCount()
	{
		std::vector<std::shared_ptr<Processor::CPU_Core>> cores = Processor::getCPUCores();
		if (cores.empty() || cores.empty())
		{
			return 0;
		}

		int initializedCoreCount = 0;
		for (int coreIndex = 0; coreIndex < cores.size(); coreIndex++)
		{
			if (cores[coreIndex]->getArchContexts().empty() || cores[coreIndex]->getArchContexts().size() != Processor::CPU_Core::SmtWays)
			{
				break;
			}

			initializedCoreCount++;
		}

		return initializedCoreCount;
	}

	std::shared_ptr<CpuCoreDiagnosticSnapshot> LegacyProcessorMachineStateSource::GetCoreSnapshot(int coreId)
	{
		return Processor::GetCoreSnapshot(coreId);
	}

	int LegacyProcessorMachineStateSource::GetPodCount()
	{
		return Processor::Pods.size();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public PodController? GetPod(int podIndex)
	std::shared_ptr<PodController> LegacyProcessorMachineStateSource::GetPod(int podIndex)
	{
		return Processor::Pods[podIndex];
	}

	std::vector<unsigned char> LegacyProcessorMachineStateSource::ReadMemory(unsigned long long address, int length)
	{
		ArgumentOutOfRangeException::ThrowIfNegative(length);

		std::vector<unsigned char> buffer(length);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: Processor.MainMemoryArea? mainMemory = Processor.MainMemory;
		std::shared_ptr<Processor::MainMemoryArea> mainMemory = Processor::MainMemory;

		if (mainMemory == nullptr)
		{
			throw LegacyMachineStateReadException(address, length, -1, LegacyMachineStateReadFailureKind::MainMemoryUnavailable, std::runtime_error());
		}

		if (address > std::numeric_limits<long long>::max())
		{
			throw LegacyMachineStateReadException(address, length, mainMemory->getLength(), LegacyMachineStateReadFailureKind::AddressOutOfRange);
		}

		long long baseAddress = static_cast<long long>(address);
		if (baseAddress >= mainMemory->getLength())
		{
			return HandleOutOfRangeRead(address, length, mainMemory->getLength(), buffer);
		}

		long long availableBytes = mainMemory->getLength() - baseAddress;
		int bytesToRead = static_cast<int>(std::min(length, availableBytes));
		if (bytesToRead < length && _memoryReadPolicy != LegacyMemoryReadPolicy::ZeroPaddedCompatibility)
		{
			throw LegacyMachineStateReadException(address, length, mainMemory->getLength(), LegacyMachineStateReadFailureKind::PartialReadRequiresCompatibilityPadding);
		}

		long long originalPosition = mainMemory->getPosition();

		try
		{
			mainMemory->setPosition(baseAddress);
			_ = mainMemory->Read(buffer, 0, bytesToRead);
			return buffer;
		}
		catch (const std::runtime_error &ex)
		{
			throw LegacyMachineStateReadException(address, length, mainMemory->getLength(), LegacyMachineStateReadFailureKind::ReadFault, ex);
		}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the exception 'finally' clause:
		finally
		{
			mainMemory->setPosition(originalPosition);
		}
	}

	std::vector<unsigned char> LegacyProcessorMachineStateSource::HandleOutOfRangeRead(unsigned long long address, int length, long long memoryLength, std::vector<unsigned char> &buffer)
	{
		if (_memoryReadPolicy == LegacyMemoryReadPolicy::ZeroPaddedCompatibility)
		{
			return buffer;
		}

		throw LegacyMachineStateReadException(address, length, memoryLength, LegacyMachineStateReadFailureKind::AddressOutOfRange);
	}

	long long LegacyProcessorMachineStateSource::GetTotalMemorySize()
	{
		return ((Processor::MainMemory == nullptr ? nullptr : Processor::MainMemory->getLength()) != nullptr) ? (Processor::MainMemory == nullptr ? nullptr : Processor::MainMemory->getLength()) : 0;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public MemorySubsystem? GetMemorySubsystem()
	std::shared_ptr<MemorySubsystem> LegacyProcessorMachineStateSource::GetMemorySubsystem()
	{
		return Processor::Memory;
	}

	std::shared_ptr<PerformanceReport> LegacyProcessorMachineStateSource::GetPerformanceReport()
	{
		return Processor::GetPerformanceStats();
	}

	ReplayPhaseMetrics LegacyProcessorMachineStateSource::GetReplayPhaseMetrics(int coreId)
	{
		return GetCoreSnapshot(coreId)->GetReplayPhaseMetrics();
	}

	SchedulerPhaseMetrics LegacyProcessorMachineStateSource::GetSchedulerPhaseMetrics(int coreId)
	{
		return GetCoreSnapshot(coreId)->GetSchedulerPhaseMetrics();
	}

	std::optional<std::shared_ptr<TypedSlotTelemetryProfile>> LegacyProcessorMachineStateSource::GetTypedSlotTelemetryProfile(int coreId, const std::wstring &programHash)
	{
		ArgumentException::ThrowIfNullOrWhiteSpace(programHash);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: PodController? pod = GetPod(coreId / PodController.CORES_PER_POD);
		std::shared_ptr<PodController> pod = GetPod(coreId / PodController::CORES_PER_POD);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((pod == nullptr ? nullptr : pod->getScheduler()) is not { } scheduler)
		{
			return std::nullopt;
		}

		return TelemetryExporter::BuildProfile(scheduler, programHash, GetCoreSnapshot(coreId)->GetPipelineControl());
	}

	std::wstring LegacyProcessorMachineStateSource::GetReplayToken()
	{
		return Processor::GetReplayToken();
	}
}
