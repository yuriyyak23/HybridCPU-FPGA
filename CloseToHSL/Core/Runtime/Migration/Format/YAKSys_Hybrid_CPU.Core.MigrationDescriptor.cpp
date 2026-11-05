#include "YAKSys_Hybrid_CPU.Core.MigrationDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	MigrationDescriptor::MigrationDescriptor() : MigrationDescriptor(false, false, false)
	{
	}

	MigrationDescriptor::MigrationDescriptor(bool allowGuestArchitecturalState, bool allowDomainDescriptorState, bool allowCompatibilityProjectionMetadata)
		: AllowGuestArchitecturalState(allowGuestArchitecturalState),
		  AllowDomainDescriptorState(allowDomainDescriptorState),
		  AllowCompatibilityProjectionMetadata(allowCompatibilityProjectionMetadata)
	{
	}

	std::shared_ptr<MigrationDescriptor> MigrationDescriptor::getFailClosed()
	{
		return FailClosed;
	}

	bool MigrationDescriptor::getAllowGuestArchitecturalState() const
	{
		return AllowGuestArchitecturalState;
	}

	bool MigrationDescriptor::getAllowDomainDescriptorState() const
	{
		return AllowDomainDescriptorState;
	}

	bool MigrationDescriptor::getAllowCompatibilityProjectionMetadata() const
	{
		return AllowCompatibilityProjectionMetadata;
	}

	bool MigrationDescriptor::CanSerialize(MigrationPayloadClass payloadClass)
	{
		bool temp_var173;
		switch (payloadClass)
		{
			case MigrationPayloadClass::GuestArchitecturalState:
				temp_var173 = getAllowGuestArchitecturalState();
				break;
			case MigrationPayloadClass::DomainDescriptorState:
				temp_var173 = getAllowDomainDescriptorState();
				break;
			case MigrationPayloadClass::CompatibilityProjectionMetadata:
				temp_var173 = getAllowCompatibilityProjectionMetadata();
				break;
			default:
				temp_var173 = false;
				break;
		}
		return temp_var173;
	}

	bool MigrationDescriptor::CanRestore(MigrationPayloadClass payloadClass)
	{
		return CanSerialize(payloadClass);
	}

	bool MigrationDescriptor::MustRecomputeAfterRestore(MigrationPayloadClass payloadClass)
	{
		return (payloadClass == MigrationPayloadClass::HostOwnedRuntimeEvidence || payloadClass == MigrationPayloadClass::SchedulerEvidence || payloadClass == MigrationPayloadClass::BackendBindingEvidence || payloadClass == MigrationPayloadClass::NativeTokenEvidence);
	}

	bool MigrationDescriptor::MustRejectImportedPayload(MigrationPayloadClass payloadClass)
	{
		return !CanSerialize(payloadClass);
	}
}
