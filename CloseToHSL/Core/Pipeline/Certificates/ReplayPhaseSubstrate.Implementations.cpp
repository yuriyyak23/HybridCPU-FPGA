#include "ReplayPhaseSubstrate.Implementations.h"
#include "YAKSys_Hybrid_CPU.Core.RuntimeLegalityService.h"
#include "StructuralIdentity/HardwareHash.h"
#include <any>

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;

namespace YAKSys_Hybrid_CPU::Core
{

	LegalityCertificateCacheCoordinator::LegalityCertificateCacheCoordinator(std::shared_ptr<ILegalityCertificateCacheTelemetrySink> telemetrySink, std::shared_ptr<IInterCoreLegalityCertificateCache> interCoreCache, std::shared_ptr<ISmtLegalityCertificateCache4Way> smtCache) : _telemetrySink(telemetrySink), _interCoreCache(interCoreCache), _smtCache(smtCache)
	{
		ArgumentNullException::ThrowIfNull(telemetrySink);
		ArgumentNullException::ThrowIfNull(interCoreCache);
		ArgumentNullException::ThrowIfNull(smtCache);

	}

	inline void LegalityCertificateCacheCoordinator::PrepareInterCore(ReplayPhaseKey phaseKey, std::shared_ptr<BundleResourceCertificate> certificate)
	{
		_interCoreCache->Prepare(CreateInterCoreTemplateKey(phaseKey, certificate), certificate);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public LegalityDecision EvaluateInterCoreLegality(ILegalityChecker legalityChecker, ReplayPhaseKey phaseKey, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
	inline LegalityDecision LegalityCertificateCacheCoordinator::EvaluateInterCoreLegality(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask)
	{
		LegalityCertificateCacheEvaluation<LegalityDecision> legalityEvaluation = _interCoreCache->EvaluateLegality(legalityChecker, bundle, bundleCertificate, CreateInterCoreTemplateKey(phaseKey, bundleCertificate), bundleOwnerThreadId, requestedDomainTag, candidate, globalHardwareMask);
		ApplyTelemetry(legalityEvaluation.getTelemetry());
		return legalityEvaluation.getLegalityResult();
	}

	inline void LegalityCertificateCacheCoordinator::RefreshInterCoreAfterMutation(ReplayPhaseKey phaseKey, std::shared_ptr<BundleResourceCertificate> certificate)
	{
		ApplyTelemetry(_interCoreCache->RefreshAfterMutation(CreateInterCoreTemplateKey(phaseKey, certificate), certificate));
	}

	inline void LegalityCertificateCacheCoordinator::PrepareSmt(ReplayPhaseKey phaseKey, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		_smtCache->Prepare(CreateSmtTemplateKey(phaseKey, certificate, bundleMetadata, boundaryGuard), certificate);
	}

	inline LegalityDecision LegalityCertificateCacheCoordinator::EvaluateSmtBoundaryGuard(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		return legalityChecker->EvaluateSmtBoundaryGuard(CreateSmtTemplateKey(phaseKey, bundleCertificate, bundleMetadata, boundaryGuard));
	}

	inline LegalityDecision LegalityCertificateCacheCoordinator::EvaluateSmtLegality(std::shared_ptr<ILegalityChecker> legalityChecker, ReplayPhaseKey phaseKey, BundleResourceCertificate4Way bundleCertificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard, std::shared_ptr<MicroOp> candidate)
	{
		LegalityCertificateCacheEvaluation<LegalityDecision> legalityEvaluation = _smtCache->EvaluateLegality(legalityChecker, bundleCertificate, CreateSmtTemplateKey(phaseKey, bundleCertificate, bundleMetadata, boundaryGuard), candidate);
		ApplyTelemetry(legalityEvaluation.getTelemetry());
		return legalityEvaluation.getLegalityResult();
	}

	inline void LegalityCertificateCacheCoordinator::RefreshSmtAfterMutation(ReplayPhaseKey phaseKey, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		ApplyTelemetry(_smtCache->RefreshAfterMutation(CreateSmtTemplateKey(phaseKey, certificate, bundleMetadata, boundaryGuard), certificate));
	}

	inline void LegalityCertificateCacheCoordinator::InvalidatePhaseMismatch(ReplayPhaseContext phase)
	{
		bool invalidateInterCore = _interCoreCache->isValid() && !_interCoreCache->Matches(phase);
		bool invalidateFourWay = _smtCache->isValid() && !_smtCache->Matches(phase);
		if (invalidateInterCore || invalidateFourWay)
		{
			Invalidate(ReplayPhaseInvalidationReason::PhaseMismatch, invalidateInterCore, invalidateFourWay);
		}
	}

	inline void LegalityCertificateCacheCoordinator::Invalidate(ReplayPhaseInvalidationReason reason, bool invalidateInterCore, bool invalidateFourWay)
	{
		bool interCoreInvalidated = invalidateInterCore && _interCoreCache->Invalidate();
		bool fourWayInvalidated = invalidateFourWay && _smtCache->Invalidate();
		ApplyTelemetry(LegalityCertificateCacheTelemetry::FromInvalidationEvent(reason, interCoreInvalidated || fourWayInvalidated));
	}

	inline void LegalityCertificateCacheCoordinator::ApplyTelemetry(LegalityCertificateCacheTelemetry telemetry)
	{
		if (telemetry.getReadyHitsDelta() > 0)
		{
			_telemetrySink->RecordLegalityCertificateCacheHit(telemetry.getEstimatedChecksSavedDelta());
		}

		if (telemetry.getReadyMissesDelta() > 0)
		{
			_telemetrySink->RecordLegalityCertificateCacheMiss();
		}

		if (telemetry.getInvalidationEventsDelta() > 0 && telemetry.getLastInvalidationReason() != ReplayPhaseInvalidationReason::None)
		{
			_telemetrySink->RecordLegalityCertificateCacheInvalidation(telemetry.getLastInvalidationReason());
		}
	}

	inline PhaseCertificateTemplateKey LegalityCertificateCacheCoordinator::CreateInterCoreTemplateKey(ReplayPhaseKey phaseKey, std::shared_ptr<BundleResourceCertificate> certificate)
	{
		return PhaseCertificateTemplateKey(phaseKey, certificate->getStructuralIdentity());
	}

	inline PhaseCertificateTemplateKey4Way LegalityCertificateCacheCoordinator::CreateSmtTemplateKey(ReplayPhaseKey phaseKey, BundleResourceCertificate4Way certificate, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		return PhaseCertificateTemplateKey4Way(phaseKey, certificate.getStructuralIdentity(), bundleMetadata, boundaryGuard);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static IRuntimeLegalityService CreateDefault(ILegalityCertificateCacheTelemetrySink telemetrySink, IInterCoreLegalityCertificateCache? interCoreCache = null, System.Nullable<ISmtLegalityCertificateCache4Way> smtCache = null, System.Nullable<ILegalityCertificateCacheCoordinator> legalityCertificateCacheCoordinator = null)
	std::shared_ptr<IRuntimeLegalityService> RuntimeLegalityServiceFactory::CreateDefault(std::shared_ptr<ILegalityCertificateCacheTelemetrySink> telemetrySink, std::shared_ptr<IInterCoreLegalityCertificateCache> interCoreCache, std::shared_ptr<ISmtLegalityCertificateCache4Way> smtCache, std::shared_ptr<ILegalityCertificateCacheCoordinator> legalityCertificateCacheCoordinator)
	{
		return CreateCompatibilityDefault(telemetrySink, RuntimeLegalityCheckerFactory::CreateCompatibilityDefault(), interCoreCache, smtCache, legalityCertificateCacheCoordinator);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static IRuntimeLegalityService CreateCompatibilityDefault(ILegalityCertificateCacheTelemetrySink telemetrySink, ILegalityChecker legalityChecker, IInterCoreLegalityCertificateCache? interCoreCache = null, System.Nullable<ISmtLegalityCertificateCache4Way> smtCache = null, System.Nullable<ILegalityCertificateCacheCoordinator> legalityCertificateCacheCoordinator = null)
	std::shared_ptr<IRuntimeLegalityService> RuntimeLegalityServiceFactory::CreateCompatibilityDefault(std::shared_ptr<ILegalityCertificateCacheTelemetrySink> telemetrySink, std::shared_ptr<ILegalityChecker> legalityChecker, std::shared_ptr<IInterCoreLegalityCertificateCache> interCoreCache, std::shared_ptr<ISmtLegalityCertificateCache4Way> smtCache, std::shared_ptr<ILegalityCertificateCacheCoordinator> legalityCertificateCacheCoordinator)
	{
		ArgumentNullException::ThrowIfNull(telemetrySink);
		ArgumentNullException::ThrowIfNull(legalityChecker);

		std::shared_ptr<ILegalityCertificateCacheCoordinator> coordinator = (legalityCertificateCacheCoordinator != nullptr) ? legalityCertificateCacheCoordinator : RuntimeLegalityCertificateCacheCoordinatorFactory::CreateDefault(telemetrySink, interCoreCache, smtCache);
		return std::make_shared<RuntimeLegalityService>(legalityChecker, coordinator);
	}

	bool InterCoreLegalityCertificateCache::isValid() const
	{
		return _template.isValid();
	}

	bool InterCoreLegalityCertificateCache::Matches(ReplayPhaseContext phase)
	{
		return _template.Matches(phase);
	}

	void InterCoreLegalityCertificateCache::Prepare(PhaseCertificateTemplateKey templateKey, std::shared_ptr<BundleResourceCertificate> certificate)
	{
		if (!templateKey.isValid())
		{
			return;
		}

		if (_template.Matches(templateKey))
		{
			return;
		}

		_template = PhaseCertificateTemplate(templateKey, certificate);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: [MethodImpl(MethodImplOptions.AggressiveInlining)] public LegalityCertificateCacheEvaluation<LegalityDecision> EvaluateLegality(ILegalityChecker legalityChecker, IReadOnlyList<MicroOp?> bundle, BundleResourceCertificate bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, int bundleOwnerThreadId, ulong requestedDomainTag, MicroOp candidate, SafetyMask128 globalHardwareMask = default)
	LegalityCertificateCacheEvaluation<LegalityDecision> InterCoreLegalityCertificateCache::EvaluateLegality(std::shared_ptr<ILegalityChecker> legalityChecker, const std::vector<std::shared_ptr<MicroOp>> &bundle, std::shared_ptr<BundleResourceCertificate> bundleCertificate, PhaseCertificateTemplateKey liveTemplateKey, int bundleOwnerThreadId, unsigned long long requestedDomainTag, std::shared_ptr<MicroOp> candidate, SafetyMask128 globalHardwareMask)
	{
		ArgumentNullException::ThrowIfNull(candidate);

		LegalityDecision decision = legalityChecker->EvaluateInterCoreLegality(bundle, bundleCertificate, liveTemplateKey, _template, bundleOwnerThreadId, requestedDomainTag, candidate, globalHardwareMask);
		return LegalityCertificateCacheEvaluation<LegalityDecision>(decision, LegalityCertificateCacheTelemetry::FromReuseDecision(decision.getAttemptedReplayCertificateReuse(), decision.getReusedReplayCertificate(), bundleCertificate->getOperationCount()));
	}

	LegalityCertificateCacheTelemetry InterCoreLegalityCertificateCache::RefreshAfterMutation(PhaseCertificateTemplateKey templateKey, std::shared_ptr<BundleResourceCertificate> certificate)
	{
		bool wasInvalidated = Invalidate();
		if (templateKey.isValid())
		{
			_template = PhaseCertificateTemplate(templateKey, certificate);
		}

		return LegalityCertificateCacheTelemetry::FromInvalidationEvent(ReplayPhaseInvalidationReason::CertificateMutation, wasInvalidated);
	}

	bool InterCoreLegalityCertificateCache::Invalidate()
	{
		bool wasValid = _template.isValid();
		_template = YAKSys_Hybrid_CPU::Core::PhaseCertificateTemplate();
		return wasValid;
	}

	BoundaryGuardState::BoundaryGuardState(long long serializingEpochId, SmtReplayBoundaryMode boundaryMode)
	{
		SerializingEpochId = serializingEpochId;
		BoundaryMode = boundaryMode;
	}

	BoundaryGuardState BoundaryGuardState::Open(long long serializingEpochId)
	{
		return BoundaryGuardState(serializingEpochId, SmtReplayBoundaryMode::Open);
	}

	long long BoundaryGuardState::getSerializingEpochId() const
	{
		return SerializingEpochId;
	}

	SmtReplayBoundaryMode BoundaryGuardState::getBoundaryMode() const
	{
		return BoundaryMode;
	}

	bool BoundaryGuardState::isValid() const
	{
		return getSerializingEpochId() >= 0;
	}

	bool BoundaryGuardState::getBlocksSmtInjection() const
	{
		return getBoundaryMode() != SmtReplayBoundaryMode::Open;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public BoundaryGuardState WithOperation(MicroOp? op)
	BoundaryGuardState BoundaryGuardState::WithOperation(std::shared_ptr<MicroOp> op) const
	{
		if (op == nullptr || getBlocksSmtInjection())
		{
			return *this;
		}

		const auto serializationClass = op->getSerializationClass();
		return serializationClass == Arch::SerializationClass::FullSerial || serializationClass == Arch::SerializationClass::VmxSerial ? BoundaryGuardState(getSerializingEpochId(), SmtReplayBoundaryMode::SerializingBundle) : *this;
	}

	bool BoundaryGuardState::Equals(BoundaryGuardState other) const
	{
		return getSerializingEpochId() == other.getSerializingEpochId() && getBoundaryMode() == other.getBoundaryMode();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool BoundaryGuardState::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<BoundaryGuardState>(&obj);
		return other != nullptr && Equals(*other);
	}

	int BoundaryGuardState::GetHashCode() const
	{
		int hash = static_cast<int>(0x5B4D8A13);
		hash = (hash * 397) ^ static_cast<int>(getSerializingEpochId());
		hash = (hash * 397) ^ static_cast<int>(getBoundaryMode());
		return hash;
	}

	SmtBundleMetadata4Way::SmtBundleMetadata4Way(int ownerVirtualThreadId, int ownerContextId, unsigned long long ownerDomainTag, unsigned long long bundleDomainXor, unsigned long long bundleDomainSum, int operationCount)
	{
		OwnerVirtualThreadId = ownerVirtualThreadId;
		OwnerContextId = ownerContextId;
		OwnerDomainTag = ownerDomainTag;
		BundleDomainXor = bundleDomainXor;
		BundleDomainSum = bundleDomainSum;
		OperationCount = operationCount;
	}

	SmtBundleMetadata4Way SmtBundleMetadata4Way::Empty(int ownerVirtualThreadId)
	{
		return SmtBundleMetadata4Way(ownerVirtualThreadId, -1, 0, 0, 0, 0);
	}

	int SmtBundleMetadata4Way::getOwnerVirtualThreadId() const
	{
		return OwnerVirtualThreadId;
	}

	int SmtBundleMetadata4Way::getOwnerContextId() const
	{
		return OwnerContextId;
	}

	unsigned long long SmtBundleMetadata4Way::getOwnerDomainTag() const
	{
		return OwnerDomainTag;
	}

	unsigned long long SmtBundleMetadata4Way::getBundleDomainXor() const
	{
		return BundleDomainXor;
	}

	unsigned long long SmtBundleMetadata4Way::getBundleDomainSum() const
	{
		return BundleDomainSum;
	}

	int SmtBundleMetadata4Way::getOperationCount() const
	{
		return OperationCount;
	}

	bool SmtBundleMetadata4Way::isValid() const
	{
		return static_cast<unsigned int>(getOwnerVirtualThreadId()) < BundleResourceCertificate4Way::SMT_WAYS && getOperationCount() >= 0;
	}

	bool SmtBundleMetadata4Way::getHasKnownOwnerContext() const
	{
		return getOwnerContextId() >= 0;
	}

	bool SmtBundleMetadata4Way::getHasDomainRestriction() const
	{
		return getOwnerDomainTag() != 0;
	}

	unsigned long long SmtBundleMetadata4Way::getBundleDomainShapeId() const
	{
		HardwareHash bundleHasher = HardwareHash();
		bundleHasher.Initialize();
		bundleHasher.Compress(getBundleDomainXor());
		bundleHasher.Compress(getBundleDomainSum());
		bundleHasher.Compress(static_cast<unsigned long long>(getOperationCount()));
		return bundleHasher.Finalize();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SmtBundleMetadata4Way WithOperation(MicroOp? op)
	SmtBundleMetadata4Way SmtBundleMetadata4Way::WithOperation(std::shared_ptr<MicroOp> op) const
	{
		if (op == nullptr)
		{
			return *this;
		}

		int ownerContextId = getOwnerContextId();
		unsigned long long ownerDomainTag = getOwnerDomainTag();
		if (op->getVirtualThreadId() == getOwnerVirtualThreadId() && ownerContextId < 0)
		{
			ownerContextId = op->getOwnerContextId();
			ownerDomainTag = op->getPlacement().DomainTag;
		}

		unsigned long long opShape = ComputeOperationDomainShape(op);
		unsigned long long bundleDomainSum;
//		unchecked
			bundleDomainSum = getBundleDomainSum() + opShape;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.

		return SmtBundleMetadata4Way(getOwnerVirtualThreadId(), ownerContextId, ownerDomainTag, getBundleDomainXor() ^ opShape, bundleDomainSum, getOperationCount() + 1);
	}

	bool SmtBundleMetadata4Way::Equals(SmtBundleMetadata4Way other) const
	{
		return getOwnerVirtualThreadId() == other.getOwnerVirtualThreadId() && getOwnerContextId() == other.getOwnerContextId() && getOwnerDomainTag() == other.getOwnerDomainTag() && getBundleDomainXor() == other.getBundleDomainXor() && getBundleDomainSum() == other.getBundleDomainSum() && getOperationCount() == other.getOperationCount();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool SmtBundleMetadata4Way::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<SmtBundleMetadata4Way>(&obj);
		return other != nullptr && Equals(*other);
	}

	int SmtBundleMetadata4Way::GetHashCode() const
	{
		int hash = static_cast<int>(0x5B4D8A13);
		hash = (hash * 397) ^ getOwnerVirtualThreadId();
		hash = (hash * 397) ^ getOwnerContextId();
		hash = (hash * 397) ^ static_cast<int>(getOwnerDomainTag());
		hash = (hash * 397) ^ static_cast<int>(getBundleDomainXor());
		hash = (hash * 397) ^ static_cast<int>(getBundleDomainSum());
		hash = (hash * 397) ^ getOperationCount();
		return hash;
	}

	unsigned long long SmtBundleMetadata4Way::ComputeOperationDomainShape(std::shared_ptr<MicroOp> op)
	{
		HardwareHash opHasher = HardwareHash();
		opHasher.Initialize();
		opHasher.Compress(static_cast<unsigned long long>(static_cast<unsigned char>(op->getVirtualThreadId())));
		opHasher.Compress(static_cast<unsigned long long>(static_cast<unsigned int>(op->getOwnerContextId())));
		opHasher.Compress(op->getPlacement().DomainTag);
		return opHasher.Finalize();
	}

	PhaseCertificateTemplateKey4Way::PhaseCertificateTemplateKey4Way(ReplayPhaseKey phaseKey, BundleResourceCertificateIdentity4Way certificateIdentity, SmtBundleMetadata4Way bundleMetadata, BoundaryGuardState boundaryGuard)
	{
		PhaseKey = phaseKey;
		CertificateIdentity = certificateIdentity;
		BundleMetadata = bundleMetadata;
		BoundaryGuard = boundaryGuard;
	}

	ReplayPhaseKey PhaseCertificateTemplateKey4Way::getPhaseKey() const
	{
		return PhaseKey;
	}

	BundleResourceCertificateIdentity4Way PhaseCertificateTemplateKey4Way::getCertificateIdentity() const
	{
		return CertificateIdentity;
	}

	SmtBundleMetadata4Way PhaseCertificateTemplateKey4Way::getBundleMetadata() const
	{
		return BundleMetadata;
	}

	BoundaryGuardState PhaseCertificateTemplateKey4Way::getBoundaryGuard() const
	{
		return BoundaryGuard;
	}

	bool PhaseCertificateTemplateKey4Way::isValid() const
	{
		return getPhaseKey().isValid() && getCertificateIdentity().isValid() && getBundleMetadata().isValid() && getBoundaryGuard().isValid();
	}

	bool PhaseCertificateTemplateKey4Way::Equals(PhaseCertificateTemplateKey4Way other) const
	{
		return getPhaseKey().Equals(other.getPhaseKey()) && getCertificateIdentity().Equals(other.getCertificateIdentity()) && getBundleMetadata().Equals(other.getBundleMetadata()) && getBoundaryGuard().Equals(other.getBoundaryGuard());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool PhaseCertificateTemplateKey4Way::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<PhaseCertificateTemplateKey4Way>(&obj);
		return other != nullptr && Equals(*other);
	}

	int PhaseCertificateTemplateKey4Way::GetHashCode() const
	{
		int hash = static_cast<int>(0x5B4D8A13);
		hash = (hash * 397) ^ getPhaseKey().GetHashCode();
		hash = (hash * 397) ^ getCertificateIdentity().GetHashCode();
		hash = (hash * 397) ^ getBundleMetadata().GetHashCode();
		hash = (hash * 397) ^ getBoundaryGuard().GetHashCode();
		return hash;
	}

	PhaseCertificateTemplate4Way::PhaseCertificateTemplate4Way(PhaseCertificateTemplateKey4Way templateKey, BundleResourceCertificate4Way certificate)
	{
		TemplateKey = templateKey;
		Certificate = certificate;
	}

	PhaseCertificateTemplateKey4Way PhaseCertificateTemplate4Way::getTemplateKey() const
	{
		return TemplateKey;
	}

	ReplayPhaseKey PhaseCertificateTemplate4Way::getPhaseKey() const
	{
		return getTemplateKey().getPhaseKey();
	}

	BundleResourceCertificate4Way PhaseCertificateTemplate4Way::getCertificate() const
	{
		return Certificate;
	}

	BundleResourceCertificateIdentity4Way PhaseCertificateTemplate4Way::getCertificateIdentity() const
	{
		return getTemplateKey().getCertificateIdentity();
	}

	SmtBundleMetadata4Way PhaseCertificateTemplate4Way::getBundleMetadata() const
	{
		return getTemplateKey().getBundleMetadata();
	}

	BoundaryGuardState PhaseCertificateTemplate4Way::getBoundaryGuard() const
	{
		return getTemplateKey().getBoundaryGuard();
	}

	bool PhaseCertificateTemplate4Way::isValid() const
	{
		return getTemplateKey().isValid();
	}

	bool PhaseCertificateTemplate4Way::Matches(ReplayPhaseContext phase) const
	{
		return isValid() && phase.Matches(getTemplateKey().getPhaseKey());
	}

	bool PhaseCertificateTemplate4Way::Matches(PhaseCertificateTemplateKey4Way templateKey) const
	{
		return isValid() && getTemplateKey().Equals(templateKey);
	}

	bool SmtLegalityCertificateCache4Way::isValid() const
	{
		return _template.isValid();
	}

	bool SmtLegalityCertificateCache4Way::Matches(ReplayPhaseContext phase)
	{
		return _template.Matches(phase);
	}

	LegalityCertificateCacheEvaluation<LegalityDecision> SmtLegalityCertificateCache4Way::EvaluateLegality(std::shared_ptr<ILegalityChecker> legalityChecker, BundleResourceCertificate4Way bundleCertificate, PhaseCertificateTemplateKey4Way liveTemplateKey, std::shared_ptr<MicroOp> candidate)
	{
		LegalityDecision decision = legalityChecker->EvaluateSmtLegality(bundleCertificate, liveTemplateKey, _template, candidate);
		return LegalityCertificateCacheEvaluation<LegalityDecision>(decision, LegalityCertificateCacheTelemetry::FromReuseDecision(decision.getAttemptedReplayCertificateReuse(), decision.getReusedReplayCertificate(), bundleCertificate.OperationCount));
	}

	void SmtLegalityCertificateCache4Way::Prepare(PhaseCertificateTemplateKey4Way templateKey, BundleResourceCertificate4Way certificate)
	{
		if (!templateKey.isValid() || _template.Matches(templateKey))
		{
			return;
		}

		_template = PhaseCertificateTemplate4Way(templateKey, certificate);
	}

	LegalityCertificateCacheTelemetry SmtLegalityCertificateCache4Way::RefreshAfterMutation(PhaseCertificateTemplateKey4Way templateKey, BundleResourceCertificate4Way certificate)
	{
		bool wasInvalidated = Invalidate();
		_template = templateKey.isValid() ? PhaseCertificateTemplate4Way(templateKey, certificate) : YAKSys_Hybrid_CPU::Core::PhaseCertificateTemplate4Way();
		return LegalityCertificateCacheTelemetry::FromInvalidationEvent(ReplayPhaseInvalidationReason::CertificateMutation, wasInvalidated);
	}

	bool SmtLegalityCertificateCache4Way::Invalidate()
	{
		bool wasValid = _template.isValid();
		_template = YAKSys_Hybrid_CPU::Core::PhaseCertificateTemplate4Way();
		return wasValid;
	}

	long long SchedulerPhaseMetrics::getReplayAwareCycles() const
	{
		return ReplayAwareCycles;
	}

	void SchedulerPhaseMetrics::setReplayAwareCycles(long long value)
	{
		ReplayAwareCycles = value;
	}

	long long SchedulerPhaseMetrics::getPhaseCertificateReadyHits() const
	{
		return PhaseCertificateReadyHits;
	}

	void SchedulerPhaseMetrics::setPhaseCertificateReadyHits(long long value)
	{
		PhaseCertificateReadyHits = value;
	}

	long long SchedulerPhaseMetrics::getPhaseCertificateReadyMisses() const
	{
		return PhaseCertificateReadyMisses;
	}

	void SchedulerPhaseMetrics::setPhaseCertificateReadyMisses(long long value)
	{
		PhaseCertificateReadyMisses = value;
	}

	long long SchedulerPhaseMetrics::getEstimatedChecksSaved() const
	{
		return EstimatedChecksSaved;
	}

	void SchedulerPhaseMetrics::setEstimatedChecksSaved(long long value)
	{
		EstimatedChecksSaved = value;
	}

	long long SchedulerPhaseMetrics::getPhaseCertificateInvalidations() const
	{
		return PhaseCertificateInvalidations;
	}

	void SchedulerPhaseMetrics::setPhaseCertificateInvalidations(long long value)
	{
		PhaseCertificateInvalidations = value;
	}

	long long SchedulerPhaseMetrics::getPhaseCertificateMutationInvalidations() const
	{
		return PhaseCertificateMutationInvalidations;
	}

	void SchedulerPhaseMetrics::setPhaseCertificateMutationInvalidations(long long value)
	{
		PhaseCertificateMutationInvalidations = value;
	}

	long long SchedulerPhaseMetrics::getPhaseCertificatePhaseMismatchInvalidations() const
	{
		return PhaseCertificatePhaseMismatchInvalidations;
	}

	void SchedulerPhaseMetrics::setPhaseCertificatePhaseMismatchInvalidations(long long value)
	{
		PhaseCertificatePhaseMismatchInvalidations = value;
	}

	ReplayPhaseInvalidationReason SchedulerPhaseMetrics::getLastCertificateInvalidationReason() const
	{
		return LastCertificateInvalidationReason;
	}

	void SchedulerPhaseMetrics::setLastCertificateInvalidationReason(ReplayPhaseInvalidationReason value)
	{
		LastCertificateInvalidationReason = value;
	}

	long long SchedulerPhaseMetrics::getDeterminismReferenceOpportunitySlots() const
	{
		return DeterminismReferenceOpportunitySlots;
	}

	void SchedulerPhaseMetrics::setDeterminismReferenceOpportunitySlots(long long value)
	{
		DeterminismReferenceOpportunitySlots = value;
	}

	long long SchedulerPhaseMetrics::getDeterminismReplayEligibleSlots() const
	{
		return DeterminismReplayEligibleSlots;
	}

	void SchedulerPhaseMetrics::setDeterminismReplayEligibleSlots(long long value)
	{
		DeterminismReplayEligibleSlots = value;
	}

	long long SchedulerPhaseMetrics::getDeterminismMaskedSlots() const
	{
		return DeterminismMaskedSlots;
	}

	void SchedulerPhaseMetrics::setDeterminismMaskedSlots(long long value)
	{
		DeterminismMaskedSlots = value;
	}

	long long SchedulerPhaseMetrics::getDeterminismEstimatedLostSlots() const
	{
		return DeterminismEstimatedLostSlots;
	}

	void SchedulerPhaseMetrics::setDeterminismEstimatedLostSlots(long long value)
	{
		DeterminismEstimatedLostSlots = value;
	}

	long long SchedulerPhaseMetrics::getDeterminismConstrainedCycles() const
	{
		return DeterminismConstrainedCycles;
	}

	void SchedulerPhaseMetrics::setDeterminismConstrainedCycles(long long value)
	{
		DeterminismConstrainedCycles = value;
	}

	long long SchedulerPhaseMetrics::getDomainIsolationProbeAttempts() const
	{
		return DomainIsolationProbeAttempts;
	}

	void SchedulerPhaseMetrics::setDomainIsolationProbeAttempts(long long value)
	{
		DomainIsolationProbeAttempts = value;
	}

	long long SchedulerPhaseMetrics::getDomainIsolationBlockedAttempts() const
	{
		return DomainIsolationBlockedAttempts;
	}

	void SchedulerPhaseMetrics::setDomainIsolationBlockedAttempts(long long value)
	{
		DomainIsolationBlockedAttempts = value;
	}

	long long SchedulerPhaseMetrics::getDomainIsolationCrossDomainBlocks() const
	{
		return DomainIsolationCrossDomainBlocks;
	}

	void SchedulerPhaseMetrics::setDomainIsolationCrossDomainBlocks(long long value)
	{
		DomainIsolationCrossDomainBlocks = value;
	}

	long long SchedulerPhaseMetrics::getDomainIsolationKernelToUserBlocks() const
	{
		return DomainIsolationKernelToUserBlocks;
	}

	void SchedulerPhaseMetrics::setDomainIsolationKernelToUserBlocks(long long value)
	{
		DomainIsolationKernelToUserBlocks = value;
	}

	long long SchedulerPhaseMetrics::getEligibilityMaskedCycles() const
	{
		return EligibilityMaskedCycles;
	}

	void SchedulerPhaseMetrics::setEligibilityMaskedCycles(long long value)
	{
		EligibilityMaskedCycles = value;
	}

	long long SchedulerPhaseMetrics::getEligibilityMaskedReadyCandidates() const
	{
		return EligibilityMaskedReadyCandidates;
	}

	void SchedulerPhaseMetrics::setEligibilityMaskedReadyCandidates(long long value)
	{
		EligibilityMaskedReadyCandidates = value;
	}

	unsigned char SchedulerPhaseMetrics::getLastEligibilityRequestedMask() const
	{
		return LastEligibilityRequestedMask;
	}

	void SchedulerPhaseMetrics::setLastEligibilityRequestedMask(unsigned char value)
	{
		LastEligibilityRequestedMask = value;
	}

	unsigned char SchedulerPhaseMetrics::getLastEligibilityNormalizedMask() const
	{
		return LastEligibilityNormalizedMask;
	}

	void SchedulerPhaseMetrics::setLastEligibilityNormalizedMask(unsigned char value)
	{
		LastEligibilityNormalizedMask = value;
	}

	unsigned char SchedulerPhaseMetrics::getLastEligibilityReadyPortMask() const
	{
		return LastEligibilityReadyPortMask;
	}

	void SchedulerPhaseMetrics::setLastEligibilityReadyPortMask(unsigned char value)
	{
		LastEligibilityReadyPortMask = value;
	}

	unsigned char SchedulerPhaseMetrics::getLastEligibilityVisibleReadyMask() const
	{
		return LastEligibilityVisibleReadyMask;
	}

	void SchedulerPhaseMetrics::setLastEligibilityVisibleReadyMask(unsigned char value)
	{
		LastEligibilityVisibleReadyMask = value;
	}

	unsigned char SchedulerPhaseMetrics::getLastEligibilityMaskedReadyMask() const
	{
		return LastEligibilityMaskedReadyMask;
	}

	void SchedulerPhaseMetrics::setLastEligibilityMaskedReadyMask(unsigned char value)
	{
		LastEligibilityMaskedReadyMask = value;
	}

	long long SchedulerPhaseMetrics::getClassTemplateReuseHits() const
	{
		return ClassTemplateReuseHits;
	}

	void SchedulerPhaseMetrics::setClassTemplateReuseHits(long long value)
	{
		ClassTemplateReuseHits = value;
	}

	long long SchedulerPhaseMetrics::getClassTemplateInvalidations() const
	{
		return ClassTemplateInvalidations;
	}

	void SchedulerPhaseMetrics::setClassTemplateInvalidations(long long value)
	{
		ClassTemplateInvalidations = value;
	}

	long long SchedulerPhaseMetrics::getTypedSlotFastPathAccepts() const
	{
		return TypedSlotFastPathAccepts;
	}

	void SchedulerPhaseMetrics::setTypedSlotFastPathAccepts(long long value)
	{
		TypedSlotFastPathAccepts = value;
	}

	long long SchedulerPhaseMetrics::getTypedSlotStandardPathAccepts() const
	{
		return TypedSlotStandardPathAccepts;
	}

	void SchedulerPhaseMetrics::setTypedSlotStandardPathAccepts(long long value)
	{
		TypedSlotStandardPathAccepts = value;
	}

	long long SchedulerPhaseMetrics::getTotalLaneBindings() const
	{
		return TotalLaneBindings;
	}

	void SchedulerPhaseMetrics::setTotalLaneBindings(long long value)
	{
		TotalLaneBindings = value;
	}

	long long SchedulerPhaseMetrics::getLaneReuseHits() const
	{
		return LaneReuseHits;
	}

	void SchedulerPhaseMetrics::setLaneReuseHits(long long value)
	{
		LaneReuseHits = value;
	}

	long long SchedulerPhaseMetrics::getLaneReuseMisses() const
	{
		return LaneReuseMisses;
	}

	void SchedulerPhaseMetrics::setLaneReuseMisses(long long value)
	{
		LaneReuseMisses = value;
	}

	long long SchedulerPhaseMetrics::getAluClassInjects() const
	{
		return AluClassInjects;
	}

	void SchedulerPhaseMetrics::setAluClassInjects(long long value)
	{
		AluClassInjects = value;
	}

	long long SchedulerPhaseMetrics::getLsuClassInjects() const
	{
		return LsuClassInjects;
	}

	void SchedulerPhaseMetrics::setLsuClassInjects(long long value)
	{
		LsuClassInjects = value;
	}

	long long SchedulerPhaseMetrics::getDmaStreamClassInjects() const
	{
		return DmaStreamClassInjects;
	}

	void SchedulerPhaseMetrics::setDmaStreamClassInjects(long long value)
	{
		DmaStreamClassInjects = value;
	}

	long long SchedulerPhaseMetrics::getMatrixTileStreamClassInjects() const
	{
		return MatrixTileStreamClassInjects;
	}

	void SchedulerPhaseMetrics::setMatrixTileStreamClassInjects(long long value)
	{
		MatrixTileStreamClassInjects = value;
	}

	long long SchedulerPhaseMetrics::getBranchControlInjects() const
	{
		return BranchControlInjects;
	}

	void SchedulerPhaseMetrics::setBranchControlInjects(long long value)
	{
		BranchControlInjects = value;
	}

	long long SchedulerPhaseMetrics::getHardPinnedInjects() const
	{
		return HardPinnedInjects;
	}

	void SchedulerPhaseMetrics::setHardPinnedInjects(long long value)
	{
		HardPinnedInjects = value;
	}

	long long SchedulerPhaseMetrics::getClassFlexibleInjects() const
	{
		return ClassFlexibleInjects;
	}

	void SchedulerPhaseMetrics::setClassFlexibleInjects(long long value)
	{
		ClassFlexibleInjects = value;
	}

	long long SchedulerPhaseMetrics::getNopAvoided() const
	{
		return NopAvoided;
	}

	void SchedulerPhaseMetrics::setNopAvoided(long long value)
	{
		NopAvoided = value;
	}

	long long SchedulerPhaseMetrics::getNopDueToPinnedConstraint() const
	{
		return NopDueToPinnedConstraint;
	}

	void SchedulerPhaseMetrics::setNopDueToPinnedConstraint(long long value)
	{
		NopDueToPinnedConstraint = value;
	}

	long long SchedulerPhaseMetrics::getNopDueToNoClassCapacity() const
	{
		return NopDueToNoClassCapacity;
	}

	void SchedulerPhaseMetrics::setNopDueToNoClassCapacity(long long value)
	{
		NopDueToNoClassCapacity = value;
	}

	long long SchedulerPhaseMetrics::getNopDueToResourceConflict() const
	{
		return NopDueToResourceConflict;
	}

	void SchedulerPhaseMetrics::setNopDueToResourceConflict(long long value)
	{
		NopDueToResourceConflict = value;
	}

	long long SchedulerPhaseMetrics::getNopDueToDynamicState() const
	{
		return NopDueToDynamicState;
	}

	void SchedulerPhaseMetrics::setNopDueToDynamicState(long long value)
	{
		NopDueToDynamicState = value;
	}

	long long SchedulerPhaseMetrics::getStaticClassOvercommitRejects() const
	{
		return StaticClassOvercommitRejects;
	}

	void SchedulerPhaseMetrics::setStaticClassOvercommitRejects(long long value)
	{
		StaticClassOvercommitRejects = value;
	}

	long long SchedulerPhaseMetrics::getDynamicClassExhaustionRejects() const
	{
		return DynamicClassExhaustionRejects;
	}

	void SchedulerPhaseMetrics::setDynamicClassExhaustionRejects(long long value)
	{
		DynamicClassExhaustionRejects = value;
	}

	long long SchedulerPhaseMetrics::getPinnedLaneConflicts() const
	{
		return PinnedLaneConflicts;
	}

	void SchedulerPhaseMetrics::setPinnedLaneConflicts(long long value)
	{
		PinnedLaneConflicts = value;
	}

	long long SchedulerPhaseMetrics::getLateBindingConflicts() const
	{
		return LateBindingConflicts;
	}

	void SchedulerPhaseMetrics::setLateBindingConflicts(long long value)
	{
		LateBindingConflicts = value;
	}

	long long SchedulerPhaseMetrics::getTypedSlotDomainRejects() const
	{
		return TypedSlotDomainRejects;
	}

	void SchedulerPhaseMetrics::setTypedSlotDomainRejects(long long value)
	{
		TypedSlotDomainRejects = value;
	}

	long long SchedulerPhaseMetrics::getSmtOwnerContextGuardRejects() const
	{
		return SmtOwnerContextGuardRejects;
	}

	void SchedulerPhaseMetrics::setSmtOwnerContextGuardRejects(long long value)
	{
		SmtOwnerContextGuardRejects = value;
	}

	long long SchedulerPhaseMetrics::getSmtDomainGuardRejects() const
	{
		return SmtDomainGuardRejects;
	}

	void SchedulerPhaseMetrics::setSmtDomainGuardRejects(long long value)
	{
		SmtDomainGuardRejects = value;
	}

	long long SchedulerPhaseMetrics::getSmtBoundaryGuardRejects() const
	{
		return SmtBoundaryGuardRejects;
	}

	void SchedulerPhaseMetrics::setSmtBoundaryGuardRejects(long long value)
	{
		SmtBoundaryGuardRejects = value;
	}

	long long SchedulerPhaseMetrics::getSmtSharedResourceCertificateRejects() const
	{
		return SmtSharedResourceCertificateRejects;
	}

	void SchedulerPhaseMetrics::setSmtSharedResourceCertificateRejects(long long value)
	{
		SmtSharedResourceCertificateRejects = value;
	}

	long long SchedulerPhaseMetrics::getSmtRegisterGroupCertificateRejects() const
	{
		return SmtRegisterGroupCertificateRejects;
	}

	void SchedulerPhaseMetrics::setSmtRegisterGroupCertificateRejects(long long value)
	{
		SmtRegisterGroupCertificateRejects = value;
	}

	RejectKind SchedulerPhaseMetrics::getLastSmtLegalityRejectKind() const
	{
		return LastSmtLegalityRejectKind;
	}

	void SchedulerPhaseMetrics::setLastSmtLegalityRejectKind(RejectKind value)
	{
		LastSmtLegalityRejectKind = value;
	}

	LegalityAuthoritySource SchedulerPhaseMetrics::getLastSmtLegalityAuthoritySource() const
	{
		return LastSmtLegalityAuthoritySource;
	}

	void SchedulerPhaseMetrics::setLastSmtLegalityAuthoritySource(LegalityAuthoritySource value)
	{
		LastSmtLegalityAuthoritySource = value;
	}

	long long SchedulerPhaseMetrics::getSmtLegalityRejectByAluClass() const
	{
		return SmtLegalityRejectByAluClass;
	}

	void SchedulerPhaseMetrics::setSmtLegalityRejectByAluClass(long long value)
	{
		SmtLegalityRejectByAluClass = value;
	}

	long long SchedulerPhaseMetrics::getSmtLegalityRejectByLsuClass() const
	{
		return SmtLegalityRejectByLsuClass;
	}

	void SchedulerPhaseMetrics::setSmtLegalityRejectByLsuClass(long long value)
	{
		SmtLegalityRejectByLsuClass = value;
	}

	long long SchedulerPhaseMetrics::getSmtLegalityRejectByDmaStreamClass() const
	{
		return SmtLegalityRejectByDmaStreamClass;
	}

	void SchedulerPhaseMetrics::setSmtLegalityRejectByDmaStreamClass(long long value)
	{
		SmtLegalityRejectByDmaStreamClass = value;
	}

	long long SchedulerPhaseMetrics::getSmtLegalityRejectByMatrixTileStreamClass() const
	{
		return SmtLegalityRejectByMatrixTileStreamClass;
	}

	void SchedulerPhaseMetrics::setSmtLegalityRejectByMatrixTileStreamClass(long long value)
	{
		SmtLegalityRejectByMatrixTileStreamClass = value;
	}

	long long SchedulerPhaseMetrics::getSmtLegalityRejectByBranchControl() const
	{
		return SmtLegalityRejectByBranchControl;
	}

	void SchedulerPhaseMetrics::setSmtLegalityRejectByBranchControl(long long value)
	{
		SmtLegalityRejectByBranchControl = value;
	}

	long long SchedulerPhaseMetrics::getSmtLegalityRejectBySystemSingleton() const
	{
		return SmtLegalityRejectBySystemSingleton;
	}

	void SchedulerPhaseMetrics::setSmtLegalityRejectBySystemSingleton(long long value)
	{
		SmtLegalityRejectBySystemSingleton = value;
	}

	long long SchedulerPhaseMetrics::getClassTemplateDomainInvalidations() const
	{
		return ClassTemplateDomainInvalidations;
	}

	void SchedulerPhaseMetrics::setClassTemplateDomainInvalidations(long long value)
	{
		ClassTemplateDomainInvalidations = value;
	}

	long long SchedulerPhaseMetrics::getClassTemplateCapacityMismatchInvalidations() const
	{
		return ClassTemplateCapacityMismatchInvalidations;
	}

	void SchedulerPhaseMetrics::setClassTemplateCapacityMismatchInvalidations(long long value)
	{
		ClassTemplateCapacityMismatchInvalidations = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRejectByAluClass() const
	{
		return CertificateRejectByAluClass;
	}

	void SchedulerPhaseMetrics::setCertificateRejectByAluClass(long long value)
	{
		CertificateRejectByAluClass = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRejectByLsuClass() const
	{
		return CertificateRejectByLsuClass;
	}

	void SchedulerPhaseMetrics::setCertificateRejectByLsuClass(long long value)
	{
		CertificateRejectByLsuClass = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRejectByDmaStreamClass() const
	{
		return CertificateRejectByDmaStreamClass;
	}

	void SchedulerPhaseMetrics::setCertificateRejectByDmaStreamClass(long long value)
	{
		CertificateRejectByDmaStreamClass = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRejectByMatrixTileStreamClass() const
	{
		return CertificateRejectByMatrixTileStreamClass;
	}

	void SchedulerPhaseMetrics::setCertificateRejectByMatrixTileStreamClass(long long value)
	{
		CertificateRejectByMatrixTileStreamClass = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRejectByBranchControl() const
	{
		return CertificateRejectByBranchControl;
	}

	void SchedulerPhaseMetrics::setCertificateRejectByBranchControl(long long value)
	{
		CertificateRejectByBranchControl = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRejectBySystemSingleton() const
	{
		return CertificateRejectBySystemSingleton;
	}

	void SchedulerPhaseMetrics::setCertificateRejectBySystemSingleton(long long value)
	{
		CertificateRejectBySystemSingleton = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRegGroupConflictVT0() const
	{
		return CertificateRegGroupConflictVT0;
	}

	void SchedulerPhaseMetrics::setCertificateRegGroupConflictVT0(long long value)
	{
		CertificateRegGroupConflictVT0 = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRegGroupConflictVT1() const
	{
		return CertificateRegGroupConflictVT1;
	}

	void SchedulerPhaseMetrics::setCertificateRegGroupConflictVT1(long long value)
	{
		CertificateRegGroupConflictVT1 = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRegGroupConflictVT2() const
	{
		return CertificateRegGroupConflictVT2;
	}

	void SchedulerPhaseMetrics::setCertificateRegGroupConflictVT2(long long value)
	{
		CertificateRegGroupConflictVT2 = value;
	}

	long long SchedulerPhaseMetrics::getCertificateRegGroupConflictVT3() const
	{
		return CertificateRegGroupConflictVT3;
	}

	void SchedulerPhaseMetrics::setCertificateRegGroupConflictVT3(long long value)
	{
		CertificateRegGroupConflictVT3 = value;
	}

	long long SchedulerPhaseMetrics::getRejectionsVT0() const
	{
		return RejectionsVT0;
	}

	void SchedulerPhaseMetrics::setRejectionsVT0(long long value)
	{
		RejectionsVT0 = value;
	}

	long long SchedulerPhaseMetrics::getRejectionsVT1() const
	{
		return RejectionsVT1;
	}

	void SchedulerPhaseMetrics::setRejectionsVT1(long long value)
	{
		RejectionsVT1 = value;
	}

	long long SchedulerPhaseMetrics::getRejectionsVT2() const
	{
		return RejectionsVT2;
	}

	void SchedulerPhaseMetrics::setRejectionsVT2(long long value)
	{
		RejectionsVT2 = value;
	}

	long long SchedulerPhaseMetrics::getRejectionsVT3() const
	{
		return RejectionsVT3;
	}

	void SchedulerPhaseMetrics::setRejectionsVT3(long long value)
	{
		RejectionsVT3 = value;
	}

	long long SchedulerPhaseMetrics::getRegGroupConflictsVT0() const
	{
		return RegGroupConflictsVT0;
	}

	void SchedulerPhaseMetrics::setRegGroupConflictsVT0(long long value)
	{
		RegGroupConflictsVT0 = value;
	}

	long long SchedulerPhaseMetrics::getRegGroupConflictsVT1() const
	{
		return RegGroupConflictsVT1;
	}

	void SchedulerPhaseMetrics::setRegGroupConflictsVT1(long long value)
	{
		RegGroupConflictsVT1 = value;
	}

	long long SchedulerPhaseMetrics::getRegGroupConflictsVT2() const
	{
		return RegGroupConflictsVT2;
	}

	void SchedulerPhaseMetrics::setRegGroupConflictsVT2(long long value)
	{
		RegGroupConflictsVT2 = value;
	}

	long long SchedulerPhaseMetrics::getRegGroupConflictsVT3() const
	{
		return RegGroupConflictsVT3;
	}

	void SchedulerPhaseMetrics::setRegGroupConflictsVT3(long long value)
	{
		RegGroupConflictsVT3 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank0() const
	{
		return BankPendingRejectBank0;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank0(long long value)
	{
		BankPendingRejectBank0 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank1() const
	{
		return BankPendingRejectBank1;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank1(long long value)
	{
		BankPendingRejectBank1 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank2() const
	{
		return BankPendingRejectBank2;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank2(long long value)
	{
		BankPendingRejectBank2 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank3() const
	{
		return BankPendingRejectBank3;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank3(long long value)
	{
		BankPendingRejectBank3 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank4() const
	{
		return BankPendingRejectBank4;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank4(long long value)
	{
		BankPendingRejectBank4 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank5() const
	{
		return BankPendingRejectBank5;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank5(long long value)
	{
		BankPendingRejectBank5 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank6() const
	{
		return BankPendingRejectBank6;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank6(long long value)
	{
		BankPendingRejectBank6 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank7() const
	{
		return BankPendingRejectBank7;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank7(long long value)
	{
		BankPendingRejectBank7 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank8() const
	{
		return BankPendingRejectBank8;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank8(long long value)
	{
		BankPendingRejectBank8 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank9() const
	{
		return BankPendingRejectBank9;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank9(long long value)
	{
		BankPendingRejectBank9 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank10() const
	{
		return BankPendingRejectBank10;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank10(long long value)
	{
		BankPendingRejectBank10 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank11() const
	{
		return BankPendingRejectBank11;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank11(long long value)
	{
		BankPendingRejectBank11 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank12() const
	{
		return BankPendingRejectBank12;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank12(long long value)
	{
		BankPendingRejectBank12 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank13() const
	{
		return BankPendingRejectBank13;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank13(long long value)
	{
		BankPendingRejectBank13 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank14() const
	{
		return BankPendingRejectBank14;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank14(long long value)
	{
		BankPendingRejectBank14 = value;
	}

	long long SchedulerPhaseMetrics::getBankPendingRejectBank15() const
	{
		return BankPendingRejectBank15;
	}

	void SchedulerPhaseMetrics::setBankPendingRejectBank15(long long value)
	{
		BankPendingRejectBank15 = value;
	}

	long long SchedulerPhaseMetrics::getMemoryClusteringEvents() const
	{
		return MemoryClusteringEvents;
	}

	void SchedulerPhaseMetrics::setMemoryClusteringEvents(long long value)
	{
		MemoryClusteringEvents = value;
	}

	long long SchedulerPhaseMetrics::getTypedSlotHardwareBudgetRejects() const
	{
		return TypedSlotHardwareBudgetRejects;
	}

	void SchedulerPhaseMetrics::setTypedSlotHardwareBudgetRejects(long long value)
	{
		TypedSlotHardwareBudgetRejects = value;
	}

	long long SchedulerPhaseMetrics::getTypedSlotAssistQuotaRejects() const
	{
		return TypedSlotAssistQuotaRejects;
	}

	void SchedulerPhaseMetrics::setTypedSlotAssistQuotaRejects(long long value)
	{
		TypedSlotAssistQuotaRejects = value;
	}

	long long SchedulerPhaseMetrics::getTypedSlotAssistBackpressureRejects() const
	{
		return TypedSlotAssistBackpressureRejects;
	}

	void SchedulerPhaseMetrics::setTypedSlotAssistBackpressureRejects(long long value)
	{
		TypedSlotAssistBackpressureRejects = value;
	}

	long long SchedulerPhaseMetrics::getSerializingBoundaryRejects() const
	{
		return SerializingBoundaryRejects;
	}

	void SchedulerPhaseMetrics::setSerializingBoundaryRejects(long long value)
	{
		SerializingBoundaryRejects = value;
	}

	long long SchedulerPhaseMetrics::getSerializingEpochCount() const
	{
		return SerializingEpochCount;
	}

	void SchedulerPhaseMetrics::setSerializingEpochCount(long long value)
	{
		SerializingEpochCount = value;
	}

	long long SchedulerPhaseMetrics::getAssistNominations() const
	{
		return AssistNominations;
	}

	void SchedulerPhaseMetrics::setAssistNominations(long long value)
	{
		AssistNominations = value;
	}

	long long SchedulerPhaseMetrics::getAssistInjections() const
	{
		return AssistInjections;
	}

	void SchedulerPhaseMetrics::setAssistInjections(long long value)
	{
		AssistInjections = value;
	}

	long long SchedulerPhaseMetrics::getAssistRejects() const
	{
		return AssistRejects;
	}

	void SchedulerPhaseMetrics::setAssistRejects(long long value)
	{
		AssistRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistBoundaryRejects() const
	{
		return AssistBoundaryRejects;
	}

	void SchedulerPhaseMetrics::setAssistBoundaryRejects(long long value)
	{
		AssistBoundaryRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInvalidations() const
	{
		return AssistInvalidations;
	}

	void SchedulerPhaseMetrics::setAssistInvalidations(long long value)
	{
		AssistInvalidations = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreNominations() const
	{
		return AssistInterCoreNominations;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreNominations(long long value)
	{
		AssistInterCoreNominations = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreInjections() const
	{
		return AssistInterCoreInjections;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreInjections(long long value)
	{
		AssistInterCoreInjections = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreRejects() const
	{
		return AssistInterCoreRejects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreRejects(long long value)
	{
		AssistInterCoreRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreDomainRejects() const
	{
		return AssistInterCoreDomainRejects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreDomainRejects(long long value)
	{
		AssistInterCoreDomainRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCorePodLocalInjections() const
	{
		return AssistInterCorePodLocalInjections;
	}

	void SchedulerPhaseMetrics::setAssistInterCorePodLocalInjections(long long value)
	{
		AssistInterCorePodLocalInjections = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreCrossPodInjections() const
	{
		return AssistInterCoreCrossPodInjections;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreCrossPodInjections(long long value)
	{
		AssistInterCoreCrossPodInjections = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCorePodLocalRejects() const
	{
		return AssistInterCorePodLocalRejects;
	}

	void SchedulerPhaseMetrics::setAssistInterCorePodLocalRejects(long long value)
	{
		AssistInterCorePodLocalRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreCrossPodRejects() const
	{
		return AssistInterCoreCrossPodRejects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreCrossPodRejects(long long value)
	{
		AssistInterCoreCrossPodRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCorePodLocalDomainRejects() const
	{
		return AssistInterCorePodLocalDomainRejects;
	}

	void SchedulerPhaseMetrics::setAssistInterCorePodLocalDomainRejects(long long value)
	{
		AssistInterCorePodLocalDomainRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreCrossPodDomainRejects() const
	{
		return AssistInterCoreCrossPodDomainRejects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreCrossPodDomainRejects(long long value)
	{
		AssistInterCoreCrossPodDomainRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreSameVtVectorInjects() const
	{
		return AssistInterCoreSameVtVectorInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreSameVtVectorInjects(long long value)
	{
		AssistInterCoreSameVtVectorInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreDonorVtVectorInjects() const
	{
		return AssistInterCoreDonorVtVectorInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreDonorVtVectorInjects(long long value)
	{
		AssistInterCoreDonorVtVectorInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreSameVtVectorWritebackInjects() const
	{
		return AssistInterCoreSameVtVectorWritebackInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreSameVtVectorWritebackInjects(long long value)
	{
		AssistInterCoreSameVtVectorWritebackInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreDonorVtVectorWritebackInjects() const
	{
		return AssistInterCoreDonorVtVectorWritebackInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreDonorVtVectorWritebackInjects(long long value)
	{
		AssistInterCoreDonorVtVectorWritebackInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreLane6DefaultStoreDonorPrefetchInjects() const
	{
		return AssistInterCoreLane6DefaultStoreDonorPrefetchInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreLane6DefaultStoreDonorPrefetchInjects(long long value)
	{
		AssistInterCoreLane6DefaultStoreDonorPrefetchInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreLane6HotLoadDonorPrefetchInjects() const
	{
		return AssistInterCoreLane6HotLoadDonorPrefetchInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreLane6HotLoadDonorPrefetchInjects(long long value)
	{
		AssistInterCoreLane6HotLoadDonorPrefetchInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreLane6HotStoreDonorPrefetchInjects() const
	{
		return AssistInterCoreLane6HotStoreDonorPrefetchInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreLane6HotStoreDonorPrefetchInjects(long long value)
	{
		AssistInterCoreLane6HotStoreDonorPrefetchInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreLane6DonorPrefetchInjects() const
	{
		return AssistInterCoreLane6DonorPrefetchInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreLane6DonorPrefetchInjects(long long value)
	{
		AssistInterCoreLane6DonorPrefetchInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreLane6ColdStoreLdsaInjects() const
	{
		return AssistInterCoreLane6ColdStoreLdsaInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreLane6ColdStoreLdsaInjects(long long value)
	{
		AssistInterCoreLane6ColdStoreLdsaInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistInterCoreLane6LdsaInjects() const
	{
		return AssistInterCoreLane6LdsaInjects;
	}

	void SchedulerPhaseMetrics::setAssistInterCoreLane6LdsaInjects(long long value)
	{
		AssistInterCoreLane6LdsaInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistQuotaRejects() const
	{
		return AssistQuotaRejects;
	}

	void SchedulerPhaseMetrics::setAssistQuotaRejects(long long value)
	{
		AssistQuotaRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistQuotaIssueRejects() const
	{
		return AssistQuotaIssueRejects;
	}

	void SchedulerPhaseMetrics::setAssistQuotaIssueRejects(long long value)
	{
		AssistQuotaIssueRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistQuotaLineRejects() const
	{
		return AssistQuotaLineRejects;
	}

	void SchedulerPhaseMetrics::setAssistQuotaLineRejects(long long value)
	{
		AssistQuotaLineRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistQuotaLinesReserved() const
	{
		return AssistQuotaLinesReserved;
	}

	void SchedulerPhaseMetrics::setAssistQuotaLinesReserved(long long value)
	{
		AssistQuotaLinesReserved = value;
	}

	long long SchedulerPhaseMetrics::getAssistBackpressureRejects() const
	{
		return AssistBackpressureRejects;
	}

	void SchedulerPhaseMetrics::setAssistBackpressureRejects(long long value)
	{
		AssistBackpressureRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistBackpressureOuterCapRejects() const
	{
		return AssistBackpressureOuterCapRejects;
	}

	void SchedulerPhaseMetrics::setAssistBackpressureOuterCapRejects(long long value)
	{
		AssistBackpressureOuterCapRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistBackpressureMshrRejects() const
	{
		return AssistBackpressureMshrRejects;
	}

	void SchedulerPhaseMetrics::setAssistBackpressureMshrRejects(long long value)
	{
		AssistBackpressureMshrRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistBackpressureDmaSrfRejects() const
	{
		return AssistBackpressureDmaSrfRejects;
	}

	void SchedulerPhaseMetrics::setAssistBackpressureDmaSrfRejects(long long value)
	{
		AssistBackpressureDmaSrfRejects = value;
	}

	long long SchedulerPhaseMetrics::getAssistDonorPrefetchInjects() const
	{
		return AssistDonorPrefetchInjects;
	}

	void SchedulerPhaseMetrics::setAssistDonorPrefetchInjects(long long value)
	{
		AssistDonorPrefetchInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistLdsaInjects() const
	{
		return AssistLdsaInjects;
	}

	void SchedulerPhaseMetrics::setAssistLdsaInjects(long long value)
	{
		AssistLdsaInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistVdsaInjects() const
	{
		return AssistVdsaInjects;
	}

	void SchedulerPhaseMetrics::setAssistVdsaInjects(long long value)
	{
		AssistVdsaInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistSameVtInjects() const
	{
		return AssistSameVtInjects;
	}

	void SchedulerPhaseMetrics::setAssistSameVtInjects(long long value)
	{
		AssistSameVtInjects = value;
	}

	long long SchedulerPhaseMetrics::getAssistDonorVtInjects() const
	{
		return AssistDonorVtInjects;
	}

	void SchedulerPhaseMetrics::setAssistDonorVtInjects(long long value)
	{
		AssistDonorVtInjects = value;
	}

	AssistInvalidationReason SchedulerPhaseMetrics::getLastAssistInvalidationReason() const
	{
		return LastAssistInvalidationReason;
	}

	void SchedulerPhaseMetrics::setLastAssistInvalidationReason(AssistInvalidationReason value)
	{
		LastAssistInvalidationReason = value;
	}

	unsigned long long SchedulerPhaseMetrics::getLastAssistOwnershipSignature() const
	{
		return LastAssistOwnershipSignature;
	}

	void SchedulerPhaseMetrics::setLastAssistOwnershipSignature(unsigned long long value)
	{
		LastAssistOwnershipSignature = value;
	}

	const std::vector<std::shared_ptr<LoopPhaseClassProfile>> SchedulerPhaseMetrics::getLoopPhaseProfiles() const
	{
		return LoopPhaseProfiles;
	}

	void SchedulerPhaseMetrics::setLoopPhaseProfiles(const std::vector<std::shared_ptr<LoopPhaseClassProfile>> &value)
	{
		LoopPhaseProfiles = value;
	}
}
