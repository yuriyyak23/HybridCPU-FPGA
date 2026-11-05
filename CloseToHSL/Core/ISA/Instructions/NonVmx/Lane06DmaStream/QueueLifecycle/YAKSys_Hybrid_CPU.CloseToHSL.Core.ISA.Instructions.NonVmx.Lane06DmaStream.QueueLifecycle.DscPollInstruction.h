#pragma once

#include <string>
#include <memory>

// DSC_POLL metadata anchor: Lane6 queue observation remains deferred until
// token/queue authority, decoder ABI, retire publication, and replay evidence close.
// No VMX frontend path is added; future execution requires virtualization-boundary policy.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane06DmaStream::QueueLifecycle
{
	class DscPollInstruction final : public std::enable_shared_from_this<DscPollInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"DSC_POLL";
		static inline const std::wstring OperandShape = L"QueueTokenOrHandle, status destination sideband";
		static inline const std::wstring ParameterDescriptor = L"TokenOrQueueHandle, optional queue scope, retire-visible status result";
		static inline const std::wstring MicroOpShape = L"Lane6QueueControl.Poll, queue-authority gated, retire-owned status publication";
		static inline const std::wstring ExecutionLaneBinding = L"Lane06DmaStream";
		static inline const std::wstring EvidenceBoundary = L"Lane6QueueControlNoExecution";
		static inline const std::wstring QueueAuthorityBoundary = L"GenericLane6QueueRuntimeOnly";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static inline const std::wstring ProductionDecision = L"Phase11NegativeDecisionGate";

		static constexpr bool IsQueueControlOwned = true;
		static constexpr bool RequiresQueueAuthority = true;
		static constexpr bool RequiresTokenNamespaceAbi = true;
		static constexpr bool RequiresQueueHandleAbi = true;
		static constexpr bool RequiresTokenLifecycleAbi = true;
		static constexpr bool RequiresQueueOwnershipModel = true;
		static constexpr bool RequiresQueueStateModel = true;
		static constexpr bool RequiresQueueRollbackJournal = true;
		static constexpr bool RequiresQueueRuntimeAdmission = true;
		static constexpr bool RequiresTypedQueueMicroOp = true;
		static constexpr bool RequiresQueueCommandEncoding = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresSchedulerLaneBinding = true;
		static constexpr bool RequiresRetireOwnedPublication = true;
		static constexpr bool RequiresFutureRetireReplayEvidence = true;
		static constexpr bool RequiresReplayDeterminism = true;
		static constexpr bool RequiresRollbackPolicy = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoGuestVisibleHostEvidence = true;
		static constexpr bool NoHostOwnedEvidencePublication = true;
		static constexpr bool ExistingDscStatusEvidenceIsInsufficient = true;
		static constexpr bool ExistingDscQueryCapsEvidenceIsInsufficient = true;
		static constexpr bool Dsc2ParserEvidenceIsInsufficient = true;
		static constexpr bool NoScalarOpcodePublication = true;
		static constexpr bool NoDecoderEncoderAbiPublication = true;
		static constexpr bool NoInstructionIrProjectionPublication = true;
		static constexpr bool NoRegistryMaterializerPublication = true;
		static constexpr bool NoTypedMicroOpPublication = true;
		static constexpr bool NoSchedulerLaneBindingPublication = true;
		static constexpr bool NoExecutionCapturePublication = true;
		static constexpr bool NoRetirePublicationBeforeQueueAuthority = true;
		static constexpr bool NoReplayRollbackPublication = true;
		static constexpr bool NoCompilerHelperEmission = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoHiddenVectorLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoDmaStreamComputeFallback = true;
		static constexpr bool NoDscStatusFallback = true;
		static constexpr bool NoDscQueryCapsFallback = true;
		static constexpr bool NoDsc2Fallback = true;
		static constexpr bool NoLane7Fallback = true;
		static constexpr bool NoExternalBackendFallback = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool NoVmxFrontendIntegrationRequired = true;
		static constexpr bool RequiresImmediateVmxProjection = false;
		static constexpr bool RequiresFutureVirtualizationBoundaryPolicy = true;
		static constexpr bool HasScalarOpcodeAllocation = false;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
