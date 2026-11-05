#include "DmaStreamComputeTokenStore.h"
#include <stdexcept>
#include "../../Pipeline/MicroOps/Lane6DmaStream/DmaStreamComputeMicroOp.h"
#include "../../Pipeline/Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../Pipeline/Safety/SafetyVerifier.Types.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	DmaStreamComputeIssueAdmissionRequest::DmaStreamComputeIssueAdmissionRequest(
		std::shared_ptr<::YAKSys_Hybrid_CPU::Core::DmaStreamComputeMicroOp> carrier,
		std::shared_ptr<DmaStreamComputeOwnerGuardDecision> issueGuardDecision,
		DmaStreamComputeIssueAdmissionMetadata metadata,
		std::shared_ptr<DmaStreamComputePressurePolicy> pressurePolicy,
		std::shared_ptr<DmaStreamComputePressureSnapshot> pressureSnapshot,
		std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
		: Carrier(std::move(carrier)), IssueGuardDecision(std::move(issueGuardDecision)), Metadata(metadata),
		  PressurePolicy(std::move(pressurePolicy)), PressureSnapshot(std::move(pressureSnapshot)), Telemetry(std::move(telemetry))
	{
		ArgumentNullException::ThrowIfNull(Carrier);
		ArgumentNullException::ThrowIfNull(IssueGuardDecision);
		ArgumentNullException::ThrowIfNull(PressurePolicy);
		ArgumentNullException::ThrowIfNull(PressureSnapshot);
	}

	std::shared_ptr<DmaStreamComputeIssueAdmissionRequest> DmaStreamComputeIssueAdmissionRequest::ForLane6(
		std::shared_ptr<::YAKSys_Hybrid_CPU::Core::DmaStreamComputeMicroOp> carrier,
		DmaStreamComputeIssueAdmissionMetadata metadata,
		std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		ArgumentNullException::ThrowIfNull(carrier);
		auto policy = DmaStreamComputePressurePolicy::Default;
		return std::make_shared<DmaStreamComputeIssueAdmissionRequest>(carrier,
			carrier->getDescriptor()->OwnerGuardDecision, metadata, policy,
			DmaStreamComputePressureSnapshot::Permissive(policy), std::move(telemetry));
	}

	DmaStreamComputeIssueAdmissionResult::DmaStreamComputeIssueAdmissionResult(
		DmaStreamComputeIssueAdmissionStatus status, DmaStreamComputeIssueAdmissionRejectKind rejectKind,
		DmaStreamComputeValidationFault validationFault, DmaStreamComputePressureRejectKind pressureRejectKind,
		std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry,
		std::shared_ptr<DmaStreamComputeFaultRecord> fault, std::wstring message)
		: Status(status), RejectKind(rejectKind), ValidationFault(validationFault), PressureRejectKind(pressureRejectKind),
		  Entry(std::move(entry)), Token(Entry ? Entry->Token : nullptr), Fault(std::move(fault)), Message(std::move(message)),
		  Handle(Entry ? Entry->Handle : DmaStreamComputeTokenHandle{}),
		  IsAccepted(Status == DmaStreamComputeIssueAdmissionStatus::Accepted && Entry != nullptr),
		  HasAllocatedToken(IsAccepted && !Handle.IsDefault()),
		  RequiresRetireExceptionPublication(Fault && Fault->getRequiresRetireExceptionPublication())
	{
	}

	std::shared_ptr<DmaStreamComputeIssueAdmissionResult> DmaStreamComputeIssueAdmissionResult::Accepted(
		std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry)
	{
		ArgumentNullException::ThrowIfNull(entry);
		return std::shared_ptr<DmaStreamComputeIssueAdmissionResult>(new DmaStreamComputeIssueAdmissionResult(
			DmaStreamComputeIssueAdmissionStatus::Accepted, DmaStreamComputeIssueAdmissionRejectKind::None,
			DmaStreamComputeValidationFault::None, DmaStreamComputePressureRejectKind::None, std::move(entry), nullptr,
			L"DmaStreamCompute token allocated by explicit issue/admission store; execution remains gated."));
	}

	std::shared_ptr<DmaStreamComputeIssueAdmissionResult> DmaStreamComputeIssueAdmissionResult::Rejected(
		DmaStreamComputeIssueAdmissionRejectKind rejectKind, DmaStreamComputeValidationFault validationFault,
		DmaStreamComputePressureRejectKind pressureRejectKind, const std::wstring& message)
	{
		return std::shared_ptr<DmaStreamComputeIssueAdmissionResult>(new DmaStreamComputeIssueAdmissionResult(
			DmaStreamComputeIssueAdmissionStatus::Rejected, rejectKind, validationFault, pressureRejectKind, nullptr, nullptr, message));
	}

	std::shared_ptr<DmaStreamComputeIssueAdmissionResult> DmaStreamComputeIssueAdmissionResult::ArchitecturalFault(
		DmaStreamComputeIssueAdmissionRejectKind rejectKind, DmaStreamComputeValidationFault validationFault,
		std::shared_ptr<DmaStreamComputeFaultRecord> fault, const std::wstring& message)
	{
		ArgumentNullException::ThrowIfNull(fault);
		return std::shared_ptr<DmaStreamComputeIssueAdmissionResult>(new DmaStreamComputeIssueAdmissionResult(
			DmaStreamComputeIssueAdmissionStatus::ArchitecturalFault, rejectKind, validationFault,
			DmaStreamComputePressureRejectKind::None, nullptr, std::move(fault), message));
	}

	DmaStreamComputeTokenStore::DmaStreamComputeTokenStore() : DmaStreamComputeTokenStore(DmaStreamComputeTokenStoreOptions::Default)
	{
	}

	DmaStreamComputeTokenStore::DmaStreamComputeTokenStore(std::shared_ptr<DmaStreamComputeTokenStoreOptions> options)
		: Options(options ? std::move(options) : DmaStreamComputeTokenStoreOptions::Default)
	{
		_nextTokenId = getOptions()->FirstTokenId;
	}

	std::shared_ptr<DmaStreamComputeTokenStoreOptions> DmaStreamComputeTokenStore::getOptions() const
	{
		return Options;
	}

	int DmaStreamComputeTokenStore::getActiveTokenCount() const
	{
		return _activeTokens.size();
	}

	unsigned long long DmaStreamComputeTokenStore::getGeneration() const
	{
		return getOptions()->Generation;
	}

	std::shared_ptr<DmaStreamComputeIssueAdmissionResult> DmaStreamComputeTokenStore::TryAllocateAtIssueAdmission(std::shared_ptr<DmaStreamComputeIssueAdmissionRequest> request)
	{
		ArgumentNullException::ThrowIfNull(request);

		std::shared_ptr<DmaStreamComputeMicroOp> carrier = request->Carrier;
		std::shared_ptr<DmaStreamComputeDescriptor> descriptor = carrier->getDescriptor();
		if (request->Telemetry != nullptr)
		{
			request->Telemetry->RecordPressureSnapshot(request->PressurePolicy, request->PressureSnapshot);
		}

		if (!request->Metadata.IsLane6IssuePlacement() || carrier->getPlacement().RequiredSlotClass != SlotClass::DmaStreamClass)
		{
			return DmaStreamComputeIssueAdmissionResult::Rejected(DmaStreamComputeIssueAdmissionRejectKind::InvalidIssuePlacement, DmaStreamComputeValidationFault::BackpressureAdmissionReject, DmaStreamComputePressureRejectKind::Lane6Unavailable, L"DmaStreamCompute token store allocates only at materialized lane6 issue/admission.");
		}

		std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> guardReject;
		if (!TryValidateIssueGuard(descriptor, request->IssueGuardDecision, guardReject))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return guardReject!;
			return guardReject.value();
		}

		std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> pressureReject;
		if (!TryPassPressure(request, pressureReject))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return pressureReject!;
			return pressureReject.value();
		}

		if (_activeTokens.size() >= getOptions()->ActiveTokenCapacity)
		{
			if (request->Telemetry != nullptr)
			{
				request->Telemetry->RecordAdmissionReject(DmaStreamComputePressureRejectKind::OutstandingTokenCap, DmaStreamComputeValidationFault::TokenCapAdmissionReject);
			}
			return DmaStreamComputeIssueAdmissionResult::Rejected(DmaStreamComputeIssueAdmissionRejectKind::StoreCapacity, DmaStreamComputeValidationFault::TokenCapAdmissionReject, DmaStreamComputePressureRejectKind::OutstandingTokenCap, L"DmaStreamCompute token store capacity rejected issue/admission before token creation.");
		}

		unsigned long long ownerDomainTag = descriptor->OwnerBinding->OwnerDomainTag;
		if (GetDomainActiveCount(ownerDomainTag) >= getOptions()->PerDomainTokenQuota)
		{
			if (request->Telemetry != nullptr)
			{
				request->Telemetry->RecordAdmissionReject(DmaStreamComputePressureRejectKind::OutstandingTokenCap, DmaStreamComputeValidationFault::QuotaAdmissionReject);
			}
			return DmaStreamComputeIssueAdmissionResult::Rejected(DmaStreamComputeIssueAdmissionRejectKind::DomainQuota, DmaStreamComputeValidationFault::QuotaAdmissionReject, DmaStreamComputePressureRejectKind::OutstandingTokenCap, L"DmaStreamCompute per-domain token quota rejected issue/admission before token creation.");
		}

		std::shared_ptr<DmaStreamComputeTokenHandle> handle = AllocateHandle(descriptor->OwnerBinding);
		std::shared_ptr<DmaStreamComputeToken> token = std::make_shared<DmaStreamComputeToken>(descriptor, handle->TokenId, request->Telemetry);
		std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry = std::make_shared<DmaStreamComputeActiveTokenEntry>(*handle, token, request->Metadata, descriptor->DescriptorIdentityHash, descriptor->NormalizedFootprintHash);

		if (!_activeTokens.emplace(handle, entry).second)
			throw std::invalid_argument("Duplicate DMA stream compute token handle" );
		_activeTokensByDomain[ownerDomainTag] = GetDomainActiveCount(ownerDomainTag) + 1;
		return DmaStreamComputeIssueAdmissionResult::Accepted(entry);
	}

	bool DmaStreamComputeTokenStore::TryGet(std::shared_ptr<DmaStreamComputeTokenHandle> handle, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, std::optional<std::shared_ptr<DmaStreamComputeActiveTokenEntry>> &entry)
	{
		ArgumentNullException::ThrowIfNull(ownerBinding);
		entry = std::nullopt;

		if (!handle || handle->IsDefault() || !handle->MatchesOwner(ownerBinding)) return false;
		auto it = _activeTokens.find(handle);
		if (it == _activeTokens.end()) return false;
		entry = it->second;
		return true;
	}

	std::shared_ptr<DmaStreamComputeStatusQueryResult> DmaStreamComputeTokenStore::QueryStatusByTokenId(unsigned long long tokenId, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding)
	{
		ArgumentNullException::ThrowIfNull(ownerBinding);

		if (tokenId == 0)
		{
			return DmaStreamComputeStatusQueryResult::Rejected(DmaStreamComputeStatusQueryRejectKind::InvalidToken, L"DSC_STATUS requires a non-zero store-owned token id.");
		}

		bool sawTokenId = false;
		for (auto entry : _activeTokens)
		{
			if (entry.second->Handle.TokenId != tokenId)
			{
				continue;
			}

			sawTokenId = true;
			if (!entry.second->Handle.MatchesOwner(ownerBinding))
			{
				continue;
			}

			return DmaStreamComputeStatusQueryResult::Accepted(DmaStreamComputeTokenStatusSnapshot::FromEntry(entry.second));
		}

		return DmaStreamComputeStatusQueryResult::Rejected(sawTokenId ? DmaStreamComputeStatusQueryRejectKind::OwnerDomainMismatch : DmaStreamComputeStatusQueryRejectKind::InvalidToken, sawTokenId ? L"DSC_STATUS owner/domain guard rejected the token status query." : L"DSC_STATUS token id was not present in the active lane6 token store.");
	}

	bool DmaStreamComputeTokenStore::TryCancel(std::shared_ptr<DmaStreamComputeTokenHandle> handle, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, DmaStreamComputeTokenCancelReason reason)
	{
		std::optional<std::shared_ptr<DmaStreamComputeActiveTokenEntry>> entry;
		if (!TryGet(handle, ownerBinding, entry))
		{
			return false;
		}

		auto token = entry.value()->Token;
		auto state = token->getState();
		if (state == DmaStreamComputeTokenState::Committed || state == DmaStreamComputeTokenState::Faulted || state == DmaStreamComputeTokenState::Canceled) return false;
		token->Cancel(reason);
		return true;
	}

	std::vector<std::shared_ptr<DmaStreamComputeTokenHandle>> DmaStreamComputeTokenStore::SnapshotActiveHandles()
	{
		std::vector<std::shared_ptr<DmaStreamComputeTokenHandle>> handles;
		handles.reserve(_activeTokens.size());
		for (const auto& entry : _activeTokens) handles.push_back(std::make_shared<DmaStreamComputeTokenHandle>(*entry.first));
		return handles;
	}

	std::shared_ptr<DmaStreamComputeTokenHandle> DmaStreamComputeTokenStore::AllocateHandle(std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding)
	{
		unsigned long long tokenId = _nextTokenId++;
		if (_nextTokenId == 0)
		{
			_nextTokenId = 1;
		}

		return std::make_shared<DmaStreamComputeTokenHandle>(tokenId, ownerBinding->OwnerVirtualThreadId, ownerBinding->OwnerContextId, ownerBinding->OwnerCoreId, ownerBinding->OwnerPodId, ownerBinding->OwnerDomainTag, ownerBinding->DeviceId, getOptions()->Generation);
	}

	int DmaStreamComputeTokenStore::GetDomainActiveCount(unsigned long long ownerDomainTag)
	{
		auto it = _activeTokensByDomain.find(ownerDomainTag);
		return it == _activeTokensByDomain.end() ? 0 : it->second;
	}

	bool DmaStreamComputeTokenStore::TryValidateIssueGuard(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> issueGuardDecision, std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> &result)
	{
		result = std::nullopt;
		std::shared_ptr<DmaStreamComputeOwnerBinding> owner = descriptor->OwnerBinding;
		ArgumentNullException::ThrowIfNull(owner);
		const auto& context = issueGuardDecision->RuntimeOwnerContext;
		const auto& bound = issueGuardDecision->DescriptorOwnerBinding;

		if (!issueGuardDecision->IsAllowed || issueGuardDecision->DescriptorOwnerBinding == nullptr || (!owner || bound->OwnerVirtualThreadId != owner->OwnerVirtualThreadId || bound->OwnerContextId != owner->OwnerContextId || bound->OwnerCoreId != owner->OwnerCoreId || bound->OwnerPodId != owner->OwnerPodId || bound->OwnerDomainTag != owner->OwnerDomainTag || bound->DeviceId != owner->DeviceId) || issueGuardDecision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || issueGuardDecision->LegalityDecision.getAttemptedReplayCertificateReuse() || context.OwnerVirtualThreadId != owner->OwnerVirtualThreadId || context.OwnerContextId != owner->OwnerContextId || context.OwnerCoreId != owner->OwnerCoreId || context.OwnerPodId != owner->OwnerPodId || context.OwnerDomainTag != owner->OwnerDomainTag || context.DeviceId != owner->DeviceId)
		{
			DmaStreamComputeTokenFaultKind faultKind = context.DeviceId != owner->DeviceId ? DmaStreamComputeTokenFaultKind::DmaDeviceFault : DmaStreamComputeTokenFaultKind::DomainViolation;
			std::shared_ptr<DmaStreamComputeFaultRecord> fault = std::make_shared<DmaStreamComputeFaultRecord>(faultKind, L"DmaStreamCompute issue/admission guard rejected before token allocation.", descriptor->DescriptorReference.DescriptorAddress, false, owner->OwnerVirtualThreadId, owner->OwnerDomainTag, context.ActiveDomainCertificate);

			result = DmaStreamComputeIssueAdmissionResult::ArchitecturalFault(DmaStreamComputeIssueAdmissionRejectKind::OwnerDomainMismatch, DmaStreamComputeValidationFault::OwnerDomainFault, fault, fault->Message);
			return false;
		}

		return true;
	}

	bool DmaStreamComputeTokenStore::TryPassPressure(std::shared_ptr<DmaStreamComputeIssueAdmissionRequest> request, std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> &result)
	{
		result = std::nullopt;
		std::shared_ptr<DmaStreamComputePressurePolicy> policy = request->PressurePolicy;
		std::shared_ptr<DmaStreamComputePressureSnapshot> snapshot = request->PressureSnapshot;

		if (!snapshot->Lane6Available)
		{
			return RejectPressure(request, DmaStreamComputePressureRejectKind::Lane6Unavailable, DmaStreamComputeValidationFault::BackpressureAdmissionReject, L"DmaStreamCompute lane6 pressure rejected issue/admission before token creation.", result);
		}

		if (snapshot->DmaCreditsAvailable < policy->RequiredDmaCredits)
		{
			return RejectPressure(request, DmaStreamComputePressureRejectKind::DmaCredits, DmaStreamComputeValidationFault::BackpressureAdmissionReject, L"DmaStreamCompute DMA credit pressure rejected issue/admission before token creation.", result);
		}

		if (snapshot->SrfCreditsAvailable < policy->RequiredSrfCredits)
		{
			return RejectPressure(request, DmaStreamComputePressureRejectKind::SrfCredits, DmaStreamComputeValidationFault::BackpressureAdmissionReject, L"DmaStreamCompute SRF credit pressure rejected issue/admission before token creation.", result);
		}

		if (snapshot->MemorySubsystemCreditsAvailable < policy->RequiredMemoryCredits)
		{
			return RejectPressure(request, DmaStreamComputePressureRejectKind::MemorySubsystemPressure, DmaStreamComputeValidationFault::BackpressureAdmissionReject, L"DmaStreamCompute memory pressure rejected issue/admission before token creation.", result);
		}

		if (snapshot->OutstandingTokens >= policy->OutstandingTokenCap)
		{
			return RejectPressure(request, DmaStreamComputePressureRejectKind::OutstandingTokenCap, DmaStreamComputeValidationFault::TokenCapAdmissionReject, L"DmaStreamCompute outstanding-token pressure rejected issue/admission before token creation.", result);
		}

		return true;
	}

	bool DmaStreamComputeTokenStore::RejectPressure(std::shared_ptr<DmaStreamComputeIssueAdmissionRequest> request, DmaStreamComputePressureRejectKind pressureRejectKind, DmaStreamComputeValidationFault validationFault, const std::wstring &message, std::optional<std::shared_ptr<DmaStreamComputeIssueAdmissionResult>> &result)
	{
		if (request->Telemetry != nullptr)
		{
			request->Telemetry->RecordAdmissionReject(pressureRejectKind, validationFault);
		}
		result = DmaStreamComputeIssueAdmissionResult::Rejected(DmaStreamComputeIssueAdmissionRejectKind::Pressure, validationFault, pressureRejectKind, message);
		return false;
	}
}
