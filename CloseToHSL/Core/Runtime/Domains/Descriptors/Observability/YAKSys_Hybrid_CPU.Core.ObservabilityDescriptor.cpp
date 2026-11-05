#include "YAKSys_Hybrid_CPU.Core.ObservabilityDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	ObservabilityDescriptor::ObservabilityDescriptor() : ObservabilityDescriptor(false, false, false)
	{
	}

	ObservabilityDescriptor::ObservabilityDescriptor(bool allowGuestArchitecturalObservability, bool allowCompatibilityAliasObservability, bool allowHostLocalRuntimeCapture)
		: AllowGuestArchitecturalObservability(allowGuestArchitecturalObservability),
		  AllowCompatibilityAliasObservability(allowCompatibilityAliasObservability),
		  AllowHostLocalRuntimeCapture(allowHostLocalRuntimeCapture)
	{
	}

	std::shared_ptr<ObservabilityDescriptor> ObservabilityDescriptor::getFailClosed()
	{
		return FailClosed;
	}

	bool ObservabilityDescriptor::getAllowGuestArchitecturalObservability() const
	{
		return AllowGuestArchitecturalObservability;
	}

	bool ObservabilityDescriptor::getAllowCompatibilityAliasObservability() const
	{
		return AllowCompatibilityAliasObservability;
	}

	bool ObservabilityDescriptor::getAllowHostLocalRuntimeCapture() const
	{
		return AllowHostLocalRuntimeCapture;
	}

	bool ObservabilityDescriptor::CanPublishToGuest(EvidenceVisibilityClass evidenceClass)
	{
		bool temp_var153;
		switch (evidenceClass)
		{
			case EvidenceVisibilityClass::GuestArchitecturalState:
				temp_var153 = getAllowGuestArchitecturalObservability();
				break;
			case EvidenceVisibilityClass::CompatibilityAlias:
				temp_var153 = getAllowCompatibilityAliasObservability();
				break;
			default:
				temp_var153 = false;
				break;
		}
		return temp_var153;
	}

	bool ObservabilityDescriptor::CanCaptureHostLocal(EvidenceVisibilityClass evidenceClass)
	{
		return getAllowHostLocalRuntimeCapture() && (evidenceClass == EvidenceVisibilityClass::HostOwnedRuntimeEvidence || evidenceClass == EvidenceVisibilityClass::SchedulerEvidence || evidenceClass == EvidenceVisibilityClass::BackendBindingEvidence || evidenceClass == EvidenceVisibilityClass::NativeTokenEvidence);
	}

	bool ObservabilityDescriptor::MustRedactForGuest(EvidenceVisibilityClass evidenceClass)
	{
		return !CanPublishToGuest(evidenceClass);
	}
}

