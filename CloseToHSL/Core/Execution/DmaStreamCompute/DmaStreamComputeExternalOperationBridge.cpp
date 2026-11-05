#include "DmaStreamComputeExternalOperationBridge.h"
#include "DmaStreamComputeDescriptor.h"
#include <array>

using namespace HybridCPU::ExternalRuntime::Contracts;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public DmaStreamComputeExternalOperationBridge(IExternalOperationProvider provider)
	DmaStreamComputeExternalOperationBridge::DmaStreamComputeExternalOperationBridge(std::shared_ptr<IExternalOperationProvider> provider) : _provider((provider != nullptr) ? provider : throw std::invalid_argument("provider"))
	{
	}

	std::shared_ptr<DmaStreamComputeExternalOperationResult> DmaStreamComputeExternalOperationBridge::Begin(std::shared_ptr<DmaStreamComputeToken> token)
	{
		ArgumentNullException::ThrowIfNull(token);
		if (token->getState() != DmaStreamComputeTokenState::Admitted || !token->getDescriptor()->OwnerGuardDecision->IsAllowed)
		{
			return Reject(token, DmaStreamComputeExternalOperationStatus::Rejected, L"DSC external operation requires an admitted token with a current CPU owner/domain guard.");
		}
		if (_operations.contains(token->getTokenId()))
		{
			return Reject(token, DmaStreamComputeExternalOperationStatus::Rejected, L"DSC external operation token already has a provider lifecycle; duplicate submit is forbidden.");
		}

		std::shared_ptr<ExternalOperationSemanticRequest> semantic = CreateSemantic(token->getDescriptor());
		std::shared_ptr<ExternalOperationProviderPollResult> admission = _provider->Admit(semantic);
		std::shared_ptr<ExternalOperationAdmissionReceipt> providerAdmission = std::dynamic_pointer_cast<ExternalOperationAdmissionReceipt>(admission->Receipt);
		if (admission->Status != ExternalOperationProviderPollStatus::Receipt || providerAdmission == nullptr || !Matches(semantic, providerAdmission->Request))
		{
			return Fault(token, DmaStreamComputeExternalOperationStatus::Faulted, L"DSC provider admission was missing, invalid, or for a different semantic request.");
		}

		std::shared_ptr<ExternalOperationBindingDecision> binding = ExternalOperationAdmissionBinding::Evaluate(providerAdmission->Request, admission->CurrentGenerations, std::make_shared<ExternalOperationCpuGuardReceipt>(providerAdmission->Request, ExternalCpuGuardStatus::Allowed), providerAdmission);
		if (!binding->IsSubmitEligible)
		{
			return Reject(token, binding->Status == ExternalOperationBindingStatus::Stale ? DmaStreamComputeExternalOperationStatus::Stale : DmaStreamComputeExternalOperationStatus::Rejected, L"DSC provider admission binding rejected or invalidated the exact token operation.");
		}

		std::shared_ptr<ExternalOperationProviderPollResult> submitted = _provider->Submit(providerAdmission->Request);
		ExternalOperationStage stage = Accept(ExternalOperationStage::Admitted, providerAdmission->Request, submitted);
		if (stage != ExternalOperationStage::Submitted)
		{
			return Fault(token, stage == ExternalOperationStage::Stale ? DmaStreamComputeExternalOperationStatus::Stale : DmaStreamComputeExternalOperationStatus::Faulted, L"DSC provider submit acknowledgement was missing, stale, or invalid.");
		}

		token->MarkIssued();
		_operations.emplace(token->getTokenId(), std::make_shared<ExternalOperationState>(token, providerAdmission->Request, stage));
		return Result(token, DmaStreamComputeExternalOperationStatus::Submitted, providerAdmission->Request, stage, L"DSC provider submission acknowledged; completion, visibility, retire publication, and release remain separate.");
	}

	std::shared_ptr<DmaStreamComputeExternalOperationResult> DmaStreamComputeExternalOperationBridge::Poll(unsigned long long tokenId)
	{
		auto operationsIterator = _operations.find(tokenId);
		if (operationsIterator == _operations.end())
			throw InvalidOperationException(L"DSC external operation is not known to this bridge.");
		const auto &state = operationsIterator->second;
		if (([&] { const auto codexPatternValue = state->getToken()->getState(); return codexPatternValue == DmaStreamComputeTokenState::Faulted || codexPatternValue == DmaStreamComputeTokenState::Canceled; }()))
		{
			return Result(state->getToken(), DmaStreamComputeExternalOperationStatus::Faulted, state->getRequest(), state->getStage(), L"DSC token is terminal; provider lifecycle cannot authorize reuse.");
		}
		if (!state->getToken()->getDescriptor()->OwnerGuardDecision->IsAllowed)
		{
			return Fault(state->getToken(), DmaStreamComputeExternalOperationStatus::Stale, L"DSC CPU owner/domain guard is no longer current before provider lifecycle reuse.");
		}

		std::shared_ptr<ExternalOperationProviderPollResult> observation = _provider->Poll(state->getRequest());
		if (!state->getRequest()->Generations->Equals(observation->CurrentGenerations) ||
			observation->Status == ExternalOperationProviderPollStatus::Stale ||
			observation->Status == ExternalOperationProviderPollStatus::Faulted ||
			observation->Status == ExternalOperationProviderPollStatus::Unavailable)
		{
			return Fault(state->getToken(), DmaStreamComputeExternalOperationStatus::Stale, L"DSC provider generation changed or provider outcome is unavailable/faulted; no publication is permitted.");
		}
		if (observation->Status == ExternalOperationProviderPollStatus::Pending)
		{
			return Result(state->getToken(), DmaStreamComputeExternalOperationStatus::Pending, state->getRequest(), state->getStage(), L"DSC provider reports Pending; completion and visibility remain unproven.");
		}

		ExternalOperationStage next = Accept(state->getStage(), state->getRequest(), observation);
		if (next == ExternalOperationStage::Failed || next == ExternalOperationStage::Stale || next == state->getStage())
		{
			return Fault(state->getToken(), next == ExternalOperationStage::Stale ? DmaStreamComputeExternalOperationStatus::Stale : DmaStreamComputeExternalOperationStatus::Faulted, L"DSC provider receipt was duplicate, late, cross-operation, or out of sequence.");
		}
		state->setStage(next);
		switch (next)
		{
		case ExternalOperationStage::DeviceComplete:
			return Result(state->getToken(), DmaStreamComputeExternalOperationStatus::DeviceComplete,
				state->getRequest(), state->getStage(), L"DSC device completion observed; output is not yet visible or publishable.");
		case ExternalOperationStage::Visible:
			return Result(state->getToken(), DmaStreamComputeExternalOperationStatus::Visible,
				state->getRequest(), state->getStage(), L"DSC visibility receipt observed; later staged-output integration must still revalidate before retire.");
		default:
			return Fault(state->getToken(), DmaStreamComputeExternalOperationStatus::Faulted,
				L"DSC bridge accepts only DeviceComplete then Visible after submission.");
		}
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return next switch
	//			{
	//				ExternalOperationStage.DeviceComplete => Result(state.Token, DmaStreamComputeExternalOperationStatus.DeviceComplete,
	//					state.Request, state.Stage, "DSC device completion observed; output is not yet visible or publishable."), ExternalOperationStage.Visible => Result(state.Token, DmaStreamComputeExternalOperationStatus.Visible,
	//					state.Request, state.Stage, "DSC visibility receipt observed; later staged-output integration must still revalidate before retire."),
	//				_ => Fault(state.Token, DmaStreamComputeExternalOperationStatus.Faulted,
	//					"DSC bridge accepts only DeviceComplete then Visible after submission.")
	//			};
	}

	std::shared_ptr<ExternalOperationSemanticRequest> DmaStreamComputeExternalOperationBridge::CreateSemantic(std::shared_ptr<DmaStreamComputeDescriptor> descriptor)
	{
		return std::make_shared<ExternalOperationSemanticRequest>(ExternalOperationContract::Version, ExternalRequestCorrelation(CorrelationFrom(descriptor->DescriptorIdentityHash, descriptor->NormalizedFootprintHash)), ExternalEffectClass::NonIdempotent, ExternalVisibilityRequirement::StagedOutput, ExternalCancellationMode::ExactAcknowledgement, ExternalReplayEffectClass::StagedReversibleUntilPublish);
	}

	Guid DmaStreamComputeExternalOperationBridge::CorrelationFrom(unsigned long long descriptorHash, unsigned long long footprintHash)
	{
		std::array<unsigned char, 16> bytes{};
		for (unsigned int i = 0; i < 8; ++i)
		{
			bytes[i] = static_cast<unsigned char>((descriptorHash >> (i * 8)) & 0xffu);
			bytes[i + 8] = static_cast<unsigned char>((footprintHash >> (i * 8)) & 0xffu);
		}
		return Guid(bytes);
	}

	bool DmaStreamComputeExternalOperationBridge::Matches(std::shared_ptr<ExternalOperationSemanticRequest> semantic, std::shared_ptr<ExternalOperationRequest> request)
	{
		return request->ContractVersion == semantic->ContractVersion && request->Correlation == semantic->Correlation && request->EffectClass == semantic->EffectClass && request->VisibilityRequirement == semantic->VisibilityRequirement && request->CancellationMode == semantic->CancellationMode;
	}

	ExternalOperationStage DmaStreamComputeExternalOperationBridge::Accept(ExternalOperationStage current, std::shared_ptr<ExternalOperationRequest> request, std::shared_ptr<ExternalOperationProviderPollResult> observation)
	{
		return observation->Status == ExternalOperationProviderPollStatus::Receipt ? ExternalOperationContract::Accept(current, request, observation->CurrentGenerations, observation->Receipt) : ExternalOperationStage::Failed;
	}

	std::shared_ptr<DmaStreamComputeExternalOperationResult> DmaStreamComputeExternalOperationBridge::Reject(std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeExternalOperationStatus status, const std::wstring &message)
	{
		return Result(token, status, std::nullopt, ExternalOperationStage::Prepared, message);
	}

	std::shared_ptr<DmaStreamComputeExternalOperationResult> DmaStreamComputeExternalOperationBridge::Fault(std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeExternalOperationStatus status, const std::wstring &message)
	{
		if (token->getState() != DmaStreamComputeTokenState::Faulted && token->getState() != DmaStreamComputeTokenState::Canceled)
		{
			token->PublishFault(DmaStreamComputeTokenFaultKind::DmaDeviceFault, message, token->getDescriptor()->DescriptorReference.DescriptorAddress, false);
		}
		return Result(token, status, std::nullopt, ExternalOperationStage::Failed, message);
	}

	std::shared_ptr<DmaStreamComputeExternalOperationResult> DmaStreamComputeExternalOperationBridge::Result(std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeExternalOperationStatus status, std::optional<std::shared_ptr<ExternalOperationRequest>> request, ExternalOperationStage stage, const std::wstring &message)
	{
		return std::make_shared<DmaStreamComputeExternalOperationResult>(std::move(token), status, std::move(request), stage, message);
	}

	DmaStreamComputeExternalOperationBridge::ExternalOperationState::ExternalOperationState(std::shared_ptr<DmaStreamComputeToken> token, std::shared_ptr<ExternalOperationRequest> request, ExternalOperationStage stage)
		: Token(std::move(token)), Request(std::move(request)), Stage(stage)
	{
	}

	std::shared_ptr<DmaStreamComputeToken> DmaStreamComputeExternalOperationBridge::ExternalOperationState::getToken() const
	{
		return Token;
	}

	std::shared_ptr<ExternalOperationRequest> DmaStreamComputeExternalOperationBridge::ExternalOperationState::getRequest() const
	{
		return Request;
	}

	ExternalOperationStage DmaStreamComputeExternalOperationBridge::ExternalOperationState::getStage() const
	{
		return Stage;
	}

	void DmaStreamComputeExternalOperationBridge::ExternalOperationState::setStage(ExternalOperationStage value)
	{
		Stage = value;
	}
}
