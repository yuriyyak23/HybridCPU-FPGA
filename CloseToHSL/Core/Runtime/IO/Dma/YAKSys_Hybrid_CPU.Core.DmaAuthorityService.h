#pragma once

#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "../../../../Memory/MMU/IOMMU.h"
#include "DmaAuthorityService.h"
#include "DmaDomainBinding.h"
#include <string>
#include <memory>

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{
	class DmaAuthorityService final : public std::enable_shared_from_this<DmaAuthorityService>
	{
	public:
		std::shared_ptr<DmaAuthorityResult> ValidateAccess(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<IommuDomainBinding> binding, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, bool fenceSatisfied = false);

	private:
		static std::shared_ptr<DmaAuthorityResult> Deny(DmaAuthorityDecision decision, DmaFaultKind faultKind, unsigned long long ioVirtualAddress, IOMMUAccessPermissions requestedPermissions, const std::wstring &message);

		static bool IsWrite(IOMMUAccessPermissions permissions);
	};
}
