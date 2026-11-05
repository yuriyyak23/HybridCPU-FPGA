#pragma once

#include "../../Decoder/Rf06ExecutionContracts.h"
#include "../Assist/InterCore/YAKSys_Hybrid_CPU.Core.MicroOpScheduler.h"
#include "../MicroOps/Types/MicroOp.h"
#include "SlotPlacementMetadata.h"
#include <vector>
#include <stdexcept>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class Rf06ScalarRoutingResult; }

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// RF-06.3 routing seam for the first scalar family.
	///
	/// The adapter resolves and checks the immutable contract at the decode/materialization
	/// boundary, then submits the already materialized carrier to the existing SMT scheduler.
	/// It does not implement admission or lane choice itself: <see cref="MicroOpScheduler"/>
	/// remains the only scheduler and its PackBundleIntraCoreSmt path remains authoritative.
	/// </summary>
	class Rf06ScalarSchedulerRouting final
	{
	private:
		static constexpr int SmtWayCount = 4;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static Rf06ScalarRoutingResult Route(MicroOpScheduler scheduler, CanonicalDecodedInstruction canonical, SourceOperationProvenance provenance, int ownerContextId, IReadOnlyList<MicroOp?> workingBundle, int ownerVirtualThreadId, int localCoreId, byte eligibleVirtualThreadMask, ulong workingBundleSequence, OperationAttemptIssuer attemptIssuer)
		static std::shared_ptr<Rf06ScalarRoutingResult> Route(std::shared_ptr<MicroOpScheduler> scheduler, std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<SourceOperationProvenance> provenance, int ownerContextId, const std::vector<std::shared_ptr<MicroOp>> &workingBundle, int ownerVirtualThreadId, int localCoreId, unsigned char eligibleVirtualThreadMask, unsigned long long workingBundleSequence, std::shared_ptr<OperationAttemptIssuer> attemptIssuer);

		/// <summary>
		/// Routes a substitute carrier under an already frozen scalar contract.
		/// The scheduler is deliberately unaware of the carrier's concrete type and
		/// applies the same Stage A/Stage B path to any carrier with equal frozen facts.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static Rf06ScalarRoutingResult Route(MicroOpScheduler scheduler, AdmissionRecord admission, MicroOp carrier, IReadOnlyList<MicroOp?> workingBundle, int ownerVirtualThreadId, int localCoreId, byte eligibleVirtualThreadMask, ulong workingBundleSequence, int workingSlotIndex, OperationAttemptIssuer attemptIssuer)
		static std::shared_ptr<Rf06ScalarRoutingResult> Route(std::shared_ptr<MicroOpScheduler> scheduler, std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier, const std::vector<std::shared_ptr<MicroOp>> &workingBundle, int ownerVirtualThreadId, int localCoreId, unsigned char eligibleVirtualThreadMask, unsigned long long workingBundleSequence, int workingSlotIndex, std::shared_ptr<OperationAttemptIssuer> attemptIssuer);

	private:
		static void PrepareCarrier(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier);

	public:
		static bool HasEquivalentCarrierFacts(std::shared_ptr<ExecutionContract> contract, std::shared_ptr<MicroOp> carrier);

		// Runtime domain is carried by the original admission/sideband owner; the
		// canonical static family contract does not invent a runtime domain value.
		static bool HasEquivalentCarrierFacts(std::shared_ptr<ExecutionContract> contract, std::shared_ptr<MicroOp> carrier, unsigned long long domainTag);

	private:
		static bool HasEquivalentPlacement(SlotPlacementMetadata carrier, std::shared_ptr<ExecutionPlacement> contract, unsigned long long domainTag);

		static long long GetStageARejectCount(std::shared_ptr<MicroOpScheduler> scheduler);

	};

	enum class Rf06ScalarRoutingRejectReason : unsigned char
	{
		IssuedAttemptReuse = 8,
		DeferredScheduler = 9,
		None = 0,
		NotScalarFamily = 1,
		TypedSchedulerPathDisabled = 2,
		StageAEligibility = 3,
		CarrierContractMismatch = 4,
		StageAReject = 5,
		StageANotSelected = 6,
		StageBNoLane = 7
	};

	class Rf06ScalarRoutingResult final : public std::enable_shared_from_this<Rf06ScalarRoutingResult>
	{
	private:
		const Rf06ScalarRoutingRejectReason RejectReason;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<AdmissionRecord> Admission;
		std::optional<std::shared_ptr<AdmissionRecord>> Admission;
		const std::shared_ptr<MicroOp> Carrier;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> ScheduledOperation;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private Rf06ScalarRoutingResult(Rf06ScalarRoutingRejectReason rejectReason, System.Nullable<AdmissionRecord> admission, MicroOp? carrier, System.Nullable<ScheduledOperation> scheduled)
		Rf06ScalarRoutingResult(Rf06ScalarRoutingRejectReason rejectReason, std::optional<std::shared_ptr<AdmissionRecord>> admission, std::shared_ptr<MicroOp> carrier, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduled);

	public:
		Rf06ScalarRoutingRejectReason getRejectReason() const;
		std::optional<std::shared_ptr<AdmissionRecord>> getAdmission() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal MicroOp? Carrier {get;}
		std::shared_ptr<MicroOp> getCarrier() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal ScheduledOperation? ScheduledOperation {get;}
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> getScheduledOperation() const;
		bool isScheduled() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static Rf06ScalarRoutingResult Rejected(Rf06ScalarRoutingRejectReason reason, System.Nullable<AdmissionRecord> admission, MicroOp? carrier)
		static std::shared_ptr<Rf06ScalarRoutingResult> Rejected(Rf06ScalarRoutingRejectReason reason, std::optional<std::shared_ptr<AdmissionRecord>> admission, std::shared_ptr<MicroOp> carrier);

		static std::shared_ptr<Rf06ScalarRoutingResult> Scheduled(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier, std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduled);
	};

}
