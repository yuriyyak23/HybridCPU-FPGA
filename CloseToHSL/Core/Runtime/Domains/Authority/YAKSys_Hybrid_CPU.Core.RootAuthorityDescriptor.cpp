#include "YAKSys_Hybrid_CPU.Core.RootAuthorityDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	RootAuthorityDescriptor::RootAuthorityDescriptor() : RootAuthorityDescriptor(RootAuthorityClass::RuntimeRoot, 0, 0, false, false)
	{
	}

	RootAuthorityDescriptor::RootAuthorityDescriptor(RootAuthorityClass authorityClass, unsigned long long authorityEpoch, unsigned long long grantedCapabilityMask, bool allowCompatibilityFrontendActivation, bool allowAuthoritativeStateMutation) : AuthorityClass(authorityClass), AuthorityEpoch(authorityEpoch), GrantedCapabilityMask(grantedCapabilityMask), AllowCompatibilityFrontendActivation(allowCompatibilityFrontendActivation), AllowAuthoritativeStateMutation(allowAuthoritativeStateMutation) {}

	std::shared_ptr<RootAuthorityDescriptor> RootAuthorityDescriptor::getFailClosed()
	{
		return FailClosed;
	}

	RootAuthorityClass RootAuthorityDescriptor::getAuthorityClass() const
	{
		return AuthorityClass;
	}

	unsigned long long RootAuthorityDescriptor::getAuthorityEpoch() const
	{
		return AuthorityEpoch;
	}

	unsigned long long RootAuthorityDescriptor::getGrantedCapabilityMask() const
	{
		return GrantedCapabilityMask;
	}

	bool RootAuthorityDescriptor::getAllowCompatibilityFrontendActivation() const
	{
		return AllowCompatibilityFrontendActivation;
	}

	bool RootAuthorityDescriptor::getAllowAuthoritativeStateMutation() const
	{
		return AllowAuthoritativeStateMutation;
	}

	bool RootAuthorityDescriptor::isRuntimeRoot() const
	{
		return getAuthorityClass() == RootAuthorityClass::RuntimeRoot;
	}

	bool RootAuthorityDescriptor::isCompatibilityFrontendOnly() const
	{
		return getAuthorityClass() == RootAuthorityClass::CompatibilityFrontend;
	}

	bool RootAuthorityDescriptor::HasCapability(unsigned long long capabilityMask)
	{
		return capabilityMask != 0 && (getGrantedCapabilityMask() & capabilityMask) == capabilityMask;
	}

	bool RootAuthorityDescriptor::getCanActivateCompatibilityFrontend() const
	{
		return isRuntimeRoot() && getAllowCompatibilityFrontendActivation();
	}

	bool RootAuthorityDescriptor::CanMutateAuthoritativeState(std::shared_ptr<DomainRuntimeOperation> operation)
	{
		return isRuntimeRoot() && getAllowAuthoritativeStateMutation() && operation->getCanMutateAuthoritativeState();
	}

	std::shared_ptr<RootAuthorityDescriptor> RootAuthorityDescriptor::WithGrantedCapabilities(unsigned long long capabilityMask)
	{
		return std::make_shared<RootAuthorityDescriptor>(getAuthorityClass(), getAuthorityEpoch(), capabilityMask, getAllowCompatibilityFrontendActivation(), getAllowAuthoritativeStateMutation());
	}
}
