#pragma once

#include "AddressSpaceIdVocabularyContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct AddressSpaceIdVocabularyRequest final
	{
		bool UsesSecondStageEpochName = false;
		bool UsesAddressSpaceTagEpochName = false;
	};

	class AddressSpaceIdVocabularyContract final : public std::enable_shared_from_this<AddressSpaceIdVocabularyContract>
	{
	public:
		AddressSpaceIdVocabularyViolation Evaluate(std::shared_ptr<AddressSpaceIdVocabularyRequest> request);

		bool IsSatisfied(std::shared_ptr<AddressSpaceIdVocabularyRequest> request);
	};
}
