#pragma once

#include "SecureComputeHostEvidenceNonLeakContract.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeHostEvidenceNonLeakContract final : public std::enable_shared_from_this<SecureComputeHostEvidenceNonLeakContract>
	{
	public:
		SecureComputeEvidenceLeakViolation Validate(bool hostEvidenceGuestVisible, bool schedulerEvidenceSerialized, bool backendBindingEvidenceSerialized, bool nativeTokenEvidenceSerialized, bool debugTraceGuestState);
	};
}
