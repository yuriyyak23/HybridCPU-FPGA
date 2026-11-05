#include "YAKSys_Hybrid_CPU.Core.FenceDomain.h"

namespace YAKSys_Hybrid_CPU::Core
{

	FenceDomain::FenceDomain() : FenceDomain(FenceDomainAuthority::Runtime, 0, FenceOrderingScope::All, true)
	{
	}

	FenceDomain::FenceDomain(FenceDomainAuthority authority, unsigned long long domainId, FenceOrderingScope orderingScope, bool allowsCompatibilityProjection) : Authority(authority), DomainId(domainId), OrderingScope(orderingScope), AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	FenceDomainAuthority FenceDomain::getAuthority() const
	{
		return Authority;
	}

	unsigned long long FenceDomain::getDomainId() const
	{
		return DomainId;
	}

	FenceOrderingScope FenceDomain::getOrderingScope() const
	{
		return OrderingScope;
	}

	bool FenceDomain::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool FenceDomain::isRuntimeAuthoritative() const
	{
		return getAuthority() == FenceDomainAuthority::Runtime;
	}

	bool FenceDomain::getHasDomainBinding() const
	{
		return getDomainId() != 0;
	}

	bool FenceDomain::getCoversRequiredOrdering() const
	{
		return (static_cast<int>(getOrderingScope()) & static_cast<int>(FenceOrderingScope::All)) == static_cast<int>(FenceOrderingScope::All);
	}

	bool FenceDomain::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	std::shared_ptr<FenceDomain> FenceDomain::WithDomainBinding(unsigned long long domainId)
	{
		return std::make_shared<FenceDomain>(getAuthority(), domainId, getOrderingScope(), getAllowsCompatibilityProjection());
	}
}
