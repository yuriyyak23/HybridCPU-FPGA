#pragma once
namespace YAKSys_Hybrid_CPU::Core { class MicroOp; class BundleResourceCertificate4Way; enum class CertificateRejectDetail : unsigned char; class LegalityDecision; class ILegalityChecker; class PhaseCertificateTemplateKey4Way; class SmtBundleMetadata4Way; class BoundaryGuardState; class InterCoreDomainGuardDecision; class TypedSlotRejectClassification; class VirtualizationAdmissionCertificate; struct VirtualizationAdmissionIssueResult; struct VirtualizationAdmissionValidationResult; struct VirtualizationOperandCaptureResult; }

#include "ReplayPhaseSubstrate.h"
#include "BundleResourceCertificate.h"
#include "SafetyMask128.h"





#include "../Scheduling/SlotLegality/SlotClassDefinitions.h"


#include <vector>
#include <memory>




namespace YAKSys_Hybrid_CPU::Core
{
	class VmxMicroOp;
	/// <summary>
	/// Runtime-local inter-core legality certificate cache seam.
	/// Keeps scheduler/runtime consumers on an explicit cache service contract
	/// instead of the concrete replay-template container type.
	/// </summary>
	class IInterCoreLegalityCertificateCache
	{
	public:
		virtual bool isValid() const = 0;

		virtual bool Matches(ReplayPhaseContext phase) = 0;

		virtual void Prepare(PhaseCertificateTemplateKey templateKey, std::shared_ptr<BundleResourceCertificate> certificate) = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: LegalityCertificateCacheEvaluation<LegalityDecision> EvaluateLegality(ILegalityChecker legalityChecker, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default);
		virtual LegalityCertificateCacheEvaluation<LegalityDecision> EvaluateLegality(std::shared_ptr<ILegalityChecker> legalityChecker, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128()) = 0;

		virtual LegalityCertificateCacheTelemetry RefreshAfterMutation(PhaseCertificateTemplateKey templateKey, std::shared_ptr<BundleResourceCertificate> certificate) = 0;

		virtual bool Invalidate() = 0;
	};

	/// <summary>
	/// Runtime-local SMT legality certificate cache seam.
	/// Mirrors the 4-way replay/template lifecycle without exposing the concrete
	/// cache container type to scheduler/runtime consumers.
	/// </summary>
	class ISmtLegalityCertificateCache4Way
	{
	public:
		virtual bool isValid() const = 0;

		virtual bool Matches(ReplayPhaseContext phase) = 0;

		virtual LegalityCertificateCacheEvaluation<LegalityDecision> EvaluateLegality(std::shared_ptr<ILegalityChecker> legalityChecker, BundleResourceCertificate4Way bundleCertificate, PhaseCertificateTemplateKey4Way liveTemplateKey, std::shared_ptr<MicroOp> candidate) = 0;

		virtual void Prepare(PhaseCertificateTemplateKey4Way templateKey, BundleResourceCertificate4Way certificate) = 0;

		virtual LegalityCertificateCacheTelemetry RefreshAfterMutation(PhaseCertificateTemplateKey4Way templateKey, BundleResourceCertificate4Way certificate) = 0;

		virtual bool Invalidate() = 0;
	};

	/// <summary>
	/// Default runtime-local legality cache provisioning.
	/// Keeps scheduler construction on the cache seam rather than concrete cache types.
	/// </summary>
	class RuntimeLegalityCertificateCacheFactory final
	{
	public:
		static std::shared_ptr<IInterCoreLegalityCertificateCache> CreateInterCore();

		static std::shared_ptr<ISmtLegalityCertificateCache4Way> CreateSmt4Way();
	};

	/// <summary>
	/// Scheduler-local sink for replay legality-cache telemetry.
	/// Exposes semantic counter updates instead of raw cache telemetry transport.
	/// </summary>
	class ILegalityCertificateCacheTelemetrySink
	{
	public:
		virtual void RecordLegalityCertificateCacheHit(long long estimatedChecksSaved) = 0;

		virtual void RecordLegalityCertificateCacheMiss() = 0;

		virtual void RecordLegalityCertificateCacheInvalidation(ReplayPhaseInvalidationReason reason) = 0;
	};

	/// <summary>
	/// Runtime-local legality service entrypoint for inter-core + SMT hot paths plus
	/// adjacent scheduler diagnostics. Owns live replay-phase handoff so scheduler
	/// callers do not coordinate checker and cache/coordinator services manually.
	/// </summary>
	class IRuntimeLegalityService
	{
	public:
		virtual bool isKernelDomainIsolationEnabled() const = 0;

		virtual InterCoreDomainGuardDecision EvaluateInterCoreDomainGuard(std::shared_ptr<MicroOp> candidate, unsigned long long requestedDomainTag) = 0;

		virtual TypedSlotRejectClassification ClassifyReject(TypedSlotRejectReason admissionReject, CertificateRejectDetail certDetail, SlotClass candidateClass, SlotPinningKind pinningKind) = 0;

		virtual void PrepareInterCore(ReplayPhaseContext phase, std::shared_ptr<BundleResourceCertificate> certificate) = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: LegalityDecision EvaluateInterCoreLegality(ReplayPhaseContext phase, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default);
		virtual LegalityDecision EvaluateInterCoreLegality(ReplayPhaseContext phase, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128()) = 0;

