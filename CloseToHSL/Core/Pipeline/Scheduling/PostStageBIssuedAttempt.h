#pragma once

namespace YAKSys_Hybrid_CPU::Core { class MicroOp; }
#include "../../Architecture/BinaryFormat/SlotEncoding/SlotId.h"
#include "../../Decoder/Rf06ExecutionContracts.h"
#include "../../Execution/Rf07ExecutionOutcomeContracts.h"
#include "../../Architecture/Registers/Retire/Rf081ScalarRegisterWriteRetireEffect.h"
namespace YAKSys_Hybrid_CPU::Core { class SafetyVerifier; class SecureComputePreparedMachineAdmission; class LaneId; }

#include "../../Architecture/Registers/Retire/RetireRecord.h"
#include <stdexcept>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core::Decoder { class ScheduledOperation; }
namespace YAKSys_Hybrid_CPU::Core::Execution { class ExecutionRecord; }
namespace YAKSys_Hybrid_CPU::Core::Registers::Retire { class ScalarRegisterWriteRetireEffect; }
// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core { class PostStageBIdentityTemplate; }

using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Execution;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Frozen identity facts assigned by the working-bundle formation owner before
	/// Stage A / Stage B. The currently connected RF-08.3d contour is explicitly
	/// source-preserving; later layouts require a separate authorised producer.
	/// </summary>
	class WorkingBundleEntry final : public std::enable_shared_from_this<WorkingBundleEntry>
	{
	private:
		const std::shared_ptr<AdmissionRecord> Admission;
		const std::shared_ptr<MicroOp> Carrier;
		const unsigned long long WorkingBundleSequence;
		const SlotId WorkingSlotId;

		WorkingBundleEntry(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier, unsigned long long workingBundleSequence, SlotId workingSlotId);

	public:
		std::shared_ptr<AdmissionRecord> getAdmission() const;
		std::shared_ptr<MicroOp> getCarrier() const;
		unsigned long long getWorkingBundleSequence() const;
		SlotId getWorkingSlotId() const;

		static std::shared_ptr<WorkingBundleEntry> CreateSourcePreserving(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier, unsigned long long workingBundleSequence, SlotId workingSlotId);

		std::shared_ptr<PostStageBIdentityTemplate> CreatePostStageBIdentityTemplate(std::shared_ptr<OperationAttemptIssuer> attemptIssuer);
	};

	/// <summary>
	/// Immutable RF-08.3d companion prepared before nomination and consumed only by
	/// the existing successful Stage-B commit arm. It has no scheduler authority.
	/// </summary>
	class PostStageBIdentityTemplate final : public std::enable_shared_from_this<PostStageBIdentityTemplate>
	{
	private:
		const std::shared_ptr<AdmissionRecord> Admission;
		const unsigned long long WorkingBundleSequence;
		const SlotId WorkingSlotId;
		const std::shared_ptr<OperationAttemptIssuer> AttemptIssuer;

	public:
		PostStageBIdentityTemplate(std::shared_ptr<AdmissionRecord> admission, unsigned long long workingBundleSequence, SlotId workingSlotId, std::shared_ptr<OperationAttemptIssuer> attemptIssuer);

		std::shared_ptr<AdmissionRecord> getAdmission() const;
		unsigned long long getWorkingBundleSequence() const;
		SlotId getWorkingSlotId() const;
		std::shared_ptr<OperationAttemptIssuer> getAttemptIssuer() const;
	};

	/// <summary>
	/// Exact result of one successful existing Stage-B lane materialization. This
	/// carrier owns neither execution decisions nor retirement/publication.
	/// </summary>
	class PostStageBIssuedAttempt final : public std::enable_shared_from_this<PostStageBIssuedAttempt>
	{
	private:
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> ScheduledOperation;
		const std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> ExecutionRecord;
		const std::shared_ptr<GeneratedStaticBinding> GeneratedBinding;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private ScalarRegisterWriteRetireEffect? _scalarRegisterWriteEffect;
		std::shared_ptr<ScalarRegisterWriteRetireEffect> _scalarRegisterWriteEffect;
		bool invalidated = false;
	public:
		bool isCurrent() const;
		void InvalidateAfterPipelineDiscard();

	private:
		PostStageBIssuedAttempt(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation);

	public:
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> getScheduledOperation() const;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> getExecutionRecord() const;
		std::shared_ptr<GeneratedStaticBinding> getGeneratedBinding() const;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public ScalarRegisterWriteRetireEffect? ScalarRegisterWriteEffect
		std::shared_ptr<ScalarRegisterWriteRetireEffect> getScalarRegisterWriteEffect() const;

		// Standalone secure composition already owns an actual successful scalar
		// Stage-B placement and a consumed registered permit. This freezes execution
		// identity only; the completion owner still owns acceptance and canonical retire.
		static std::shared_ptr<PostStageBIssuedAttempt> RequireOriginalSecureStageBAttempt(std::shared_ptr<SafetyVerifier> issuer, std::shared_ptr<SecureComputePreparedMachineAdmission> prepared);

		static std::shared_ptr<PostStageBIssuedAttempt> CreateAfterSuccessfulStageB(std::shared_ptr<PostStageBIdentityTemplate> template_Keyword, std::shared_ptr<LaneId> physicalLane);

		void CompleteScalarRegisterWrite(RetireRecord const retireRecord);
	};

}
