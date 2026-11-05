#include "AcceleratorTokenStore.h"
#include "../Descriptors/AcceleratorDescriptorTaxonomyCatalog.h"
#include "../../../Pipeline/Safety/SafetyVerifier.Types.h"
#include "AcceleratorObservationControlPolicies.h"
#include "../Capabilities/AcceleratorCapabilityDescriptor.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorTokenStore(AcceleratorTelemetry? telemetry = null, System.Nullable<ExternalAcceleratorFeatureSwitch> featureSwitch = null)
	AcceleratorTokenStore::AcceleratorTokenStore(std::shared_ptr<AcceleratorTelemetry> telemetry, std::optional<std::shared_ptr<ExternalAcceleratorFeatureSwitch>> featureSwitch) : _telemetry(telemetry), _featureSwitch(featureSwitch.value_or(ExternalAcceleratorFeatureSwitch::Enabled))
	{
	}

	int AcceleratorTokenStore::getCount() const
	{
		return _tokens.size();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorTokenAdmissionResult Create(AcceleratorCommandDescriptor descriptor, AcceleratorCapabilityAcceptanceResult capabilityAcceptance, System.Nullable<AcceleratorGuardEvidence> submitGuardEvidence, AcceleratorTokenAdmissionRejectPolicy rejectPolicy = AcceleratorTokenAdmissionRejectPolicy.NonTrappingReject, ExternalAcceleratorConflictManager? conflictManager = null)
	std::shared_ptr<AcceleratorTokenAdmissionResult> AcceleratorTokenStore::Create(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &submitGuardEvidence, AcceleratorTokenAdmissionRejectPolicy rejectPolicy, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(capabilityAcceptance);

		std::wstring descriptorGuardMessage;
		if (!AcceleratorOwnerDomainGuard::getDefault()->IsDescriptorGuardBacked(descriptor, descriptorGuardMessage))
		{
			return RecordSubmitResult(AcceleratorTokenAdmissionResult::Reject(AcceleratorTokenFaultCode::DescriptorNotGuardBacked, descriptorGuardMessage, descriptor->OwnerGuardDecision, rejectPolicy));
		}

		std::wstring capabilityMessage;
		if (!IsCapabilityGuardBackedForDescriptor(capabilityAcceptance, descriptor, capabilityMessage))
		{
			return RecordSubmitResult(AcceleratorTokenAdmissionResult::Reject(capabilityAcceptance->isAccepted() ? AcceleratorTokenFaultCode::CapabilityNotAccepted : AcceleratorTokenFaultCode::CapabilityRejected, capabilityMessage, capabilityAcceptance->getGuardDecision(), rejectPolicy));
		}

		std::wstring taxonomyMessage;
		if (!IsCurrentRuntimeTaxonomyForTokenAdmission(descriptor, capabilityAcceptance, taxonomyMessage))
		{
			return RecordSubmitResult(AcceleratorTokenAdmissionResult::Reject(AcceleratorTokenFaultCode::CapabilityNotAccepted, taxonomyMessage, capabilityAcceptance->getGuardDecision(), rejectPolicy));
		}

		std::shared_ptr<AcceleratorGuardDecision> submitGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeSubmit(descriptor, submitGuardEvidence);
		if (!submitGuardDecision->IsAllowed)
		{
			return RecordSubmitResult(AcceleratorTokenAdmissionResult::Reject(MapGuardFault(submitGuardDecision->Fault), submitGuardDecision->Message, submitGuardDecision, rejectPolicy));
		}

		if (!_featureSwitch->SubmitAdmissionEnabled)
		{
			return RecordSubmitResult(AcceleratorTokenAdmissionResult::Reject(AcceleratorTokenFaultCode::SubmitAdmissionRejected, L"L7-SDC submit admission is disabled by rollback feature switch after descriptor, capability, and guard validation; no token was created.", submitGuardDecision, rejectPolicy));
		}

		unsigned long long tokenId = AllocateTokenId();
		std::shared_ptr<AcceleratorTokenHandle> handle = AllocateHandle(tokenId);
		std::shared_ptr<AcceleratorToken> token = std::make_shared<AcceleratorToken>(tokenId, handle, descriptor, capabilityAcceptance, submitGuardDecision, _telemetry);
		_tokens.emplace(handle->Value, token);

		if (conflictManager != nullptr)
		{
			std::shared_ptr<AcceleratorConflictDecision> reservation = conflictManager->TryReserveOnSubmit(token, submitGuardEvidence);
			if (reservation->IsRejected)
			{
				_tokens.erase(handle->Value);
				return RecordSubmitResult(AcceleratorTokenAdmissionResult::Reject(reservation->TokenFaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::ConflictRejected : reservation->TokenFaultCode, reservation->Message, reservation->GuardDecision, rejectPolicy));
			}
		}

		return RecordSubmitResult(AcceleratorTokenAdmissionResult::Accepted(token));
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::TryLookup(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, AcceleratorTokenLookupIntent intent)
	{
		if (!handle || !handle->IsValid)
		{
			return RecordLookupResult(AcceleratorTokenLookupResult::Rejected(intent, AcceleratorTokenState::Faulted, AcceleratorTokenFaultCode::InvalidHandle, L"L7-SDC token handle zero is invalid and cannot authorize lookup."));
		}

		auto tokensIterator = _tokens.find(handle->Value);
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!_tokens.TryGetValue(handle.Value, out AcceleratorToken? token))
		if (tokensIterator == _tokens.end())
		{
			return RecordLookupResult(AcceleratorTokenLookupResult::Rejected(intent, AcceleratorTokenState::Faulted, AcceleratorTokenFaultCode::InvalidHandle, L"L7-SDC token handle is unknown, stale, or forged."));
		}

		std::shared_ptr<AcceleratorToken> token = tokensIterator->second;

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return RecordLookupResult(AcceleratorTokenLookupResult::Rejected(intent, token->getState(), MapGuardFault(guardDecision->Fault), L"L7-SDC token lookup requires current guard-plane owner/domain and epoch authority. " + guardDecision->Message, token, guardDecision));
		}

		return AcceleratorTokenLookupResult::Allowed(intent, token, guardDecision, L"L7-SDC token lookup accepted after owner/domain and mapping/IOMMU epoch revalidation.");
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::TryPoll(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return TryLookup(handle, currentGuardEvidence, AcceleratorTokenLookupIntent::Observe);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::Poll(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return TryPoll(handle, currentGuardEvidence);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::TryStatus(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return TryLookup(handle, currentGuardEvidence, AcceleratorTokenLookupIntent::Observe);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::Status(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return TryStatus(handle, currentGuardEvidence);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::TryWait(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorWaitPolicy> waitPolicy)
	{
		std::shared_ptr<AcceleratorTokenLookupResult> lookup = TryLookup(handle, currentGuardEvidence, AcceleratorTokenLookupIntent::Wait);
		if (!lookup->IsAllowed || lookup->Token == nullptr)
		{
			return lookup;
		}

		std::shared_ptr<AcceleratorToken> token = lookup->Token;
		if (token->isTerminal() || ([&] { const auto codexPatternValue = token->getState(); return codexPatternValue == AcceleratorTokenState::DeviceComplete || codexPatternValue == AcceleratorTokenState::CommitPending; }()))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return AcceleratorTokenLookupResult.Allowed( AcceleratorTokenLookupIntent.Wait, token, lookup.GuardDecision!.Value, "ACCEL_WAIT observed a final or device-complete token state after fresh guard revalidation; no staged write was published.");
			return AcceleratorTokenLookupResult::Allowed(AcceleratorTokenLookupIntent::Wait, token, lookup->GuardDecision, L"ACCEL_WAIT observed a final or device-complete token state after fresh guard revalidation; no staged write was published.");
		}

		if (!waitPolicy->TimeoutExpired)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return AcceleratorTokenLookupResult.Allowed( AcceleratorTokenLookupIntent.Wait, token, lookup.GuardDecision!.Value, "ACCEL_WAIT observed a non-final token state after fresh guard revalidation; no direct device write was published.");
			return AcceleratorTokenLookupResult::Allowed(AcceleratorTokenLookupIntent::Wait, token, lookup->GuardDecision, L"ACCEL_WAIT observed a non-final token state after fresh guard revalidation; no direct device write was published.");
		}

		if (waitPolicy->MarkTokenTimedOutOnTimeout)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition timeout = token.MarkTimedOut(currentGuardEvidence!);
			std::shared_ptr<AcceleratorTokenTransition> timeout = token->MarkTimedOut(*currentGuardEvidence);
			if (timeout->Rejected)
			{
				return AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Wait, token->getState(), timeout->FaultCode, timeout->Message, token, lookup->GuardDecision);
			}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return AcceleratorTokenLookupResult.AllowedWithStatus( AcceleratorTokenLookupIntent.Wait, token, lookup.GuardDecision!.Value, AcceleratorTokenStatusWord.FromToken(token), AcceleratorTokenFaultCode.TimedOut, true, string.Format("ACCEL_WAIT timeout policy marked the guarded token TimedOut after {0} tick(s); no commit authority was granted.", waitPolicy.TimeoutTicks));
			return AcceleratorTokenLookupResult::AllowedWithStatus(AcceleratorTokenLookupIntent::Wait, token, lookup->GuardDecision, AcceleratorTokenStatusWord::FromToken(*token), AcceleratorTokenFaultCode::TimedOut, true, std::format(L"ACCEL_WAIT timeout policy marked the guarded token TimedOut after {} tick(s); no commit authority was granted.", waitPolicy->TimeoutTicks));
		}

		AcceleratorTokenStatusWord timeoutStatus = AcceleratorTokenStatusWord(token->getState(), AcceleratorTokenFaultCode::TimedOut, AcceleratorTokenStatusFlags::ModelOnly | AcceleratorTokenStatusFlags::TimeoutObserved, token->getStatusSequence());
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return AcceleratorTokenLookupResult.AllowedWithStatus( AcceleratorTokenLookupIntent.Wait, token, lookup.GuardDecision!.Value, timeoutStatus, AcceleratorTokenFaultCode.TimedOut, false, string.Format("ACCEL_WAIT observed timeout after {0} tick(s); token state was not promoted and staged writes remain invisible.", waitPolicy.TimeoutTicks));
		return AcceleratorTokenLookupResult::AllowedWithStatus(AcceleratorTokenLookupIntent::Wait, token, lookup->GuardDecision, timeoutStatus, AcceleratorTokenFaultCode::TimedOut, false, std::format(L"ACCEL_WAIT observed timeout after {} tick(s); token state was not promoted and staged writes remain invisible.", waitPolicy->TimeoutTicks));
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::Wait(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return TryWait(handle, currentGuardEvidence, AcceleratorWaitPolicy::ObserveOnly);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::Fence(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return TryLookup(handle, currentGuardEvidence, AcceleratorTokenLookupIntent::Fence);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::TryCancel(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorCancelPolicy> cancelPolicy)
	{
		std::shared_ptr<AcceleratorTokenLookupResult> lookup = TryLookup(handle, currentGuardEvidence, AcceleratorTokenLookupIntent::Cancel);
		if (!lookup->IsAllowed || lookup->Token == nullptr)
		{
			return lookup;
		}

		std::shared_ptr<AcceleratorToken> token = lookup->Token;
		if (token->isTerminal())
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return AcceleratorTokenLookupResult.Allowed( AcceleratorTokenLookupIntent.Cancel, token, lookup.GuardDecision!.Value, "ACCEL_CANCEL observed an already-terminal token after guard revalidation; no commit authority was granted.");
			return AcceleratorTokenLookupResult::Allowed(AcceleratorTokenLookupIntent::Cancel, token, lookup->GuardDecision, L"ACCEL_CANCEL observed an already-terminal token after guard revalidation; no commit authority was granted.");
		}

		if (([&] { const auto codexPatternValue = token->getState(); return codexPatternValue == AcceleratorTokenState::DeviceComplete || codexPatternValue == AcceleratorTokenState::CommitPending; }()))
		{
			return AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Cancel, token->getState(), AcceleratorTokenFaultCode::CommitCoordinatorRequired, L"ACCEL_CANCEL cannot discard DeviceComplete or CommitPending staged-write obligations; Phase 08 commit coordinator or a guarded fault path must resolve them.", token, lookup->GuardDecision);
		}

		if (token->getState() == AcceleratorTokenState::Running)
		{
			if (cancelPolicy->RunningDisposition == AcceleratorRunningCancelDisposition::Reject)
			{
				return AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Cancel, token->getState(), AcceleratorTokenFaultCode::CancelRejected, L"ACCEL_CANCEL rejected a running token under non-cooperative cancel policy; cancellation did not grant commit authority.", token, lookup->GuardDecision);
			}

			if (cancelPolicy->RunningDisposition == AcceleratorRunningCancelDisposition::Fault)
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition faulted = token.MarkFaulted( AcceleratorTokenFaultCode.CancelRejected, currentGuardEvidence!);
				std::shared_ptr<AcceleratorTokenTransition> faulted = token->MarkFaulted(AcceleratorTokenFaultCode::CancelRejected, *currentGuardEvidence);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return faulted.Succeeded ? AcceleratorTokenLookupResult.AllowedWithStatus( AcceleratorTokenLookupIntent.Cancel, token, lookup.GuardDecision!.Value, AcceleratorTokenStatusWord.FromToken(token), AcceleratorTokenFaultCode.CancelRejected, true, "ACCEL_CANCEL faulted a running token under explicit guarded policy; no staged writes were committed.") : AcceleratorTokenLookupResult.Rejected( AcceleratorTokenLookupIntent.Cancel, token.State, faulted.FaultCode, faulted.Message, token, lookup.GuardDecision);
				return faulted->Succeeded ? AcceleratorTokenLookupResult::AllowedWithStatus(AcceleratorTokenLookupIntent::Cancel, token, lookup->GuardDecision, AcceleratorTokenStatusWord::FromToken(*token), AcceleratorTokenFaultCode::CancelRejected, true, L"ACCEL_CANCEL faulted a running token under explicit guarded policy; no staged writes were committed.") : AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Cancel, token->getState(), faulted->FaultCode, faulted->Message, token, lookup->GuardDecision);
			}
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition transition = token.MarkCanceled(currentGuardEvidence!);
		std::shared_ptr<AcceleratorTokenTransition> transition = token->MarkCanceled(*currentGuardEvidence);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return transition.Succeeded ? AcceleratorTokenLookupResult.Allowed( AcceleratorTokenLookupIntent.Cancel, token, lookup.GuardDecision!.Value, transition.Message) : AcceleratorTokenLookupResult.Rejected( AcceleratorTokenLookupIntent.Cancel, token.State, transition.FaultCode, transition.Message, token, lookup.GuardDecision);
		return transition->Succeeded ? AcceleratorTokenLookupResult::Allowed(AcceleratorTokenLookupIntent::Cancel, token, lookup->GuardDecision, transition->Message) : AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::Cancel, token->getState(), transition->FaultCode, transition->Message, token, lookup->GuardDecision);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::TryCancel(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return TryCancel(handle, currentGuardEvidence, AcceleratorCancelPolicy::Cooperative);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::Cancel(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return TryCancel(handle, currentGuardEvidence, AcceleratorCancelPolicy::Cooperative);
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::TryCommitPublication(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		std::shared_ptr<AcceleratorTokenLookupResult> lookup = TryLookup(handle, currentGuardEvidence, AcceleratorTokenLookupIntent::CommitPublication);
		if (!lookup->IsAllowed || lookup->Token == nullptr)
		{
			return lookup;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition transition = lookup.Token.MarkCommitted(currentGuardEvidence!);
		std::shared_ptr<AcceleratorTokenTransition> transition = lookup->Token->MarkCommitted(*currentGuardEvidence);
		return AcceleratorTokenLookupResult::Rejected(AcceleratorTokenLookupIntent::CommitPublication, lookup->Token->getState(), transition->FaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::MemoryPublicationForbidden : transition->FaultCode, transition->Message, lookup->Token, lookup->GuardDecision);
	}

	AcceleratorTokenFaultCode AcceleratorTokenStore::MapGuardFault(AcceleratorGuardFault guardFault)
	{
		switch (guardFault) {
		case AcceleratorGuardFault::None: return AcceleratorTokenFaultCode::None;
		case AcceleratorGuardFault::MissingGuardEvidence: return AcceleratorTokenFaultCode::MissingGuardEvidence;
		case AcceleratorGuardFault::MappingEpochDrift: return AcceleratorTokenFaultCode::MappingEpochDrift;
		case AcceleratorGuardFault::IommuDomainEpochDrift: return AcceleratorTokenFaultCode::IommuDomainEpochDrift;
		case AcceleratorGuardFault::EvidenceSourceNotAuthority: return AcceleratorTokenFaultCode::TokenHandleNotAuthority;
		case AcceleratorGuardFault::CapabilityGuardMissing: return AcceleratorTokenFaultCode::CapabilityNotAccepted;
		case AcceleratorGuardFault::OwnerMismatch: case AcceleratorGuardFault::DomainMismatch: case AcceleratorGuardFault::DescriptorOwnerBindingMismatch: case AcceleratorGuardFault::RejectedGuard: case AcceleratorGuardFault::InvalidOwnerCompletion: return AcceleratorTokenFaultCode::OwnerDomainRejected;
		default: return AcceleratorTokenFaultCode::Unknown;
		}
	}

	bool AcceleratorTokenStore::IsCapabilityGuardBackedForDescriptor(std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::wstring &message)
	{
		if (!capabilityAcceptance->isAccepted())
		{
			message = StringHelper::isEmptyOrWhiteSpace(capabilityAcceptance->getRejectReason()) ? L"L7-SDC capability acceptance rejected; registry metadata cannot authorize token creation." : capabilityAcceptance->getRejectReason();
			return false;
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = capabilityAcceptance->getGuardDecision();
		if (!guardDecision->IsAllowed || (!guardDecision->Evidence || guardDecision->Evidence->Source != AcceleratorGuardEvidenceSource::GuardPlane) || guardDecision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || guardDecision->LegalityDecision.getAttemptedReplayCertificateReuse())
		{
			message = L"L7-SDC capability acceptance must be guard-backed; metadata-only registry success is not token authority.";
			return false;
		}

		if (guardDecision->DescriptorOwnerBinding == nullptr || !guardDecision->DescriptorOwnerBinding->Equals(descriptor->OwnerBinding))
		{
			message = L"L7-SDC capability guard decision does not match the descriptor owner binding.";
			return false;
		}

		message = L"L7-SDC capability acceptance is guard-backed for token admission.";
		return true;
	}

	bool AcceleratorTokenStore::IsCurrentRuntimeTaxonomyForTokenAdmission(std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorCapabilityAcceptanceResult> capabilityAcceptance, std::wstring &message)
	{
		std::shared_ptr<AcceleratorDescriptorTaxonomyKey> descriptorKey = std::make_shared<AcceleratorDescriptorTaxonomyKey>(descriptor->AcceleratorClass, descriptor->AcceleratorId, descriptor->Operation, descriptor->Shape);

		std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> taxonomyEntry;
		if (!AcceleratorDescriptorTaxonomyCatalog::TryGetEntry(descriptorKey, taxonomyEntry))
		{
			message = L"L7-SDC token admission requires a known current-runtime descriptor taxonomy entry; unknown descriptor metadata is not token authority.";
			return false;
		}

		if (!taxonomyEntry->isCurrentRuntimeContour())
		{
			message = std::format(L"L7-SDC token admission rejects '{}' because taxonomy status {} grants no token, queue, execution, or commit authority.", taxonomyEntry->getName(), static_cast<int>(taxonomyEntry->getStatus()));
			return false;
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: AcceleratorCapabilityDescriptor? capabilityDescriptor = capabilityAcceptance.Descriptor;
		std::shared_ptr<AcceleratorCapabilityDescriptor> capabilityDescriptor = capabilityAcceptance->getDescriptor();
		if (capabilityDescriptor == nullptr || (!capabilityDescriptor->getTaxonomyKey() || !(*capabilityDescriptor->getTaxonomyKey() == *descriptorKey)) || !capabilityDescriptor->getTaxonomyEntry()->isCurrentRuntimeContour())
		{
			message = L"L7-SDC token admission requires capability metadata pinned to the descriptor current-runtime taxonomy entry; metadata-only and non-current accelerator metadata are not token authority.";
			return false;
		}

		message = L"L7-SDC descriptor and capability are pinned to the current runtime taxonomy entry.";
		return true;
	}

	unsigned long long AcceleratorTokenStore::AllocateTokenId()
	{
		unsigned long long tokenId = _nextTokenId++;
		if (_nextTokenId == 0)
		{
			_nextTokenId = 1;
		}

		return tokenId;
	}

	std::shared_ptr<AcceleratorTokenHandle> AcceleratorTokenStore::AllocateHandle(unsigned long long tokenId)
	{
		while (true)
		{
			unsigned long long generation = _handleGeneration++;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: ulong value = unchecked(HandleSalt ^ (tokenId * HandleStride) ^ (generation << 17) ^ (generation >> 11));
			unsigned long long value = HandleSalt ^ (tokenId * HandleStride) ^ (generation << 17) ^ (generation >> 11);
			if (value != 0 && !_tokens.contains(value))
			{
				return AcceleratorTokenHandle::FromOpaqueValue(value);
			}
		}
	}

	std::shared_ptr<AcceleratorTokenAdmissionResult> AcceleratorTokenStore::RecordSubmitResult(std::shared_ptr<AcceleratorTokenAdmissionResult> result)
	{
		if (_telemetry != nullptr)
		{
			_telemetry->RecordSubmit(result->IsAccepted, result->FaultCode, result->GuardDecision, result->Message);
		}
		if (result->IsAccepted && result->Token != nullptr)
		{
			if (_telemetry != nullptr)
			{
				_telemetry->RecordTokenTransition(result->Token, AcceleratorTokenState::Created, AcceleratorTokenState::Created, AcceleratorTokenFaultCode::None, L"L7-SDC token was created by guarded submit admission.");
			}
		}

		return result;
	}

	std::shared_ptr<AcceleratorTokenLookupResult> AcceleratorTokenStore::RecordLookupResult(std::shared_ptr<AcceleratorTokenLookupResult> result)
	{
		if (result->IsRejected)
		{
			if (_telemetry != nullptr)
			{
				std::optional<std::shared_ptr<AcceleratorGuardDecision>> guard = result->GuardDecision;
				_telemetry->RecordGuardReject(guard, result->Message);
			}
		}

		return result;
	}
}
