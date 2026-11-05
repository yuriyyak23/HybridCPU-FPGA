#pragma once

#include "SecureComputeDescriptorSidebandEnvelope.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeDescriptorSidebandEnvelope final : public std::enable_shared_from_this<SecureComputeDescriptorSidebandEnvelope>
	{
	private:
		const SecureComputeDescriptorSidebandKind Kind;
		const unsigned long long DomainTag;
		const unsigned long long ProvenanceHash;

	public:
		SecureComputeDescriptorSidebandEnvelope();

		SecureComputeDescriptorSidebandEnvelope(SecureComputeDescriptorSidebandKind kind, unsigned long long domainTag, unsigned long long provenanceHash);

		SecureComputeDescriptorSidebandKind getKind() const;

		unsigned long long getDomainTag() const;

		unsigned long long getProvenanceHash() const;

		bool isAuthorityCarrier() const;
	};
}
