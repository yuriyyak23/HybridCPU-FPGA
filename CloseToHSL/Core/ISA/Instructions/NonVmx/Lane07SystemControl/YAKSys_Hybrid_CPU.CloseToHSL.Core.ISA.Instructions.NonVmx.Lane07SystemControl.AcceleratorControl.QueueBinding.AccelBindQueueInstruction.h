#pragma once

#include <string>
#include <memory>

// Iteration 14A materialized Lane7 counter/hint metadata into per-instruction leaf partial files.

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane07SystemControl::AcceleratorControl::QueueBinding
{
	class AccelBindQueueInstruction final : public std::enable_shared_from_this<AccelBindQueueInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"ACCEL_BIND_QUEUE";
		static inline const std::wstring EvidenceBoundary = L"Lane7AcceleratorControlDeferred";
		static constexpr bool RequiresRetireOwnedPublication = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;


		static inline const std::wstring ProductionDecision = L"Phase14NegativeDecisionGate";
		static inline const std::wstring AcceleratorAuthorityBoundary = L"Lane7AcceleratorQueueBindingProductionPathOnly";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";

		static constexpr bool IsAcceleratorQueueBindingControl = true;
		static constexpr bool RequiresAcceleratorRuntimeAuthority = true;
		static constexpr bool RequiresQueueAuthority = true;
		static constexpr bool RequiresTokenAuthority = true;
		static constexpr bool RequiresLane6TokenAuthorityGate = true;
		static constexpr bool RequiresOwnerDomainGuard = true;
		static constexpr bool RequiresBindUnbindQueueAbi = true;
		static constexpr bool RequiresQueueOwnershipModel = true;
		static constexpr bool RequiresCommandQueueSemantics = true;
		static constexpr bool RequiresNoHostEvidenceLeak = true;
		static constexpr bool NoHostEvidenceLeak = true;
		static constexpr bool RequiresDecoderEncoderAbiPublication = true;
		static constexpr bool RequiresInstructionIrProjectionPublication = true;
		static constexpr bool RequiresLane7MaterializerPublication = true;
		static constexpr bool RequiresTypedAcceleratorControlMicroOpPublication = true;
		static constexpr bool RequiresConformanceAndGoldenArtifacts = true;
		static constexpr bool NoHostOwnedEvidencePublication = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoGenericSystemOpFallback = true;
		static constexpr bool NoLane6DmaFallback = true;
		static constexpr bool NoLane7SubmitFallback = true;
		static constexpr bool NoExternalBackendFallback = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool ExistingAccelSubmitEvidenceIsInsufficient = true;
		static constexpr bool ExistingAccelQueryCapsEvidenceIsInsufficient = true;
		static constexpr bool ExistingTopologyQueueTaxonomyEvidenceIsInsufficient = true;
		static constexpr bool ExistingLane6DmaEvidenceIsInsufficient = true;
		static constexpr bool ExistingLane7ControlPlaneEvidenceIsInsufficient = true;




		static constexpr bool RequiresRetireOwnedSideEffectPublication = true;
		static constexpr bool RequiresReplayStableQueueBindingModel = true;
		static constexpr bool RequiresRollbackConformance = true;
		static constexpr bool RequiresQueueBindUnbindOrderingModel = true;
		static constexpr bool RequiresMigrationCheckpointPolicy = true;
		static constexpr bool RequiresFutureVirtualizationBoundaryPolicy = true;
		static constexpr bool RequiresImmediateVmxProjection = false;
		static constexpr bool VmxBackendAuthorityEvidenceIsInsufficient = true;
		static constexpr bool VmxMigrationCheckpointEvidenceIsInsufficient = true;
		static constexpr bool NoQueueBindingPublicationBeforeRetire = true;
		static constexpr bool NoQueueBindingBeforeTokenAuthority = true;
	};
}
