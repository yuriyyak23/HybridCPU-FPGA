#pragma once

#include "NestedDomainDescriptor.h"
#include "../../../Virtualization/Compatibility/Frontend/Projection/Nested/NestedDomainController.h"
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Nested;

namespace YAKSys_Hybrid_CPU::Core
{
	class NestedDomainDescriptor final : public std::enable_shared_from_this<NestedDomainDescriptor>
	{
	private:
		const NestedDomainAuthority Authority;
		const unsigned long long ParentDomainId;
		const unsigned long long ChildDomainId;
		const NestedCapabilityGrantMask Capabilities;
		const bool DomainCompositionEnabled;
		const bool AllowsCompatibilityProjection;
		const bool HostEvidenceExcluded;
		const bool LanePassthroughBlocked;

	public:
		NestedDomainDescriptor();

		NestedDomainDescriptor(NestedDomainAuthority authority, unsigned long long parentDomainId, unsigned long long childDomainId, NestedCapabilityGrantMask capabilities, bool domainCompositionEnabled, bool allowsCompatibilityProjection, bool hostEvidenceExcluded, bool lanePassthroughBlocked);

		NestedDomainAuthority getAuthority() const;

		unsigned long long getParentDomainId() const;

		unsigned long long getChildDomainId() const;

		NestedCapabilityGrantMask getCapabilities() const;

		bool getDomainCompositionEnabled() const;

		bool getAllowsCompatibilityProjection() const;

		bool getHostEvidenceExcluded() const;

		bool getLanePassthroughBlocked() const;

		bool isRuntimeAuthoritative() const;

		bool getHasParentChildBinding() const;

		bool HasCapability(NestedCapabilityGrantMask capability);

		bool getHasRequiredSecurityGates() const;

		bool getCanComposeDomain() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<NestedDomainDescriptor> WithDomainBinding(unsigned long long parentDomainId, unsigned long long childDomainId);
	};
}
