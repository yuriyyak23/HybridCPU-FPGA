#pragma once

#include "SecureDomainDescriptorSideband.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureDomainDescriptorSideband final : public std::enable_shared_from_this<SecureDomainDescriptorSideband>
	{
	private:
		const unsigned long long DomainTag;
		const SecureSidebandVisibility Visibility;
		const unsigned long long ProvenanceHash;

	public:
		SecureDomainDescriptorSideband();

		SecureDomainDescriptorSideband(unsigned long long domainTag, SecureSidebandVisibility visibility, unsigned long long provenanceHash);

		unsigned long long getDomainTag() const;

		SecureSidebandVisibility getVisibility() const;

		unsigned long long getProvenanceHash() const;

		bool isAuthorityCarrier() const;
	};
}
