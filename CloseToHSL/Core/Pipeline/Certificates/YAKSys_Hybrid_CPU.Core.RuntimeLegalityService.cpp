#include "YAKSys_Hybrid_CPU.Core.RuntimeLegalityService.h"
#include "../MicroOps/Types/MicroOp.IO.h"
#include "../../Runtime/Events/Hypercalls/Governance/VirtualizationOperationOwnerSnapshot.h"
#include "../Safety/SafetyVerifier.VirtualizationAdmission.h"

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;

namespace YAKSys_Hybrid_CPU::Core
{

	bool RuntimeLegalityService::isKernelDomainIsolationEnabled() const
	{
		return _legalityChecker->isKernelDomainIsolationEnabled();
	}

	RuntimeLegalityService::RuntimeLegalityService(std::shared_ptr<ILegalityChecker> legalityChecker, std::shared_ptr<ILegalityCertificateCacheCoordinator> cacheCoordinator) : _legalityChecker(legalityChecker), _cacheCoordinator(cacheCoordinator)
	{
		ArgumentNullException::ThrowIfNull(legalityChecker);
		ArgumentNullException::ThrowIfNull(cacheCoordinator);

	}

	inline InterCoreDomainGuardDecision RuntimeLegalityService::EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag)
	{
		return _legalityChecker->EvaluateInterCoreDomainGuard(candidate, requestedDomainTag);
	}

	inline TypedSlotRejectClassification RuntimeLegalityService::ClassifyReject(TypedSlotRejectReason admissionReject, CertificateRejectDetail certDetail, SlotClass candidateClass, SlotPinningKind pinningKind)
	{
		return _legalityChecker->ClassifyReject(admissionReject, certDetail, candidateClass, pinningKind);
	}

	inline void RuntimeLegalityService::PrepareInterCore(ReplayPhaseContext phase, std::shared_ptr<BundleResourceCertificate> certificate)
	{
		_cacheCoordinator->PrepareInterCore(phase.getKey(), certificate);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public LegalityDecision EvaluateInterCoreLegality(ReplayPhaseContext phase, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
	inline LegalityDecision RuntimeLegalityService::EvaluateInterCoreLegality(ReplayPhaseContext phase, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask)
	{
		return _cacheCoordinator->EvaluateInterCoreLegality(_legalityChecker, phase.getKey(), bundle, bundleCertificate, bundleOwnerThreadId, requestedDomainTag, candidate, globalHardwareMask);
	}

	inline void RuntimeLegalityService::RefreshInterCoreAfterMutation(ReplayPhaseContext phase, std::shared_ptr<BundleResourceCertificate> certificate)
	{
		_cacheCoordinator->RefreshInterCoreAfterMutation(phase.getKey(), certificate);
	}

	inline void RuntimeLegalityService::PrepareSmt(ReplayPhaseContext phase, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		_cacheCoordinator->PrepareSmt(phase.getKey(), certificate, bundleMetadata, boundaryGuard);
	}

	inline LegalityDecision RuntimeLegalityService::EvaluateSmtBoundaryGuard(ReplayPhaseContext phase, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		return _cacheCoordinator->EvaluateSmtBoundaryGuard(_legalityChecker, phase.getKey(), bundleCertificate, bundleMetadata, boundaryGuard);
	}

	inline LegalityDecision RuntimeLegalityService::EvaluateSmtLegality(ReplayPhaseContext phase, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, std::shared_ptr<MicroOp> candidate)
	{
		return _cacheCoordinator->EvaluateSmtLegality(_legalityChecker, phase.getKey(), bundleCertificate, bundleMetadata, boundaryGuard, candidate);
	}

	inline std::shared_ptr<VirtualizationAdmissionIssueResult> RuntimeLegalityService::IssueVirtualizationAdmissionAfterStageB(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane)
	{
		std::shared_ptr<SafetyVerifier> verifier = std::dynamic_pointer_cast<SafetyVerifier>(_legalityChecker);
		return verifier != nullptr ? verifier->IssueVirtualizationAdmissionAfterStageB(phase, bundleMetadata, candidate, sourceSlotId, selectedLane) : std::make_shared<VirtualizationAdmissionIssueResult>(VirtualizationAdmissionIssueDecision::NotVmxMicroOp, nullptr, L"Only the canonical SafetyVerifier can issue E1 admission.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public VirtualizationAdmissionValidationResult ValidateVirtualizationAdmission(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, MicroOp candidate, int sourceSlotId, int selectedLane, SafetyVerifier.VirtualizationAdmissionCertificate? certificate)
	inline std::shared_ptr<VirtualizationAdmissionValidationResult> RuntimeLegalityService::ValidateVirtualizationAdmission(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane, std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> certificate)
	{
		std::shared_ptr<SafetyVerifier> verifier = std::dynamic_pointer_cast<SafetyVerifier>(_legalityChecker);
		return verifier != nullptr ? verifier->ValidateVirtualizationAdmission(phase, bundleMetadata, candidate, sourceSlotId, selectedLane, certificate) : std::make_shared<VirtualizationAdmissionValidationResult>(VirtualizationAdmissionValidationDecision::IssuerMismatch, L"Only the canonical SafetyVerifier can validate E1 admission.");
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public VirtualizationOperandCaptureResult CaptureVirtualizationOperandsAfterE1Validation(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, int sourceSlotId, int selectedLane, SafetyVerifier.VirtualizationAdmissionCertificate? certificate, ulong rs1Value, ulong restoreGeneration)
	inline std::shared_ptr<VirtualizationOperandCaptureResult> RuntimeLegalityService::CaptureVirtualizationOperandsAfterE1Validation(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, int sourceSlotId, int selectedLane, std::shared_ptr<SafetyVerifier::VirtualizationAdmissionCertificate> certificate, unsigned long long rs1Value, unsigned long long restoreGeneration)
	{
		std::shared_ptr<VirtualizationAdmissionValidationResult> validation = ValidateVirtualizationAdmission(phase, bundleMetadata, carrier, sourceSlotId, selectedLane, certificate);
		if (!validation->IsValidForFaultOnlyTransport)
		{
			return std::make_shared<VirtualizationOperandCaptureResult>(VirtualizationOperandCaptureDecision::InvalidE1, nullptr, validation->Reason);
		}

		return _virtualizationOperandMaterializer->CaptureAfterValidatedE1(carrier, certificate, rs1Value, restoreGeneration, Phase38VirtualizationOperationOwnerSnapshotRegistry::getExactSnapshot());
	}

	inline void RuntimeLegalityService::RefreshSmtAfterMutation(ReplayPhaseContext phase, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		_cacheCoordinator->RefreshSmtAfterMutation(phase.getKey(), certificate, bundleMetadata, boundaryGuard);
	}

	inline void RuntimeLegalityService::InvalidatePhaseMismatch(ReplayPhaseContext phase)
	{
		_cacheCoordinator->InvalidatePhaseMismatch(phase);
		std::shared_ptr<SafetyVerifier> verifier = std::dynamic_pointer_cast<SafetyVerifier>(_legalityChecker);
		if (verifier != nullptr)
		{
			verifier->InvalidateVirtualizationAdmissions(ReplayPhaseInvalidationReason::PhaseMismatch);
		}
	}

	inline void RuntimeLegalityService::Invalidate(ReplayPhaseInvalidationReason reason, bool invalidateInterCore, bool invalidateFourWay)
	{
		_cacheCoordinator->Invalidate(reason, invalidateInterCore, invalidateFourWay);
		std::shared_ptr<SafetyVerifier> verifier = std::dynamic_pointer_cast<SafetyVerifier>(_legalityChecker);
		if (verifier != nullptr)
		{
			verifier->InvalidateVirtualizationAdmissions(reason);
		}
	}

	std::shared_ptr<SafetyVerifier> RuntimeLegalityService::getResearchVirtualizationCanonicalVerifier() const
	{
		return std::dynamic_pointer_cast<SafetyVerifier>(_legalityChecker);
	}

	std::shared_ptr<SafetyVerifier> RuntimeLegalityService::getCanonicalVirtualizationVerifier() const
	{
		return std::dynamic_pointer_cast<SafetyVerifier>(_legalityChecker);
	}
}
