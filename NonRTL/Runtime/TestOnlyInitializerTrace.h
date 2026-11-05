#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <chrono>
#include "../../Guid.h"
#include "HybridCpuIseBootBlobContractsV1.h"
#include "../../CloseToHSL/Core/Runtime/Managed/HybridCpuIseManagedGuestExecutionV1.h"

using namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed;

namespace HybridCPU_ISE::NonRTL::Runtime
{
	struct TestOnlyInitializerEvent final
	{
		long long Sequence; Guid RunId; std::chrono::system_clock::time_point Utc; std::wstring Kind;
		std::shared_ptr<HybridCpuIseManagedInitializerBindingV1> Initializer; unsigned long long ImageBase;
		std::wstring BeginState, CompletionState; bool BeginAccepted, CompletionAccepted;
		std::optional<int> PipelineCycleDelta; int GcSafepointsObserved;
		unsigned long long ProcessExitCount, FinalPc, LastRetiredPc, RetireSequence;
		std::optional<bool> Success; std::wstring Reason;
	};

	/// <summary>Opt-in test evidence only. No core, memory, scheduler or execution control surface.</summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public sealed record TestOnlyInitializerEvent(long Sequence, Guid RunId, DateTimeOffset Utc, string Kind, System.Nullable<HybridCpuIseManagedInitializerBindingV1> Initializer, ulong ImageBase, string? BeginState, string? CompletionState, bool BeginAccepted, bool CompletionAccepted, System.Nullable<int> PipelineCycleDelta, int GcSafepointsObserved, ulong ProcessExitCount, ulong FinalPc, ulong LastRetiredPc, ulong RetireSequence, System.Nullable<bool> Success, string Reason);
//	public sealed record TestOnlyInitializerEvent(long Sequence, Guid RunId, DateTimeOffset Utc, string Kind, System.Nullable<HybridCpuIseManagedInitializerBindingV1> Initializer, ulong ImageBase, string BeginState, string CompletionState, bool BeginAccepted, bool CompletionAccepted, System.Nullable<int> PipelineCycleDelta, int GcSafepointsObserved, ulong ProcessExitCount, ulong FinalPc, ulong LastRetiredPc, ulong RetireSequence, System.Nullable<bool> Success, string Reason);

	class TestOnlyInitializerTrace final : public std::enable_shared_from_this<TestOnlyInitializerTrace>
	{
	private:
		const Guid RunId = Guid::NewGuid();
		long long Dropped = 0;
		std::optional<std::shared_ptr<HybridCpuIseManagedInitializerBindingV1>> Active;

		static constexpr int Capacity = 4096;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<TestOnlyInitializerEvent> events = new();
		std::vector<std::shared_ptr<TestOnlyInitializerEvent>> events = std::vector<std::shared_ptr<TestOnlyInitializerEvent>>();
	public:
		Guid getRunId() const;
		long long getDropped() const;
	private:
		void setDropped(long long value);
	public:
		std::optional<std::shared_ptr<HybridCpuIseManagedInitializerBindingV1>> getActive() const;
	private:
		void setActive(const std::optional<std::shared_ptr<HybridCpuIseManagedInitializerBindingV1>> &value);
	public:
		const std::vector<std::shared_ptr<TestOnlyInitializerEvent>> Snapshot();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal void Record(string kind, System.Nullable<HybridCpuIseManagedInitializerBindingV1> initializer, ulong imageBase, string? beginState, string? completionState, bool begun, bool completed, System.Nullable<HybridCpuIseManagedGuestExecutionResultV1> result = null)
		void Record(const std::wstring &kind, std::shared_ptr<HybridCpuIseManagedInitializerBindingV1> initializer, unsigned long long imageBase, const std::wstring &beginState, const std::wstring &completionState, bool begun, bool completed, std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> result = nullptr);
	};

}
