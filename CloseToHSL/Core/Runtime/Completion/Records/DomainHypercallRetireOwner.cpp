#include "DomainHypercallRetireOwner.h"
#include "../../../Decoder/Sha256Digest.h"
#include <algorithm>
#include <stdexcept>
#include <limits>
#include "../../../Pipeline/MicroOps/Types/MicroOp.IO.h"
#include "../../../Pipeline/Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../../../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../../Migration/Checkpoint/DomainHypercallLifecycleGate.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DomainHypercallRetireOwner::VirtualizationRetireGrant::VirtualizationRetireGrant(DomainHypercallIdentitySealToken issuerSeal, unsigned long long sequence, std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> e5, std::shared_ptr<DomainHypercallRetireEligibility> eligibility, const std::wstring &digest)
		: Sequence(sequence), AttemptId(e5->getAttemptId()), VirtualThreadId(eligibility->VirtualThreadId), DomainTag(eligibility->DomainTag), SourceSlotId(eligibility->SourceSlotId), WorkingSlotId(eligibility->WorkingSlotId), WorkingBundleSequence(eligibility->WorkingBundleSequence), OperationAttempt(eligibility->OperationAttempt), PhysicalLaneId(eligibility->PhysicalLaneId), RetireWindowIdentity(eligibility->RetireWindowIdentity), OrderEpoch(eligibility->OrderEpoch), RestoreGeneration(e5->getRestoreGeneration()), E5Digest(e5->getTokenDigest()), GrantDigest(digest), _issuerSeal(std::move(issuerSeal))
	{
		ArgumentNullException::ThrowIfNull(_issuerSeal);
	}

	unsigned int DomainHypercallRetireOwner::VirtualizationRetireGrant::getSchemaVersion() const
	{
		return 1;
	}

	unsigned long long DomainHypercallRetireOwner::VirtualizationRetireGrant::getSequence() const
	{
		return Sequence;
	}

	unsigned long long DomainHypercallRetireOwner::VirtualizationRetireGrant::getAttemptId() const
	{
		return AttemptId;
	}

	int DomainHypercallRetireOwner::VirtualizationRetireGrant::getVirtualThreadId() const
	{
		return VirtualThreadId;
	}

	unsigned long long DomainHypercallRetireOwner::VirtualizationRetireGrant::getDomainTag() const
	{
		return DomainTag;
	}

	int DomainHypercallRetireOwner::VirtualizationRetireGrant::getSourceSlotId() const
	{
		return SourceSlotId;
	}

	int DomainHypercallRetireOwner::VirtualizationRetireGrant::getWorkingSlotId() const
	{
		return WorkingSlotId;
	}

	unsigned long long DomainHypercallRetireOwner::VirtualizationRetireGrant::getWorkingBundleSequence() const
	{
		return WorkingBundleSequence;
	}

	unsigned long long DomainHypercallRetireOwner::VirtualizationRetireGrant::getOperationAttempt() const
	{
		return OperationAttempt;
	}

	int DomainHypercallRetireOwner::VirtualizationRetireGrant::getPhysicalLaneId() const
	{
		return PhysicalLaneId;
	}

	unsigned long long DomainHypercallRetireOwner::VirtualizationRetireGrant::getRetireWindowIdentity() const
	{
		return RetireWindowIdentity;
	}

	unsigned long long DomainHypercallRetireOwner::VirtualizationRetireGrant::getOrderEpoch() const
	{
		return OrderEpoch;
	}

	unsigned long long DomainHypercallRetireOwner::VirtualizationRetireGrant::getRestoreGeneration() const
	{
		return RestoreGeneration;
	}

	std::wstring DomainHypercallRetireOwner::VirtualizationRetireGrant::getE5Digest() const
	{
		return E5Digest;
	}

	std::wstring DomainHypercallRetireOwner::VirtualizationRetireGrant::getGrantDigest() const
	{
		return GrantDigest;
	}

	bool DomainHypercallRetireOwner::VirtualizationRetireGrant::WasIssuedBy(const DomainHypercallIdentitySealToken &seal) const
	{
		return _issuerSeal != nullptr && _issuerSeal == seal;
	}

	std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant> DomainHypercallRetireOwner::VirtualizationRetireGrant::Create(DomainHypercallIdentitySealToken issuerSeal, unsigned long long sequence, std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> e5, std::shared_ptr<DomainHypercallRetireEligibility> eligibility, const std::wstring &digest)
	{
		return std::shared_ptr<DomainHypercallRetireOwner::VirtualizationRetireGrant>(new DomainHypercallRetireOwner::VirtualizationRetireGrant(std::move(issuerSeal), sequence, e5, eligibility, digest));
	}

	std::any DomainHypercallRetireOwner::getConsumerSeal() const
	{
		return std::any(_consumerSeal);
	}

	std::shared_ptr<DomainHypercallRetireResult> DomainHypercallRetireOwner::Issue(std::shared_ptr<DomainHypercallCompletionOwner> completionOwner, std::shared_ptr<DomainHypercallCompletionPublicationResult> publication, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, std::shared_ptr<DomainHypercallRetireEligibility> eligibility)
	{
		ArgumentNullException::ThrowIfNull(completionOwner);
		ArgumentNullException::ThrowIfNull(restoreOwner);
		if (publication == nullptr || eligibility == nullptr || !publication->IsPublished || publication->E5 == nullptr || publication->Completion == nullptr || publication->Completion->isEmpty())
		{
			return Deny(DomainHypercallRetireDecision::MissingCompletion, L"E6 requires one atomic live CompletionRecord/E5 publication.");
		}
		if (!eligibility->IsCanonicalHead || eligibility->RetireOrderIndex != 0 || eligibility->IsSquashed || eligibility->HasWinningException)
		{
			return Deny(DomainHypercallRetireDecision::NotCanonicalHead, L"E6 requires the unsquashed exception-free canonical retire head.");
		}
		if (eligibility->RetireWindowIdentity == 0 || eligibility->OrderEpoch == 0 || eligibility->OperationAttempt == 0 || eligibility->PhysicalLaneId < 0 || eligibility->PhysicalLaneId > 7)
		{
			return Deny(DomainHypercallRetireDecision::InvalidWindow, L"E6 retire-window/order identity is incomplete.");
		}

		std::shared_ptr<VmxMicroOp> carrier = eligibility->Carrier;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: SafetyVerifier.VirtualizationAdmissionCertificate? e1 = carrier == null ? null : carrier.VirtualizationAdmission;
		std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> e1 = carrier == nullptr ? nullptr : carrier->getVirtualizationAdmission();
		std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> e5 = publication->E5;
		std::shared_ptr<DomainHypercallCompletionPublicationResult> carrierPublication;
		if (carrier != nullptr)
		{
			auto exactPublication = carrier->getExactHypercallCompletionPublication();
			if (exactPublication.has_value())
			{
				carrierPublication = *exactPublication;
			}
		}
		if (carrier == nullptr || carrierPublication == nullptr || carrierPublication->E5 != e5 || e1 == nullptr || e1->getOperation() != VmxOperationKind::VmCall)
		{
			return Deny(DomainHypercallRetireDecision::InvalidCarrier, L"E6 requires the original exact VMCALL carrier and its E1/E5 chain.");
		}
		if (e5->getRestoreGeneration() != restoreOwner->getCurrentGeneration())
		{
			return Deny(DomainHypercallRetireDecision::StaleRestoreGeneration, L"E5 restore generation is stale at retire.");
		}
		if (e5->getAttemptId() != e1->getAttemptId() || e5->getVirtualThreadId() != eligibility->VirtualThreadId || e5->getDomainTag() != eligibility->DomainTag || e1->getVirtualThreadId() != eligibility->VirtualThreadId || e1->getDomainTag() != eligibility->DomainTag || e1->getSourceSlotId() != eligibility->SourceSlotId || e1->getWorkingSlotId() != eligibility->WorkingSlotId || e1->getBundleIdentity() != eligibility->WorkingBundleSequence)
		{
			return Deny(DomainHypercallRetireDecision::IdentityMismatch, L"E1/E5 and canonical WB attempt identity do not match.");
		}
		if (!completionOwner->ValidateLive(e5, publication->Completion, restoreOwner))
		{
			return Deny(DomainHypercallRetireDecision::DuplicateOrForeignE5, L"E5 is duplicate, stale, or no longer live.");
		}

		std::shared_ptr<DomainHypercallLifecycleGate::TransitionLease> transition;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!completionOwner.TryBeginE5ToE6Transition(e5, out DomainHypercallLifecycleGate.TransitionLease? transition) || transition == null)
		if (!completionOwner->TryBeginE5ToE6Transition(e5, transition) || transition == nullptr)
		{
			return Deny(DomainHypercallRetireDecision::LifecycleGateDenied, L"E5-to-E6 handoff denied by the disabled or draining lifecycle gate.");
		}

