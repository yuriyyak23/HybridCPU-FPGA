#include "Rf06ScalarSchedulerRouting.h"
#include "../../Decoder/Rf06ScalarLegacyProjection.h"
#include "PostStageBIssuedAttempt.h"
#include "../Metadata/BundleMetadata.h"
#include <algorithm>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static Rf06ScalarRoutingResult Route(MicroOpScheduler scheduler, CanonicalDecodedInstruction canonical, SourceOperationProvenance provenance, int ownerContextId, IReadOnlyList<MicroOp?> workingBundle, int ownerVirtualThreadId, int localCoreId, byte eligibleVirtualThreadMask, ulong workingBundleSequence, OperationAttemptIssuer attemptIssuer)
	std::shared_ptr<Rf06ScalarRoutingResult> Rf06ScalarSchedulerRouting::Route(std::shared_ptr<MicroOpScheduler> scheduler, std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<SourceOperationProvenance> provenance, int ownerContextId, const std::vector<std::shared_ptr<MicroOp>> &workingBundle, int ownerVirtualThreadId, int localCoreId, unsigned char eligibleVirtualThreadMask, unsigned long long workingBundleSequence, std::shared_ptr<OperationAttemptIssuer> attemptIssuer)
	{
		ArgumentNullException::ThrowIfNull(scheduler);
		ArgumentNullException::ThrowIfNull(canonical);
		ArgumentNullException::ThrowIfNull(provenance);
		ArgumentNullException::ThrowIfNull(workingBundle);
		ArgumentNullException::ThrowIfNull(attemptIssuer);

		if (canonical->SlotIndex != provenance->SourceSlotId)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Source provenance slot must match canonical slot index.", nameof(provenance));
			throw std::invalid_argument("Source provenance slot must match canonical slot index.");
		}

		if (static_cast<unsigned int>(provenance->SourceVirtualThreadId) >= SmtWayCount)
		{
			throw ArgumentOutOfRangeException(L"provenance");
		}

		std::shared_ptr<ExecutionContract> contract;
		std::shared_ptr<CheckedScalarLegacyProjection> projection;
		try
		{
			contract = Rf06ScalarLegacyProjection::CreateContract(canonical);
			projection = Rf06ScalarLegacyProjection::Project(canonical, contract);
		}
		catch (const InvalidOperationException &e1)
		{
			return Rf06ScalarRoutingResult::Rejected(Rf06ScalarRoutingRejectReason::NotScalarFamily, std::nullopt, nullptr);
		}

		std::shared_ptr<AdmissionRecord> admission = AdmissionRecord::Create(provenance, contract, provenance->SourceVirtualThreadId, ownerContextId, contract->getPlacement()->DomainTag);

		return Route(scheduler, admission, projection->getCarrier(), workingBundle, ownerVirtualThreadId, localCoreId, eligibleVirtualThreadMask, workingBundleSequence, provenance->SourceSlotId, attemptIssuer);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static Rf06ScalarRoutingResult Route(MicroOpScheduler scheduler, AdmissionRecord admission, MicroOp carrier, IReadOnlyList<MicroOp?> workingBundle, int ownerVirtualThreadId, int localCoreId, byte eligibleVirtualThreadMask, ulong workingBundleSequence, int workingSlotIndex, OperationAttemptIssuer attemptIssuer)
	std::shared_ptr<Rf06ScalarRoutingResult> Rf06ScalarSchedulerRouting::Route(std::shared_ptr<MicroOpScheduler> scheduler, std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier, const std::vector<std::shared_ptr<MicroOp>> &workingBundle, int ownerVirtualThreadId, int localCoreId, unsigned char eligibleVirtualThreadMask, unsigned long long workingBundleSequence, int workingSlotIndex, std::shared_ptr<OperationAttemptIssuer> attemptIssuer)
	{
		ArgumentNullException::ThrowIfNull(scheduler);
		ArgumentNullException::ThrowIfNull(admission);
		ArgumentNullException::ThrowIfNull(carrier);
		ArgumentNullException::ThrowIfNull(workingBundle);
		ArgumentNullException::ThrowIfNull(attemptIssuer);

		if (workingBundle.size() != BundleMetadata::BundleSlotCount)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("The existing scheduler path requires an eight-lane working bundle.", nameof(workingBundle));
			throw std::invalid_argument("The existing scheduler path requires an eight-lane working bundle.");
		}

		if (static_cast<unsigned int>(ownerVirtualThreadId) >= SmtWayCount)
		{
			throw ArgumentOutOfRangeException(L"ownerVirtualThreadId");
		}

		if (static_cast<unsigned int>(workingSlotIndex) >= BundleMetadata::BundleSlotCount)
		{
			throw ArgumentOutOfRangeException(L"workingSlotIndex");
		}

		if (!scheduler->getTypedSlotEnabled())
		{
			return Rf06ScalarRoutingResult::Rejected(Rf06ScalarRoutingRejectReason::TypedSchedulerPathDisabled, admission, carrier);
		}

		if (static_cast<unsigned int>(admission->VirtualThreadId) >= SmtWayCount || admission->VirtualThreadId == ownerVirtualThreadId || (eligibleVirtualThreadMask & (1 << admission->VirtualThreadId)) == 0)
		{
			return Rf06ScalarRoutingResult::Rejected(Rf06ScalarRoutingRejectReason::StageAEligibility, admission, carrier);
		}

		// An issued carrier belongs permanently to that attempt. Replay uses a
		// fresh semantic carrier; do not mutate/re-nominate the old one.
		if (carrier->getPostStageBIssuedAttempt() != nullptr || carrier->getPostStageBIdentityTemplate() != nullptr)
		{
			return Rf06ScalarRoutingResult::Rejected(Rf06ScalarRoutingRejectReason::IssuedAttemptReuse, admission, carrier);
		}

		// This adapter returns a final placement synchronously. A live SCHED1/
		// SCHED2 owner must retain its own deferred nomination, not receive an
		// orphan template after this call has already reported denial.
		if (scheduler->getPipelinedFspEnabled())
		{
			return Rf06ScalarRoutingResult::Rejected(Rf06ScalarRoutingRejectReason::DeferredScheduler, admission, carrier);
		}

		PrepareCarrier(admission, carrier);
		if (!HasEquivalentCarrierFacts(admission->Contract, carrier))
		{
			return Rf06ScalarRoutingResult::Rejected(Rf06ScalarRoutingRejectReason::CarrierContractMismatch, admission, carrier);
		}

		long long stageARejectsBefore = GetStageARejectCount(scheduler);
		long long stageBRejectsBefore = scheduler->getPinnedLaneConflicts() + scheduler->getInvalidPinnedLaneRejects() + scheduler->getLateBindingConflicts();

		// This is the existing scheduler path. Nomination is only a transport step;
		// PackBundleIntraCoreSmt owns owner/domain, legality, capacity, pressure and lane choice.
		carrier->setPostStageBIdentityTemplate(std::make_shared<PostStageBIdentityTemplate>(admission, workingBundleSequence, SlotId::Create(workingSlotIndex), attemptIssuer));
		scheduler->NominateSmtCandidate(admission->VirtualThreadId, carrier);
		std::vector<std::shared_ptr<MicroOp>> packed;
		struct CandidateWithdrawal final
		{
			std::shared_ptr<MicroOpScheduler> Scheduler;
			std::shared_ptr<MicroOp> Carrier;
			int VirtualThreadId;
			~CandidateWithdrawal()
			{
				Scheduler->WithdrawSmtCandidate(VirtualThreadId, Carrier);
				if (Carrier->getPostStageBIssuedAttempt() == nullptr)
					Carrier->setPostStageBIdentityTemplate(nullptr);
			}
		};
		{
			CandidateWithdrawal withdrawal{scheduler, carrier, admission->VirtualThreadId};
			packed = scheduler->PackBundleIntraCoreSmt(workingBundle, ownerVirtualThreadId, localCoreId, eligibleVirtualThreadMask);
		}

		auto selected = std::find(packed.begin(), packed.end(), carrier);
		int selectedLane = selected == packed.end() ? -1 : static_cast<int>(std::distance(packed.begin(), selected));
		if (selectedLane < 0)
		{
			Rf06ScalarRoutingRejectReason reason = scheduler->getPinnedLaneConflicts() + scheduler->getInvalidPinnedLaneRejects() + scheduler->getLateBindingConflicts() > stageBRejectsBefore ? Rf06ScalarRoutingRejectReason::StageBNoLane : GetStageARejectCount(scheduler) > stageARejectsBefore ? Rf06ScalarRoutingRejectReason::StageAReject : Rf06ScalarRoutingRejectReason::StageANotSelected;

			return Rf06ScalarRoutingResult::Rejected(reason, admission, carrier);
		}

		// The successful Stage-B commit is the sole identity mint, shared with
		// live SMT/FSP. Never create a second scheduled/issued attempt here.
	std::shared_ptr<PostStageBIssuedAttempt> issued = carrier->getPostStageBIssuedAttempt();
	if (issued == nullptr)
		throw InvalidOperationException(L"Successful typed Stage B must retain its original issued attempt.");
		std::shared_ptr<ScheduledOperation> scheduled = issued->getScheduledOperation();
		if (scheduled->getAdmission() != admission || scheduled->getPhysicalLane() != selectedLane)
		{
			throw InvalidOperationException(L"Stage-B identity does not match the routed admission and lane.");
		}
		return Rf06ScalarRoutingResult::Scheduled(admission, carrier, scheduled);
	}

	void Rf06ScalarSchedulerRouting::PrepareCarrier(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier)
	{
		carrier->setVirtualThreadId(admission->VirtualThreadId);
		carrier->setOwnerThreadId(admission->VirtualThreadId);
		carrier->setOwnerContextId(admission->OwnerContextId);
		carrier->RefreshAdmissionMetadata();
	}

	bool Rf06ScalarSchedulerRouting::HasEquivalentCarrierFacts(std::shared_ptr<ExecutionContract> contract, std::shared_ptr<MicroOp> carrier)
	{
		return HasEquivalentCarrierFacts(contract, carrier, contract->getPlacement()->DomainTag);
	}

	bool Rf06ScalarSchedulerRouting::HasEquivalentCarrierFacts(std::shared_ptr<ExecutionContract> contract, std::shared_ptr<MicroOp> carrier, unsigned long long domainTag)
	{
		return contract->getGeneratedBinding()->Opcode == carrier->getOpCode() && carrier->getInstructionClass() == contract->getInstructionClass() && carrier->getSerializationClass() == contract->getSerializationClass() && HasEquivalentPlacement(carrier->getPlacement(), contract->getPlacement(), domainTag) && contract->getStaticMemoryPlan() == nullptr && carrier->isMemoryOp() == (contract->getMemory()->Kind != MemoryCapabilityKind::NoMemory) && carrier->getWritesRegister() == (contract->getStaticEffectContract() == L"RegisterWrite") && carrier->isStealable() == contract->isStealable() && carrier->isRetireVisible() == contract->isRetireVisible() && carrier->getReadRegisters() == contract->getReadRegisters() && carrier->getWriteRegisters() == contract->getWriteRegisters() && carrier->getResourceMask() == contract->getResourceMask();
	}

	bool Rf06ScalarSchedulerRouting::HasEquivalentPlacement(SlotPlacementMetadata carrier, std::shared_ptr<ExecutionPlacement> contract, unsigned long long domainTag)
	{
		return carrier.RequiredSlotClass == contract->RequiredSlotClass && carrier.PinningKind == contract->PinningKind && carrier.PinnedLaneId == contract->PinnedLaneId && carrier.DomainTag == domainTag;
	}

	long long Rf06ScalarSchedulerRouting::GetStageARejectCount(std::shared_ptr<MicroOpScheduler> scheduler)
	{
		return scheduler->getStaticClassOvercommitRejects() + scheduler->getDynamicClassExhaustionRejects() + scheduler->getTypedSlotResourceConflictRejects() + scheduler->getTypedSlotScoreboardRejects() + scheduler->getTypedSlotBankPendingRejects() + scheduler->getTypedSlotHardwareBudgetRejects() + scheduler->getTypedSlotSpeculationBudgetRejects() + scheduler->getTypedSlotDomainRejects() + scheduler->getTypedSlotAssistQuotaRejects() + scheduler->getTypedSlotAssistBackpressureRejects();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private Rf06ScalarRoutingResult(Rf06ScalarRoutingRejectReason rejectReason, System.Nullable<AdmissionRecord> admission, MicroOp? carrier, System.Nullable<ScheduledOperation> scheduled)
	Rf06ScalarRoutingResult::Rf06ScalarRoutingResult(Rf06ScalarRoutingRejectReason rejectReason, std::optional<std::shared_ptr<AdmissionRecord>> admission, std::shared_ptr<MicroOp> carrier, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduled)
		: RejectReason(rejectReason), Admission(std::move(admission)), Carrier(std::move(carrier)), ScheduledOperation(std::move(scheduled))
	{
	}

	Rf06ScalarRoutingRejectReason Rf06ScalarRoutingResult::getRejectReason() const
	{
		return RejectReason;
	}

	std::optional<std::shared_ptr<AdmissionRecord>> Rf06ScalarRoutingResult::getAdmission() const
	{
		return Admission;
	}

	std::shared_ptr<MicroOp> Rf06ScalarRoutingResult::getCarrier() const
	{
		return Carrier;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> Rf06ScalarRoutingResult::getScheduledOperation() const
	{
		return ScheduledOperation;
	}

	bool Rf06ScalarRoutingResult::isScheduled() const
	{
		return getScheduledOperation() != nullptr;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static Rf06ScalarRoutingResult Rejected(Rf06ScalarRoutingRejectReason reason, System.Nullable<AdmissionRecord> admission, MicroOp? carrier)
	std::shared_ptr<Rf06ScalarRoutingResult> Rf06ScalarRoutingResult::Rejected(Rf06ScalarRoutingRejectReason reason, std::optional<std::shared_ptr<AdmissionRecord>> admission, std::shared_ptr<MicroOp> carrier)
	{
		return std::shared_ptr<Rf06ScalarRoutingResult>(new Rf06ScalarRoutingResult(reason, std::move(admission), std::move(carrier), nullptr));
	}

	std::shared_ptr<Rf06ScalarRoutingResult> Rf06ScalarRoutingResult::Scheduled(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduled)
	{
		return std::shared_ptr<Rf06ScalarRoutingResult>(new Rf06ScalarRoutingResult(Rf06ScalarRoutingRejectReason::None, std::move(admission), std::move(carrier), std::move(scheduled)));
	}
}
