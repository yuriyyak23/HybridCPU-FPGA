#include "YAKSys_Hybrid_CPU.Core.DescriptorSidebandEnvelope.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DescriptorSidebandEnvelope::DescriptorSidebandEnvelope() : DescriptorSidebandEnvelope(DescriptorSidebandKind::None, 0, 0, 0, false, EvidenceVisibilityClass::HostOwnedRuntimeEvidence)
	{
	}

	DescriptorSidebandEnvelope::DescriptorSidebandEnvelope(DescriptorSidebandKind kind, unsigned long long descriptorId, unsigned long long descriptorEpoch, unsigned long long descriptorHash, bool isValidated, EvidenceVisibilityClass visibilityClass)
		: Kind(kind),
		  DescriptorId(descriptorId),
		  DescriptorEpoch(descriptorEpoch),
		  DescriptorHash(descriptorHash),
		  IsValidated(isValidated),
		  VisibilityClass(visibilityClass)
	{
	}

	std::shared_ptr<DescriptorSidebandEnvelope> DescriptorSidebandEnvelope::getEmpty()
	{
		return Empty;
	}

	DescriptorSidebandKind DescriptorSidebandEnvelope::getKind() const
	{
		return Kind;
	}

	unsigned long long DescriptorSidebandEnvelope::getDescriptorId() const
	{
		return DescriptorId;
	}

	unsigned long long DescriptorSidebandEnvelope::getDescriptorEpoch() const
	{
		return DescriptorEpoch;
	}

	unsigned long long DescriptorSidebandEnvelope::getDescriptorHash() const
	{
		return DescriptorHash;
	}

	bool DescriptorSidebandEnvelope::isValidated() const
	{
		return IsValidated;
	}

	EvidenceVisibilityClass DescriptorSidebandEnvelope::getVisibilityClass() const
	{
		return VisibilityClass;
	}

	bool DescriptorSidebandEnvelope::getHasDescriptor() const
	{
		return getKind() != DescriptorSidebandKind::None && getDescriptorHash() != 0;
	}

	bool DescriptorSidebandEnvelope::isCompatibilityProjection() const
	{
		return getKind() == DescriptorSidebandKind::CompatibilityProjection;
	}

	bool DescriptorSidebandEnvelope::getRequiresHostHandling() const
	{
		return !isValidated() || ([&] { const auto codexPatternValue = getVisibilityClass(); return codexPatternValue == EvidenceVisibilityClass::HostOwnedRuntimeEvidence || codexPatternValue == EvidenceVisibilityClass::SchedulerEvidence || codexPatternValue == EvidenceVisibilityClass::BackendBindingEvidence || codexPatternValue == EvidenceVisibilityClass::NativeTokenEvidence; }());
	}

	bool DescriptorSidebandEnvelope::CanExposeToGuest(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy)
	{
		return isValidated() && evidencePolicy != nullptr && evidencePolicy->CanExposeToGuest(getVisibilityClass());
	}
}
