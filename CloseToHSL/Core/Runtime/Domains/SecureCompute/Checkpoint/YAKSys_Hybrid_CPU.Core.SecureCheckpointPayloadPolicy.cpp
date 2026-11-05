#include "YAKSys_Hybrid_CPU.Core.SecureCheckpointPayloadPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureCheckpointPayloadPolicy::SecureCheckpointPayloadPolicy() : SecureCheckpointPayloadPolicy(false)
	{
	}

	SecureCheckpointPayloadPolicy::SecureCheckpointPayloadPolicy(bool allowPrivateSealedPayload)
		: AllowPrivateSealedPayload(allowPrivateSealedPayload)
	{
	}

	std::shared_ptr<SecureCheckpointPayloadPolicy> SecureCheckpointPayloadPolicy::getFailClosed()
	{
		return FailClosed;
	}

	bool SecureCheckpointPayloadPolicy::getAllowPrivateSealedPayload() const
	{
		return AllowPrivateSealedPayload;
	}

	SecureCheckpointPayloadDecision SecureCheckpointPayloadPolicy::Classify(SecureCheckpointPayloadClass payloadClass)
	{
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			return payloadClass switch
	//			{
	//				SecureCheckpointPayloadClass.HostOwnedEvidence or SecureCheckpointPayloadClass.SchedulerEvidence or SecureCheckpointPayloadClass.BackendBindingEvidence or SecureCheckpointPayloadClass.NativeTokenEvidence => SecureCheckpointPayloadDecision.DeniedHostOwnedEvidence,
	//ignore
	//ignore
	//ignore
	//
	//				SecureCheckpointPayloadClass.DebugTrace => SecureCheckpointPayloadDecision.DeniedDebugTraceAsGuestState,
	//
	//				SecureCheckpointPayloadClass.RawMeasurementSecret => SecureCheckpointPayloadDecision.DeniedRawMeasurementSecret,
	//
	//				SecureCheckpointPayloadClass.RawSealingKey => SecureCheckpointPayloadDecision.DeniedRawSealingKey,
	//
	//				SecureCheckpointPayloadClass.ActiveHostPointer => SecureCheckpointPayloadDecision.DeniedActiveHostPointer,
	//
	//				SecureCheckpointPayloadClass.VmcsProjectionMetadata or SecureCheckpointPayloadClass.CompatibilityProjectionMetadata => SecureCheckpointPayloadDecision.DeniedCompatibilityProjectionAuthority,
	//ignore
	//
	//				SecureCheckpointPayloadClass.SecurePrivateMemory when !AllowPrivateSealedPayload => SecureCheckpointPayloadDecision.DeniedPrivateMemoryWithoutSealedPayload,
	//
	//				_ => SecureCheckpointPayloadDecision.Allowed,
	//			};
	}
}
