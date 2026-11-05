#include "MatrixTileLayoutPolicyAbi.h"
#include "MatrixTileAccumulatorAndTransposePolicyAbi.h"
#include "MatrixTileArchitecturalTileStateAndDescriptorAbi.h"

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile
{

	std::shared_ptr<MatrixTileLayoutPolicy> MatrixTileLayoutPolicyAbi::CreateMaccPolicy()
	{
		std::shared_ptr<MatrixTileLayoutPolicy> policy = std::make_shared<MatrixTileLayoutPolicy>(CurrentAbiVersion, MatrixTileLayoutProfileId::MaccCanonicalRowMajorAscendingK, MatrixTileProjectedOperationKind::Macc, MatrixTileElementAddressingKind::CanonicalPackedRowMajor, MatrixTileElementAddressingKind::CanonicalPackedRowMajor, MatrixTileElementAddressingKind::CanonicalPackedRowMajor, MatrixTileKIterationOrderKind::AscendingZeroToKMinusOne, MatrixTileTransposePermutationKind::NotApplicable, MatrixTileTransposeAliasPolicyKind::Unspecified, 0);
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		return policy with { Fingerprint = ComputeFingerprint(policy) };
		policy->Fingerprint = ComputeFingerprint(policy);
		return policy;
	}

	std::shared_ptr<MatrixTileLayoutPolicy> MatrixTileLayoutPolicyAbi::CreateTransposePolicy()
	{
		std::shared_ptr<MatrixTileLayoutPolicy> policy = std::make_shared<MatrixTileLayoutPolicy>(CurrentAbiVersion, MatrixTileLayoutProfileId::TransposeCanonicalRowMajor, MatrixTileProjectedOperationKind::Transpose, MatrixTileElementAddressingKind::CanonicalPackedRowMajor, MatrixTileElementAddressingKind::NotApplicable, MatrixTileElementAddressingKind::CanonicalPackedRowMajor, MatrixTileKIterationOrderKind::NotApplicable, MatrixTileTransposePermutationKind::DestinationColumnRowFromSourceRowColumn, MatrixTileTransposeAliasPolicyKind::OutOfPlaceOrSquareInPlaceOnly, 0);
// C# TO C++ CONVERTER TASK: C# 'with expressions' are not converted by C# to C++ Converter:
//		return policy with { Fingerprint = ComputeFingerprint(policy) };
		policy->Fingerprint = ComputeFingerprint(policy);
		return policy;
	}

	std::shared_ptr<MatrixTileLayoutPolicyValidationResult> MatrixTileLayoutPolicyAbi::Validate(std::optional<std::shared_ptr<MatrixTileLayoutPolicy>> &policy, MatrixTileProjectedOperationKind expectedOperationKind)
	{
		if (!policy || !*policy)
		{
			return MatrixTileLayoutPolicyValidationResult::Fault(MatrixTileLayoutPolicyFaultKind::MissingPolicy);
		}

		std::shared_ptr<MatrixTileLayoutPolicy> value = policy.value();
		if (value->AbiVersion != CurrentAbiVersion)
		{
			return MatrixTileLayoutPolicyValidationResult::Fault(MatrixTileLayoutPolicyFaultKind::UnsupportedAbiVersion);
		}

		if (value->ProfileId == MatrixTileLayoutProfileId::Unspecified)
		{
			return MatrixTileLayoutPolicyValidationResult::Fault(MatrixTileLayoutPolicyFaultKind::ReservedProfile);
		}

		if (value->OperationKind != expectedOperationKind)
		{
			return MatrixTileLayoutPolicyValidationResult::Fault(MatrixTileLayoutPolicyFaultKind::UnsupportedOperation);
		}

	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			MatrixTileLayoutPolicy expected = expectedOperationKind switch
	//			{
	//				MatrixTileProjectedOperationKind.Macc => CreateMaccPolicy(),
	//				MatrixTileProjectedOperationKind.Transpose => CreateTransposePolicy(),
	//				_ => default,
	//			};
		std::shared_ptr<MatrixTileLayoutPolicy> expected;
		switch (expectedOperationKind) {
		case MatrixTileProjectedOperationKind::Macc: expected = CreateMaccPolicy(); break;
		case MatrixTileProjectedOperationKind::Transpose: expected = CreateTransposePolicy(); break;
		default: break;
		}
		if (!expected)
		{
			return MatrixTileLayoutPolicyValidationResult::Fault(MatrixTileLayoutPolicyFaultKind::UnsupportedOperation);
		}

		if (value->Fingerprint == 0 || value->Fingerprint != ComputeFingerprint(value))
		{
			return MatrixTileLayoutPolicyValidationResult::Fault(MatrixTileLayoutPolicyFaultKind::FingerprintMismatch);
		}

		return *value == *expected ? MatrixTileLayoutPolicyValidationResult::Valid() : MatrixTileLayoutPolicyValidationResult::Fault(MatrixTileLayoutPolicyFaultKind::ContradictoryRule);
	}

	std::shared_ptr<MatrixTileLayoutPolicyValidationResult> MatrixTileLayoutPolicyAbi::ValidateDescriptors(std::shared_ptr<MatrixTileLayoutPolicy> policy, std::vector<MatrixTileCanonicalDescriptorAbi> &descriptors)
	{
		for (int index = 0; static_cast<size_t>(index) < descriptors.size(); index++)
		{
			if (descriptors[index].Layout != MatrixTileDescriptorLayoutKind::RowMajor)
			{
				return MatrixTileLayoutPolicyValidationResult::Fault(MatrixTileLayoutPolicyFaultKind::DescriptorLayoutMismatch);
			}
		}

		return MatrixTileLayoutPolicyValidationResult::Valid();
	}

	int MatrixTileLayoutPolicyAbi::GetPackedOffset(std::shared_ptr<MatrixTileCanonicalDescriptorAbi> descriptor, unsigned short row, unsigned short column, MatrixTileElementAddressingKind addressing)
	{
		if (addressing != MatrixTileElementAddressingKind::CanonicalPackedRowMajor || descriptor->Layout != MatrixTileDescriptorLayoutKind::RowMajor || row >= descriptor->Rows || column >= descriptor->Columns)
		{
			throw InvalidOperationException(L"MatrixTile element addressing requires an in-range canonical row-major coordinate.");
		}

//		checked
			return ((row * descriptor->Columns) + column) * descriptor->ElementSizeBytes;
// C# TO C++ CONVERTER TASK: End of the original C# 'checked' block.
	}

	unsigned long long MatrixTileLayoutPolicyAbi::ComputeFingerprint(std::shared_ptr<MatrixTileLayoutPolicy> policy)
	{
		unsigned long long hash = 14695981039346656037ULL;
		Mix(hash, policy->AbiVersion);
		Mix(hash, static_cast<unsigned char>(policy->ProfileId));
		Mix(hash, static_cast<unsigned char>(policy->OperationKind));
		Mix(hash, static_cast<unsigned char>(policy->SourceAddressing));
		Mix(hash, static_cast<unsigned char>(policy->SecondaryAddressing));
		Mix(hash, static_cast<unsigned char>(policy->DestinationAddressing));
		Mix(hash, static_cast<unsigned char>(policy->KIterationOrder));
		Mix(hash, static_cast<unsigned char>(policy->TransposePermutation));
		Mix(hash, static_cast<unsigned char>(policy->TransposeAliasPolicy));
		return hash;
	}

	void MatrixTileLayoutPolicyAbi::Mix(unsigned long long &hash, unsigned short value)
	{
		Mix(hash, static_cast<unsigned char>(value));
		Mix(hash, static_cast<unsigned char>(value >> 8));
	}

	void MatrixTileLayoutPolicyAbi::Mix(unsigned long long &hash, unsigned char value)
	{
		hash ^= value;
		hash *= 1099511628211ULL;
	}
}
