#pragma once

#include "Capabilities/MatMulCapabilityProvider.h"
#include "../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "Telemetry/AcceleratorTelemetry.h"
#include "Capabilities/AcceleratorCapabilityRegistry.h"
#include "Tokens/AcceleratorTokenStore.h"
#include "Queues/AcceleratorCommandQueue.h"
#include "Backends/ExternalAcceleratorBackends.h"
#include "Memory/AcceleratorMemoryModel.h"
#include "Fences/AcceleratorFenceModel.h"
#include "Commit/AcceleratorCommitModel.h"
#include "Conflicts/ExternalAcceleratorConflictManager.h"
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <functional>
#include <optional>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core { enum class SystemDeviceCommandKind : unsigned char; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens { class AcceleratorRegisterAbiResult; }
using namespace YAKSys_Hybrid_CPU::Core;
using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Commit;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Fences;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators
{
	class ExternalAcceleratorRuntimeCommandResult final {
	public:
		YAKSys_Hybrid_CPU::Core::SystemDeviceCommandKind CommandKind;
		std::shared_ptr<AcceleratorRegisterAbiResult> RegisterAbi;
		std::shared_ptr<AcceleratorCapabilityAcceptanceResult> CapabilityAcceptance;
		std::shared_ptr<AcceleratorTokenAdmissionResult> SubmitAdmission;
		std::shared_ptr<AcceleratorTokenLookupResult> TokenLookup;
		std::shared_ptr<AcceleratorFenceResult> FenceResult;
		std::shared_ptr<AcceleratorBackendResult> BackendSubmitResult, BackendTickResult;
		std::shared_ptr<AcceleratorCommitResult> CommitResult;
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record ExternalAcceleratorRuntimeCommandResult
//	{
//		public required SystemDeviceCommandKind CommandKind
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorRegisterAbiResult RegisterAbi
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorCapabilityAcceptanceResult> CapabilityAcceptance
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorTokenAdmissionResult> SubmitAdmission
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorTokenLookupResult> TokenLookup
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorFenceResult> FenceResult
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorBackendResult> BackendSubmitResult
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorBackendResult> BackendTickResult
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<AcceleratorCommitResult> CommitResult
//		{
//			get;
//			set;
//		}
//	}

	/// <summary>
	/// Per-core runtime owner for the Phase 08 current L7-SDC executable contour.
	/// Token handles remain evidence only; the guard-plane evidence is retained here
	/// and revalidated on each command before status, cancel, fence, or commit.
	/// </summary>
	class ExternalAcceleratorRuntime final : public std::enable_shared_from_this<ExternalAcceleratorRuntime>
	{
	private:
		static inline const std::wstring ReferenceAcceleratorId = MatMulCapabilityProvider::AcceleratorId;

		const std::shared_ptr<Processor::MainMemoryArea> _mainMemory;
		const std::shared_ptr<AcceleratorTelemetry> _telemetry;
		const std::shared_ptr<AcceleratorCapabilityRegistry> _capabilities;
		const std::shared_ptr<AcceleratorTokenStore> _tokens;
		const std::shared_ptr<AcceleratorCommandQueue> _queue;
		const std::shared_ptr<IExternalAcceleratorBackend> _backend;
		const std::shared_ptr<MainMemoryReadOnlyAcceleratorMemoryPortal> _memoryPortal;
		const std::shared_ptr<AcceleratorStagingBuffer> _stagingBuffer;
		const std::shared_ptr<AcceleratorFenceCoordinator> _fenceCoordinator;
		const std::shared_ptr<AcceleratorCommitCoordinator> _commitCoordinator;
		const std::shared_ptr<ExternalAcceleratorConflictManager> _conflictManager;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<ulong, AcceleratorGuardEvidence> _guardEvidenceByHandle = new();
		std::unordered_map<unsigned long long, std::shared_ptr<AcceleratorGuardEvidence>> _guardEvidenceByHandle = std::unordered_map<unsigned long long, std::shared_ptr<AcceleratorGuardEvidence>>();

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public ExternalAcceleratorRuntime(Processor.MainMemoryArea mainMemory, IExternalAcceleratorBackend? backend = null, System.Nullable<AcceleratorTelemetry> telemetry = null)
		ExternalAcceleratorRuntime(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<IExternalAcceleratorBackend> backend = nullptr, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

		std::shared_ptr<AcceleratorTokenStore> getTokens() const;

		std::shared_ptr<AcceleratorCapabilityRegistry> getCapabilities() const;

		std::shared_ptr<AcceleratorStagingBuffer> getStagingBuffer() const;

		std::shared_ptr<ExternalAcceleratorConflictManager> getConflictManager() const;

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> QueryCaps(std::shared_ptr<AcceleratorOwnerBinding> ownerBinding, std::shared_ptr<AcceleratorGuardEvidence> guardEvidence);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> Submit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> Poll(std::shared_ptr<AcceleratorTokenHandle> handle);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> Status(std::shared_ptr<AcceleratorTokenHandle> handle);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> Wait(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorWaitPolicy>> waitPolicy = std::nullopt);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> Cancel(std::shared_ptr<AcceleratorTokenHandle> handle);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> FenceObserve(std::shared_ptr<AcceleratorTokenHandle> handle);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> FenceCommit(std::shared_ptr<AcceleratorTokenHandle> handle);

	private:
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> ResolveGuardEvidence(std::shared_ptr<AcceleratorTokenHandle> handle);

		static std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> FromLookup(YAKSys_Hybrid_CPU::Core::SystemDeviceCommandKind commandKind, std::shared_ptr<AcceleratorTokenLookupResult> lookup);

		static std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> FromFence(std::shared_ptr<AcceleratorFenceResult> fence, std::optional<std::shared_ptr<AcceleratorCommitResult>> &commitResult);
	};

}




