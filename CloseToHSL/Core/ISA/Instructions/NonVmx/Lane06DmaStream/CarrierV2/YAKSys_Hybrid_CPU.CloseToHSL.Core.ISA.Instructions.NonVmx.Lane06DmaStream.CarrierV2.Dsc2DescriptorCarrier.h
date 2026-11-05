#pragma once

#include <string>
#include <memory>

// DSC2 metadata anchor: descriptor-v2 carrier remains parser-only until a
// descriptor-v2 ADR closes parser, runtime admission, retire, replay, and ABI evidence.
// It is Lane6 descriptor/carrier-owned and does not become a scalar opcode or VMX path.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane06DmaStream::CarrierV2
{
	class Dsc2DescriptorCarrier final : public std::enable_shared_from_this<Dsc2DescriptorCarrier>
	{
	public:
		static inline const std::wstring Mnemonic = L"DSC2";
		static inline const std::wstring OperandShape = L"DescriptorV2CarrierSideband, no architectural scalar operands";
		static inline const std::wstring ParameterDescriptor = L"Descriptor-v2 header, extension table, owner/domain, shape/type policy sideband";
		static inline const std::wstring MicroOpShape = L"ParserOnlyLane6DescriptorCarrierV2, no runtime MicroOp until descriptor-v2 ADR closes";
		static inline const std::wstring ExecutionLaneBinding = L"Lane06DmaStream";
		static inline const std::wstring EvidenceBoundary = L"ParserOnlyCarrierNoExecution";
		static inline const std::wstring CarrierAuthorityBoundary = L"ParserOnlyLane6DescriptorV2Carrier";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";
		static inline const std::wstring ProductionDecision = L"Phase11NegativeDecisionGate";

		static constexpr bool IsDescriptorOwned = true;
		static constexpr bool IsCarrierOnly = true;
		static constexpr bool IsParserOnly = true;
		static constexpr bool RequiresDescriptorV2Adr = true;
		static constexpr bool RequiresDescriptorV2ParserManifest = true;
		static constexpr bool RequiresBackwardCompatibleDecoder = true;
		static constexpr bool RequiresDescriptorV2ExecutionPolicy = true;
		static constexpr bool RequiresDescriptorV2AdmissionPolicy = true;
		static constexpr bool RequiresRuntimeAdmission = true;
		static constexpr bool RequiresRetireCommitAuthority = true;
		static constexpr bool RequiresDescriptorV2RetireReplayPolicy = true;
		static constexpr bool RequiresReplayDeterminism = true;
		static constexpr bool RequiresParserOnlyConformance = true;
		static constexpr bool RequiresDescriptorV2GoldenArtifacts = true;
		static constexpr bool NoDsc2ExecutionBeforeAdr = true;
		static constexpr bool NoDescriptorV2ExecutionBeforeAdr = true;
		static constexpr bool ParserAcceptanceIsNotExecutionEvidence = true;
		static constexpr bool NoGuestVisibleHostEvidence = true;
		static constexpr bool NoHostEvidenceLeak = true;
		static constexpr bool NoHostOwnedEvidencePublication = true;
		static constexpr bool ExistingDmaStreamComputeEvidenceIsInsufficient = true;
		static constexpr bool ExistingDscStatusEvidenceIsInsufficient = true;
		static constexpr bool ExistingDscQueryCapsEvidenceIsInsufficient = true;
		static constexpr bool Phase10DescriptorOpEvidenceIsInsufficient = true;
		static constexpr bool NoScalarOpcodePublication = true;
		static constexpr bool NoExecutableDecoderEncoderAbiPublication = true;
		static constexpr bool NoInstructionIrProjectionPublication = true;
		static constexpr bool NoRegistryMaterializerPublication = true;
		static constexpr bool NoTypedMicroOpPublication = true;
		static constexpr bool NoSchedulerLaneBindingPublication = true;
		static constexpr bool NoRuntimeAdmissionPublication = true;
		static constexpr bool NoExecutionCapturePublication = true;
		static constexpr bool NoRetireCommitPublication = true;
		static constexpr bool NoReplayRollbackPublication = true;
		static constexpr bool NoCompilerHelperEmission = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoHiddenVectorLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoParserToExecutionPromotion = true;
		static constexpr bool NoDmaStreamComputeFallback = true;
		static constexpr bool NoDscStatusFallback = true;
		static constexpr bool NoDscQueryCapsFallback = true;
		static constexpr bool NoQueueRuntimeFallback = true;
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
