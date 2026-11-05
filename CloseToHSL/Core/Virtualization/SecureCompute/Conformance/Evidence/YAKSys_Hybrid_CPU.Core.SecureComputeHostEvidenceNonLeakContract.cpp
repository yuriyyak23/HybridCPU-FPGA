#include "YAKSys_Hybrid_CPU.Core.SecureComputeHostEvidenceNonLeakContract.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeEvidenceLeakViolation SecureComputeHostEvidenceNonLeakContract::Validate(bool hostEvidenceGuestVisible, bool schedulerEvidenceSerialized, bool backendBindingEvidenceSerialized, bool nativeTokenEvidenceSerialized, bool debugTraceGuestState)
	{
		if (hostEvidenceGuestVisible)
		{
			return SecureComputeEvidenceLeakViolation::HostEvidenceGuestVisible;
		}

		if (schedulerEvidenceSerialized)
		{
			return SecureComputeEvidenceLeakViolation::SchedulerEvidenceSerialized;
		}

		if (backendBindingEvidenceSerialized)
		{
			return SecureComputeEvidenceLeakViolation::BackendBindingEvidenceSerialized;
		}

		if (nativeTokenEvidenceSerialized)
		{
			return SecureComputeEvidenceLeakViolation::NativeTokenEvidenceSerialized;
		}

		if (debugTraceGuestState)
		{
			return SecureComputeEvidenceLeakViolation::DebugTraceGuestState;
		}

		return SecureComputeEvidenceLeakViolation::None;
	}
}
