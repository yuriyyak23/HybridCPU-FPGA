#include "PostStageBIssuedAttempt.h"
#include "../Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
#include "../Safety/SecureComputeIssuePermitOwner.h"
#include "../../Architecture/Registers/Retire/Rf08RetireEffectIdentityContracts.h"

using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Execution;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	WorkingBundleEntry::WorkingBundleEntry(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier, unsigned long long workingBundleSequence, SlotId workingSlotId)
		: Admission(std::move(admission)), Carrier(std::move(carrier)), WorkingBundleSequence(workingBundleSequence), WorkingSlotId(workingSlotId) {}

	std::shared_ptr<AdmissionRecord> WorkingBundleEntry::getAdmission() const
	{
		return Admission;
	}

	std::shared_ptr<MicroOp> WorkingBundleEntry::getCarrier() const
	{
		return Carrier;
	}

	unsigned long long WorkingBundleEntry::getWorkingBundleSequence() const
	{
		return WorkingBundleSequence;
	}

	SlotId WorkingBundleEntry::getWorkingSlotId() const
	{
		return WorkingSlotId;
	}

	std::shared_ptr<WorkingBundleEntry> WorkingBundleEntry::CreateSourcePreserving(std::shared_ptr<AdmissionRecord> admission, std::shared_ptr<MicroOp> carrier, unsigned long long workingBundleSequence, SlotId workingSlotId)
	{
		ArgumentNullException::ThrowIfNull(admission);
		ArgumentNullException::ThrowIfNull(carrier);

		std::shared_ptr<SourceOperationProvenance> source = admission->SourceProvenance;
		if (workingBundleSequence != source->SourceBundleSerial)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Source-preserving working bundle sequence must equal the source bundle serial.", nameof(workingBundleSequence));
			throw std::invalid_argument("Source-preserving working bundle sequence must equal the source bundle serial.");
		}

		if (workingSlotId != source->SourceSlotId)
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Source-preserving working slot must equal the source slot.", nameof(workingSlotId));
			throw std::invalid_argument("Source-preserving working slot must equal the source slot.");
		}

		return std::shared_ptr<WorkingBundleEntry>(new WorkingBundleEntry(admission, carrier, workingBundleSequence, workingSlotId));
	}

	std::shared_ptr<PostStageBIdentityTemplate> WorkingBundleEntry::CreatePostStageBIdentityTemplate(std::shared_ptr<OperationAttemptIssuer> attemptIssuer)
	{
		return std::make_shared<PostStageBIdentityTemplate>(getAdmission(), getWorkingBundleSequence(), getWorkingSlotId(), attemptIssuer);
	}

	PostStageBIdentityTemplate::PostStageBIdentityTemplate(std::shared_ptr<AdmissionRecord> admission, unsigned long long workingBundleSequence, SlotId workingSlotId, std::shared_ptr<OperationAttemptIssuer> attemptIssuer)
		: Admission(admission ? std::move(admission) : throw std::invalid_argument("admission")),
		  WorkingBundleSequence(workingBundleSequence), WorkingSlotId(workingSlotId),
		  AttemptIssuer(attemptIssuer ? std::move(attemptIssuer) : throw std::invalid_argument("attemptIssuer")) {}

	std::shared_ptr<AdmissionRecord> PostStageBIdentityTemplate::getAdmission() const
	{
		return Admission;
	}

	unsigned long long PostStageBIdentityTemplate::getWorkingBundleSequence() const
	{
		return WorkingBundleSequence;
	}

	SlotId PostStageBIdentityTemplate::getWorkingSlotId() const
	{
		return WorkingSlotId;
	}

	std::shared_ptr<OperationAttemptIssuer> PostStageBIdentityTemplate::getAttemptIssuer() const
	{
		return AttemptIssuer;
	}

	bool PostStageBIssuedAttempt::isCurrent() const
	{
		return !invalidated;
	}

	void PostStageBIssuedAttempt::InvalidateAfterPipelineDiscard()
	{
		invalidated = true;
	}

	PostStageBIssuedAttempt::PostStageBIssuedAttempt(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation)
		: ScheduledOperation(scheduledOperation),
		  ExecutionRecord(YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord::Create(scheduledOperation)),
		  GeneratedBinding(scheduledOperation->getAdmission()->Contract->getGeneratedBinding()) {}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> PostStageBIssuedAttempt::getScheduledOperation() const
	{
		return ScheduledOperation;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> PostStageBIssuedAttempt::getExecutionRecord() const
	{
		return ExecutionRecord;
	}

	std::shared_ptr<GeneratedStaticBinding> PostStageBIssuedAttempt::getGeneratedBinding() const
	{
		return GeneratedBinding;
	}

	std::shared_ptr<ScalarRegisterWriteRetireEffect> PostStageBIssuedAttempt::getScalarRegisterWriteEffect() const
	{
		return _scalarRegisterWriteEffect;
	}

	std::shared_ptr<PostStageBIssuedAttempt> PostStageBIssuedAttempt::RequireOriginalSecureStageBAttempt(std::shared_ptr<SafetyVerifier> issuer, std::shared_ptr<SecureComputePreparedMachineAdmission> prepared)
	{
		if (!issuer->HasConsumedSecureMachinePermit(prepared))
		{
			throw InvalidOperationException(L"Current consumed standalone machine permit required.");
		}
		auto placed = prepared->getCertificate()->getPlaced();
		auto issued = placed->getIssuedAttempt();
		if (!issued)
			throw InvalidOperationException(L"Original Stage-B carrier required.");
		if (!issued->isCurrent() || issued->getScheduledOperation() != placed->getScheduledOperation() || issued->getExecutionRecord()->getState() != ExecutionRecordState::Issued)
		{
			throw InvalidOperationException(L"Original Stage-B attempt is no longer issued.");
		}
		return issued;
	}

	std::shared_ptr<PostStageBIssuedAttempt> PostStageBIssuedAttempt::CreateAfterSuccessfulStageB(std::shared_ptr<PostStageBIdentityTemplate> template_Keyword, std::shared_ptr<LaneId> physicalLane)
	{
		ArgumentNullException::ThrowIfNull(template_Keyword);
		ArgumentNullException::ThrowIfNull(physicalLane);
		auto scheduledOperation = ScheduledOperation::CreateAfterStageB(template_Keyword->getAdmission(), template_Keyword->getWorkingBundleSequence(), template_Keyword->getWorkingSlotId().getValue(), physicalLane->getValue(), template_Keyword->getAttemptIssuer());
		return std::shared_ptr<PostStageBIssuedAttempt>(new PostStageBIssuedAttempt(scheduledOperation));
	}

	void PostStageBIssuedAttempt::CompleteScalarRegisterWrite(RetireRecord const retireRecord)
	{
		if (invalidated || retireRecord.getKind() != RetireRecordKind::RegisterWrite || retireRecord.getVtId() != getScheduledOperation()->getOperationId()->VirtualThreadId)
		{
			throw RetireEffectIdentityContractViolationException(L"Issued scalar attempt does not match the live RegisterWrite retire record.");
		}

		if (getExecutionRecord()->getState() == ExecutionRecordState::Issued)
		{
			int effectCount = retireRecord.getArchReg() == 0 ? 0 : 1;
			std::shared_ptr<ExecutionResultContract> result = ExecutionResultContract::Scalar(retireRecord.getValue(), effectCount);
			getExecutionRecord()->ApplyTerminalTransition(getExecutionRecord()->CreateTerminalTransition(ExecutionOutcome::Completed(result)));
		}

		if (retireRecord.getArchReg() == 0)
		{
			return;
		}

		if (_scalarRegisterWriteEffect != nullptr)
		{
			throw RetireEffectIdentityContractViolationException(L"Issued scalar attempt produced more than one RegisterWrite retire effect.");
		}

		std::shared_ptr<RetireVisibleEffectIdentity> identity = RetireVisibleEffectIdentity::Freeze(getExecutionRecord(), RetireVisibleEffectKind::RegisterWrite, 0, retireRecord.getVtId(), retireRecord.getArchReg());
		_scalarRegisterWriteEffect = ScalarRegisterWriteRetireEffect::Freeze(retireRecord, identity);
	}
}
