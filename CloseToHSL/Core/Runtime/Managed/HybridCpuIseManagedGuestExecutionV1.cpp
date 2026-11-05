#include "HybridCpuIseManagedGuestExecutionV1.h"
#include "../../../../NonRTL/Runtime/TestOnlyInitializerTrace.h"
#include "HybridCpuIseManagedEcallBridgeV1.h"
#include "../../../../NonRTL/Runtime/OwnedCoreObservationSession.h"
#include "../../ManagedRuntime/HybridCpuManagedGcRuntimeV1.h"
#include "../../../../NonRTL/Processor/Core/Processor.h"
#include "../../Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "../../State/CpuCorePlatformContext.h"
#include "../../Decoder/Sha256Digest.h"
#include <algorithm>
#include <array>
#include <span>
#include <string_view>
#include <stdexcept>

using namespace HybridCPU::ManagedRuntime;
using namespace HybridCPU::Platform::Contracts;
using namespace HybridCPU::RuntimeKernel;
using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace HybridCPU_ISE::NonRTL::Runtime;

namespace HybridCPU_ISE::CloseToHSL::Core::Runtime::Managed
{
	namespace
	{
		std::wstring HexLower(std::span<const unsigned char> bytes)
		{
			static constexpr wchar_t digits[] = L"0123456789abcdef";
			std::wstring result;
			result.reserve(bytes.size() * 2);
			for (unsigned char byte : bytes)
			{
				result.push_back(digits[byte >> 4]);
				result.push_back(digits[byte & 0x0f]);
			}
			return result;
		}

		std::wstring FormatHexList(const std::vector<unsigned long long> &values, std::size_t limit)
		{
			std::wstring result;
			const std::size_t count = (std::min)(values.size(), limit);
			for (std::size_t i = 0; i < count; ++i)
			{
				if (i != 0) result.push_back(L',');
				result += std::format(L"{:x}", values[i]);
			}
			return result;
		}

		template<typename TRange>
		std::wstring JoinWide(wchar_t separator, const TRange &values)
		{
			std::wstring result;
			bool first = true;
			for (const auto &value : values)
			{
				if (!first) result.push_back(separator);
				first = false;
				result += value;
			}
			return result;
		}
	}

