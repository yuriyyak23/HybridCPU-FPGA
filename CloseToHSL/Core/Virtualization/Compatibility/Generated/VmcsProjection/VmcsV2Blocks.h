#pragma once

#include "../../FrozenAbi/VmcsFieldAliases/VmExitReason.h"
#include "../../../../Runtime/Lanes/VectorStream/State/VectorStreamState.h"
#include "../../../../Runtime/Events/Injection/EventInjectionDescriptor.h"
#include "../../../../System/VectorStreamExceptionInfo.h"
#include "../../../../System/VectorStreamDescriptorFaultInfo.h"
#include "../../Frontend/Projection/Memory/MemoryTranslationControl.h"
#include "../../../../Runtime/Memory/Translation/NestedTranslationResult.h"
#include "../../../../Runtime/Lanes/VectorStream/SaveRestore/YAKSys_Hybrid_CPU.Core.VectorStreamSnapshot.h"
#include "../../../../System/VmxDirtyLogProjectionTypes.h"
#include "../../../../System/VmxDebugTracePlane.h"
#include "../../../../System/Vmcs/V2/VmcsV2HostEvidenceKind.h"
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;
using YAKSys_Hybrid_CPU::Memory::TranslationViolationInfo;

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	enum class VmcsV2GprPersistenceKind : unsigned char
	{
		None = 0,
		LazyPending = 1,
		Complete = 2
	};

	class VmxRootControlBlock final : public std::enable_shared_from_this<VmxRootControlBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getRootDescriptorAddress() const;

		unsigned long long getOwnershipEpoch() const;
	};

	class VmxNptBlock final : public std::enable_shared_from_this<VmxNptBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		std::shared_ptr<MemoryTranslationControl> getLastControl() const;

		unsigned long long getControlEpoch() const;
	};

	class ExitInfoBlock final : public std::enable_shared_from_this<ExitInfoBlock>
	{
	private:
		VmExitReason ExitReason = static_cast<VmExitReason>(0);
		unsigned long long ExitQualification = 0;
		unsigned long long GuestPhysicalAddress = 0;
		unsigned long long EptViolationQualification = 0;
		std::shared_ptr<TranslationViolationInfo> NptViolation;

	public:
		VmExitReason getExitReason() const;
	private:
		void setExitReason(VmExitReason value);

	public:
		unsigned long long getExitQualification() const;
	private:
		void setExitQualification(unsigned long long value);

	public:
		unsigned long long getGuestPhysicalAddress() const;
	private:
		void setGuestPhysicalAddress(unsigned long long value);

	public:
		unsigned long long getEptViolationQualification() const;
	private:
		void setEptViolationQualification(unsigned long long value);

	public:
		std::shared_ptr<TranslationViolationInfo> getNptViolation() const;
	private:
		void setNptViolation(std::shared_ptr<TranslationViolationInfo> value);

	public:
		void RecordVectorException(std::shared_ptr<VectorStreamExceptionInfo> info);

		void RecordStreamDescriptorFault(std::shared_ptr<VectorStreamDescriptorFaultInfo> info);

		void RecordStreamReplayRequired(unsigned short ownerVirtualThreadId, unsigned short addressSpaceTag, unsigned long long streamReplayEpoch);

	};

	class VirtualCpuBlock final : public std::enable_shared_from_this<VirtualCpuBlock>
	{
	private:
		unsigned long long GuestPc = 0;
		unsigned long long GuestSp = 0;
		VmcsV2GprPersistenceKind GprPersistence = static_cast<VmcsV2GprPersistenceKind>(0);

	public:
		static constexpr int IntegerRegisterCount = 32;

	private:
		const std::vector<unsigned long long> _guestIntegerRegisters = std::vector<unsigned long long>(IntegerRegisterCount);

	public:
		unsigned long long getGuestPc() const;
	private:
		void setGuestPc(unsigned long long value);

	public:
		unsigned long long getGuestSp() const;
	private:
		void setGuestSp(unsigned long long value);

	public:
		VmcsV2GprPersistenceKind getGprPersistence() const;
	private:
		void setGprPersistence(VmcsV2GprPersistenceKind value);

	public:
		const std::vector<unsigned long long> getGuestIntegerRegisters() const;

	};

	class BundleExecutionBlock final : public std::enable_shared_from_this<BundleExecutionBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getBundlePc() const;

		unsigned long long getExecutionEpoch() const;
	};

	class VmxPreemptionTimerBlock final : public std::enable_shared_from_this<VmxPreemptionTimerBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getTimerEpoch() const;
	};

	class VirtualInterruptFabricBlock final : public std::enable_shared_from_this<VirtualInterruptFabricBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getRoutingEpoch() const;
	};

	class InterceptBitmapBlock final : public std::enable_shared_from_this<InterceptBitmapBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getPolicyEpoch() const;
	};

	class EventInjectionBlock final : public std::enable_shared_from_this<EventInjectionBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getInjectionEpoch() const;

		unsigned long long getDeliveryEpoch() const;

		std::shared_ptr<EventInjectionDescriptor> getLastDelivered() const;

		int getPendingCount() const;

		unsigned long long getDroppedCount() const;

		unsigned long long getInvalidDroppedCount() const;

		unsigned long long getCapacityDroppedCount() const;

		unsigned long long getRemapDroppedCount() const;

		unsigned long long getRemappedCount() const;

		unsigned long long getCoalescedCount() const;

		unsigned long long getRemapPolicyEpoch() const;
	};

	class LaneCompletionRoutingBlock final : public std::enable_shared_from_this<LaneCompletionRoutingBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getRoutingEpoch() const;

		unsigned long long getRoutedCount() const;

		unsigned long long getIgnoredDisabledCount() const;

		unsigned long long getInvalidDroppedCount() const;

		unsigned long long getCapacityDroppedCount() const;

		unsigned long long getEventQueueDroppedCount() const;
	};

	class VectorStreamStateBlock final : public std::enable_shared_from_this<VectorStreamStateBlock>
	{
	public:
		static constexpr unsigned int DefaultMaxStreamLength = 32;

		bool isReadOnlyCompatibilityProjection() const;

		bool getVirtualizationEnabled() const;

		VectorStreamSaveMask getSaveRestoreMask() const;

		VectorExceptionAction getExceptionPolicy() const;

		unsigned long long getStreamDescriptorTableBase() const;

		unsigned long long getStreamDescriptorTableLimit() const;

		unsigned int getMaxStreamLength() const;

		unsigned long long getStreamEpoch() const;

		unsigned long long getPolicyEpoch() const;

		unsigned long long getArchitecturalEpoch() const;

		unsigned long long getDirtyEpoch() const;

		unsigned long long getStreamReplayEpoch() const;

		unsigned long long getStreamQueueEpoch() const;

		unsigned long long getStreamCompletionEpoch() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public VectorStreamSnapshot? LastSnapshot
		std::shared_ptr<VectorStreamSnapshot> getLastSnapshot() const;

		std::shared_ptr<VectorStreamExceptionInfo> getLastVectorException() const;

		std::shared_ptr<VectorStreamDescriptorFaultInfo> getLastDescriptorFault() const;

		bool getHasDescriptorTable() const;

		bool getRequiresMigratableVectorState() const;

		bool isMigrationReady() const;
	};

	class SecurityIsolationBlock final : public std::enable_shared_from_this<SecurityIsolationBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getIsolationEpoch() const;
	};

	class CapabilityNegotiationBlock final : public std::enable_shared_from_this<CapabilityNegotiationBlock>
	{
	public:
		bool isReadOnlyCompatibilityProjection() const;

		unsigned long long getCapabilityEpoch() const;
	};

	class DirtyLogBlock final : public std::enable_shared_from_this<DirtyLogBlock>
	{
	public:
		unsigned short getVersion() const;

		bool isReadOnlyCompatibilityProjection() const;

		bool getEnabled() const;

		unsigned long long getGuestPhysicalBase() const;

		unsigned long long getGuestPhysicalLimitExclusive() const;

		unsigned int getPageSize() const;

		unsigned int getMaxDirtyPages() const;

		VmxDirtyLogOverflowPolicy getOverflowPolicy() const;

		unsigned long long getGeneration() const;

		unsigned long long getWriteSequence() const;

		bool getOverflowed() const;

		unsigned long long getCpuWriteCount() const;

		unsigned long long getAtomicWriteCount() const;

		unsigned long long getDmaWriteCount() const;

		unsigned long long getLane6WriteCount() const;

		unsigned long long getLane7WriteCount() const;

		unsigned long long getNptWriteProtectCount() const;

		unsigned long long getDroppedWriteCount() const;

		bool ContainsHostEvidence(VmcsV2HostEvidenceKind evidence);

		std::shared_ptr<VmxDirtyLogStatus> SnapshotStatus();
	};

	class DebugTraceBlock final : public std::enable_shared_from_this<DebugTraceBlock>
	{
	public:
		unsigned short getVersion() const;

		bool isReadOnlyCompatibilityProjection() const;

		bool getExportEnabled() const;

		unsigned long long getPolicyEpoch() const;

		std::shared_ptr<VmxDebugTraceCounters> getCounters() const;

		bool ContainsHostEvidence(VmcsV2HostEvidenceKind evidence);
	};

}
