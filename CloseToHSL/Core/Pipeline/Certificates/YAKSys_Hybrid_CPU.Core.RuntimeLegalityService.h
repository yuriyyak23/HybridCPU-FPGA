#pragma once

#include "ReplayPhaseSubstrate.Interfaces.h"
#include "../Safety/SafetyVerifier.Types.h"
#include "../../Runtime/Events/Hypercalls/VirtualizationOperandSnapshot.h"
#include "../MicroOps/Types/MicroOp.h"
#include "BundleResourceCertificate4Way.h"
#include "../Scheduling/SlotLegality/SlotClassDefinitions.h"
#include "BundleResourceCertificate.h"
#include "ReplayPhaseSubstrate.h"
#include "SafetyMask128.h"
#include "ReplayPhaseSubstrate.Implementations.h"
#include "YAKSys_Hybrid_CPU.Core.BundleResourceCertificate4Way.h"
#include "../Safety/YAKSys_Hybrid_CPU.Core.SafetyVerifier.h"
namespace YAKSys_Hybrid_CPU::Core { class VmxMicroOp; }
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;


namespace YAKSys_Hybrid_CPU::Core
{
	/// </summary>

















	/// <summary>
	/// Internal runtime-local legality service provisioning.
	/// Keeps scheduler construction on a single inter-core/SMT legality service seam
	/// without exposing checker/cache wiring as a public runtime contract.
	/// </summary>



	/// <summary>
	/// Internal runtime legality service for inter-core + SMT hot paths plus adjacent diagnostics.
	/// </summary>
	class RuntimeLegalityService final : public IRuntimeLegalityService, public IVirtualizationAdmissionService
	{
	private:
		const std::shared_ptr<ILegalityChecker> _legalityChecker;
		const std::shared_ptr<ILegalityCertificateCacheCoordinator> _cacheCoordinator;
		const std::shared_ptr<VirtualizationOperandSnapshotMaterializer> _virtualizationOperandMaterializer = std::make_shared<VirtualizationOperandSnapshotMaterializer>();

	public:
		bool isKernelDomainIsolationEnabled() const override;

		RuntimeLegalityService(std::shared_ptr<ILegalityChecker> legalityChecker, std::shared_ptr<ILegalityCertificateCacheCoordinator> cacheCoordinator);

		inline InterCoreDomainGuardDecision EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag) override;

		inline TypedSlotRejectClassification ClassifyReject(TypedSlotRejectReason admissionReject, CertificateRejectDetail certDetail, SlotClass candidateClass, SlotPinningKind pinningKind) override;

		inline void PrepareInterCore(ReplayPhaseContext phase, std::shared_ptr<BundleResourceCertificate> certificate) override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public LegalityDecision EvaluateInterCoreLegality(ReplayPhaseContext phase, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
		inline LegalityDecision EvaluateInterCoreLegality(ReplayPhaseContext phase, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128()) override;

		inline void RefreshInterCoreAfterMutation(ReplayPhaseContext phase, std::shared_ptr<BundleResourceCertificate> certificate) override;

		inline void PrepareSmt(ReplayPhaseContext phase, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) override;

		inline LegalityDecision EvaluateSmtBoundaryGuard(ReplayPhaseContext phase, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) override;

		inline LegalityDecision EvaluateSmtLegality(ReplayPhaseContext phase, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, std::shared_ptr<MicroOp> candidate) override;

		inline std::shared_ptr<VirtualizationAdmissionIssueResult> IssueVirtualizationAdmissionAfterStageB(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane) override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public VirtualizationAdmissionValidationResult ValidateVirtualizationAdmission(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, MicroOp candidate, int sourceSlotId, int selectedLane, SafetyVerifier.VirtualizationAdmissionCertificate? certificate)
		inline std::shared_ptr<VirtualizationAdmissionValidationResult> ValidateVirtualizationAdmission(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane, std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> certificate) override;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public VirtualizationOperandCaptureResult CaptureVirtualizationOperandsAfterE1Validation(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, int sourceSlotId, int selectedLane, SafetyVerifier.VirtualizationAdmissionCertificate? certificate, ulong rs1Value, ulong restoreGeneration)
		inline std::shared_ptr<VirtualizationOperandCaptureResult> CaptureVirtualizationOperandsAfterE1Validation(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, int sourceSlotId, int selectedLane, std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> certificate, unsigned long long rs1Value, unsigned long long restoreGeneration) override;

		inline void RefreshSmtAfterMutation(ReplayPhaseContext phase, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) override;

		inline void InvalidatePhaseMismatch(ReplayPhaseContext phase) override;

		inline void Invalidate(ReplayPhaseInvalidationReason reason, bool invalidateInterCore = true, bool invalidateFourWay = true) override;


// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SafetyVerifier? ResearchVirtualizationCanonicalVerifier
		std::shared_ptr<SafetyVerifier> getResearchVirtualizationCanonicalVerifier() const;


// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal SafetyVerifier? CanonicalVirtualizationVerifier
		std::shared_ptr<SafetyVerifier> getCanonicalVirtualizationVerifier() const;
	};
}
