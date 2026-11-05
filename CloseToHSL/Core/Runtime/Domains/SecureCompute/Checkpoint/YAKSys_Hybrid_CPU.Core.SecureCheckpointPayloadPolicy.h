#pragma once

#include "SecureCheckpointPayloadPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureCheckpointPayloadPolicy final : public std::enable_shared_from_this<SecureCheckpointPayloadPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureCheckpointPayloadPolicy> FailClosed = std::make_shared<SecureCheckpointPayloadPolicy>();
		const bool AllowPrivateSealedPayload;

	public:
		SecureCheckpointPayloadPolicy();

		SecureCheckpointPayloadPolicy(bool allowPrivateSealedPayload);

		static std::shared_ptr<SecureCheckpointPayloadPolicy> getFailClosed();

		bool getAllowPrivateSealedPayload() const;

		SecureCheckpointPayloadDecision Classify(SecureCheckpointPayloadClass payloadClass);
	};
}
