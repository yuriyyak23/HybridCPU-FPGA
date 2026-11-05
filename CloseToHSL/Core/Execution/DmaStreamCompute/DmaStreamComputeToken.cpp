#include "DmaStreamComputeToken.h"
#include "../../Memory/MemoryCoherencyObserver.h"
#include "DmaStreamComputeTelemetry.h"
#include "DmaStreamComputeDescriptor.h"
#include "../../Pipeline/Safety/SafetyVerifier.Types.h"
#include "DmaStreamComputeReplayEvidence.h"
#include <algorithm>
#include <format>
#include <stdexcept>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Memory;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	namespace
	{
		const wchar_t *TokenStateName(DmaStreamComputeTokenState state) noexcept
		{
			switch (state)
			{
			case DmaStreamComputeTokenState::Admitted: return L"Admitted";
			case DmaStreamComputeTokenState::Issued: return L"Issued";
			case DmaStreamComputeTokenState::ReadsComplete: return L"ReadsComplete";
			case DmaStreamComputeTokenState::ComputeComplete: return L"ComputeComplete";
			case DmaStreamComputeTokenState::CommitPending: return L"CommitPending";
			case DmaStreamComputeTokenState::Committed: return L"Committed";
			case DmaStreamComputeTokenState::Faulted: return L"Faulted";
			case DmaStreamComputeTokenState::Canceled: return L"Canceled";
			default: return L"Unknown";
			}
		}
	}

	DmaStreamComputeFaultRecord::DmaStreamComputeFaultRecord(DmaStreamComputeTokenFaultKind kind,const std::wstring& message,unsigned long long address,bool write,int vt,unsigned long long ownerTag,unsigned long long certificate,DmaStreamComputeFaultSourcePhase phase,bool normalized,std::optional<std::wstring> hostType,DmaStreamComputeFaultPublicationContract contract) : FaultKind(kind), Message(message.empty()?L"DmaStreamCompute token fault.":message), FaultAddress(address), IsWrite(write), VirtualThreadId(vt), OwnerDomainTag(ownerTag), ActiveDomainCertificate(certificate), SourcePhase(phase), BackendExceptionNormalized(normalized), NormalizedHostExceptionType(std::move(hostType)), PublicationContract(contract) { if(kind==DmaStreamComputeTokenFaultKind::None) throw std::invalid_argument("faultKind"); }
	std::runtime_error DmaStreamComputeFaultRecord::CreateRetireException() const { return std::runtime_error(std::string(Message.begin(),Message.end())); }
	std::shared_ptr<DmaStreamComputeFaultRecord> DmaStreamComputeFaultRecord::FromBackendException(std::shared_ptr<DmaStreamComputeDescriptor> descriptor,std::runtime_error exception,unsigned long long faultAddress,bool isWrite,DmaStreamComputeFaultSourcePhase phase) { auto& context=descriptor->OwnerGuardDecision->RuntimeOwnerContext; return std::make_shared<DmaStreamComputeFaultRecord>(DmaStreamComputeTokenFaultKind::DmaDeviceFault,L"DmaStreamCompute backend exception normalized before retire publication.",faultAddress,isWrite,descriptor->OwnerBinding->OwnerVirtualThreadId,descriptor->OwnerBinding->OwnerDomainTag,context.ActiveDomainCertificate,phase,true,std::optional<std::wstring>(L"std::runtime_error"),DmaStreamComputeFaultPublicationContract::FuturePreciseRetireRequiresPublicationMetadata); }
	bool DmaStreamComputeCommitResult::getRequiresRetireExceptionPublication() const { return Fault.has_value() && Fault.value()->getRequiresRetireExceptionPublication(); }
	std::runtime_error DmaStreamComputeCommitResult::CreateRetireException() const
	{
		if (!Fault || !Fault.value())
			throw std::logic_error("A successful, pending, or canceled DMA stream compute result has no retire exception.");
		return Fault.value()->CreateRetireException();
	}
	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeCommitResult::Pending(DmaStreamComputeTokenState state) { return std::make_shared<DmaStreamComputeCommitResult>(false,false,state); }
	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeCommitResult::Success() { return std::make_shared<DmaStreamComputeCommitResult>(true,false,DmaStreamComputeTokenState::Committed); }
	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeCommitResult::Canceled() { return std::make_shared<DmaStreamComputeCommitResult>(false,true,DmaStreamComputeTokenState::Canceled); }
	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeCommitResult::Faulted(std::shared_ptr<DmaStreamComputeFaultRecord> fault) { return std::make_shared<DmaStreamComputeCommitResult>(false,false,DmaStreamComputeTokenState::Faulted,std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>>(std::move(fault))); }

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamComputeToken(DmaStreamComputeDescriptor descriptor, ulong tokenId, DmaStreamComputeTelemetryCounters? telemetry = null)
	DmaStreamComputeToken::DmaStreamComputeToken(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, unsigned long long tokenId, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry) : TokenId(tokenId), Descriptor(descriptor), _telemetry(telemetry)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		setState(DmaStreamComputeTokenState::Admitted);

		std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> fault;
		if (TryValidateDescriptorGuards(descriptor, descriptor->OwnerGuardDecision, L"DmaStreamCompute token admission", fault))
		{
			return;
		}

		setState(DmaStreamComputeTokenState::Faulted);
		setLastFault(fault);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: throw new InvalidOperationException(fault!.Message);
		if (fault.has_value() && fault.value())
			throw InvalidOperationException(fault.value()->Message);
		throw InvalidOperationException(L"DmaStreamCompute token admission failed its owner/domain guard validation.");
	}

	unsigned long long DmaStreamComputeToken::getTokenId() const
	{
		return TokenId;
	}

	std::shared_ptr<DmaStreamComputeDescriptor> DmaStreamComputeToken::getDescriptor() const
	{
		return Descriptor;
	}

	DmaStreamComputeTokenState DmaStreamComputeToken::getState() const
	{
		return State;
	}

	void DmaStreamComputeToken::setState(DmaStreamComputeTokenState value)
	{
		State = value;
	}

	std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> DmaStreamComputeToken::getLastFault() const
	{
		return LastFault;
	}

	void DmaStreamComputeToken::setLastFault(const std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> &value)
	{
		LastFault = value;
	}

	DmaStreamComputeTokenCancelReason DmaStreamComputeToken::getCancelReason() const
	{
		return CancelReason;
	}

	void DmaStreamComputeToken::setCancelReason(DmaStreamComputeTokenCancelReason value)
	{
		CancelReason = value;
	}

	bool DmaStreamComputeToken::getHasCommitted() const
	{
		return getState() == DmaStreamComputeTokenState::Committed;
	}

	int DmaStreamComputeToken::getStagedWriteCount() const
	{
		return _stagedWrites.size();
	}

	std::shared_ptr<DmaStreamComputeProgressDiagnostics> DmaStreamComputeToken::getProgressDiagnostics() const
	{
		return _progressDiagnostics;
	}

	const std::vector<std::shared_ptr<DmaStreamComputeStagedWrite>> DmaStreamComputeToken::GetStagedWriteSnapshot()
	{
		return _stagedWrites;
	}

	std::shared_ptr<DmaStreamComputeTokenLifecycleEvidence> DmaStreamComputeToken::ExportLifecycleEvidence()
	{
		return std::make_shared<DmaStreamComputeTokenLifecycleEvidence>(DmaStreamComputeTokenLifecycleEvidence::FromToken(shared_from_this()));
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeTokenAdmissionResult TryAdmit(DmaStreamComputeValidationResult validationResult, ulong tokenId, DmaStreamComputeTelemetryCounters? telemetry = null)
	std::shared_ptr<DmaStreamComputeTokenAdmissionResult> DmaStreamComputeToken::TryAdmit(std::shared_ptr<DmaStreamComputeValidationResult> validationResult, unsigned long long tokenId, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		ArgumentNullException::ThrowIfNull(validationResult);

		if (validationResult->IsDescriptorAbiAccepted && validationResult->Descriptor != nullptr)
		{
			auto token = std::make_shared<DmaStreamComputeToken>(validationResult->Descriptor, tokenId, telemetry);
			if (telemetry != nullptr)
				telemetry->RecordTokenAccepted(token);
			return DmaStreamComputeTokenAdmissionResult::Accepted(std::move(token));
		}

		if (IsTelemetryOnlyAdmissionReject(validationResult->Fault))
		{
			if (telemetry != nullptr)
			{
				telemetry->RecordTokenAdmissionFault(validationResult->Fault);
			}
			return DmaStreamComputeTokenAdmissionResult::TelemetryReject(validationResult->Fault, validationResult->Message);
		}

		if (telemetry != nullptr)
		{
			telemetry->RecordTokenAdmissionFault(validationResult->Fault);
		}
		std::shared_ptr<DmaStreamComputeFaultRecord> fault = CreateFaultFromValidationFailure(validationResult->Fault, validationResult->Message);
		return DmaStreamComputeTokenAdmissionResult::ArchitecturalFault(validationResult->Fault, fault, validationResult->Message);
	}

	void DmaStreamComputeToken::MarkIssued()
	{
		EnsureState(DmaStreamComputeTokenState::Admitted, L"MarkIssued");
		setState(DmaStreamComputeTokenState::Issued);
		if (_telemetry != nullptr)
		{
			_telemetry->RecordTokenActive(shared_from_this());
		}
	}

	std::shared_ptr<DmaStreamComputeProgressDiagnostics> DmaStreamComputeToken::RecordProgressDiagnostics(unsigned long long bytesRead, unsigned long long bytesStaged, unsigned long long elementOperations, unsigned long long modeledLatencyCycles, unsigned long long backendStepCount)
	{
		_progressDiagnostics = _progressDiagnostics->Add(bytesRead, bytesStaged, elementOperations, modeledLatencyCycles, backendStepCount);
		return _progressDiagnostics;
	}

	void DmaStreamComputeToken::MarkReadsComplete()
	{
		EnsureState(DmaStreamComputeTokenState::Issued, L"MarkReadsComplete");
		setState(DmaStreamComputeTokenState::ReadsComplete);
	}

	void DmaStreamComputeToken::StageDestinationWrite(unsigned long long address, std::span<const unsigned char> data)
	{
		EnsureState(DmaStreamComputeTokenState::ReadsComplete, L"StageDestinationWrite");
		if (data.empty() || address > std::numeric_limits<unsigned long long>::max() - static_cast<unsigned long long>(data.size()) || !IsRangeCoveredByFootprint(address, static_cast<unsigned long long>(data.size()), getDescriptor()->NormalizedWriteMemoryRanges))
		{
			std::shared_ptr<DmaStreamComputeFaultRecord> fault = CreateFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, L"DmaStreamCompute staged destination write is outside the accepted normalized write footprint.", address, true);
			PublishFault(fault);
			throw InvalidOperationException(fault->Message);
		}

		_stagedWrites.push_back(std::make_shared<DmaStreamComputeStagedWrite>(address, data));
		if (_telemetry != nullptr)
		{
			_telemetry->RecordTokenStaged(shared_from_this(), static_cast<unsigned long long>(data.size()));
		}
	}

	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeToken::MarkComputeComplete()
	{
		if (getState() == DmaStreamComputeTokenState::Faulted)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return DmaStreamComputeCommitResult.Faulted(LastFault!);
			const auto lastFault = getLastFault();
			if (lastFault.has_value() && lastFault.value())
				return DmaStreamComputeCommitResult::Faulted(lastFault.value());
			return DmaStreamComputeCommitResult::Faulted(CreateFault(DmaStreamComputeTokenFaultKind::MemoryFault, L"DmaStreamCompute token entered a faulted state without a retained fault record.", 0, false));
		}

		EnsureState(DmaStreamComputeTokenState::ReadsComplete, L"MarkComputeComplete");
		setState(DmaStreamComputeTokenState::ComputeComplete);

		unsigned long long faultAddress;
		std::wstring failureMessage;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!HasExactStagedWriteCoverage(out ulong faultAddress, out string? failureMessage))
		if (!HasExactStagedWriteCoverage(faultAddress, failureMessage))
		{
			return PublishFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, (failureMessage != L"") ? failureMessage: L"DmaStreamCompute staged destination writes do not exactly cover the normalized write footprint.", faultAddress, true);
		}

		setState(DmaStreamComputeTokenState::CommitPending);
		return DmaStreamComputeCommitResult::Pending(getState());
	}

	void DmaStreamComputeToken::Cancel(DmaStreamComputeTokenCancelReason reason)
	{
		if (getState() == DmaStreamComputeTokenState::Committed)
		{
			throw InvalidOperationException(L"DmaStreamCompute token cannot be canceled after architectural commit.");
		}

		setCancelReason(reason == DmaStreamComputeTokenCancelReason::None ? DmaStreamComputeTokenCancelReason::Flush : reason);
		_stagedWrites.clear();
		setState(DmaStreamComputeTokenState::Canceled);
		if (_telemetry != nullptr)
		{
			_telemetry->RecordTokenCanceled(shared_from_this());
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DmaStreamComputeCommitResult Commit(Processor.MainMemoryArea mainMemory, DmaStreamComputeOwnerGuardDecision commitGuardDecision, MemoryCoherencyObserver? coherencyObserver = null)
	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeToken::Commit(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> commitGuardDecision, std::shared_ptr<MemoryCoherencyObserver> coherencyObserver)
	{
		ArgumentNullException::ThrowIfNull(mainMemory);

		if (getState() == DmaStreamComputeTokenState::Canceled)
		{
			return DmaStreamComputeCommitResult::Canceled();
		}

		if (getState() == DmaStreamComputeTokenState::Faulted)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return DmaStreamComputeCommitResult.Faulted(LastFault!);
			const auto lastFault = getLastFault();
			if (lastFault.has_value() && lastFault.value())
				return DmaStreamComputeCommitResult::Faulted(lastFault.value());
			return DmaStreamComputeCommitResult::Faulted(CreateFault(DmaStreamComputeTokenFaultKind::MemoryFault, L"DmaStreamCompute token entered a faulted state without a retained fault record.", 0, false));
		}

		EnsureState(DmaStreamComputeTokenState::CommitPending, L"Commit");

		std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> guardFault;
		if (!TryValidateDescriptorGuards(getDescriptor(), commitGuardDecision, L"DmaStreamCompute token commit", guardFault))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return PublishFault(guardFault!);
			return PublishFault(guardFault.value());
		}

		unsigned long long partialFaultAddress;
		std::wstring failureMessage;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!HasExactStagedWriteCoverage(out ulong partialFaultAddress, out string? failureMessage))
		if (!HasExactStagedWriteCoverage(partialFaultAddress, failureMessage))
		{
			return PublishFault(DmaStreamComputeTokenFaultKind::PartialCompletionFault, (failureMessage != L"") ? failureMessage: L"DmaStreamCompute commit reached an incomplete staged destination buffer.", partialFaultAddress, true);
		}

		std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> memoryFault;
		if (!TryCommitAllOrNone(mainMemory, memoryFault))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return PublishFault(memoryFault!);
			return PublishFault(memoryFault.value());
		}

		NotifyCommittedWrites(coherencyObserver);
		setState(DmaStreamComputeTokenState::Committed);
		if (_telemetry != nullptr)
		{
			_telemetry->RecordTokenCommitted(shared_from_this());
		}
		return DmaStreamComputeCommitResult::Success();
	}

	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeToken::PublishFault(DmaStreamComputeTokenFaultKind faultKind, const std::wstring &message, unsigned long long faultAddress, bool isWrite)
	{
		return PublishFault(CreateFault(faultKind, message, faultAddress, isWrite));
	}

	std::shared_ptr<DmaStreamComputeCommitResult> DmaStreamComputeToken::PublishFault(std::shared_ptr<DmaStreamComputeFaultRecord> fault)
	{
		ArgumentNullException::ThrowIfNull(fault);
		setLastFault(fault);
		_stagedWrites.clear();
		setState(DmaStreamComputeTokenState::Faulted);
		if (_telemetry != nullptr)
		{
			_telemetry->RecordTokenFaulted(shared_from_this(), fault->FaultKind);
		}
		return DmaStreamComputeCommitResult::Faulted(fault);
	}

	bool DmaStreamComputeToken::IsTelemetryOnlyAdmissionReject(DmaStreamComputeValidationFault fault)
	{
		return (fault == DmaStreamComputeValidationFault::QuotaAdmissionReject || fault == DmaStreamComputeValidationFault::BackpressureAdmissionReject || fault == DmaStreamComputeValidationFault::TokenCapAdmissionReject);
	}

	bool DmaStreamComputeToken::TryCommitAllOrNone(std::shared_ptr<Processor::MainMemoryArea> mainMemory, std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> &fault)
	{
		fault = std::nullopt;
		std::vector<std::shared_ptr<DmaStreamComputeStagedWrite>> backups;
		backups.reserve(_stagedWrites.size());
		for (int i = 0; i < _stagedWrites.size(); i++)
		{
			std::shared_ptr<DmaStreamComputeStagedWrite> stagedWrite = _stagedWrites[i];
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int length = checked((int)stagedWrite.Length);
			int length = static_cast<int>(stagedWrite->Length);
			if (!HasExactMainMemoryRange(mainMemory, stagedWrite->Address, length))
			{
				fault = CreateFault(DmaStreamComputeTokenFaultKind::MemoryFault, std::format(L"DmaStreamCompute token commit reached out-of-range destination 0x{:X} covering {} byte(s).", stagedWrite->Address, length), stagedWrite->Address, true);
				return false;
			}

			std::vector<unsigned char> backup(length);
			if (!mainMemory->TryReadPhysicalRange(stagedWrite->Address, backup))
			{
				fault = CreateFault(DmaStreamComputeTokenFaultKind::MemoryFault, std::format(L"DmaStreamCompute token commit could not snapshot destination 0x{:X} before all-or-none commit.", stagedWrite->Address), stagedWrite->Address, true);
				return false;
			}

			backups.push_back(std::make_shared<DmaStreamComputeStagedWrite>(stagedWrite->Address, backup));
		}

		for (int i = 0; i < _stagedWrites.size(); i++)
		{
			std::shared_ptr<DmaStreamComputeStagedWrite> stagedWrite = _stagedWrites[i];
			if (mainMemory->TryWritePhysicalRange(stagedWrite->Address, std::span<const unsigned char>(stagedWrite->Data)))
			{
				continue;
			}

			for (int rollbackIndex = i; rollbackIndex >= 0; rollbackIndex--)
			{
				std::shared_ptr<DmaStreamComputeStagedWrite> backup = backups[rollbackIndex];
				if (!mainMemory->TryWritePhysicalRange(backup->Address, std::span<const unsigned char>(backup->Data)))
				{
		throw InvalidOperationException(std::format(L"DmaStreamCompute token commit failed to roll back destination 0x{:X} after a partial write failure.", backup->Address));
				}
			}

			fault = CreateFault(DmaStreamComputeTokenFaultKind::MemoryFault, std::format(L"DmaStreamCompute token commit failed to write destination 0x{:X}; staged writes were not reported as visible success.", stagedWrite->Address), stagedWrite->Address, true);
			return false;
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private void NotifyCommittedWrites(MemoryCoherencyObserver? coherencyObserver)
	void DmaStreamComputeToken::NotifyCommittedWrites(std::shared_ptr<MemoryCoherencyObserver> coherencyObserver)
	{
		if (coherencyObserver == nullptr)
		{
			return;
		}

		const unsigned long long ownerDomainTag = getDescriptor()->OwnerBinding->OwnerDomainTag;
		for (int index = 0; index < _stagedWrites.size(); index++)
		{
			std::shared_ptr<DmaStreamComputeStagedWrite> stagedWrite = _stagedWrites[index];
			coherencyObserver->NotifyWrite(std::make_shared<MemoryCoherencyWriteNotification>(stagedWrite->Address, stagedWrite->Length, ownerDomainTag, MemoryCoherencyWriteSourceKind::DmaStreamComputeCommit));
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private bool HasExactStagedWriteCoverage(out ulong faultAddress, out string? failureMessage)
	bool DmaStreamComputeToken::HasExactStagedWriteCoverage(unsigned long long &faultAddress, std::wstring &failureMessage)
	{
		const auto &descriptorRanges = getDescriptor()->NormalizedWriteMemoryRanges;
		faultAddress = !descriptorRanges.empty() ? descriptorRanges.front().Address : 0;
		failureMessage = L"";

		if (_stagedWrites.empty())
		{
			failureMessage = L"DmaStreamCompute all-or-none policy requires a staged write image for every normalized destination range.";
			return false;
		}

		std::vector<DmaStreamComputeMemoryRange> stagedRanges;
		stagedRanges.reserve(_stagedWrites.size());
		for (const auto &stagedWrite : _stagedWrites)
		{
			stagedRanges.emplace_back(DmaStreamComputeMemoryRange{stagedWrite->Address, stagedWrite->Length});
		}

		std::sort(stagedRanges.begin(), stagedRanges.end(), [](const auto &left, const auto &right)
		{
			return left.Address != right.Address ? left.Address < right.Address : left.Length < right.Length;
		});

		std::vector<DmaStreamComputeMemoryRange> mergedRanges;
		mergedRanges.reserve(stagedRanges.size());
		mergedRanges.push_back(stagedRanges.front());

		for (int i = 1; i < stagedRanges.size(); i++)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to the C# index operator:
			DmaStreamComputeMemoryRange &previous = mergedRanges.back();
			const unsigned long long previousEnd = previous.Address + previous.Length;
			if (stagedRanges[i].Address < previousEnd)
			{
				faultAddress = stagedRanges[i].Address;
				failureMessage = L"DmaStreamCompute staged destination writes overlap; v1 commit policy is all-or-none.";
				return false;
			}

			if (stagedRanges[i].Address == previousEnd)
			{
				previous.Length += stagedRanges[i].Length;
				continue;
			}

			mergedRanges.push_back(stagedRanges[i]);
		}

		if (mergedRanges.size() != descriptorRanges.size())
		{
			failureMessage = L"DmaStreamCompute staged destination writes do not cover every normalized descriptor write range.";
			return false;
		}

		for (int i = 0; i < mergedRanges.size(); i++)
		{
			const DmaStreamComputeMemoryRange &expected = descriptorRanges[i];
			if (mergedRanges[i].Address == expected.Address && mergedRanges[i].Length == expected.Length)
			{
				continue;
			}

			faultAddress = expected.Address;
			failureMessage = L"DmaStreamCompute staged destination writes do not exactly match the normalized descriptor write footprint.";
			return false;
		}

		return true;
	}

	void DmaStreamComputeToken::EnsureState(DmaStreamComputeTokenState expectedState, const std::wstring &operation)
	{
		if (getState() == expectedState)
		{
			return;
		}

		throw InvalidOperationException(std::format(L"{} requires DmaStreamCompute token state {}, but token {} is {}.", operation, TokenStateName(expectedState), getTokenId(), TokenStateName(getState())));
	}

	std::shared_ptr<DmaStreamComputeFaultRecord> DmaStreamComputeToken::CreateFault(DmaStreamComputeTokenFaultKind faultKind, const std::wstring &message, unsigned long long faultAddress, bool isWrite)
	{
		const auto descriptor = getDescriptor();
		const auto &guardContext = descriptor->OwnerGuardDecision->RuntimeOwnerContext;
		return std::make_shared<DmaStreamComputeFaultRecord>(faultKind, message, faultAddress, isWrite,
			descriptor->OwnerBinding->OwnerVirtualThreadId, descriptor->OwnerBinding->OwnerDomainTag,
			guardContext.ActiveDomainCertificate);
	}

	std::shared_ptr<DmaStreamComputeFaultRecord> DmaStreamComputeToken::CreateFaultFromValidationFailure(DmaStreamComputeValidationFault validationFault, const std::wstring &message)
	{
		DmaStreamComputeTokenFaultKind tokenFault = DmaStreamComputeTokenFaultKind::DescriptorDecodeFault;
		switch (validationFault)
		{
		case DmaStreamComputeValidationFault::UnsupportedAbiVersion:
		case DmaStreamComputeValidationFault::UnsupportedOperation:
		case DmaStreamComputeValidationFault::UnsupportedElementType:
		case DmaStreamComputeValidationFault::UnsupportedShape:
			tokenFault = DmaStreamComputeTokenFaultKind::UnsupportedAbiOrOperation;
			break;
		case DmaStreamComputeValidationFault::AlignmentFault:
			tokenFault = DmaStreamComputeTokenFaultKind::AlignmentFault;
			break;
		case DmaStreamComputeValidationFault::AliasOverlapFault:
			tokenFault = DmaStreamComputeTokenFaultKind::AliasOverlapFault;
			break;
		case DmaStreamComputeValidationFault::OwnerDomainFault:
			tokenFault = DmaStreamComputeTokenFaultKind::DomainViolation;
			break;
		case DmaStreamComputeValidationFault::ExecutionDisabled:
			tokenFault = DmaStreamComputeTokenFaultKind::ExecutionDisabled;
			break;
		default:
			break;
		}
		return std::make_shared<DmaStreamComputeFaultRecord>(tokenFault, message, 0, false, 0, 0, 0);
	}

	bool DmaStreamComputeToken::TryValidateDescriptorGuards(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> guardDecision, const std::wstring &surface, std::optional<std::shared_ptr<DmaStreamComputeFaultRecord>> &fault)
	{
		fault = std::nullopt;

		if (descriptor->PartialCompletionPolicy != DmaStreamComputePartialCompletionPolicy::AllOrNone)
		{
			fault = CreateDescriptorFault(descriptor, DmaStreamComputeTokenFaultKind::PartialCompletionFault, std::format(L"{} requires all-or-none partial completion policy.", surface));
			return false;
		}

		if (descriptor->WriteMemoryRanges.empty() || descriptor->NormalizedWriteMemoryRanges.empty())
		{
			fault = CreateDescriptorFault(descriptor, DmaStreamComputeTokenFaultKind::PartialCompletionFault, std::format(L"{} requires a mandatory normalized write footprint before token authority exists.", surface));
			return false;
		}

		if (!guardDecision->IsAllowed)
		{
			fault = CreateGuardFault(descriptor, guardDecision, guardDecision->LegalityDecision.getRejectKind() == RejectKind::DomainMismatch ? DmaStreamComputeTokenFaultKind::DomainViolation : DmaStreamComputeTokenFaultKind::OwnerContextViolation, std::format(L"{} requires an allowed owner/domain guard decision. {}", surface, guardDecision->Message));
			return false;
		}

		if (guardDecision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || guardDecision->LegalityDecision.getAttemptedReplayCertificateReuse())
		{
			fault = CreateGuardFault(descriptor, guardDecision, DmaStreamComputeTokenFaultKind::OwnerContextViolation, std::format(L"{} requires guard-plane authority; replay/certificate identity is evidence, not commit authority.", surface));
			return false;
		}

		const auto &acceptedBinding = guardDecision->DescriptorOwnerBinding;
		const auto &descriptorBinding = descriptor->OwnerBinding;
		const bool sameOwnerBinding = acceptedBinding && descriptorBinding &&
			acceptedBinding->OwnerVirtualThreadId == descriptorBinding->OwnerVirtualThreadId &&
			acceptedBinding->OwnerContextId == descriptorBinding->OwnerContextId &&
			acceptedBinding->OwnerCoreId == descriptorBinding->OwnerCoreId &&
			acceptedBinding->OwnerPodId == descriptorBinding->OwnerPodId &&
			acceptedBinding->OwnerDomainTag == descriptorBinding->OwnerDomainTag &&
			acceptedBinding->DeviceId == descriptorBinding->DeviceId;
		if (!sameOwnerBinding)
		{
			fault = CreateGuardFault(descriptor, guardDecision, DmaStreamComputeTokenFaultKind::OwnerContextViolation, std::format(L"{} owner binding does not match the accepted descriptor.", surface));
			return false;
		}

		const auto &context = guardDecision->RuntimeOwnerContext;
		if (descriptor->OwnerBinding->OwnerVirtualThreadId != context.OwnerVirtualThreadId || descriptor->OwnerBinding->OwnerContextId != context.OwnerContextId || descriptor->OwnerBinding->OwnerCoreId != context.OwnerCoreId || descriptor->OwnerBinding->OwnerPodId != context.OwnerPodId)
		{
			fault = CreateGuardFault(descriptor, guardDecision, DmaStreamComputeTokenFaultKind::OwnerContextViolation, std::format(L"{} owner/context/core/pod guard changed before token commit.", surface));
			return false;
		}

		if (descriptor->OwnerBinding->DeviceId != DmaStreamComputeDescriptor::CanonicalLane6DeviceId || descriptor->OwnerBinding->DeviceId != context.DeviceId)
		{
			fault = CreateGuardFault(descriptor, guardDecision, DmaStreamComputeTokenFaultKind::DmaDeviceFault, std::format(L"{} device guard changed before token commit.", surface));
			return false;
		}

		if (descriptor->OwnerBinding->OwnerDomainTag != context.OwnerDomainTag || !IsDomainCoveredByCertificate(context.OwnerDomainTag, context.ActiveDomainCertificate))
		{
			fault = CreateGuardFault(descriptor, guardDecision, DmaStreamComputeTokenFaultKind::DomainViolation, std::format(L"{} owner/domain guard changed before token commit.", surface));
			return false;
		}

		return true;
	}

	bool DmaStreamComputeToken::IsDomainCoveredByCertificate(unsigned long long ownerDomainTag, unsigned long long activeDomainCertificate)
	{
		if (ownerDomainTag == 0)
		{
			return activeDomainCertificate == 0;
		}

		return activeDomainCertificate == 0 || (ownerDomainTag & activeDomainCertificate) != 0;
	}

	std::shared_ptr<DmaStreamComputeFaultRecord> DmaStreamComputeToken::CreateDescriptorFault(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, DmaStreamComputeTokenFaultKind faultKind, const std::wstring &message)
	{
		const auto &normalizedWriteRanges = descriptor->NormalizedWriteMemoryRanges;
		const unsigned long long faultAddress = !normalizedWriteRanges.empty()
			? normalizedWriteRanges.front().Address
			: descriptor->DescriptorReference.DescriptorAddress;
		return std::make_shared<DmaStreamComputeFaultRecord>(faultKind, message, faultAddress, true,
			descriptor->OwnerBinding->OwnerVirtualThreadId,
			descriptor->OwnerBinding->OwnerDomainTag,
			descriptor->OwnerGuardDecision->RuntimeOwnerContext.ActiveDomainCertificate);
	}

	std::shared_ptr<DmaStreamComputeFaultRecord> DmaStreamComputeToken::CreateGuardFault(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> guardDecision, DmaStreamComputeTokenFaultKind faultKind, const std::wstring &message)
	{
		return std::make_shared<DmaStreamComputeFaultRecord>(faultKind, message, descriptor->DescriptorReference.DescriptorAddress, false, descriptor->OwnerBinding->OwnerVirtualThreadId, descriptor->OwnerBinding->OwnerDomainTag, guardDecision->RuntimeOwnerContext.ActiveDomainCertificate);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool IsRangeCoveredByFootprint(ulong address, ulong length, IReadOnlyList<DmaStreamComputeMemoryRange> footprint)
	bool DmaStreamComputeToken::IsRangeCoveredByFootprint(unsigned long long address, unsigned long long length, const std::vector<DmaStreamComputeMemoryRange> &footprint)
	{
		if (length == 0 || footprint.empty())
		{
			return false;
		}

		unsigned long long end = address + length;
		for (int i = 0; i < footprint.size(); i++)
		{
			const DmaStreamComputeMemoryRange& range = footprint[i];
			unsigned long long rangeEnd = range.Address + range.Length;
			if (address >= range.Address && end <= rangeEnd)
			{
				return true;
			}
		}

		return false;
	}

	bool DmaStreamComputeToken::HasExactMainMemoryRange(std::shared_ptr<Processor::MainMemoryArea> mainMemory, unsigned long long address, int size)
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



