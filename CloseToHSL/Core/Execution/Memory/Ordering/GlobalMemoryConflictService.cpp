#include "GlobalMemoryConflictService.h"
#include "../../DmaStreamCompute/DmaStreamComputeDescriptor.h"
#include "../../DmaStreamCompute/DmaStreamComputeTokenStore.h"
#include "../../ExternalAccelerators/Descriptors/AcceleratorCommandDescriptor.h"
#include "../../ExternalAccelerators/Auth/AcceleratorOwnerDomainGuard.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::MemoryOrdering
{
	std::shared_ptr<GlobalMemoryFootprint> GlobalMemoryFootprint::FromDmaStreamCompute(
		std::shared_ptr<DmaStreamComputeDescriptor> descriptor, unsigned long long tokenId,
		unsigned long long issueAge, unsigned long long replayGeneration)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
		if (!descriptor->OwnerBinding) throw std::invalid_argument("descriptor.OwnerBinding");
		if (!descriptor->OwnerGuardDecision) throw std::invalid_argument("descriptor.OwnerGuardDecision");
		const auto& owner = descriptor->OwnerBinding;
		const auto& guardContext = descriptor->OwnerGuardDecision->RuntimeOwnerContext;
		auto identity = std::make_shared<GlobalMemoryParticipantIdentity>(owner->OwnerVirtualThreadId,
			owner->OwnerContextId, owner->OwnerCoreId, owner->OwnerPodId, owner->OwnerDomainTag,
			guardContext.ActiveDomainCertificate, owner->DeviceId, 0, false, tokenId, 0,
			descriptor->DescriptorIdentityHash, descriptor->NormalizedFootprintHash, 0, issueAge, replayGeneration);
		std::vector<GlobalMemoryFootprintRange> ranges;
		ranges.reserve(descriptor->NormalizedReadMemoryRanges.size() + descriptor->NormalizedWriteMemoryRanges.size());
		for (const auto& range : descriptor->NormalizedReadMemoryRanges) ranges.emplace_back(range.Address, range.Length, GlobalMemoryRangeAccessKind::Read);
		for (const auto& range : descriptor->NormalizedWriteMemoryRanges) ranges.emplace_back(range.Address, range.Length, GlobalMemoryRangeAccessKind::Write);
		return std::make_shared<GlobalMemoryFootprint>(GlobalMemoryParticipantKind::DmaStreamComputeToken,
			GlobalMemoryOperationKind::ReadWrite, GlobalMemoryAddressSpaceKind::Physical, std::move(identity), std::move(ranges));
	}

	std::shared_ptr<GlobalMemoryFootprint> GlobalMemoryFootprint::FromDmaStreamCompute(
		std::shared_ptr<DmaStreamComputeActiveTokenEntry> entry)
	{
		if (!entry) throw std::invalid_argument("entry");
		if (!entry->Token) throw std::invalid_argument("entry.Token");
		return FromDmaStreamCompute(entry->Token->getDescriptor(), entry->Handle.TokenId,
			entry->Metadata.IssueCycle, entry->Metadata.ReplayEpoch);
	}

	std::shared_ptr<GlobalMemoryFootprint> GlobalMemoryFootprint::FromL7Accelerator(
		std::shared_ptr<AcceleratorCommandDescriptor> descriptor, unsigned long long tokenId,
		unsigned long long submissionSequence, unsigned long long issueAge, unsigned long long replayGeneration)
	{
		if (!descriptor) throw std::invalid_argument("descriptor");
		if (!descriptor->OwnerBinding) throw std::invalid_argument("descriptor.OwnerBinding");
		if (!descriptor->OwnerGuardDecision) throw std::invalid_argument("descriptor.OwnerGuardDecision");
		const auto& owner = descriptor->OwnerBinding;
		const auto& guard = descriptor->OwnerGuardDecision;
		const auto& evidence = guard->Evidence;
		if (!evidence) throw std::logic_error("descriptor.OwnerGuardDecision.MappingEpoch is unavailable.");
		const auto mappingEpoch = guard->getMappingEpoch();
		auto identity = std::make_shared<GlobalMemoryParticipantIdentity>(owner->OwnerVirtualThreadId,
			owner->OwnerContextId, owner->OwnerCoreId, owner->OwnerPodId, owner->DomainTag,
			evidence ? evidence->ActiveDomainCertificate : 0,
			static_cast<unsigned int>(descriptor->AcceleratorId), mappingEpoch.Value, static_cast<bool>(evidence),
			tokenId, 0, descriptor->Identity.DescriptorIdentityHash,
			descriptor->Identity.NormalizedFootprintHash, submissionSequence, issueAge, replayGeneration);
		std::vector<GlobalMemoryFootprintRange> ranges;
		const auto& footprint = descriptor->NormalizedFootprint;
		ranges.reserve(footprint.SourceRanges.size() + footprint.DestinationRanges.size() + footprint.ScratchRanges.size());
		for (const auto& range : footprint.SourceRanges)
		{
			if (!range) throw std::invalid_argument("descriptor.NormalizedFootprint.SourceRanges contains null.");
			ranges.emplace_back(range->Address, range->Length, GlobalMemoryRangeAccessKind::Read);
		}
		for (const auto& range : footprint.DestinationRanges)
		{
			if (!range) throw std::invalid_argument("descriptor.NormalizedFootprint.DestinationRanges contains null.");
			ranges.emplace_back(range->Address, range->Length, GlobalMemoryRangeAccessKind::Write);
		}
		for (const auto& range : footprint.ScratchRanges)
		{
			if (!range) throw std::invalid_argument("descriptor.NormalizedFootprint.ScratchRanges contains null.");
			ranges.emplace_back(range->Address, range->Length,
				GlobalMemoryRangeAccessKind::Read | GlobalMemoryRangeAccessKind::Write);
		}
		return std::make_shared<GlobalMemoryFootprint>(GlobalMemoryParticipantKind::L7Accelerator,
			GlobalMemoryOperationKind::ReadWrite, GlobalMemoryAddressSpaceKind::IommuTranslated,
			std::move(identity), std::move(ranges));
	}

	GlobalMemoryConflictService::GlobalMemoryConflictService(GlobalMemoryConflictServiceMode mode) : Mode(mode)
	{
	}

	GlobalMemoryConflictServiceMode GlobalMemoryConflictService::getMode() const
	{
		return Mode;
	}

	int GlobalMemoryConflictService::getActiveFootprintCount() const
	{
		return _activeFootprints.size();
	}

	const std::vector<std::shared_ptr<GlobalMemoryConflictObservation>> GlobalMemoryConflictService::getObservations() const
	{
		return _observations;
	}

	std::shared_ptr<GlobalMemoryConflictService> GlobalMemoryConflictService::CreateAbsent()
	{
		return std::shared_ptr<GlobalMemoryConflictService>(new GlobalMemoryConflictService(GlobalMemoryConflictServiceMode::Absent));
	}

	std::shared_ptr<GlobalMemoryConflictService> GlobalMemoryConflictService::CreatePresentPassive()
	{
		return std::shared_ptr<GlobalMemoryConflictService>(new GlobalMemoryConflictService(GlobalMemoryConflictServiceMode::PresentPassive));
	}

	std::shared_ptr<GlobalMemoryConflictService> GlobalMemoryConflictService::CreatePresentEnforcingForFutureGate()
	{
		return std::shared_ptr<GlobalMemoryConflictService>(new GlobalMemoryConflictService(GlobalMemoryConflictServiceMode::PresentEnforcing));
	}

	std::shared_ptr<GlobalMemoryConflictObservation> GlobalMemoryConflictService::RegisterActive(std::shared_ptr<GlobalMemoryFootprint> footprint)
	{
		ArgumentNullException::ThrowIfNull(footprint);
		if (getMode() == GlobalMemoryConflictServiceMode::Absent)
		{
			return AbsentObservation(footprint);
		}

		std::shared_ptr<GlobalMemoryConflictObservation> observation = ClassifyAgainstActive(footprint);
		_observations.push_back(observation);

		if (getMode() == GlobalMemoryConflictServiceMode::PresentPassive || observation->DecisionKind == GlobalMemoryConflictDecisionKind::Accept)
		{
			_activeFootprints.push_back(footprint);
		}

		return observation;
	}

	std::shared_ptr<GlobalMemoryConflictObservation> GlobalMemoryConflictService::ObserveAccess(std::shared_ptr<GlobalMemoryFootprint> footprint)
	{
		ArgumentNullException::ThrowIfNull(footprint);
		if (getMode() == GlobalMemoryConflictServiceMode::Absent)
		{
			return AbsentObservation(footprint);
		}

		std::shared_ptr<GlobalMemoryConflictObservation> observation = ClassifyAgainstActive(footprint);
		_observations.push_back(observation);
		return observation;
	}

	int GlobalMemoryConflictService::ReleaseByTokenOrCommand(GlobalMemoryParticipantKind participantKind, unsigned long long tokenId, unsigned long long commandId)
	{
		if (getMode() == GlobalMemoryConflictServiceMode::Absent || (tokenId == 0 && commandId == 0))
		{
			return 0;
		}

		int removed = 0;
		for (int index = _activeFootprints.size() - 1; index >= 0; index--)
		{
			std::shared_ptr<GlobalMemoryFootprint> footprint = _activeFootprints[index];
			if (footprint->ParticipantKind != participantKind)
			{
				continue;
			}

			bool tokenMatches = tokenId != 0 && footprint->Identity->TokenId == tokenId;
			bool commandMatches = commandId != 0 && footprint->Identity->CommandId == commandId;
			if (!tokenMatches && !commandMatches)
			{
				continue;
			}

			_activeFootprints.erase(_activeFootprints.begin() + index);
			removed++;
		}

		return removed;
	}

	std::shared_ptr<GlobalMemoryConflictObservation> GlobalMemoryConflictService::ClassifyAgainstActive(std::shared_ptr<GlobalMemoryFootprint> incoming)
	{
		if (incoming->IsBoundaryObservation)
		{
			return BoundaryObservation(incoming);
		}

		if (!incoming->HasCompleteFootprintTruth)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Reject, GlobalMemoryConflictClass::IncompleteFootprintTruth, incoming, {}, nullptr, L"Global memory footprint is incomplete; future enforcing mode cannot treat it as safe overlap.");
		}

		for (int index = 0; index < _activeFootprints.size(); index++)
		{
			std::shared_ptr<GlobalMemoryConflictObservation> observation = ClassifyPair(_activeFootprints[index], incoming);
			if (observation->IsConflict)
			{
				return observation;
			}
		}

		return Observation(GlobalMemoryConflictDecisionKind::Accept, GlobalMemoryConflictClass::None, incoming, {}, nullptr, L"Global memory conflict service observed no overlapping active footprint.");
	}

	std::shared_ptr<GlobalMemoryConflictObservation> GlobalMemoryConflictService::ClassifyPair(std::shared_ptr<GlobalMemoryFootprint> active, std::shared_ptr<GlobalMemoryFootprint> incoming)
	{
		if (!active->HasCompleteFootprintTruth)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Reject, GlobalMemoryConflictClass::IncompleteFootprintTruth, incoming, active, nullptr, L"Active global memory footprint truth is incomplete.");
		}

		std::shared_ptr<GlobalMemoryFootprintRange> activeRange;
		std::shared_ptr<GlobalMemoryFootprintRange> incomingRange;
		std::shared_ptr<GlobalMemoryFootprintRange> overlap;
		if (!TryFindFirstOverlap(active, incoming, activeRange, incomingRange, overlap))
		{
			return Observation(GlobalMemoryConflictDecisionKind::Accept, GlobalMemoryConflictClass::None, incoming, {}, nullptr, L"Global memory conflict service observed no range overlap for this active footprint.");
		}

		if (active->AddressSpaceKind == GlobalMemoryAddressSpaceKind::Unresolved || incoming->AddressSpaceKind == GlobalMemoryAddressSpaceKind::Unresolved)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Serialize, GlobalMemoryConflictClass::UnresolvedAddressSpaceRequiresPolicy, incoming, active, overlap, L"Unresolved address-space kind requires future serialization, rejection, or fault policy.");
		}

		if (active->AddressSpaceKind != incoming->AddressSpaceKind)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Serialize, GlobalMemoryConflictClass::AddressSpaceMismatchRequiresPolicy, incoming, active, overlap, L"Address-space kind mismatch requires a future translation/domain policy before overlap can execute.");
		}

		if (active->RequiresMappingEpochBeforeEnforcing || incoming->RequiresMappingEpochBeforeEnforcing)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Reject, GlobalMemoryConflictClass::MappingEpochRequiredForEnforcingMode, incoming, active, overlap, L"Translated or unresolved footprint requires a mapping epoch before enforcing mode can accept overlap.");
		}

		if (active->Identity->MappingEpochKnown && incoming->Identity->MappingEpochKnown && active->Identity->MappingEpoch != incoming->Identity->MappingEpoch)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Reject, GlobalMemoryConflictClass::MappingEpochMismatchRequiresPolicy, incoming, active, overlap, L"Mapping epoch mismatch requires future reject, serialize, or fault policy.");
		}

		if (active->Identity->MemoryDomainTag != incoming->Identity->MemoryDomainTag)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Serialize, GlobalMemoryConflictClass::DomainMismatchRequiresPolicy, incoming, active, overlap, L"Overlapping footprints carry different memory domains; future policy must serialize, reject, or fault explicitly.");
		}

		bool activeWrites = activeRange->IsWrite;
		bool incomingWrites = incomingRange->IsWrite;
		bool activeReads = activeRange->IsRead;
		bool incomingReads = incomingRange->IsRead;
		bool atomicOrReservation = active->OperationKind == GlobalMemoryOperationKind::Atomic || active->OperationKind == GlobalMemoryOperationKind::Reservation || incoming->OperationKind == GlobalMemoryOperationKind::Atomic || incoming->OperationKind == GlobalMemoryOperationKind::Reservation || activeRange->IsAtomicOrReservation || incomingRange->IsAtomicOrReservation;

		if (atomicOrReservation && (activeWrites || incomingWrites))
		{
			return Observation(GlobalMemoryConflictDecisionKind::Stall, GlobalMemoryConflictClass::AtomicReservationOverlap, incoming, active, overlap, L"Atomic or reservation footprint overlaps a write-capable participant.");
		}

		if (activeWrites && incomingWrites)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Serialize, GlobalMemoryConflictClass::WriteWriteOverlap, incoming, active, overlap, L"Two write-capable global memory footprints overlap.");
		}

		if (activeWrites && incomingReads)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Serialize, GlobalMemoryConflictClass::ReadAfterWriteOverlap, incoming, active, overlap, L"Incoming read overlaps an active write footprint.");
		}

		if (activeReads && incomingWrites)
		{
			return Observation(GlobalMemoryConflictDecisionKind::Serialize, GlobalMemoryConflictClass::WriteAfterReadOverlap, incoming, active, overlap, L"Incoming write overlaps an active read footprint.");
		}

		return Observation(GlobalMemoryConflictDecisionKind::Accept, GlobalMemoryConflictClass::None, incoming, {}, nullptr, L"Read-only overlap does not create a Phase05 conflict observation.");
	}

	bool GlobalMemoryConflictService::TryFindFirstOverlap(std::shared_ptr<GlobalMemoryFootprint> left, std::shared_ptr<GlobalMemoryFootprint> right, std::shared_ptr<GlobalMemoryFootprintRange> &leftRange, std::shared_ptr<GlobalMemoryFootprintRange> &rightRange, std::shared_ptr<GlobalMemoryFootprintRange> &overlap)
	{
		leftRange = nullptr;
		rightRange = nullptr;
		overlap = nullptr;
		bool foundReadOnlyOverlap = false;
		std::shared_ptr<GlobalMemoryFootprintRange> savedLeftRange = nullptr;
		std::shared_ptr<GlobalMemoryFootprintRange> savedRightRange = nullptr;
		std::shared_ptr<GlobalMemoryFootprintRange> savedOverlap = nullptr;

		for (int leftIndex = 0; leftIndex < left->Ranges.Count; leftIndex++)
		{
			std::shared_ptr<GlobalMemoryFootprintRange> leftCandidate = left->Ranges[leftIndex];
			for (int rightIndex = 0; rightIndex < right->Ranges.Count; rightIndex++)
			{
				std::shared_ptr<GlobalMemoryFootprintRange> rightCandidate = right->Ranges[rightIndex];
				if (!leftCandidate->Overlaps(*rightCandidate))
				{
					continue;
				}

				unsigned long long start = std::max(leftCandidate->Address, rightCandidate->Address);
				unsigned long long end = std::min(leftCandidate->Address + leftCandidate->Length, rightCandidate->Address + rightCandidate->Length);
				leftRange = leftCandidate;
				rightRange = rightCandidate;
				overlap = std::make_shared<GlobalMemoryFootprintRange>(start, end - start, leftCandidate->AccessKind | rightCandidate->AccessKind);

				bool writeOrAtomicRelevant = leftCandidate->IsWrite || rightCandidate->IsWrite || leftCandidate->IsAtomicOrReservation || rightCandidate->IsAtomicOrReservation;
				if (!writeOrAtomicRelevant)
				{
					foundReadOnlyOverlap = true;
					savedLeftRange = leftRange;
					savedRightRange = rightRange;
					savedOverlap = overlap;
					continue;
				}

				return true;
			}
		}

		if (!foundReadOnlyOverlap)
		{
			return false;
		}

		leftRange = savedLeftRange;
		rightRange = savedRightRange;
		overlap = savedOverlap;
		return true;
	}

	std::shared_ptr<GlobalMemoryConflictObservation> GlobalMemoryConflictService::BoundaryObservation(std::shared_ptr<GlobalMemoryFootprint> incoming)
	{
		GlobalMemoryConflictDecisionKind decisionKind = GlobalMemoryConflictDecisionKind::Serialize;
		if (incoming->OperationKind == GlobalMemoryOperationKind::Poll)
		{
			decisionKind = GlobalMemoryConflictDecisionKind::Accept;
		}
		else if (incoming->OperationKind == GlobalMemoryOperationKind::Wait)
		{
			decisionKind = GlobalMemoryConflictDecisionKind::Stall;
		}

		return Observation(decisionKind, GlobalMemoryConflictClass::FenceWaitPollFutureGated, incoming, {}, nullptr, L"Fence, wait, and poll are Phase05 future-gated observations only and cannot publish memory.");
	}

	std::shared_ptr<GlobalMemoryConflictObservation> GlobalMemoryConflictService::Observation(GlobalMemoryConflictDecisionKind decisionKind, GlobalMemoryConflictClass conflictClass, std::shared_ptr<GlobalMemoryFootprint> incoming, std::optional<std::shared_ptr<GlobalMemoryFootprint>> conflicting, std::shared_ptr<GlobalMemoryFootprintRange> conflictRange, const std::wstring &message)
	{
		const std::shared_ptr<GlobalMemoryFootprint> conflictingFootprint = conflicting.has_value() ? conflicting.value() : nullptr;
		const GlobalMemoryFootprintRange rangeValue = conflictRange ? *conflictRange : GlobalMemoryFootprintRange{};
		return std::make_shared<GlobalMemoryConflictObservation>(getMode(), decisionKind, conflictClass, incoming, conflictingFootprint, rangeValue, message);
	}

	std::shared_ptr<GlobalMemoryConflictObservation> GlobalMemoryConflictService::AbsentObservation(std::shared_ptr<GlobalMemoryFootprint> incoming)
	{
		return std::make_shared<GlobalMemoryConflictObservation>(GlobalMemoryConflictServiceMode::Absent, GlobalMemoryConflictDecisionKind::Accept, GlobalMemoryConflictClass::ServiceAbsent, incoming, nullptr, GlobalMemoryFootprintRange{}, L"Global memory conflict service is absent; current scalar/DSC/L7 behavior is unchanged.");
	}
}





