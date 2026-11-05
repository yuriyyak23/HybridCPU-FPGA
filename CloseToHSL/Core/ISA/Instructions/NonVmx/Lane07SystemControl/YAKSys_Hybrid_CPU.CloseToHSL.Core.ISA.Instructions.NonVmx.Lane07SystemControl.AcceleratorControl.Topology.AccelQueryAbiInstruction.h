#pragma once

#include <string>
#include <memory>

// Iteration 14A materialized Lane7 counter/hint metadata into per-instruction leaf partial files.

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane07SystemControl::AcceleratorControl::Topology
{
	class AccelQueryAbiInstruction final : public std::enable_shared_from_this<AccelQueryAbiInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"ACCEL_QUERY_ABI";
		static inline const std::wstring EvidenceBoundary = L"Lane7AcceleratorControlDeferred";
		static constexpr bool NoHostEvidenceLeak = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;


		static constexpr bool RequiresBackendCapabilityAuthority = true;
		static constexpr bool RequiresGuestVisibleCapabilityPolicy = true;
		static constexpr bool RequiresFutureVirtualizationBoundaryPolicy = true;
		static constexpr bool RequiresImmediateVmxProjection = false;
		static constexpr bool VmxCapabilityEvidenceIsInsufficient = true;
		static constexpr bool VmxMigrationCheckpointEvidenceIsInsufficient = true;
		static constexpr bool NoCapabilityPublicationBeforeAuthority = true;


		static inline const std::wstring ProductionDecision = L"Phase14NegativeDecisionGate";
		static inline const std::wstring CapabilityAuthorityBoundary = L"Lane7AcceleratorCapabilityProductionPathOnly";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";

		static constexpr bool IsAcceleratorCapabilityQuery = true;
		static constexpr bool RequiresCapabilityAuthority = true;
		static constexpr bool RequiresAcceleratorAbiQueryContract = true;
		static constexpr bool RequiresBoundedCapabilityResultFootprint = true;
		static constexpr bool RequiresResultScrubbingPolicy = true;
		static constexpr bool RequiresOwnerDomainGuard = true;
		static constexpr bool RequiresCommandQueueSemantics = true;
		static constexpr bool RequiresNoHostEvidenceLeak = true;
		static constexpr bool RequiresRetireOwnedPublication = true;
		static constexpr bool RequiresReplayStableCapabilityModel = true;
		static constexpr bool RequiresMigrationCheckpointPolicy = true;
		static constexpr bool RequiresVirtualizationBoundaryPolicy = true;
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
		static constexpr bool ExistingLane7ControlPlaneEvidenceIsInsufficient = true;


	};
}
