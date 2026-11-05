#include "ExternalAcceleratorConflictManager.h"
#include "../Telemetry/AcceleratorTelemetry.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"
#include "../Tokens/AcceleratorTokenStore.h"
#include "../../../Pipeline/Safety/SafetyVerifier.Types.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts
{
	namespace
	{
		const wchar_t *TokenStateName(AcceleratorTokenState state) noexcept
		{
			switch (state)
			{
			case AcceleratorTokenState::Created: return L"Created";
			case AcceleratorTokenState::Validated: return L"Validated";
			case AcceleratorTokenState::Queued: return L"Queued";
			case AcceleratorTokenState::Running: return L"Running";
			case AcceleratorTokenState::DeviceComplete: return L"DeviceComplete";
			case AcceleratorTokenState::CommitPending: return L"CommitPending";
			case AcceleratorTokenState::Committed: return L"Committed";
			case AcceleratorTokenState::Faulted: return L"Faulted";
			case AcceleratorTokenState::Canceled: return L"Canceled";
			case AcceleratorTokenState::TimedOut: return L"TimedOut";
			case AcceleratorTokenState::Abandoned: return L"Abandoned";
			default: return L"Unknown";
			}
		}
	}

	int AcceleratorActiveFootprintTable::getCount() const
	{
		return static_cast<int>(_reservations.size());
	}

	const std::vector<std::shared_ptr<AcceleratorFootprintReservation>> AcceleratorActiveFootprintTable::getActiveReservations() const
	{
		std::vector<std::shared_ptr<AcceleratorFootprintReservation>> reservations;
		reservations.reserve(_reservations.size());
		for (const auto &entry : _reservations)
			reservations.push_back(entry.second);
		return reservations;
	}

	bool AcceleratorActiveFootprintTable::TryGet(std::shared_ptr<AcceleratorTokenHandle> handle, std::optional<std::shared_ptr<AcceleratorFootprintReservation>> &reservation)
	{
		if (!handle || !handle->IsValid)
		{
			reservation = std::nullopt;
			return false;
		}

		auto reservationsIterator = _reservations.find(handle->Value);
		if (reservationsIterator == _reservations.end())
		{
			reservation = std::nullopt;
			return false;
		}
		reservation = reservationsIterator->second;
		return true;
	}

	std::shared_ptr<AcceleratorConflictDecision> AcceleratorActiveFootprintTable::TryReserve(std::shared_ptr<AcceleratorFootprintReservation> reservation)
	{
		ArgumentNullException::ThrowIfNull(reservation);

		if (!reservation->HasCompleteFootprintTruth)
		{
			return AcceleratorConflictDecision::Reject(AcceleratorConflictClass::IncompleteFootprintTruth, AcceleratorTokenFaultCode::ConflictRejected, L"L7-SDC active footprint reservation rejected because normalized footprint truth is incomplete.", nullptr, false, reservation->TokenHandle, reservation->ReservationGuardDecision);
		}

		if (_reservations.contains(reservation->TokenHandle->Value))
		{
			std::shared_ptr<AcceleratorFootprintReservation> existing = _reservations[reservation->TokenHandle->Value];
			if (existing->TokenId == reservation->TokenId && existing->DescriptorIdentityHash == reservation->DescriptorIdentityHash && existing->NormalizedFootprintHash == reservation->NormalizedFootprintHash && existing->OwnerBinding->Equals(reservation->OwnerBinding))
			{
				return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::SubmitReservation, L"L7-SDC active footprint reservation already exists for the guarded token.", reservation, reservation->ReservationGuardDecision);
			}

			return AcceleratorConflictDecision::Reject(AcceleratorConflictClass::AcceleratorWriteWriteOverlap, AcceleratorTokenFaultCode::TokenHandleNotAuthority, L"L7-SDC active footprint table observed an opaque handle collision with different descriptor or footprint evidence; token handle identity is not conflict authority.", FirstRangeOrDefault(existing->DestinationRanges), true, existing->TokenHandle, reservation->ReservationGuardDecision);
		}

		for (const auto &active : _reservations)
		{
			std::shared_ptr<AcceleratorMemoryRange> overlap;
			if (FirstWriteWriteOverlap(active.second, reservation, overlap))
			{
				return AcceleratorConflictDecision::Reject(AcceleratorConflictClass::AcceleratorWriteWriteOverlap, AcceleratorTokenFaultCode::ConflictRejected, L"L7-SDC submit reservation rejected because two active accelerator tokens would write the same region.", overlap, true, active.second->TokenHandle, reservation->ReservationGuardDecision);
			}
		}

		_reservations.emplace(reservation->TokenHandle->Value, reservation);
		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::SubmitReservation, L"L7-SDC active footprint reservation accepted after guard-backed conflict validation.", reservation, reservation->ReservationGuardDecision);
	}

	bool AcceleratorActiveFootprintTable::Release(std::shared_ptr<AcceleratorTokenHandle> handle)
	{
		return handle && handle->IsValid && _reservations.erase(handle->Value) != 0;
	}

	std::shared_ptr<AcceleratorConflictDecision> AcceleratorActiveFootprintTable::FirstActiveDestinationOverlap(std::shared_ptr<AcceleratorMemoryRange> range, AcceleratorConflictClass conflictClass, const std::wstring &message, std::function<std::optional<std::shared_ptr<AcceleratorGuardDecision>>(std::shared_ptr<AcceleratorFootprintReservation>)> guardSelector)
	{
		for (const auto &entry : _reservations)
		{
			const auto &reservation = entry.second;
			std::shared_ptr<AcceleratorMemoryRange> overlap;
			if (!reservation->DestinationOverlaps(range, overlap))
			{
				continue;
			}

			std::optional<std::shared_ptr<AcceleratorGuardDecision>> guardDecision = guardSelector(reservation);
			if (guardDecision && guardDecision.value() && !guardDecision.value()->IsAllowed)
			{
				return AcceleratorConflictDecision::Reject(AcceleratorConflictClass::VmDomainMappingTransitionWhileTokenActive, AcceleratorTokenStore::MapGuardFault(guardDecision.value()->Fault), L"L7-SDC active conflict notification observed owner/domain or epoch drift before overlap handling. " + guardDecision.value()->Message, overlap, true, reservation->TokenHandle, guardDecision.value());
			}

			return AcceleratorConflictDecision::Serialize(conflictClass, reservation, guardDecision && guardDecision.value() ? guardDecision.value() : reservation->ReservationGuardDecision, overlap, message, true);
		}

		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::None, L"L7-SDC conflict table observed no active accelerator destination overlap.");
	}

	bool AcceleratorActiveFootprintTable::FirstWriteWriteOverlap(std::shared_ptr<AcceleratorFootprintReservation> active, std::shared_ptr<AcceleratorFootprintReservation> incoming, std::shared_ptr<AcceleratorMemoryRange> &overlap)
	{
		overlap = nullptr;
		for (std::size_t index = 0; index < incoming->DestinationRanges.size(); index++)
		{
			if (AcceleratorFootprintReservation::FirstOverlap(active->DestinationRanges, incoming->DestinationRanges[index], overlap))
			{
				return true;
			}
		}

		return false;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static AcceleratorMemoryRange FirstRangeOrDefault(IReadOnlyList<AcceleratorMemoryRange> ranges)
	std::shared_ptr<AcceleratorMemoryRange> AcceleratorActiveFootprintTable::FirstRangeOrDefault(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges)
	{
		return !ranges.empty() ? ranges.front() : nullptr;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ExternalAcceleratorConflictManager(AcceleratorTelemetry? telemetry = null)
	ExternalAcceleratorConflictManager::ExternalAcceleratorConflictManager(std::shared_ptr<AcceleratorTelemetry> telemetry) : _telemetry(telemetry)
	{
	}

	std::shared_ptr<AcceleratorActiveFootprintTable> ExternalAcceleratorConflictManager::getActiveFootprints() const
	{
		return _activeFootprints;
	}

	int ExternalAcceleratorConflictManager::getActiveReservationCount() const
	{
		return _activeFootprints->getCount();
	}

	int ExternalAcceleratorConflictManager::getSrfWarmWindowCount() const
	{
		return static_cast<int>(_srfWarmWindows.size());
	}

	int ExternalAcceleratorConflictManager::getAssistIngressWindowCount() const
	{
		return static_cast<int>(_assistIngressWindows.size());
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::TryReserveOnSubmit(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateTokenAndDescriptorGuard(token, currentGuardEvidence, AcceleratorGuardSurface::SubmitAdmission);
		if (!guardDecision->IsAllowed)
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::SubmitReservation, AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"L7-SDC submit footprint reservation requires fresh owner/domain and mapping/IOMMU authority. " + guardDecision->Message, nullptr, false, nullptr, guardDecision));
		}

		std::shared_ptr<AcceleratorFootprintReservation> reservation = std::make_shared<AcceleratorFootprintReservation>(token, guardDecision);
		if (!reservation->HasCompleteFootprintTruth)
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::IncompleteFootprintTruth, AcceleratorTokenFaultCode::ConflictRejected, L"L7-SDC submit footprint reservation rejected because descriptor footprint truth is incomplete.", nullptr, false, token->getHandle(), guardDecision));
		}

		std::shared_ptr<AcceleratorMemoryRange> srfOverlap;
		if (FirstWindowOverlap(reservation->DestinationRanges, _srfWarmWindows, srfOverlap))
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::AcceleratorWriteOverlapsSrfWarmedWindow, AcceleratorTokenFaultCode::ConflictRejected, L"L7-SDC submit rejected because accelerator writes overlap an SRF warmed window; v1 does not relax this dependency.", srfOverlap, true, token->getHandle(), guardDecision));
		}

		std::shared_ptr<AcceleratorMemoryRange> assistOverlap;
		if (FirstWindowOverlap(reservation->DestinationRanges, _assistIngressWindows, assistOverlap))
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::AssistIngressOverlapsAcceleratorWrite, AcceleratorTokenFaultCode::ConflictRejected, L"L7-SDC submit rejected because accelerator writes overlap assist ingress warm evidence; assist evidence is not device authority.", assistOverlap, true, token->getHandle(), guardDecision));
		}

		return RecordConflictDecision(_activeFootprints->TryReserve(reservation));
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::NotifyCpuLoad(std::shared_ptr<AcceleratorMemoryRange> loadRange, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		if (!AcceleratorFootprintReservation::IsRangeWellFormed(loadRange))
		{
			return RecordConflictDecision(RejectIncompleteRange(AcceleratorConflictClass::CpuLoadOverlapsAcceleratorWrite, loadRange, false));
		}

		return RecordConflictDecision(_activeFootprints->FirstActiveDestinationOverlap(loadRange, AcceleratorConflictClass::CpuLoadOverlapsAcceleratorWrite, L"CPU load overlaps an active accelerator write footprint; v1 requires serialization or rejection before the load observes memory.", [&] (std::shared_ptr<AcceleratorFootprintReservation> reservation)
		{
			return ValidateReservationGuard(reservation, currentGuardEvidence);
		}));
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::NotifyCpuStore(std::shared_ptr<AcceleratorMemoryRange> storeRange, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		if (!AcceleratorFootprintReservation::IsRangeWellFormed(storeRange))
		{
			return RecordConflictDecision(RejectIncompleteRange(AcceleratorConflictClass::CpuStoreOverlapsAcceleratorWrite, storeRange, true));
		}

		for (auto reservation : _activeFootprints->getActiveReservations())
		{
			std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateReservationGuard(reservation, currentGuardEvidence);
			if (!guardDecision->IsAllowed)
			{
				return RecordConflictDecision(RejectDrift(reservation, guardDecision, storeRange));
			}

			std::shared_ptr<AcceleratorMemoryRange> writeOverlap;
			if (reservation->DestinationOverlaps(storeRange, writeOverlap))
			{
				return RecordConflictDecision(AcceleratorConflictDecision::Serialize(AcceleratorConflictClass::CpuStoreOverlapsAcceleratorWrite, reservation, guardDecision, writeOverlap, L"CPU store overlaps an active accelerator write footprint; v1 requires serialization or rejection before either write becomes visible."));
			}

			std::shared_ptr<AcceleratorMemoryRange> readOverlap;
			if (reservation->SourceOverlaps(storeRange, readOverlap))
			{
				return RecordConflictDecision(AcceleratorConflictDecision::Serialize(AcceleratorConflictClass::CpuStoreOverlapsAcceleratorRead, reservation, guardDecision, readOverlap, L"CPU store overlaps an active accelerator read footprint; v1 requires serialization or rejection before the source image can drift.", true));
			}
		}

		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::None, L"CPU store observed no active L7-SDC source/destination overlap.");
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorConflictDecision NotifyDmaStreamComputeAdmission(IReadOnlyList<AcceleratorMemoryRange> readRanges, IReadOnlyList<AcceleratorMemoryRange> writeRanges, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence)
	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::NotifyDmaStreamComputeAdmission(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &readRanges, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &writeRanges, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		if (!AreRangesWellFormed(readRanges) || !AreRangesWellFormed(writeRanges))
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::IncompleteFootprintTruth, AcceleratorTokenFaultCode::ConflictRejected, L"DmaStreamCompute admission conflict notification requires complete normalized read/write footprint evidence."));
		}

		for (auto reservation : _activeFootprints->getActiveReservations())
		{
			std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateReservationGuard(reservation, currentGuardEvidence);
			if (!guardDecision->IsAllowed)
			{
				return RecordConflictDecision(RejectDrift(reservation, guardDecision, FirstRangeOrDefault(writeRanges)));
			}

			std::shared_ptr<AcceleratorMemoryRange> readOverlap;
			if (FirstWindowOverlap(reservation->DestinationRanges, readRanges, readOverlap) || FirstWindowOverlap(reservation->DestinationRanges, writeRanges, readOverlap))
			{
				return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::DmaStreamComputeOverlapsAcceleratorWrite, AcceleratorTokenFaultCode::ConflictRejected, L"DmaStreamCompute lane6 admission overlaps an active L7-SDC accelerator write footprint; v1 rejects one side and does not route either contour as fallback.", readOverlap, true, reservation->TokenHandle, guardDecision));
			}
		}

		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::None, L"DmaStreamCompute admission observed no active L7-SDC accelerator write overlap.");
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::NotifySrfWarmWindow(std::shared_ptr<AcceleratorMemoryRange> warmedWindow, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		if (!AcceleratorFootprintReservation::IsRangeWellFormed(warmedWindow))
		{
			return RecordConflictDecision(RejectIncompleteRange(AcceleratorConflictClass::AcceleratorWriteOverlapsSrfWarmedWindow, warmedWindow, false));
		}

		std::shared_ptr<AcceleratorConflictDecision> activeOverlap = _activeFootprints->FirstActiveDestinationOverlap(warmedWindow, AcceleratorConflictClass::AcceleratorWriteOverlapsSrfWarmedWindow, L"SRF warm window overlaps an active accelerator write footprint; v1 rejects the warm/accelerator overlap instead of treating warm evidence as authority.", [&] (std::shared_ptr<AcceleratorFootprintReservation> reservation)
		{
			return ValidateReservationGuard(reservation, currentGuardEvidence);
		});
		if (!activeOverlap->IsAccepted)
		{
			const auto faultRange = activeOverlap->Fault ? std::make_shared<AcceleratorMemoryRange>(AcceleratorMemoryRange{activeOverlap->Fault->FaultAddress, warmedWindow->Length}) : warmedWindow;
			const auto conflictingHandle = activeOverlap->Fault ? activeOverlap->Fault->ConflictingTokenHandle : nullptr;
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::AcceleratorWriteOverlapsSrfWarmedWindow, activeOverlap->TokenFaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::ConflictRejected : activeOverlap->TokenFaultCode, activeOverlap->Message, faultRange, true, conflictingHandle, activeOverlap->GuardDecision));
		}

		_srfWarmWindows.push_back(warmedWindow);
		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::AcceleratorWriteOverlapsSrfWarmedWindow, L"SRF warm window recorded as non-authoritative conflict evidence for future L7-SDC submit checks.");
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::NotifyAssistIngressWindow(std::shared_ptr<AcceleratorMemoryRange> ingressWindow, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		if (!AcceleratorFootprintReservation::IsRangeWellFormed(ingressWindow))
		{
			return RecordConflictDecision(RejectIncompleteRange(AcceleratorConflictClass::AssistIngressOverlapsAcceleratorWrite, ingressWindow, false));
		}

		std::shared_ptr<AcceleratorConflictDecision> activeOverlap = _activeFootprints->FirstActiveDestinationOverlap(ingressWindow, AcceleratorConflictClass::AssistIngressOverlapsAcceleratorWrite, L"Assist ingress window overlaps an active accelerator write footprint; v1 rejects the overlap and assist evidence remains non-authoritative.", [&] (std::shared_ptr<AcceleratorFootprintReservation> reservation)
		{
			return ValidateReservationGuard(reservation, currentGuardEvidence);
		});
		if (!activeOverlap->IsAccepted)
		{
			const auto faultRange = activeOverlap->Fault ? std::make_shared<AcceleratorMemoryRange>(AcceleratorMemoryRange{activeOverlap->Fault->FaultAddress, ingressWindow->Length}) : ingressWindow;
			const auto conflictingHandle = activeOverlap->Fault ? activeOverlap->Fault->ConflictingTokenHandle : nullptr;
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::AssistIngressOverlapsAcceleratorWrite, activeOverlap->TokenFaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::ConflictRejected : activeOverlap->TokenFaultCode, activeOverlap->Message, faultRange, true, conflictingHandle, activeOverlap->GuardDecision));
		}

		_assistIngressWindows.push_back(ingressWindow);
		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::AssistIngressOverlapsAcceleratorWrite, L"Assist ingress window recorded as non-authoritative conflict evidence for future L7-SDC submit checks.");
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::ValidateBeforeCommit(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateTokenAndDescriptorGuard(token, currentGuardEvidence, AcceleratorGuardSurface::Commit);
		if (!guardDecision->IsAllowed)
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Faulted(AcceleratorConflictClass::CommitConflictValidationRejected, AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"L7-SDC commit conflict validation requires fresh owner/domain and mapping/IOMMU authority. " + guardDecision->Message, FirstRangeOrDefault(token->getDescriptor()->NormalizedFootprint.DestinationRanges), true, token->getHandle(), guardDecision));
		}

		std::optional<std::shared_ptr<AcceleratorFootprintReservation>> reservation;
		if (!_activeFootprints->TryGet(token->getHandle(), reservation) || !reservation || !reservation.value())
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Faulted(AcceleratorConflictClass::CommitFootprintReservationMissing, AcceleratorTokenFaultCode::CommitConflictRejected, L"L7-SDC commit rejected because the conflict manager has no active footprint reservation for the token; conflict success is not commit authority.", FirstRangeOrDefault(token->getDescriptor()->NormalizedFootprint.DestinationRanges), true, token->getHandle(), guardDecision));
		}

		const auto reservedFootprint = reservation.value();
		if (!reservedFootprint->IsBoundTo(token))
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Faulted(AcceleratorConflictClass::CommitConflictValidationRejected, AcceleratorTokenFaultCode::CommitConflictRejected, L"L7-SDC commit rejected because active footprint truth no longer matches the token-bound descriptor identity and footprint hash.", FirstRangeOrDefault(token->getDescriptor()->NormalizedFootprint.DestinationRanges), true, token->getHandle(), guardDecision));
		}

		std::shared_ptr<AcceleratorMemoryRange> srfOverlap;
		if (FirstWindowOverlap(reservedFootprint->DestinationRanges, _srfWarmWindows, srfOverlap))
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Faulted(AcceleratorConflictClass::AcceleratorWriteOverlapsSrfWarmedWindow, AcceleratorTokenFaultCode::CommitConflictRejected, L"L7-SDC commit rejected because a recorded SRF warmed window overlaps the token write footprint.", srfOverlap, true, token->getHandle(), guardDecision));
		}

		std::shared_ptr<AcceleratorMemoryRange> assistOverlap;
		if (FirstWindowOverlap(reservedFootprint->DestinationRanges, _assistIngressWindows, assistOverlap))
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Faulted(AcceleratorConflictClass::AssistIngressOverlapsAcceleratorWrite, AcceleratorTokenFaultCode::CommitConflictRejected, L"L7-SDC commit rejected because a recorded assist ingress window overlaps the token write footprint.", assistOverlap, true, token->getHandle(), guardDecision));
		}

		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::CommitConflictValidationRejected, L"L7-SDC conflict manager validated the active footprint before commit; Phase 08 coordinator preconditions still own publication authority.", reservedFootprint, guardDecision);
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::ReleaseTokenFootprint(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateTokenAndDescriptorGuard(token, currentGuardEvidence, AcceleratorGuardSurface::MappingEpochValidation);
		if (!guardDecision->IsAllowed)
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::VmDomainMappingTransitionWhileTokenActive, AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"L7-SDC footprint release requires current guard-plane owner/domain and epoch authority. " + guardDecision->Message, FirstRangeOrDefault(token->getDescriptor()->NormalizedFootprint.DestinationRanges), true, token->getHandle(), guardDecision));
		}

		bool released = _activeFootprints->Release(token->getHandle());
		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::None, released ? L"L7-SDC active footprint reservation released after guarded token resolution." : L"L7-SDC active footprint release observed no reservation; no authority was inferred from the token handle.", nullptr, guardDecision);
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::InvalidateSrfCacheOnCommit(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);

		if (token->getState() != AcceleratorTokenState::Committed)
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::CommitConflictValidationRejected, AcceleratorTokenFaultCode::CommitCoordinatorRequired, std::format(L"L7-SDC conflict-manager SRF/cache cleanup requires a Phase 08 committed token, but token is {}.", TokenStateName(token->getState())), FirstRangeOrDefault(token->getDescriptor()->NormalizedFootprint.DestinationRanges), true, token->getHandle()));
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateTokenAndDescriptorGuard(token, currentGuardEvidence, AcceleratorGuardSurface::Commit);
		if (!guardDecision->IsAllowed)
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::CommitConflictValidationRejected, AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"L7-SDC conflict-manager SRF/cache cleanup requires fresh guard authority and cannot replace Phase 08 invalidation. " + guardDecision->Message, FirstRangeOrDefault(token->getDescriptor()->NormalizedFootprint.DestinationRanges), true, token->getHandle(), guardDecision));
		}

		RemoveOverlappingWindows(_srfWarmWindows, token->getDescriptor()->NormalizedFootprint.DestinationRanges);
		RemoveOverlappingWindows(_assistIngressWindows, token->getDescriptor()->NormalizedFootprint.DestinationRanges);
		return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::None, L"L7-SDC conflict-manager warmed-window evidence was retired after Phase 08 commit invalidation boundary completed.", nullptr, guardDecision);
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::NotifySerializingBoundary(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);

		std::optional<std::shared_ptr<AcceleratorFootprintReservation>> reservation;
		if (!_activeFootprints->TryGet(token->getHandle(), reservation) || !reservation || !reservation.value() || token->isTerminal())
		{
			return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::FenceOrSerializingBoundaryWhileTokenActive, L"Serializing boundary observed no active L7-SDC footprint reservation for the scoped token.");
		}

		const auto reservedFootprint = reservation.value();
		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateReservationGuard(reservedFootprint, currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return RecordConflictDecision(RejectDrift(reservedFootprint, guardDecision, FirstRangeOrDefault(reservedFootprint->DestinationRanges)));
		}

		return RecordConflictDecision(AcceleratorConflictDecision::Reject(AcceleratorConflictClass::FenceOrSerializingBoundaryWhileTokenActive, AcceleratorTokenFaultCode::FenceRejected, L"Serializing boundary reached an active L7-SDC footprint; v1 requires guarded drain, cancel, fault, or conservative rejection.", FirstRangeOrDefault(reservedFootprint->DestinationRanges), true, token->getHandle(), guardDecision));
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::NotifyVmDomainOrMappingTransition(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);

		std::optional<std::shared_ptr<AcceleratorFootprintReservation>> reservation;
		if (!_activeFootprints->TryGet(token->getHandle(), reservation) || !reservation || !reservation.value() || token->isTerminal())
		{
			return AcceleratorConflictDecision::Accepted(AcceleratorConflictClass::VmDomainMappingTransitionWhileTokenActive, L"VM/domain/mapping transition observed no active L7-SDC footprint reservation for the scoped token.");
		}

		const auto reservedFootprint = reservation.value();
		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateReservationGuard(reservedFootprint, currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return RecordConflictDecision(AcceleratorConflictDecision::Faulted(AcceleratorConflictClass::VmDomainMappingTransitionWhileTokenActive, AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"VM/domain/mapping transition invalidated an active L7-SDC token footprint; commit must be blocked by Phase 08 guard validation. " + guardDecision->Message, FirstRangeOrDefault(reservedFootprint->DestinationRanges), true, token->getHandle(), guardDecision));
		}

		return AcceleratorConflictDecision::Serialize(AcceleratorConflictClass::VmDomainMappingTransitionWhileTokenActive, reservedFootprint, guardDecision, FirstRangeOrDefault(reservedFootprint->DestinationRanges), L"VM/domain/mapping transition encountered an active L7-SDC token; v1 requires serialization, drain, cancel, or fault before authority changes.");
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::RejectIncompleteRange(AcceleratorConflictClass conflictClass, std::shared_ptr<AcceleratorMemoryRange> range, bool isWrite)
	{
		return AcceleratorConflictDecision::Reject(conflictClass, AcceleratorTokenFaultCode::ConflictRejected, L"L7-SDC conflict notification rejected because the memory range is zero length or overflows.", range, isWrite);
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::RecordConflictDecision(std::shared_ptr<AcceleratorConflictDecision> decision)
	{
		if (decision->IsRejected)
		{
			if (_telemetry != nullptr)
			{
				_telemetry->RecordFootprintConflictReject(decision->ConflictClass, decision->TokenFaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::ConflictRejected : decision->TokenFaultCode, decision->Message);
			}
			if (_telemetry != nullptr)
			{
				_telemetry->RecordGuardReject(decision->GuardDecision, decision->Message);
			}
		}

		return decision;
	}

	std::shared_ptr<AcceleratorGuardDecision> ExternalAcceleratorConflictManager::ValidateTokenAndDescriptorGuard(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, AcceleratorGuardSurface surface)
	{
		std::shared_ptr<AcceleratorGuardDecision> tokenGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!tokenGuardDecision->IsAllowed)
		{
			return tokenGuardDecision;
		}

		auto ownerDomainGuard = AcceleratorOwnerDomainGuard::getDefault();
		std::shared_ptr<AcceleratorGuardDecision> descriptorDecision;
		switch (surface)
		{
		case AcceleratorGuardSurface::Commit:
			descriptorDecision = ownerDomainGuard->EnsureBeforeCommit(token->getDescriptor(), currentGuardEvidence);
			break;
		case AcceleratorGuardSurface::SubmitAdmission:
			descriptorDecision = ownerDomainGuard->EnsureBeforeSubmit(token->getDescriptor(), currentGuardEvidence);
			break;
		default:
			descriptorDecision = ownerDomainGuard->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
			break;
		}
		if (!descriptorDecision || !descriptorDecision->IsAllowed)
		{
			return descriptorDecision;
		}

		if (!tokenGuardDecision->DescriptorOwnerBinding || !descriptorDecision->DescriptorOwnerBinding ||
			!tokenGuardDecision->DescriptorOwnerBinding->Equals(descriptorDecision->DescriptorOwnerBinding))
		{
			return AcceleratorGuardDecision::Reject(surface, AcceleratorGuardFault::DescriptorOwnerBindingMismatch, token->getDescriptor()->OwnerBinding, currentGuardEvidence.value_or(nullptr), RejectKind::OwnerMismatch, L"L7-SDC conflict manager token guard and descriptor guard bind different owners.");
		}

		return descriptorDecision;
	}

	std::shared_ptr<AcceleratorGuardDecision> ExternalAcceleratorConflictManager::ValidateReservationGuard(std::shared_ptr<AcceleratorFootprintReservation> reservation, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		return AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(reservation->ReservationGuardDecision, currentGuardEvidence);
	}

	std::shared_ptr<AcceleratorConflictDecision> ExternalAcceleratorConflictManager::RejectDrift(std::shared_ptr<AcceleratorFootprintReservation> reservation, std::shared_ptr<AcceleratorGuardDecision> guardDecision, std::shared_ptr<AcceleratorMemoryRange> faultRange)
	{
		return AcceleratorConflictDecision::Reject(AcceleratorConflictClass::VmDomainMappingTransitionWhileTokenActive, AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"L7-SDC conflict notification observed active-token owner/domain or epoch drift before overlap resolution. " + guardDecision->Message, faultRange, true, reservation->TokenHandle, guardDecision);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool AreRangesWellFormed(IReadOnlyList<AcceleratorMemoryRange>? ranges)
	bool ExternalAcceleratorConflictManager::AreRangesWellFormed(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges)
	{
		return !ranges.empty() && AcceleratorFootprintReservation::AreRangesWellFormed(ranges);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool FirstWindowOverlap(IReadOnlyList<AcceleratorMemoryRange> leftRanges, IReadOnlyList<AcceleratorMemoryRange> rightRanges, out AcceleratorMemoryRange overlap)
	bool ExternalAcceleratorConflictManager::FirstWindowOverlap(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &leftRanges, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &rightRanges, std::shared_ptr<AcceleratorMemoryRange> &overlap)
	{
		overlap = nullptr;
		if (leftRanges.empty() || rightRanges.empty())
		{
			return false;
		}

		for (std::size_t index = 0; index < rightRanges.size(); index++)
		{
			if (AcceleratorFootprintReservation::FirstOverlap(leftRanges, rightRanges[index], overlap))
			{
				return true;
			}
		}

		return false;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static AcceleratorMemoryRange FirstRangeOrDefault(IReadOnlyList<AcceleratorMemoryRange> ranges)
	std::shared_ptr<AcceleratorMemoryRange> ExternalAcceleratorConflictManager::FirstRangeOrDefault(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges)
	{
		return !ranges.empty() ? ranges.front() : nullptr;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void RemoveOverlappingWindows(List<AcceleratorMemoryRange> windows, IReadOnlyList<AcceleratorMemoryRange> committedRanges)
	void ExternalAcceleratorConflictManager::RemoveOverlappingWindows(std::vector<std::shared_ptr<AcceleratorMemoryRange>> &windows, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &committedRanges)
	{
		for (std::size_t remaining = windows.size(); remaining > 0; --remaining)
		{
			const std::size_t index = remaining - 1;
			std::shared_ptr<AcceleratorMemoryRange> window = windows[index];
			std::shared_ptr<AcceleratorMemoryRange> overlap;
			if (AcceleratorFootprintReservation::FirstOverlap(committedRanges, window, overlap))
			{
				windows.erase(windows.begin() + index);
			}
		}
	}
}
