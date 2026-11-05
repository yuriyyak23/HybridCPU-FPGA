#include "ReplayPhaseSubstrate.h"

#include "../../Diagnostics/LoopPhaseClassProfile.h"
#include <any>

using namespace YAKSys_Hybrid_CPU::Core::Diagnostics;

namespace YAKSys_Hybrid_CPU::Core
{

	unsigned char ClassCapacityTemplate::getAluFree() const
	{
		return AluFree;
	}

	unsigned char ClassCapacityTemplate::getLsuFree() const
	{
		return LsuFree;
	}

	unsigned char ClassCapacityTemplate::getDmaStreamFree() const
	{
		return DmaStreamFree;
	}

	unsigned char ClassCapacityTemplate::getMatrixTileStreamFree() const
	{
		return MatrixTileStreamFree;
	}

	unsigned char ClassCapacityTemplate::getBranchControlFree() const
	{
		return BranchControlFree;
	}

	unsigned char ClassCapacityTemplate::getSystemSingletonFree() const
	{
		return SystemSingletonFree;
	}

	ClassCapacityTemplate::ClassCapacityTemplate(SlotClassCapacityState state)
	{
		AluFree = ClampToByte(state.GetFreeCapacity(SlotClass::AluClass));
		LsuFree = ClampToByte(state.GetFreeCapacity(SlotClass::LsuClass));
		DmaStreamFree = ClampToByte(state.GetFreeCapacity(SlotClass::DmaStreamClass));
		MatrixTileStreamFree = ClampToByte(state.GetFreeCapacity(SlotClass::MatrixTileStreamClass));
		BranchControlFree = ClampToByte(state.GetFreeCapacity(SlotClass::BranchControl));
		SystemSingletonFree = ClampToByte(state.GetFreeCapacity(SlotClass::SystemSingleton));
	}

	bool ClassCapacityTemplate::IsCompatibleWith(SlotClassCapacityState current) const
	{
		return current.GetFreeCapacity(SlotClass::AluClass) >= getAluFree() && current.GetFreeCapacity(SlotClass::LsuClass) >= getLsuFree() && current.GetFreeCapacity(SlotClass::DmaStreamClass) >= getDmaStreamFree() && current.GetFreeCapacity(SlotClass::MatrixTileStreamClass) >= getMatrixTileStreamFree() && current.GetFreeCapacity(SlotClass::BranchControl) >= getBranchControlFree() && current.GetFreeCapacity(SlotClass::SystemSingleton) >= getSystemSingletonFree();
	}

	inline int ClassCapacityTemplate::GetCapturedFree(SlotClass slotClass) const
	{
		int temp_var103;
		switch (slotClass)
		{
			case SlotClass::AluClass:
				temp_var103 = getAluFree();
				break;
			case SlotClass::LsuClass:
				temp_var103 = getLsuFree();
				break;
			case SlotClass::DmaStreamClass:
				temp_var103 = getDmaStreamFree();
				break;
			case SlotClass::MatrixTileStreamClass:
				temp_var103 = getMatrixTileStreamFree();
				break;
			case SlotClass::BranchControl:
				temp_var103 = getBranchControlFree();
				break;
			case SlotClass::SystemSingleton:
				temp_var103 = getSystemSingletonFree();
				break;
			default:
				temp_var103 = 0;
				break;
		}
		return temp_var103;
	}

	bool ClassCapacityTemplate::Equals(ClassCapacityTemplate other) const
	{
		return getAluFree() == other.getAluFree() && getLsuFree() == other.getLsuFree() && getDmaStreamFree() == other.getDmaStreamFree() && getMatrixTileStreamFree() == other.getMatrixTileStreamFree() && getBranchControlFree() == other.getBranchControlFree() && getSystemSingletonFree() == other.getSystemSingletonFree();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool ClassCapacityTemplate::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<ClassCapacityTemplate>(&obj);
		return other != nullptr && Equals(*other);
	}

