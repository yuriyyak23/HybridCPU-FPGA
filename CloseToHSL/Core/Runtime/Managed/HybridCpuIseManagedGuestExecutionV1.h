#pragma once

#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../../../../NonRTL/Runtime/OwnedCoreObservationDiagnostics.h"
#include "../../../../NonRTL/Runtime/HybridCpuIseBootBlobContractsV1.h"
#include "SoftwareSecureOwnedEntry.h"
#include <string>
#include <vector>
#include <deque>
#include <stdexcept>
#include <optional>
#include <format>
#include <memory>
#include <functional>

using namespace HybridCPU::ManagedRuntime;
using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace HybridCPU_ISE::NonRTL::Runtime;

namespace HybridCPU_ISE::NonRTL::Runtime
{
	class IOwnedCoreObservationConsumer;
	class TestOnlyInitializerTrace;
}

namespace HybridCPU::ManagedRuntime
{
	struct HybridCpuManagedRetiredSafepointResultV1;
	class HybridCpuManagedGcStackBoundaryV1;
}

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{
	struct HybridCpuIseManagedInitializerBindingV1 final
	{
		int Order;
		std::wstring ModuleIdentity;
		std::wstring InitializerSymbol;
		unsigned long long EntryAddress;
		std::optional<unsigned long long> TypeId;
		std::optional<unsigned long long> TypeHandle;

		HybridCpuIseManagedInitializerBindingV1(int order, std::wstring moduleIdentity,
			std::wstring initializerSymbol, unsigned long long entryAddress,
			std::optional<unsigned long long> typeId, std::optional<unsigned long long> typeHandle)
			: Order(order), ModuleIdentity(std::move(moduleIdentity)), InitializerSymbol(std::move(initializerSymbol)),
			  EntryAddress(entryAddress), TypeId(typeId), TypeHandle(typeHandle) {}
	};
	struct HybridCpuIseGuestRegisterSeedV1 final
	{
		int Register; unsigned long long Value;
		HybridCpuIseGuestRegisterSeedV1(int reg, unsigned long long value) : Register(reg), Value(value) {}
	};
	struct HybridCpuIseManagedGuestExecutionRequestV1 final
	{
		unsigned long long ImageBase, EntryAddress, ReturnSentinel;
		int StackPointerRegister, FramePointerRegister, ThreadPointerRegister, ReturnAddressRegister, ReturnValueRegister, GlobalPointerRegister;
		unsigned long long StackPointer, FramePointer, ThreadPointer, ReturnAddress, GlobalPointer;
		std::shared_ptr<HybridCpuIseManagedImageLoadResultV1> LoadResult;
		int VirtualThreadId, MaximumPipelineCycles;
		bool RequireGcSafepointEvidence = false;
		std::vector<std::shared_ptr<HybridCpuIseGuestRegisterSeedV1>> AdditionalRegisterSeeds;
		bool RunPendingInitializers = true, TreatReturnSentinelAsProcessExit = true;
		std::function<std::shared_ptr<IOwnedCoreObservationConsumer>(OwnedCoreObservationBinding)> ObservationAttach;
		std::shared_ptr<TestOnlyInitializerTrace> TestOnlyInitializerTrace;
		std::shared_ptr<SoftwareSecureOwnedEntry> SoftwareSecureEntry;
	};

	/// <summary>
	/// Immutable startup facts extracted by the HCEXE inspector. HCEXE parsing deliberately
	/// remains outside ISE so this execution owner has no production compiler dependency.
	/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public sealed record HybridCpuIseManagedGuestExecutionRequestV1(ulong ImageBase, ulong EntryAddress, ulong ReturnSentinel, int StackPointerRegister, int FramePointerRegister, int ThreadPointerRegister, int ReturnAddressRegister, int ReturnValueRegister, int GlobalPointerRegister, ulong StackPointer, ulong FramePointer, ulong ThreadPointer, ulong ReturnAddress, ulong GlobalPointer, HybridCpuIseManagedImageLoadResultV1 LoadResult, int VirtualThreadId, int MaximumPipelineCycles, bool RequireGcSafepointEvidence = false, System.Nullable<IReadOnlyList<HybridCpuIseGuestRegisterSeedV1>> AdditionalRegisterSeeds = null, bool RunPendingInitializers = true, bool TreatReturnSentinelAsProcessExit = true, System.Func<OwnedCoreObservationBinding, IOwnedCoreObservationConsumer>? ObservationAttach = null, System.Nullable<TestOnlyInitializerTrace> TestOnlyInitializerTrace = null)
//	public sealed record HybridCpuIseManagedGuestExecutionRequestV1(ulong ImageBase, ulong EntryAddress, ulong ReturnSentinel, int StackPointerRegister, int FramePointerRegister, int ThreadPointerRegister, int ReturnAddressRegister, int ReturnValueRegister, int GlobalPointerRegister, ulong StackPointer, ulong FramePointer, ulong ThreadPointer, ulong ReturnAddress, ulong GlobalPointer, HybridCpuIseManagedImageLoadResultV1 LoadResult, int VirtualThreadId, int MaximumPipelineCycles, bool RequireGcSafepointEvidence = false, System.Nullable<IReadOnlyList<HybridCpuIseGuestRegisterSeedV1>> AdditionalRegisterSeeds = nullptr, bool RunPendingInitializers = true, bool TreatReturnSentinelAsProcessExit = true, System.Func<OwnedCoreObservationBinding, IOwnedCoreObservationConsumer> ObservationAttach = nullptr, TestOnlyInitializerTrace TestOnlyInitializerTrace = nullptr)
//	{
//		public System.Nullable<SoftwareSecureOwnedEntry> SoftwareSecureEntry
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HybridCpuIseGuestRegisterSeedV1(int Register, ulong Value);

	enum class HybridCpuIseManagedGuestExecutionStatusV1 : unsigned char
	{
		Completed = 0,
		InvalidRequest = 1,
		CycleBudgetExceeded = 2,
		KernelRejected = 3,
		ExecutionFault = 4,
		SoftwareSecureEntryStopped = 5
	};
	struct HybridCpuIseManagedGuestExecutionResultV1 final
	{
		HybridCpuIseManagedGuestExecutionStatusV1 Status; std::wstring Reason; int RetiredPipelineCycles;
		std::optional<int> ProcessExitCode; unsigned long long FinalProgramCounter; int GcSafepointsObserved;
		std::optional<std::vector<std::wstring>> GcResultDigests; unsigned long long ProcessExitEcallsObserved;
		unsigned long long LastRetiredBundlePc, LastRetireSequence;
		std::shared_ptr<OwnedCoreObservationDiagnostics> ObservationDiagnostics;
		bool IsSuccess;
		HybridCpuIseManagedGuestExecutionResultV1(HybridCpuIseManagedGuestExecutionStatusV1 status, std::wstring reason, int cycles, std::optional<int> exitCode, unsigned long long finalPc, int safepoints = 0, std::optional<std::vector<std::wstring>> digests = std::nullopt, unsigned long long processExitEcalls = 0, unsigned long long lastRetiredPc = 0, unsigned long long lastRetireSequence = 0)
			: Status(status), Reason(std::move(reason)), RetiredPipelineCycles(cycles), ProcessExitCode(exitCode), FinalProgramCounter(finalPc), GcSafepointsObserved(safepoints), GcResultDigests(std::move(digests)), ProcessExitEcallsObserved(processExitEcalls), LastRetiredBundlePc(lastRetiredPc), LastRetireSequence(lastRetireSequence), IsSuccess(status == HybridCpuIseManagedGuestExecutionStatusV1::Completed) {}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record HybridCpuIseManagedGuestExecutionResultV1(HybridCpuIseManagedGuestExecutionStatusV1 Status, string Reason, int RetiredPipelineCycles, System.Nullable<int> ProcessExitCode, ulong FinalProgramCounter, int GcSafepointsObserved = 0, System.Nullable<IReadOnlyList<string>> GcResultDigests = nullptr, ulong ProcessExitEcallsObserved = 0, ulong LastRetiredBundlePc = 0, ulong LastRetireSequence = 0)
//	{
//		public bool IsSuccess
//		{
//			get
//			{
//				return Status == HybridCpuIseManagedGuestExecutionStatusV1.Completed;
//			}
//		}
//		public System.Nullable<OwnedCoreObservationDiagnostics> ObservationDiagnostics
//		{
//			get;
//			set;
//		}
//	}

	/// <summary>
	/// Explicit, bounded managed-guest execution authority. It starts the real ISE pipeline,
	/// observes only retired state, and maps only the exact inspected return sentinel to the
	/// RuntimeKernel process-exit boundary. Host exceptions never become guest success.
	/// </summary>
	class HybridCpuIseManagedGuestExecutionRunnerV1 final : public std::enable_shared_from_this<HybridCpuIseManagedGuestExecutionRunnerV1>
	{
	private:
		static constexpr unsigned long long BundleBytes = 256;

	public:
		std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> Execute(std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request, std::shared_ptr<Processor::MainMemoryArea> memory, std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private HybridCpuIseManagedGuestExecutionResultV1 ExecuteCore(HybridCpuIseManagedGuestExecutionRequestV1 request, Processor.MainMemoryArea memory, IHybridCpuRuntimeKernelV1 kernel, OwnedCoreObservationLedger? diagnostics, string segmentKind)
		std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> ExecuteCore(std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request, std::shared_ptr<Processor::MainMemoryArea> memory, std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel, std::shared_ptr<OwnedCoreObservationLedger> diagnostics, const std::wstring &segmentKind);

		static std::wstring DescribeExceptionChain(std::runtime_error exception);

		static bool ValidRegister(int register_Keyword);

		static bool ValidAdditionalSeeds(std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request);

		static std::shared_ptr<HybridCpuManagedRetiredSafepointResultV1> CollectSafepoint(std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request, std::shared_ptr<Processor::MainMemoryArea> memory, std::shared_ptr<Processor::CPU_Core> core, unsigned long long retiredBundlePc);

		static void SeedStartupState(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request);

		static std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> Failure(HybridCpuIseManagedGuestExecutionStatusV1 status, const std::wstring &reason);
	};

}
