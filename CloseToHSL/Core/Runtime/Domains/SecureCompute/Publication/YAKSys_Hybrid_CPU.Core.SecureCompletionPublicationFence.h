#pragma once

#include "SecureCompletionPublicationFence.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureCompletionPublicationFence final : public std::enable_shared_from_this<SecureCompletionPublicationFence>
	{
	private:
		static inline const std::shared_ptr<SecureCompletionPublicationFence> Denied = std::make_shared<SecureCompletionPublicationFence>();
		const SecureCompletionFenceState State;
		const SecureRetirePublicationRule RetireRule;

	public:
		SecureCompletionPublicationFence();

		SecureCompletionPublicationFence(SecureCompletionFenceState state, SecureRetirePublicationRule retireRule);

		static std::shared_ptr<SecureCompletionPublicationFence> getDenied();

		SecureCompletionFenceState getState() const;

		SecureRetirePublicationRule getRetireRule() const;

		bool getCanPublishCompletion() const;

		bool getCanPublishRetire() const;
	};
}
