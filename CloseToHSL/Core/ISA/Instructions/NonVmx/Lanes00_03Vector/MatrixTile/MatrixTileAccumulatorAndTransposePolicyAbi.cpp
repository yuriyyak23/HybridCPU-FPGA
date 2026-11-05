#include "MatrixTileAccumulatorAndTransposePolicyAbi.h"
#include "MatrixTileNumericPolicyAbi.h"
#include "MatrixTileLayoutPolicyAbi.h"
#include "MatrixTileIrProjectionAndMaterializer.h"
#include <optional>
#include <utility>

using namespace HybridCPU_ISE::Arch;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	namespace
	{
		bool IsDefaultNumericPolicy(const MatrixTileNumericPolicy &policy)
		{
			return policy.AbiVersion == 0 &&
				policy.ProfileId == MatrixTileNumericProfileId::Unspecified &&
				policy.ElementType == DataTypeEnum{} &&
				policy.AccumulatorType == DataTypeEnum{} &&
				policy.PublishFormat == DataTypeEnum{} &&
				policy.Signedness == MatrixTileNumericSignedness::Unspecified &&
				policy.WideningRule == MatrixTileNumericWideningRule::Unspecified &&
				policy.MultiplyRule == MatrixTileNumericMultiplyRule::Unspecified &&
				policy.AddRule == MatrixTileNumericAddRule::Unspecified &&
				policy.RoundingMode == MatrixTileNumericRoundingMode::Unspecified &&
				policy.SaturationMode == MatrixTileNumericSaturationMode::Unspecified &&
				policy.OverflowMode == MatrixTileNumericOverflowMode::Unspecified &&
				policy.NaNPolicy == MatrixTileNumericNaNPolicy::Unspecified &&
				policy.InfinityPolicy == MatrixTileNumericInfinityPolicy::Unspecified &&
				policy.DenormalPolicy == MatrixTileNumericDenormalPolicy::Unspecified &&
				policy.ReproducibilityMode == MatrixTileNumericReproducibilityMode::Unspecified &&
				policy.ExceptionPolicy == MatrixTileNumericExceptionPolicy::Unspecified &&
				policy.Fingerprint == 0;
		}
	}

	std::shared_ptr<MatrixTileMaccSemanticContract> MatrixTileAccumulatorAndTransposePolicyAbi::CreateMaccContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> left, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> right, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> accumulator, MatrixTileNumericElementKind elementKind)
	{
		std::shared_ptr<MatrixTileMaccSemanticContract> contract = std::make_shared<MatrixTileMaccSemanticContract>(left, right, accumulator, elementKind, MatrixTileAccumulatorPolicyKind::WideningIntegerAccumulatorWithOverflowTrap);

		DataTypeEnum elementType;
		std::shared_ptr<MatrixTileNumericPolicy> policy;
		if (TryGetLegacyElementType(left->ElementSizeBytes, elementKind, elementType) && MatrixTileNumericPolicyAbi::TryCreateLegacyPhase14Policy(elementType, policy))
		{
			contract->NumericPolicy = policy;
		}
		else
		{
			contract->NumericPolicy = std::make_shared<MatrixTileNumericPolicy>();
		}
		contract->LayoutPolicy = MatrixTileLayoutPolicyAbi::CreateMaccPolicy();
		return contract;
	}

	std::shared_ptr<MatrixTileMaccSemanticContract> MatrixTileAccumulatorAndTransposePolicyAbi::CreateMaccContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> left, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> right, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> accumulator, std::shared_ptr<MatrixTileNumericPolicy> numericPolicy)
	{
		MatrixTileNumericElementKind elementKind = MatrixTileNumericElementKind::Unspecified;
		if (numericPolicy != nullptr)
		{
			switch (numericPolicy->Signedness)
			{
			case MatrixTileNumericSignedness::Signed: elementKind = MatrixTileNumericElementKind::SignedInteger; break;
			case MatrixTileNumericSignedness::Unsigned: elementKind = MatrixTileNumericElementKind::UnsignedInteger; break;
			case MatrixTileNumericSignedness::NotApplicable: elementKind = MatrixTileNumericElementKind::FloatingPoint; break;
			default: break;
			}
		}

		std::shared_ptr<MatrixTileMaccSemanticContract> contract = std::make_shared<MatrixTileMaccSemanticContract>(left, right, accumulator, elementKind, GetAccumulatorPolicy(numericPolicy));
		contract->NumericPolicy = numericPolicy;
		contract->HasExplicitNumericPolicy = true;
		contract->LayoutPolicy = MatrixTileLayoutPolicyAbi::CreateMaccPolicy();
		return contract;
	}

	std::shared_ptr<MatrixTileMaccSemanticContract> MatrixTileAccumulatorAndTransposePolicyAbi::CreateMaccContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> left, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> right, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> accumulator, std::shared_ptr<MatrixTileNumericPolicy> numericPolicy, std::shared_ptr<MatrixTileLayoutPolicy> layoutPolicy)
	{
		std::shared_ptr<MatrixTileMaccSemanticContract> contract = CreateMaccContract(left, right, accumulator, numericPolicy);
		contract->LayoutPolicy = std::move(layoutPolicy);
		contract->HasExplicitLayoutPolicy = true;
		return contract;
	}

	std::shared_ptr<MatrixTileTransposeSemanticContract> MatrixTileAccumulatorAndTransposePolicyAbi::CreateTransposeContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> source, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> destination, unsigned short sourceTileId, unsigned short destinationTileId)
	{
		std::shared_ptr<MatrixTileTransposeSemanticContract> temp_var = std::make_shared<MatrixTileTransposeSemanticContract>(source, destination, sourceTileId, destinationTileId, MatrixTileTransposeAliasPolicyKind::OutOfPlaceOrSquareInPlaceOnly);
		temp_var->LayoutPolicy = MatrixTileLayoutPolicyAbi::CreateTransposePolicy();
		return temp_var;
	}

	std::shared_ptr<MatrixTileTransposeSemanticContract> MatrixTileAccumulatorAndTransposePolicyAbi::CreateTransposeContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> source, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> destination, unsigned short sourceTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileLayoutPolicy> layoutPolicy)
	{
		std::shared_ptr<MatrixTileTransposeSemanticContract> contract = CreateTransposeContract(source, destination, sourceTileId, destinationTileId);
		contract->LayoutPolicy = std::move(layoutPolicy);
		contract->HasExplicitLayoutPolicy = true;
		return contract;
	}

	std::shared_ptr<MatrixTileSemanticValidationResult> MatrixTileAccumulatorAndTransposePolicyAbi::ValidateMacc(std::shared_ptr<MatrixTileMaccSemanticContract> contract)
	{
		return ValidateMaccCore(contract, false, false);
	}

	std::shared_ptr<MatrixTileSemanticValidationResult> MatrixTileAccumulatorAndTransposePolicyAbi::ValidateRuntimeMacc(std::shared_ptr<MatrixTileMaccSemanticContract> contract)
	{
		return ValidateMaccCore(contract, true, true);
	}

	std::shared_ptr<MatrixTileSemanticValidationResult> MatrixTileAccumulatorAndTransposePolicyAbi::ValidateMaccCore(std::shared_ptr<MatrixTileMaccSemanticContract> contract, bool requireExplicitNumericPolicy, bool requireExplicitLayoutPolicy)
	{
		std::vector<MatrixTileCanonicalDescriptorAbi> descriptors{ contract->Left, contract->Right, contract->Accumulator };
		MatrixTileSemanticFaultKind descriptorFault = ValidateDescriptors(descriptors);
		if (descriptorFault != MatrixTileSemanticFaultKind::None)
		{
			return MatrixTileSemanticValidationResult::Fault(descriptorFault);
		}

		if (!IsSupportedElementKind(contract->ElementKind))
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::UnsupportedElementKind);
		}

		if (contract->AccumulatorPolicy != GetAccumulatorPolicy(contract->NumericPolicy))
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::UnsupportedAccumulatorPolicy);
		}

		if (requireExplicitNumericPolicy && !contract->HasExplicitNumericPolicy)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::MissingNumericPolicy);
		}

		if (requireExplicitLayoutPolicy && !contract->HasExplicitLayoutPolicy)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::MissingLayoutPolicy);
		}

		std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> layoutPolicy = contract->LayoutPolicy;
		std::shared_ptr<MatrixTileLayoutPolicyValidationResult> layoutValidation = MatrixTileLayoutPolicyAbi::Validate(layoutPolicy, MatrixTileProjectedOperationKind::Macc);
		if (!layoutValidation->isRuntimeOwnedLayoutPolicyAccepted())
		{
			return MatrixTileSemanticValidationResult::Fault(!contract->LayoutPolicy || *contract->LayoutPolicy == MatrixTileLayoutPolicy{} ? MatrixTileSemanticFaultKind::MissingLayoutPolicy : MatrixTileSemanticFaultKind::InvalidLayoutPolicy);
		}

		if (!MatrixTileLayoutPolicyAbi::ValidateDescriptors(contract->LayoutPolicy, descriptors)->isRuntimeOwnedLayoutPolicyAccepted())
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::LayoutPolicyDescriptorMismatch);
		}

		std::optional<std::shared_ptr<MatrixTileNumericPolicy>> numericPolicy = contract->NumericPolicy;
		std::shared_ptr<MatrixTileNumericPolicyValidationResult> numericValidation = MatrixTileNumericPolicyAbi::Validate(numericPolicy);
		if (!numericValidation->isRuntimeOwnedNumericPolicyAccepted())
		{
			const bool isMissingPolicy = !contract->NumericPolicy || IsDefaultNumericPolicy(*contract->NumericPolicy);
			return MatrixTileSemanticValidationResult::Fault(isMissingPolicy ? MatrixTileSemanticFaultKind::MissingNumericPolicy : MatrixTileSemanticFaultKind::InvalidNumericPolicy);
		}

		if (!IsSupportedSourceElementSize(contract->Left.ElementSizeBytes) || contract->Left.ElementSizeBytes != contract->Right.ElementSizeBytes)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::UnsupportedElementSize);
		}

		if (contract->Left.Columns != contract->Right.Rows)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::MaccInnerDimensionMismatch);
		}

		if (contract->Accumulator.Rows != contract->Left.Rows || contract->Accumulator.Columns != contract->Right.Columns)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::MaccAccumulatorShapeMismatch);
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ushort expectedSourceElementSize = checked((ushort) MatrixTileNumericPolicyAbi.GetElementSizeBytes(contract.NumericPolicy.ElementType));
		unsigned short expectedSourceElementSize = static_cast<unsigned short>(MatrixTileNumericPolicyAbi::GetElementSizeBytes(contract->NumericPolicy->ElementType));
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ushort expectedAccumulatorElementSize = checked((ushort) MatrixTileNumericPolicyAbi.GetElementSizeBytes(contract.NumericPolicy.AccumulatorType));
		unsigned short expectedAccumulatorElementSize = static_cast<unsigned short>(MatrixTileNumericPolicyAbi::GetElementSizeBytes(contract->NumericPolicy->AccumulatorType));
		if (contract->Left.ElementSizeBytes != expectedSourceElementSize || contract->Right.ElementSizeBytes != expectedSourceElementSize || !ElementKindMatchesPolicy(contract->ElementKind, contract->NumericPolicy))
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::NumericPolicyDescriptorMismatch);
		}

		if (contract->Accumulator.ElementSizeBytes != expectedAccumulatorElementSize)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::MaccAccumulatorElementSizeMismatch);
		}

		return MatrixTileSemanticValidationResult::Valid(std::make_shared<MatrixTileCanonicalDescriptorAbi>(contract->Accumulator), expectedAccumulatorElementSize);
	}

	std::shared_ptr<MatrixTileSemanticValidationResult> MatrixTileAccumulatorAndTransposePolicyAbi::ValidateTranspose(std::shared_ptr<MatrixTileTransposeSemanticContract> contract)
	{
		return ValidateTransposeCore(contract, false);
	}

	std::shared_ptr<MatrixTileSemanticValidationResult> MatrixTileAccumulatorAndTransposePolicyAbi::ValidateRuntimeTranspose(std::shared_ptr<MatrixTileTransposeSemanticContract> contract)
	{
		return ValidateTransposeCore(contract, true);
	}

	std::shared_ptr<MatrixTileSemanticValidationResult> MatrixTileAccumulatorAndTransposePolicyAbi::ValidateTransposeCore(std::shared_ptr<MatrixTileTransposeSemanticContract> contract, bool requireExplicitLayoutPolicy)
	{
		std::vector<MatrixTileCanonicalDescriptorAbi> descriptors{ contract->Source, contract->Destination };
		MatrixTileSemanticFaultKind descriptorFault = ValidateDescriptors(descriptors);
		if (descriptorFault != MatrixTileSemanticFaultKind::None)
		{
			return MatrixTileSemanticValidationResult::Fault(descriptorFault);
		}

		if (requireExplicitLayoutPolicy && !contract->HasExplicitLayoutPolicy)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::MissingLayoutPolicy);
		}

		std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> layoutPolicy = contract->LayoutPolicy;
		std::shared_ptr<MatrixTileLayoutPolicyValidationResult> layoutValidation = MatrixTileLayoutPolicyAbi::Validate(layoutPolicy, MatrixTileProjectedOperationKind::Transpose);
		if (!layoutValidation->isRuntimeOwnedLayoutPolicyAccepted())
		{
			return MatrixTileSemanticValidationResult::Fault(!contract->LayoutPolicy || *contract->LayoutPolicy == MatrixTileLayoutPolicy{} ? MatrixTileSemanticFaultKind::MissingLayoutPolicy : MatrixTileSemanticFaultKind::InvalidLayoutPolicy);
		}

		if (!MatrixTileLayoutPolicyAbi::ValidateDescriptors(contract->LayoutPolicy, descriptors)->isRuntimeOwnedLayoutPolicyAccepted())
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::LayoutPolicyDescriptorMismatch);
		}

		if (contract->AliasPolicy != contract->LayoutPolicy->TransposeAliasPolicy)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::InvalidLayoutPolicy);
		}

		if (contract->Source.ElementSizeBytes != contract->Destination.ElementSizeBytes)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::TransposeElementSizeMismatch);
		}

		bool isInPlace = contract->SourceTileId == contract->DestinationTileId;
		if (isInPlace)
		{
			if (contract->AliasPolicy != MatrixTileTransposeAliasPolicyKind::OutOfPlaceOrSquareInPlaceOnly)
			{
				return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::TransposeInPlaceRequiresSquareShape);
			}

			if (contract->Source.Rows != contract->Source.Columns)
			{
				return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::TransposeInPlaceRequiresSquareShape);
			}

			if (contract->Destination.Rows != contract->Source.Rows || contract->Destination.Columns != contract->Source.Columns || contract->Destination.StrideBytes != contract->Source.StrideBytes)
			{
				return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::TransposeInPlaceDescriptorMismatch);
			}
		}
		else if (contract->Destination.Rows != contract->Source.Columns || contract->Destination.Columns != contract->Source.Rows)
		{
			return MatrixTileSemanticValidationResult::Fault(MatrixTileSemanticFaultKind::TransposeShapeMismatch);
		}

		return MatrixTileSemanticValidationResult::Valid(std::make_shared<MatrixTileCanonicalDescriptorAbi>(contract->Destination), contract->Destination.ElementSizeBytes);
	}

	unsigned short MatrixTileAccumulatorAndTransposePolicyAbi::GetAccumulatorElementSizeBytes(unsigned short sourceElementSizeBytes)
	{
		int temp_var67;
		switch (sourceElementSizeBytes)
		{
			case 1:
				temp_var67 = 4;
				break;
			case 2:
				temp_var67 = 4;
				break;
			case 4:
				temp_var67 = 8;
				break;
			case 8:
				temp_var67 = 8;
				break;
			default:
				temp_var67 = 0;
				break;
		}
		return temp_var67;
	}

	bool MatrixTileAccumulatorAndTransposePolicyAbi::IsSupportedElementKind(MatrixTileNumericElementKind elementKind)
	{
		return (elementKind == MatrixTileNumericElementKind::SignedInteger || elementKind == MatrixTileNumericElementKind::UnsignedInteger || elementKind == MatrixTileNumericElementKind::FloatingPoint);
	}

	bool MatrixTileAccumulatorAndTransposePolicyAbi::IsSupportedSourceElementSize(unsigned short elementSizeBytes)
	{
		return elementSizeBytes == 1 || elementSizeBytes == 2 || elementSizeBytes == 4 || elementSizeBytes == 8;
	}

	bool MatrixTileAccumulatorAndTransposePolicyAbi::ElementKindMatchesPolicy(MatrixTileNumericElementKind elementKind, std::shared_ptr<MatrixTileNumericPolicy> policy)
	{
		if (policy == nullptr)
		{
			return false;
		}

		switch (elementKind)
		{
		case MatrixTileNumericElementKind::SignedInteger:
			return policy->Signedness == MatrixTileNumericSignedness::Signed;
		case MatrixTileNumericElementKind::UnsignedInteger:
			return policy->Signedness == MatrixTileNumericSignedness::Unsigned;
		case MatrixTileNumericElementKind::FloatingPoint:
			return policy->Signedness == MatrixTileNumericSignedness::NotApplicable;
		default:
			return false;
		}
	}

	MatrixTileAccumulatorPolicyKind MatrixTileAccumulatorAndTransposePolicyAbi::GetAccumulatorPolicy(std::shared_ptr<MatrixTileNumericPolicy> policy)
	{
		return policy != nullptr && policy->Signedness == MatrixTileNumericSignedness::NotApplicable ? MatrixTileAccumulatorPolicyKind::PreserveBinaryFloatingAccumulator : MatrixTileAccumulatorPolicyKind::WideningIntegerAccumulatorWithOverflowTrap;
	}

	bool MatrixTileAccumulatorAndTransposePolicyAbi::TryGetLegacyElementType(unsigned short elementSizeBytes, MatrixTileNumericElementKind elementKind, DataTypeEnum &elementType)
	{
		switch (elementSizeBytes)
		{
		case 1:
			if (elementKind == MatrixTileNumericElementKind::SignedInteger) { elementType = DataTypeEnum::INT8; return true; }
			if (elementKind == MatrixTileNumericElementKind::UnsignedInteger) { elementType = DataTypeEnum::UINT8; return true; }
			break;
		case 2:
			if (elementKind == MatrixTileNumericElementKind::SignedInteger) { elementType = DataTypeEnum::INT16; return true; }
			if (elementKind == MatrixTileNumericElementKind::UnsignedInteger) { elementType = DataTypeEnum::UINT16; return true; }
			break;
		case 4:
			if (elementKind == MatrixTileNumericElementKind::SignedInteger) { elementType = DataTypeEnum::INT32; return true; }
			if (elementKind == MatrixTileNumericElementKind::UnsignedInteger) { elementType = DataTypeEnum::UINT32; return true; }
			break;
		case 8:
			if (elementKind == MatrixTileNumericElementKind::SignedInteger) { elementType = DataTypeEnum::INT64; return true; }
			if (elementKind == MatrixTileNumericElementKind::UnsignedInteger) { elementType = DataTypeEnum::UINT64; return true; }
			break;
		default:
			break;
		}
		elementType = DataTypeEnum{};
		return false;
	}

	MatrixTileSemanticFaultKind MatrixTileAccumulatorAndTransposePolicyAbi::ValidateDescriptors(const std::vector<MatrixTileCanonicalDescriptorAbi> &descriptors)
	{
		for (const MatrixTileCanonicalDescriptorAbi &descriptor : descriptors)
		{
			if (descriptor.isZeroEncoding())
			{
				return MatrixTileSemanticFaultKind::ZeroDescriptor;
			}

			if (descriptor.isReservedEncoding())
			{
				return MatrixTileSemanticFaultKind::ReservedDescriptor;
			}
		}

		return MatrixTileSemanticFaultKind::None;
	}
}
