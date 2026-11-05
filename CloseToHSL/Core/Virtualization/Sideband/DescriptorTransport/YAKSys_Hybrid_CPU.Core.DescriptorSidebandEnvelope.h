#pragma once

#include "DescriptorSidebandEnvelope.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/EvidencePolicyDescriptor.h"
#include "../../../Runtime/Domains/Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DescriptorSidebandEnvelope final : public std::enable_shared_from_this<DescriptorSidebandEnvelope>
	{
	private:
		static inline const std::shared_ptr<DescriptorSidebandEnvelope> Empty = std::make_shared<DescriptorSidebandEnvelope>();
		const DescriptorSidebandKind Kind;
		const unsigned long long DescriptorId;
		const unsigned long long DescriptorEpoch;
		const unsigned long long DescriptorHash;
		const bool IsValidated;
		const EvidenceVisibilityClass VisibilityClass;

	public:
		DescriptorSidebandEnvelope();

		DescriptorSidebandEnvelope(DescriptorSidebandKind kind, unsigned long long descriptorId, unsigned long long descriptorEpoch, unsigned long long descriptorHash, bool isValidated, EvidenceVisibilityClass visibilityClass);

		static std::shared_ptr<DescriptorSidebandEnvelope> getEmpty();

		DescriptorSidebandKind getKind() const;

		unsigned long long getDescriptorId() const;

		unsigned long long getDescriptorEpoch() const;

		unsigned long long getDescriptorHash() const;

		bool isValidated() const;

		EvidenceVisibilityClass getVisibilityClass() const;

		bool getHasDescriptor() const;

		bool isCompatibilityProjection() const;

		bool getRequiresHostHandling() const;

		bool CanExposeToGuest(std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy);
	};
}
