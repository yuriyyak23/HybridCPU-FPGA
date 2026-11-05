#pragma once

#include "TrapPolicyDescriptor.h"
#include "../../../Events/Traps/TrapPolicyBitmap.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class TrapPolicyDescriptor final : public std::enable_shared_from_this<TrapPolicyDescriptor>
	{
	private:
		const TrapPolicyAuthority Authority;
		const TrapPolicyClass EnabledClasses;
		const bool RequiresValidatedDomain;
		const bool AllowsCompatibilityProjection;

	public:
		TrapPolicyDescriptor();

		TrapPolicyDescriptor(TrapPolicyAuthority authority, TrapPolicyClass enabledClasses, bool requiresValidatedDomain, bool allowsCompatibilityProjection);

		TrapPolicyAuthority getAuthority() const;

		TrapPolicyClass getEnabledClasses() const;

		bool getRequiresValidatedDomain() const;

		bool getAllowsCompatibilityProjection() const;

		bool isRuntimeAuthoritative() const;

		bool getHasTrapClasses() const;

		bool getCanProjectToCompatibilityFrontend() const;

		bool AllowsClass(TrapPolicyClass policyClass);

		std::shared_ptr<TrapPolicyDescriptor> WithEnabledClasses(TrapPolicyClass enabledClasses);
	};
}
