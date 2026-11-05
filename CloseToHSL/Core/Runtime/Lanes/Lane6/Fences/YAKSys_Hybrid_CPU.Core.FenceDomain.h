#pragma once

#include "FenceDomain.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class FenceDomain final : public std::enable_shared_from_this<FenceDomain>
	{
	private:
		const FenceDomainAuthority Authority;
		const unsigned long long DomainId;
		const FenceOrderingScope OrderingScope;
		const bool AllowsCompatibilityProjection;

	public:
		FenceDomain();

		FenceDomain(FenceDomainAuthority authority, unsigned long long domainId, FenceOrderingScope orderingScope, bool allowsCompatibilityProjection);

		FenceDomainAuthority getAuthority() const;

		unsigned long long getDomainId() const;

		FenceOrderingScope getOrderingScope() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasDomainBinding() const;

		bool getCoversRequiredOrdering() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<FenceDomain> WithDomainBinding(unsigned long long domainId);
	};
}
