#pragma once

#include "RootAuthorityDescriptor.h"
#include "../Services/YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class RootAuthorityDescriptor final : public std::enable_shared_from_this<RootAuthorityDescriptor>
	{
	private:
		static inline const std::shared_ptr<RootAuthorityDescriptor> FailClosed = std::make_shared<RootAuthorityDescriptor>();
		const RootAuthorityClass AuthorityClass;
		const unsigned long long AuthorityEpoch;
		const unsigned long long GrantedCapabilityMask;
		const bool AllowCompatibilityFrontendActivation;
		const bool AllowAuthoritativeStateMutation;

	public:
		RootAuthorityDescriptor();

		RootAuthorityDescriptor(RootAuthorityClass authorityClass, unsigned long long authorityEpoch, unsigned long long grantedCapabilityMask, bool allowCompatibilityFrontendActivation, bool allowAuthoritativeStateMutation);

		static std::shared_ptr<RootAuthorityDescriptor> getFailClosed();

		RootAuthorityClass getAuthorityClass() const;

		unsigned long long getAuthorityEpoch() const;

		unsigned long long getGrantedCapabilityMask() const;

		bool getAllowCompatibilityFrontendActivation() const;

		bool getAllowAuthoritativeStateMutation() const;

		bool isRuntimeRoot() const;

		bool isCompatibilityFrontendOnly() const;

		bool HasCapability(unsigned long long capabilityMask);

		bool getCanActivateCompatibilityFrontend() const;

		bool CanMutateAuthoritativeState(std::shared_ptr<DomainRuntimeOperation> operation);

		std::shared_ptr<RootAuthorityDescriptor> WithGrantedCapabilities(unsigned long long capabilityMask);
	};
}
