#include "YAKSys_Hybrid_CPU.Core.DmaAuthorityService.h"
#include "../../Domains/Descriptors/IoDomain/YAKSys_Hybrid_CPU.Core.IoDomainDescriptor.h"
#include "YAKSys_Hybrid_CPU.Core.DmaWindowDescriptor.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;
using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core
{

	std::shared_ptr<DmaAuthorityResult> DmaAuthorityService::ValidateAccess(std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<IommuDomainBinding> binding, unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions, bool fenceSatisfied)
	{
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: IoDomainDescriptor? io = context.Io;
		std::shared_ptr<IoDomainDescriptor> io = context->getIo();
		if (io == nullptr)
		{
			return Deny(DmaAuthorityDecision::MissingIoDomain, DmaFaultKind::QueueOwnershipFault, ioVirtualAddress, requestedPermissions, L"DMA authority requires an I/O-domain descriptor.");
		}

		if (!io->getHasRequiredIoAuthority())
		{
			return Deny(DmaAuthorityDecision::RuntimeAuthorityMissing, DmaFaultKind::ValidationEvidenceMissing, ioVirtualAddress, requestedPermissions, L"I/O-domain descriptor does not own DMA and IOMMU authority.");
		}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: DmaWindowDescriptor? window = io.DmaWindow;
		std::shared_ptr<DmaWindowDescriptor> window = io->getDmaWindow();
		if (window == nullptr)
		{
			return Deny(DmaAuthorityDecision::MissingDmaWindow, DmaFaultKind::DescriptorFault, ioVirtualAddress, requestedPermissions, L"DMA authority requires a descriptor-owned DMA window.");
		}

		if (!window->isRuntimeAuthoritative())
		{
			return Deny(DmaAuthorityDecision::RuntimeAuthorityMissing, DmaFaultKind::ValidationEvidenceMissing, ioVirtualAddress, requestedPermissions, L"Compatibility projection cannot own DMA window authority.");
		}

		if (!binding || !binding->isValid())
		{
			return Deny(DmaAuthorityDecision::MissingIommuBinding, DmaFaultKind::MissingDomainBinding, ioVirtualAddress, requestedPermissions, L"DMA authority requires a valid IOMMU domain binding.");
		}

		if (!binding->Allows(requestedPermissions))
		{
			return Deny(DmaAuthorityDecision::PermissionDenied, DmaFaultKind::PermissionFault, ioVirtualAddress, requestedPermissions, L"IOMMU domain binding denies the requested DMA permissions.");
		}

		if (!window->AllowsRange(ioVirtualAddress, accessSize, requestedPermissions))
		{
			return Deny(DmaAuthorityDecision::RangeDenied, DmaFaultKind::PermissionFault, ioVirtualAddress, requestedPermissions, L"Descriptor-owned DMA window denies the requested range.");
		}

		if ((window->getRequiresFence() || binding->RequiresFence) && !fenceSatisfied)
		{
			return DmaAuthorityResult::Denied(DmaAuthorityDecision::FenceRequired, DmaFault::Replay(DmaFaultKind::NonCoherentFenceRequired, ioVirtualAddress, IsWrite(requestedPermissions), L"DMA authority requires a fence before this access can be admitted."));
		}

		return DmaAuthorityResult::getAllowed();
	}

	std::shared_ptr<DmaAuthorityResult> DmaAuthorityService::Deny(DmaAuthorityDecision decision, DmaFaultKind faultKind, unsigned long long ioVirtualAddress, IOMMUAccessPermissions requestedPermissions, const std::wstring &message)
	{
		return DmaAuthorityResult::Denied(decision, DmaFault::Abort(faultKind, ioVirtualAddress, IsWrite(requestedPermissions), message));
	}

	bool DmaAuthorityService::IsWrite(IOMMUAccessPermissions permissions)
	{
		return (static_cast<int>(permissions) & static_cast<int>(IOMMUAccessPermissions::Write)) != 0;
	}
}
