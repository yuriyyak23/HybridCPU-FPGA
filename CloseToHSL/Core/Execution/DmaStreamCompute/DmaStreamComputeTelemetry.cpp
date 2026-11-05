#include "DmaStreamComputeTelemetry.h"
#include "DmaStreamComputeReplayEvidenceComparison.h"
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{

	std::shared_ptr<DmaStreamComputeTelemetrySnapshot> DmaStreamComputeTelemetryCounters::Snapshot()
	{
		auto result = std::make_shared<DmaStreamComputeTelemetrySnapshot>();
		result->DescriptorParseAttempts = _descriptorParseAttempts;
		result->DescriptorAccepted = _descriptorAccepted;
		result->DescriptorRejected = _descriptorRejected;
		result->ComputeAccepted = _acceptedTokens.size();
		result->ComputeActive = _activeTokens.size();
		result->ComputeStaged = _stagedTokens.size();
		result->ComputeCommitted = _committedTokens.size();
		result->ComputeCanceled = _canceledTokens.size();
		result->ComputeFaulted = _faultedTokens.size();
		result->ComputeRejected = _computeRejected;
		result->DescriptorFaults = _descriptorFaults;
		result->OwnerDomainFaults = _ownerDomainFaults;
		result->DeviceFaults = _deviceFaults;
		result->TokenFaults = _tokenFaults;
		result->QuotaRejects = _quotaRejects;
		result->BackpressureRejects = _backpressureRejects;
		result->Lane6BackpressureRejects = _lane6BackpressureRejects;
		result->DmaCreditRejects = _dmaCreditRejects;
		result->SrfCreditRejects = _srfCreditRejects;
		result->MemorySubsystemPressureRejects = _memorySubsystemPressureRejects;
		result->OutstandingTokenCapRejects = _outstandingTokenCapRejects;
		result->UnsupportedCarrierRejects = _unsupportedCarrierRejects;
		result->ReplayEnvelopeReuseHits = _replayEnvelopeReuseHits;
		result->ReplayEnvelopeRejects = _replayEnvelopeRejects;
		result->BytesRead = _bytesRead;
		result->BytesStaged = _bytesStaged;
		result->ElementOperations = _elementOperations;
		result->CopyOperations = _copyOperations;
		result->AddOperations = _addOperations;
		result->MulOperations = _mulOperations;
		result->FmaOperations = _fmaOperations;
		result->ReduceOperations = _reduceOperations;
		result->LastValidationFault = _lastValidationFault;
		result->LastTokenFaultKind = _lastTokenFaultKind;
		result->LastPressureRejectKind = _lastPressureRejectKind;
		result->LastReplayInvalidationReason = _lastReplayInvalidationReason;
		result->LastReplayMismatchField = _lastReplayMismatchField;
		result->LastLane6Available = _lastLane6Available;
		result->LastDmaCreditsAvailable = _lastDmaCreditsAvailable;
		result->LastSrfCreditsAvailable = _lastSrfCreditsAvailable;
		result->LastMemorySubsystemCreditsAvailable = _lastMemorySubsystemCreditsAvailable;
		result->LastOutstandingTokens = _lastOutstandingTokens;
		result->LastOutstandingTokenCap = _lastOutstandingTokenCap;
		return result;
	}

	void DmaStreamComputeTelemetryCounters::RecordDescriptorParseAttempt()
	{
		_descriptorParseAttempts++;
	}

	void DmaStreamComputeTelemetryCounters::RecordDescriptorAccepted(std::shared_ptr<DmaStreamComputeDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
		_descriptorAccepted++;
		_lastValidationFault = DmaStreamComputeValidationFault::None;
	}

	void DmaStreamComputeTelemetryCounters::RecordDescriptorRejected(DmaStreamComputeValidationFault fault)
	{
		if (fault == DmaStreamComputeValidationFault::None)
		{
			return;
		}

		_descriptorRejected++;
		_computeRejected++;
		_lastValidationFault = fault;
		RecordValidationFaultFamily(fault);
	}

	void DmaStreamComputeTelemetryCounters::RecordOwnerGuardRejected(std::shared_ptr<DmaStreamComputeOwnerBinding> structuralOwnerBinding, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> ownerGuardDecision)
	{
		if (!structuralOwnerBinding) throw std::invalid_argument("structuralOwnerBinding");

		_descriptorRejected++;
		_computeRejected++;
		_lastValidationFault = DmaStreamComputeValidationFault::OwnerDomainFault;

		if (structuralOwnerBinding->DeviceId != DmaStreamComputeDescriptor::CanonicalLane6DeviceId || structuralOwnerBinding->DeviceId != ownerGuardDecision->RuntimeOwnerContext.DeviceId)
		{
			_deviceFaults++;
			return;
		}

		_ownerDomainFaults++;
	}

	void DmaStreamComputeTelemetryCounters::RecordPressureSnapshot(std::shared_ptr<DmaStreamComputePressurePolicy> policy, std::shared_ptr<DmaStreamComputePressureSnapshot> snapshot)
	{
		_lastLane6Available = snapshot->Lane6Available;
		_lastDmaCreditsAvailable = snapshot->DmaCreditsAvailable;
		_lastSrfCreditsAvailable = snapshot->SrfCreditsAvailable;
		_lastMemorySubsystemCreditsAvailable = snapshot->MemorySubsystemCreditsAvailable;
		_lastOutstandingTokens = snapshot->OutstandingTokens;
		_lastOutstandingTokenCap = policy->OutstandingTokenCap;
	}

	void DmaStreamComputeTelemetryCounters::RecordAdmissionReject(DmaStreamComputePressureRejectKind rejectKind, DmaStreamComputeValidationFault validationFault)
	{
		if (rejectKind == DmaStreamComputePressureRejectKind::None)
		{
			return;
		}

		_computeRejected++;
		_lastPressureRejectKind = rejectKind;
		_lastValidationFault = validationFault;

		switch (rejectKind)
		{
			case DmaStreamComputePressureRejectKind::OutstandingTokenCap:
				_quotaRejects++;
				_outstandingTokenCapRejects++;
				break;
			case DmaStreamComputePressureRejectKind::Lane6Unavailable:
				_backpressureRejects++;
				_lane6BackpressureRejects++;
				break;
			case DmaStreamComputePressureRejectKind::DmaCredits:
				_backpressureRejects++;
				_dmaCreditRejects++;
				break;
			case DmaStreamComputePressureRejectKind::SrfCredits:
				_backpressureRejects++;
				_srfCreditRejects++;
				break;
			case DmaStreamComputePressureRejectKind::MemorySubsystemPressure:
				_backpressureRejects++;
				_memorySubsystemPressureRejects++;
				break;
		}
	}

	void DmaStreamComputeTelemetryCounters::RecordTokenAccepted(std::shared_ptr<DmaStreamComputeToken> token)
	{
		if (!token) throw std::invalid_argument("token");
		_acceptedTokens.insert(token->getTokenId());
	}

	void DmaStreamComputeTelemetryCounters::RecordTokenActive(std::shared_ptr<DmaStreamComputeToken> token)
	{
		if (!token) throw std::invalid_argument("token");
		_activeTokens.insert(token->getTokenId());
	}

	void DmaStreamComputeTelemetryCounters::RecordTokenStaged(std::shared_ptr<DmaStreamComputeToken> token, unsigned long long byteCount)
	{
		if (!token) throw std::invalid_argument("token");
		_stagedTokens.insert(token->getTokenId());
		_bytesStaged += byteCount;
	}

	void DmaStreamComputeTelemetryCounters::RecordTokenCanceled(std::shared_ptr<DmaStreamComputeToken> token)
	{
		if (!token) throw std::invalid_argument("token");
		_canceledTokens.insert(token->getTokenId());
	}

	void DmaStreamComputeTelemetryCounters::RecordTokenCommitted(std::shared_ptr<DmaStreamComputeToken> token)
	{
		if (!token) throw std::invalid_argument("token");
		_committedTokens.insert(token->getTokenId());
	}

	void DmaStreamComputeTelemetryCounters::RecordTokenFaulted(std::shared_ptr<DmaStreamComputeToken> token, DmaStreamComputeTokenFaultKind faultKind)
	{
		if (!token) throw std::invalid_argument("token");
		_faultedTokens.insert(token->getTokenId());
		_lastTokenFaultKind = faultKind;
		RecordTokenFaultFamily(faultKind);
	}

	void DmaStreamComputeTelemetryCounters::RecordTokenAdmissionFault(DmaStreamComputeValidationFault validationFault)
	{
		_computeRejected++;
		_lastValidationFault = validationFault;
		RecordValidationFaultFamily(validationFault);
	}

	void DmaStreamComputeTelemetryCounters::RecordRuntimeRead(unsigned long long byteCount)
	{
		_bytesRead += byteCount;
	}

	void DmaStreamComputeTelemetryCounters::RecordElementOperations(DmaStreamComputeOperationKind operation, unsigned long long elementCount)
	{
		unsigned long long count = elementCount == 0 ? 1ULL : elementCount;
		_elementOperations += count;

		switch (operation)
		{
			case DmaStreamComputeOperationKind::Copy:
				_copyOperations += count;
				break;
			case DmaStreamComputeOperationKind::Add:
				_addOperations += count;
				break;
			case DmaStreamComputeOperationKind::Mul:
				_mulOperations += count;
				break;
			case DmaStreamComputeOperationKind::Fma:
				_fmaOperations += count;
				break;
			case DmaStreamComputeOperationKind::Reduce:
				_reduceOperations += count;
				break;
		}
	}

	void DmaStreamComputeTelemetryCounters::RecordReplayEvidenceComparison(std::shared_ptr<DmaStreamComputeReplayEvidenceComparison> comparison)
	{
		if (comparison->CanReuse)
		{
			_replayEnvelopeReuseHits++;
			_lastReplayInvalidationReason = ReplayPhaseInvalidationReason::None;
			_lastReplayMismatchField = L"";
			return;
		}

		_replayEnvelopeRejects++;
		_computeRejected++;
		_lastReplayInvalidationReason = comparison->InvalidationReason;
		_lastReplayMismatchField = comparison->MismatchField;
	}

	void DmaStreamComputeTelemetryCounters::RecordValidationFaultFamily(DmaStreamComputeValidationFault fault)
	{
		switch (fault)
		{
			case DmaStreamComputeValidationFault::OwnerDomainFault:
				_ownerDomainFaults++;
				break;
			case DmaStreamComputeValidationFault::DescriptorCarrierDecodeFault:
			case DmaStreamComputeValidationFault::DescriptorReferenceLost:
				_unsupportedCarrierRejects++;
				_descriptorFaults++;
				break;
			case DmaStreamComputeValidationFault::QuotaAdmissionReject:
			case DmaStreamComputeValidationFault::TokenCapAdmissionReject:
				_quotaRejects++;
				break;
			case DmaStreamComputeValidationFault::BackpressureAdmissionReject:
				_backpressureRejects++;
				break;
			default:
				_descriptorFaults++;
				break;
		}
	}

	void DmaStreamComputeTelemetryCounters::RecordTokenFaultFamily(DmaStreamComputeTokenFaultKind faultKind)
	{
		if (faultKind == DmaStreamComputeTokenFaultKind::None)
		{
			return;
		}

		_tokenFaults++;
		switch (faultKind)
		{
			case DmaStreamComputeTokenFaultKind::DomainViolation:
			case DmaStreamComputeTokenFaultKind::OwnerContextViolation:
				_ownerDomainFaults++;
				break;
			case DmaStreamComputeTokenFaultKind::DmaDeviceFault:
				_deviceFaults++;
				break;
			case DmaStreamComputeTokenFaultKind::DescriptorDecodeFault:
			case DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation:
			case DmaStreamComputeTokenFaultKind::ExecutionDisabled:
				_descriptorFaults++;
				break;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeTokenAdmissionResult TryAdmit(DmaStreamComputeValidationResult validationResult, ulong tokenId, DmaStreamComputeTelemetryCounters? telemetry)
	std::shared_ptr<DmaStreamComputeTokenAdmissionResult> DmaStreamComputeAdmissionController::TryAdmit(std::shared_ptr<DmaStreamComputeValidationResult> validationResult, unsigned long long tokenId, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		return TryAdmit(validationResult, tokenId, DmaStreamComputePressurePolicy::Default, DmaStreamComputePressureSnapshot::Permissive(DmaStreamComputePressurePolicy::Default), telemetry);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeTokenAdmissionResult TryAdmit(DmaStreamComputeValidationResult validationResult, ulong tokenId, DmaStreamComputePressurePolicy pressurePolicy, DmaStreamComputePressureSnapshot pressureSnapshot, DmaStreamComputeTelemetryCounters? telemetry)
	std::shared_ptr<DmaStreamComputeTokenAdmissionResult> DmaStreamComputeAdmissionController::TryAdmit(std::shared_ptr<DmaStreamComputeValidationResult> validationResult, unsigned long long tokenId, std::shared_ptr<DmaStreamComputePressurePolicy> pressurePolicy, std::shared_ptr<DmaStreamComputePressureSnapshot> pressureSnapshot, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		if (!validationResult) throw std::invalid_argument("validationResult");

		if (!validationResult->IsDescriptorAbiAccepted)
		{
			return DmaStreamComputeToken::TryAdmit(validationResult, tokenId, telemetry);
		}

		if (telemetry != nullptr)
		{
			telemetry->RecordPressureSnapshot(pressurePolicy, pressureSnapshot);
		}
		DmaStreamComputePressureRejectKind rejectKind;
		DmaStreamComputeValidationFault validationFault;
		std::wstring message;
		if (!TryPassPressure(pressurePolicy, pressureSnapshot, rejectKind, validationFault, message))
		{
			if (telemetry != nullptr)
			{
				telemetry->RecordAdmissionReject(rejectKind, validationFault);
			}
			return DmaStreamComputeTokenAdmissionResult::TelemetryReject(validationFault, message);
		}

		return DmaStreamComputeTokenAdmissionResult::Accepted(std::make_shared<DmaStreamComputeToken>(validationResult->RequireDescriptorForAdmission(), tokenId, telemetry));
	}

	bool DmaStreamComputeAdmissionController::TryPassPressure(std::shared_ptr<DmaStreamComputePressurePolicy> policy, std::shared_ptr<DmaStreamComputePressureSnapshot> snapshot, DmaStreamComputePressureRejectKind &rejectKind, DmaStreamComputeValidationFault &validationFault, std::wstring &message)
	{
		rejectKind = DmaStreamComputePressureRejectKind::None;
		validationFault = DmaStreamComputeValidationFault::None;
		message = L"";

		if (!snapshot->Lane6Available)
		{
			return Reject(DmaStreamComputePressureRejectKind::Lane6Unavailable, DmaStreamComputeValidationFault::BackpressureAdmissionReject, L"DmaStreamCompute lane6 placement surface is unavailable; admission fails closed.", rejectKind, validationFault, message);
		}

		if (snapshot->DmaCreditsAvailable < policy->RequiredDmaCredits)
		{
			return Reject(DmaStreamComputePressureRejectKind::DmaCredits, DmaStreamComputeValidationFault::BackpressureAdmissionReject, L"DmaStreamCompute DMA credit pressure rejected admission before token creation.", rejectKind, validationFault, message);
		}

		if (snapshot->SrfCreditsAvailable < policy->RequiredSrfCredits)
		{
			return Reject(DmaStreamComputePressureRejectKind::SrfCredits, DmaStreamComputeValidationFault::BackpressureAdmissionReject, L"DmaStreamCompute stream-register credit pressure rejected admission before token creation.", rejectKind, validationFault, message);
		}

		if (snapshot->MemorySubsystemCreditsAvailable < policy->RequiredMemoryCredits)
		{
			return Reject(DmaStreamComputePressureRejectKind::MemorySubsystemPressure, DmaStreamComputeValidationFault::BackpressureAdmissionReject, L"DmaStreamCompute memory-subsystem pressure rejected admission before token creation.", rejectKind, validationFault, message);
		}

		if (snapshot->OutstandingTokens >= policy->OutstandingTokenCap)
		{
			return Reject(DmaStreamComputePressureRejectKind::OutstandingTokenCap, DmaStreamComputeValidationFault::TokenCapAdmissionReject, L"DmaStreamCompute outstanding token cap rejected admission before token creation.", rejectKind, validationFault, message);
		}

		return true;
	}

	bool DmaStreamComputeAdmissionController::Reject(DmaStreamComputePressureRejectKind reject, DmaStreamComputeValidationFault fault, const std::wstring &failureMessage, DmaStreamComputePressureRejectKind &rejectKind, DmaStreamComputeValidationFault &validationFault, std::wstring &message)
	{
		rejectKind = reject;
		validationFault = fault;
		message = failureMessage;
		return false;
	}
}
