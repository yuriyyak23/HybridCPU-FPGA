#include "YAKSys_Hybrid_CPU.Core.AddressSpaceIdVocabularyContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	AddressSpaceIdVocabularyViolation AddressSpaceIdVocabularyContract::Evaluate(std::shared_ptr<AddressSpaceIdVocabularyRequest> request)
	{
		if (!request->UsesSecondStageEpochName)
		{
			return AddressSpaceIdVocabularyViolation::MissingSecondStageEpoch;
		}

		return request->UsesAddressSpaceTagEpochName ? AddressSpaceIdVocabularyViolation::None : AddressSpaceIdVocabularyViolation::MissingAddressSpaceTagEpoch;
	}

	bool AddressSpaceIdVocabularyContract::IsSatisfied(std::shared_ptr<AddressSpaceIdVocabularyRequest> request)
	{
		return Evaluate(request) == AddressSpaceIdVocabularyViolation::None;
	}
}
