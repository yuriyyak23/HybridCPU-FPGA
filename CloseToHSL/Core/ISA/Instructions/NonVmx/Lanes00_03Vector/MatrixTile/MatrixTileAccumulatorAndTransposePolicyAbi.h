#pragma once
#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"

#include "../../../../../../../Arch/DataTypeEnum.h"
#include <string>
#include <vector>
#include <memory>

using namespace HybridCPU_ISE::Arch;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	enum class MatrixTileNumericElementKind : unsigned char
	{
		Unspecified = 0,
		SignedInteger = 1,
		UnsignedInteger = 2,
		FloatingPoint = 3
	};

	enum class MatrixTileAccumulatorPolicyKind : unsigned char
	{
		Unspecified = 0,
		WideningIntegerAccumulatorWithOverflowTrap = 1,
		PreserveBinaryFloatingAccumulator = 2
	};

	enum class MatrixTileTransposeAliasPolicyKind : unsigned char
	{
		Unspecified = 0,
		OutOfPlaceOrSquareInPlaceOnly = 1
	};

	enum class MatrixTileSemanticFaultKind : unsigned char
	{
		None = 0,
		ZeroDescriptor = 1,
		ReservedDescriptor = 2,
		UnsupportedElementSize = 3,
		UnsupportedElementKind = 4,
		MaccInnerDimensionMismatch = 5,
		MaccAccumulatorShapeMismatch = 6,
		MaccAccumulatorElementSizeMismatch = 7,
		TransposeShapeMismatch = 8,
		TransposeElementSizeMismatch = 9,
		TransposeInPlaceRequiresSquareShape = 10,
		TransposeInPlaceDescriptorMismatch = 11,
		MissingNumericPolicy = 12,
		InvalidNumericPolicy = 13,
		NumericPolicyDescriptorMismatch = 14,
		UnsupportedAccumulatorPolicy = 15,
		MissingLayoutPolicy = 16,
		InvalidLayoutPolicy = 17,
		LayoutPolicyDescriptorMismatch = 18
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileMaccSemanticContract(MatrixTileCanonicalDescriptorAbi Left, MatrixTileCanonicalDescriptorAbi Right, MatrixTileCanonicalDescriptorAbi Accumulator, MatrixTileNumericElementKind ElementKind, MatrixTileAccumulatorPolicyKind AccumulatorPolicy)
//	{
//		public MatrixTileNumericPolicy NumericPolicy
//		{
//			get;
//			set;
//		}
//
//		public bool HasExplicitNumericPolicy
//		{
//			get;
//			set;
//		}
//
//		public MatrixTileLayoutPolicy LayoutPolicy
//		{
//			get;
//			set;
//		}
//
//		public bool HasExplicitLayoutPolicy
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileTransposeSemanticContract(MatrixTileCanonicalDescriptorAbi Source, MatrixTileCanonicalDescriptorAbi Destination, ushort SourceTileId, ushort DestinationTileId, MatrixTileTransposeAliasPolicyKind AliasPolicy)
//	{
//		public MatrixTileLayoutPolicy LayoutPolicy
//		{
//			get;
//			set;
//		}
//
//		public bool HasExplicitLayoutPolicy
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileSemanticValidationResult(bool IsValid, MatrixTileSemanticFaultKind FaultKind, MatrixTileCanonicalDescriptorAbi ResultDescriptor, ushort ResultElementSizeBytes, bool RequiresRetirePublication, bool RequiresReplayIdentity, bool UsesFallbackPath)
//	{
//		public bool IsSemanticAbiAccepted
//		{
//			get
//			{
//				return IsValid;
//			}
//		}
//
//		public static MatrixTileSemanticValidationResult Valid(MatrixTileCanonicalDescriptorAbi resultDescriptor, ushort resultElementSizeBytes)
//		{
//			return new MatrixTileSemanticValidationResult(IsValid: true, FaultKind: MatrixTileSemanticFaultKind.None, resultDescriptor, resultElementSizeBytes, RequiresRetirePublication: true, RequiresReplayIdentity: true, UsesFallbackPath: false);
//		}
//
//		public static MatrixTileSemanticValidationResult Fault(MatrixTileSemanticFaultKind faultKind)
//		{
//			return new MatrixTileSemanticValidationResult(IsValid: false, faultKind, ResultDescriptor: default, ResultElementSizeBytes: 0, RequiresRetirePublication: false, RequiresReplayIdentity: false, UsesFallbackPath: false);
//		}
//	}

	struct MatrixTileNumericPolicy;
	struct MatrixTileLayoutPolicy;
	struct MatrixTileMaccSemanticContract final {
		MatrixTileCanonicalDescriptorAbi Left, Right, Accumulator;
		MatrixTileNumericElementKind ElementKind;
		MatrixTileAccumulatorPolicyKind AccumulatorPolicy;
		std::shared_ptr<MatrixTileNumericPolicy> NumericPolicy;
		bool HasExplicitNumericPolicy = false;
		std::shared_ptr<MatrixTileLayoutPolicy> LayoutPolicy;
		bool HasExplicitLayoutPolicy = false;
		MatrixTileMaccSemanticContract(const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& left, const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& right, const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& accumulator, MatrixTileNumericElementKind elementKind, MatrixTileAccumulatorPolicyKind policy) : Left(*left), Right(*right), Accumulator(*accumulator), ElementKind(elementKind), AccumulatorPolicy(policy) {}
	};
	struct MatrixTileTransposeSemanticContract final {
		MatrixTileCanonicalDescriptorAbi Source, Destination;
		unsigned short SourceTileId, DestinationTileId;
		MatrixTileTransposeAliasPolicyKind AliasPolicy;
		std::shared_ptr<MatrixTileLayoutPolicy> LayoutPolicy;
		bool HasExplicitLayoutPolicy = false;
		MatrixTileTransposeSemanticContract(const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& source, const std::shared_ptr<MatrixTileCanonicalDescriptorAbi>& destination, unsigned short sourceTileId, unsigned short destinationTileId, MatrixTileTransposeAliasPolicyKind aliasPolicy) : Source(*source), Destination(*destination), SourceTileId(sourceTileId), DestinationTileId(destinationTileId), AliasPolicy(aliasPolicy) {}
	};
	struct MatrixTileCanonicalDescriptorAbi;
	struct MatrixTileSemanticValidationResult final {
		bool IsValid = false;
		MatrixTileSemanticFaultKind FaultKind = MatrixTileSemanticFaultKind::None;
		std::shared_ptr<MatrixTileCanonicalDescriptorAbi> ResultDescriptor;
		unsigned short ResultElementSizeBytes = 0;
		bool RequiresRetirePublication = false, RequiresReplayIdentity = false, UsesFallbackPath = false;
		bool isSemanticAbiAccepted() const noexcept { return IsValid; }
		static std::shared_ptr<MatrixTileSemanticValidationResult> Valid(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned short size) {
			return std::make_shared<MatrixTileSemanticValidationResult>(MatrixTileSemanticValidationResult{true, MatrixTileSemanticFaultKind::None, std::move(descriptor), size, true, true, false});
		}
		static std::shared_ptr<MatrixTileSemanticValidationResult> Fault(MatrixTileSemanticFaultKind fault) {
			return std::make_shared<MatrixTileSemanticValidationResult>(MatrixTileSemanticValidationResult{false, fault, nullptr, 0, false, false, false});
		}
	};
	class MatrixTileAccumulatorAndTransposePolicyAbi final
	{
	public:
		static inline const std::wstring AccumulatorTransposeDecision = L"ClosedAccumulatorAndTransposeSemanticAbi";
		static inline const std::wstring MaccAccumulatorDecision = L"ClosedAccumulatorTileAbi";
		static inline const std::wstring MaccShapeDecision = L"MaccRowsKColumnsShapeCompatibilityValidated";
		static inline const std::wstring MaccDtypeDecision = L"WideningIntegerAccumulatorDtypePolicySelected";
		static inline const std::wstring MaccExceptionDecision = L"AccumulatorOverflowTrapPolicySelected";
		static inline const std::wstring MaccRetireReplayDecision = L"RetireOwnedAccumulatorPublicationAndReplayIdentitySelected";
		static inline const std::wstring TransposeCarrierDecision = L"ClosedTransposePolicyAbi";
		static inline const std::wstring TransposeAliasDecision = L"OutOfPlaceOrSquareInPlaceAliasPolicySelected";
		static inline const std::wstring TransposeLayoutDecision = L"RowMajorTransposeShapePermutationSelected";
		static inline const std::wstring TransposeRetireReplayDecision = L"RetireOwnedTransposePublicationAndReplayIdentitySelected";
		static inline const std::wstring FallbackDecision = L"VectorTransposeOrExternalBackendEvidenceIsNotMtileAuthority";

		static constexpr bool HasAccumulatorTileAbi = true;
		static constexpr bool HasTransposePolicyAbi = true;
		static constexpr bool HasAccumulatorTileStateOwner = true;
		static constexpr bool HasAccumulatorTileFootprintPolicy = true;
		static constexpr bool HasAccumulatorDtypePolicy = true;
		static constexpr bool HasMaccShapeCompatibilityPolicy = true;
		static constexpr bool HasMaccExceptionOrSaturationPolicy = true;
		static constexpr bool HasTransposePolicyCarrier = true;
		static constexpr bool HasTransposeSourceDestinationAliasPolicy = true;
		static constexpr bool HasInPlaceTransposePolicy = true;
		static constexpr bool HasTransposeLayoutPermutationPolicy = true;
		static constexpr bool HasInvalidShapeTypeAliasDeterminism = true;
		static constexpr bool KeepsVectorTransposeNonAuthority = true;
		static constexpr bool KeepsExternalBackendNonAuthority = true;
		static constexpr bool KeepsCompilerMatrixIrIndependent = true;
		static constexpr bool KeepsCompilerHandoffBlocked = true;

		static std::shared_ptr<MatrixTileMaccSemanticContract> CreateMaccContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> left, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> right, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> accumulator, MatrixTileNumericElementKind elementKind = MatrixTileNumericElementKind::SignedInteger);

		static std::shared_ptr<MatrixTileMaccSemanticContract> CreateMaccContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> left, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> right, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> accumulator, std::shared_ptr<MatrixTileNumericPolicy> numericPolicy);

		static std::shared_ptr<MatrixTileMaccSemanticContract> CreateMaccContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> left, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> right, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> accumulator, std::shared_ptr<MatrixTileNumericPolicy> numericPolicy, std::shared_ptr<MatrixTileLayoutPolicy> layoutPolicy);

		static std::shared_ptr<MatrixTileTransposeSemanticContract> CreateTransposeContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> source, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> destination, unsigned short sourceTileId, unsigned short destinationTileId);

		static std::shared_ptr<MatrixTileTransposeSemanticContract> CreateTransposeContract(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> source, std::shared_ptr<MatrixTileCanonicalDescriptorAbi> destination, unsigned short sourceTileId, unsigned short destinationTileId, std::shared_ptr<MatrixTileLayoutPolicy> layoutPolicy);

		static std::shared_ptr<MatrixTileSemanticValidationResult> ValidateMacc(std::shared_ptr<MatrixTileMaccSemanticContract> contract);

		static std::shared_ptr<MatrixTileSemanticValidationResult> ValidateRuntimeMacc(std::shared_ptr<MatrixTileMaccSemanticContract> contract);

	private:
		static std::shared_ptr<MatrixTileSemanticValidationResult> ValidateMaccCore(std::shared_ptr<MatrixTileMaccSemanticContract> contract, bool requireExplicitNumericPolicy, bool requireExplicitLayoutPolicy = false);

	public:
		static std::shared_ptr<MatrixTileSemanticValidationResult> ValidateTranspose(std::shared_ptr<MatrixTileTransposeSemanticContract> contract);

		static std::shared_ptr<MatrixTileSemanticValidationResult> ValidateRuntimeTranspose(std::shared_ptr<MatrixTileTransposeSemanticContract> contract);

	private:
		static std::shared_ptr<MatrixTileSemanticValidationResult> ValidateTransposeCore(std::shared_ptr<MatrixTileTransposeSemanticContract> contract, bool requireExplicitLayoutPolicy);

	public:
		static unsigned short GetAccumulatorElementSizeBytes(unsigned short sourceElementSizeBytes);

		static bool IsSupportedElementKind(MatrixTileNumericElementKind elementKind);

		static bool IsSupportedSourceElementSize(unsigned short elementSizeBytes);

	private:
		static bool ElementKindMatchesPolicy(MatrixTileNumericElementKind elementKind, std::shared_ptr<MatrixTileNumericPolicy> policy);

		static MatrixTileAccumulatorPolicyKind GetAccumulatorPolicy(std::shared_ptr<MatrixTileNumericPolicy> policy);

		static bool TryGetLegacyElementType(unsigned short elementSizeBytes, MatrixTileNumericElementKind elementKind, DataTypeEnum &elementType);

		static MatrixTileSemanticFaultKind ValidateDescriptors(const std::vector<MatrixTileCanonicalDescriptorAbi> &descriptors);
	};

}
