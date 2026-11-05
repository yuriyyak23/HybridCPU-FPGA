#pragma once

#include <string>
#include <memory>

// Iteration 14A materialized Lane7 counter/hint metadata into per-instruction leaf partial files.

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane07SystemControl::CacheMaintenance
{
	class IcacheInvalInstruction final : public std::enable_shared_from_this<IcacheInvalInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"ICACHE_INVAL";
		static inline const std::wstring EvidenceBoundary = L"Lane7CacheMaintenanceDeferred";
		static constexpr bool RequiresRetireOwnedPublication = true;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;




		static inline const std::wstring ProductionDecision = L"Phase13NegativeDecisionGate";
		static inline const std::wstring MaintenanceAuthorityBoundary = L"Lane7CacheMaintenanceProductionPathOnly";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";

		static constexpr bool RequiresCacheMaintenanceAbi = true;
		static constexpr bool RequiresInstructionFetchCoherencyModel = true;
		static constexpr bool RequiresCacheHierarchyAuthorityModel = true;
		static constexpr bool RequiresAddressRangeScopeAbi = true;
		static constexpr bool RequiresPrivilegeAndAdmissionPolicy = true;
		static constexpr bool RequiresRetireOwnedSideEffectPublication = true;
		static constexpr bool RequiresDecoderEncoderAbiPublication = true;
		static constexpr bool RequiresInstructionIrProjectionPublication = true;
		static constexpr bool RequiresLane7MaterializerPublication = true;
		static constexpr bool RequiresTypedMaintenanceMicroOpPublication = true;
		static constexpr bool RequiresConformanceAndGoldenArtifacts = true;
		static constexpr bool NoGenericFenceFallback = true;
		static constexpr bool NoFenceIFallback = true;
		static constexpr bool NoGuestVisibleHostEvidence = true;
		static constexpr bool NoHostEvidenceLeak = true;
		static constexpr bool NoHostOwnedEvidencePublication = true;
		static constexpr bool NoHiddenScalarLowering = true;
		static constexpr bool NoMultiOpEmission = true;
		static constexpr bool NoLane6DmaFallback = true;
		static constexpr bool NoLane7AcceleratorFallback = true;
		static constexpr bool NoExternalBackendFallback = true;
		static constexpr bool NoVmxSpecificPath = true;
		static constexpr bool ExistingFenceEvidenceIsInsufficient = true;
		static constexpr bool ExistingFenceIEvidenceIsInsufficient = true;
		static constexpr bool ExistingAtomicOrderingEvidenceIsInsufficient = true;
		static constexpr bool ExistingLane7ControlPlaneEvidenceIsInsufficient = true;


		static constexpr bool RequiresReplayStableInvalidationModel = true;
		static constexpr bool RequiresRollbackConformance = true;
		static constexpr bool RequiresInvalidationOrderingModel = true;
		static constexpr bool NoSpeculativeMaintenancePublication = true;
		static constexpr bool NoRetireSideEffectBeforeAuthority = true;


		static constexpr bool RequiresFutureVirtualizationBoundaryPolicy = true;
		static constexpr bool RequiresImmediateVmxProjection = false;
		static constexpr bool VmxCacheEvidenceIsInsufficient = true;
		static constexpr bool VmxMigrationCheckpointEvidenceIsInsufficient = true;
	};
}