// C# TO C++ CONVERTER NOTE: The following 'using' block is replaced by its C++ equivalent:
// ORIGINAL LINE: using (transition)
		{
			{
				std::scoped_lock<std::mutex> lock(_sync);
				if (!completionOwner->ConsumeForRetire(e5, publication->Completion, restoreOwner, _consumerSeal))
				{
					return Deny(DomainHypercallRetireDecision::DuplicateOrForeignE5, L"E5 is duplicate, stale, or bound to another retire owner.");
				}

	#if defined(TESTING)
				transition->getLifecycleGate()->NotifyTransitionGapForTesting(DomainHypercallTransitionKind::E5ToE6);
	#endif
				unsigned long long sequence = AllocateSequence();
				std::wstring digest = ComputeDigest(sequence, e5, eligibility);
				std::shared_ptr<VirtualizationRetireGrant> grant = VirtualizationRetireGrant::Create(_issuerSeal, sequence, e5, eligibility, digest);
				_live.emplace(std::weak_ptr<VirtualizationRetireGrant>(grant), std::make_shared<LiveGrant>(digest));
				_liveGrants.insert(grant);
				return std::make_shared<DomainHypercallRetireResult>(DomainHypercallRetireDecision::Issued, grant, L"Canonical CPU retire owner issued one opaque no-state E6.");
			}
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ConsumeAtPreciseRetire(VirtualizationRetireGrant? grant, VirtualizationRestoreGenerationOwner restoreOwner, ulong retireWindowIdentity, ulong orderEpoch)
	bool DomainHypercallRetireOwner::ConsumeAtPreciseRetire(std::shared_ptr<VirtualizationRetireGrant> grant, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, unsigned long long retireWindowIdentity, unsigned long long orderEpoch)
	{
		ArgumentNullException::ThrowIfNull(restoreOwner);
		{
			std::scoped_lock<std::mutex> lock(_sync);
			const std::weak_ptr<VirtualizationRetireGrant> weakGrant(grant);
			auto live = grant ? _live.find(weakGrant) : _live.end();
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
			if (grant == nullptr || !grant->WasIssuedBy(_issuerSeal) || grant->getRestoreGeneration() != restoreOwner->getCurrentGeneration() || grant->getRetireWindowIdentity() != retireWindowIdentity || grant->getOrderEpoch() != orderEpoch || live == _live.end() || !(live->second->Digest == grant->getGrantDigest()))
			{
				return false;
			}
			_liveGrants.erase(grant);
			return _live.erase(weakGrant) != 0;
		}
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal bool ValidateLive(VirtualizationRetireGrant? grant, System.Nullable<VirtualizationRestoreGenerationOwner> restoreOwner, ulong retireWindowIdentity, ulong orderEpoch)
	bool DomainHypercallRetireOwner::ValidateLive(std::shared_ptr<VirtualizationRetireGrant> grant, std::shared_ptr<VirtualizationRestoreGenerationOwner> restoreOwner, unsigned long long retireWindowIdentity, unsigned long long orderEpoch)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			const std::weak_ptr<VirtualizationRetireGrant> weakGrant(grant);
			auto live = grant ? _live.find(weakGrant) : _live.end();
// C# TO C++ CONVERTER TASK: The following .NET 'String.Equals' reference is not converted:
			return grant != nullptr && restoreOwner != nullptr && grant->WasIssuedBy(_issuerSeal) && grant->getRestoreGeneration() == restoreOwner->getCurrentGeneration() && grant->getRetireWindowIdentity() == retireWindowIdentity && grant->getOrderEpoch() == orderEpoch && live != _live.end() && (live->second->Digest == grant->getGrantDigest());
		}
	}

	int DomainHypercallRetireOwner::CountLiveGrants(unsigned long long domainTag)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			return static_cast<int>(std::count_if(_liveGrants.begin(), _liveGrants.end(),
				[domainTag](const auto &grant) { return grant != nullptr && grant->getDomainTag() == domainTag; }));
		}
	}

	int DomainHypercallRetireOwner::CancelLiveGrantsForDrain(unsigned long long domainTag)
	{
		{
			std::scoped_lock<std::mutex> lock(_sync);
			std::vector<std::shared_ptr<VirtualizationRetireGrant>> cancelled;
			for (const auto &grant : _liveGrants)
			{
				if (grant != nullptr && grant->getDomainTag() == domainTag)
				{
					cancelled.push_back(grant);
				}
			}
			for (auto grant : cancelled)
			{
				_liveGrants.erase(grant);
				_live.erase(std::weak_ptr<VirtualizationRetireGrant>(grant));
			}
			return static_cast<int>(cancelled.size());
		}
	}

	unsigned long long DomainHypercallRetireOwner::AllocateSequence()
	{
		unsigned long long value = _nextSequence++;
		return value == 0 ? _nextSequence++: value;
	}

	std::wstring DomainHypercallRetireOwner::ComputeDigest(unsigned long long sequence, std::shared_ptr<DomainHypercallCompletionOwner::CompletionPublicationToken> e5, std::shared_ptr<DomainHypercallRetireEligibility> eligibility)
	{
        if (eligibility->VirtualThreadId < 0 || eligibility->SourceSlotId < 0 || eligibility->WorkingSlotId < 0 || eligibility->PhysicalLaneId < 0) throw std::overflow_error("negative retire identity");
        std::vector<unsigned char> stream = Envelope;
        WriteU64(stream, sequence); WriteU64(stream, e5->getAttemptId());
        WriteU32(stream, static_cast<unsigned int>(eligibility->VirtualThreadId)); WriteU64(stream, eligibility->DomainTag);
        WriteU32(stream, static_cast<unsigned int>(eligibility->SourceSlotId)); WriteU32(stream, static_cast<unsigned int>(eligibility->WorkingSlotId));
        WriteU64(stream, eligibility->WorkingBundleSequence); WriteU64(stream, eligibility->OperationAttempt);
        WriteU32(stream, static_cast<unsigned int>(eligibility->PhysicalLaneId));
        WriteU64(stream, eligibility->RetireWindowIdentity); WriteU64(stream, eligibility->OrderEpoch); WriteU64(stream, e5->getRestoreGeneration());
        const auto hex = e5->getTokenDigest();
        if (hex.size() % 2 || hex.size() / 2 > (std::numeric_limits<unsigned int>::max)()) throw std::invalid_argument("Invalid hexadecimal digest");
        auto digit = [](wchar_t c) -> unsigned char {
            if (c >= L'0' && c <= L'9') return static_cast<unsigned char>(c - L'0');
            if (c >= L'a' && c <= L'f') return static_cast<unsigned char>(c - L'a' + 10);
            if (c >= L'A' && c <= L'F') return static_cast<unsigned char>(c - L'A' + 10);
            throw std::invalid_argument("Invalid hexadecimal digest");
        };
        WriteU32(stream, static_cast<unsigned int>(hex.size() / 2));
        for (size_t i = 0; i < hex.size(); i += 2) stream.push_back(static_cast<unsigned char>((digit(hex[i]) << 4) | digit(hex[i + 1])));
        return YAKSys_Hybrid_CPU::Core::Decoder::Sha256Hex(stream);
    }
    void DomainHypercallRetireOwner::WriteU32(std::vector<unsigned char>& stream, unsigned int value)
    { for (int shift = 0; shift <= 24; shift += 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }
    void DomainHypercallRetireOwner::WriteU64(std::vector<unsigned char>& stream, unsigned long long value)
    { for (int shift = 0; shift <= 56; shift += 8) stream.push_back(static_cast<unsigned char>(value >> shift)); }


	std::shared_ptr<DomainHypercallRetireResult> DomainHypercallRetireOwner::Deny(DomainHypercallRetireDecision decision, const std::wstring &reason)
	{
		return std::make_shared<DomainHypercallRetireResult>(decision, nullptr, reason);
	}
}
