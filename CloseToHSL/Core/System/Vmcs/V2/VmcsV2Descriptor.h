#pragma once

#include "../../../Virtualization/Compatibility/Generated/VmcsProjection/VmcsV2Header.h"
#include "VmcsV2BlockDirectory.h"
#include "../../../Virtualization/Compatibility/Generated/VmcsProjection/VmcsV2Blocks.h"
#include "../../../Runtime/Domains/Descriptors/IoDomain/IoVirtualizationBlock.h"
#include "../../../Runtime/Lanes/Lane6/Lane6StateBlock.h"
#include "../../../Runtime/Lanes/Lane7/YAKSys_Hybrid_CPU.Core.Lane7StateBlock.h"
#include <string>
#include <stdexcept>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Vmcs::V2
{

	class VmcsV2Descriptor final : public std::enable_shared_from_this<VmcsV2Descriptor>
	{
	private:
		const std::shared_ptr<VmcsV2Header> Header;
		const std::shared_ptr<VmcsV2BlockDirectory> Directory;
		const std::shared_ptr<VmxRootControlBlock> RootControl = std::make_shared<VmxRootControlBlock>();
		const std::shared_ptr<VirtualCpuBlock> VirtualCpu = std::make_shared<VirtualCpuBlock>();
		const std::shared_ptr<BundleExecutionBlock> BundleExecution = std::make_shared<BundleExecutionBlock>();
		const std::shared_ptr<VmxPreemptionTimerBlock> PreemptionTimer = std::make_shared<VmxPreemptionTimerBlock>();
		const std::shared_ptr<VirtualInterruptFabricBlock> InterruptFabric = std::make_shared<VirtualInterruptFabricBlock>();
		const std::shared_ptr<VectorStreamStateBlock> VectorStreamState = std::make_shared<VectorStreamStateBlock>();
		const std::shared_ptr<SecurityIsolationBlock> SecurityIsolation = std::make_shared<SecurityIsolationBlock>();
		const std::shared_ptr<CapabilityNegotiationBlock> CapabilityNegotiation = std::make_shared<CapabilityNegotiationBlock>();
		const std::shared_ptr<VmxNptBlock> NptBlock = std::make_shared<VmxNptBlock>();
		const std::shared_ptr<ExitInfoBlock> ExitInfo = std::make_shared<ExitInfoBlock>();
		const std::shared_ptr<InterceptBitmapBlock> InterceptBitmap = std::make_shared<InterceptBitmapBlock>();
		const std::shared_ptr<EventInjectionBlock> EventInjection = std::make_shared<EventInjectionBlock>();
		const std::shared_ptr<LaneCompletionRoutingBlock> LaneCompletionRouting = std::make_shared<LaneCompletionRoutingBlock>();
		const std::shared_ptr<IoVirtualizationBlock> IoVirt = std::make_shared<IoVirtualizationBlock>();
		const std::shared_ptr<Lane6StateBlock> Lane6State = std::make_shared<Lane6StateBlock>();
		const std::shared_ptr<Lane7StateBlock> Lane7State = std::make_shared<Lane7StateBlock>();
		const std::shared_ptr<DirtyLogBlock> DirtyLog = std::make_shared<DirtyLogBlock>();
		const std::shared_ptr<DebugTraceBlock> DebugTrace = std::make_shared<DebugTraceBlock>();

	public:
		VmcsV2Descriptor(std::shared_ptr<VmcsV2Header> header, std::shared_ptr<VmcsV2BlockDirectory> directory);

		std::shared_ptr<VmcsV2Header> getHeader() const;

		std::shared_ptr<VmcsV2BlockDirectory> getDirectory() const;

		std::shared_ptr<VmxRootControlBlock> getRootControl() const;

		std::shared_ptr<VirtualCpuBlock> getVirtualCpu() const;

		std::shared_ptr<BundleExecutionBlock> getBundleExecution() const;

		std::shared_ptr<VmxPreemptionTimerBlock> getPreemptionTimer() const;

		std::shared_ptr<VirtualInterruptFabricBlock> getInterruptFabric() const;

		std::shared_ptr<VectorStreamStateBlock> getVectorStreamState() const;

		std::shared_ptr<SecurityIsolationBlock> getSecurityIsolation() const;

		std::shared_ptr<CapabilityNegotiationBlock> getCapabilityNegotiation() const;

		std::shared_ptr<VmxNptBlock> getNptBlock() const;

		std::shared_ptr<ExitInfoBlock> getExitInfo() const;

		std::shared_ptr<InterceptBitmapBlock> getInterceptBitmap() const;

		std::shared_ptr<EventInjectionBlock> getEventInjection() const;

		std::shared_ptr<LaneCompletionRoutingBlock> getLaneCompletionRouting() const;

		std::shared_ptr<IoVirtualizationBlock> getIoVirt() const;

		std::shared_ptr<Lane6StateBlock> getLane6State() const;

		std::shared_ptr<Lane7StateBlock> getLane7State() const;

		std::shared_ptr<DirtyLogBlock> getDirtyLog() const;

		std::shared_ptr<DebugTraceBlock> getDebugTrace() const;

		static std::shared_ptr<VmcsV2Descriptor> CreateDefault();

		bool TryReadScalarField(unsigned short fieldId, long long &value, std::shared_ptr<VmcsV2ValidationResult> &validation);

		std::shared_ptr<VmcsV2ValidationResult> ValidateMigrationReadiness();

		std::shared_ptr<VmcsV2ValidationResult> ValidateNestedEnablementReadiness();

		void RecordVectorExceptionExit(std::shared_ptr<VectorStreamExceptionInfo> info);

		void RecordStreamDescriptorFaultExit(std::shared_ptr<VectorStreamDescriptorFaultInfo> info);

		void RecordStreamReplayRequiredExit(unsigned short ownerVirtualThreadId, unsigned short addressSpaceTag, unsigned long long streamReplayEpoch);

	private:
		static std::shared_ptr<VmcsV2ValidationResult> UnknownField(unsigned short fieldId);

		static std::shared_ptr<VmcsV2ValidationResult> AccessDenied(std::shared_ptr<VmcsV2FieldDescriptor> descriptor, const std::wstring &message);
	};

}
