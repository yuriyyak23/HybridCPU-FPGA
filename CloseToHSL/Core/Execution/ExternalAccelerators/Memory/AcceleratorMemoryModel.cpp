#include "AcceleratorMemoryModel.h"
#include "../Tokens/AcceleratorTokenStatusWord.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "../Tokens/AcceleratorTokenStore.h"

using namespace YAKSys_Hybrid_CPU;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Memory
{

// C# TO C++ CONVERTER TASK: Throw expressions are not converted by C# to C++ Converter:
// ORIGINAL LINE: public MainMemoryReadOnlyAcceleratorMemoryPortal(Processor.MainMemoryArea mainMemory)
	MainMemoryReadOnlyAcceleratorMemoryPortal::MainMemoryReadOnlyAcceleratorMemoryPortal(std::shared_ptr<Processor::MainMemoryArea> mainMemory) : _mainMemory((mainMemory != nullptr) ? mainMemory : throw std::invalid_argument("mainMemory"))
	{
	}

	std::shared_ptr<AcceleratorMemoryPortalReadResult> MainMemoryReadOnlyAcceleratorMemoryPortal::ReadSourceRanges(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(descriptor);
		if ((token->getDescriptor() != descriptor && !token->getDescriptor()->Equals(descriptor)) || token->getState() != AcceleratorTokenState::Running)
		{
			return AcceleratorMemoryPortalReadResult::Reject(AcceleratorTokenFaultCode::SourceReadRejected, nullptr, L"L7-SDC source reads require the token-bound descriptor in Running state; descriptor or token identity alone is not device-read authority.");
		}

		std::shared_ptr<AcceleratorGuardDecision> tokenGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!tokenGuardDecision->IsAllowed)
		{
			return AcceleratorMemoryPortalReadResult::Reject(AcceleratorTokenStore::MapGuardFault(tokenGuardDecision->Fault), tokenGuardDecision, L"L7-SDC source reads require fresh token-bound owner/domain and epoch authority. " + tokenGuardDecision->Message);
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeDeviceExecution(descriptor, currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return AcceleratorMemoryPortalReadResult::Reject(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), guardDecision, L"L7-SDC source reads require fresh guard-backed device execution authority. " + guardDecision->Message);
		}

		if (tokenGuardDecision->DescriptorOwnerBinding == nullptr || guardDecision->DescriptorOwnerBinding == nullptr || !tokenGuardDecision->DescriptorOwnerBinding->Equals(guardDecision->DescriptorOwnerBinding))
		{
			return AcceleratorMemoryPortalReadResult::Reject(AcceleratorTokenFaultCode::SourceReadRejected, guardDecision, L"L7-SDC source read token guard and device execution guard must bind the same descriptor owner.");
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRead>> reads;
		reads.reserve(descriptor->SourceRanges.size());
		for (int index = 0; index < descriptor->SourceRanges.size(); index++)
		{
			std::shared_ptr<AcceleratorMemoryRange> range = descriptor->SourceRanges[index];
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: if (range.Length > int.MaxValue || !HasExactMainMemoryRange(range.Address, checked((int)range.Length)))
			if (range->Length > std::numeric_limits<int>::max() || !HasExactMainMemoryRange(range->Address, static_cast<int>(range->Length)))
			{
				return AcceleratorMemoryPortalReadResult::Reject(AcceleratorTokenFaultCode::SourceReadRejected, guardDecision, std::format(L"L7-SDC read-only memory portal rejected out-of-range source read at 0x{:X} for {} byte(s).", range->Address, range->Length));
			}

			std::vector<unsigned char> buffer(static_cast<int>(range->Length));
			if (!_mainMemory->TryReadPhysicalRange(range->Address, buffer))
			{
				return AcceleratorMemoryPortalReadResult::Reject(AcceleratorTokenFaultCode::SourceReadRejected, guardDecision, std::format(L"L7-SDC read-only memory portal could not read source range at 0x{:X} for {} byte(s).", range->Address, range->Length));
			}

			reads.push_back(std::make_shared<AcceleratorMemoryRead>(range, buffer));
		}

		return AcceleratorMemoryPortalReadResult::Accepted(reads, guardDecision, L"L7-SDC source ranges read through a guarded read-only memory portal.");
	}

	bool MainMemoryReadOnlyAcceleratorMemoryPortal::HasExactMainMemoryRange(unsigned long long address, int size)
	{
		if (size <= 0)
		{
			return false;
		}

		unsigned long long memoryLength = static_cast<unsigned long long>(_mainMemory->getLength());
		unsigned long long requestSize = static_cast<unsigned long long>(size);
		return requestSize <= memoryLength && address <= memoryLength - requestSize;
	}

	int AcceleratorStagingBuffer::getTotalStagedWriteCount() const
	{
		int count = 0;
		for (auto writes : _writesByHandle)
		{
			count += static_cast<int>(writes.second.size());
		}

		return count;
	}

	std::shared_ptr<AcceleratorStagingResult> AcceleratorStagingBuffer::StageWrite(std::shared_ptr<AcceleratorToken> token, std::shared_ptr<AcceleratorMemoryRange> destinationRange, std::span<const unsigned char> data, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);
		ArgumentNullException::ThrowIfNull(destinationRange);
		if (token->getState() != AcceleratorTokenState::Running)
		{
			return AcceleratorStagingResult::Reject(AcceleratorTokenFaultCode::StagingRejected, nullptr, L"L7-SDC staged writes require a Running token produced by guarded queue/backend execution.");
		}

		std::shared_ptr<AcceleratorGuardDecision> guardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return AcceleratorStagingResult::Reject(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), guardDecision, L"L7-SDC staged writes require fresh token owner/domain and epoch authority. " + guardDecision->Message);
		}

		if (static_cast<unsigned long long>(data.size()) != destinationRange->Length || !IsRangeCoveredByFootprint(destinationRange, token->getDescriptor()->NormalizedFootprint.DestinationRanges))
		{
			return AcceleratorStagingResult::Reject(AcceleratorTokenFaultCode::StagingRejected, guardDecision, L"L7-SDC fake backend staged write must exactly target the accepted normalized destination footprint.");
		}

		std::vector<unsigned char> copy(data.begin(), data.end());
		std::shared_ptr<AcceleratorStagedWrite> stagedWrite = std::make_shared<AcceleratorStagedWrite>(token->getHandle(), destinationRange->Address, copy);
		auto [writesIterator, inserted] = _writesByHandle.try_emplace(token->getHandle()->Value);
		writesIterator->second.push_back(stagedWrite);
		return AcceleratorStagingResult::Accepted(stagedWrite, guardDecision, L"L7-SDC fake backend staged bytes into backend-private staging only; no architectural memory was published.");
	}

	std::shared_ptr<AcceleratorStagingReadResult> AcceleratorStagingBuffer::GetStagedWriteSet(std::shared_ptr<AcceleratorToken> token, std::optional<std::shared_ptr<AcceleratorGuardEvidence>> &currentGuardEvidence)
	{
		ArgumentNullException::ThrowIfNull(token);
		std::shared_ptr<AcceleratorGuardDecision> guardDecision = AcceleratorOwnerDomainGuard::getDefault()->ValidateMappingEpoch(token->getSubmitGuardDecision(), currentGuardEvidence);
		if (!guardDecision->IsAllowed)
		{
			return AcceleratorStagingReadResult::Reject(AcceleratorTokenStore::MapGuardFault(guardDecision->Fault), guardDecision, L"L7-SDC staging observation requires fresh token owner/domain and epoch authority. " + guardDecision->Message);
		}

		const auto writesIterator = _writesByHandle.find(token->getHandle()->Value);
		if (!token->getHandle()->IsValid || writesIterator == _writesByHandle.end() || writesIterator->second.empty())
		{
			return AcceleratorStagingReadResult::Accepted(std::vector<std::shared_ptr<AcceleratorStagedWrite>>{}, guardDecision, L"L7-SDC staging buffer has no staged writes for the guarded token.");
		}

		return AcceleratorStagingReadResult::Accepted(writesIterator->second, guardDecision, L"L7-SDC staging write-set observed after token guard revalidation; data remains non-architectural.");
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool IsRangeCoveredByFootprint(AcceleratorMemoryRange range, IReadOnlyList<AcceleratorMemoryRange> footprint)
	bool AcceleratorStagingBuffer::IsRangeCoveredByFootprint(std::shared_ptr<AcceleratorMemoryRange> range, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &footprint)
	{
		if (range->Length == 0 || range->Address > std::numeric_limits<unsigned long long>::max() - range->Length || footprint.empty() || footprint.empty())
		{
			return false;
		}

		unsigned long long end = range->Address + range->Length;
		for (int index = 0; index < footprint.size(); index++)
		{
			std::shared_ptr<AcceleratorMemoryRange> accepted = footprint[index];
			if (accepted->Length == 0 || accepted->Address > std::numeric_limits<unsigned long long>::max() - accepted->Length)
			{
				continue;
			}

			unsigned long long acceptedEnd = accepted->Address + accepted->Length;
			if (range->Address >= accepted->Address && end <= acceptedEnd)
			{
				return true;
			}
		}

		return false;
	}
}
