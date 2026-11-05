#pragma once

#include "MigrationDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class MigrationDescriptor final : public std::enable_shared_from_this<MigrationDescriptor>
	{
	private:
		static inline const std::shared_ptr<MigrationDescriptor> FailClosed = std::make_shared<MigrationDescriptor>();
		const bool AllowGuestArchitecturalState;
		const bool AllowDomainDescriptorState;
		const bool AllowCompatibilityProjectionMetadata;

	public:
		MigrationDescriptor();

		MigrationDescriptor(bool allowGuestArchitecturalState, bool allowDomainDescriptorState, bool allowCompatibilityProjectionMetadata);

		static std::shared_ptr<MigrationDescriptor> getFailClosed();

		bool getAllowGuestArchitecturalState() const;

		bool getAllowDomainDescriptorState() const;

		bool getAllowCompatibilityProjectionMetadata() const;

		bool CanSerialize(MigrationPayloadClass payloadClass);

		bool CanRestore(MigrationPayloadClass payloadClass);

		bool MustRecomputeAfterRestore(MigrationPayloadClass payloadClass);

		bool MustRejectImportedPayload(MigrationPayloadClass payloadClass);
	};
}
