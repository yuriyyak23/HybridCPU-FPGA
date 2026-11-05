#include "YAKSys_Hybrid_CPU.Core.DmaWindowDescriptor.h"

using namespace HybridCPU_ISE::CloseToHSL::Memory::MMU;

namespace YAKSys_Hybrid_CPU::Core
{

	DmaWindowDescriptor::DmaWindowDescriptor() : DmaWindowDescriptor(DmaWindowAuthority::IoDomain, 0, 0, IOMMUAccessPermissions::None, true, true)
	{
	}

	DmaWindowDescriptor::DmaWindowDescriptor(DmaWindowAuthority authority, unsigned long long baseAddress, unsigned long long length, IOMMUAccessPermissions permissions, bool requiresFence, bool allowsCompatibilityProjection)
		: Authority(authority),
		  BaseAddress(baseAddress),
		  Length(length),
		  Permissions(permissions),
		  RequiresFence(requiresFence),
		  AllowsCompatibilityProjection(allowsCompatibilityProjection)
	{
	}

	DmaWindowAuthority DmaWindowDescriptor::getAuthority() const
	{
		return Authority;
	}

	unsigned long long DmaWindowDescriptor::getBaseAddress() const
	{
		return BaseAddress;
	}

	unsigned long long DmaWindowDescriptor::getLength() const
	{
		return Length;
	}

	IOMMUAccessPermissions DmaWindowDescriptor::getPermissions() const
	{
		return Permissions;
	}

	bool DmaWindowDescriptor::getRequiresFence() const
	{
		return RequiresFence;
	}

	bool DmaWindowDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool DmaWindowDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == DmaWindowAuthority::IoDomain;
	}

	bool DmaWindowDescriptor::isEmpty() const
	{
		return getLength() == 0;
	}

	bool DmaWindowDescriptor::isValid() const
	{
		return !isEmpty() && getPermissions() != IOMMUAccessPermissions::None && getBaseAddress() <= std::numeric_limits<unsigned long long>::max() - (getLength() - 1);
	}

	bool DmaWindowDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	bool DmaWindowDescriptor::AllowsRange(unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions)
	{
		if (!isValid() || accessSize == 0 || requestedPermissions == IOMMUAccessPermissions::None || ioVirtualAddress > std::numeric_limits<unsigned long long>::max() - (accessSize - 1) || (static_cast<unsigned int>(getPermissions()) & static_cast<unsigned int>(requestedPermissions)) != static_cast<unsigned int>(requestedPermissions))
		{
			return false;
		}

		unsigned long long inclusiveEnd = ioVirtualAddress + accessSize - 1;
		unsigned long long windowEnd = getBaseAddress() + getLength() - 1;
		return ioVirtualAddress >= getBaseAddress() && inclusiveEnd <= windowEnd;
	}

	std::shared_ptr<DmaWindowDescriptor> DmaWindowDescriptor::WithCompatibilityProjection(bool enabled)
	{
		return std::make_shared<DmaWindowDescriptor>(getAuthority(), getBaseAddress(), getLength(), getPermissions(), getRequiresFence(), enabled);
	}
}
