#include "VmcsV2Descriptor.h"
#include "VmcsV2FieldModel.h"
#include <utility>

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	VmcsV2Descriptor::VmcsV2Descriptor(std::shared_ptr<VmcsV2Header> header, std::shared_ptr<VmcsV2BlockDirectory> directory)
		: Header(header ? std::move(header) : throw std::invalid_argument("header")),
		  Directory(directory ? std::move(directory) : throw std::invalid_argument("directory"))
	{}

	std::shared_ptr<VmcsV2Header> VmcsV2Descriptor::getHeader() const
	{
		return Header;
	}

	std::shared_ptr<VmcsV2BlockDirectory> VmcsV2Descriptor::getDirectory() const
	{
		return Directory;
	}

	std::shared_ptr<VmxRootControlBlock> VmcsV2Descriptor::getRootControl() const
	{
		return RootControl;
	}

	std::shared_ptr<VirtualCpuBlock> VmcsV2Descriptor::getVirtualCpu() const
	{
		return VirtualCpu;
	}

	std::shared_ptr<BundleExecutionBlock> VmcsV2Descriptor::getBundleExecution() const
	{
		return BundleExecution;
	}

	std::shared_ptr<VmxPreemptionTimerBlock> VmcsV2Descriptor::getPreemptionTimer() const
	{
		return PreemptionTimer;
	}

	std::shared_ptr<VirtualInterruptFabricBlock> VmcsV2Descriptor::getInterruptFabric() const
	{
		return InterruptFabric;
	}

	std::shared_ptr<VectorStreamStateBlock> VmcsV2Descriptor::getVectorStreamState() const
	{
		return VectorStreamState;
	}

	std::shared_ptr<SecurityIsolationBlock> VmcsV2Descriptor::getSecurityIsolation() const
	{
		return SecurityIsolation;
	}

	std::shared_ptr<CapabilityNegotiationBlock> VmcsV2Descriptor::getCapabilityNegotiation() const
	{
		return CapabilityNegotiation;
	}

	std::shared_ptr<VmxNptBlock> VmcsV2Descriptor::getNptBlock() const
	{
		return NptBlock;
	}

	std::shared_ptr<ExitInfoBlock> VmcsV2Descriptor::getExitInfo() const
	{
		return ExitInfo;
	}

	std::shared_ptr<InterceptBitmapBlock> VmcsV2Descriptor::getInterceptBitmap() const
	{
		return InterceptBitmap;
	}

	std::shared_ptr<EventInjectionBlock> VmcsV2Descriptor::getEventInjection() const
	{
		return EventInjection;
	}

	std::shared_ptr<LaneCompletionRoutingBlock> VmcsV2Descriptor::getLaneCompletionRouting() const
	{
		return LaneCompletionRouting;
	}

	std::shared_ptr<IoVirtualizationBlock> VmcsV2Descriptor::getIoVirt() const
	{
		return IoVirt;
	}

	std::shared_ptr<Lane6StateBlock> VmcsV2Descriptor::getLane6State() const
	{
		return Lane6State;
	}

	std::shared_ptr<Lane7StateBlock> VmcsV2Descriptor::getLane7State() const
	{
		return Lane7State;
	}

	std::shared_ptr<DirtyLogBlock> VmcsV2Descriptor::getDirtyLog() const
	{
		return DirtyLog;
	}

	std::shared_ptr<DebugTraceBlock> VmcsV2Descriptor::getDebugTrace() const
	{
		return DebugTrace;
	}

	std::shared_ptr<VmcsV2Descriptor> VmcsV2Descriptor::CreateDefault()
	{
		return std::make_shared<VmcsV2Descriptor>(std::make_shared<VmcsV2Header>(), VmcsV2BlockDirectory::CreateDefault());
	}

	bool VmcsV2Descriptor::TryReadScalarField(unsigned short fieldId, long long &value, std::shared_ptr<VmcsV2ValidationResult> &validation)
	{
		std::shared_ptr<VmcsV2FieldDescriptor> descriptor;
		if (!getDirectory()->TryGetField(fieldId, descriptor))
		{
			value = 0;
			validation = UnknownField(fieldId);
			return false;
		}

		if (!descriptor->AccessPolicy.CanReadViaVmRead)
		{
			value = 0;
			validation = AccessDenied(descriptor, L"VMREAD is not permitted for this VMCSv2 field.");
			return false;
		}

		if (descriptor->ValueType != VmcsV2FieldValueType::UInt64 && descriptor->ValueType != VmcsV2FieldValueType::Epoch && descriptor->ValueType != VmcsV2FieldValueType::Boolean)
		{
			value = 0;
			validation = VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::TypeMismatch, fieldId, L"The field is not a scalar VMREAD payload.");
			return false;
		}

		value = 0;
		validation = AccessDenied(descriptor, L"VMCSv2 scalar projection cache was removed; scalar VMREAD requires generated read-only projection over neutral descriptors.");
		return false;
	}

	std::shared_ptr<VmcsV2ValidationResult> VmcsV2Descriptor::ValidateMigrationReadiness()
	{
		return getVirtualCpu()->getGprPersistence() != VmcsV2GprPersistenceKind::Complete ? VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::GuestGprPersistenceIncomplete, VmcsV2BlockDirectory::GuestIntegerRegisterFileFieldId, L"Migration requires PC, SP, and all 32 guest integer registers.") : getDirtyLog()->getOverflowed() ? VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::DirtyLogOverflow, VmcsV2BlockDirectory::DirtyLogBlockFieldId, L"Migration is blocked because dirty logging overflowed.") :!getVectorStreamState()->isMigrationReady() ? VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::VectorStreamStateIncomplete, VmcsV2BlockDirectory::VectorStreamStateBlockFieldId, L"Migration requires materialized vector/stream guest state when vector/stream virtualization is enabled.") : VmcsV2ValidationResult::Success(VmcsV2BlockDirectory::GuestIntegerRegisterFileFieldId);
	}

	std::shared_ptr<VmcsV2ValidationResult> VmcsV2Descriptor::ValidateNestedEnablementReadiness()
	{
		if (getVirtualCpu()->getGprPersistence() != VmcsV2GprPersistenceKind::Complete)
		{
			return VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::GuestGprPersistenceIncomplete, VmcsV2BlockDirectory::GuestIntegerRegisterFileFieldId, L"Nested enablement is blocked until the guest GPR file is materialized.");
		}

		return VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::NestedVmxDisabled, VmcsV2BlockDirectory::NptBlockFieldId, L"Legacy Shadow VMCS block was removed without replacement; nested enablement must use a generic nested-domain projection service.");
	}

	void VmcsV2Descriptor::RecordVectorExceptionExit(std::shared_ptr<VectorStreamExceptionInfo> info)
	{
		if (!info || !info->RequiresCompatibilityExitProjection)
		{
			return;
		}

		getExitInfo()->RecordVectorException(info);
	}

	void VmcsV2Descriptor::RecordStreamDescriptorFaultExit(std::shared_ptr<VectorStreamDescriptorFaultInfo> info)
	{
		if (!info || !info->IsFaulted)
		{
			return;
		}

		getExitInfo()->RecordStreamDescriptorFault(info);
	}

	void VmcsV2Descriptor::RecordStreamReplayRequiredExit(unsigned short ownerVirtualThreadId, unsigned short addressSpaceTag, unsigned long long streamReplayEpoch)
	{
		getExitInfo()->RecordStreamReplayRequired(ownerVirtualThreadId, addressSpaceTag, streamReplayEpoch);
	}

	std::shared_ptr<VmcsV2ValidationResult> VmcsV2Descriptor::UnknownField(unsigned short fieldId)
	{
		return VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::UnknownField, fieldId, L"The VMCSv2 field is not present in the typed block directory.");
	}

	std::shared_ptr<VmcsV2ValidationResult> VmcsV2Descriptor::AccessDenied(std::shared_ptr<VmcsV2FieldDescriptor> descriptor, const std::wstring &message)
	{
		return VmcsV2ValidationResult::Fail(VmcsV2ValidationCode::AccessDenied, descriptor->FieldId, message);
	}
}