	int ClassCapacityTemplate::GetHashCode() const
	{
		int hash = static_cast<int>(0x5B4D8A13);
		hash = (hash * 397) ^ getAluFree();
		hash = (hash * 397) ^ getLsuFree();
		hash = (hash * 397) ^ getDmaStreamFree();
		hash = (hash * 397) ^ getMatrixTileStreamFree();
		hash = (hash * 397) ^ getBranchControlFree();
		hash = (hash * 397) ^ getSystemSingletonFree();
		return hash;
	}

	inline unsigned char ClassCapacityTemplate::ClampToByte(int value)
	{
		return value <= 0 ? static_cast<unsigned char>(0) : static_cast<unsigned char>(value);
	}

	TemplateBudget::TemplateBudget(ClassCapacityTemplate template_Keyword)
	{
		_aluRemaining = template_Keyword.getAluFree();
		_lsuRemaining = template_Keyword.getLsuFree();
		_dmaStreamRemaining = template_Keyword.getDmaStreamFree();
		_matrixTileStreamRemaining = template_Keyword.getMatrixTileStreamFree();
		_branchControlRemaining = template_Keyword.getBranchControlFree();
		_systemSingletonRemaining = template_Keyword.getSystemSingletonFree();
	}

	int TemplateBudget::GetRemaining(SlotClass slotClass) const
	{
		int temp_var104;
		switch (slotClass)
		{
			case SlotClass::AluClass:
				temp_var104 = _aluRemaining;
				break;
			case SlotClass::LsuClass:
				temp_var104 = _lsuRemaining;
				break;
			case SlotClass::DmaStreamClass:
				temp_var104 = _dmaStreamRemaining;
				break;
			case SlotClass::MatrixTileStreamClass:
				temp_var104 = _matrixTileStreamRemaining;
				break;
			case SlotClass::BranchControl:
				temp_var104 = _branchControlRemaining;
				break;
			case SlotClass::SystemSingleton:
				temp_var104 = _systemSingletonRemaining;
				break;
			default:
				temp_var104 = 0;
				break;
		}
		return temp_var104;
	}

	void TemplateBudget::Decrement(SlotClass slotClass)
	{
		switch (slotClass)
		{
			case SlotClass::AluClass:
				if (_aluRemaining > 0)
				{
					_aluRemaining--;
				}
				break;
			case SlotClass::LsuClass:
				if (_lsuRemaining > 0)
				{
					_lsuRemaining--;
				}
				break;
			case SlotClass::DmaStreamClass:
				if (_dmaStreamRemaining > 0)
				{
					_dmaStreamRemaining--;
				}
				break;
			case SlotClass::MatrixTileStreamClass:
				if (_matrixTileStreamRemaining > 0)
				{
					_matrixTileStreamRemaining--;
				}
				break;
			case SlotClass::BranchControl:
				if (_branchControlRemaining > 0)
				{
					_branchControlRemaining--;
				}
				break;
			case SlotClass::SystemSingleton:
				if (_systemSingletonRemaining > 0)
				{
					_systemSingletonRemaining--;
				}
				break;
		}
	}

	ReplayPhaseKey::ReplayPhaseKey(unsigned long long epochId, unsigned long long cachedPc, unsigned char stableDonorMask, int validSlotCount)
	{
		EpochId = epochId;
		CachedPc = cachedPc;
		StableDonorMask = stableDonorMask;
		ValidSlotCount = validSlotCount;
		ClassTemplate = YAKSys_Hybrid_CPU::Core::ClassCapacityTemplate();
		DomainScopeId = 0;
	}

	ReplayPhaseKey::ReplayPhaseKey(unsigned long long epochId, unsigned long long cachedPc, unsigned char stableDonorMask, int validSlotCount, ClassCapacityTemplate classTemplate, int domainScopeId)
	{
		EpochId = epochId;
		CachedPc = cachedPc;
		StableDonorMask = stableDonorMask;
		ValidSlotCount = validSlotCount;
		ClassTemplate = classTemplate;
		DomainScopeId = domainScopeId;
	}

	unsigned long long ReplayPhaseKey::getEpochId() const
	{
		return EpochId;
	}

	unsigned long long ReplayPhaseKey::getCachedPc() const
	{
		return CachedPc;
	}

	unsigned char ReplayPhaseKey::getStableDonorMask() const
	{
		return StableDonorMask;
	}

