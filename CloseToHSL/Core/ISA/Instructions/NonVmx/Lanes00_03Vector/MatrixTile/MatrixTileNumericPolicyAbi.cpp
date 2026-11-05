#include "MatrixTileNumericPolicyAbi.h"
#include <stdexcept>
#include "../../../../../../../Arch/DataType.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{
	const std::vector<std::shared_ptr<MatrixTileNumericProfileRow>> MatrixTileNumericPolicyAbi::ProfileTable = {Supported(MatrixTileNumericProfileId::SignedInt8ToInt32, DataTypeEnum::INT8, DataTypeEnum::INT32), Supported(MatrixTileNumericProfileId::UnsignedInt8ToUInt32, DataTypeEnum::UINT8, DataTypeEnum::UINT32), Supported(MatrixTileNumericProfileId::SignedInt16ToInt32, DataTypeEnum::INT16, DataTypeEnum::INT32), Supported(MatrixTileNumericProfileId::UnsignedInt16ToUInt32, DataTypeEnum::UINT16, DataTypeEnum::UINT32), Supported(MatrixTileNumericProfileId::SignedInt32ToInt64, DataTypeEnum::INT32, DataTypeEnum::INT64), Supported(MatrixTileNumericProfileId::UnsignedInt32ToUInt64, DataTypeEnum::UINT32, DataTypeEnum::UINT64), Supported(MatrixTileNumericProfileId::SignedInt64ToInt64, DataTypeEnum::INT64, DataTypeEnum::INT64), Supported(MatrixTileNumericProfileId::UnsignedInt64ToUInt64, DataTypeEnum::UINT64, DataTypeEnum::UINT64), Supported(MatrixTileNumericProfileId::Binary32ToBinary32, DataTypeEnum::FLOAT32, DataTypeEnum::FLOAT32), Supported(MatrixTileNumericProfileId::Binary64ToBinary64, DataTypeEnum::FLOAT64, DataTypeEnum::FLOAT64),};


	std::vector<std::shared_ptr<MatrixTileNumericProfileRow>> MatrixTileNumericPolicyAbi::getSupportedProfiles()
	{
		return ProfileTable;
	}

	std::shared_ptr<MatrixTileNumericPolicy> MatrixTileNumericPolicyAbi::CreateSupportedPolicy(MatrixTileNumericProfileId profileId)
	{
		std::shared_ptr<MatrixTileNumericProfileRow> row = GetProfile(profileId);
		if (!row->IsExecutionSupported)
		{
			throw std::out_of_range("MatrixTile numeric profile is not executable");
		}

		std::shared_ptr<MatrixTileNumericPolicy> policy = CreateWithoutFingerprint(row);
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		return policy with { Fingerprint = ComputeFingerprint(policy) };
		policy->Fingerprint = ComputeFingerprint(policy);
		return policy;
	}

	std::shared_ptr<MatrixTileNumericPolicyValidationResult> MatrixTileNumericPolicyAbi::Validate(std::optional<std::shared_ptr<MatrixTileNumericPolicy>> &policy)
	{
		if (!policy)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::MissingPolicy);
		}

		std::shared_ptr<MatrixTileNumericPolicy> value = policy.value();
		if (value->AbiVersion != CurrentAbiVersion)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::UnsupportedAbiVersion);
		}

		if (value->ProfileId == MatrixTileNumericProfileId::Unspecified)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::ReservedProfile);
		}

		std::shared_ptr<MatrixTileNumericProfileRow> profile;
		if (!TryGetProfile(value->ProfileId, profile))
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::ReservedProfile);
		}

		if (!profile->IsExecutionSupported)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::UnsupportedProfile, profile);
		}

		if (value->Fingerprint == 0 || value->Fingerprint != ComputeFingerprint(value))
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::FingerprintMismatch, profile);
		}

		if (value->ElementType != profile->ElementType)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::ContradictoryElementType, profile);
		}

		if (value->AccumulatorType != profile->AccumulatorType)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::ContradictoryAccumulatorType, profile);
		}

		if (value->PublishFormat != profile->PublishFormat)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::ContradictoryPublishFormat, profile);
		}

		std::shared_ptr<MatrixTileNumericPolicy> expected = CreateWithoutFingerprint(profile);
		if (value->Signedness != expected->Signedness)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::ContradictorySignedness, profile);
		}

		if (value->WideningRule != expected->WideningRule || value->MultiplyRule != expected->MultiplyRule || value->AddRule != expected->AddRule || value->RoundingMode != expected->RoundingMode || value->SaturationMode != expected->SaturationMode || value->OverflowMode != expected->OverflowMode || value->NaNPolicy != expected->NaNPolicy || value->InfinityPolicy != expected->InfinityPolicy || value->DenormalPolicy != expected->DenormalPolicy || value->ReproducibilityMode != expected->ReproducibilityMode || value->ExceptionPolicy != expected->ExceptionPolicy)
		{
			return MatrixTileNumericPolicyValidationResult::Fault(MatrixTileNumericPolicyFaultKind::ContradictoryRule, profile);
		}

		return MatrixTileNumericPolicyValidationResult::Valid(profile);
	}

	bool MatrixTileNumericPolicyAbi::TryCreateLegacyPhase14Policy(DataTypeEnum elementType, std::shared_ptr<MatrixTileNumericPolicy> &policy)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			MatrixTileNumericProfileId profileId = elementType switch
	//			{
	//				DataTypeEnum.INT8 => MatrixTileNumericProfileId.SignedInt8ToInt32,
	//				DataTypeEnum.UINT8 => MatrixTileNumericProfileId.UnsignedInt8ToUInt32,
	//				DataTypeEnum.INT16 => MatrixTileNumericProfileId.SignedInt16ToInt32,
	//				DataTypeEnum.UINT16 => MatrixTileNumericProfileId.UnsignedInt16ToUInt32,
	//				DataTypeEnum.INT32 => MatrixTileNumericProfileId.SignedInt32ToInt64,
	//				DataTypeEnum.UINT32 => MatrixTileNumericProfileId.UnsignedInt32ToUInt64,
	//				DataTypeEnum.INT64 => MatrixTileNumericProfileId.SignedInt64ToInt64,
	//				DataTypeEnum.UINT64 => MatrixTileNumericProfileId.UnsignedInt64ToUInt64,
	//				_ => MatrixTileNumericProfileId.Unspecified,
	//			};

		MatrixTileNumericProfileId profileId = MatrixTileNumericProfileId::Unspecified;
		switch (elementType) {
		case DataTypeEnum::INT8: profileId = MatrixTileNumericProfileId::SignedInt8ToInt32; break;
		case DataTypeEnum::UINT8: profileId = MatrixTileNumericProfileId::UnsignedInt8ToUInt32; break;
		case DataTypeEnum::INT16: profileId = MatrixTileNumericProfileId::SignedInt16ToInt32; break;
		case DataTypeEnum::UINT16: profileId = MatrixTileNumericProfileId::UnsignedInt16ToUInt32; break;
		case DataTypeEnum::INT32: profileId = MatrixTileNumericProfileId::SignedInt32ToInt64; break;
		case DataTypeEnum::UINT32: profileId = MatrixTileNumericProfileId::UnsignedInt32ToUInt64; break;
		case DataTypeEnum::INT64: profileId = MatrixTileNumericProfileId::SignedInt64ToInt64; break;
		case DataTypeEnum::UINT64: profileId = MatrixTileNumericProfileId::UnsignedInt64ToUInt64; break;
		default: break;
		}
		if (profileId == MatrixTileNumericProfileId::Unspecified)
		{
			policy = nullptr;
			return false;
		}

		policy = CreateSupportedPolicy(profileId);
		return true;
	}

	int MatrixTileNumericPolicyAbi::GetElementSizeBytes(DataTypeEnum dataType)
	{
		return DataTypeUtils::SizeOf(dataType);
	}

	unsigned long long MatrixTileNumericPolicyAbi::ComputeFingerprint(std::shared_ptr<MatrixTileNumericPolicy> policy)
	{
		unsigned long long hash = 14695981039346656037ULL;
		Mix(hash, policy->AbiVersion);
		Mix(hash, static_cast<unsigned char>(policy->ProfileId));
		Mix(hash, static_cast<unsigned char>(policy->ElementType));
		Mix(hash, static_cast<unsigned char>(policy->AccumulatorType));
		Mix(hash, static_cast<unsigned char>(policy->PublishFormat));
		Mix(hash, static_cast<unsigned char>(policy->Signedness));
		Mix(hash, static_cast<unsigned char>(policy->WideningRule));
		Mix(hash, static_cast<unsigned char>(policy->MultiplyRule));
		Mix(hash, static_cast<unsigned char>(policy->AddRule));
		Mix(hash, static_cast<unsigned char>(policy->RoundingMode));
		Mix(hash, static_cast<unsigned char>(policy->SaturationMode));
		Mix(hash, static_cast<unsigned char>(policy->OverflowMode));
		Mix(hash, static_cast<unsigned char>(policy->NaNPolicy));
		Mix(hash, static_cast<unsigned char>(policy->InfinityPolicy));
		Mix(hash, static_cast<unsigned char>(policy->DenormalPolicy));
		Mix(hash, static_cast<unsigned char>(policy->ReproducibilityMode));
		Mix(hash, static_cast<unsigned char>(policy->ExceptionPolicy));
		return hash;
	}

	std::shared_ptr<MatrixTileNumericPolicy> MatrixTileNumericPolicyAbi::CreateWithoutFingerprint(std::shared_ptr<MatrixTileNumericProfileRow> profile)
	{
		bool signed_Keyword = DataTypeUtils::IsSignedInteger(profile->ElementType);
		bool unsigned_Keyword = DataTypeUtils::IsUnsignedInteger(profile->ElementType);
		bool floating = DataTypeUtils::IsFloatingPoint(profile->ElementType);

		return std::make_shared<MatrixTileNumericPolicy>(CurrentAbiVersion, profile->ProfileId, profile->ElementType, profile->AccumulatorType, profile->PublishFormat, signed_Keyword ? MatrixTileNumericSignedness::Signed : unsigned_Keyword ? MatrixTileNumericSignedness::Unsigned : MatrixTileNumericSignedness::NotApplicable, signed_Keyword ? MatrixTileNumericWideningRule::SignExtendToAccumulator : unsigned_Keyword ? MatrixTileNumericWideningRule::ZeroExtendToAccumulator : MatrixTileNumericWideningRule::PreserveBinaryFormat, floating ? MatrixTileNumericMultiplyRule::SeparatelyRoundedIeee754Product : MatrixTileNumericMultiplyRule::ExactIntegerProduct, floating ? MatrixTileNumericAddRule::SeparatelyRoundedIeee754Add : MatrixTileNumericAddRule::ExactUnboundedIntermediate, floating ? MatrixTileNumericRoundingMode::ToNearestTiesToEven : MatrixTileNumericRoundingMode::NotApplicableExactInteger, MatrixTileNumericSaturationMode::Disabled, floating ? MatrixTileNumericOverflowMode::Ieee754Infinity : MatrixTileNumericOverflowMode::TrapOnFinalAccumulatorEncoding, floating ? MatrixTileNumericNaNPolicy::CanonicalQuietNaN : MatrixTileNumericNaNPolicy::NotApplicable, floating ? MatrixTileNumericInfinityPolicy::PreserveIeee754Infinity : MatrixTileNumericInfinityPolicy::NotApplicable, floating ? MatrixTileNumericDenormalPolicy::Preserve : MatrixTileNumericDenormalPolicy::NotApplicable, floating ? MatrixTileNumericReproducibilityMode::DeterministicIeee754LittleEndian : MatrixTileNumericReproducibilityMode::ExactIntegerLittleEndian, floating ? MatrixTileNumericExceptionPolicy::PublishCanonicalIeee754Result : MatrixTileNumericExceptionPolicy::CaptureTypedArithmeticFault, 0);
	}

	std::shared_ptr<MatrixTileNumericProfileRow> MatrixTileNumericPolicyAbi::GetProfile(MatrixTileNumericProfileId profileId)
	{
		std::shared_ptr<MatrixTileNumericProfileRow> profile;
		if (TryGetProfile(profileId, profile))
		{
			return profile;
		}

		throw std::out_of_range("Unknown MatrixTile numeric profile");
	}

	bool MatrixTileNumericPolicyAbi::TryGetProfile(MatrixTileNumericProfileId profileId, std::shared_ptr<MatrixTileNumericProfileRow> &profile)
	{
		for (auto row : ProfileTable)
		{
			if (row->ProfileId == profileId)
			{
				profile = row;
				return true;
			}
		}

		profile = nullptr;
		return false;
	}

	std::shared_ptr<MatrixTileNumericProfileRow> MatrixTileNumericPolicyAbi::Supported(MatrixTileNumericProfileId profileId, DataTypeEnum elementType, DataTypeEnum accumulatorType)
	{
		return std::make_shared<MatrixTileNumericProfileRow>(profileId, elementType, accumulatorType, accumulatorType, true, L"Phase15IntegerProfileSupported");
	}

	std::shared_ptr<MatrixTileNumericProfileRow> MatrixTileNumericPolicyAbi::Unsupported(MatrixTileNumericProfileId profileId, DataTypeEnum elementType, DataTypeEnum accumulatorType, const std::wstring &reason)
	{
		return std::make_shared<MatrixTileNumericProfileRow>(profileId, elementType, accumulatorType, accumulatorType, false, reason);
	}

	void MatrixTileNumericPolicyAbi::Mix(unsigned long long &hash, unsigned short value)
	{
		Mix(hash, static_cast<unsigned char>(value));
		Mix(hash, static_cast<unsigned char>(value >> 8));
	}

	void MatrixTileNumericPolicyAbi::Mix(unsigned long long &hash, unsigned char value)
	{
		hash ^= value;
		hash *= 1099511628211ULL;
	}
}
