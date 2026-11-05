#include "DmaStreamComputeQueryCapsMicroOp.h"
#include "../../../Architecture/Common/StreamEngineId.h"
#include "../../../Diagnostics/DecodeProjectionFaultException.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;

namespace YAKSys_Hybrid_CPU::Core
{

	DmaStreamComputeQueryCapsMicroOp::DmaStreamComputeQueryCapsMicroOp(unsigned short destinationRegister)
		: DestinationRegister(NormalizeDestinationRegister(destinationRegister))
	{
		setOpCode(Processor::CPU_Core::IsaOpcodeValues::DSC_QUERY_CAPS);
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

	unsigned short DmaStreamComputeQueryCapsMicroOp::NormalizeDestinationRegister(unsigned short destinationRegister)
	{
		int destRegId;
		if (!TryNormalizeFlatArchRegId(destinationRegister, destRegId))
		{
			throw DecodeProjectionFaultException(L"DSC_QUERY_CAPS requires a flat architectural rd register id.");
		}
		return static_cast<unsigned short>(destRegId);
	}

	unsigned short DmaStreamComputeQueryCapsMicroOp::getDestinationRegister() const
	{
		return DestinationRegister;
	}

	std::optional<std::shared_ptr<DmaStreamComputeCapabilityQueryResult>> DmaStreamComputeQueryCapsMicroOp::getLastQueryResult() const
	{
		return _lastQueryResult;
	}

	std::optional<std::shared_ptr<DmaStreamComputeOwnerBinding>> DmaStreamComputeQueryCapsMicroOp::getLastOwnerBinding() const
	{
		return _lastOwnerBinding;
	}

	std::shared_ptr<DmaStreamComputeCapabilityQueryReplayEvidence> DmaStreamComputeQueryCapsMicroOp::getLastQueryReplayEvidence() const
	{
		return LastQueryReplayEvidence;
	}

	void DmaStreamComputeQueryCapsMicroOp::setLastQueryReplayEvidence(std::shared_ptr<DmaStreamComputeCapabilityQueryReplayEvidence> value)
	{
		LastQueryReplayEvidence = value;
	}

	bool DmaStreamComputeQueryCapsMicroOp::getUsedStreamEngineFallback() const
	{
		return false;
	}

	bool DmaStreamComputeQueryCapsMicroOp::getUsedDmaControllerFallback() const
	{
		return false;
	}

	CanonicalDecodePublicationMode DmaStreamComputeQueryCapsMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	bool DmaStreamComputeQueryCapsMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		RefreshOwnerDependentMetadata();

		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		auto ownerBinding = BuildOwnerBinding(core, vtId);
		_lastOwnerBinding = ownerBinding;
		auto queryResult = DmaStreamComputeCapabilityQueryResult::Capture(ownerBinding);
		_lastQueryResult = queryResult;
		_capturedCapabilityWord = queryResult->EncodedCapabilityWord;
		setLastQueryReplayEvidence(DmaStreamComputeCapabilityQueryReplayEvidence::Capture(ownerBinding, queryResult,
			DmaStreamComputeLanePlacementEvidence::MaterializedLane6(6, SlotClassLaneMap::GetLaneMask(SlotClass::DmaStreamClass), 0, false)));

		return true;
	}

	void DmaStreamComputeQueryCapsMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		if (!_lastQueryResult || !*_lastQueryResult)
		{
			throw InvalidOperationException(L"DSC_QUERY_CAPS retire requires a prior execute/capture result.");
		}

		if (!getWritesRegister())
		{
			return;
		}

		int vtId = NormalizeExecutionVtId(getOwnerThreadId());
		AppendWriteBackRetireRecord(retireRecords, retireRecordCount, RetireRecord::RegisterWrite(vtId, getDestinationRegister(), _capturedCapabilityWord));
	}

	bool DmaStreamComputeQueryCapsMicroOp::TryGetPrimaryWriteBackResult(unsigned long long &value)
	{
		value = _capturedCapabilityWord;
		return _lastQueryResult && *_lastQueryResult && (*_lastQueryResult)->IsAccepted && getWritesRegister();
	}

	void DmaStreamComputeQueryCapsMicroOp::CapturePrimaryWriteBackResult(unsigned long long value)
	{
		_capturedCapabilityWord = value;
	}

	void DmaStreamComputeQueryCapsMicroOp::RefreshWriteMetadata()
	{
		RefreshOwnerDependentMetadata();
	}

	void DmaStreamComputeQueryCapsMicroOp::RefreshOwnerDependentMetadata()
	{
		setReadRegisters(std::vector<int>{});
		setWriteRegisters(getWritesRegister() ? std::vector<int>{static_cast<int>(getDestinationRegister())} : std::vector<int>{});
		setReadMemoryRanges(std::vector<MemoryRange>{});
		setWriteMemoryRanges(std::vector<MemoryRange>{});
		setResourceMask(BuildResourceMask(getPlacement().DomainTag));
		PublishExplicitStructuralSafetyMask();
		RefreshAdmissionMetadata();
	}

	std::wstring DmaStreamComputeQueryCapsMicroOp::GetDescription()
	{
		return std::format(L"DSC_QUERY_CAPS: rd=x{}", getDestinationRegister());
	}

	std::shared_ptr<DmaStreamComputeOwnerBinding> DmaStreamComputeQueryCapsMicroOp::BuildOwnerBinding(std::shared_ptr<Processor::CPU_Core> &core, int vtId)
	{
		if (getOwnerContextId() < 0)
		{
			throw InvalidOperationException(L"DSC_QUERY_CAPS owner context id is not materialized.");
		}

		std::shared_ptr<DmaStreamComputeOwnerBinding> temp_var = std::make_shared<DmaStreamComputeOwnerBinding>();
		temp_var->OwnerVirtualThreadId = static_cast<unsigned short>(vtId);
		temp_var->OwnerContextId = static_cast<unsigned int>(getOwnerContextId());
		temp_var->OwnerCoreId = core->getCoreID();
		temp_var->OwnerPodId = Lane6QueryOwnerPodId;
		temp_var->OwnerDomainTag = getPlacement().DomainTag;
		temp_var->DeviceId = DmaStreamComputeDescriptor::CanonicalLane6DeviceId;
		return temp_var;
	}

	ResourceBitset DmaStreamComputeQueryCapsMicroOp::BuildResourceMask(unsigned long long ownerDomainTag)
	{
		int resourceDomainBucket = static_cast<int>(ownerDomainTag & 0xFULL);
		std::shared_ptr<StreamEngineId> streamEngine = std::make_shared<StreamEngineId>(StreamEngineId::Zero);

		return ResourceMaskBuilder::ForDMAChannel(0) | ResourceMaskBuilder::ForStreamEngine(streamEngine) | ResourceMaskBuilder::ForMemoryDomain(resourceDomainBucket);
	}
}
