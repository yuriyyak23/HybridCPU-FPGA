#include "YAKSys_Hybrid_CPU.Core.NestedDomainDescriptor.h"

using namespace YAKSys_Hybrid_CPU::Core::Nested;

namespace YAKSys_Hybrid_CPU::Core
{

	NestedDomainDescriptor::NestedDomainDescriptor() : NestedDomainDescriptor(NestedDomainAuthority::Runtime, 0, 0, NestedCapabilityGrantMask::None, false, false, true, true)
	{
	}

	NestedDomainDescriptor::NestedDomainDescriptor(NestedDomainAuthority authority, unsigned long long parentDomainId, unsigned long long childDomainId, NestedCapabilityGrantMask capabilities, bool domainCompositionEnabled, bool allowsCompatibilityProjection, bool hostEvidenceExcluded, bool lanePassthroughBlocked)
		: Authority(authority), ParentDomainId(parentDomainId), ChildDomainId(childDomainId), Capabilities(capabilities),
		  DomainCompositionEnabled(domainCompositionEnabled), AllowsCompatibilityProjection(allowsCompatibilityProjection),
		  HostEvidenceExcluded(hostEvidenceExcluded), LanePassthroughBlocked(lanePassthroughBlocked)
	{
	}

	NestedDomainAuthority NestedDomainDescriptor::getAuthority() const
	{
		return Authority;
	}

	unsigned long long NestedDomainDescriptor::getParentDomainId() const
	{
		return ParentDomainId;
	}

	unsigned long long NestedDomainDescriptor::getChildDomainId() const
	{
		return ChildDomainId;
	}

	NestedCapabilityGrantMask NestedDomainDescriptor::getCapabilities() const
	{
		return Capabilities;
	}

	bool NestedDomainDescriptor::getDomainCompositionEnabled() const
	{
		return DomainCompositionEnabled;
	}

	bool NestedDomainDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool NestedDomainDescriptor::getHostEvidenceExcluded() const
	{
		return HostEvidenceExcluded;
	}

	bool NestedDomainDescriptor::getLanePassthroughBlocked() const
	{
		return LanePassthroughBlocked;
	}

	bool NestedDomainDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == NestedDomainAuthority::Runtime;
	}

	bool NestedDomainDescriptor::getHasParentChildBinding() const
	{
		return getParentDomainId() != 0 && getChildDomainId() != 0 && getParentDomainId() != getChildDomainId();
	}

	bool NestedDomainDescriptor::HasCapability(NestedCapabilityGrantMask capability)
	{
		return capability == NestedCapabilityGrantMask::None || (getCapabilities() & capability) == capability;
	}

	bool NestedDomainDescriptor::getHasRequiredSecurityGates() const
	{
		return getHostEvidenceExcluded() && getLanePassthroughBlocked();
	}

	bool NestedDomainDescriptor::getCanComposeDomain() const
	{
		return isRuntimeAuthoritative() && getDomainCompositionEnabled() && getHasParentChildBinding() && getHasRequiredSecurityGates();
	}

	bool NestedDomainDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && getCanComposeDomain();
	}

	std::shared_ptr<NestedDomainDescriptor> NestedDomainDescriptor::WithDomainBinding(unsigned long long parentDomainId, unsigned long long childDomainId)
	{
		return std::make_shared<NestedDomainDescriptor>(getAuthority(), parentDomainId, childDomainId, getCapabilities(),
			getDomainCompositionEnabled(), getAllowsCompatibilityProjection(), getHostEvidenceExcluded(), getLanePassthroughBlocked());
	}
}
