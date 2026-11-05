#pragma once

#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"
#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeContext.h"
#include "YAKSys_Hybrid_CPU.Core.RootAuthorityDescriptor.h"
#include "DomainRuntimeAuthority.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	struct CapabilityBoundaryRequirement;
	struct DomainBoundaryDescriptor;
	class DomainRuntimeAuthority final : public std::enable_shared_from_this<DomainRuntimeAuthority>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeAuthorityResult Validate(RootAuthorityDescriptor? root, DomainRuntimeContext context, DomainRuntimeOperation operation, ulong requiredCapabilityMask = 0)
		std::shared_ptr<DomainRuntimeAuthorityResult> Validate(std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, unsigned long long requiredCapabilityMask = 0);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeAuthorityResult Validate(RootAuthorityDescriptor? root, DomainRuntimeContext context, DomainRuntimeOperation operation, CapabilityBoundaryRequirement capabilityRequirement)
		std::shared_ptr<DomainRuntimeAuthorityResult> Validate(std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public DomainRuntimeAuthorityResult Validate(RootAuthorityDescriptor? root, DomainRuntimeContext context, DomainRuntimeOperation operation, DomainBoundaryDescriptor operationDomainRequirement, CapabilityBoundaryRequirement capabilityRequirement)
		std::shared_ptr<DomainRuntimeAuthorityResult> Validate(std::shared_ptr<RootAuthorityDescriptor> root, std::shared_ptr<DomainRuntimeContext> context, std::shared_ptr<DomainRuntimeOperation> operation, std::shared_ptr<DomainBoundaryDescriptor> operationDomainRequirement, std::shared_ptr<CapabilityBoundaryRequirement> capabilityRequirement);

	private:
		static std::shared_ptr<DomainRuntimeAuthorityResult> Deny(DomainRuntimeAuthorityDecision decision, const std::wstring &message);
	};
}
