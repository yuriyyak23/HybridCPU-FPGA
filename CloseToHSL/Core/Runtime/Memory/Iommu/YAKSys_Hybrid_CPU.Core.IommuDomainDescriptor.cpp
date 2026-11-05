#include "YAKSys_Hybrid_CPU.Core.IommuDomainDescriptor.h"

#include <utility>

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	IommuDomainDescriptor::IommuDomainDescriptor() : IommuDomainDescriptor(IommuDomainAuthority::Runtime, nullptr, IOMMUAccessPermissions::None, true, true)
	{
	}

	IommuDomainDescriptor::IommuDomainDescriptor(IommuDomainAuthority authority, std::shared_ptr<IommuDomainBinding> binding, IOMMUAccessPermissions requiredPermissions, bool ownsIotlbInvalidation, bool allowsCompatibilityProjection)
		: Authority(authority),
		  Binding(std::move(binding)),
		  RequiredPermissions(requiredPermissions),
		  OwnsIotlbInvalidation(ownsIotlbInvalidation),
		  AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	IommuDomainAuthority IommuDomainDescriptor::getAuthority() const
	{
		return Authority;
	}

	std::shared_ptr<IommuDomainBinding> IommuDomainDescriptor::getBinding() const
	{
		return Binding;
	}

	IOMMUAccessPermissions IommuDomainDescriptor::getRequiredPermissions() const
	{
		return RequiredPermissions;
	}

	bool IommuDomainDescriptor::getOwnsIotlbInvalidation() const
	{
		return OwnsIotlbInvalidation;
	}

	bool IommuDomainDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool IommuDomainDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == IommuDomainAuthority::Runtime;
	}

	bool IommuDomainDescriptor::getHasValidBinding() const
	{
		return Binding != nullptr && Binding->isValid();
	}

	bool IommuDomainDescriptor::getHasPermissionGrant() const
	{
		return RequiredPermissions == IOMMUAccessPermissions::None ||
			(Binding != nullptr && Binding->Allows(RequiredPermissions));
	}

	bool IommuDomainDescriptor::getCanInvalidateIotlb() const
	{
		return isRuntimeAuthoritative() && getOwnsIotlbInvalidation() && getHasValidBinding();
	}

	bool IommuDomainDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative() && getHasValidBinding();
	}

	std::shared_ptr<IommuDomainDescriptor> IommuDomainDescriptor::WithBinding(std::shared_ptr<IommuDomainBinding> binding, IOMMUAccessPermissions requiredPermissions)
	{
		return std::make_shared<IommuDomainDescriptor>(
			getAuthority(),
			std::move(binding),
			requiredPermissions,
			getOwnsIotlbInvalidation(),
			getAllowsCompatibilityProjection());
	}
}
