#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class AddressSpaceIdVocabularyViolation : unsigned char
	{
		None = 0,
		MissingSecondStageEpoch = 1,
		MissingAddressSpaceTagEpoch = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AddressSpaceIdVocabularyRequest(bool UsesSecondStageEpochName, bool UsesAddressSpaceTagEpochName);


}
