#pragma once

#include "MatrixTileIrProjectionAndMaterializer.h"
#include <string>
#include <vector>
#include <optional>
#include "tangible_exception_helper.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	enum class MatrixTileLayoutProfileId : unsigned char
	{
		Unspecified = 0,
		MaccCanonicalRowMajorAscendingK = 1,
		TransposeCanonicalRowMajor = 2
	};

	enum class MatrixTileElementAddressingKind : unsigned char
	{
		Unspecified = 0,
		NotApplicable = 1,
		CanonicalPackedRowMajor = 2,
		ColumnMajor = 3,
		Blocked = 4,
		Interleaved = 5
	};

	enum class MatrixTileKIterationOrderKind : unsigned char
	{
		Unspecified = 0,
		NotApplicable = 1,
		AscendingZeroToKMinusOne = 2
	};

	enum class MatrixTileTransposePermutationKind : unsigned char
	{
		Unspecified = 0,
		NotApplicable = 1,
		DestinationColumnRowFromSourceRowColumn = 2
	};

	enum class MatrixTileLayoutPolicyFaultKind : unsigned char
	{
		None = 0,
		MissingPolicy = 1,
		UnsupportedAbiVersion = 2,
		ReservedProfile = 3,
		UnsupportedOperation = 4,
		FingerprintMismatch = 5,
		ContradictoryRule = 6,
		DescriptorLayoutMismatch = 7
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileLayoutPolicy(ushort AbiVersion, MatrixTileLayoutProfileId ProfileId, MatrixTileProjectedOperationKind OperationKind, MatrixTileElementAddressingKind SourceAddressing, MatrixTileElementAddressingKind SecondaryAddressing, MatrixTileElementAddressingKind DestinationAddressing, MatrixTileKIterationOrderKind KIterationOrder, MatrixTileTransposePermutationKind TransposePermutation, MatrixTileTransposeAliasPolicyKind TransposeAliasPolicy, ulong Fingerprint);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileLayoutPolicyValidationResult(bool IsValid, MatrixTileLayoutPolicyFaultKind FaultKind)
//	{
//		public bool IsRuntimeOwnedLayoutPolicyAccepted
//		{
//			get
//			{
//				return IsValid;
//			}
//		}
//
//		public static MatrixTileLayoutPolicyValidationResult Valid()
//		{
//			return new(true, MatrixTileLayoutPolicyFaultKind.None);
//		}
//
//		public static MatrixTileLayoutPolicyValidationResult Fault(MatrixTileLayoutPolicyFaultKind faultKind)
//		{
//			return new(false, faultKind);
//		}
//	}

	struct MatrixTileLayoutPolicy final
	{
		unsigned short AbiVersion = 0;
		MatrixTileLayoutProfileId ProfileId{};
		MatrixTileProjectedOperationKind OperationKind{};
		MatrixTileElementAddressingKind SourceAddressing{}, SecondaryAddressing{}, DestinationAddressing{};
		MatrixTileKIterationOrderKind KIterationOrder{};
		MatrixTileTransposePermutationKind TransposePermutation{};
		MatrixTileTransposeAliasPolicyKind TransposeAliasPolicy{};
		unsigned long long Fingerprint = 0;
		bool operator==(const MatrixTileLayoutPolicy&) const = default;
	};

	struct MatrixTileLayoutPolicyValidationResult final
	{
		bool IsValid = false;
		MatrixTileLayoutPolicyFaultKind FaultKind = MatrixTileLayoutPolicyFaultKind::None;
		bool isRuntimeOwnedLayoutPolicyAccepted() const noexcept { return IsValid; }
		static std::shared_ptr<MatrixTileLayoutPolicyValidationResult> Valid() { return std::make_shared<MatrixTileLayoutPolicyValidationResult>(true, MatrixTileLayoutPolicyFaultKind::None); }
		static std::shared_ptr<MatrixTileLayoutPolicyValidationResult> Fault(MatrixTileLayoutPolicyFaultKind kind) { return std::make_shared<MatrixTileLayoutPolicyValidationResult>(false, kind); }
	};

	class MatrixTileLayoutPolicyAbi final
	{
	public:
		static constexpr unsigned short CurrentAbiVersion = 1;
		static inline const std::wstring PolicyDecision = L"ClosedVersionedRuntimeOwnedMatrixTileLayoutPolicyAbi";
		static inline const std::wstring MaccLayoutDecision = L"CanonicalPackedRowMajorWithAscendingKOrder";
		static inline const std::wstring TransposeLayoutDecision = L"CanonicalPackedRowMajorCoordinatePermutation";
		static inline const std::wstring UnsupportedLayoutDecision = L"ColumnMajorBlockedAndInterleavedFailClosed";
		static constexpr bool HasImplicitLayoutDefaults = false;
		static constexpr bool UsesDescriptorLayoutAsPolicyAuthority = false;
		static constexpr bool UsesCompilerMetadataAsLayoutAuthority = false;

		static std::shared_ptr<MatrixTileLayoutPolicy> CreateMaccPolicy();

		static std::shared_ptr<MatrixTileLayoutPolicy> CreateTransposePolicy();

		static std::shared_ptr<MatrixTileLayoutPolicyValidationResult> Validate(std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> &policy, MatrixTileProjectedOperationKind expectedOperationKind);

		static std::shared_ptr<MatrixTileLayoutPolicyValidationResult> ValidateDescriptors(std::shared_ptr<MatrixTileLayoutPolicy> policy, std::vector<MatrixTileCanonicalDescriptorAbi> &descriptors);

		static int GetPackedOffset(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing);

		static unsigned long long ComputeFingerprint(std::shared_ptr<MatrixTileLayoutPolicy> policy);

	private:
		static void Mix(unsigned long long &hash, unsigned short value);

		static void Mix(unsigned long long &hash, unsigned char value);

	};

}
