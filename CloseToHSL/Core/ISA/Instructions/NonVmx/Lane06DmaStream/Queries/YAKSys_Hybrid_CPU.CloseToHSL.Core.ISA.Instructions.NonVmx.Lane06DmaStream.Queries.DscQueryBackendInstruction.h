#pragma once

#include <string>
#include <memory>

// DSC_QUERY_BACKEND metadata anchor: Lane6 backend capability query remains
// deferred until capability ABI, bounded publication, and replay evidence close.
// It must never publish hidden host-owned evidence as guest architectural state.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane06DmaStream::Queries
{
	class DscQueryBackendInstruction final : public std::enable_shared_from_this<DscQueryBackendInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"DSC_QUERY_BACKEND";
		static inline const std::wstring OperandShape = L"Capability query sideband, scalar/status destination sideband";
		static inline const std::wstring ParameterDescriptor = L"Query selector, optional queue/device scope, retire-visible bounded capability result";
		static inline const std::wstring MicroOpShape = L"Lane6CapabilityQuery.Backend, read-only capability query, retire-owned scalar publication";
		static inline const std::wstring ExecutionLaneBinding = L"Lane06DmaStream";
		static inline const std::wstring EvidenceBoundary = L"Lane6CapabilityQueryNoExecution";
		static inline const std::wstring QueryAuthorityBoundary = L"GenericLane6CapabilityQueryRuntimeOnly";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static inline const std::wstring ProductionDecision = L"Phase11NegativeDecisionGate";

		static constexpr bool IsCapabilityQuery = true;
		static constexpr bool IsReadOnlyQuery = true;
		static constexpr bool RequiresCapabilityQueryAbi = true;
		static constexpr bool RequiresBackendCapabilityAbi = true;
		static constexpr bool RequiresQuerySelectorAbi = true;
		static constexpr bool RequiresCapabilityResultAbi = true;
		static constexpr bool RequiresResultScrubbingPolicy = true;
		static constexpr bool RequiresQueryRuntimeAdmission = true;
		static constexpr bool RequiresTypedQueryMicroOp = true;
		static constexpr bool RequiresBoundedResultFootprint = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresSchedulerLaneBinding = true;
		static constexpr bool RequiresRetireOwnedPublication = true;
		static constexpr bool RequiresFutureRetireReplayEvidence = true;
		static constexpr bool RequiresReplayStableResult = true;
		static constexpr bool RequiresReplayRollbackConformance = true;
		static constexpr bool RequiresGoldenArtifacts = true;
		static constexpr bool NoHostEvidenceLeak = true;
		static constexpr bool NoGuestVisibleHostEvidence = true;
		static constexpr bool NoHostOwnedEvidencePublication = true;
		static constexpr bool ExistingDscQueryCapsEvidenceIsInsufficient = true;
		static constexpr bool ExistingDscStatusEvidenceIsInsufficient = true;
		static constexpr bool Dsc2ParserEvidenceIsInsufficient = true;
		static constexpr bool NoScalarOpcodePublication = true;
		static constexpr bool NoDecoderEncoderAbiPublication = true;
		static constexpr bool NoInstructionIrProjectionPublication = true;
		static constexpr bool NoRegistryMaterializerPublication = true;
		static constexpr bool NoTypedMicroOpPublication = true;
		static constexpr bool NoSchedulerLaneBindingPublication = true;
		static constexpr bool NoExecutionCapturePublication = true;
		static constexpr bool NoRetirePublicationBeforeQueryAuthority = true;
		static constexpr bool NoReplayRollbackPublication = true;
		static constexpr bool NoCompilerHelperEmission = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoHiddenVectorLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoDmaStreamComputeFallback = true;
		static constexpr bool NoDscQueryCapsFallback = true;
		static constexpr bool NoDscStatusFallback = true;
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
