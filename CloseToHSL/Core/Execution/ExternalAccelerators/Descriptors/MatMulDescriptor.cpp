#include "MatMulDescriptor.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "../Capabilities/MatMulCapabilityProvider.h"
#include "AcceleratorDescriptorParser.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{

	std::shared_ptr<MatMulDescriptorValidationResult> MatMulDescriptorValidator::Validate(std::shared_ptr<MatMulDescriptor> descriptor, std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> commandDescriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor must not be null.");

		std::shared_ptr<MatMulDescriptorValidationResult> shape = ValidateShape(descriptor);
		if (shape->IsRejected)
		{
			return shape;
		}

		std::shared_ptr<MatMulDescriptorValidationResult> datatypes = ValidateDatatypes(descriptor);
		if (datatypes->IsRejected)
		{
			return datatypes;
		}

		std::shared_ptr<MatMulDescriptorValidationResult> strides = ValidateStrides(descriptor);
		if (strides->IsRejected)
		{
			return strides;
		}

		std::shared_ptr<MatMulDescriptorValidationResult> footprint = NormalizeFootprints(descriptor);
		if (footprint->IsRejected)
		{
			return footprint;
		}

		if (commandDescriptor && *commandDescriptor)
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: MatMulDescriptorValidationResult binding = ValidateCommandDescriptorBinding( descriptor, footprint.Footprint!, commandDescriptor.value());
			std::shared_ptr<MatMulDescriptorValidationResult> binding = ValidateCommandDescriptorBinding(descriptor, footprint->Footprint, commandDescriptor.value());
			if (binding->IsRejected)
			{
				return binding;
			}
		}

		return footprint;
	}

	std::shared_ptr<MatMulDescriptorValidationResult> MatMulDescriptorValidator::ValidateShape(std::shared_ptr<MatMulDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor must not be null.");

		if (descriptor->M == 0 || descriptor->N == 0 || descriptor->K == 0)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedShape, L"MatMul descriptor dimensions M, N, and K must be non-zero.");
		}

		if (descriptor->TileM == 0 || descriptor->TileN == 0 || descriptor->TileK == 0 || descriptor->TileM > descriptor->M || descriptor->TileN > descriptor->N || descriptor->TileK > descriptor->K)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedShape, L"MatMul tile dimensions must be non-zero and must not exceed M, N, or K.");
		}

		unsigned long long outputElements;
		unsigned long long outputElementsWithK;
		if (!TryMul(descriptor->M, descriptor->N, outputElements) || !TryMul(outputElements, descriptor->K, outputElementsWithK))
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::RangeOverflow, L"MatMul descriptor shape overflows the v1 resource model.");
		}

		if (outputElements > MaxOutputElements)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedShape, L"MatMul descriptor output shape exceeds the v1 provider-supported element limit.");
		}
	auto temp_var = std::make_shared<MatMulFootprint>();
		return MatMulDescriptorValidationResult::Valid(descriptor, temp_var, L"MatMul shape accepted.");
	}

	std::shared_ptr<MatMulDescriptorValidationResult> MatMulDescriptorValidator::ValidateStrides(std::shared_ptr<MatMulDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor must not be null.");

		MatMulLayoutFlags unknown = descriptor->LayoutFlags & ~SupportedLayoutMask;
		if (unknown != (YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors::MatMulLayoutFlags)0 || (descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorA) == MatMulLayoutFlags::None || (descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorB) == MatMulLayoutFlags::None || (descriptor->LayoutFlags & MatMulLayoutFlags::RowMajorC) == MatMulLayoutFlags::None)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedShape, L"MatMul v1 accepts explicit row-major A, B, and C layout flags only.");
		}

		if ((descriptor->LayoutFlags & MatMulLayoutFlags::TransposeA) != MatMulLayoutFlags::None || (descriptor->LayoutFlags & MatMulLayoutFlags::TransposeB) != MatMulLayoutFlags::None)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedShape, L"MatMul v1 descriptor validation rejects transposed layouts conservatively.");
		}

		if (descriptor->Lda < descriptor->K || descriptor->Ldb < descriptor->N || descriptor->Ldc < descriptor->N)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedShape, L"MatMul strides must cover the logical row width for row-major A, B, and C.");
		}

		unsigned short alignment = static_cast<unsigned short>(std::max(DatatypeSizeBytes(descriptor->Datatypes.InputDatatype), DatatypeSizeBytes(descriptor->Datatypes.OutputDatatype)));
		if (!IsAligned(descriptor->ABase, alignment) || !IsAligned(descriptor->BBase, alignment) || !IsAligned(descriptor->CBase, alignment))
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::AlignmentFault, L"MatMul base addresses must satisfy datatype alignment.");
		}
	auto temp_var = std::make_shared<MatMulFootprint>();
		return MatMulDescriptorValidationResult::Valid(descriptor, temp_var, L"MatMul strides accepted.");
	}

	std::shared_ptr<MatMulDescriptorValidationResult> MatMulDescriptorValidator::ValidateDatatypes(std::shared_ptr<MatMulDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor must not be null.");

		if (descriptor->PartialPolicy != AcceleratorPartialCompletionPolicy::AllOrNone)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedPartialCompletionPolicy, L"MatMul v1 requires AllOrNone partial completion policy.");
		}

		const MatMulDatatypeTriple& triple = descriptor->Datatypes;
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		bool supported = (triple.InputDatatype == AcceleratorDatatype::Float32 && triple.AccumulatorDatatype == AcceleratorDatatype::Float32 && triple.OutputDatatype == AcceleratorDatatype::Float32) || (triple.InputDatatype == AcceleratorDatatype::Float64 && triple.AccumulatorDatatype == AcceleratorDatatype::Float64 && triple.OutputDatatype == AcceleratorDatatype::Float64) || (triple.InputDatatype == AcceleratorDatatype::Int32 && triple.AccumulatorDatatype == AcceleratorDatatype::Int32 && triple.OutputDatatype == AcceleratorDatatype::Int32);
		if (!supported)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedDatatype, L"MatMul v1 supports only homogeneous f32, f64, or int32 datatype triples.");
		}
	auto temp_var = std::make_shared<MatMulFootprint>();
		return MatMulDescriptorValidationResult::Valid(descriptor, temp_var, L"MatMul datatype triple accepted.");
	}

	std::shared_ptr<MatMulDescriptorValidationResult> MatMulDescriptorValidator::NormalizeFootprints(std::shared_ptr<MatMulDescriptor> descriptor)
	{
		if (!descriptor) throw std::invalid_argument("descriptor must not be null.");

		unsigned short inputBytes = DatatypeSizeBytes(descriptor->Datatypes.InputDatatype);
		unsigned short outputBytes = DatatypeSizeBytes(descriptor->Datatypes.OutputDatatype);
		unsigned long long aBytes;
		unsigned long long bBytes;
		unsigned long long cBytes;
		if (!TryMatrixSpanBytes(descriptor->M, descriptor->K, descriptor->Lda, inputBytes, aBytes) || !TryMatrixSpanBytes(descriptor->K, descriptor->N, descriptor->Ldb, inputBytes, bBytes) || !TryMatrixSpanBytes(descriptor->M, descriptor->N, descriptor->Ldc, outputBytes, cBytes))
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::RangeOverflow, L"MatMul descriptor footprint overflows UInt64.");
		}

		if (!IsRangeValid(descriptor->ABase, aBytes) || !IsRangeValid(descriptor->BBase, bBytes) || !IsRangeValid(descriptor->CBase, cBytes))
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::RangeOverflow, L"MatMul descriptor base plus footprint length overflows UInt64.");
		}

		auto footprint = std::make_shared<MatMulFootprint>();
		footprint->ARange = std::make_shared<AcceleratorMemoryRange>(descriptor->ABase, aBytes);
		footprint->BRange = std::make_shared<AcceleratorMemoryRange>(descriptor->BBase, bBytes);
		footprint->CRange = std::make_shared<AcceleratorMemoryRange>(descriptor->CBase, cBytes);

		std::wstring aliasMessage;
		if (HasAmbiguousAliases(footprint, aliasMessage))
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::AliasAmbiguousFootprint, aliasMessage);
		}

		return MatMulDescriptorValidationResult::Valid(descriptor, footprint, L"MatMul descriptor footprints normalized as non-authoritative read/write evidence.");
	}

	unsigned short MatMulDescriptorValidator::DatatypeSizeBytes(AcceleratorDatatype datatype)
	{
		int temp_var49;
		switch (datatype)
		{
			case AcceleratorDatatype::Float32:
				temp_var49 = sizeof(float);
				break;
			case AcceleratorDatatype::Float64:
				temp_var49 = sizeof(double);
				break;
			case AcceleratorDatatype::Int32:
				temp_var49 = sizeof(int);
				break;
			default:
				temp_var49 = 0;
				break;
		}
		return temp_var49;
	}

	std::shared_ptr<MatMulDescriptorValidationResult> MatMulDescriptorValidator::ValidateCommandDescriptorBinding(std::shared_ptr<MatMulDescriptor> descriptor, std::shared_ptr<MatMulFootprint> footprint, std::shared_ptr<AcceleratorCommandDescriptor> commandDescriptor)
	{
		if (!MatMulCapabilityProvider::Matches(commandDescriptor))
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedOperation, L"MatMul typed descriptor is not bound to a ReferenceMatMul L7-SDC command descriptor.");
		}

		if (!commandDescriptor->OwnerGuardDecision || !commandDescriptor->OwnerGuardDecision->IsAllowed)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::OwnerDomainFault, L"MatMul typed descriptor requires the carrier command descriptor to be owner/domain guard-backed.");
		}

		if (commandDescriptor->PartialCompletionPolicy != descriptor->PartialPolicy)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedPartialCompletionPolicy, L"MatMul typed descriptor partial policy does not match the command descriptor.");
		}

		if (commandDescriptor->Datatype != descriptor->Datatypes.OutputDatatype)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedDatatype, L"MatMul command descriptor datatype must match the typed output datatype.");
		}

		if (commandDescriptor->Shape != AcceleratorShapeKind::Matrix2D || commandDescriptor->ShapeRank != 2)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedShape, L"MatMul command descriptor must carry a rank-2 matrix shape.");
		}

		unsigned long long outputElements;
		unsigned long long outputElementsWithK;
		if (!TryMul(descriptor->M, descriptor->N, outputElements) || commandDescriptor->ElementCount != outputElements)
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::UnsupportedShape, L"MatMul command descriptor element count must match M*N from the typed descriptor.");
		}

		if (!RangesEqual(AcceleratorDescriptorParser::NormalizeMemoryRanges(footprint->SourceRanges()), commandDescriptor->NormalizedFootprint.SourceRanges) || !RangesEqual(AcceleratorDescriptorParser::NormalizeMemoryRanges(footprint->DestinationRanges()), commandDescriptor->NormalizedFootprint.DestinationRanges))
		{
			return MatMulDescriptorValidationResult::Reject(AcceleratorDescriptorFault::NormalizedFootprintHashMismatch, L"MatMul typed descriptor footprints must match the guard-accepted L7-SDC command descriptor footprint.");
		}

		return MatMulDescriptorValidationResult::Valid(descriptor, footprint, L"MatMul typed descriptor is bound to the guard-accepted L7-SDC command descriptor.");
	}

	bool MatMulDescriptorValidator::TryMatrixSpanBytes(unsigned int rows, unsigned int cols, unsigned int stride, unsigned short elementSize, unsigned long long &bytes)
	{
		bytes = 0;
		if (rows == 0 || cols == 0 || stride < cols || elementSize == 0)
		{
			return false;
		}

		unsigned long long logicalElements = (static_cast<unsigned long long>(rows - 1) * stride) + cols;
		if (!TryMul(logicalElements, elementSize, bytes))
		{
			return false;
		}

		return bytes != 0;
	}

	bool MatMulDescriptorValidator::TryMul(unsigned long long left, unsigned long long right, unsigned long long &result)
	{
		result = 0;
		if (left != 0 && right > std::numeric_limits<unsigned long long>::max() / left)
		{
			return false;
		}

		result = left * right;
		return true;
	}

	bool MatMulDescriptorValidator::IsRangeValid(unsigned long long address, unsigned long long length)
	{
		return length != 0 && address <= std::numeric_limits<unsigned long long>::max() - length;
	}

	bool MatMulDescriptorValidator::IsAligned(unsigned long long address, unsigned short alignment)
	{
		return alignment != 0 && (address % alignment) == 0;
	}

	bool MatMulDescriptorValidator::HasAmbiguousAliases(std::shared_ptr<MatMulFootprint> footprint, std::wstring &message)
	{
		if (RangesOverlap(footprint->ARange, footprint->BRange))
		{
			message = L"MatMul descriptor rejects overlapping A and B source footprints conservatively.";
			return true;
		}

		if (RangesOverlap(footprint->ARange, footprint->CRange))
		{
			message = L"MatMul descriptor rejects overlapping A source and C destination footprints conservatively.";
			return true;
		}

		if (RangesOverlap(footprint->BRange, footprint->CRange))
		{
			message = L"MatMul descriptor rejects overlapping B source and C destination footprints conservatively.";
			return true;
		}

		message = L"";
		return false;
	}

	bool MatMulDescriptorValidator::RangesOverlap(std::shared_ptr<AcceleratorMemoryRange> left, std::shared_ptr<AcceleratorMemoryRange> right)
	{
		if (left->Length == 0 || right->Length == 0 || left->Address > std::numeric_limits<unsigned long long>::max() - left->Length || right->Address > std::numeric_limits<unsigned long long>::max() - right->Length)
		{
			return false;
		}

		unsigned long long leftEnd = left->Address + left->Length;
		unsigned long long rightEnd = right->Address + right->Length;
		return left->Address < rightEnd && right->Address < leftEnd;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool RangesEqual(IReadOnlyList<AcceleratorMemoryRange> left, IReadOnlyList<AcceleratorMemoryRange> right)
	bool MatMulDescriptorValidator::RangesEqual(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &left, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &right)
	{
		if (left.size() != right.size())
		{
			return false;
		}

		for (int index = 0; index < left.size(); index++)
		{
			if (!(*left[index] == *right[index]))
			{
				return false;
			}
		}

		return true;
	}
}
