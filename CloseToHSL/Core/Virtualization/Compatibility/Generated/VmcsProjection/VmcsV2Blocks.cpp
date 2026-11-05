#include "VmcsV2Blocks.h"

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	bool VmxRootControlBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long VmxRootControlBlock::getRootDescriptorAddress() const
	{
		return 0;
	}

	unsigned long long VmxRootControlBlock::getOwnershipEpoch() const
	{
		return 0;
	}

	bool VmxNptBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	std::shared_ptr<MemoryTranslationControl> VmxNptBlock::getLastControl() const
	{
		return std::make_shared<MemoryTranslationControl>(MemoryTranslationControl::Disabled());
	}

	unsigned long long VmxNptBlock::getControlEpoch() const
	{
		return 0;
	}

	YAKSys_Hybrid_CPU::Core::VmExitReason ExitInfoBlock::getExitReason() const
	{
		return ExitReason;
	}

	void ExitInfoBlock::setExitReason(VmExitReason value)
	{
		ExitReason = value;
	}

	unsigned long long ExitInfoBlock::getExitQualification() const
	{
		return ExitQualification;
	}

	void ExitInfoBlock::setExitQualification(unsigned long long value)
	{
		ExitQualification = value;
	}

	unsigned long long ExitInfoBlock::getGuestPhysicalAddress() const
	{
		return GuestPhysicalAddress;
	}

	void ExitInfoBlock::setGuestPhysicalAddress(unsigned long long value)
	{
		GuestPhysicalAddress = value;
	}

	unsigned long long ExitInfoBlock::getEptViolationQualification() const
	{
		return EptViolationQualification;
	}

	void ExitInfoBlock::setEptViolationQualification(unsigned long long value)
	{
		EptViolationQualification = value;
	}

	std::shared_ptr<TranslationViolationInfo> ExitInfoBlock::getNptViolation() const
	{
		return NptViolation;
	}

	void ExitInfoBlock::setNptViolation(std::shared_ptr<TranslationViolationInfo> value)
	{
		NptViolation = value;
	}

	void ExitInfoBlock::RecordVectorException(std::shared_ptr<VectorStreamExceptionInfo> info)
	{
		setExitReason(VmExitReason::VectorException);
		setExitQualification(info->EncodeCompatibilityQualification());
		setGuestPhysicalAddress(info->FaultingPc);
		setEptViolationQualification(0);
		setNptViolation(nullptr);
	}

	void ExitInfoBlock::RecordStreamDescriptorFault(std::shared_ptr<VectorStreamDescriptorFaultInfo> info)
	{
		setExitReason(VmExitReason::StreamDescriptorFault);
		setExitQualification(info->EncodeCompatibilityQualification());
		setGuestPhysicalAddress(info->GuestDescriptorAddress);
		setEptViolationQualification(0);
		setNptViolation(nullptr);
	}

	void ExitInfoBlock::RecordStreamReplayRequired(unsigned short ownerVirtualThreadId, unsigned short addressSpaceTag, unsigned long long streamReplayEpoch)
	{
		setExitReason(VmExitReason::StreamReplayRequired);
		setExitQualification(((static_cast<unsigned long long>(ownerVirtualThreadId) & 0xFFFFULL) << 16) | ((static_cast<unsigned long long>(addressSpaceTag) & 0xFFFFULL) << 32));
		setGuestPhysicalAddress(streamReplayEpoch);
		setEptViolationQualification(0);
		setNptViolation(nullptr);
	}

	unsigned long long VirtualCpuBlock::getGuestPc() const
	{
		return GuestPc;
	}

	void VirtualCpuBlock::setGuestPc(unsigned long long value)
	{
		GuestPc = value;
	}

	unsigned long long VirtualCpuBlock::getGuestSp() const
	{
		return GuestSp;
	}

	void VirtualCpuBlock::setGuestSp(unsigned long long value)
	{
		GuestSp = value;
	}

	VmcsV2GprPersistenceKind VirtualCpuBlock::getGprPersistence() const
	{
		return GprPersistence;
	}

	void VirtualCpuBlock::setGprPersistence(VmcsV2GprPersistenceKind value)
	{
		GprPersistence = value;
	}

	const std::vector<unsigned long long> VirtualCpuBlock::getGuestIntegerRegisters() const
	{
		return _guestIntegerRegisters;
	}

	bool BundleExecutionBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long BundleExecutionBlock::getBundlePc() const
	{
		return 0;
	}

	unsigned long long BundleExecutionBlock::getExecutionEpoch() const
	{
		return 0;
	}

	bool VmxPreemptionTimerBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long VmxPreemptionTimerBlock::getTimerEpoch() const
	{
		return 0;
	}

	bool VirtualInterruptFabricBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long VirtualInterruptFabricBlock::getRoutingEpoch() const
	{
		return 0;
	}

	bool InterceptBitmapBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long InterceptBitmapBlock::getPolicyEpoch() const
	{
		return 0;
	}

	bool EventInjectionBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long EventInjectionBlock::getInjectionEpoch() const
	{
		return 0;
	}

	unsigned long long EventInjectionBlock::getDeliveryEpoch() const
	{
		return 0;
	}

	std::shared_ptr<EventInjectionDescriptor> EventInjectionBlock::getLastDelivered() const
	{
		return nullptr;
	}

	int EventInjectionBlock::getPendingCount() const
	{
		return 0;
	}

	unsigned long long EventInjectionBlock::getDroppedCount() const
	{
		return 0;
	}

	unsigned long long EventInjectionBlock::getInvalidDroppedCount() const
	{
		return 0;
	}

	unsigned long long EventInjectionBlock::getCapacityDroppedCount() const
	{
		return 0;
	}

	unsigned long long EventInjectionBlock::getRemapDroppedCount() const
	{
		return 0;
	}

	unsigned long long EventInjectionBlock::getRemappedCount() const
	{
		return 0;
	}

	unsigned long long EventInjectionBlock::getCoalescedCount() const
	{
		return 0;
	}

	unsigned long long EventInjectionBlock::getRemapPolicyEpoch() const
	{
		return 0;
	}

	bool LaneCompletionRoutingBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long LaneCompletionRoutingBlock::getRoutingEpoch() const
	{
		return 0;
	}

	unsigned long long LaneCompletionRoutingBlock::getRoutedCount() const
	{
		return 0;
	}

	unsigned long long LaneCompletionRoutingBlock::getIgnoredDisabledCount() const
	{
		return 0;
	}

	unsigned long long LaneCompletionRoutingBlock::getInvalidDroppedCount() const
	{
		return 0;
	}

	unsigned long long LaneCompletionRoutingBlock::getCapacityDroppedCount() const
	{
		return 0;
	}

	unsigned long long LaneCompletionRoutingBlock::getEventQueueDroppedCount() const
	{
		return 0;
	}

	bool VectorStreamStateBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	bool VectorStreamStateBlock::getVirtualizationEnabled() const
	{
		return false;
	}

	YAKSys_Hybrid_CPU::Core::VectorStreamSaveMask VectorStreamStateBlock::getSaveRestoreMask() const
	{
		return VectorStreamSaveMask::None;
	}

	YAKSys_Hybrid_CPU::Core::VectorExceptionAction VectorStreamStateBlock::getExceptionPolicy() const
	{
		return VectorExceptionAction::Accumulate;
	}

	unsigned long long VectorStreamStateBlock::getStreamDescriptorTableBase() const
	{
		return 0;
	}

	unsigned long long VectorStreamStateBlock::getStreamDescriptorTableLimit() const
	{
		return 0;
	}

	unsigned int VectorStreamStateBlock::getMaxStreamLength() const
	{
		return DefaultMaxStreamLength;
	}

	unsigned long long VectorStreamStateBlock::getStreamEpoch() const
	{
		return 0;
	}

	unsigned long long VectorStreamStateBlock::getPolicyEpoch() const
	{
		return 0;
	}

	unsigned long long VectorStreamStateBlock::getArchitecturalEpoch() const
	{
		return 0;
	}

	unsigned long long VectorStreamStateBlock::getDirtyEpoch() const
	{
		return 0;
	}

	unsigned long long VectorStreamStateBlock::getStreamReplayEpoch() const
	{
		return 0;
	}

	unsigned long long VectorStreamStateBlock::getStreamQueueEpoch() const
	{
		return 0;
	}

	unsigned long long VectorStreamStateBlock::getStreamCompletionEpoch() const
	{
		return 0;
	}

	std::shared_ptr<VectorStreamSnapshot> VectorStreamStateBlock::getLastSnapshot() const
	{
		return nullptr;
	}

	std::shared_ptr<VectorStreamExceptionInfo> VectorStreamStateBlock::getLastVectorException() const
	{
		return nullptr;
	}

	std::shared_ptr<VectorStreamDescriptorFaultInfo> VectorStreamStateBlock::getLastDescriptorFault() const
	{
		return std::make_shared<VectorStreamDescriptorFaultInfo>(VectorStreamDescriptorFaultInfo::None);
	}

	bool VectorStreamStateBlock::getHasDescriptorTable() const
	{
		return getStreamDescriptorTableLimit() > getStreamDescriptorTableBase();
	}

	bool VectorStreamStateBlock::getRequiresMigratableVectorState() const
	{
		return getVirtualizationEnabled() &&
			(static_cast<unsigned int>(getSaveRestoreMask()) & static_cast<unsigned int>(VectorStreamSaveMask::Architectural)) != 0;
	}

	bool VectorStreamStateBlock::isMigrationReady() const
	{
		return !getRequiresMigratableVectorState() || getLastSnapshot() != nullptr;
	}

	bool SecurityIsolationBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long SecurityIsolationBlock::getIsolationEpoch() const
	{
		return 0;
	}

	bool CapabilityNegotiationBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	unsigned long long CapabilityNegotiationBlock::getCapabilityEpoch() const
	{
		return 0;
	}

	unsigned short DirtyLogBlock::getVersion() const
	{
		return 1;
	}

	bool DirtyLogBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	bool DirtyLogBlock::getEnabled() const
	{
		return false;
	}

	unsigned long long DirtyLogBlock::getGuestPhysicalBase() const
	{
		return 0;
	}

	unsigned long long DirtyLogBlock::getGuestPhysicalLimitExclusive() const
	{
		return 0;
	}

	unsigned int DirtyLogBlock::getPageSize() const
	{
		return VmxDirtyLogConfiguration::DefaultPageSize;
	}

	unsigned int DirtyLogBlock::getMaxDirtyPages() const
	{
		return VmxDirtyLogConfiguration::DefaultMaxDirtyPages;
	}

	YAKSys_Hybrid_CPU::Core::VmxDirtyLogOverflowPolicy DirtyLogBlock::getOverflowPolicy() const
	{
		return VmxDirtyLogOverflowPolicy::FailClosed;
	}

	unsigned long long DirtyLogBlock::getGeneration() const
	{
		return 0;
	}

	unsigned long long DirtyLogBlock::getWriteSequence() const
	{
		return 0;
	}

	bool DirtyLogBlock::getOverflowed() const
	{
		return false;
	}

	unsigned long long DirtyLogBlock::getCpuWriteCount() const
	{
		return 0;
	}

	unsigned long long DirtyLogBlock::getAtomicWriteCount() const
	{
		return 0;
	}

	unsigned long long DirtyLogBlock::getDmaWriteCount() const
	{
		return 0;
	}

	unsigned long long DirtyLogBlock::getLane6WriteCount() const
	{
		return 0;
	}

	unsigned long long DirtyLogBlock::getLane7WriteCount() const
	{
		return 0;
	}

	unsigned long long DirtyLogBlock::getNptWriteProtectCount() const
	{
		return 0;
	}

	unsigned long long DirtyLogBlock::getDroppedWriteCount() const
	{
		return 0;
	}

	bool DirtyLogBlock::ContainsHostEvidence(VmcsV2HostEvidenceKind evidence)
	{
		return false;
	}

	std::shared_ptr<VmxDirtyLogStatus> DirtyLogBlock::SnapshotStatus()
	{
		return std::make_shared<VmxDirtyLogStatus>(getEnabled(), getPageSize(), getGeneration(), getWriteSequence(),
			getOverflowed(), 0, getCpuWriteCount(), getAtomicWriteCount(), getDmaWriteCount(),
			getLane6WriteCount(), getLane7WriteCount(), getNptWriteProtectCount(), getDroppedWriteCount());
	}

	unsigned short DebugTraceBlock::getVersion() const
	{
		return 1;
	}

	bool DebugTraceBlock::isReadOnlyCompatibilityProjection() const
	{
		return true;
	}

	bool DebugTraceBlock::getExportEnabled() const
	{
		return false;
	}

	unsigned long long DebugTraceBlock::getPolicyEpoch() const
	{
		return 0;
	}

	std::shared_ptr<VmxDebugTraceCounters> DebugTraceBlock::getCounters() const
	{
		return nullptr;
	}

	bool DebugTraceBlock::ContainsHostEvidence(VmcsV2HostEvidenceKind evidence)
	{
		return false;
	}
}
