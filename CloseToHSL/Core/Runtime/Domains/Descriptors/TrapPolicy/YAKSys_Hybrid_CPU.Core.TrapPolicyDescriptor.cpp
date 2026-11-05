#include "YAKSys_Hybrid_CPU.Core.TrapPolicyDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	TrapPolicyDescriptor::TrapPolicyDescriptor() : TrapPolicyDescriptor(TrapPolicyAuthority::Runtime, TrapPolicyClass::None, true, true)
	{
	}

	TrapPolicyDescriptor::TrapPolicyDescriptor(TrapPolicyAuthority authority, TrapPolicyClass enabledClasses, bool requiresValidatedDomain, bool allowsCompatibilityProjection) : Authority(authority), EnabledClasses(enabledClasses), RequiresValidatedDomain(requiresValidatedDomain), AllowsCompatibilityProjection(allowsCompatibilityProjection) {}

	TrapPolicyAuthority TrapPolicyDescriptor::getAuthority() const
	{
		return Authority;
	}

	TrapPolicyClass TrapPolicyDescriptor::getEnabledClasses() const
	{
		return EnabledClasses;
	}

	bool TrapPolicyDescriptor::getRequiresValidatedDomain() const
	{
		return RequiresValidatedDomain;
	}

	bool TrapPolicyDescriptor::getAllowsCompatibilityProjection() const
	{
		return AllowsCompatibilityProjection;
	}

	bool TrapPolicyDescriptor::isRuntimeAuthoritative() const
	{
		return getAuthority() == TrapPolicyAuthority::Runtime;
	}

	bool TrapPolicyDescriptor::getHasTrapClasses() const
	{
		return getEnabledClasses() != TrapPolicyClass::None;
	}

	bool TrapPolicyDescriptor::getCanProjectToCompatibilityFrontend() const
	{
		return getAllowsCompatibilityProjection() && isRuntimeAuthoritative();
	}

	bool TrapPolicyDescriptor::AllowsClass(TrapPolicyClass policyClass)
	{
		return policyClass != TrapPolicyClass::None && (static_cast<unsigned long long>(getEnabledClasses()) & static_cast<unsigned long long>(policyClass)) == static_cast<unsigned long long>(policyClass);
	}

	std::shared_ptr<TrapPolicyDescriptor> TrapPolicyDescriptor::WithEnabledClasses(TrapPolicyClass enabledClasses)
	{
		return std::make_shared<TrapPolicyDescriptor>(getAuthority(), enabledClasses, getRequiresValidatedDomain(), getAllowsCompatibilityProjection());
	}
}