		virtual void RefreshInterCoreAfterMutation(ReplayPhaseContext phase, std::shared_ptr<BundleResourceCertificate> certificate) = 0;

		virtual void PrepareSmt(ReplayPhaseContext phase, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) = 0;

		virtual LegalityDecision EvaluateSmtBoundaryGuard(ReplayPhaseContext phase, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) = 0;

		virtual LegalityDecision EvaluateSmtLegality(ReplayPhaseContext phase, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, std::shared_ptr<MicroOp> candidate) = 0;

		virtual void RefreshSmtAfterMutation(ReplayPhaseContext phase, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) = 0;

		virtual void InvalidatePhaseMismatch(ReplayPhaseContext phase) = 0;

		virtual void Invalidate(ReplayPhaseInvalidationReason reason, bool invalidateInterCore = true, bool invalidateFourWay = true) = 0;
	};

	/// <summary>
	/// Internal E1-only issuance seam. It is deliberately absent from the public
	/// runtime-legality interface so compatibility callers cannot request authority.
	/// </summary>
	class IVirtualizationAdmissionService
	{
	public:
		virtual std::shared_ptr<VirtualizationAdmissionIssueResult> IssueVirtualizationAdmissionAfterStageB(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane) = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: VirtualizationAdmissionValidationResult ValidateVirtualizationAdmission(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, MicroOp candidate, int sourceSlotId, int selectedLane, SafetyVerifier.VirtualizationAdmissionCertificate? certificate);
		virtual std::shared_ptr<VirtualizationAdmissionValidationResult> ValidateVirtualizationAdmission(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<MicroOp> candidate, int sourceSlotId, int selectedLane, std::shared_ptr<VirtualizationAdmissionCertificate> certificate) = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: VirtualizationOperandCaptureResult CaptureVirtualizationOperandsAfterE1Validation(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, VmxMicroOp carrier, int sourceSlotId, int selectedLane, SafetyVerifier.VirtualizationAdmissionCertificate? certificate, ulong rs1Value, ulong restoreGeneration);
		virtual std::shared_ptr<VirtualizationOperandCaptureResult> CaptureVirtualizationOperandsAfterE1Validation(ReplayPhaseContext phase, SmtBundleMetadata4Way bundleMetadata, std::shared_ptr<VmxMicroOp> carrier, int sourceSlotId, int selectedLane, std::shared_ptr<VirtualizationAdmissionCertificate> certificate, unsigned long long rs1Value, unsigned long long restoreGeneration) = 0;
	};

	/// <summary>
	/// Runtime-local coordinator that owns cache lifecycle seams adjacent to legality checking.
	/// Keeps scheduler callers off raw cache invalidation/telemetry mechanics while preserving
	/// the existing prepare/evaluate/refresh/invalidate behavior.
	/// </summary>
	class ILegalityCertificateCacheCoordinator
	{
	public:
		virtual void PrepareInterCore(ReplayPhaseKey phaseKey, std::shared_ptr<BundleResourceCertificate> certificate) = 0;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: LegalityDecision EvaluateInterCoreLegality(ILegalityChecker legalityChecker, ReplayPhaseKey phaseKey, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default);
		virtual LegalityDecision EvaluateInterCoreLegality(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask = YAKSys_Hybrid_CPU::Core::SafetyMask128()) = 0;

		virtual void RefreshInterCoreAfterMutation(ReplayPhaseKey phaseKey, std::shared_ptr<BundleResourceCertificate> certificate) = 0;

		virtual void PrepareSmt(ReplayPhaseKey phaseKey, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) = 0;

		virtual LegalityDecision EvaluateSmtBoundaryGuard(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) = 0;

		virtual LegalityDecision EvaluateSmtLegality(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, std::shared_ptr<MicroOp> candidate) = 0;

		virtual void RefreshSmtAfterMutation(ReplayPhaseKey phaseKey, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard) = 0;

		virtual void InvalidatePhaseMismatch(ReplayPhaseContext phase) = 0;

		virtual void Invalidate(ReplayPhaseInvalidationReason reason, bool invalidateInterCore = true, bool invalidateFourWay = true) = 0;
	};

	/// <summary>
	/// Default runtime-local legality cache coordinator provisioning.
	/// Keeps scheduler construction on a single cache/coordinator seam.
	/// </summary>
	class RuntimeLegalityCertificateCacheCoordinatorFactory final
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static ILegalityCertificateCacheCoordinator CreateDefault(ILegalityCertificateCacheTelemetrySink telemetrySink, IInterCoreLegalityCertificateCache? interCoreCache = null, System.Nullable<ISmtLegalityCertificateCache4Way> smtCache = null)
		static std::shared_ptr<ILegalityCertificateCacheCoordinator> CreateDefault(std::shared_ptr<ILegalityCertificateCacheTelemetrySink> telemetrySink, std::shared_ptr<IInterCoreLegalityCertificateCache> interCoreCache = nullptr, std::shared_ptr<ISmtLegalityCertificateCache4Way> smtCache = nullptr);
	};

	/// <summary>
	/// Default runtime legality/cache coordinator for inter-core + SMT replay certificate reuse.
}
