#include "../../Pipeline/MicroOps/Lane7Accelerator/SystemDeviceCommandMicroOp.h"
#include "ExternalAcceleratorRuntime.h"
#include "Auth/AcceleratorOwnerDomainGuard.h"
#include "Tokens/AcceleratorTokenStatusWord.h"
#include "Tokens/AcceleratorRegisterAbi.h"
#include "Tokens/AcceleratorTokenResults.h"
#include "Tokens/AcceleratorToken.h"
#include "Backends/ReferenceExternalAcceleratorBackend.h"

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

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public ExternalAcceleratorRuntime(Processor.MainMemoryArea mainMemory, IExternalAcceleratorBackend? backend = null, System.Nullable<AcceleratorTelemetry> telemetry = null)
	ExternalAcceleratorRuntime::ExternalAcceleratorRuntime(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<IExternalAcceleratorBackend> backend, std::shared_ptr<AcceleratorTelemetry> telemetry) : _mainMemory((mainMemory != nullptr) ? mainMemory : throw std::invalid_argument("mainMemory")), _telemetry((telemetry != nullptr) ? telemetry : std::make_shared<AcceleratorTelemetry>()), _capabilities(std::make_shared<AcceleratorCapabilityRegistry>(_telemetry)), _tokens(std::make_shared<AcceleratorTokenStore>(_telemetry)), _queue(std::make_shared<AcceleratorCommandQueue>(16, std::make_shared<AcceleratorDevice>(ReferenceAcceleratorId), _telemetry)), _backend((backend != nullptr) ? backend : std::make_shared<ReferenceExternalAcceleratorBackend>(nullptr, _telemetry, std::nullopt)), _memoryPortal(std::make_shared<MainMemoryReadOnlyAcceleratorMemoryPortal>(_mainMemory)), _stagingBuffer(std::make_shared<AcceleratorStagingBuffer>()), _fenceCoordinator(std::make_shared<AcceleratorFenceCoordinator>()), _commitCoordinator(std::make_shared<AcceleratorCommitCoordinator>(_telemetry)), _conflictManager(std::make_shared<ExternalAcceleratorConflictManager>(_telemetry))
	{
		_capabilities->RegisterProvider(std::make_shared<MatMulCapabilityProvider>());
	}

	std::shared_ptr<AcceleratorTokenStore> ExternalAcceleratorRuntime::getTokens() const
	{
		return _tokens;
	}

	std::shared_ptr<AcceleratorCapabilityRegistry> ExternalAcceleratorRuntime::getCapabilities() const
	{
		return _capabilities;
	}

	std::shared_ptr<AcceleratorStagingBuffer> ExternalAcceleratorRuntime::getStagingBuffer() const
	{
		return _stagingBuffer;
	}

	std::shared_ptr<ExternalAcceleratorConflictManager> ExternalAcceleratorRuntime::getConflictManager() const
	{
		return _conflictManager;
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::QueryCaps(std::shared_ptr<AcceleratorOwnerBinding> ownerBinding, std::shared_ptr<AcceleratorGuardEvidence> guardEvidence)
	{
		ArgumentNullException::ThrowIfNull(ownerBinding);
		ArgumentNullException::ThrowIfNull(guardEvidence);

		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> currentGuardEvidence = guardEvidence;
		std::shared_ptr<AcceleratorGuardDecision> descriptorGuard = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeDescriptorAcceptance(ownerBinding, currentGuardEvidence);
		std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capability = _capabilities->AcceptCapability(ReferenceAcceleratorId, ownerBinding, descriptorGuard);

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> temp_var = std::make_shared<ExternalAcceleratorRuntimeCommandResult>();
		temp_var->CommandKind = SystemDeviceCommandKind::QueryCaps;
		temp_var->RegisterAbi = AcceleratorRegisterAbi::FromCapabilityQuery(capability);
		temp_var->CapabilityAcceptance = capability;
		return temp_var;
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::Submit(std::shared_ptr<AcceleratorCommandDescriptor> descriptor)
	{
		ArgumentNullException::ThrowIfNull(descriptor);

		ArgumentNullException::ThrowIfNull(descriptor->OwnerGuardDecision);
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> evidence = descriptor->OwnerGuardDecision->Evidence;
		std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capability = _capabilities->AcceptCapability(ReferenceAcceleratorId, descriptor->OwnerBinding, descriptor->OwnerGuardDecision);
		std::shared_ptr<AcceleratorTokenAdmissionResult> admission = _tokens->Create(descriptor, capability, evidence, AcceleratorTokenAdmissionRejectPolicy::NonTrappingReject, _conflictManager);

		std::shared_ptr<AcceleratorBackendResult> backendSubmit;
		std::shared_ptr<AcceleratorBackendResult> backendTick;
		if (admission->IsAccepted && admission->Token != nullptr)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: _guardEvidenceByHandle[admission.Handle.Value] = evidence!;
			_guardEvidenceByHandle[admission->getHandle()->Value] = evidence.value();
			std::shared_ptr<AcceleratorQueueAdmissionRequest> request = std::make_shared<AcceleratorQueueAdmissionRequest>();
			request->Descriptor = descriptor;
			request->CapabilityAcceptance = capability;
			request->TokenAdmission = admission;
			request->ConflictAccepted = true;
			request->ConflictEvidenceMessage = L"Phase 08 runtime-owned submit accepted conflict reservation through ExternalAcceleratorConflictManager.";
			backendSubmit = _backend->TrySubmit(request, _queue, evidence);
			if (backendSubmit->IsAccepted)
			{
				backendTick = _backend->Tick(_queue, _memoryPortal, _stagingBuffer, evidence);
			}
		}

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> temp_var = std::make_shared<ExternalAcceleratorRuntimeCommandResult>();
		temp_var->CommandKind = SystemDeviceCommandKind::Submit;
		temp_var->RegisterAbi = AcceleratorRegisterAbi::FromSubmitAdmission(admission);
		temp_var->CapabilityAcceptance = capability;
		temp_var->SubmitAdmission = admission;
		temp_var->BackendSubmitResult = backendSubmit;
		temp_var->BackendTickResult = backendTick;
		return temp_var;
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::Poll(std::shared_ptr<AcceleratorTokenHandle> handle)
	{
		std::shared_ptr<AcceleratorBackendResult> backendTick;
		if (std::dynamic_pointer_cast<IExternalAcceleratorBackendPollsExternalOperation>(_backend) != nullptr)
		{
			std::optional<std::shared_ptr<AcceleratorGuardEvidence>> backendEvidence = ResolveGuardEvidence(handle);
			backendTick = _backend->Tick(_queue, _memoryPortal, _stagingBuffer, backendEvidence);
		}
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> lookupEvidence = ResolveGuardEvidence(handle);
		std::shared_ptr<AcceleratorTokenLookupResult> lookup = _tokens->Poll(handle, lookupEvidence);
		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> result = FromLookup(SystemDeviceCommandKind::Poll, lookup);
		result->BackendTickResult = backendTick;
		return result;
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::Status(std::shared_ptr<AcceleratorTokenHandle> handle)
	{
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> currentGuardEvidence = ResolveGuardEvidence(handle);
		std::shared_ptr<AcceleratorTokenLookupResult> lookup = _tokens->Status(handle, currentGuardEvidence);
		return FromLookup(SystemDeviceCommandKind::Status, lookup);
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::Wait(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorWaitPolicy>> waitPolicy)
	{
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> currentGuardEvidence = ResolveGuardEvidence(handle);
		std::shared_ptr<AcceleratorTokenLookupResult> lookup = waitPolicy && *waitPolicy
			? _tokens->TryWait(handle, currentGuardEvidence, *waitPolicy)
			: _tokens->Wait(handle, currentGuardEvidence);
		return FromLookup(SystemDeviceCommandKind::Wait, lookup);
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::Cancel(std::shared_ptr<AcceleratorTokenHandle> handle)
	{
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> backendEvidence = ResolveGuardEvidence(handle);
		std::shared_ptr<AcceleratorBackendResult> backendCancel = _backend->TryCancel(_tokens, handle, backendEvidence);
		std::shared_ptr<AcceleratorTokenLookupResult> lookup;
		if (backendCancel->TokenLookupResult != nullptr)
		{
			lookup = backendCancel->TokenLookupResult;
		}
		else
		{
			std::optional<std::shared_ptr<AcceleratorGuardEvidence>> lookupEvidence = ResolveGuardEvidence(handle);
			lookup = _tokens->TryLookup(handle, lookupEvidence, AcceleratorTokenLookupIntent::Cancel);
		}
		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> temp_var = std::make_shared<ExternalAcceleratorRuntimeCommandResult>();
		temp_var->CommandKind = SystemDeviceCommandKind::Cancel;
		temp_var->RegisterAbi = AcceleratorRegisterAbi::FromStatusLookup(lookup);
		temp_var->TokenLookup = lookup;
		temp_var->BackendSubmitResult = backendCancel;
		return temp_var;
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::FenceObserve(std::shared_ptr<AcceleratorTokenHandle> handle)
	{
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> currentGuardEvidence = ResolveGuardEvidence(handle);
		std::shared_ptr<AcceleratorFenceResult> fence = _fenceCoordinator->TryFence(_tokens, AcceleratorFenceScope::ForToken(handle, false, true, AcceleratorFenceActiveTokenDisposition::Reject, std::nullopt), currentGuardEvidence, nullptr, nullptr, nullptr, nullptr, _conflictManager);
		std::optional<std::shared_ptr<AcceleratorCommitResult>> commitResult;
		return FromFence(fence, commitResult);
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::FenceCommit(std::shared_ptr<AcceleratorTokenHandle> handle)
	{
		std::shared_ptr<IExternalAcceleratorPublicationEvidenceProvider> evidenceProvider = std::dynamic_pointer_cast<IExternalAcceleratorPublicationEvidenceProvider>(_backend);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: System.Func<AcceleratorToken, System.Nullable<ExternalOperationPublicationEvidence>>? externalPublicationEvidenceFactory = _backend is IExternalAcceleratorPublicationEvidenceProvider evidenceProvider ? evidenceProvider.GetPublicationEvidence : null;
		std::function<std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>>(std::shared_ptr<AcceleratorToken>)> externalPublicationEvidenceFactory;
		if (evidenceProvider != nullptr)
		{
			externalPublicationEvidenceFactory = [evidenceProvider](std::shared_ptr<AcceleratorToken> token)
			{
				return evidenceProvider->GetPublicationEvidence(std::move(token));
			};
		}
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> currentGuardEvidence = ResolveGuardEvidence(handle);
		std::shared_ptr<AcceleratorFenceResult> fence = _fenceCoordinator->TryFence(_tokens, AcceleratorFenceScope::ForToken(handle, true, true, AcceleratorFenceActiveTokenDisposition::Reject, std::nullopt), currentGuardEvidence, _stagingBuffer, _mainMemory, _commitCoordinator, AcceleratorCommitInvalidationPlan::getNone(), _conflictManager, externalPublicationEvidenceFactory);

// C# TO C++ CONVERTER TASK: C++ has no equivalent to the C# index operator:
		std::optional<std::shared_ptr<AcceleratorCommitResult>> commitResult = fence->CommitResults.empty() ? std::nullopt : std::optional<std::shared_ptr<AcceleratorCommitResult>>(fence->CommitResults.back());
		return FromFence(fence, commitResult);
	}

	std::optional<std::shared_ptr<AcceleratorGuardEvidence>> ExternalAcceleratorRuntime::ResolveGuardEvidence(std::shared_ptr<AcceleratorTokenHandle> handle)
	{
		if (handle == nullptr || !handle->IsValid)
		{
			return std::nullopt;
		}

		const auto iterator = _guardEvidenceByHandle.find(handle->Value);
		if (iterator == _guardEvidenceByHandle.end() || iterator->second == nullptr)
		{
			return std::nullopt;
		}

		return iterator->second;
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::FromLookup(SystemDeviceCommandKind commandKind, std::shared_ptr<AcceleratorTokenLookupResult> lookup)
	{
		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> temp_var = std::make_shared<ExternalAcceleratorRuntimeCommandResult>();
		temp_var->CommandKind = commandKind;
		temp_var->RegisterAbi = AcceleratorRegisterAbi::FromStatusLookup(lookup);
		temp_var->TokenLookup = lookup;
		return temp_var;
	}

	std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> ExternalAcceleratorRuntime::FromFence(std::shared_ptr<AcceleratorFenceResult> fence, std::optional<std::shared_ptr<AcceleratorCommitResult>> &commitResult)
	{
		std::shared_ptr<AcceleratorRegisterAbiResult> abi = fence->Succeeded ? AcceleratorRegisterAbiResult::Write(fence->PackedFenceStatus, L"ACCEL_FENCE rd receives packed fence status after guarded scoped-token serialization and optional commit.") : AcceleratorRegisterAbiResult::NoWriteRejected(fence->FaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::FenceRejected : fence->FaultCode, L"ACCEL_FENCE rd is written only after the scoped token set serializes without rejection.");

		std::shared_ptr<ExternalAcceleratorRuntimeCommandResult> temp_var = std::make_shared<ExternalAcceleratorRuntimeCommandResult>();
		temp_var->CommandKind = SystemDeviceCommandKind::Fence;
		temp_var->RegisterAbi = abi;
		temp_var->FenceResult = fence;
// C# TO C++ CONVERTER TASK: C++ has no equivalent to the C# index operator:
		temp_var->TokenLookup = fence->TokenResults.empty() ? nullptr : fence->TokenResults.back();
		temp_var->CommitResult = commitResult ? *commitResult : nullptr;
		return temp_var;
	}
}