	int ReplayPhaseKey::getValidSlotCount() const
	{
		return ValidSlotCount;
	}

	ClassCapacityTemplate ReplayPhaseKey::getClassTemplate() const
	{
		return ClassTemplate;
	}

	int ReplayPhaseKey::getDomainScopeId() const
	{
		return DomainScopeId;
	}

	bool ReplayPhaseKey::isValid() const
	{
		return getEpochId() != 0;
	}

	bool ReplayPhaseKey::Equals(ReplayPhaseKey other) const
	{
		return getEpochId() == other.getEpochId() && getCachedPc() == other.getCachedPc() && getStableDonorMask() == other.getStableDonorMask() && getValidSlotCount() == other.getValidSlotCount();
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool ReplayPhaseKey::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<ReplayPhaseKey>(&obj);
		return other != nullptr && Equals(*other);
	}

	int ReplayPhaseKey::GetHashCode() const
	{
		int hash = static_cast<int>(0x5B4D8A13);
		hash = (hash * 397) ^ static_cast<int>(getEpochId());
		hash = (hash * 397) ^ static_cast<int>(getCachedPc());
		hash = (hash * 397) ^ getStableDonorMask();
		hash = (hash * 397) ^ getValidSlotCount();
		return hash;
	}

	ReplayPhaseContext::ReplayPhaseContext(bool isActive, unsigned long long epochId, unsigned long long cachedPc, unsigned long long epochLength, unsigned long long completedReplays, int validSlotCount, unsigned char stableDonorMask, ReplayPhaseInvalidationReason lastInvalidationReason)
	{
		IsActive = isActive;
		EpochId = epochId;
		CachedPc = cachedPc;
		EpochLength = epochLength;
		CompletedReplays = completedReplays;
		ValidSlotCount = validSlotCount;
		StableDonorMask = stableDonorMask;
		LastInvalidationReason = lastInvalidationReason;
	}

	bool ReplayPhaseContext::isActive() const
	{
		return IsActive;
	}

	unsigned long long ReplayPhaseContext::getEpochId() const
	{
		return EpochId;
	}

	unsigned long long ReplayPhaseContext::getCachedPc() const
	{
		return CachedPc;
	}

	unsigned long long ReplayPhaseContext::getEpochLength() const
	{
		return EpochLength;
	}

	unsigned long long ReplayPhaseContext::getCompletedReplays() const
	{
		return CompletedReplays;
	}

	int ReplayPhaseContext::getValidSlotCount() const
	{
		return ValidSlotCount;
	}

	unsigned char ReplayPhaseContext::getStableDonorMask() const
	{
		return StableDonorMask;
	}

	ReplayPhaseInvalidationReason ReplayPhaseContext::getLastInvalidationReason() const
	{
		return LastInvalidationReason;
	}

	ReplayPhaseKey ReplayPhaseContext::getKey() const
	{
		return ReplayPhaseKey(getEpochId(), getCachedPc(), getStableDonorMask(), getValidSlotCount());
	}

	int ReplayPhaseContext::getStableDonorSlotCount() const
	{
		return CountBits(getStableDonorMask());
	}

	bool ReplayPhaseContext::getHasStableDonorStructure() const
	{
		return isActive() && getCompletedReplays() > 0 && getStableDonorMask() != 0;
	}

	bool ReplayPhaseContext::getCanReusePhaseCertificate() const
	{
		return isActive() && getKey().isValid();
	}

	bool ReplayPhaseContext::IsDonorStable(int slotIndex) const
	{
		if (static_cast<unsigned int>(slotIndex) >= 8)
		{
			return false;
		}

		return (getStableDonorMask() & (1 << slotIndex)) != 0;
	}

	bool ReplayPhaseContext::Matches(ReplayPhaseKey key) const
	{
		return isActive() && getKey().Equals(key);
	}

	int ReplayPhaseContext::CountBits(unsigned char value)
	{
		int count = 0;
		unsigned char remaining = value;
		while (remaining != 0)
		{
			count += remaining & 1;
			remaining >>= 1;
		}

		return count;
	}

