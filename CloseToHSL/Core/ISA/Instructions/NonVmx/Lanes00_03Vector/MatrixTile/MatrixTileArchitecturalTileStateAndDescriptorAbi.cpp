#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	std::shared_ptr<MatrixTileArchitecturalTileStateContract> MatrixTileArchitecturalTileStateAndDescriptorAbi::getGuestArchitecturalTileStateContract()
	{
		return GuestArchitecturalTileStateContract;
	}

	bool MatrixTileArchitecturalTileStateAndDescriptorAbi::IsGuestArchitecturalOwner(MatrixTileStateOwnerKind ownerKind)
	{
		return ownerKind == MatrixTileStateOwnerKind::GuestArchitecturalTileRegisterFile;
	}

	bool MatrixTileArchitecturalTileStateAndDescriptorAbi::HasCanonicalLifetimePolicy(MatrixTileLifetimePolicyKind lifetimePolicy)
	{
		return lifetimePolicy == MatrixTileLifetimePolicyKind::GuestArchitecturalTileRegisterFile;
	}

	bool MatrixTileArchitecturalTileStateAndDescriptorAbi::IsCanonicalDescriptor(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		return descriptor->isCanonical();
	}

	bool MatrixTileArchitecturalTileStateAndDescriptorAbi::IsZeroDescriptor(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		return descriptor->isZeroEncoding();
	}

	bool MatrixTileArchitecturalTileStateAndDescriptorAbi::IsReservedDescriptor(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		return descriptor->isReservedEncoding();
	}

	std::wstring MatrixTileArchitecturalTileStateAndDescriptorAbi::ValidateDescriptor(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor)
	{
		return descriptor->isCanonical() ? DescriptorValidationDecision : descriptor->isZeroEncoding() ? L"ZeroMatrixTileDescriptorEncodingRejected" : L"ReservedMatrixTileDescriptorEncodingRejected";
	}
}
