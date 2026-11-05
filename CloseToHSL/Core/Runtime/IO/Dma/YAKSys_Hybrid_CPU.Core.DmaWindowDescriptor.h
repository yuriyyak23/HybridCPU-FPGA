#pragma once

#include "../../../../Memory/MMU/IOMMU.h"
#include <limits>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	using HybridCPU_ISE::CloseToHSL::Memory::MMU::IOMMUAccessPermissions;

	enum class DmaWindowAuthority : unsigned char
	{
		IoDomain = 0,
		CompatibilityProjection = 1
	};

	class DmaWindowDescriptor final : public std::enable_shared_from_this<DmaWindowDescriptor>
	{
	private:
		const DmaWindowAuthority Authority;
		const unsigned long long BaseAddress;
		const unsigned long long Length;
		const IOMMUAccessPermissions Permissions;
		const bool RequiresFence;
		const bool AllowsCompatibilityProjection;

	public:
		DmaWindowDescriptor();

		DmaWindowDescriptor(DmaWindowAuthority authority, unsigned long long baseAddress, unsigned long long length, IOMMUAccessPermissions permissions, bool requiresFence, bool allowsCompatibilityProjection);

		DmaWindowAuthority getAuthority() const;

		unsigned long long getBaseAddress() const;

		unsigned long long getLength() const;

		IOMMUAccessPermissions getPermissions() const;

		bool getRequiresFence() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool isEmpty() const;

		bool isValid() const;

		bool getCanProjectToCompatibilityFrontend() const;

		bool AllowsRange(unsigned long long ioVirtualAddress, unsigned long long accessSize, IOMMUAccessPermissions requestedPermissions);

		std::shared_ptr<DmaWindowDescriptor> WithCompatibilityProjection(bool enabled);
	};
}
