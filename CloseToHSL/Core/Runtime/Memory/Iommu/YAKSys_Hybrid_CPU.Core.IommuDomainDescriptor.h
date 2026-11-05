#pragma once

#include "IommuDomainDescriptor.h"
#include "../../IO/Dma/DmaDomainBinding.h"
#include "../../../../Memory/MMU/IOMMU.h"
#include <memory>

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	class IommuDomainDescriptor final : public std::enable_shared_from_this<IommuDomainDescriptor>
	{
	private:
		const IommuDomainAuthority Authority;
		const std::shared_ptr<IommuDomainBinding> Binding;
		const IOMMUAccessPermissions RequiredPermissions;
		const bool OwnsIotlbInvalidation;
		const bool AllowsCompatibilityProjection;

	public:
		IommuDomainDescriptor();

		IommuDomainDescriptor(IommuDomainAuthority authority, std::shared_ptr<IommuDomainBinding> binding, IOMMUAccessPermissions requiredPermissions, bool ownsIotlbInvalidation, bool allowsCompatibilityProjection);

		IommuDomainAuthority getAuthority() const;

		std::shared_ptr<IommuDomainBinding> getBinding() const;

		IOMMUAccessPermissions getRequiredPermissions() const;

		bool getOwnsIotlbInvalidation() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasValidBinding() const;

		bool getHasPermissionGrant() const;

		bool getCanInvalidateIotlb() const;

		bool getCanProjectToCompatibilityFrontend() const;

		std::shared_ptr<IommuDomainDescriptor> WithBinding(std::shared_ptr<IommuDomainBinding> binding, IOMMUAccessPermissions requiredPermissions);
	};
}