	std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> HybridCpuIseManagedGuestExecutionRunnerV1::Execute(std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request, std::shared_ptr<Processor::MainMemoryArea> memory, std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel)
	{
		if (!request) throw std::invalid_argument("request");
		std::shared_ptr<OwnedCoreObservationLedger> diagnostics = request->ObservationAttach == nullptr ? nullptr : std::make_shared<OwnedCoreObservationLedger>();
		std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> result = ExecuteCore(request, memory, kernel, diagnostics, L"Main");
		if (request->TestOnlyInitializerTrace != nullptr)
		{
			request->TestOnlyInitializerTrace->Record(L"Terminal", nullptr, request->ImageBase, L"", L"", false, false, result);
		}
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
		if (diagnostics && result) result->ObservationDiagnostics = diagnostics->Snapshot();
		return result;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private HybridCpuIseManagedGuestExecutionResultV1 ExecuteCore(HybridCpuIseManagedGuestExecutionRequestV1 request, Processor.MainMemoryArea memory, IHybridCpuRuntimeKernelV1 kernel, OwnedCoreObservationLedger? diagnostics, string segmentKind)
	std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> HybridCpuIseManagedGuestExecutionRunnerV1::ExecuteCore(std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request, std::shared_ptr<Processor::MainMemoryArea> memory, std::shared_ptr<IHybridCpuRuntimeKernelV1> kernel, std::shared_ptr<OwnedCoreObservationLedger> diagnostics, const std::wstring &segmentKind)
	{
		if (!request || !memory || !kernel) throw std::invalid_argument("request, memory and kernel are required");

		std::shared_ptr<HybridCpuIseManagedImageLoadResultV1> load = request->LoadResult;
		if (!load)
			return Failure(HybridCpuIseManagedGuestExecutionStatusV1::InvalidRequest, L"Execution requires a successfully inspected and loaded HCEXE.");
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (request->SoftwareSecureEntry && (segmentKind != L"Main" || (load->PendingInitializers && !load->PendingInitializers->empty())))
		{
			return Failure(HybridCpuIseManagedGuestExecutionStatusV1::InvalidRequest, L"Secure entry requires one main segment without initializers.");
		}
		if (!load->IsSuccess || !load->Context || request->ImageBase == 0 || request->EntryAddress == 0 ||
			request->ReturnSentinel == 0 || request->VirtualThreadId < 0 ||
			request->VirtualThreadId >= Processor::CPU_Core::SmtWays || request->MaximumPipelineCycles <= 0 ||
			!ValidAdditionalSeeds(request))
		{
			return Failure(HybridCpuIseManagedGuestExecutionStatusV1::InvalidRequest, L"Execution requires one successfully inspected and loaded HCEXE, a live kernel context, a valid VT and a finite cycle budget.");
		}

		if (request->EntryAddress < request->ImageBase || request->EntryAddress % BundleBytes != 0 ||
			!ValidRegister(request->StackPointerRegister) || !ValidRegister(request->FramePointerRegister) ||
			!ValidRegister(request->ThreadPointerRegister) || !ValidRegister(request->ReturnAddressRegister) ||
			!ValidRegister(request->ReturnValueRegister) || !ValidRegister(request->GlobalPointerRegister) ||
			load->Context->VirtualThreadCarrier != request->VirtualThreadId)
		{
			return Failure(HybridCpuIseManagedGuestExecutionStatusV1::InvalidRequest, L"HCEXE entry, register state or RuntimeKernel virtual-thread binding is invalid for guest execution.");
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (request->RunPendingInitializers && load->PendingInitializers && !load->PendingInitializers->empty())
		{
			auto pending = *load->PendingInitializers;
			std::stable_sort(pending.begin(), pending.end(), [](const auto& left, const auto& right) {
				return left && right ? left->Order < right->Order : static_cast<bool>(right);
			});
			for (const auto& initializer : pending)
			{
				if (!initializer)
					return Failure(HybridCpuIseManagedGuestExecutionStatusV1::InvalidRequest, L"Initializer binding is missing.");
				auto trace = request->TestOnlyInitializerTrace;
				const auto typeSystem = load->TypeSystem;
				const auto describeTypeState = [&]() -> std::wstring {
					return initializer->TypeId && typeSystem
						? std::to_wstring(static_cast<int>(typeSystem->InitializationState(*initializer->TypeId)))
						: std::wstring{};
				};
				const std::wstring beginState = trace ? describeTypeState() : std::wstring{};
				bool begun = false;
				bool completed = false;
				std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> initialized;
				struct Finalizer final
				{
					std::function<void()> Action;
					~Finalizer() { try { if (Action) Action(); } catch (...) {} }
				} finalizer;
				finalizer.Action = [&]() {
					if (trace)
						trace->Record(begun ? L"Finalizer" : L"BeginRejected", initializer, request->ImageBase,
							beginState, describeTypeState(), begun, completed, initialized);
				};

				auto managedBridge = std::dynamic_pointer_cast<HybridCpuIseManagedEcallBridgeV1>(load->EcallBridge);
				if (initializer->TypeId && (!typeSystem || !typeSystem->TryBeginInitialization(*initializer->TypeId)))
					return Failure(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
						std::format(L"Initializer '{}' type-state transition to Running was rejected for TypeId 0x{:016x}.",
							initializer->InitializerSymbol, *initializer->TypeId));
				if (initializer->TypeHandle && (!managedBridge || !managedBridge->TryBeginCpuInitializer(*initializer->TypeHandle)))
					return Failure(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
						std::format(L"Initializer '{}' exact CPU-context TypeHandle activation was rejected.", initializer->InitializerSymbol));

				begun = true;
				if (trace)
					trace->Record(L"Begin", initializer, request->ImageBase, beginState, describeTypeState(), true, false);

				auto initializerRequest = std::make_shared<HybridCpuIseManagedGuestExecutionRequestV1>(*request);
				initializerRequest->EntryAddress = initializer->EntryAddress;
				initializerRequest->RequireGcSafepointEvidence = false;
				initializerRequest->RunPendingInitializers = false;
				initializerRequest->TreatReturnSentinelAsProcessExit = false;
				initialized = ExecuteCore(initializerRequest, memory, kernel, diagnostics, L"Initializer");
				const bool succeeded = initialized && initialized->IsSuccess;

				if (initializer->TypeHandle && (!managedBridge || !managedBridge->TryCompleteCpuInitializer(*initializer->TypeHandle)))
					return Failure(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
						std::format(L"Initializer '{}' exact CPU-context TypeHandle completion was rejected.", initializer->InitializerSymbol));
				if (initializer->TypeId && (!typeSystem || !typeSystem->TryCompleteInitialization(*initializer->TypeId, succeeded)))
					return Failure(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
						std::format(L"Initializer '{}' type-state completion was rejected.", initializer->InitializerSymbol));

				completed = true;
				if (!succeeded)
				{
					if (!initialized)
						return Failure(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
							std::format(L"Initializer '{}' produced no result.", initializer->InitializerSymbol));
					initialized->Reason = std::format(L"Initializer '{}' failed on the ISE CPU: {}",
						initializer->InitializerSymbol, initialized->Reason);
					return initialized;
				}
			}
		}

		std::shared_ptr<Processor::CPU_Core> core = nullptr;
		unsigned long long lastRetiredBundlePc = 0;
		unsigned long long lastRetireSequence = 0;
		std::wstring priorRetiredLaneDiagnostic = L"unavailable";
		std::wstring lastRetiredLaneDiagnostic = L"unavailable";
		std::vector<std::wstring> x17RetireTrace;
		std::deque<std::wstring> x18RetireTrace;
		std::deque<std::wstring> x1RetireTrace;
		std::deque<std::wstring> x20RetireTrace;
		std::deque<std::wstring> x10RetireTrace;
		std::deque<std::wstring> x24RetireTrace;
		constexpr int diagnosticControlRetireTraceCapacity = 300'000;
		std::deque<std::wstring> controlRetireTrace;
		unsigned long long firstNonEmptyDecodedPc = 0;
		unsigned long long lastNonEmptyDecodedPc = 0;
		unsigned char observedDecodedValidMask = 0;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: byte[] entryBundle = new byte[checked((int)BundleBytes)];
		std::vector<unsigned char> entryBundle(static_cast<int>(BundleBytes));
		if (!memory->TryReadPhysicalRange(request->EntryAddress, entryBundle))
		{
			return Failure(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault, L"ISE memory rejected a read-back of the exact HCEXE entry bundle.");
		}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to this .NET Convert class method:
// C# TO C++ CONVERTER TASK: There is no direct C++ equivalent to this .NET String method:
		std::wstring entryBundleDigest = YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(std::span<const unsigned char>(entryBundle));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to this .NET Convert class method:
// C# TO C++ CONVERTER TASK: There is no direct C++ equivalent to this .NET String method:
		std::wstring entryInstructionPrefix = HexLower(std::span<const unsigned char>(entryBundle.data(), (std::min)(entryBundle.size(), static_cast<std::size_t>(32))));
		int safepoints = 0;
		std::vector<std::wstring> gcDigests;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: OwnedCoreObservationSession? observationSession = null;
		std::shared_ptr<OwnedCoreObservationSession> observationSession = nullptr;
		try
		{
			YAKSys_Hybrid_CPU::Core::CpuCorePlatformContext platformContext = request->SoftwareSecureEntry == nullptr
				? YAKSys_Hybrid_CPU::Core::CpuCorePlatformContext::CreateFixed(memory, ProcessorMode::Compiler, load->EcallBridge)
				: YAKSys_Hybrid_CPU::Core::CpuCorePlatformContext::CreateSoftwareSecureFixed(memory, ProcessorMode::Compiler, load->EcallBridge);
			core = std::make_shared<Processor::CPU_Core>(0, std::make_shared<YAKSys_Hybrid_CPU::Core::CpuCorePlatformContext>(std::move(platformContext)));
			core->InitializePipeline();
			core->PrepareExecutionStart(request->EntryAddress, request->VirtualThreadId);
			core->SetExecutionPrivilegeForStartup(request->VirtualThreadId, PrivilegeLevel::User);
			SeedStartupState(core, request);
			if (!request->AdditionalRegisterSeeds.empty())
			{
				for (const std::shared_ptr<HybridCpuIseGuestRegisterSeedV1> &seed : request->AdditionalRegisterSeeds)
				{
					core->WriteCommittedArch(request->VirtualThreadId, seed->Register, seed->Value);
				}
			}
			std::shared_ptr<HybridCpuVmRangeV1> executableImageRange;
			unsigned int executableImageMatchCount = 0;
			const auto &contextDescriptor = load->Context;
			const auto mappings = contextDescriptor ? contextDescriptor->VmMappings : nullptr;
			if (mappings)
			{
				for (const std::shared_ptr<HybridCpuVmRangeV1> &mapping : *mappings)
				{
					if (!mapping || (static_cast<unsigned char>(mapping->Protection) & static_cast<unsigned char>(HybridCpuVmProtectionV1::Execute)) == 0)
						continue;
					if (mapping->Address <= request->EntryAddress && request->EntryAddress < mapping->Address + mapping->Size)
					{
						++executableImageMatchCount;
						executableImageRange = mapping;
					}
				}
			}
			if (executableImageMatchCount != 1)
			{
				return Failure(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault, L"RuntimeKernel did not publish one exact executable image mapping for the HCEXE entry.");
			}
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong executableImageEnd = checked(executableImageRange.Address + executableImageRange.Size);
			unsigned long long executableImageEnd = executableImageRange->Address + executableImageRange->Size;
			unsigned long long observedRetireSequence = 0;
			std::deque<std::wstring> gcRetireTrace;
			observationSession = request->ObservationAttach == nullptr ? nullptr : std::make_shared<OwnedCoreObservationSession>(core, request->EntryAddress, request->ObservationAttach, segmentKind);

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
			if (request->SoftwareSecureEntry != nullptr)
			{
				request->SoftwareSecureEntry->Execute(core, request->MaximumPipelineCycles, request->VirtualThreadId);
				return Failure(HybridCpuIseManagedGuestExecutionStatusV1::SoftwareSecureEntryStopped, L"Explicit secure entry stopped; not an ordinary guest terminal result.");
			}

			for (int cycle = 0; cycle < request->MaximumPipelineCycles; cycle++)
			{
				if (observationSession != nullptr)
				{
					observationSession->OnBoundary();
				}
				core->ExecuteRuntimePipelineCycle();
				Processor::CPU_Core::PipelineObservationSnapshot cycleObservation = core->GetPipelineObservationSnapshot();
				if (cycleObservation.getDecodedBundleValidMask() != 0)
				{
					if (firstNonEmptyDecodedPc == 0)
					{
						firstNonEmptyDecodedPc = cycleObservation.getDecodedBundlePc();
					}
					lastNonEmptyDecodedPc = cycleObservation.getDecodedBundlePc();
					observedDecodedValidMask |= cycleObservation.getDecodedBundleValidMask();
				}
				unsigned long long retiredBundlePc;
				unsigned long long retireSequence;
				if (core->TryReadLastRetiredBundle(retiredBundlePc, retireSequence) && retireSequence != observedRetireSequence)
				{
					observedRetireSequence = retireSequence;
					lastRetiredBundlePc = retiredBundlePc;
					lastRetireSequence = retireSequence;
					priorRetiredLaneDiagnostic = lastRetiredLaneDiagnostic;
					unsigned char retiredLaneIndex;
					unsigned int retiredOpcode;
					bool retiredWritesRegister;
					unsigned short retiredDestinationRegister;
					unsigned long long retiredResultValue;
					std::wstring retiredMicroOpType;
					std::wstring retiredMicroOpDescription;
					if (core->TryReadLastRetiredLaneDiagnostic(retiredLaneIndex, retiredOpcode, retiredWritesRegister, retiredDestinationRegister, retiredResultValue, retiredMicroOpType, retiredMicroOpDescription))
					{
						lastRetiredLaneDiagnostic = std::format(L"lane={},opcode=0x{:x},writes={},", static_cast<unsigned int>(retiredLaneIndex), retiredOpcode, retiredWritesRegister) + std::format(L"dest=x{},value=0x{:016x},uop={}", retiredDestinationRegister, retiredResultValue, retiredMicroOpType);
						if (retiredWritesRegister && retiredDestinationRegister == 17)
						{
							x17RetireTrace.push_back(std::format(L"pc=0x{:x},op=0x{:x},value=0x{:x},desc={}", retiredBundlePc, retiredOpcode, retiredResultValue, retiredMicroOpDescription));
						}
						if (retiredWritesRegister && retiredDestinationRegister == 18)
						{
							if (x18RetireTrace.size() == 128)
							{
								x18RetireTrace.pop_front();
							}
							x18RetireTrace.push_back(std::format(L"pc=0x{:x},op=0x{:x},value=0x{:x},desc={}", retiredBundlePc, retiredOpcode, retiredResultValue, retiredMicroOpDescription));
						}
						if (retiredWritesRegister && retiredDestinationRegister == 20)
						{
							if (x20RetireTrace.size() == 512)
							{
								x20RetireTrace.pop_front();
							}
							x20RetireTrace.push_back(std::format(L"pc=0x{:x},op=0x{:x},value=0x{:x},desc={}", retiredBundlePc, retiredOpcode, retiredResultValue, retiredMicroOpDescription));
						}
						if (retiredWritesRegister && (retiredDestinationRegister == 10 || retiredDestinationRegister == 24))
						{
							std::deque<std::wstring> &trace = retiredDestinationRegister == 10 ? x10RetireTrace : x24RetireTrace;
							if (trace.size() == 512)
							{
								trace.pop_front();
							}
							trace.push_back(std::format(L"pc=0x{:x},op=0x{:x},value=0x{:x},desc={}", retiredBundlePc, retiredOpcode, retiredResultValue, retiredMicroOpDescription));
						}
						if (retiredWritesRegister && retiredDestinationRegister == 1)
						{
							if (x1RetireTrace.size() == 128)
							{
								x1RetireTrace.pop_front();
							}
							x1RetireTrace.push_back(std::format(L"pc=0x{:x},op=0x{:x},value=0x{:x},desc={}", retiredBundlePc, retiredOpcode, retiredResultValue, retiredMicroOpDescription));
						}
						if (retiredWritesRegister && retiredDestinationRegister == 11 || retiredOpcode == 0xb0 or 0xb1 or 0xb2 or 0xb3 or 0xb4 or 0xb5)
						{
							if (controlRetireTrace.size() == diagnosticControlRetireTraceCapacity)
							{
								controlRetireTrace.pop_front();
							}
							controlRetireTrace.push_back(std::format(L"pc=0x{:x},op=0x{:x},writes={},", retiredBundlePc, retiredOpcode, retiredWritesRegister) + std::format(L"dest=x{},value=0x{:x},desc={}", retiredDestinationRegister, retiredResultValue, retiredMicroOpDescription));
						}
					}
					if (request->RequireGcSafepointEvidence)
					{
						std::shared_ptr<HybridCpuManagedRetiredSafepointResultV1> safepoint = CollectSafepoint(request, memory, core, retiredBundlePc);
						if (safepoint == nullptr)
						{
							return Failure(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault, L"Managed GC safepoint collector returned no result.");
						}
						if (safepoint->Status == HybridCpuManagedRetiredSafepointStatusV1::Rejected)
						{
							const std::wstring reason = L"Retired GC safepoint was rejected: " + safepoint->Reason + L" Prior collections: " + JoinWide(L'|', gcRetireTrace) +
								L" x10-retire-trace=[" + JoinWide(L';', x10RetireTrace) + L"]," +
								L" x18-retire-trace=[" + JoinWide(L';', x18RetireTrace) + L"]," +
								L" x24-retire-trace=[" + JoinWide(L';', x24RetireTrace) + L"]," +
								L" control-retire-trace=[" + JoinWide(L';', controlRetireTrace) + L"].";
							return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault, reason, cycle + 1, std::nullopt, core->ReadCommittedPc(request->VirtualThreadId), safepoints, gcDigests, 0, lastRetiredBundlePc, lastRetireSequence);
						}
						if (safepoint->IsSuccess && safepoint->Collection != nullptr)
						{
							safepoints++;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: gcDigests.Add(safepoint.Collection!.ResultDigest);
							gcDigests.push_back(safepoint->Collection->ResultDigest);
							if (gcRetireTrace.size() == 32)
							{
								gcRetireTrace.pop_front();
							}
							const auto &collection = *safepoint->Collection;
							gcRetireTrace.push_back(std::format(L"pc=0x{:x},sp=0x{:x},roots={}[{}],reclaimed={}[{}]",
								retiredBundlePc, core->ReadArch(request->VirtualThreadId, request->StackPointerRegister),
								collection.Roots.size(), FormatHexList(collection.Roots, 64),
								collection.ReclaimedObjects.size(), FormatHexList(collection.ReclaimedObjects, 64)));
						}
					}
				}
				auto managedBridge = std::dynamic_pointer_cast<HybridCpuIseManagedEcallBridgeV1>(load->EcallBridge);
				const std::optional<int> ecallExitCode = managedBridge ? managedBridge->getLastProcessExitCode() : std::nullopt;
				if (managedBridge && ecallExitCode.has_value())
				{
					if (!request->TreatReturnSentinelAsProcessExit)
					{
						const unsigned long long diagnosticX10 = core->ReadArch(request->VirtualThreadId, 10);
						const unsigned long long diagnosticX11 = core->ReadArch(request->VirtualThreadId, 11);
						const unsigned long long diagnosticX12 = core->ReadArch(request->VirtualThreadId, 12);
						const unsigned long long diagnosticX17 = core->ReadArch(request->VirtualThreadId, 17);
						const unsigned long long diagnosticX1 = core->ReadArch(request->VirtualThreadId, 1);
						const unsigned long long diagnosticX2 = core->ReadArch(request->VirtualThreadId, 2);
						const std::vector<int> diagnosticCalleeSavedRegisters = {8, 9, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
						std::wstring diagnosticCalleeSaved;
						for (int registerKeyword : diagnosticCalleeSavedRegisters)
						{
							if (!diagnosticCalleeSaved.empty()) diagnosticCalleeSaved.push_back(L',');
							diagnosticCalleeSaved += std::format(L"x{}=0x{:x}", registerKeyword, core->ReadArch(request->VirtualThreadId, registerKeyword));
						}

						std::wstring reason = std::format(L"Image initializer invoked ProcessExit({}) instead of returning to its exact sentinel; ", *ecallExitCode);
						reason += std::format(L"last-managed-operation=0x{:x}, ", managedBridge->getLastManagedOperation());
						const auto managedStatus = managedBridge->getLastManagedStatus();
						reason += managedStatus ? std::format(L"last-managed-status={}, ", static_cast<unsigned int>(*managedStatus)) : L"last-managed-status=none, ";
						reason += std::format(L"last-managed-reason='{}', ", managedBridge->getLastManagedReason());
						const auto externalStatus = managedBridge->getLastManagedExternalStatus();
						reason += externalStatus ? std::format(L"last-managed-external-status={}, ", static_cast<unsigned int>(*externalStatus)) : L"last-managed-external-status=none, ";
						const auto externalError = managedBridge->getLastManagedExternalError();
						reason += externalError ? std::format(L"last-managed-external-error={}, ", *externalError) : L"last-managed-external-error=none, ";
						reason += std::format(L"last-managed-external-value=0x{:x}, ", managedBridge->getLastManagedExternalValue());
						reason += std::format(L"last-managed-receiver=0x{:x},last-managed-argument1=0x{:x},last-managed-argument2=0x{:x}, ",
							managedBridge->getLastManagedReceiver(), managedBridge->getLastManagedArgument1(), managedBridge->getLastManagedArgument2());
						reason += std::format(L"x1=0x{:x},x2=0x{:x},x10=0x{:x},x11=0x{:x},x12=0x{:x},x17=0x{:x}, ",
							diagnosticX1, diagnosticX2, diagnosticX10, diagnosticX11, diagnosticX12, diagnosticX17);
						reason += std::format(L"callee-saved=[{}], last-retired-lane=({}), ", diagnosticCalleeSaved, lastRetiredLaneDiagnostic);
						reason += L"x18-retire-trace=[" + JoinWide(L';', x18RetireTrace) + L"], ";
						reason += L"x10-retire-trace=[" + JoinWide(L';', x10RetireTrace) + L"], ";
						reason += L"x24-retire-trace=[" + JoinWide(L';', x24RetireTrace) + L"], ";
						reason += L"x17-retire-trace=[" + JoinWide(L';', x17RetireTrace) + L"], ";
						reason += L"control-retire-trace=[" + JoinWide(L';', controlRetireTrace) + L"].";
						return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
							std::move(reason), cycle + 1, ecallExitCode, core->ReadCommittedPc(request->VirtualThreadId), safepoints,
							gcDigests, managedBridge->getProcessExitRetireCount(), lastRetiredBundlePc, lastRetireSequence);
					}
					if (request->RequireGcSafepointEvidence && safepoints == 0)
					{
						return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
							L"Guest reached ECALL process exit without one exact retired GC safepoint.", cycle + 1, std::nullopt,
							core->ReadCommittedPc(request->VirtualThreadId), 0, gcDigests, managedBridge->getProcessExitRetireCount());
					}
					return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::Completed,
						L"", cycle + 1, ecallExitCode, core->ReadCommittedPc(request->VirtualThreadId), safepoints,
						gcDigests, managedBridge->getProcessExitRetireCount());
				}				unsigned long long retiredPc = core->ReadCommittedPc(request->VirtualThreadId);
				if (retiredPc != request->ReturnSentinel)
				{
					unsigned long long activePc = cycleObservation.getActiveLivePc();
					if (activePc != request->ReturnSentinel && (activePc < executableImageRange->Address || activePc >= executableImageEnd))
					{
						unsigned long long x3 = core->ReadArch(request->VirtualThreadId, 3);
						unsigned long long x5 = core->ReadArch(request->VirtualThreadId, 5);
						unsigned long long x10 = core->ReadArch(request->VirtualThreadId, 10);
						unsigned long long x17 = core->ReadArch(request->VirtualThreadId, 17);
						unsigned long long x28 = core->ReadArch(request->VirtualThreadId, 28);
						unsigned long long x5Vt0 = core->ReadArch(0, 5);
						unsigned long long x5Vt1 = core->ReadArch(1, 5);
						unsigned long long x5Vt2 = core->ReadArch(2, 5);
						unsigned long long x5Vt3 = core->ReadArch(3, 5);
// C# TO C++ CONVERTER TASK: There is no equivalent to 'stackalloc' in C++:
						std::array<unsigned char, 8> pointedBytes{};
						unsigned long long pointedValue = 0;
						if (memory->TryReadPhysicalRange(x28, std::span<unsigned char>(pointedBytes)))
						{
							for (std::size_t byteIndex = 0; byteIndex < pointedBytes.size(); ++byteIndex)
								pointedValue |= static_cast<unsigned long long>(pointedBytes[byteIndex]) << (byteIndex * 8);
						}
						std::vector<unsigned char> priorRetiredBundleBytes(32);
						constexpr unsigned long long bundleSizeBytes = 256;
						unsigned long long priorRetiredBundlePc = lastRetiredBundlePc >= bundleSizeBytes ? lastRetiredBundlePc - bundleSizeBytes : 0;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to this .NET Convert class method:
// C# TO C++ CONVERTER TASK: There is no direct C++ equivalent to this .NET String method:
						std::wstring priorRetiredInstruction = priorRetiredBundlePc != 0 && memory->TryReadPhysicalRange(priorRetiredBundlePc, priorRetiredBundleBytes) ? HexLower(priorRetiredBundleBytes) : L"unavailable";
						std::shared_ptr<HybridCpuIseManagedEcallBridgeV1> rejectedBridge = std::dynamic_pointer_cast<HybridCpuIseManagedEcallBridgeV1>(load->EcallBridge);
						std::wstring ecallRejection = rejectedBridge != nullptr ? rejectedBridge->getLastRejectedReason() : L"unavailable";
						std::wstring reason = L"Guest control flow escaped its exact RuntimeKernel executable image mapping: ";
						reason += std::format(L"active-pc=0x{:016x}, mapping=[0x{:016x},0x{:016x}), ", activePc, executableImageRange->Address, executableImageEnd);
						reason += std::format(L"x3=0x{:016x}, x5=0x{:016x}, x10=0x{:016x}, x17=0x{:016x}, x28=0x{:016x}, ", x3, x5, x10, x17, x28);
						reason += std::format(L"x5-by-vt=[0x{:016x},0x{:016x},0x{:016x},0x{:016x}], ", x5Vt0, x5Vt1, x5Vt2, x5Vt3);
						reason += std::format(L"memory[x28]=0x{:016x}, prior-retired-pc=0x{:016x}, ", pointedValue, priorRetiredBundlePc);
						reason += std::format(L"prior-retired-instruction={}, prior-retired-lane=({}), last-retired-lane=({}), ", priorRetiredInstruction, priorRetiredLaneDiagnostic, lastRetiredLaneDiagnostic);
						reason += L"x17-retire-trace=[" + JoinWide(L';', x17RetireTrace) + L"], ecall-rejection=(" + ecallRejection + L").";
						return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
							std::move(reason), cycle + 1, std::nullopt, retiredPc, safepoints, gcDigests, 0, lastRetiredBundlePc, lastRetireSequence);
					}
					continue;
				}

				if (request->RequireGcSafepointEvidence && safepoints == 0)
				{
					return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault, L"Guest reached process exit without one exact retired GC safepoint.", cycle + 1, std::nullopt, retiredPc, 0, gcDigests);
				}

