#include "MatrixTileMemoryShapeAndFaultAbi.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	std::shared_ptr<MatrixTileMemoryShapeContract> MatrixTileMemoryShapeAndFaultAbi::CreateLoadContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned long long baseAddress, unsigned short pageSizeBytes)
	{
		return std::make_shared<MatrixTileMemoryShapeContract>(MatrixTileMemoryOperationKind::Load, MatrixTileEffectiveAddressSourceKind::ExplicitRuntimeTileMemoryOperand, descriptor, baseAddress, pageSizeBytes);
	}

	std::shared_ptr<MatrixTileMemoryShapeContract> MatrixTileMemoryShapeAndFaultAbi::CreateStoreContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned long long baseAddress, unsigned short pageSizeBytes)
	{
		return std::make_shared<MatrixTileMemoryShapeContract>(MatrixTileMemoryOperationKind::Store, MatrixTileEffectiveAddressSourceKind::ExplicitRuntimeTileMemoryOperand, descriptor, baseAddress, pageSizeBytes);
	}

	std::shared_ptr<MatrixTileMemoryShapeValidationResult> MatrixTileMemoryShapeAndFaultAbi::Validate(std::shared_ptr<MatrixTileMemoryShapeContract> contract)
	{
		if (contract->Operation != MatrixTileMemoryOperationKind::Load && contract->Operation != MatrixTileMemoryOperationKind::Store)
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::UnsupportedOperation);
		}

		if (contract->EffectiveAddressSource != MatrixTileEffectiveAddressSourceKind::ExplicitRuntimeTileMemoryOperand)
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::UnsupportedEffectiveAddressSource);
		}

		if (contract->PageSizeBytes == 0)
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::InvalidPageSize);
		}

		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor = contract->Descriptor;
		if (descriptor->isZeroEncoding())
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::ZeroDescriptor);
		}

		if (descriptor->isReservedEncoding())
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::ReservedDescriptor);
		}

		if (!IsSupportedElementSize(descriptor->ElementSizeBytes))
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::UnsupportedElementSize);
		}

		unsigned long long rowBytes = static_cast<unsigned long long>(descriptor->Columns) * descriptor->ElementSizeBytes;
		if (rowBytes == 0 || rowBytes > std::numeric_limits<unsigned int>::max())
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::RowByteOverflow);
		}

		if (descriptor->StrideBytes < rowBytes)
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::StrideTooSmall);
		}

		if (!IsElementAligned(contract->BaseAddress, descriptor->ElementSizeBytes) || !IsElementAligned(descriptor->StrideBytes, descriptor->ElementSizeBytes))
		{
			std::shared_ptr<MatrixTileMemoryFaultPoint> point = std::make_shared<MatrixTileMemoryFaultPoint>(MatrixTileMemoryFaultPoint{0, 0, 0, contract->BaseAddress, contract->Operation == MatrixTileMemoryOperationKind::Store});
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::AlignmentFault, *point, true);
		}

		unsigned long long totalBytes;
		if (!TryGetTotalByteFootprint(descriptor, rowBytes, totalBytes))
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::AddressOverflow);
		}

		if (totalBytes == 0 || contract->BaseAddress > std::numeric_limits<unsigned long long>::max() - (totalBytes - 1))
		{
			return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::AddressOverflow);
		}

		unsigned long long lastByteAddress = contract->BaseAddress + totalBytes - 1;
		bool crossesPageBoundary = CrossesPageBoundary(contract->BaseAddress, lastByteAddress, contract->PageSizeBytes);

		return MatrixTileMemoryShapeValidationResult::Valid(contract->BaseAddress, lastByteAddress, totalBytes, static_cast<unsigned int>(rowBytes), crossesPageBoundary, GetPublicationPolicy(contract->Operation));
	}

	std::shared_ptr<MatrixTileMemoryShapeValidationResult> MatrixTileMemoryShapeAndFaultAbi::ProjectPartialMemoryFault(std::shared_ptr<MatrixTileMemoryShapeContract> contract, unsigned short row, unsigned short column, unsigned short byteOffsetInElement)
	{
		std::shared_ptr<MatrixTileMemoryFaultPoint> point = CreatePreciseFaultPoint(contract, row, column, byteOffsetInElement);

		return MatrixTileMemoryShapeValidationResult::Fault(MatrixTileMemoryFaultKind::PartialMemoryFault, *point, true);
	}

	std::shared_ptr<MatrixTileMemoryFaultPoint> MatrixTileMemoryShapeAndFaultAbi::CreatePreciseFaultPoint(std::shared_ptr<MatrixTileMemoryShapeContract> contract, unsigned short row, unsigned short column, unsigned short byteOffsetInElement)
	{
		std::shared_ptr<MatrixTileMemoryShapeValidationResult> validation = Validate(contract);
		if (!validation->isMemoryShapeAbiAccepted())
		{
			throw std::invalid_argument("Cannot project a precise MTILE memory fault point from invalid shape.");
		}

		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor = contract->Descriptor;
		if (row >= descriptor->Rows)
		{
			throw std::out_of_range("Tile memory fault row is outside the descriptor shape.");
		}

		if (column >= descriptor->Columns)
		{
			throw std::out_of_range("Tile memory fault column is outside the descriptor shape.");
		}

		if (byteOffsetInElement >= descriptor->ElementSizeBytes)
		{
			throw std::out_of_range("Tile memory fault byte offset is outside the element width.");
		}

		unsigned long long byteOffsetInRow = static_cast<unsigned long long>(column) * descriptor->ElementSizeBytes + byteOffsetInElement;
		unsigned long long address = contract->BaseAddress + static_cast<unsigned long long>(row) * descriptor->StrideBytes + byteOffsetInRow;

		return std::make_shared<MatrixTileMemoryFaultPoint>(row, column, static_cast<unsigned int>(byteOffsetInRow), address, contract->Operation == MatrixTileMemoryOperationKind::Store);
	}

	bool MatrixTileMemoryShapeAndFaultAbi::IsSupportedElementSize(unsigned short elementSizeBytes)
	{
		return elementSizeBytes == 1 || elementSizeBytes == 2 || elementSizeBytes == 4 || elementSizeBytes == 8;
	}

	bool MatrixTileMemoryShapeAndFaultAbi::IsElementAligned(unsigned long long value, unsigned short elementSizeBytes)
	{
		return IsSupportedElementSize(elementSizeBytes) && value % elementSizeBytes == 0;
	}

	MatrixTileMemoryPublicationPolicyKind MatrixTileMemoryShapeAndFaultAbi::GetPublicationPolicy(MatrixTileMemoryOperationKind operation)
	{
		switch (operation) {
		case MatrixTileMemoryOperationKind::Load: return MatrixTileMemoryPublicationPolicyKind::RetireStagedLoadPublication;
		case MatrixTileMemoryOperationKind::Store: return MatrixTileMemoryPublicationPolicyKind::RetireStagedStoreCommit;
		default: return MatrixTileMemoryPublicationPolicyKind::Unspecified;
		}
	}

	bool MatrixTileMemoryShapeAndFaultAbi::TryGetTotalByteFootprint(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned long long rowBytes, unsigned long long &totalBytes)
	{
		unsigned long long interRowBytes = static_cast<unsigned long long>(descriptor->Rows - 1) * descriptor->StrideBytes;
		if (interRowBytes > std::numeric_limits<unsigned long long>::max() - rowBytes)
		{
			totalBytes = 0;
			return false;
		}

		totalBytes = interRowBytes + rowBytes;
		return true;
	}

	bool MatrixTileMemoryShapeAndFaultAbi::CrossesPageBoundary(unsigned long long firstByteAddress, unsigned long long lastByteAddress, unsigned short pageSizeBytes)
	{
		return firstByteAddress / pageSizeBytes != lastByteAddress / pageSizeBytes;
	}
}
