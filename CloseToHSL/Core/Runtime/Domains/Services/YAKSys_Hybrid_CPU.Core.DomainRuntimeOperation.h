#pragma once

#include "DomainRuntimeOperation.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainRuntimeOperation final : public std::enable_shared_from_this<DomainRuntimeOperation>
	{
	private:
		const DomainRuntimeOperationKind Kind;
		const DomainRuntimeOperationSource Source;
		const bool RequiresCapabilityGrant;
		const DomainRuntimeOperationAuthorityClass AuthorityClass;

	public:
		DomainRuntimeOperation();

		DomainRuntimeOperation(DomainRuntimeOperationKind kind, DomainRuntimeOperationSource source, bool requiresCapabilityGrant, bool isProjectionOnly);

		DomainRuntimeOperation(DomainRuntimeOperationKind kind, DomainRuntimeOperationSource source, bool requiresCapabilityGrant, DomainRuntimeOperationAuthorityClass authorityClass);

		DomainRuntimeOperationKind getKind() const;

		DomainRuntimeOperationSource getSource() const;

		bool getRequiresCapabilityGrant() const;

		DomainRuntimeOperationAuthorityClass getAuthorityClass() const;

		bool isProjectionOnly() const;

		bool isNoStateExecution() const;

		static std::shared_ptr<DomainRuntimeOperation> FromCompatibilityFrontend(DomainRuntimeOperationKind kind, bool requiresCapabilityGrant = false, bool isProjectionOnly = false);

		bool getCanMutateAuthoritativeState() const;
	};
}