				unsigned long long exitRegister = core->ReadArch(request->VirtualThreadId, request->ReturnValueRegister);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: if (exitRegister != unchecked((ulong)(uint)exitRegister))
				if (exitRegister != static_cast<unsigned long long>(static_cast<unsigned int>(exitRegister)))
				{
					return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault, L"Guest process-exit value is not a canonical 32-bit exit code.", cycle + 1, std::nullopt, retiredPc, safepoints, gcDigests);
				}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: int exitCode = unchecked((int)(uint)exitRegister);
				int exitCode = static_cast<int>(static_cast<unsigned int>(exitRegister));
				if (!request->TreatReturnSentinelAsProcessExit)
				{
					return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::Completed, L"", cycle + 1, std::nullopt, retiredPc, safepoints, gcDigests, 0, lastRetiredBundlePc, lastRetireSequence);
				}
				std::shared_ptr<HybridCpuKernelResultV1> exited = kernel->ProcessExit(exitCode);
				return exited->IsSuccess
					? std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::Completed, L"", cycle + 1, exitCode, retiredPc, safepoints, gcDigests, 0, lastRetiredBundlePc, lastRetireSequence)
					: std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::KernelRejected, exited->Reason, cycle + 1, std::nullopt, retiredPc, safepoints, gcDigests, 0, lastRetiredBundlePc, lastRetireSequence);
			}
		}
		catch (const std::exception &exception)
		{
			if (dynamic_cast<const InvalidOperationException *>(&exception) == nullptr &&
				dynamic_cast<const std::overflow_error *>(&exception) == nullptr)
			{
				throw;
			}
			const unsigned long long committedPc = core ? core->ReadCommittedPc(request->VirtualThreadId) : 0;
			return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::ExecutionFault,
				L"ISE pipeline rejected the guest image: " + std::wstring(exception.what(), exception.what() + std::char_traits<char>::length(exception.what())),
				0, std::nullopt, committedPc, 0, std::nullopt, 0, lastRetiredBundlePc, lastRetireSequence);
		}
		if (observationSession != nullptr)
		{
			observationSession->dispose();
			if (diagnostics) diagnostics->Record(observationSession->getReport());
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: Processor.CPU_Core.PipelineObservationSnapshot observation = core!.GetPipelineObservationSnapshot();
		Processor::CPU_Core::PipelineObservationSnapshot observation = core->GetPipelineObservationSnapshot();
		Processor::CPU_Core::PipelineControl control = observation.getPipelineControl();
		std::vector<int> boundedDiagnosticRegisters = {1, 2, 5, 8, 9, 10, 11, 12, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};
		std::wstring boundedRegisterState;
		for (std::size_t i = 0; i < boundedDiagnosticRegisters.size(); ++i)
		{
			if (i) boundedRegisterState += L',';
			const int reg = boundedDiagnosticRegisters[i];
			boundedRegisterState += std::format(L"x{}=0x{:x}", reg, core->ReadArch(request->VirtualThreadId, reg));
		}
		std::wstring reason = L"Guest did not reach the exact restricted process-exit sentinel within its bounded pipeline-cycle budget; ";
		reason += std::format(L"committed-pc=0x{:016x}, active-live-pc=0x{:016x}, ", core->ReadCommittedPc(request->VirtualThreadId), observation.getActiveLivePc());
		reason += std::format(L"vt-state={}, stalled={}, stall-kind={}, stall-cycles={}, ", static_cast<unsigned int>(observation.getCurrentVirtualThreadPipelineState()), control.Stalled, static_cast<unsigned int>(control.StallReason), control.getStallCycles());
		reason += std::format(L"decoded-pc=0x{:016x}, valid-mask=0x{:02x}, nop-mask=0x{:02x}, decode-fault={}, ", observation.getDecodedBundlePc(), observation.getDecodedBundleValidMask(), observation.getDecodedBundleNopMask(), observation.getDecodedBundleHasDecodeFault());
		reason += std::format(L"first-nonempty-decoded-pc=0x{:016x}, last-nonempty-decoded-pc=0x{:016x}, observed-valid-mask=0x{:02x}, instructions-retired={}, ", firstNonEmptyDecodedPc, lastNonEmptyDecodedPc, observedDecodedValidMask, control.getInstructionsRetired());
		reason += L"bounded-register-state=[" + boundedRegisterState + L"], x1-retire-trace=[" + JoinWide(L';', x1RetireTrace) + L"], x20-retire-trace=[" + JoinWide(L';', x20RetireTrace) + L"], ";
		reason += L"x10-retire-trace=[" + JoinWide(L';', x10RetireTrace) + L"], x24-retire-trace=[" + JoinWide(L';', x24RetireTrace) + L"], control-retire-trace=[" + JoinWide(L';', controlRetireTrace) + L"], ";
		reason += std::format(L"entry-bundle-sha256={}, entry-instruction={}, last-retired-bundle=0x{:016x}, retire-sequence={}", entryBundleDigest, entryInstructionPrefix, lastRetiredBundlePc, lastRetireSequence);
		return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(HybridCpuIseManagedGuestExecutionStatusV1::CycleBudgetExceeded, std::move(reason), request->MaximumPipelineCycles, std::nullopt, core->ReadCommittedPc(request->VirtualThreadId), safepoints, gcDigests, 0, lastRetiredBundlePc, lastRetireSequence);
	}

	std::wstring HybridCpuIseManagedGuestExecutionRunnerV1::DescribeExceptionChain(std::runtime_error exception)
	{
		std::vector<std::wstring> messages;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: for (Exception? current = exception; current != null; current = current.InnerException)
		messages.emplace_back(exception.what(), exception.what() + std::char_traits<char>::length(exception.what()));
		return JoinWide(L' ', messages);
	}

	bool HybridCpuIseManagedGuestExecutionRunnerV1::ValidRegister(int register_Keyword)
	{
		return register_Keyword >= 0 && register_Keyword <= 31;
	}

	bool HybridCpuIseManagedGuestExecutionRunnerV1::ValidAdditionalSeeds(std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request)
	{
		const auto& seeds = request->AdditionalRegisterSeeds;
		if (seeds.size() > 31)
			return false;
		const std::array<int, 6> startupRegisters = {
			request->StackPointerRegister, request->FramePointerRegister,
			request->ThreadPointerRegister, request->ReturnAddressRegister,
			request->ReturnValueRegister, request->GlobalPointerRegister
		};
		std::array<bool, 32> seen{};
		for (const auto& seed : seeds)
		{
			if (!seed || seed->Register <= 0 || seed->Register > 31 || seen[seed->Register])
				return false;
			if (std::find(startupRegisters.begin(), startupRegisters.end(), seed->Register) != startupRegisters.end())
				return false;
			seen[seed->Register] = true;
		}
		return true;
	}

	std::shared_ptr<HybridCpuManagedRetiredSafepointResultV1> HybridCpuIseManagedGuestExecutionRunnerV1::CollectSafepoint(std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request, std::shared_ptr<Processor::MainMemoryArea> memory, std::shared_ptr<Processor::CPU_Core> core, unsigned long long retiredBundlePc)
	{
		std::shared_ptr<HybridCpuIseManagedImageLoadResultV1> load = request->LoadResult;
		if (load->Gc == nullptr || load->StackMaps == nullptr)
		{
			return std::make_shared<HybridCpuManagedRetiredSafepointResultV1>(HybridCpuManagedRetiredSafepointStatusV1::Rejected, L"Loader did not publish image-owned GC registrations.");
		}
		int relativePc;
		try
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: relativePc = checked((int)(retiredBundlePc - request.ImageBase));
			relativePc = static_cast<int>(retiredBundlePc - request->ImageBase);
		}
		catch (const std::overflow_error &e1)
		{
			return std::make_shared<HybridCpuManagedRetiredSafepointResultV1>(HybridCpuManagedRetiredSafepointStatusV1::Rejected, L"Retired bundle PC is outside the inspected image coordinate space.");
		}
		std::vector<unsigned long long> registers(64);
		for (int register_Keyword = 0; register_Keyword < 32; register_Keyword++)
		{
			registers[register_Keyword] = core->ReadArch(request->VirtualThreadId, register_Keyword);
		}
		unsigned long long stackPointer = registers[request->StackPointerRegister];
		return load->Gc->CollectRetiredSafepoint(load->StackMaps, relativePc, registers, stackPointer);
	}

	void HybridCpuIseManagedGuestExecutionRunnerV1::SeedStartupState(std::shared_ptr<Processor::CPU_Core> core, std::shared_ptr<HybridCpuIseManagedGuestExecutionRequestV1> request)
	{
		core->WriteCommittedPc(request->VirtualThreadId, request->EntryAddress);
		core->WriteCommittedArch(request->VirtualThreadId, request->StackPointerRegister, request->StackPointer);
		core->WriteCommittedArch(request->VirtualThreadId, request->FramePointerRegister, request->FramePointer);
		core->WriteCommittedArch(request->VirtualThreadId, request->ThreadPointerRegister, request->ThreadPointer);
		core->WriteCommittedArch(request->VirtualThreadId, request->ReturnAddressRegister, request->ReturnAddress);
		core->WriteCommittedArch(request->VirtualThreadId, request->GlobalPointerRegister, request->GlobalPointer);
	}

	std::shared_ptr<HybridCpuIseManagedGuestExecutionResultV1> HybridCpuIseManagedGuestExecutionRunnerV1::Failure(HybridCpuIseManagedGuestExecutionStatusV1 status, const std::wstring &reason)
	{
		return std::make_shared<HybridCpuIseManagedGuestExecutionResultV1>(status, reason, 0, std::nullopt, 0);
	}
}
