#include "ProviderNeutralExternalAcceleratorBackend.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"
#include <algorithm>
#include <array>
#include <cstdint>

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Queues;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Backends
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public ProviderNeutralExternalAcceleratorBackend(IExternalOperationProvider provider, IAcceleratorBackendClock? clock = null)
	ProviderNeutralExternalAcceleratorBackend::ProviderNeutralExternalAcceleratorBackend(std::shared_ptr<IExternalOperationProvider> provider, std::shared_ptr<IAcceleratorBackendClock> clock) : _provider((provider != nullptr) ? provider : throw std::invalid_argument("provider")), _clock((clock != nullptr) ? clock : std::make_shared<ManualAcceleratorBackendClock>())
	{
	}

	std::shared_ptr<AcceleratorBackendResult> ProviderNeutralExternalAcceleratorBackend::TrySubmit(std::shared_ptr<AcceleratorQueueAdmissionRequest> request, std::shared_ptr<IAcceleratorCommandQueue> queue, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(request);
		ArgumentNullException::ThrowIfNull(queue);
		unsigned long long tick = _clock->Advance();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: AcceleratorToken? token = request.TokenAdmission.Token;
		const auto tokenAdmission = request->TokenAdmission;
		std::shared_ptr<AcceleratorToken> token = tokenAdmission ? tokenAdmission->Token : nullptr;
		if (!tokenAdmission || !tokenAdmission->IsAccepted || token == nullptr || !currentGuardEvidence || !*currentGuardEvidence)
		{
			return AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::SubmitAdmissionRejected, L"External-operation bridge requires an accepted L7 token and fresh CPU guard evidence.", tick, token);
		}

		std::shared_ptr<ExternalOperationSemanticRequest> semantic = CreateSemanticRequest(request->Descriptor);
		std::shared_ptr<ExternalOperationProviderPollResult> admission = _provider->Admit(semantic);
		if (!admission)
		{
			return RejectAndFault(token, currentGuardEvidence, tick, L"External-operation provider returned no admission result.");
		}
		std::shared_ptr<ExternalOperationAdmissionReceipt> providerAdmission = std::dynamic_pointer_cast<ExternalOperationAdmissionReceipt>(admission->Receipt);
		if (admission->Status != ExternalOperationProviderPollStatus::Receipt || providerAdmission == nullptr || !MatchesSemantic(providerAdmission->Request, semantic))
		{
			return RejectAndFault(token, currentGuardEvidence, tick, L"External-operation provider did not return an exact successful admission receipt.");
		}

		std::shared_ptr<ExternalOperationBindingDecision> binding = ExternalOperationAdmissionBinding::Evaluate(providerAdmission->Request, admission->CurrentGenerations, std::make_shared<ExternalOperationCpuGuardReceipt>(providerAdmission->Request, ExternalCpuGuardStatus::Allowed), providerAdmission);
		if (!binding->IsSubmitEligible)
		{
			return RejectAndFault(token, currentGuardEvidence, tick, L"External-operation admission binding rejected or became stale.");
		}

		std::shared_ptr<AcceleratorQueueAdmissionResult> queued = queue->TryEnqueue(request, currentGuardEvidence);
		if (!queued->IsAccepted || queued->Command == nullptr)
		{
			return AcceleratorBackendResult::Rejected(queued->FaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::QueueAdmissionRejected : queued->FaultCode, queued->Message, tick, token, queued);
		}
		std::optional<std::shared_ptr<AcceleratorQueuedCommand>> command;
		std::shared_ptr<AcceleratorQueueAdmissionResult> dequeue;
		if (!queue->TryDequeueReady(currentGuardEvidence, command, dequeue) || !command || !*command)
		{
			return AcceleratorBackendResult::Rejected(dequeue->FaultCode, dequeue->Message, tick, token, queued);
		}

		std::shared_ptr<ExternalOperationProviderPollResult> submitted = _provider->Submit(providerAdmission->Request);
		ExternalOperationStage stage = Accept(ExternalOperationStage::Admitted, providerAdmission->Request, submitted);
		if (stage != ExternalOperationStage::Submitted)
		{
			return RejectAndFault(token, currentGuardEvidence, tick, L"External-operation provider submit acknowledgement was missing, stale, or invalid.");
		}

		std::shared_ptr<AcceleratorTokenTransition> running = token->MarkRunning(*currentGuardEvidence);
		if (running->Rejected)
		{
			return AcceleratorBackendResult::Rejected(running->FaultCode, running->Message, tick, token, queued);
		}

		_operations.emplace(token->getHandle()->Value, std::make_shared<OperationState>(token, request->Descriptor, providerAdmission->Request, ExternalOperationStage::Submitted));
		return AcceleratorBackendResult::Submitted(queued, tick);
	}

	std::shared_ptr<AcceleratorBackendResult> ProviderNeutralExternalAcceleratorBackend::Tick(std::shared_ptr<IAcceleratorCommandQueue> queue, std::shared_ptr<IAcceleratorMemoryPortal> memoryPortal, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(queue);
		ArgumentNullException::ThrowIfNull(memoryPortal);
		ArgumentNullException::ThrowIfNull(stagingBuffer);
		unsigned long long tick = _clock->Advance();
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: OperationState? state = _operations.Values.FirstOrDefault(static item => !item.Token.IsTerminal && !item.Visible);
		std::shared_ptr<OperationState> state;
		for (const auto &entry : _operations)
		{
			if (entry.second && !entry.second->getToken()->isTerminal() && !entry.second->getVisible())
			{
				state = entry.second;
				break;
			}
		}
		if (state == nullptr)
		{
			return AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::BackendRejected, L"External-operation bridge has no pending L7 operation to poll.", tick);
		}
		if (!currentGuardEvidence || !*currentGuardEvidence)
		{
			return RejectAndFault(state->getToken(), currentGuardEvidence, tick, L"External-operation bridge lost CPU guard evidence before provider poll.");
		}

		std::shared_ptr<ExternalOperationProviderPollResult> poll = _provider->Poll(state->getRequest());
		if (!poll || !state->getRequest()->Generations->Equals(poll->CurrentGenerations) ||
			poll->Status == ExternalOperationProviderPollStatus::Stale ||
			poll->Status == ExternalOperationProviderPollStatus::Faulted ||
			poll->Status == ExternalOperationProviderPollStatus::Unavailable)
		{
			return RejectAndFault(state->getToken(), currentGuardEvidence, tick, L"External-operation provider lifecycle became stale, faulted, or unavailable.");
		}
		if (poll->Status == ExternalOperationProviderPollStatus::Pending)
		{
			return AcceleratorBackendResult::Pending(state->getToken(), tick, L"External-operation provider reports pending; DeviceComplete and visibility remain unproven.");
		}

		ExternalOperationStage next = Accept(state->getStage(), state->getRequest(), poll);
		if (next == ExternalOperationStage::Failed || next == ExternalOperationStage::Stale || next == state->getStage())
		{
			return RejectAndFault(state->getToken(), currentGuardEvidence, tick, L"External-operation provider receipt was duplicate, late, cross-operation, or out of sequence.");
		}
		state->setStage(next);
		std::shared_ptr<ExternalOperationCompletionReceipt> completion = std::dynamic_pointer_cast<ExternalOperationCompletionReceipt>(poll->Receipt);
		if (completion != nullptr)
		{
			state->setCompletion(completion);
		}
		std::shared_ptr<ExternalOperationProgressReceipt> visibility = std::dynamic_pointer_cast<ExternalOperationProgressReceipt>(poll->Receipt);
		if (visibility != nullptr && visibility->Stage == ExternalOperationStage::Visible)
		{
			state->setVisibility(visibility);
		}
		if (next == ExternalOperationStage::DeviceComplete)
		{
			return AcceleratorBackendResult::Pending(state->getToken(), tick, L"External-operation DeviceComplete was observed; visibility and architectural publication remain unproven.");
		}
		if (next != ExternalOperationStage::Visible)
		{
			return RejectAndFault(state->getToken(), currentGuardEvidence, tick, L"External-operation bridge accepts only completion then visibility before L7 commit.");
		}

		std::shared_ptr<AcceleratorTokenTransition> complete = state->getToken()->MarkDeviceComplete(*currentGuardEvidence);
		if (complete->Rejected)
		{
			return AcceleratorBackendResult::Rejected(complete->FaultCode, complete->Message, tick, state->getToken());
		}
		state->setVisible(true);
		return AcceleratorBackendResult::DeviceCompleted(state->getToken(), 0, 0, 0, tick, L"External-operation visibility receipt was validated; L7 token is DeviceComplete and remains unpublished.");
	}

	std::shared_ptr<AcceleratorBackendResult> ProviderNeutralExternalAcceleratorBackend::TryCancel(std::shared_ptr<AcceleratorTokenStore> tokenStore, std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(tokenStore);
		unsigned long long tick = _clock->Advance();
		ArgumentNullException::ThrowIfNull(handle);
		std::shared_ptr<OperationState> state;
		auto operationsIterator = _operations.find(handle->Value);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!_operations.TryGetValue(handle.Value, out OperationState? state) || currentGuardEvidence == null)
		if (operationsIterator == _operations.end() || !operationsIterator->second || !currentGuardEvidence || !*currentGuardEvidence)
		{
			return AcceleratorBackendResult::Rejected(AcceleratorTokenFaultCode::CancelRejected, L"External-operation bridge cannot prove a current cancellable provider operation.", tick);
		}
		state = operationsIterator->second;
		std::shared_ptr<ExternalOperationProviderPollResult> cancel = _provider->Cancel(state->getRequest());
		if (!cancel || cancel->Status != ExternalOperationProviderPollStatus::Receipt || !state->getRequest()->Generations->Equals(cancel->CurrentGenerations))
		{
			return RejectAndFault(state->getToken(), currentGuardEvidence, tick, L"External-operation cancellation is ambiguous or stale and cannot prove release.");
		}
		std::shared_ptr<AcceleratorTokenLookupResult> lookup = tokenStore->Cancel(handle, currentGuardEvidence);
		return lookup->IsAllowed ? AcceleratorBackendResult::Canceled(lookup, tick) : AcceleratorBackendResult::Rejected(lookup->FaultCode, lookup->Message, tick, state->getToken(), nullptr, lookup);
	}

	std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>> ProviderNeutralExternalAcceleratorBackend::GetPublicationEvidence(std::shared_ptr<AcceleratorToken> token)
	{
		ArgumentNullException::ThrowIfNull(token);
		std::shared_ptr<OperationState> state;
		auto operationsIterator = _operations.find(token->getHandle()->Value);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!_operations.TryGetValue(token.Handle.Value, out OperationState? state) || !ReferenceEquals(state.Token, token) || !state.Visible || state.Completion == null || state.Visibility == null)
		if (operationsIterator == _operations.end() || !operationsIterator->second || operationsIterator->second->getToken() != token || !operationsIterator->second->getVisible() || !operationsIterator->second->getCompletion() || !operationsIterator->second->getCompletion().value() || !operationsIterator->second->getVisibility() || !operationsIterator->second->getVisibility().value())
		{
			return std::nullopt;
		}
		state = operationsIterator->second;

		// This is deliberately a provider call outside any token-store lock.
		// A pending response is the provider's explicit current-state answer;
		// any new receipt, failure, unavailable result, or generation drift is
		// ambiguous at publication time and therefore cannot be used.
		std::shared_ptr<ExternalOperationProviderPollResult> revalidation = _provider->Poll(state->getRequest());
		if (!revalidation || revalidation->Status != ExternalOperationProviderPollStatus::Pending || !state->getRequest()->Generations->Equals(revalidation->CurrentGenerations))
		{
			return std::nullopt;
		}

		// The CPU still applies the pure gate before its staged publication.
		return std::make_shared<ExternalOperationPublicationEvidence>(state->getRequest(), state->getRequest()->Generations, state->getCompletion().value(), state->getVisibility().value());
	}

	std::shared_ptr<ExternalOperationSemanticRequest> ProviderNeutralExternalAcceleratorBackend::CreateSemanticRequest(std::shared_ptr<AcceleratorCommandDescriptor> descriptor)
	{
		Guid correlation = CorrelationFrom(std::make_shared<AcceleratorDescriptorIdentity>(descriptor->Identity));
		return std::make_shared<ExternalOperationSemanticRequest>(ExternalOperationContract::Version, ExternalRequestCorrelation(correlation), ExternalEffectClass::NonIdempotent, ExternalVisibilityRequirement::StagedOutput, ExternalCancellationMode::ExactAcknowledgement, ExternalReplayEffectClass::StagedReversibleUntilPublish);
	}

	Guid ProviderNeutralExternalAcceleratorBackend::CorrelationFrom(std::shared_ptr<AcceleratorDescriptorIdentity> identity)
	{
		ArgumentNullException::ThrowIfNull(identity);
		std::array<unsigned char, 16> bytes{};
		const auto writeLittleEndian = [&bytes](std::size_t offset, std::uint64_t value)
		{
			for (std::size_t index = 0; index < sizeof(value); ++index)
				bytes[offset + index] = static_cast<unsigned char>((value >> (index * 8)) & 0xffu);
		};
		writeLittleEndian(0, static_cast<std::uint64_t>(identity->DescriptorIdentityHash));
		writeLittleEndian(8, static_cast<std::uint64_t>(identity->NormalizedFootprintHash));
		return Guid(bytes);
	}

	bool ProviderNeutralExternalAcceleratorBackend::MatchesSemantic(std::shared_ptr<ExternalOperationRequest> request, std::shared_ptr<ExternalOperationSemanticRequest> semantic)
	{
		return request->ContractVersion == semantic->ContractVersion && request->Correlation == semantic->Correlation && request->EffectClass == semantic->EffectClass && request->VisibilityRequirement == semantic->VisibilityRequirement && request->CancellationMode == semantic->CancellationMode;
	}

	ExternalOperationStage ProviderNeutralExternalAcceleratorBackend::Accept(ExternalOperationStage current, std::shared_ptr<ExternalOperationRequest> request, std::shared_ptr<ExternalOperationProviderPollResult> result)
	{
		return result && result->Status == ExternalOperationProviderPollStatus::Receipt ? ExternalOperationContract::Accept(current, request, result->CurrentGenerations, result->Receipt) : ExternalOperationStage::Failed;
	}

	std::shared_ptr<AcceleratorBackendResult> ProviderNeutralExternalAcceleratorBackend::RejectAndFault(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &evidence, unsigned long long tick, const std::wstring &message)
	{
		if (token && evidence && *evidence)
		{
			token->MarkFaulted(AcceleratorTokenFaultCode::BackendRejected, *evidence);
		}
		return AcceleratorBackendResult::Faulted(token, AcceleratorTokenFaultCode::BackendRejected, message, tick);
	}

	std::shared_ptr<AcceleratorToken> ProviderNeutralExternalAcceleratorBackend::OperationState::getToken() const
	{
		return Token;
	}

	std::shared_ptr<AcceleratorCommandDescriptor> ProviderNeutralExternalAcceleratorBackend::OperationState::getDescriptor() const
	{
		return Descriptor;
	}

	std::shared_ptr<ExternalOperationRequest> ProviderNeutralExternalAcceleratorBackend::OperationState::getRequest() const
	{
		return Request;
	}

	ExternalOperationStage ProviderNeutralExternalAcceleratorBackend::OperationState::getStage() const
	{
		return Stage;
	}

	void ProviderNeutralExternalAcceleratorBackend::OperationState::setStage(ExternalOperationStage value)
	{
		Stage = value;
	}

	bool ProviderNeutralExternalAcceleratorBackend::OperationState::getVisible() const
	{
		return Visible;
	}

	void ProviderNeutralExternalAcceleratorBackend::OperationState::setVisible(bool value)
	{
		Visible = value;
	}

	std::optional<std::shared_ptr<ExternalOperationCompletionReceipt>> ProviderNeutralExternalAcceleratorBackend::OperationState::getCompletion() const
	{
		return Completion;
	}

	void ProviderNeutralExternalAcceleratorBackend::OperationState::setCompletion(const std::optional<std::shared_ptr<ExternalOperationCompletionReceipt>> &value)
	{
		Completion = value;
	}

	std::optional<std::shared_ptr<ExternalOperationProgressReceipt>> ProviderNeutralExternalAcceleratorBackend::OperationState::getVisibility() const
	{
		return Visibility;
	}

	void ProviderNeutralExternalAcceleratorBackend::OperationState::setVisibility(const std::optional<std::shared_ptr<ExternalOperationProgressReceipt>> &value)
	{
		Visibility = value;
	}

	ProviderNeutralExternalAcceleratorBackend::OperationState::OperationState(
		std::shared_ptr<AcceleratorToken> token,
		std::shared_ptr<AcceleratorCommandDescriptor> descriptor,
		std::shared_ptr<ExternalOperationRequest> request,
		ExternalOperationStage stage)
		: Token(std::move(token)), Descriptor(std::move(descriptor)), Request(std::move(request)), Stage(std::move(stage))
	{
		ArgumentNullException::ThrowIfNull(Token);
		ArgumentNullException::ThrowIfNull(Descriptor);
		ArgumentNullException::ThrowIfNull(Request);
	}
}

