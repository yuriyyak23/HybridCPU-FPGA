#include "DmaStreamComputeStatusMicroOp.h"
#include "../../../Architecture/Common/StreamEngineId.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeTokenStore.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../Types/MicroOp.Exceptions.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	DmaStreamComputeStatusMicroOp::DmaStreamComputeStatusMicroOp(unsigned short destinationRegister, unsigned short tokenRegister)
		: DestinationRegister(NormalizeDestinationRegister(destinationRegister)), TokenRegister(NormalizeTokenRegister(tokenRegister))
	{
		setOpCode(Processor::CPU_Core::IsaOpcodeValues::DSC_STATUS);
		setDestRegID(getDestinationRegister());

		setMemoryOp(false);
		setHasSideEffects(true);
		setClass(MicroOpClass::Dma);
		setInstructionClass(Arch::InstructionClass::Memory);
		setSerializationClass(Arch::SerializationClass::MemoryOrdered);
		setWritesRegister(getDestinationRegister() != 0);

		SetHardPinnedPlacement(SlotClass::DmaStreamClass, 6);
		RefreshOwnerDependentMetadata();
	}

	unsigned short DmaStreamComputeStatusMicroOp::NormalizeDestinationRegister(unsigned short destinationRegister)
	{
		int normalizedRegister;
		if (!TryNormalizeFlatArchRegId(destinationRegister, normalizedRegister))
		{
			throw DecodeProjectionFaultException(L"DSC_STATUS requires a flat architectural rd register id.");
		}
		return static_cast<unsigned short>(normalizedRegister);
	}

	unsigned short DmaStreamComputeStatusMicroOp::NormalizeTokenRegister(unsigned short tokenRegister)
	{
		int normalizedRegister;
		if (!TryNormalizeFlatArchRegId(tokenRegister, normalizedRegister))
		{
			throw DecodeProjectionFaultException(L"DSC_STATUS requires a flat architectural rs1 token-id register id.");
		}
		return static_cast<unsigned short>(normalizedRegister);
	}

	unsigned short DmaStreamComputeStatusMicroOp::getDestinationRegister() const
	{
		return DestinationRegister;
	}

	unsigned short DmaStreamComputeStatusMicroOp::getTokenRegister() const
	{
		return TokenRegister;
	}

	std::optional<std::shared_ptr<DmaStreamComputeStatusQueryResult>> DmaStreamComputeStatusMicroOp::getLastStatusResult() const
	{
		return _lastStatusResult;
	}

	std::optional<std::shared_ptr<DmaStreamComputeOwnerBinding>> DmaStreamComputeStatusMicroOp::getLastOwnerBinding() const
	{
		return _lastOwnerBinding;
	}

	unsigned long long DmaStreamComputeStatusMicroOp::getLastRequestedTokenId() const
	{
		return _lastRequestedTokenId;
	}

	std::shared_ptr<DmaStreamComputeStatusReplayEvidence> DmaStreamComputeStatusMicroOp::getLastStatusReplayEvidence() const
	{
		return LastStatusReplayEvidence;
	}

	void DmaStreamComputeStatusMicroOp::setLastStatusReplayEvidence(std::shared_ptr<DmaStreamComputeStatusReplayEvidence> value)
	{
		LastStatusReplayEvidence = value;
	}

	bool DmaStreamComputeStatusMicroOp::getUsedStreamEngineFallback() const
	{
		return false;
	}

	bool DmaStreamComputeStatusMicroOp::getUsedDmaControllerFallback() const
	{
		return false;
	}

	CanonicalDecodePublicationMode DmaStreamComputeStatusMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	bool DmaStreamComputeStatusMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		RefreshOwnerDependentMetadata();

		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		_lastRequestedTokenId = ReadUnifiedScalarSourceOperand(core, vtId, getTokenRegister());
		auto ownerBinding = BuildOwnerBinding(core, vtId);
		_lastOwnerBinding = ownerBinding;
		auto statusResult = core->GetDmaStreamComputeTokenStore()->QueryStatusByTokenId(_lastRequestedTokenId, ownerBinding);
		_lastStatusResult = statusResult;
		_capturedStatusWord = statusResult->Snapshot ? statusResult->Snapshot->EncodedStatusWord : 0ULL;
		setLastStatusReplayEvidence(DmaStreamComputeStatusReplayEvidence::Capture(_lastRequestedTokenId, ownerBinding,
			statusResult, DmaStreamComputeLanePlacementEvidence::MaterializedLane6(6,
				SlotClassLaneMap::GetLaneMask(SlotClass::DmaStreamClass), 0, false)));

		return true;
	}

	void DmaStreamComputeStatusMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (!_lastStatusResult || !*_lastStatusResult)
		{
			throw InvalidOperationException(L"DSC_STATUS retire requires a prior execute/capture result.");
		}
		auto result = *_lastStatusResult;

		if (!result->IsAccepted)
		{
			ThrowRetireException(result);
		}

		if (!getWritesRegister())
		{
			return;
		}

		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestinationRegister(), _capturedStatusWord));
	}

	bool DmaStreamComputeStatusMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _capturedStatusWord;
		return _lastStatusResult && *_lastStatusResult && (*_lastStatusResult)->IsAccepted && getWritesRegister();
	}

	void DmaStreamComputeStatusMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_capturedStatusWord = value;
	}

	void DmaStreamComputeStatusMicroOp::RefreshWriteMetadata()
	{
		RefreshOwnerDependentMetadata();
	}

	void DmaStreamComputeStatusMicroOp::RefreshOwnerDependentMetadata()
	{
		setReadRegisters(getTokenRegister() == 0 ? std::vector<int>{} : std::vector<int>{static_cast<int>(getTokenRegister())});
		setWriteRegisters(getWritesRegister() ? std::vector<int>{static_cast<int>(getDestinationRegister())} : std::vector<int>{});
		setReadMemoryRanges(std::vector<MemoryRange>{});
		setWriteMemoryRanges(std::vector<MemoryRange>{});
		setResourceMask(BuildResourceMask(getPlacement().DomainTag));
		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata();
	}

	std::wstring DmaStreamComputeStatusMicroOp::GetDescription()
	{
		return std::format(L"DSC_STATUS: rd=x{}, token=x{}", getDestinationRegister(), getTokenRegister());
	}

	std::shared_ptr<DmaStreamComputeOwnerBinding> DmaStreamComputeStatusMicroOp::BuildOwnerBinding(std::shared_ptr<Processor::CPU_Core> &core, int vtId)
	{
		if (getOwnerContextId() < 0)
		{
			throw InvalidOperationException(L"DSC_STATUS owner context id is not materialized.");
		}

		std::shared_ptr<DmaStreamComputeOwnerBinding> temp_var = std::make_shared<DmaStreamComputeOwnerBinding>();
		temp_var->OwnerVirtualThreadId = static_cast<unsigned short>(vtId);
		temp_var->OwnerContextId = static_cast<unsigned int>(getOwnerContextId());
		temp_var->OwnerCoreId = core->getCoreID();
		temp_var->OwnerPodId = Lane6QueueControlOwnerPodId;
		temp_var->OwnerDomainTag = getPlacement().DomainTag;
		temp_var->DeviceId = DmaStreamComputeDescriptor::CanonicalLane6DeviceId;
		return temp_var;
	}

	void DmaStreamComputeStatusMicroOp::ThrowRetireException(std::shared_ptr<DmaStreamComputeStatusQueryResult> result)
	{
		if (result->RejectKind == DmaStreamComputeStatusQueryRejectKind::OwnerDomainMismatch && _lastOwnerBinding && *_lastOwnerBinding)
		{
			throw DomainFaultException(NormalizeExecutionVtId(getOwnerThreadId()), 0,
				(*_lastOwnerBinding)->OwnerDomainTag, (*_lastOwnerBinding)->OwnerDomainTag);
		}

		throw InvalidOperationException(result->Message);
	}

	ResourceBitset DmaStreamComputeStatusMicroOp::BuildResourceMask(unsigned long long ownerDomainTag)
	{
		int resourceDomainBucket = static_cast<int>(ownerDomainTag & 0xFULL);
		std::shared_ptr<StreamEngineId> streamEngine = std::make_shared<StreamEngineId>(StreamEngineId::Zero);

		return ResourceMaskBuilder::ForDMAChannel(0) | ResourceMaskBuilder::ForStreamEngine(streamEngine) | ResourceMaskBuilder::ForMemoryDomain(resourceDomainBucket);
	}
}
