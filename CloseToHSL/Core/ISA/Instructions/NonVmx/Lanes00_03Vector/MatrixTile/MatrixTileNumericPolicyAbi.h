#pragma once

#include "../../../../../../../Arch/DataTypeEnum.h"
#include <string>
#include <vector>
#include <optional>
#include <format>
#include <memory>

using namespace HybridCPU_ISE::Arch;


namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	enum class MatrixTileNumericProfileId : unsigned char
	{
		Unspecified = 0,
		SignedInt8ToInt32 = 1,
		UnsignedInt8ToUInt32 = 2,
		SignedInt16ToInt32 = 3,
		UnsignedInt16ToUInt32 = 4,
		SignedInt32ToInt64 = 5,
		UnsignedInt32ToUInt64 = 6,
		SignedInt64ToInt64 = 7,
		UnsignedInt64ToUInt64 = 8,
		Binary32ToBinary32 = 9,
		Binary64ToBinary64 = 10
	};

	enum class MatrixTileNumericSignedness : unsigned char
	{
		Unspecified = 0,
		Signed = 1,
		Unsigned = 2,
		NotApplicable = 3
	};

	enum class MatrixTileNumericWideningRule : unsigned char
	{
		Unspecified = 0,
		SignExtendToAccumulator = 1,
		ZeroExtendToAccumulator = 2,
		PreserveBinaryFormat = 3
	};

	enum class MatrixTileNumericMultiplyRule : unsigned char
	{
		Unspecified = 0,
		ExactIntegerProduct = 1,
		SeparatelyRoundedIeee754Product = 2
	};

	enum class MatrixTileNumericAddRule : unsigned char
	{
		Unspecified = 0,
		ExactUnboundedIntermediate = 1,
		SeparatelyRoundedIeee754Add = 2
	};

	enum class MatrixTileNumericRoundingMode : unsigned char
	{
		Unspecified = 0,
		NotApplicableExactInteger = 1,
		ToNearestTiesToEven = 2
	};

	enum class MatrixTileNumericSaturationMode : unsigned char
	{
		Unspecified = 0,
		Disabled = 1
	};

	enum class MatrixTileNumericOverflowMode : unsigned char
	{
		Unspecified = 0,
		TrapOnFinalAccumulatorEncoding = 1,
		Ieee754Infinity = 2
	};

	enum class MatrixTileNumericNaNPolicy : unsigned char
	{
		Unspecified = 0,
		NotApplicable = 1,
		CanonicalQuietNaN = 2
	};

	enum class MatrixTileNumericInfinityPolicy : unsigned char
	{
		Unspecified = 0,
		NotApplicable = 1,
		PreserveIeee754Infinity = 2
	};

	enum class MatrixTileNumericDenormalPolicy : unsigned char
	{
		Unspecified = 0,
		NotApplicable = 1,
		Preserve = 2
	};

	enum class MatrixTileNumericReproducibilityMode : unsigned char
	{
		Unspecified = 0,
		ExactIntegerLittleEndian = 1,
		DeterministicIeee754LittleEndian = 2
	};

	enum class MatrixTileNumericExceptionPolicy : unsigned char
	{
		Unspecified = 0,
		CaptureTypedArithmeticFault = 1,
		PublishCanonicalIeee754Result = 2
	};

	enum class MatrixTileNumericPolicyFaultKind : unsigned char
	{
		None = 0,
		MissingPolicy = 1,
		UnsupportedAbiVersion = 2,
		ReservedProfile = 3,
		UnsupportedProfile = 4,
		FingerprintMismatch = 5,
		ContradictoryElementType = 6,
		ContradictoryAccumulatorType = 7,
		ContradictoryPublishFormat = 8,
		ContradictorySignedness = 9,
		ContradictoryRule = 10
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileNumericPolicy(ushort AbiVersion, MatrixTileNumericProfileId ProfileId, DataTypeEnum ElementType, DataTypeEnum AccumulatorType, DataTypeEnum PublishFormat, MatrixTileNumericSignedness Signedness, MatrixTileNumericWideningRule WideningRule, MatrixTileNumericMultiplyRule MultiplyRule, MatrixTileNumericAddRule AddRule, MatrixTileNumericRoundingMode RoundingMode, MatrixTileNumericSaturationMode SaturationMode, MatrixTileNumericOverflowMode OverflowMode, MatrixTileNumericNaNPolicy NaNPolicy, MatrixTileNumericInfinityPolicy InfinityPolicy, MatrixTileNumericDenormalPolicy DenormalPolicy, MatrixTileNumericReproducibilityMode ReproducibilityMode, MatrixTileNumericExceptionPolicy ExceptionPolicy, ulong Fingerprint);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileNumericProfileRow(MatrixTileNumericProfileId ProfileId, DataTypeEnum ElementType, DataTypeEnum AccumulatorType, DataTypeEnum PublishFormat, bool IsExecutionSupported, string SupportDecision);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct MatrixTileNumericPolicyValidationResult(bool IsValid, MatrixTileNumericPolicyFaultKind FaultKind, MatrixTileNumericProfileRow Profile)
//	{
//		public bool IsRuntimeOwnedNumericPolicyAccepted
//		{
//			get
//			{
//				return IsValid;
//			}
//		}
//
//		public static MatrixTileNumericPolicyValidationResult Valid(MatrixTileNumericProfileRow profile)
//		{
//			return new(true, MatrixTileNumericPolicyFaultKind.None, profile);
//		}
//
//		public static MatrixTileNumericPolicyValidationResult Fault(MatrixTileNumericPolicyFaultKind faultKind, MatrixTileNumericProfileRow profile = default)
//		{
//			return new(false, faultKind, profile);
//		}
//	}

	struct MatrixTileNumericPolicy final {
		unsigned short AbiVersion = 0;
		MatrixTileNumericProfileId ProfileId = MatrixTileNumericProfileId::Unspecified;
		DataTypeEnum ElementType{}, AccumulatorType{}, PublishFormat{};
		MatrixTileNumericSignedness Signedness{};
		MatrixTileNumericWideningRule WideningRule{};
		MatrixTileNumericMultiplyRule MultiplyRule{};
		MatrixTileNumericAddRule AddRule{};
		MatrixTileNumericRoundingMode RoundingMode{};
		MatrixTileNumericSaturationMode SaturationMode{};
		MatrixTileNumericOverflowMode OverflowMode{};
		MatrixTileNumericNaNPolicy NaNPolicy{};
		MatrixTileNumericInfinityPolicy InfinityPolicy{};
		MatrixTileNumericDenormalPolicy DenormalPolicy{};
		MatrixTileNumericReproducibilityMode ReproducibilityMode{};
		MatrixTileNumericExceptionPolicy ExceptionPolicy{};
		unsigned long long Fingerprint = 0;
	};
	struct MatrixTileNumericProfileRow final {
		MatrixTileNumericProfileId ProfileId{};
		DataTypeEnum ElementType{}, AccumulatorType{}, PublishFormat{};
		bool IsExecutionSupported = false;
		std::wstring SupportDecision;
	};
	struct MatrixTileNumericPolicyValidationResult final {
		bool IsValid = false;
		MatrixTileNumericPolicyFaultKind FaultKind = MatrixTileNumericPolicyFaultKind::None;
		std::shared_ptr<MatrixTileNumericProfileRow> Profile;
		bool isRuntimeOwnedNumericPolicyAccepted() const noexcept { return IsValid; }
		static std::shared_ptr<MatrixTileNumericPolicyValidationResult> Valid(std::shared_ptr<MatrixTileNumericProfileRow> profile) { return std::make_shared<MatrixTileNumericPolicyValidationResult>(MatrixTileNumericPolicyValidationResult{true, MatrixTileNumericPolicyFaultKind::None, std::move(profile)}); }
		static std::shared_ptr<MatrixTileNumericPolicyValidationResult> Fault(MatrixTileNumericPolicyFaultKind kind, std::shared_ptr<MatrixTileNumericProfileRow> profile = nullptr) { return std::make_shared<MatrixTileNumericPolicyValidationResult>(MatrixTileNumericPolicyValidationResult{false, kind, std::move(profile)}); }
	};
	class MatrixTileNumericPolicyAbi final
	{
	public:
		static constexpr unsigned short CurrentAbiVersion = 1;
		static inline const std::wstring PolicyDecision = L"ClosedVersionedRuntimeOwnedMatrixTileNumericPolicyAbi";
		static inline const std::wstring IntegerArithmeticDecision = L"ExactIntegerProductAndUnboundedSumFinalEncodingTrap";
		static inline const std::wstring FloatingPolicyDecision = L"Binary32AndBinary64SoftwareIeee754ArithmeticRatified";
		static inline const std::wstring ByteOrderDecision = L"CanonicalLittleEndianPackedElementEncoding";
		static inline const std::wstring MissingPolicyDecision = L"MissingReservedContradictoryOrUnsupportedPolicyFailsClosedBeforeArithmetic";
		static constexpr bool HasImplicitNumericDefaults = false;
		static constexpr bool UsesHostFloatingPointModeAsAuthority = false;
		static constexpr bool UsesCompilerMetadataAsNumericAuthority = false;

	private:
		static const std::vector<std::shared_ptr<MatrixTileNumericProfileRow>> ProfileTable;
/* ORIGINAL ProfileTable = {Supported(MatrixTileNumericProfileId::SignedInt8ToInt32, DataTypeEnum::INT8, DataTypeEnum::INT32), Supported(MatrixTileNumericProfileId::UnsignedInt8ToUInt32, DataTypeEnum::UINT8, DataTypeEnum::UINT32), Supported(MatrixTileNumericProfileId::SignedInt16ToInt32, DataTypeEnum::INT16, DataTypeEnum::INT32), Supported(MatrixTileNumericProfileId::UnsignedInt16ToUInt32, DataTypeEnum::UINT16, DataTypeEnum::UINT32), Supported(MatrixTileNumericProfileId::SignedInt32ToInt64, DataTypeEnum::INT32, DataTypeEnum::INT64), Supported(MatrixTileNumericProfileId::UnsignedInt32ToUInt64, DataTypeEnum::UINT32, DataTypeEnum::UINT64), Supported(MatrixTileNumericProfileId::SignedInt64ToInt64, DataTypeEnum::INT64, DataTypeEnum::INT64), Supported(MatrixTileNumericProfileId::UnsignedInt64ToUInt64, DataTypeEnum::UINT64, DataTypeEnum::UINT64), Supported(MatrixTileNumericProfileId::Binary32ToBinary32, DataTypeEnum::FLOAT32, DataTypeEnum::FLOAT32), Supported(MatrixTileNumericProfileId::Binary64ToBinary64, DataTypeEnum::FLOAT64, DataTypeEnum::FLOAT64),}; */

	public:
		static std::vector<std::shared_ptr<MatrixTileNumericProfileRow>> getSupportedProfiles();

		static std::shared_ptr<MatrixTileNumericPolicy> CreateSupportedPolicy(MatrixTileNumericProfileId profileId);

		static std::shared_ptr<MatrixTileNumericPolicyValidationResult> Validate(std::optional<std::shared_ptr<MatrixTileNumericPolicy>> &policy);

		static bool TryCreateLegacyPhase14Policy(DataTypeEnum elementType, std::shared_ptr<MatrixTileNumericPolicy> &policy);

		static int GetElementSizeBytes(DataTypeEnum dataType);

		static unsigned long long ComputeFingerprint(std::shared_ptr<MatrixTileNumericPolicy> policy);

	private:
		static std::shared_ptr<MatrixTileNumericPolicy> CreateWithoutFingerprint(std::shared_ptr<MatrixTileNumericProfileRow> profile);

		static std::shared_ptr<MatrixTileNumericProfileRow> GetProfile(MatrixTileNumericProfileId profileId);

		static bool TryGetProfile(MatrixTileNumericProfileId profileId, std::shared_ptr<MatrixTileNumericProfileRow> &profile);

		static std::shared_ptr<MatrixTileNumericProfileRow> Supported(MatrixTileNumericProfileId profileId, DataTypeEnum elementType, DataTypeEnum accumulatorType);

		static std::shared_ptr<MatrixTileNumericProfileRow> Unsupported(MatrixTileNumericProfileId profileId, DataTypeEnum elementType, DataTypeEnum accumulatorType, const std::wstring &reason);

		static void Mix(unsigned long long &hash, unsigned short value);

		static void Mix(unsigned long long &hash, unsigned char value);
	};

}
