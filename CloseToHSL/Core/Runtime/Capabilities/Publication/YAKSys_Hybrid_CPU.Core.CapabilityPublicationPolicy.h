#pragma once

#include "CapabilityPublicationPolicy.h"
#include "../Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class CapabilityPublicationPolicy final : public std::enable_shared_from_this<CapabilityPublicationPolicy>
	{
	private:
		static inline const std::shared_ptr<CapabilityPublicationPolicy> FailClosed = std::make_shared<CapabilityPublicationPolicy>();
		static inline const std::shared_ptr<CapabilityPublicationPolicy> ReadOnlyCompatibilityAlias = std::make_shared<CapabilityPublicationPolicy>(true, CapabilityWriteDisposition::Reject);
		const bool AllowCompatibilityAliasPublication;
		const CapabilityWriteDisposition WriteDisposition;

	public:
		CapabilityPublicationPolicy();

		CapabilityPublicationPolicy(bool allowCompatibilityAliasPublication, CapabilityWriteDisposition writeDisposition);

		static std::shared_ptr<CapabilityPublicationPolicy> getFailClosed();

		static std::shared_ptr<CapabilityPublicationPolicy> getReadOnlyCompatibilityAlias();

		bool getAllowCompatibilityAliasPublication() const;

		CapabilityWriteDisposition getWriteDisposition() const;

		unsigned long long ProjectCompatibilityAlias(std::shared_ptr<CapabilityDescriptorSet> descriptorSet);

		bool CanPublishCapability(std::shared_ptr<CapabilityDescriptorSet> descriptorSet, unsigned long long capabilityMask);

		bool ShouldRejectCompatibilityWrite();

		bool IsCompatibilityWriteArchitecturallyNoEffect();
	};
}
