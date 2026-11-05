#include "Rf08RetireEffectIdentityContracts.h"
#include "../Physical/PhysicalRegisterFile.h"
#include "../../../Pipeline/Metadata/BundleMetadata.h"
#include "../../../Decoder/CanonicalDecodedContracts.h"
#include <tuple>
#include <set>

using namespace YAKSys_Hybrid_CPU::Core::Decoder;
using namespace YAKSys_Hybrid_CPU::Core::Execution;

namespace YAKSys_Hybrid_CPU::Core::Registers::Retire
{

	RetireVisibleEffectIdentity::RetireVisibleEffectIdentity(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> executionRecord, RetireVisibleEffectKind effectKind, int effectOrdinal, std::optional<int> architecturalRegisterId)
		: ExecutionRecord(executionRecord), ScheduledOperation(executionRecord->getScheduledOperation()),
		  OperationId(executionRecord->getOperationId()), GeneratedBinding(executionRecord->getGeneratedBinding()),
		  SourceProvenance(ScheduledOperation->getAdmission()->SourceProvenance),
		  EffectKind(effectKind), EffectOrdinal(effectOrdinal), ArchitecturalRegisterId(architecturalRegisterId)
	{
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> RetireVisibleEffectIdentity::getExecutionRecord() const
	{
		return ExecutionRecord;
	}

	std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> RetireVisibleEffectIdentity::getScheduledOperation() const
	{
		return ScheduledOperation;
	}

	std::shared_ptr<VliwOperationId> RetireVisibleEffectIdentity::getOperationId() const
	{
		return OperationId;
	}

	std::shared_ptr<GeneratedStaticBinding> RetireVisibleEffectIdentity::getGeneratedBinding() const
	{
		return GeneratedBinding;
	}

	std::shared_ptr<SourceOperationProvenance> RetireVisibleEffectIdentity::getSourceProvenance() const
	{
		return SourceProvenance;
	}

	RetireVisibleEffectKind RetireVisibleEffectIdentity::getEffectKind() const
	{
		return EffectKind;
	}

	int RetireVisibleEffectIdentity::getEffectOrdinal() const
	{
		return EffectOrdinal;
	}

	std::optional<int> RetireVisibleEffectIdentity::getArchitecturalRegisterId() const
	{
		return ArchitecturalRegisterId;
	}

	int RetireVisibleEffectIdentity::getVirtualThreadId() const
	{
		return getOperationId()->VirtualThreadId;
	}

	unsigned long long RetireVisibleEffectIdentity::getWorkingBundleSequence() const
	{
		return getOperationId()->WorkingBundleSequence;
	}

	int RetireVisibleEffectIdentity::getWorkingSlotIndex() const
	{
		return getOperationId()->WorkingSlotIndex;
	}

	unsigned long long RetireVisibleEffectIdentity::getOperationAttempt() const
	{
		return getOperationId()->OperationAttempt;
	}

	int RetireVisibleEffectIdentity::getPhysicalLaneIndex() const
	{
		return getScheduledOperation()->getPhysicalLane();
	}

	unsigned long long RetireVisibleEffectIdentity::getSourceBundleSerial() const
	{
		return getSourceProvenance()->SourceBundleSerial;
	}

	int RetireVisibleEffectIdentity::getSourceSlotIndex() const
	{
		return getSourceProvenance()->SourceSlotIndex;
	}

	std::shared_ptr<RetireVisibleEffectIdentity> RetireVisibleEffectIdentity::Freeze(std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExecutionRecord> executionRecord, RetireVisibleEffectKind effectKind, int effectOrdinal, int effectVirtualThreadId, std::optional<int> architecturalRegisterId)
	{
		ArgumentNullException::ThrowIfNull(executionRecord);
		if (static_cast<unsigned char>(effectKind) < static_cast<unsigned char>(RetireVisibleEffectKind::RegisterWrite) || static_cast<unsigned char>(effectKind) > static_cast<unsigned char>(RetireVisibleEffectKind::AcceleratorCommit))
		{
			throw Violation(std::format("Unknown retire-visible effect kind value {}.", static_cast<unsigned int>(effectKind)));
		}

		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Decoder::ScheduledOperation> scheduledOperation = executionRecord->getScheduledOperation();
		if (scheduledOperation == nullptr)
		{
			throw Violation(L"Retire-visible effect identity requires an exact ScheduledOperation.");
		}

		if (executionRecord->getOperationId() != scheduledOperation->getOperationId())
		{
			throw Violation(L"ExecutionRecord operation identity does not match ScheduledOperation.");
		}

		if (executionRecord->getGeneratedBinding() != scheduledOperation->getAdmission()->Contract->getGeneratedBinding())
		{
			throw Violation(L"ExecutionRecord does not carry the exact frozen GeneratedStaticBinding from ScheduledOperation.");
		}

		std::shared_ptr<VliwOperationId> operationId = executionRecord->getOperationId();
		if (operationId->OperationAttempt == 0 || static_cast<unsigned int>(operationId->WorkingSlotIndex) >= BundleMetadata::BundleSlotCount || static_cast<unsigned int>(scheduledOperation->getPhysicalLane()) >= BundleMetadata::BundleSlotCount || static_cast<unsigned int>(scheduledOperation->getAdmission()->SourceProvenance->SourceSlotIndex) >= BundleMetadata::BundleSlotCount)
		{
			throw Violation(L"Retire-visible effect identity is missing a valid attempt, bundle slot, source slot, or physical lane.");
		}

		if (effectVirtualThreadId != operationId->VirtualThreadId)
		{
			throw Violation(std::format("Retire-visible effect VT{} does not match issued attempt VT{}.", effectVirtualThreadId, operationId->VirtualThreadId));
		}

		if (!scheduledOperation->getAdmission()->IsRetireVisible || !scheduledOperation->getAdmission()->Contract->isRetireVisible() || scheduledOperation->getAdmission()->Contract->isAssist())
		{
			throw Violation(L"A non-retire-visible or assist operation cannot create retire-visible effect identity.");
		}

		std::shared_ptr<ExecutionOutcome> completed = executionRecord->getOutcome();
		if (executionRecord->getState() != ExecutionRecordState::Terminal || completed == nullptr || completed->getKind() != ExecutionOutcomeKind::Completed || completed->getResult() == nullptr)
		{
			throw Violation(L"Retire-visible effect identity requires one terminal Completed execution outcome.");
		}

		auto completedResult = completed->getResult();
		if (effectOrdinal < 0 || effectOrdinal >= completedResult->getArchitecturalEffectCount())
		{
			throw Violation(std::format("Effect ordinal {} is outside completed effect count {}.", effectOrdinal, completedResult->getArchitecturalEffectCount()));
		}

		if (effectKind == RetireVisibleEffectKind::RegisterWrite)
		{
			if (!architecturalRegisterId)
			{
				throw Violation(L"RegisterWrite identity requires an architectural register.");
			}

			if (architecturalRegisterId.value() == 0)
			{
				throw Violation(L"Architectural x0 mutation is forbidden.");
			}

			if (static_cast<unsigned int>(architecturalRegisterId.value()) >= static_cast<unsigned int>(RenameMap::ArchRegs))
			{
				throw Violation(std::format("Architectural register {} is outside the live rename namespace.", architecturalRegisterId.value()));
			}
		}
		else if (architecturalRegisterId)
		{
			throw Violation(std::format("Effect kind {} cannot claim an architectural-register mutation.", static_cast<unsigned int>(effectKind)));
		}

		return std::shared_ptr<RetireVisibleEffectIdentity>(new RetireVisibleEffectIdentity(executionRecord, effectKind, effectOrdinal, architecturalRegisterId));
	}

	RetireEffectIdentityContractViolationException RetireVisibleEffectIdentity::Violation(const std::wstring &message)
	{
		return RetireEffectIdentityContractViolationException(message);
	}

	RetireEffectIdentityContractViolationException RetireVisibleEffectIdentity::Violation(const std::string &message)
	{
		return RetireEffectIdentityContractViolationException(message);
	}

	RetireRecordIdentityProjection::RetireRecordIdentityProjection(YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord retireRecord, std::shared_ptr<RetireVisibleEffectIdentity> identity)
		: RetireRecord(retireRecord), Identity(std::move(identity))
	{
	}

	YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord RetireRecordIdentityProjection::getRetireRecord() const
	{
		return RetireRecord;
	}

	std::shared_ptr<RetireVisibleEffectIdentity> RetireRecordIdentityProjection::getIdentity() const
	{
		return Identity;
	}

	std::shared_ptr<RetireRecordIdentityProjection> RetireRecordIdentityProjection::Create(YAKSys_Hybrid_CPU::Core::Registers::Retire::RetireRecord const retireRecord, std::shared_ptr<RetireVisibleEffectIdentity> identity)
	{
		ArgumentNullException::ThrowIfNull(identity);
		if (retireRecord.getVtId() != identity->getVirtualThreadId())
		{
			throw RetireEffectIdentityContractViolationException(std::format("RetireRecord VT{} does not match ScheduledOperation VT{}.", retireRecord.getVtId(), identity->getVirtualThreadId()));
		}

		RetireVisibleEffectKind expectedKind;
		switch (retireRecord.getKind())
		{
		case RetireRecordKind::RegisterWrite: expectedKind = RetireVisibleEffectKind::RegisterWrite; break;
		case RetireRecordKind::PcWrite: expectedKind = RetireVisibleEffectKind::PcWrite; break;
		default: throw RetireEffectIdentityContractViolationException("Unsupported RetireRecord kind.");
		}
		if (identity->getEffectKind() != expectedKind)
		{
			throw RetireEffectIdentityContractViolationException(std::format("RetireRecord kind {} does not match effect identity {}.", static_cast<unsigned int>(retireRecord.getKind()), static_cast<unsigned int>(identity->getEffectKind())));
		}

		if (retireRecord.getKind() == RetireRecordKind::RegisterWrite && (!identity->getArchitecturalRegisterId() || retireRecord.getArchReg() != identity->getArchitecturalRegisterId().value()))
		{
			throw RetireEffectIdentityContractViolationException("RetireRecord architectural register does not match effect identity register.");
		}

		return std::shared_ptr<RetireRecordIdentityProjection>(new RetireRecordIdentityProjection(retireRecord, std::move(identity)));
	}

	void RetireVisibleEffectCoherence::ValidatePublicationClaim(std::shared_ptr<RetireVisibleEffectIdentity> identity, std::shared_ptr<ScheduledOperation> scheduledOperation, std::shared_ptr<ExecutionRecord> executionRecord, std::shared_ptr<GeneratedStaticBinding> generatedBinding, bool prevalidationComplete, bool selectedByRetireProtocol, bool requestsStoreVisibility)
	{
		ArgumentNullException::ThrowIfNull(identity);
		ArgumentNullException::ThrowIfNull(scheduledOperation);
		ArgumentNullException::ThrowIfNull(executionRecord);
		ArgumentNullException::ThrowIfNull(generatedBinding);

		if (identity->getScheduledOperation() != scheduledOperation || identity->getExecutionRecord() != executionRecord || identity->getOperationId() != scheduledOperation->getOperationId() || executionRecord->getOperationId() != identity->getOperationId())
		{
			throw Violation(L"Publication claim does not carry the exact ScheduledOperation/ExecutionRecord/VliwOperationId attempt.");
		}

		if (identity->getGeneratedBinding() != generatedBinding || executionRecord->getGeneratedBinding() != generatedBinding)
		{
			throw Violation(L"Publication claim reconstructed or mismatched the frozen GeneratedStaticBinding.");
		}

		if (!prevalidationComplete)
		{
			throw Violation(L"Retire-visible publication cannot precede complete bounded-window prevalidation.");
		}

		bool isStore = ([&] { const auto codexPatternValue = identity->getEffectKind(); return codexPatternValue == RetireVisibleEffectKind::DeferredStoreCommit || codexPatternValue == RetireVisibleEffectKind::ScalarMemoryStoreCommit; }());
		if (requestsStoreVisibility && (!isStore || !selectedByRetireProtocol))
		{
			throw Violation(L"Store visibility is permitted only for a store effect selected by the retire protocol.");
		}

		if (!requestsStoreVisibility && isStore && selectedByRetireProtocol)
		{
			return;
		}

		if (!selectedByRetireProtocol)
		{
			throw Violation(L"Architectural-effect publication requires the selected retire protocol.");
		}
	}

	void RetireVisibleEffectCoherence::ValidateDistinctTerminalEffects(std::vector<std::shared_ptr<RetireVisibleEffectIdentity>> &identities)
	{
		std::set<std::tuple<int, unsigned long long, int, unsigned long long, int>> seen;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: foreach (RetireVisibleEffectIdentity? identity in identities)
		for (auto identity : identities)
		{
			if (identity == nullptr)
			{
				throw Violation(L"Retire-visible effect set contains missing identity.");
			}

			if (!seen.insert(std::make_tuple(identity->getOperationId()->VirtualThreadId, identity->getOperationId()->WorkingBundleSequence, identity->getOperationId()->WorkingSlotIndex, identity->getOperationId()->OperationAttempt, identity->getEffectOrdinal())).second)
			{
				throw Violation(std::format("Issued attempt at bundle {} slot {} contains duplicate terminal effect ordinal {}.", identity->getOperationId()->WorkingBundleSequence, identity->getOperationId()->WorkingSlotIndex, identity->getEffectOrdinal()));
			}
		}
	}

	RetireEffectIdentityContractViolationException RetireVisibleEffectCoherence::Violation(const std::wstring &message)
	{
		return RetireEffectIdentityContractViolationException(message);
	}

	RetireEffectIdentityContractViolationException RetireVisibleEffectCoherence::Violation(const std::string &message)
	{
		return RetireEffectIdentityContractViolationException(message);
	}

	RetireEffectIdentityContractViolationException::RetireEffectIdentityContractViolationException(const std::wstring &message) : InvalidOperationException(message)
	{
	}

	RetireEffectIdentityContractViolationException::RetireEffectIdentityContractViolationException(const std::string &message) : InvalidOperationException(message)
	{
	}
}
