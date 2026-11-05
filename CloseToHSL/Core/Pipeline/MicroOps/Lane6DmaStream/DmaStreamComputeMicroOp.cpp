#include "DmaStreamComputeMicroOp.h"
#include "../../../Architecture/Common/StreamEngineId.h"
#include "../../../../../NonRTL/Arch/InstructionClass.h"
#include "../../../../../NonRTL/Arch/SerializationClass.h"
#include "../../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "../../../Execution/DmaStreamCompute/DmaStreamComputeRuntime.h"
#include "../../../../../Arch/CsrAddresses.h"
#include "../../../Architecture/Registers/CSR/CsrAccessPolicy.h"
#include "../../../Architecture/State/Pipeline/PipelineState.h"

using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Registers;
using namespace YAKSys_Hybrid_CPU::Core::Registers::Retire;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	namespace
	{
		const wchar_t* OperationName(DmaStreamComputeOperationKind value)
		{
		switch (value)
		{
			case DmaStreamComputeOperationKind::Copy: return L"Copy";
			case DmaStreamComputeOperationKind::Add: return L"Add";
			case DmaStreamComputeOperationKind::Mul: return L"Mul";
			case DmaStreamComputeOperationKind::Fma: return L"Fma";
			case DmaStreamComputeOperationKind::Reduce: return L"Reduce";
			default: return L"Unknown";
		}
		}

		const wchar_t* ElementTypeName(DmaStreamComputeElementType value)
		{
		switch (value)
		{
			case DmaStreamComputeElementType::UInt8: return L"UInt8";
			case DmaStreamComputeElementType::UInt16: return L"UInt16";
			case DmaStreamComputeElementType::UInt32: return L"UInt32";
			case DmaStreamComputeElementType::UInt64: return L"UInt64";
			case DmaStreamComputeElementType::Float32: return L"Float32";
			case DmaStreamComputeElementType::Float64: return L"Float64";
			default: return L"Unknown";
		}
		}

		const wchar_t* ShapeName(DmaStreamComputeShapeKind value)
		{
		switch (value)
		{
			case DmaStreamComputeShapeKind::Contiguous1D: return L"Contiguous1D";
			case DmaStreamComputeShapeKind::FixedReduce: return L"FixedReduce";
			default: return L"Unknown";
		}
		}
	}

	DmaStreamComputeMicroOp::DmaStreamComputeMicroOp(std::shared_ptr<DmaStreamComputeDescriptor> descriptor)
		: Descriptor(ValidateDescriptorForConstruction(descriptor)),
		  DescriptorReference(std::make_shared<DmaStreamComputeDescriptorReference>(Descriptor->DescriptorReference)),
		  DescriptorIdentityHash(Descriptor->DescriptorIdentityHash),
		  CertificateInputHash(Descriptor->CertificateInputHash),
		  NormalizedFootprintHash(Descriptor->NormalizedFootprintHash),
		  Operation(Descriptor->Operation),
		  ElementType(Descriptor->ElementType),
		  Shape(Descriptor->Shape),
		  RangeEncoding(Descriptor->RangeEncoding),
		  PartialCompletionPolicy(Descriptor->PartialCompletionPolicy),
		  OwnerBinding(Descriptor->OwnerBinding),
		  ReplayEvidence(DmaStreamComputeReplayEvidence::CreateForDescriptor(Descriptor))
	{
		setLastExecutionReplayEvidence(getReplayEvidence());

		setMemoryOp(true);
		setHasSideEffects(true);
		setClass(MicroOpClass::Dma);
		setInstructionClass(Arch::InstructionClass::Memory);
		setSerializationClass(Arch::SerializationClass::MemoryOrdered);
		setWritesRegister(false);

		setOwnerThreadId(OwnerBinding->OwnerVirtualThreadId);
		setVirtualThreadId(OwnerBinding->OwnerVirtualThreadId);
		setOwnerContextId(ConvertOwnerContextId(OwnerBinding->OwnerContextId));

		SetHardPinnedPlacement(SlotClass::DmaStreamClass, 6);
		auto placement = getPlacement();
		placement.DomainTag = OwnerBinding->OwnerDomainTag;
		setPlacement(placement);
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		Placement = Placement with { DomainTag = descriptor.OwnerBinding.OwnerDomainTag };

		setReadRegisters(std::vector<int>{});
		setWriteRegisters(std::vector<int>{});
		setReadMemoryRanges(ConvertRanges(Descriptor->NormalizedReadMemoryRanges));
		setWriteMemoryRanges(ConvertRanges(Descriptor->NormalizedWriteMemoryRanges));

		setResourceMask(BuildResourceMask(OwnerBinding->OwnerDomainTag));
		PublishExplicitStructuralSafetyMask();
// C# TO C++ CONVERTER TASK: You cannot use 'shared_from_this' in a constructor:
		RefreshAdmissionMetadata();
	}

	std::shared_ptr<DmaStreamComputeDescriptor> DmaStreamComputeMicroOp::ValidateDescriptorForConstruction(std::shared_ptr<DmaStreamComputeDescriptor> descriptor)
	{
		ArgumentNullException::ThrowIfNull(descriptor);
		EnsureOwnerDomainGuardAccepted(descriptor);
		EnsureMandatoryFootprints(descriptor);
		return descriptor;
	}

	std::shared_ptr<DmaStreamComputeDescriptor> DmaStreamComputeMicroOp::getDescriptor() const
	{
		return Descriptor;
	}

	std::shared_ptr<DmaStreamComputeDescriptorReference> DmaStreamComputeMicroOp::getDescriptorReference() const
	{
		return DescriptorReference;
	}

	unsigned long long DmaStreamComputeMicroOp::getDescriptorIdentityHash() const
	{
		return DescriptorIdentityHash;
	}

	unsigned long long DmaStreamComputeMicroOp::getCertificateInputHash() const
	{
		return CertificateInputHash;
	}

	unsigned long long DmaStreamComputeMicroOp::getNormalizedFootprintHash() const
	{
		return NormalizedFootprintHash;
	}

	DmaStreamComputeOperationKind DmaStreamComputeMicroOp::getOperation() const
	{
		return Operation;
	}

	DmaStreamComputeElementType DmaStreamComputeMicroOp::getElementType() const
	{
		return ElementType;
	}

	DmaStreamComputeShapeKind DmaStreamComputeMicroOp::getShape() const
	{
		return Shape;
	}

	DmaStreamComputeRangeEncoding DmaStreamComputeMicroOp::getRangeEncoding() const
	{
		return RangeEncoding;
	}

	DmaStreamComputePartialCompletionPolicy DmaStreamComputeMicroOp::getPartialCompletionPolicy() const
	{
		return PartialCompletionPolicy;
	}

	std::shared_ptr<DmaStreamComputeOwnerBinding> DmaStreamComputeMicroOp::getOwnerBinding() const
	{
		return OwnerBinding;
	}

	std::shared_ptr<DmaStreamComputeReplayEvidence> DmaStreamComputeMicroOp::getReplayEvidence() const
	{
		return ReplayEvidence;
	}

	std::optional<std::shared_ptr<DmaStreamComputeExecutionResult>> DmaStreamComputeMicroOp::getLastExecutionResult() const
	{
		return _lastExecutionResult;
	}

	std::optional<std::shared_ptr<DmaStreamComputeCommitResult>> DmaStreamComputeMicroOp::getLastRetireCommitResult() const
	{
		return _lastRetireCommitResult;
	}

	std::shared_ptr<DmaStreamComputeToken> DmaStreamComputeMicroOp::getLastExecutionToken() const
	{
		return (!_lastExecutionResult || !*_lastExecutionResult) ? nullptr : _lastExecutionResult.value()->Token;
	}

	DmaStreamComputeTokenHandle DmaStreamComputeMicroOp::getLastExecutionTokenHandle() const
	{
		return (!_lastExecutionResult || !*_lastExecutionResult) ? DmaStreamComputeTokenHandle{} : _lastExecutionResult.value()->TokenHandle;
	}

	int DmaStreamComputeMicroOp::getActiveTokenCount() const
	{
		return _tokenStore->getActiveTokenCount();
	}

	std::shared_ptr<DmaStreamComputeReplayEvidence> DmaStreamComputeMicroOp::getLastExecutionReplayEvidence() const
	{
		return LastExecutionReplayEvidence;
	}

	void DmaStreamComputeMicroOp::setLastExecutionReplayEvidence(std::shared_ptr<DmaStreamComputeReplayEvidence> value)
	{
		LastExecutionReplayEvidence = value;
	}

	CanonicalDecodePublicationMode DmaStreamComputeMicroOp::getCanonicalDecodePublication() const
	{
		return CanonicalDecodePublicationMode::SelfPublishes;
	}

	std::shared_ptr<DmaStreamComputeReplayEvidence> DmaStreamComputeMicroOp::ExportReplayEvidence(DmaStreamComputeTokenLifecycleEvidence tokenLifecycleEvidence, DmaStreamComputeLanePlacementEvidence lanePlacementEvidence)
	{
		return DmaStreamComputeReplayEvidence::CreateForDescriptor(getDescriptor(), {}, tokenLifecycleEvidence, lanePlacementEvidence);
	}

	bool DmaStreamComputeMicroOp::Execute(std::shared_ptr<Processor::CPU_Core> &core)
	{
		EnsurePhase06ProductionContour(getDescriptor());
		EnsureNoUnretiredExecution();
		RejectGuestCompatibilityExecution(core);
		_tokenStore = core->GetDmaStreamComputeTokenStore();

		std::shared_ptr<DmaStreamComputeIssueAdmissionMetadata> issueMetadata = std::make_shared<DmaStreamComputeIssueAdmissionMetadata>(DmaStreamComputeIssueAdmissionMetadata::Lane6());
		std::shared_ptr<DmaStreamComputeExecutionResult> execution = DmaStreamComputeRuntime::ExecuteMaterializedMicroOpToCommitPending(shared_from_this(), _tokenStore, issueMetadata);

		_lastExecutionResult = execution;
		auto lifecycleEvidence = execution->Token->ExportLifecycleEvidence();
		setLastExecutionReplayEvidence(ExportReplayEvidence(*lifecycleEvidence, DmaStreamComputeLanePlacementEvidence::MaterializedLane6(6, SlotClassLaneMap::GetLaneMask(SlotClass::DmaStreamClass), 0, false)));
		return execution->IsCommitPending || execution->RequiresRetireExceptionPublication;
	}

	void DmaStreamComputeMicroOp::Commit(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (!_lastExecutionResult)
		{
			throw InvalidOperationException(L"DmaStreamComputeMicroOp retire commit requires a prior successful lane6 Execute capture.");
		}

		auto execution = _lastExecutionResult.value();
		_lastRetireCommitResult = core->ApplyDmaStreamComputeRetireCommit(execution->Token, getDescriptor()->OwnerGuardDecision);
		auto lifecycleEvidence = execution->Token->ExportLifecycleEvidence();
		setLastExecutionReplayEvidence(ExportReplayEvidence(*lifecycleEvidence, DmaStreamComputeLanePlacementEvidence::MaterializedLane6(6, SlotClassLaneMap::GetLaneMask(SlotClass::DmaStreamClass), 0, false)));
	}

	void DmaStreamComputeMicroOp::EmitWriteBackRetireRecords(std::shared_ptr<Processor::CPU_Core> &core, std::span<RetireRecord> retireRecords, int &retireRecordCount)
	{
		Commit(core);
	}

	std::wstring DmaStreamComputeMicroOp::GetDescription()
	{
		return std::format(L"DmaStreamCompute: Op={}, Type={}, Shape={}, Descriptor=0x{:016X}, Footprint=0x{:016X}", OperationName(getOperation()), ElementTypeName(getElementType()), ShapeName(getShape()), getDescriptorIdentityHash(), getNormalizedFootprintHash());
	}

	void DmaStreamComputeMicroOp::EnsureMandatoryFootprints(std::shared_ptr<DmaStreamComputeDescriptor> descriptor)
	{
		if (descriptor->ReadMemoryRanges.empty() || descriptor->NormalizedReadMemoryRanges.empty())
		{
			throw InvalidOperationException(L"DmaStreamComputeMicroOp requires non-empty ReadMemoryRanges before typed-slot admission.");
		}

		if (IsMemoryWritingOperation(descriptor->Operation) && (descriptor->WriteMemoryRanges.empty() || descriptor->NormalizedWriteMemoryRanges.empty()))
		{
			throw InvalidOperationException(L"DmaStreamComputeMicroOp requires non-empty WriteMemoryRanges for memory-writing operations before typed-slot admission.");
		}
	}

	void DmaStreamComputeMicroOp::EnsureOwnerDomainGuardAccepted(std::shared_ptr<DmaStreamComputeDescriptor> descriptor)
	{
		if (!descriptor->OwnerGuardDecision || !descriptor->OwnerGuardDecision->IsAllowed)
		{
			throw InvalidOperationException(L"DmaStreamComputeMicroOp requires an accepted owner/domain guard decision before typed-slot admission.");
		}

		if (!descriptor->OwnerBinding || !descriptor->OwnerGuardDecision->DescriptorOwnerBinding || *descriptor->OwnerGuardDecision->DescriptorOwnerBinding != *descriptor->OwnerBinding)
		{
			throw InvalidOperationException(L"DmaStreamComputeMicroOp owner/domain guard decision does not match the descriptor owner binding.");
		}
	}

	bool DmaStreamComputeMicroOp::IsMemoryWritingOperation(DmaStreamComputeOperationKind operation)
	{
		return (operation == DmaStreamComputeOperationKind::Copy || operation == DmaStreamComputeOperationKind::Add || operation == DmaStreamComputeOperationKind::Mul || operation == DmaStreamComputeOperationKind::Fma || operation == DmaStreamComputeOperationKind::Reduce);
	}

	void DmaStreamComputeMicroOp::EnsurePhase06ProductionContour(std::shared_ptr<DmaStreamComputeDescriptor> descriptor)
	{
		if (!IsMemoryWritingOperation(descriptor->Operation) || ResolveElementSize(descriptor->ElementType) == 0 || !IsPhase06Shape(descriptor->Operation, descriptor->Shape) || descriptor->RangeEncoding != DmaStreamComputeRangeEncoding::InlineContiguous || descriptor->PartialCompletionPolicy != DmaStreamComputePartialCompletionPolicy::AllOrNone)
		{
			throw InvalidOperationException(L"DmaStreamComputeMicroOp production execution rejects descriptors outside the Phase 06 DSC1 contour and fails closed without StreamEngine or DMAController fallback.");
		}
	}

	void DmaStreamComputeMicroOp::EnsureNoUnretiredExecution()
	{
		if (!_lastExecutionResult || !*_lastExecutionResult)
		{
			return;
		}

		const auto& token = _lastExecutionResult.value()->Token;
		if (token && (token->getState() == DmaStreamComputeTokenState::CommitPending || token->getState() == DmaStreamComputeTokenState::Faulted))
		{
			throw InvalidOperationException(L"DmaStreamComputeMicroOp already has a captured token that must retire, fault, or be canceled before re-execution.");
		}
	}

	std::vector<DmaStreamComputeMicroOp::MemoryRange> DmaStreamComputeMicroOp::ConvertRanges(const std::vector<DmaStreamComputeMemoryRange>& ranges)
	{
		std::vector<MemoryRange> converted;
		converted.reserve(ranges.size());
		for (const auto& range : ranges)
		{
			converted.push_back(MemoryRange{range.Address, range.Length});
		}
		return converted;
	}

	void DmaStreamComputeMicroOp::RejectGuestCompatibilityExecution(std::shared_ptr<Processor::CPU_Core> &core)
	{
		if (core->getCsr()->Read(CsrAddresses::VmxEnable, PrivilegeLevel::Machine) == 0)
		{
			return;
		}

		bool compatibilityGuestExecution;
		try
		{
			compatibilityGuestExecution = core->ReadVirtualThreadPipelineState(getOwnerThreadId()) == PipelineState::GuestExecution;
		}
		catch (const ArgumentOutOfRangeException &e1)
		{
			compatibilityGuestExecution = core->HasAnyVirtualThreadPipelineState(PipelineState::GuestExecution);
		}

		if (compatibilityGuestExecution)
		{
			throw InvalidOperationException(L"Guest Lane6 compatibility execution is fail-closed: no runtime-owned DMA domain binding is admitted for the frozen VMX frontend.");
		}
	}

	bool DmaStreamComputeMicroOp::IsPhase06Shape(DmaStreamComputeOperationKind operation, DmaStreamComputeShapeKind shape)
	{
		return shape == DmaStreamComputeShapeKind::Contiguous1D || (operation == DmaStreamComputeOperationKind::Reduce && shape == DmaStreamComputeShapeKind::FixedReduce);
	}

	int DmaStreamComputeMicroOp::ResolveElementSize(DmaStreamComputeElementType elementType)
	{
		int temp_var118;
		switch (elementType)
		{
			case DmaStreamComputeElementType::UInt8:
				temp_var118 = 1;
				break;
			case DmaStreamComputeElementType::UInt16:
				temp_var118 = 2;
				break;
			case DmaStreamComputeElementType::UInt32:
				temp_var118 = 4;
				break;
			case DmaStreamComputeElementType::UInt64:
				temp_var118 = 8;
				break;
			case DmaStreamComputeElementType::Float32:
				temp_var118 = 4;
				break;
			case DmaStreamComputeElementType::Float64:
				temp_var118 = 8;
				break;
			default:
				temp_var118 = 0;
				break;
		}
		return temp_var118;
	}

	ResourceBitset DmaStreamComputeMicroOp::BuildResourceMask(unsigned long long ownerDomainTag)
	{
		int resourceDomainBucket = static_cast<int>(ownerDomainTag & 0xFULL);
		std::shared_ptr<StreamEngineId> streamEngine = std::make_shared<StreamEngineId>(StreamEngineId::Zero);

		return ResourceMaskBuilder::ForDMAChannel(0) | ResourceMaskBuilder::ForStreamEngine(streamEngine) | ResourceMaskBuilder::ForLoad() | ResourceMaskBuilder::ForStore() | ResourceMaskBuilder::ForMemoryDomain(resourceDomainBucket);
	}

	int DmaStreamComputeMicroOp::ConvertOwnerContextId(unsigned int ownerContextId)
	{
		if (ownerContextId > std::numeric_limits<int>::max())
		{
			throw InvalidOperationException(L"DmaStreamCompute owner context id exceeds the current runtime owner-context field width.");
		}

		return static_cast<int>(ownerContextId);
	}
}
