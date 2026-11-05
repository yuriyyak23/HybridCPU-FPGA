#include "AcceleratorToken.h"
#include "../Capabilities/AcceleratorCapabilityRegistry.h"
#include "../Telemetry/AcceleratorTelemetry.h"
#include "AcceleratorTokenStore.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal AcceleratorToken(ulong tokenId, AcceleratorTokenHandle handle, AcceleratorCommandDescriptor descriptor, AcceleratorCapabilityAcceptanceResult capabilityAcceptance, AcceleratorGuardDecision submitGuardDecision, AcceleratorTelemetry? telemetry = null)
	AcceleratorToken::AcceleratorToken(unsigned long long tokenId, std::shared_ptr<AcceleratorTokenHandle> handle, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::shared_ptr<AcceleratorGuardDecision> submitGuardDecision, std::shared_ptr<AcceleratorTelemetry> telemetry)
		: TokenId(tokenId), Handle(handle), Descriptor(descriptor), CapabilityAcceptance(capabilityAcceptance), SubmitGuardDecision(submitGuardDecision), Telemetry(telemetry)
	{
		if (tokenId == 0)
		{
			throw std::out_of_range("L7-SDC token id zero is reserved.");
		}

		if (!handle || !handle->IsValid)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("L7-SDC token handle zero is invalid.", nameof(handle));
			throw std::invalid_argument("L7-SDC token handle zero is invalid.");
		}

		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(capabilityAcceptance);
		if (!submitGuardDecision || !submitGuardDecision->IsAllowed)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("L7-SDC token creation requires accepted submit guard authority.", nameof(submitGuardDecision));
			throw std::invalid_argument("L7-SDC token creation requires accepted submit guard authority.");
		}
		setState(AcceleratorTokenState::Created);
		setFaultCode(AcceleratorTokenFaultCode::None);
		setStatusSequence(1);
	}

	unsigned long long AcceleratorToken::getTokenId() const
	{
		return TokenId;
	}

	std::shared_ptr<AcceleratorTokenHandle> AcceleratorToken::getHandle() const
	{
		return Handle;
	}

	std::shared_ptr<AcceleratorCommandDescriptor> AcceleratorToken::getDescriptor() const
	{
		return Descriptor;
	}

	std::shared_ptr<AcceleratorCapabilityAcceptanceResult> AcceleratorToken::getCapabilityAcceptance() const
	{
		return CapabilityAcceptance;
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorToken::getSubmitGuardDecision() const
	{
		return SubmitGuardDecision;
	}

	std::shared_ptr<AcceleratorTelemetry> AcceleratorToken::getTelemetry() const
	{
		return Telemetry;
	}

	AcceleratorTokenState AcceleratorToken::getState() const
	{
		return State;
	}

	void AcceleratorToken::setState(AcceleratorTokenState value)
	{
		State = value;
	}

	AcceleratorTokenFaultCode AcceleratorToken::getFaultCode() const
	{
		return FaultCode;
	}

	void AcceleratorToken::setFaultCode(AcceleratorTokenFaultCode value)
	{
		FaultCode = value;
	}

	unsigned int AcceleratorToken::getStatusSequence() const
	{
		return StatusSequence;
	}

	void AcceleratorToken::setStatusSequence(unsigned int value)
	{
		StatusSequence = value;
	}

	std::shared_ptr<AcceleratorMappingEpoch> AcceleratorToken::getMappingEpoch() const
	{
		return std::make_shared<AcceleratorMappingEpoch>(getSubmitGuardDecision()->getMappingEpoch());
	}

	std::shared_ptr<AcceleratorIommuDomainEpoch> AcceleratorToken::getIommuDomainEpoch() const
	{
		return std::make_shared<AcceleratorIommuDomainEpoch>(getSubmitGuardDecision()->getIommuDomainEpoch());
	}

	bool AcceleratorToken::isTerminal() const
	{
		return ([&] { const auto codexPatternValue = getState(); return codexPatternValue == AcceleratorTokenState::Committed || codexPatternValue == AcceleratorTokenState::Faulted || codexPatternValue == AcceleratorTokenState::Canceled || codexPatternValue == AcceleratorTokenState::TimedOut || codexPatternValue == AcceleratorTokenState::Abandoned; }());
	}

	bool AcceleratorToken::getHasBackendExecution() const
	{
		return false;
	}

	bool AcceleratorToken::getHasQueueExecution() const
	{
		return false;
	}

	bool AcceleratorToken::getHasStagedWrites() const
	{
		return false;
	}

	bool AcceleratorToken::getHasArchitecturalCommit() const
	{
		return getState() == AcceleratorTokenState::Committed;
	}

	bool AcceleratorToken::getUserVisiblePublicationAllowed() const
	{
		return getState() == AcceleratorTokenState::Committed;
	}

	std::shared_ptr<AcceleratorTokenStatusWord> AcceleratorToken::GetStatusWord()
	{
		return std::make_shared<AcceleratorTokenStatusWord>(AcceleratorTokenStatusWord::FromToken(*this));
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkValidated(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTransition(AcceleratorTokenState::Validated, currentGuardEvidence, [] (AcceleratorTokenState state)
		{
			return state == AcceleratorTokenState::Created;
		}, L"ACCEL_SUBMIT token validated as model-only Phase 06 state.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkQueued(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTransition(AcceleratorTokenState::Queued, currentGuardEvidence, [] (AcceleratorTokenState state)
		{
			return state == AcceleratorTokenState::Validated;
		}, L"L7-SDC token entered guarded Phase 07 model Queued state; no architectural publication authority is granted.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkRunning(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTransition(AcceleratorTokenState::Running, currentGuardEvidence, [] (AcceleratorTokenState state)
		{
			return state == AcceleratorTokenState::Queued;
		}, L"L7-SDC token entered guarded Phase 07 model Running state; backend execution remains non-publishing.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkDeviceComplete(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTransition(AcceleratorTokenState::DeviceComplete, currentGuardEvidence, [] (AcceleratorTokenState state)
		{
			return state == AcceleratorTokenState::Running;
		}, L"L7-SDC token entered model DeviceComplete state; no staged writes are published.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkCommitPending(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		if (isTerminal())
		{
			return AcceleratorTokenTransition::Reject(getState(), AcceleratorTokenState::CommitPending, AcceleratorTokenFaultCode::TerminalState, std::format(L"L7-SDC token {} is terminal ({}) and cannot transition to CommitPending.", getHandle()->Value, static_cast<int>(getState())));
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateCurrentGuard(currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return RejectGuardedTransition(AcceleratorTokenState::CommitPending, guardDecision, L"L7-SDC token CommitPending promotion requires a fresh owner/domain and epoch guard.");
		}

		if (getState() != AcceleratorTokenState::DeviceComplete)
		{
			return AcceleratorTokenTransition::Reject(getState(), AcceleratorTokenState::CommitPending, AcceleratorTokenFaultCode::IllegalTransition, std::format(L"Illegal L7-SDC token transition {} -> CommitPending.", static_cast<int>(getState())));
		}

		return AcceleratorTokenTransition::Reject(getState(), AcceleratorTokenState::CommitPending, AcceleratorTokenFaultCode::CommitCoordinatorRequired, L"L7-SDC DeviceComplete -> CommitPending promotion is owned by the Phase 08 commit coordinator; token identity cannot promote itself.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkCommitted(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		if (isTerminal())
		{
			return AcceleratorTokenTransition::Reject(getState(), AcceleratorTokenState::Committed, AcceleratorTokenFaultCode::TerminalState, std::format(L"L7-SDC token {} is terminal ({}) and cannot transition to Committed.", getHandle()->Value, static_cast<int>(getState())));
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateCurrentGuard(currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return RejectGuardedTransition(AcceleratorTokenState::Committed, guardDecision, L"L7-SDC token commit requires a fresh owner/domain and epoch guard.");
		}

		if (getState() == AcceleratorTokenState::DeviceComplete)
		{
			return AcceleratorTokenTransition::Reject(getState(), AcceleratorTokenState::Committed, AcceleratorTokenFaultCode::CommitCoordinatorRequired, L"L7-SDC DeviceComplete tokens must enter CommitPending through the Phase 08 commit coordinator before architectural commit.");
		}

		if (getState() != AcceleratorTokenState::CommitPending)
		{
			return AcceleratorTokenTransition::Reject(getState(), AcceleratorTokenState::Committed, AcceleratorTokenFaultCode::IllegalTransition, std::format(L"Illegal L7-SDC token transition {} -> Committed.", static_cast<int>(getState())));
		}

		return AcceleratorTokenTransition::Reject(getState(), AcceleratorTokenState::Committed, AcceleratorTokenFaultCode::CommitNotImplemented, L"Phase 06 has no staged-write or architectural commit contour; token identity cannot commit.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkCommitPendingFromCommitCoordinator(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTransition(AcceleratorTokenState::CommitPending, currentGuardEvidence, [] (AcceleratorTokenState state)
		{
			return state == AcceleratorTokenState::DeviceComplete;
		}, L"L7-SDC token entered CommitPending through the guarded Phase 08 commit coordinator.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkCommittedFromCommitCoordinator(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTransition(AcceleratorTokenState::Committed, currentGuardEvidence, [] (AcceleratorTokenState state)
		{
			return state == AcceleratorTokenState::CommitPending;
		}, L"L7-SDC staged writes were published architecturally by the guarded Phase 08 commit coordinator.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkFaulted(AcceleratorTokenFaultCode faultCode, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		if (faultCode == AcceleratorTokenFaultCode::None)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Faulted token transition requires a non-None fault code.", nameof(faultCode));
			throw std::invalid_argument("Faulted token transition requires a non-None fault code.");
		}

		return TryTerminalTransition(AcceleratorTokenState::Faulted, faultCode, currentGuardEvidence, L"L7-SDC token marked Faulted after guard-backed model transition.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkCanceled(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTransition(AcceleratorTokenState::Canceled, currentGuardEvidence, [] (AcceleratorTokenState state)
		{
			return (state == AcceleratorTokenState::Created || state == AcceleratorTokenState::Validated || state == AcceleratorTokenState::Queued || state == AcceleratorTokenState::Running);
		}, L"L7-SDC token canceled in Phase 06 model; no backend cancel path is invoked.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkTimedOut(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTerminalTransition(AcceleratorTokenState::TimedOut, AcceleratorTokenFaultCode::TimedOut, currentGuardEvidence, L"L7-SDC token timed out in Phase 06 model; no backend wait path is invoked.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::MarkAbandoned(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		return TryTerminalTransition(AcceleratorTokenState::Abandoned, AcceleratorTokenFaultCode::OwnerDomainRejected, currentGuardEvidence, L"L7-SDC token abandoned; user-visible publication remains forbidden.");
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::TryTerminalTransition(AcceleratorTokenState targetState, AcceleratorTokenFaultCode terminalFaultCode, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, const std::wstring &message)
	{
		return TryTransition(targetState, currentGuardEvidence, [] (AcceleratorTokenState state)
		{
			return state != AcceleratorTokenState::Committed && state != AcceleratorTokenState::Faulted && state != AcceleratorTokenState::Canceled && state != AcceleratorTokenState::TimedOut && state != AcceleratorTokenState::Abandoned;
		}, message, terminalFaultCode);
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::TryTransition(AcceleratorTokenState targetState, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, std::function<bool(AcceleratorTokenState)> isLegalSource, const std::wstring &successMessage, AcceleratorTokenFaultCode targetFaultCode)
	{
		if (!isLegalSource) throw std::invalid_argument("isLegalSource");

		AcceleratorTokenState originalState = getState();
		if (isTerminal())
		{
			return AcceleratorTokenTransition::Reject(originalState, targetState, AcceleratorTokenFaultCode::TerminalState, std::format(L"L7-SDC token {} is terminal ({}) and cannot transition to {}.", getHandle()->Value, static_cast<int>(getState()), static_cast<int>(targetState)));
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateCurrentGuard(currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return RejectGuardedTransition(targetState, guardDecision, successMessage);
		}

		if (!isLegalSource(originalState))
		{
			return AcceleratorTokenTransition::Reject(originalState, targetState, AcceleratorTokenFaultCode::IllegalTransition, std::format(L"Illegal L7-SDC token transition {} -> {}.", static_cast<int>(originalState), static_cast<int>(targetState)));
		}

		setState(targetState);
		setFaultCode(targetFaultCode);
		AdvanceStatusSequence();
		if (getTelemetry() != nullptr)
		{
			getTelemetry()->RecordTokenTransition(shared_from_this(), originalState, targetState, targetFaultCode, successMessage);
		}
		return AcceleratorTokenTransition::Success(originalState, targetState, successMessage);
	}

	std::shared_ptr<AcceleratorTokenTransition> AcceleratorToken::RejectGuardedTransition(AcceleratorTokenState targetState, std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &surfaceMessage)
	{
		if (getTelemetry() != nullptr)
		{
			std::optional<std::shared_ptr<AcceleratorGuardDecision>> guard = guardDecision;
			getTelemetry()->RecordGuardReject(guard, surfaceMessage + L" " + guardDecision->Message);
		}
		return AcceleratorTokenTransition::Reject(getState(), targetState, AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), surfaceMessage + L" " + guardDecision->Message);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorToken::ValidateCurrentGuard(std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence)
	{
		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> evidence = currentGuardEvidence;
		return AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(getSubmitGuardDecision(), evidence);
	}

	void AcceleratorToken::AdvanceStatusSequence()
	{
//		unchecked
			setStatusSequence(getStatusSequence() + 1);
			if (getStatusSequence() == 0)
			{
				setStatusSequence(1);
			}
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}
}
