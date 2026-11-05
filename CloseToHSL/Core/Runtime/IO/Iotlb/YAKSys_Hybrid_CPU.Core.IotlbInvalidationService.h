#pragma once

#include "../../Domains/Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "IotlbInvalidationService.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class IotlbInvalidationService final : public std::enable_shared_from_this<IotlbInvalidationService>
	{
	public:
		std::shared_ptr<IotlbInvalidationResult> Invalidate(std::shared_ptr<DomainRuntimeContext> context, IotlbInvalidationScope scope, unsigned short ioDomainTag = 0, unsigned int domainId = 0, unsigned int deviceId = 0);

	private:
		static std::shared_ptr<IotlbInvalidationResult> Deny(IotlbInvalidationDecision decision, const std::wstring &message);
	};
}
