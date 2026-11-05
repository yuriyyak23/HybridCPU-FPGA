#include "AcceleratorCommitModel.h"
#include "../Tokens/AcceleratorTokenStore.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "../../../Pipeline/Safety/SafetyVerifier.Types.h"
#include <algorithm>
#include <utility>

using namespace HybridCPU::ExternalRuntime::Contracts;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Conflicts;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;
using namespace YAKSys_Hybrid_CPU::Core::Memory;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Commit
{


// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private AcceleratorCommitInvalidationPlan(StreamRegisterFile? streamRegisterFile, IReadOnlyList<AcceleratorMemoryRange> srfWindows, IReadOnlyList<AcceleratorMemoryRange> cacheWindows, IReadOnlyList<AcceleratorCommitInvalidationRecord> records)
	AcceleratorCommitInvalidationPlan::AcceleratorCommitInvalidationPlan(std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> streamRegisterFile, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &srfWindows, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &cacheWindows, const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> &records)
		: StreamRegisterFile(std::move(streamRegisterFile)), SrfWindows(FreezeRanges(srfWindows)),
		  CacheWindows(FreezeRanges(cacheWindows)), Records(FreezeRecords(records))
	{
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> AcceleratorCommitInvalidationPlan::getStreamRegisterFile() const
	{
		return StreamRegisterFile;
	}

	const std::vector<std::shared_ptr<AcceleratorMemoryRange>> AcceleratorCommitInvalidationPlan::getSrfWindows() const
	{
		return SrfWindows;
	}

	const std::vector<std::shared_ptr<AcceleratorMemoryRange>> AcceleratorCommitInvalidationPlan::getCacheWindows() const
	{
		return CacheWindows;
	}

	const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> AcceleratorCommitInvalidationPlan::getRecords() const
	{
		return Records;
	}

	int AcceleratorCommitInvalidationPlan::getSrfInvalidationCount() const
	{
		return CountInvalidated(AcceleratorCommitInvalidationTarget::SrfWindow);
	}

	int AcceleratorCommitInvalidationPlan::getCacheInvalidationCount() const
	{
		return CountInvalidated(AcceleratorCommitInvalidationTarget::CacheWindow);
	}

	bool AcceleratorCommitInvalidationPlan::getHasInvalidationEvidence() const
	{
		return getRecords().size() != 0;
	}

	std::shared_ptr<AcceleratorCommitInvalidationPlan> AcceleratorCommitInvalidationPlan::getNone()
	{
		static const std::shared_ptr<AcceleratorCommitInvalidationPlan> none(new AcceleratorCommitInvalidationPlan(nullptr, {}, {}, {}));
		return none;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static AcceleratorCommitInvalidationPlan Observe(IReadOnlyList<AcceleratorMemoryRange>? srfWindows = null, IReadOnlyList<AcceleratorMemoryRange>? cacheWindows = null, System.Nullable<StreamRegisterFile> streamRegisterFile = null)
	std::shared_ptr<AcceleratorCommitInvalidationPlan> AcceleratorCommitInvalidationPlan::Observe(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &srfWindows, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &cacheWindows, std::shared_ptr<YAKSys_Hybrid_CPU::Memory::StreamRegisterFile> streamRegisterFile)
	{
		return std::shared_ptr<AcceleratorCommitInvalidationPlan>(new AcceleratorCommitInvalidationPlan(streamRegisterFile, srfWindows, cacheWindows, {}));
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorCommitInvalidationPlan WithRecords(IReadOnlyList<AcceleratorCommitInvalidationRecord> records)
	std::shared_ptr<AcceleratorCommitInvalidationPlan> AcceleratorCommitInvalidationPlan::WithRecords(const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> &records)
	{
		return std::shared_ptr<AcceleratorCommitInvalidationPlan>(new AcceleratorCommitInvalidationPlan(getStreamRegisterFile(), getSrfWindows(), getCacheWindows(), records));
	}

	int AcceleratorCommitInvalidationPlan::CountInvalidated(AcceleratorCommitInvalidationTarget target) const
	{
		int count = 0;
		for (int index = 0; index < getRecords().size(); index++)
		{
			std::shared_ptr<AcceleratorCommitInvalidationRecord> record = getRecords()[index];
			if (record->Target == target && record->Invalidated)
			{
				count++;
			}
		}

		return count;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<AcceleratorMemoryRange> FreezeRanges(IReadOnlyList<AcceleratorMemoryRange> ranges)
	const std::vector<std::shared_ptr<AcceleratorMemoryRange>> AcceleratorCommitInvalidationPlan::FreezeRanges(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges)
	{
		if (ranges.empty() || ranges.empty())
		{
			return std::vector<std::shared_ptr<AcceleratorMemoryRange>>();
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> copy(ranges.size());
		for (int index = 0; index < ranges.size(); index++)
		{
			copy[index] = ranges[index];
		}

		return copy;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<AcceleratorCommitInvalidationRecord> FreezeRecords(IReadOnlyList<AcceleratorCommitInvalidationRecord> records)
	const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> AcceleratorCommitInvalidationPlan::FreezeRecords(const std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> &records)
	{
		if (records.empty() || records.empty())
		{
			return std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>>();
		}

		std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> copy(records.size());
		for (int index = 0; index < records.size(); index++)
		{
			copy[index] = records[index];
		}

		return copy;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorStagedWriteSet(IReadOnlyList<AcceleratorStagedWrite> writes)
	AcceleratorStagedWriteSet::AcceleratorStagedWriteSet(const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &writes)
		: Writes(writes)
	{
	}

	const std::vector<std::shared_ptr<AcceleratorStagedWrite>> AcceleratorStagedWriteSet::getWrites() const
	{
		return Writes;
	}

	int AcceleratorStagedWriteSet::getCount() const
	{
		return getWrites().size();
	}

	unsigned long long AcceleratorStagedWriteSet::getBytesStaged() const
	{
		unsigned long long bytes = 0;
		for (int index = 0; index < getWrites().size(); index++)
		{
			bytes += getWrites()[index]->Length;
		}

		return bytes;
	}

	std::shared_ptr<AcceleratorStagedWriteSet> AcceleratorStagedWriteSet::FromStagingReadResult(std::shared_ptr<AcceleratorStagingReadResult> readResult)
	{
		ArgumentNullException::ThrowIfNull(readResult);
		return std::make_shared<AcceleratorStagedWriteSet>(readResult->StagedWrites);
	}

	const std::vector<std::shared_ptr<AcceleratorMemoryRange>> AcceleratorStagedWriteSet::NormalizeDestinationFootprint()
	{
		std::vector<std::shared_ptr<AcceleratorMemoryRange>> normalized;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryNormalizeStagedWriteRanges(Writes, out IReadOnlyList<AcceleratorMemoryRange> normalized, out _, out _))
		std::shared_ptr<AcceleratorMemoryRange> faultRange;
        std::wstring message;
        if (!TryNormalizeStagedWriteRanges(getWrites(), normalized, faultRange, message))
		{
			return std::vector<std::shared_ptr<AcceleratorMemoryRange>>();
		}

		return normalized;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool CoversExactly(IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationFootprint)
	bool AcceleratorStagedWriteSet::CoversExactly(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedDestinationFootprint)
	{
		std::shared_ptr<AcceleratorMemoryRange> faultRange;
        std::wstring message;
        return CoversExactly(normalizedDestinationFootprint, faultRange, message);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool CoversExactly(IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationFootprint, out AcceleratorMemoryRange faultRange, out string message)
	bool AcceleratorStagedWriteSet::CoversExactly(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedDestinationFootprint, std::shared_ptr<AcceleratorMemoryRange> &faultRange, std::wstring &message)
	{
		std::vector<std::shared_ptr<AcceleratorMemoryRange>> stagedRanges;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryNormalizeStagedWriteRanges(Writes, out IReadOnlyList<AcceleratorMemoryRange> stagedRanges, out faultRange, out message))
		if (!TryNormalizeStagedWriteRanges(getWrites(), stagedRanges, faultRange, message))
		{
			return false;
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> expectedRanges;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryNormalizeExpectedRanges(normalizedDestinationFootprint, out IReadOnlyList<AcceleratorMemoryRange> expectedRanges, out faultRange, out message))
		if (!TryNormalizeExpectedRanges(normalizedDestinationFootprint, expectedRanges, faultRange, message))
		{
			return false;
		}

		if (stagedRanges.size() != expectedRanges.size())
		{
			faultRange = expectedRanges.size() > 0 ? expectedRanges[0] : nullptr;
			message = L"L7-SDC staged writes do not cover every normalized destination footprint range.";
			return false;
		}

		for (int index = 0; index < stagedRanges.size(); index++)
		{
			if (stagedRanges[index]->Address == expectedRanges[index]->Address && stagedRanges[index]->Length == expectedRanges[index]->Length)
			{
				continue;
			}

			faultRange = expectedRanges[index];
			message = L"L7-SDC staged writes do not exactly match the normalized destination footprint.";
			return false;
		}

		faultRange = nullptr;
		message = L"L7-SDC staged writes exactly cover the normalized destination footprint.";
		return true;
	}

	bool AcceleratorStagedWriteSet::IsTokenBound(std::shared_ptr<AcceleratorTokenHandle> tokenHandle, std::wstring &message)
	{
		for (int index = 0; index < getWrites().size(); index++)
		{
			if (getWrites()[index]->TokenHandle && tokenHandle && *getWrites()[index]->TokenHandle == *tokenHandle)
			{
				continue;
			}

			message = L"L7-SDC staged write-set contains bytes not bound to the token selected for commit.";
			return false;
		}

		message = L"L7-SDC staged write-set is token-bound.";
		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryNormalizeStagedWriteRanges(IReadOnlyList<AcceleratorStagedWrite> writes, out IReadOnlyList<AcceleratorMemoryRange> normalized, out AcceleratorMemoryRange faultRange, out string message)
	bool AcceleratorStagedWriteSet::TryNormalizeStagedWriteRanges(const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &writes, std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalized, std::shared_ptr<AcceleratorMemoryRange> &faultRange, std::wstring &message)
	{
		if (writes.empty() || writes.empty())
		{
			normalized = std::vector<std::shared_ptr<AcceleratorMemoryRange>>();
			faultRange = nullptr;
			message = L"L7-SDC all-or-none commit requires at least one staged destination write.";
			return false;
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> ranges(writes.size());
		for (int index = 0; index < writes.size(); index++)
		{
			std::shared_ptr<AcceleratorStagedWrite> write = writes[index];
			ranges[index] = std::make_shared<AcceleratorMemoryRange>(write->Address, write->Length);
		}

		return TryNormalizeExpectedRanges(ranges, normalized, faultRange, message);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryNormalizeExpectedRanges(IReadOnlyList<AcceleratorMemoryRange> ranges, out IReadOnlyList<AcceleratorMemoryRange> normalized, out AcceleratorMemoryRange faultRange, out string message)
	bool AcceleratorStagedWriteSet::TryNormalizeExpectedRanges(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges, std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalized, std::shared_ptr<AcceleratorMemoryRange> &faultRange, std::wstring &message)
	{
		normalized = std::vector<std::shared_ptr<AcceleratorMemoryRange>>();
		faultRange = nullptr;

		if (ranges.empty() || ranges.empty())
		{
			message = L"L7-SDC normalized destination footprint is empty; commit cannot publish memory.";
			return false;
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> sorted(ranges.size());
		for (int index = 0; index < ranges.size(); index++)
		{
			std::shared_ptr<AcceleratorMemoryRange> range = ranges[index];
			if (range->Length == 0 || range->Address > std::numeric_limits<unsigned long long>::max() - range->Length)
			{
				faultRange = range;
				message = L"L7-SDC destination footprint contains a zero-length or overflowing range.";
				return false;
			}

			sorted[index] = range;
		}

		std::sort(sorted.begin(), sorted.end(), [] (const auto& left, const auto& right)
		{
				if (left->Address != right->Address) return left->Address < right->Address;
				return left->Length < right->Length;
		});

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> merged = {sorted[0]};

		for (int index = 1; index < sorted.size(); index++)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to the C# index operator:
			std::shared_ptr<AcceleratorMemoryRange> previous = merged.back();
			std::shared_ptr<AcceleratorMemoryRange> current = sorted[index];
			unsigned long long previousEnd = previous->Address + previous->Length;
			if (current->Address < previousEnd)
			{
				normalized = std::vector<std::shared_ptr<AcceleratorMemoryRange>>();
				faultRange = current;
				message = L"L7-SDC staged destination writes overlap; Phase 08 all-or-none commit rejects overlapping write evidence.";
				return false;
			}

			if (current->Address == previousEnd)
			{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to the C# index operator:
				merged.back() = std::make_shared<AcceleratorMemoryRange>(previous->Address, previous->Length + current->Length);
				continue;
			}

			merged.push_back(current);
		}

		normalized = std::move(merged);
		message = L"L7-SDC destination footprint normalized.";
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCommitCoordinator(AcceleratorTelemetry? telemetry = null)
	AcceleratorCommitCoordinator::AcceleratorCommitCoordinator(std::shared_ptr<AcceleratorTelemetry> telemetry) : _telemetry(telemetry)
	{
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public AcceleratorCommitResult TryCommit(AcceleratorToken token, AcceleratorCommandDescriptor descriptor, IAcceleratorStagingBuffer stagingBuffer, Processor.MainMemoryArea mainMemory, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, AcceleratorCommitInvalidationPlan invalidationPlan, bool commitConflictPlaceholderAccepted, bool directWriteViolationDetected = false, ExternalAcceleratorConflictManager? conflictManager = null, System.Nullable<MemoryCoherencyObserver> coherencyObserver = null, System.Nullable<ExternalOperationPublicationEvidence> externalPublicationEvidence = null, bool externalPublicationEvidenceRequired = false)
	std::shared_ptr<AcceleratorCommitResult> AcceleratorCommitCoordinator::TryCommit(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidationPlan, bool commitConflictPlaceholderAccepted, bool directWriteViolationDetected, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager, std::shared_ptr<MemoryCoherencyObserver> coherencyObserver, std::optional<std::shared_ptr<ExternalOperationPublicationEvidence>> externalPublicationEvidence, bool externalPublicationEvidenceRequired)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(stagingBuffer);
		ArgumentNullException::ThrowIfNull(mainMemory);
		invalidationPlan = (invalidationPlan != nullptr) ? invalidationPlan : AcceleratorCommitInvalidationPlan::getNone();

		if (externalPublicationEvidenceRequired && ExternalOperationPublicationGate::Evaluate(externalPublicationEvidence) != ExternalOperationPublicationEligibility::Eligible)
		{
			return RecordCommitResult(AcceleratorCommitResult::Rejected(token, std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::BackendRejected, L"External-operation publication evidence is missing, stale, mismatched, or not eligible for staged CPU publication.")), token);
		}

		std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet;
		std::optional<std::shared_ptr<AcceleratorCommitResult>> preconditionReject;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!ValidateCommitPreconditions(token, descriptor, stagingBuffer, currentGuardEvidence, commitConflictPlaceholderAccepted, directWriteViolationDetected, conflictManager, out AcceleratorStagedWriteSet? stagedWriteSet, out System.Nullable<AcceleratorCommitResult> preconditionReject))
		if (!ValidateCommitPreconditions(token, descriptor, stagingBuffer, currentGuardEvidence, commitConflictPlaceholderAccepted, directWriteViolationDetected, conflictManager, stagedWriteSet, preconditionReject))
		{
			ReleaseTerminalReservationIfNeeded(conflictManager, token, currentGuardEvidence);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return RecordCommitResult(preconditionReject!, token);
			return RecordCommitResult(preconditionReject.value_or(nullptr), token);
		}

		if (token->getState() == AcceleratorTokenState::DeviceComplete)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition pending = token.MarkCommitPendingFromCommitCoordinator(currentGuardEvidence!);
			std::shared_ptr<AcceleratorTokenTransition> pending = token->MarkCommitPendingFromCommitCoordinator(currentGuardEvidence.value_or(nullptr));
			if (pending->Rejected)
			{
				return RecordCommitResult(AcceleratorCommitResult::Rejected(token, std::make_shared<AcceleratorCommitFault>(pending->FaultCode, pending->Message, 0, false, token->getSubmitGuardDecision())), token);
			}
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (!ApplyAllOrNone( token, descriptor, mainMemory, stagedWriteSet!, currentGuardEvidence, out AcceleratorRollbackRecord rollback, out AcceleratorCommitFault? memoryFault, out ulong bytesCommitted))
		std::shared_ptr<AcceleratorRollbackRecord> rollback;
		std::optional<std::shared_ptr<AcceleratorCommitFault>> memoryFault;
		unsigned long long bytesCommitted;
		if (!ApplyAllOrNone(token, descriptor, mainMemory, stagedWriteSet, currentGuardEvidence, rollback, memoryFault, bytesCommitted))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: token.MarkFaulted( memoryFault!.FaultCode, currentGuardEvidence!);
				token->MarkFaulted(memoryFault.value()->FaultCode, currentGuardEvidence.value_or(nullptr));
			ReleaseTerminalReservationIfNeeded(conflictManager, token, currentGuardEvidence);
			return RecordCommitResult(AcceleratorCommitResult::Rejected(token, memoryFault.value(), rollback), token);
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (!InvalidateSrfAndCache( token, descriptor, stagedWriteSet!, invalidationPlan, currentGuardEvidence, out AcceleratorCommitInvalidationPlan invalidated, out AcceleratorCommitFault? invalidationFault))
		std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidated;
		std::optional<std::shared_ptr<AcceleratorCommitFault>> invalidationFault;
		if (!InvalidateSrfAndCache(token, descriptor, stagedWriteSet, invalidationPlan, currentGuardEvidence, invalidated, invalidationFault))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorRollbackRecord invalidationRollback = Rollback( token, descriptor, mainMemory, rollback.Backups, stagedWriteSet!.Writes, writeFailureIndex: stagedWriteSet.Writes.Count - 1, currentGuardEvidence);
			std::shared_ptr<AcceleratorRollbackRecord> invalidationRollback = Rollback(token, descriptor, mainMemory, rollback->Backups, stagedWriteSet->getWrites(), stagedWriteSet->getWrites().size() - 1, currentGuardEvidence);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: token.MarkFaulted( invalidationFault!.FaultCode, currentGuardEvidence!);
			token->MarkFaulted(invalidationFault.value()->FaultCode, currentGuardEvidence.value_or(nullptr));
			ReleaseTerminalReservationIfNeeded(conflictManager, token, currentGuardEvidence);
			return RecordCommitResult(AcceleratorCommitResult::Rejected(token, invalidationFault.value(), invalidationRollback), token);
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorTokenTransition committed = token.MarkCommittedFromCommitCoordinator(currentGuardEvidence!);
		std::shared_ptr<AcceleratorTokenTransition> committed = token->MarkCommittedFromCommitCoordinator(currentGuardEvidence.value_or(nullptr));
		if (committed->Rejected)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: AcceleratorRollbackRecord postWriteRollback = Rollback( token, descriptor, mainMemory, rollback.Backups, stagedWriteSet!.Writes, writeFailureIndex: stagedWriteSet.Writes.Count - 1, currentGuardEvidence);
			std::shared_ptr<AcceleratorRollbackRecord> postWriteRollback = Rollback(token, descriptor, mainMemory, rollback->Backups, stagedWriteSet->getWrites(), stagedWriteSet->getWrites().size() - 1, currentGuardEvidence);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: token.MarkFaulted( committed.FaultCode == AcceleratorTokenFaultCode.None ? AcceleratorTokenFaultCode.CommitMemoryFault : committed.FaultCode, currentGuardEvidence!);
			token->MarkFaulted(committed->FaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::CommitMemoryFault : committed->FaultCode, currentGuardEvidence.value_or(nullptr));
			ReleaseTerminalReservationIfNeeded(conflictManager, token, currentGuardEvidence);
			return RecordCommitResult(AcceleratorCommitResult::Rejected(token, std::make_shared<AcceleratorCommitFault>(committed->FaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::CommitMemoryFault : committed->FaultCode, committed->Message, 0, false, token->getSubmitGuardDecision()), postWriteRollback), token);
		}

		if (conflictManager != nullptr)
		{
			conflictManager->InvalidateSrfCacheOnCommit(token, currentGuardEvidence);
		}
		if (conflictManager != nullptr)
		{
			conflictManager->ReleaseTokenFootprint(token, currentGuardEvidence);
		}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: NotifyCommittedWrites( coherencyObserver, descriptor, stagedWriteSet!);
		NotifyCommittedWrites(coherencyObserver, descriptor, stagedWriteSet);

		return RecordCommitResult(AcceleratorCommitResult::Success(token, bytesCommitted, invalidated), token);
	}

	std::shared_ptr<AcceleratorCommitResult> AcceleratorCommitCoordinator::RecordCommitResult(std::shared_ptr<AcceleratorCommitResult> result, std::shared_ptr<AcceleratorToken> token)
	{
		if (_telemetry == nullptr)
		{
			return result;
		}

		if (result->Succeeded)
		{
			_telemetry->RecordBytesCommitted(result->BytesCommitted, result->Message);
			_telemetry->RecordSrfInvalidations(result->InvalidationPlan->getSrfInvalidationCount(), result->Message);
			_telemetry->RecordCacheInvalidations(result->InvalidationPlan->getCacheInvalidationCount(), result->Message);
		}
		else
		{
			_telemetry->RecordCommitReject(result->getFaultCode(), token, result->Message);
			_telemetry->RecordGuardReject((result->Fault == nullptr ? nullptr : result->Fault->GuardDecision), result->Message);
		}

		_telemetry->RecordCommitRollback(result->Rollback->RollbackAttempted, result->Rollback->RollbackSucceeded, result->Rollback->Message);
		return result;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static void NotifyCommittedWrites(MemoryCoherencyObserver? coherencyObserver, AcceleratorCommandDescriptor descriptor, AcceleratorStagedWriteSet stagedWriteSet)
	void AcceleratorCommitCoordinator::NotifyCommittedWrites(std::shared_ptr<MemoryCoherencyObserver> coherencyObserver, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet)
	{
		unsigned long long domainTag = descriptor->OwnerBinding->DomainTag;
		for (int index = 0; index < stagedWriteSet->getWrites().size(); index++)
		{
			std::shared_ptr<AcceleratorStagedWrite> write = stagedWriteSet->getWrites()[index];
			if (coherencyObserver == nullptr)
			{
				continue;
			}

			coherencyObserver->NotifyWrite(std::make_shared<MemoryCoherencyWriteNotification>(write->Address, write->Length, domainTag, MemoryCoherencyWriteSourceKind::L7AcceleratorCommit));
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool ValidateCommitPreconditions(AcceleratorToken token, AcceleratorCommandDescriptor descriptor, IAcceleratorStagingBuffer stagingBuffer, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence, bool commitConflictPlaceholderAccepted, bool directWriteViolationDetected, ExternalAcceleratorConflictManager? conflictManager, out System.Nullable<AcceleratorStagedWriteSet> stagedWriteSet, out System.Nullable<AcceleratorCommitResult> rejection)
	bool AcceleratorCommitCoordinator::ValidateCommitPreconditions(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<IAcceleratorStagingBuffer> stagingBuffer, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, bool commitConflictPlaceholderAccepted, bool directWriteViolationDetected, std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager, std::shared_ptr<AcceleratorStagedWriteSet> &stagedWriteSet, std::optional<std::shared_ptr<AcceleratorCommitResult>> &rejection)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(stagingBuffer);

		stagedWriteSet = nullptr;
		rejection = std::nullopt;

		if (token->getState() != AcceleratorTokenState::DeviceComplete && token->getState() != AcceleratorTokenState::CommitPending)
		{
			AcceleratorTokenFaultCode faultCode = token->isTerminal() ? AcceleratorTokenFaultCode::TerminalState : AcceleratorTokenFaultCode::IllegalTransition;
			rejection = AcceleratorCommitResult::Rejected(token, std::make_shared<AcceleratorCommitFault>(faultCode, std::format(L"L7-SDC commit requires DeviceComplete or CommitPending token state, but token is {0}.", static_cast<int>(token->getState()))));
			return false;
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateOwnerDomainAndMapping(token, descriptor, currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			rejection = AcceleratorCommitResult::Rejected(token, std::make_shared<AcceleratorCommitFault>(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"L7-SDC commit requires fresh owner/domain and mapping/IOMMU epoch authority. " + guardDecision->Message, 0, false, guardDecision));
			return false;
		}

		if (!IsDescriptorIdentityBound(token, descriptor))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: rejection = FaultToken( token, AcceleratorTokenFaultCode.DescriptorIdentityMismatch, "L7-SDC commit descriptor identity hash does not match the token-bound descriptor identity.", currentGuardEvidence!, guardDecision, descriptor.DescriptorReference.DescriptorAddress, false);
			rejection = FaultToken(token, AcceleratorTokenFaultCode::DescriptorIdentityMismatch, L"L7-SDC commit descriptor identity hash does not match the token-bound descriptor identity.", currentGuardEvidence.value_or(nullptr), guardDecision, descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		if (!IsNormalizedFootprintBound(token, descriptor))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: rejection = FaultToken( token, AcceleratorTokenFaultCode.NormalizedFootprintMismatch, "L7-SDC commit normalized footprint hash does not match the token-bound footprint identity.", currentGuardEvidence!, guardDecision, descriptor.DescriptorReference.DescriptorAddress, false);
			rejection = FaultToken(token, AcceleratorTokenFaultCode::NormalizedFootprintMismatch, L"L7-SDC commit normalized footprint hash does not match the token-bound footprint identity.", currentGuardEvidence.value_or(nullptr), guardDecision, descriptor->DescriptorReference.DescriptorAddress, false);
			return false;
		}

		if (directWriteViolationDetected)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: rejection = FaultToken( token, AcceleratorTokenFaultCode.DirectWriteViolation, "L7-SDC direct-write violation evidence cannot count as commit authority.", currentGuardEvidence!, guardDecision, 0, true);
			rejection = FaultToken(token, AcceleratorTokenFaultCode::DirectWriteViolation, L"L7-SDC direct-write violation evidence cannot count as commit authority.", currentGuardEvidence.value_or(nullptr), guardDecision, 0, true);
			return false;
		}

		if (conflictManager == nullptr && !commitConflictPlaceholderAccepted)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: rejection = FaultToken( token, AcceleratorTokenFaultCode.CommitConflictRejected, "L7-SDC Phase 08 commit requires explicit placeholder conflict acceptance; Phase 10 conflict manager is not implemented.", currentGuardEvidence!, guardDecision, descriptor.NormalizedFootprint.DestinationRanges.Count > 0 ? descriptor.NormalizedFootprint.DestinationRanges[0]->Address : 0, true);
			rejection = FaultToken(token, AcceleratorTokenFaultCode::CommitConflictRejected, L"L7-SDC Phase 08 commit requires explicit placeholder conflict acceptance; Phase 10 conflict manager is not implemented.", currentGuardEvidence.value_or(nullptr), guardDecision, descriptor->NormalizedFootprint.DestinationRanges.size() > 0 ? descriptor->NormalizedFootprint.DestinationRanges[0]->Address: 0, true);
			return false;
		}

		std::shared_ptr<AcceleratorStagingReadResult> stagingRead = stagingBuffer->GetStagedWriteSet(token, currentGuardEvidence);
		if (stagingRead->IsRejected)
		{
			rejection = AcceleratorCommitResult::Rejected(token, std::make_shared<AcceleratorCommitFault>(stagingRead->FaultCode, stagingRead->Message, 0, false, stagingRead->GuardDecision));
			return false;
		}

		stagedWriteSet = AcceleratorStagedWriteSet::FromStagingReadResult(stagingRead);
		std::wstring tokenBindingMessage;
		if (!stagedWriteSet->IsTokenBound(token->getHandle(), tokenBindingMessage))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: rejection = FaultToken( token, AcceleratorTokenFaultCode.TokenHandleNotAuthority, tokenBindingMessage, currentGuardEvidence!, guardDecision, 0, true);
			rejection = FaultToken(token, AcceleratorTokenFaultCode::TokenHandleNotAuthority, tokenBindingMessage, currentGuardEvidence.value_or(nullptr), guardDecision, 0, true);
			return false;
		}

		std::shared_ptr<AcceleratorMemoryRange> coverageFaultRange;
		std::wstring coverageMessage;
		if (!ValidateExactCoverage(stagedWriteSet, token->getDescriptor()->NormalizedFootprint.DestinationRanges, coverageFaultRange, coverageMessage))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: rejection = FaultToken( token, AcceleratorTokenFaultCode.StagedCoverageMismatch, coverageMessage, currentGuardEvidence!, guardDecision, coverageFaultRange->Address, true);
			rejection = FaultToken(token, AcceleratorTokenFaultCode::StagedCoverageMismatch, coverageMessage, currentGuardEvidence.value_or(nullptr), guardDecision, coverageFaultRange->Address, true);
			return false;
		}

		if (conflictManager != nullptr)
		{
			std::shared_ptr<AcceleratorConflictDecision> conflictDecision = conflictManager->ValidateBeforeCommit(token, currentGuardEvidence);
			if (conflictDecision->IsRejected)
			{
				AcceleratorTokenFaultCode faultCode = conflictDecision->TokenFaultCode == AcceleratorTokenFaultCode::None ? AcceleratorTokenFaultCode::CommitConflictRejected : conflictDecision->TokenFaultCode;
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: rejection = FaultToken( token, faultCode, conflictDecision.Message, currentGuardEvidence!, conflictDecision.GuardDecision ?? guardDecision, conflictDecision.Fault?.FaultAddress ?? (token.Descriptor.NormalizedFootprint.DestinationRanges.Count > 0 ? token.Descriptor.NormalizedFootprint.DestinationRanges[0]->Address : 0), true);
				const unsigned long long fallbackAddress = token->getDescriptor()->NormalizedFootprint.DestinationRanges.empty() ? 0 : token->getDescriptor()->NormalizedFootprint.DestinationRanges[0]->Address;
				const unsigned long long faultAddress = conflictDecision->Fault ? conflictDecision->Fault->FaultAddress : fallbackAddress;
				rejection = FaultToken(token, faultCode, conflictDecision->Message, currentGuardEvidence.value_or(nullptr), conflictDecision->GuardDecision ? conflictDecision->GuardDecision : guardDecision, faultAddress, true);
				return false;
			}
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool ValidateExactCoverage(AcceleratorStagedWriteSet stagedWriteSet, IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationFootprint, out AcceleratorMemoryRange faultRange, out string message)
	bool AcceleratorCommitCoordinator::ValidateExactCoverage(std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedDestinationFootprint, std::shared_ptr<AcceleratorMemoryRange> &faultRange, std::wstring &message)
	{
		ArgumentNullException::ThrowIfNull(stagedWriteSet);
		return stagedWriteSet->CoversExactly(normalizedDestinationFootprint, faultRange, message);
	}

	std::shared_ptr<AcceleratorGuardDecision> AcceleratorCommitCoordinator::ValidateOwnerDomainAndMapping(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(descriptor);

		std::shared_ptr<AcceleratorGuardDecision> tokenGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!tokenGuardDecision->IsAllowed)
		{
			return tokenGuardDecision;
		}

		std::shared_ptr<AcceleratorGuardDecision> commitGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeCommit(descriptor, currentGuardEvidence);
		if (!commitGuardDecision->IsAllowed)
		{
			return commitGuardDecision;
		}

		if (tokenGuardDecision->DescriptorOwnerBinding == nullptr || commitGuardDecision->DescriptorOwnerBinding == nullptr || !tokenGuardDecision->DescriptorOwnerBinding->Equals(commitGuardDecision->DescriptorOwnerBinding))
		{
			return AcceleratorGuardDecision::Reject(AcceleratorGuardSurface::Commit, AcceleratorGuardFault::DescriptorOwnerBindingMismatch, descriptor->OwnerBinding, currentGuardEvidence.value_or(nullptr), RejectKind::OwnerMismatch, L"L7-SDC commit token guard and descriptor commit guard bind different owners.");
		}

		return commitGuardDecision;
	}

	bool AcceleratorCommitCoordinator::ApplyAllOrNone(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorRollbackRecord> &rollbackRecord, std::optional<std::shared_ptr<AcceleratorCommitFault>> &fault, unsigned long long &bytesCommitted)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(mainMemory);
		ArgumentNullException::ThrowIfNull(stagedWriteSet);

		rollbackRecord = AcceleratorRollbackRecord::None;
		fault = std::nullopt;
		bytesCommitted = 0;

		if (token->getState() != AcceleratorTokenState::CommitPending)
		{
			fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::IllegalTransition, std::format(L"L7-SDC all-or-none publication requires CommitPending token state, but token is {0}.", static_cast<int>(token->getState())));
			return false;
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateOwnerDomainAndMapping(token, descriptor, currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"L7-SDC all-or-none publication requires fresh owner/domain and mapping/IOMMU epoch authority. " + guardDecision->Message, 0, false, guardDecision);
			return false;
		}

		if (!ValidateDescriptorAndFootprintBinding(token, descriptor, guardDecision, fault))
		{
			return false;
		}

		std::shared_ptr<AcceleratorMemoryRange> coverageFaultRange;
		std::wstring coverageMessage;
		if (!ValidateExactCoverage(stagedWriteSet, token->getDescriptor()->NormalizedFootprint.DestinationRanges, coverageFaultRange, coverageMessage))
		{
			fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::StagedCoverageMismatch, coverageMessage, coverageFaultRange->Address, true, guardDecision);
			return false;
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return ApplyAllOrNoneAfterValidation( token, descriptor, mainMemory, stagedWriteSet, currentGuardEvidence!, out rollbackRecord, out fault, out bytesCommitted);
		return ApplyAllOrNoneAfterValidation(token, descriptor, mainMemory, stagedWriteSet, currentGuardEvidence.value_or(nullptr), rollbackRecord, fault, bytesCommitted);
	}

	bool AcceleratorCommitCoordinator::ApplyAllOrNoneAfterValidation(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, std::shared_ptr<AcceleratorRollbackRecord> &rollbackRecord, std::optional<std::shared_ptr<AcceleratorCommitFault>> &fault, unsigned long long &bytesCommitted)
	{
		rollbackRecord = AcceleratorRollbackRecord::None;
		fault = std::nullopt;
		bytesCommitted = 0;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<AcceleratorStagedWrite> writes = stagedWriteSet.Writes;
		const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &writes = stagedWriteSet->getWrites();
		std::vector<std::shared_ptr<AcceleratorStagedWrite>> backups;
		backups.reserve(writes.size());
		for (int index = 0; index < writes.size(); index++)
		{
			std::shared_ptr<AcceleratorStagedWrite> stagedWrite = writes[index];
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: if (stagedWrite.Length > int.MaxValue || !HasExactMainMemoryRange(mainMemory, stagedWrite.Address, checked((int)stagedWrite.Length)))
			if (stagedWrite->Length > std::numeric_limits<int>::max() || !HasExactMainMemoryRange(mainMemory, stagedWrite->Address, static_cast<int>(stagedWrite->Length)))
			{
				fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::CommitMemoryFault, std::format(L"L7-SDC commit reached out-of-range destination 0x{0:X} covering {1} byte(s).", stagedWrite->Address, stagedWrite->Length), stagedWrite->Address, true);
				return false;
			}

			std::vector<unsigned char> backup(static_cast<int>(stagedWrite->Length));
			if (!mainMemory->TryReadPhysicalRange(stagedWrite->Address, backup))
			{
				fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::CommitMemoryFault, std::format(L"L7-SDC commit could not snapshot destination 0x{0:X} before all-or-none publication.", stagedWrite->Address), stagedWrite->Address, true);
				return false;
			}

			backups.push_back(std::make_shared<AcceleratorStagedWrite>(stagedWrite->TokenHandle, stagedWrite->Address, backup));
		}

		rollbackRecord = AcceleratorRollbackRecord::SnapshotOnly(writes, backups);
		for (int index = 0; index < writes.size(); index++)
		{
			std::shared_ptr<AcceleratorStagedWrite> stagedWrite = writes[index];
			if (mainMemory->TryWritePhysicalRange(stagedWrite->Address, std::span<const unsigned char>(stagedWrite->Data)))
			{
				bytesCommitted += stagedWrite->Length;
				continue;
			}

			std::optional<std::shared_ptr<AcceleratorGuardEvidence>> rollbackEvidence = currentGuardEvidence;
			rollbackRecord = Rollback(token, descriptor, mainMemory, backups, writes, index, rollbackEvidence);
			fault = std::make_shared<AcceleratorCommitFault>(rollbackRecord->RollbackSucceeded ? AcceleratorTokenFaultCode::CommitMemoryFault : AcceleratorTokenFaultCode::RollbackFailed, rollbackRecord->RollbackSucceeded ? std::format(L"L7-SDC commit failed to write destination 0x{0:X}; staged writes were rolled back and not reported as visible success.", stagedWrite->Address) : std::format(L"L7-SDC commit failed to write destination 0x{0:X} and rollback failed.", stagedWrite->Address), stagedWrite->Address, true);
			bytesCommitted = 0;
			return false;
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorRollbackRecord Rollback(AcceleratorToken token, AcceleratorCommandDescriptor descriptor, Processor.MainMemoryArea mainMemory, IReadOnlyList<AcceleratorStagedWrite> backups, IReadOnlyList<AcceleratorStagedWrite> attemptedWrites, int writeFailureIndex, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence)
	std::shared_ptr<AcceleratorRollbackRecord> AcceleratorCommitCoordinator::Rollback(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<Processor::MainMemoryArea> mainMemory, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &backups, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &attemptedWrites, int writeFailureIndex, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(mainMemory);
		ArgumentNullException::ThrowIfNull(backups);
		ArgumentNullException::ThrowIfNull(attemptedWrites);

		if (token->getState() != AcceleratorTokenState::CommitPending)
		{
			return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, false, std::format(L"L7-SDC rollback requires CommitPending token state, but token is {0}; rollback side effects were rejected.", static_cast<int>(token->getState())));
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateOwnerDomainAndMapping(token, descriptor, currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, false, L"L7-SDC rollback requires fresh owner/domain and mapping/IOMMU epoch authority; rollback side effects were rejected. " + guardDecision->Message);
		}

		if (!IsDescriptorIdentityBound(token, descriptor))
		{
			return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, false, L"L7-SDC rollback descriptor identity hash does not match the token-bound descriptor identity; rollback side effects were rejected.");
		}

		if (!IsNormalizedFootprintBound(token, descriptor))
		{
			return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, false, L"L7-SDC rollback normalized footprint hash does not match the token-bound footprint identity; rollback side effects were rejected.");
		}

		if (!IsRollbackFailureIndexValid(writeFailureIndex, backups.size()))
		{
			return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, false, L"L7-SDC rollback failure index is outside the token-bound rollback evidence; rollback side effects were rejected.");
		}

		std::wstring backupMessage;
		if (!ValidateRollbackWriteEvidence(token, backups, guardDecision, L"backup", backupMessage))
		{
			return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, false, backupMessage);
		}

		std::wstring attemptedMessage;
		if (!ValidateRollbackWriteEvidence(token, attemptedWrites, guardDecision, L"attempted", attemptedMessage))
		{
			return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, false, attemptedMessage);
		}

		return RollbackAfterValidation(mainMemory, backups, attemptedWrites, writeFailureIndex);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static AcceleratorRollbackRecord RollbackAfterValidation(Processor.MainMemoryArea mainMemory, IReadOnlyList<AcceleratorStagedWrite> backups, IReadOnlyList<AcceleratorStagedWrite> attemptedWrites, int writeFailureIndex)
	std::shared_ptr<AcceleratorRollbackRecord> AcceleratorCommitCoordinator::RollbackAfterValidation(std::shared_ptr<Processor::MainMemoryArea> mainMemory, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &backups, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &attemptedWrites, int writeFailureIndex)
	{
		int start = std::min(writeFailureIndex, static_cast<int>(backups.size()) - 1);
		for (int index = start; index >= 0; index--)
		{
			std::shared_ptr<AcceleratorStagedWrite> backup = backups[index];
			if (!mainMemory->TryWritePhysicalRange(backup->Address, std::span<const unsigned char>(backup->Data)))
			{
				return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, false, std::format(L"L7-SDC rollback failed while restoring destination 0x{0:X}.", backup->Address));
			}
		}

		return AcceleratorRollbackRecord::Completed(attemptedWrites, backups, writeFailureIndex, true, L"L7-SDC all-or-none rollback restored every destination snapshot.");
	}

	bool AcceleratorCommitCoordinator::InvalidateSrfAndCache(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidationPlan, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence, std::shared_ptr<AcceleratorCommitInvalidationPlan> &invalidated, std::optional<std::shared_ptr<AcceleratorCommitFault>> &fault)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(descriptor);
		ArgumentNullException::ThrowIfNull(stagedWriteSet);
		invalidationPlan = (invalidationPlan != nullptr) ? invalidationPlan : AcceleratorCommitInvalidationPlan::getNone();
		invalidated = invalidationPlan;
		fault = std::nullopt;

		if (token->getState() != AcceleratorTokenState::CommitPending)
		{
			fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::IllegalTransition, std::format(L"L7-SDC SRF/cache invalidation requires CommitPending token state, but token is {0}.", static_cast<int>(token->getState())));
			return false;
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = ValidateOwnerDomainAndMapping(token, descriptor, currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), L"L7-SDC SRF/cache invalidation requires fresh owner/domain and mapping/IOMMU epoch authority. " + guardDecision->Message, 0, false, guardDecision);
			return false;
		}

		if (!ValidateDescriptorAndFootprintBinding(token, descriptor, guardDecision, fault))
		{
			return false;
		}

		std::shared_ptr<AcceleratorMemoryRange> coverageFaultRange;
		std::wstring coverageMessage;
		if (!ValidateExactCoverage(stagedWriteSet, token->getDescriptor()->NormalizedFootprint.DestinationRanges, coverageFaultRange, coverageMessage))
		{
			fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::StagedCoverageMismatch, coverageMessage, coverageFaultRange->Address, true, guardDecision);
			return false;
		}

		invalidated = InvalidateSrfAndCacheAfterValidation(stagedWriteSet, invalidationPlan);
		return true;
	}

	std::shared_ptr<AcceleratorCommitInvalidationPlan> AcceleratorCommitCoordinator::InvalidateSrfAndCacheAfterValidation(std::shared_ptr<AcceleratorStagedWriteSet> stagedWriteSet, std::shared_ptr<AcceleratorCommitInvalidationPlan> invalidationPlan)
	{
		std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> records;
		for (int writeIndex = 0; writeIndex < stagedWriteSet->getWrites().size(); writeIndex++)
		{
			std::shared_ptr<AcceleratorStagedWrite> write = stagedWriteSet->getWrites()[writeIndex];
			std::shared_ptr<AcceleratorMemoryRange> committedRange = std::make_shared<AcceleratorMemoryRange>(write->Address, write->Length);

			if (write->Length <= std::numeric_limits<unsigned int>::max())
			{
				if (invalidationPlan->getStreamRegisterFile() != nullptr)
				{
					invalidationPlan->getStreamRegisterFile()->InvalidateOverlappingRange(write->Address, static_cast<unsigned int>(write->Length));
				}
			}

			RecordInvalidations(AcceleratorCommitInvalidationTarget::SrfWindow, invalidationPlan->getSrfWindows(), committedRange, records);
			RecordInvalidations(AcceleratorCommitInvalidationTarget::CacheWindow, invalidationPlan->getCacheWindows(), committedRange, records);
		}

		return invalidationPlan->WithRecords(records);
	}

	bool AcceleratorCommitCoordinator::ValidateDescriptorAndFootprintBinding(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::shared_ptr<AcceleratorGuardDecision> guardDecision, std::optional<std::shared_ptr<AcceleratorCommitFault>> &fault)
	{
		if (!IsDescriptorIdentityBound(token, descriptor))
		{
			fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::DescriptorIdentityMismatch, L"L7-SDC side-effect helper descriptor identity hash does not match the token-bound descriptor identity.", descriptor->DescriptorReference.DescriptorAddress, false, guardDecision);
			return false;
		}

		if (!IsNormalizedFootprintBound(token, descriptor))
		{
			fault = std::make_shared<AcceleratorCommitFault>(AcceleratorTokenFaultCode::NormalizedFootprintMismatch, L"L7-SDC side-effect helper normalized footprint hash does not match the token-bound footprint identity.", descriptor->DescriptorReference.DescriptorAddress, false, guardDecision);
			return false;
		}

		fault = std::nullopt;
		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private bool ValidateRollbackWriteEvidence(AcceleratorToken token, IReadOnlyList<AcceleratorStagedWrite> writes, AcceleratorGuardDecision guardDecision, string evidenceName, out string message)
	bool AcceleratorCommitCoordinator::ValidateRollbackWriteEvidence(std::shared_ptr<AcceleratorToken> token, const std::vector<std::shared_ptr<AcceleratorStagedWrite>> &writes, std::shared_ptr<AcceleratorGuardDecision> guardDecision, const std::wstring &evidenceName, std::wstring &message)
	{
		std::shared_ptr<AcceleratorStagedWriteSet> writeSet = std::make_shared<AcceleratorStagedWriteSet>(writes);
		std::wstring tokenBindingMessage;
		if (!writeSet->IsTokenBound(token->getHandle(), tokenBindingMessage))
		{
			message = std::format(L"L7-SDC rollback {0} write evidence is not token-bound; {1} Rollback side effects were rejected.", evidenceName, tokenBindingMessage);
			return false;
		}

		std::shared_ptr<AcceleratorMemoryRange> coverageFaultRange;
		std::wstring coverageMessage;
		if (!ValidateExactCoverage(writeSet, token->getDescriptor()->NormalizedFootprint.DestinationRanges, coverageFaultRange, coverageMessage))
		{
			message = std::format(L"L7-SDC rollback {0} write evidence does not exactly cover the token-bound destination footprint at 0x{1:X}; {2} Rollback side effects were rejected.", evidenceName, coverageFaultRange->Address, coverageMessage);
			return false;
		}

		message = std::format(L"L7-SDC rollback {0} write evidence is token-bound and exact for guard surface {1}.", evidenceName, static_cast<int>(guardDecision->Surface));
		return true;
	}

	bool AcceleratorCommitCoordinator::IsRollbackFailureIndexValid(int writeFailureIndex, int backupCount)
	{
		if (backupCount == 0)
		{
			return writeFailureIndex == -1;
		}

		return writeFailureIndex >= 0 && writeFailureIndex < backupCount;
	}

	std::shared_ptr<AcceleratorCommitResult> AcceleratorCommitCoordinator::FaultToken(std::shared_ptr<AcceleratorToken> token, AcceleratorTokenFaultCode faultCode, const std::wstring &message, std::shared_ptr<AcceleratorGuardEvidence> currentGuardEvidence, std::shared_ptr<AcceleratorGuardDecision> guardDecision, unsigned long long faultAddress, bool isWrite)
	{
		std::shared_ptr<AcceleratorTokenTransition> fault = token->MarkFaulted(faultCode, currentGuardEvidence);
		AcceleratorTokenFaultCode visibleFault = fault->Succeeded ? faultCode : fault->FaultCode;
		std::wstring visibleMessage = fault->Succeeded ? message : fault->Message;
		return AcceleratorCommitResult::Rejected(token, std::make_shared<AcceleratorCommitFault>(visibleFault, visibleMessage, faultAddress, isWrite, guardDecision));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static void ReleaseTerminalReservationIfNeeded(ExternalAcceleratorConflictManager? conflictManager, AcceleratorToken token, System.Nullable<AcceleratorGuardEvidence> currentGuardEvidence)
	void AcceleratorCommitCoordinator::ReleaseTerminalReservationIfNeeded(std::shared_ptr<ExternalAcceleratorConflictManager> conflictManager, std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		if (conflictManager == nullptr || !token->isTerminal())
		{
			return;
		}

		conflictManager->ReleaseTokenFootprint(token, currentGuardEvidence);
	}

	bool AcceleratorCommitCoordinator::IsDescriptorIdentityBound(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor)
	{
		return token->getDescriptor()->Identity.DescriptorIdentityHash == descriptor->Identity.DescriptorIdentityHash && token->getDescriptor()->DescriptorReference.DescriptorIdentityHash == descriptor->DescriptorReference.DescriptorIdentityHash;
	}

	bool AcceleratorCommitCoordinator::IsNormalizedFootprintBound(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor)
	{
		return token->getDescriptor()->Identity.NormalizedFootprintHash == descriptor->Identity.NormalizedFootprintHash && token->getDescriptor()->NormalizedFootprint.Hash == descriptor->NormalizedFootprint.Hash;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void RecordInvalidations(AcceleratorCommitInvalidationTarget target, IReadOnlyList<AcceleratorMemoryRange> windows, AcceleratorMemoryRange committedRange, List<AcceleratorCommitInvalidationRecord> records)
	void AcceleratorCommitCoordinator::RecordInvalidations(AcceleratorCommitInvalidationTarget target, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &windows, std::shared_ptr<AcceleratorMemoryRange> committedRange, std::vector<std::shared_ptr<AcceleratorCommitInvalidationRecord>> &records)
	{
		for (int index = 0; index < windows.size(); index++)
		{
			std::shared_ptr<AcceleratorMemoryRange> window = windows[index];
			bool overlapped = RangesOverlap(committedRange, window);
			records.push_back(std::make_shared<AcceleratorCommitInvalidationRecord>(target, window, committedRange, overlapped, overlapped));
		}
	}

	bool AcceleratorCommitCoordinator::RangesOverlap(std::shared_ptr<AcceleratorMemoryRange> left, std::shared_ptr<AcceleratorMemoryRange> right)
	{
		if (left->Length == 0 || right->Length == 0 || left->Address > std::numeric_limits<unsigned long long>::max() - left->Length || right->Address > std::numeric_limits<unsigned long long>::max() - right->Length)
		{
			return false;
		}

		unsigned long long leftEnd = left->Address + left->Length;
		unsigned long long rightEnd = right->Address + right->Length;
		return left->Address < rightEnd && right->Address < leftEnd;
	}

	bool AcceleratorCommitCoordinator::HasExactMainMemoryRange(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, int size)
	{
		if (size <= 0)
		{
			return false;
		}

		unsigned long long memoryLength = static_cast<unsigned long long>(mainMemory->getLength());
		unsigned long long requestSize = static_cast<unsigned long long>(size);
		return requestSize <= memoryLength && address <= memoryLength - requestSize;
	}
}






