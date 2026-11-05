#include "TestOnlyInitializerTrace.h"
#include "../../CloseToHSL/Core/Runtime/Managed/HybridCpuIseManagedGuestExecutionV1.h"

using namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed;

namespace HybridCPU_ISE::NonRTL::Runtime
{

	Guid TestOnlyInitializerTrace::getRunId() const
	{
		return RunId;
	}

	long long TestOnlyInitializerTrace::getDropped() const
	{
		return Dropped;
	}

	void TestOnlyInitializerTrace::setDropped(long long value)
	{
		Dropped = value;
	}

	std::optional<std::shared_ptr<HybridCpuIseManagedInitializerBindingV1>> TestOnlyInitializerTrace::getActive() const
	{
		return Active;
	}

	void TestOnlyInitializerTrace::setActive(const std::optional<std::shared_ptr<HybridCpuIseManagedInitializerBindingV1>> &value)
	{
		Active = value;
	}

	const std::vector<std::shared_ptr<TestOnlyInitializerEvent>> TestOnlyInitializerTrace::Snapshot()
	{
		return events;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal void Record(string kind, System.Nullable<HybridCpuIseManagedInitializerBindingV1> initializer, ulong imageBase, string? beginState, string? completionState, bool begun, bool completed, System.Nullable<HybridCpuIseManagedGuestExecutionResultV1> result = null)
	void TestOnlyInitializerTrace::Record(const std::wstring &kind, std::shared_ptr<HybridCpuIseManagedInitializerBindingV1> initializer, unsigned long long imageBase, const std::wstring &beginState, const std::wstring &completionState, bool begun, bool completed, std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> result)
	{
		if (kind == L"Begin")
		{
			setActive(initializer);
		}
		if (kind == L"Finalizer" || kind == L"Terminal")
		{
			setActive(std::nullopt);
		}
		if (events.size() == Capacity)
		{
			setDropped(getDropped() + 1);
			return;
		}
		// ExecutionFault catch paths in the existing runner can return zero after executing cycles.
		// Do not turn that legacy result into an exact measured delta.
		std::optional<int> cycles = !result ? std::nullopt : (result->Status == HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault && result->RetiredPipelineCycles == 0 ? std::nullopt : std::optional<int>(result->RetiredPipelineCycles));
		auto event = std::make_shared<TestOnlyInitializerEvent>(TestOnlyInitializerEvent{
			static_cast<long long>(events.size()) + 1, getRunId(), std::chrono::system_clock::now(), kind, std::move(initializer), imageBase,
			beginState, completionState, begun, completed, cycles, result ? result->GcSafepointsObserved : 0,
			result ? result->ProcessExitEcallsObserved : 0, result ? result->FinalProgramCounter : 0,
			result ? result->LastRetiredBundlePc : 0, result ? result->LastRetireSequence : 0,
			result ? std::optional<bool>(result->IsSuccess) : std::nullopt,
			result ? result->Reason : (kind == L"BeginRejected" ? L"Initializer admission rejected before ExecuteCore." : L"")});
		events.push_back(std::move(event));
	}
}