	unsigned long long ReplayPhaseMetrics::getReplayEpochCount() const
	{
		return ReplayEpochCount;
	}

	void ReplayPhaseMetrics::setReplayEpochCount(unsigned long long value)
	{
		ReplayEpochCount = value;
	}

	unsigned long long ReplayPhaseMetrics::getTotalEpochLength() const
	{
		return TotalEpochLength;
	}

	void ReplayPhaseMetrics::setTotalEpochLength(unsigned long long value)
	{
		TotalEpochLength = value;
	}

	unsigned long long ReplayPhaseMetrics::getStableDonorSlotSamples() const
	{
		return StableDonorSlotSamples;
	}

	void ReplayPhaseMetrics::setStableDonorSlotSamples(unsigned long long value)
	{
		StableDonorSlotSamples = value;
	}

	unsigned long long ReplayPhaseMetrics::getTotalReplaySlotSamples() const
	{
		return TotalReplaySlotSamples;
	}

	void ReplayPhaseMetrics::setTotalReplaySlotSamples(unsigned long long value)
	{
		TotalReplaySlotSamples = value;
	}

	unsigned long long ReplayPhaseMetrics::getDeterministicTransitionCount() const
	{
		return DeterministicTransitionCount;
	}

	void ReplayPhaseMetrics::setDeterministicTransitionCount(unsigned long long value)
	{
		DeterministicTransitionCount = value;
	}

	double ReplayPhaseMetrics::getAverageEpochLength() const
	{
		return getReplayEpochCount() == 0 ? 0.0 : static_cast<double>(getTotalEpochLength()) / getReplayEpochCount();
	}

	double ReplayPhaseMetrics::getStableDonorSlotRatio() const
	{
		return getTotalReplaySlotSamples() == 0 ? 0.0 : static_cast<double>(getStableDonorSlotSamples()) / getTotalReplaySlotSamples();
	}

	LegalityCertificateCacheTelemetry::LegalityCertificateCacheTelemetry()
		: LegalityCertificateCacheTelemetry(0, 0, 0, 0, 0, 0, ReplayPhaseInvalidationReason::None)
	{
	}

	LegalityCertificateCacheTelemetry::LegalityCertificateCacheTelemetry(long long readyHitsDelta, long long readyMissesDelta, long long estimatedChecksSavedDelta, long long invalidationEventsDelta, long long mutationInvalidationsDelta, long long phaseMismatchInvalidationsDelta, ReplayPhaseInvalidationReason lastInvalidationReason)
		: ReadyHitsDelta(readyHitsDelta), ReadyMissesDelta(readyMissesDelta), EstimatedChecksSavedDelta(estimatedChecksSavedDelta),
		  InvalidationEventsDelta(invalidationEventsDelta), MutationInvalidationsDelta(mutationInvalidationsDelta),
		  PhaseMismatchInvalidationsDelta(phaseMismatchInvalidationsDelta), LastInvalidationReason(lastInvalidationReason)
	{
	}

	long long LegalityCertificateCacheTelemetry::getReadyHitsDelta() const
	{
		return ReadyHitsDelta;
	}

	long long LegalityCertificateCacheTelemetry::getReadyMissesDelta() const
	{
		return ReadyMissesDelta;
	}

	long long LegalityCertificateCacheTelemetry::getEstimatedChecksSavedDelta() const
	{
		return EstimatedChecksSavedDelta;
	}

	long long LegalityCertificateCacheTelemetry::getInvalidationEventsDelta() const
	{
		return InvalidationEventsDelta;
	}

	long long LegalityCertificateCacheTelemetry::getMutationInvalidationsDelta() const
	{
		return MutationInvalidationsDelta;
	}

	long long LegalityCertificateCacheTelemetry::getPhaseMismatchInvalidationsDelta() const
	{
		return PhaseMismatchInvalidationsDelta;
	}

	ReplayPhaseInvalidationReason LegalityCertificateCacheTelemetry::getLastInvalidationReason() const
	{
		return LastInvalidationReason;
	}

	LegalityCertificateCacheTelemetry LegalityCertificateCacheTelemetry::FromReuseDecision(bool attemptedReplayReuse, bool reusedReplayCertificate, long long estimatedChecksSaved)
	{
		if (!attemptedReplayReuse)
		{
			return YAKSys_Hybrid_CPU::Core::LegalityCertificateCacheTelemetry();
		}

		return reusedReplayCertificate ? LegalityCertificateCacheTelemetry(1, 0, std::max(estimatedChecksSaved, 1LL), 0, 0, 0, ReplayPhaseInvalidationReason::None) : LegalityCertificateCacheTelemetry(0, 1, 0, 0, 0, 0, ReplayPhaseInvalidationReason::None);
	}

	LegalityCertificateCacheTelemetry LegalityCertificateCacheTelemetry::FromInvalidationEvent(ReplayPhaseInvalidationReason reason, bool anyCacheInvalidated)
	{
		if (!anyCacheInvalidated)
		{
			return YAKSys_Hybrid_CPU::Core::LegalityCertificateCacheTelemetry();
		}

		return LegalityCertificateCacheTelemetry(0, 0, 0, 1, reason == ReplayPhaseInvalidationReason::CertificateMutation ? 1: 0, reason == ReplayPhaseInvalidationReason::PhaseMismatch || reason == ReplayPhaseInvalidationReason::InactivePhase ? 1: 0, reason);
	}

	PhaseCertificateTemplateKey::PhaseCertificateTemplateKey(ReplayPhaseKey phaseKey, BundleResourceCertificateIdentity certificateIdentity)
	{
		PhaseKey = phaseKey;
		CertificateIdentity = certificateIdentity;
	}

	ReplayPhaseKey PhaseCertificateTemplateKey::getPhaseKey() const
	{
		return PhaseKey;
	}

	BundleResourceCertificateIdentity PhaseCertificateTemplateKey::getCertificateIdentity() const
	{
		return CertificateIdentity;
	}

	bool PhaseCertificateTemplateKey::isValid() const
	{
		return getPhaseKey().isValid() && getCertificateIdentity().isValid();
	}

	bool PhaseCertificateTemplateKey::Equals(PhaseCertificateTemplateKey other) const
	{
		return getPhaseKey().Equals(other.getPhaseKey()) && getCertificateIdentity().Equals(other.getCertificateIdentity());
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
	bool PhaseCertificateTemplateKey::Equals(std::any obj) const
	{
		const auto *other = std::any_cast<PhaseCertificateTemplateKey>(&obj);
		return other != nullptr && Equals(*other);
	}

	int PhaseCertificateTemplateKey::GetHashCode() const
	{
		int hash = static_cast<int>(0x5B4D8A13);
		hash = (hash * 397) ^ getPhaseKey().GetHashCode();
		hash = (hash * 397) ^ getCertificateIdentity().GetHashCode();
		return hash;
	}

	PhaseCertificateTemplate::PhaseCertificateTemplate(PhaseCertificateTemplateKey templateKey, std::shared_ptr<BundleResourceCertificate> certificate)
	{
		TemplateKey = templateKey;
		Certificate = certificate;
	}

	PhaseCertificateTemplateKey PhaseCertificateTemplate::getTemplateKey() const
	{
		return TemplateKey;
	}

	ReplayPhaseKey PhaseCertificateTemplate::getPhaseKey() const
	{
		return getTemplateKey().getPhaseKey();
	}

	BundleResourceCertificateIdentity PhaseCertificateTemplate::getCertificateIdentity() const
	{
		return getTemplateKey().getCertificateIdentity();
	}

	std::shared_ptr<BundleResourceCertificate> PhaseCertificateTemplate::getCertificate() const
	{
		return Certificate;
	}

	bool PhaseCertificateTemplate::isValid() const
	{
		return getTemplateKey().isValid() && getCertificate() != nullptr && getCertificate()->IsValid();
	}

	bool PhaseCertificateTemplate::Matches(ReplayPhaseContext phase) const
	{
		return isValid() && phase.Matches(getTemplateKey().getPhaseKey());
	}

	bool PhaseCertificateTemplate::Matches(PhaseCertificateTemplateKey templateKey) const
	{
		return isValid() && getTemplateKey().Equals(templateKey);
	}
}
