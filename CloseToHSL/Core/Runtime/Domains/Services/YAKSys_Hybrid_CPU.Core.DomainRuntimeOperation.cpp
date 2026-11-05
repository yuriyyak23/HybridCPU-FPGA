#include "YAKSys_Hybrid_CPU.Core.DomainRuntimeOperation.h"

namespace YAKSys_Hybrid_CPU::Core
{

	DomainRuntimeOperation::DomainRuntimeOperation() : DomainRuntimeOperation(DomainRuntimeOperationKind::ActivateCompatibilityFrontend, DomainRuntimeOperationSource::CompatibilityFrontend, false, false)
	{
	}

	DomainRuntimeOperation::DomainRuntimeOperation(DomainRuntimeOperationKind kind, DomainRuntimeOperationSource source, bool requiresCapabilityGrant, bool isProjectionOnly) : DomainRuntimeOperation(kind, source, requiresCapabilityGrant, isProjectionOnly ? DomainRuntimeOperationAuthorityClass::ProjectionOnly : DomainRuntimeOperationAuthorityClass::AuthoritativeMutation)
	{
	}

	DomainRuntimeOperation::DomainRuntimeOperation(DomainRuntimeOperationKind kind, DomainRuntimeOperationSource source, bool requiresCapabilityGrant, DomainRuntimeOperationAuthorityClass authorityClass) : Kind(kind), Source(source), RequiresCapabilityGrant(requiresCapabilityGrant), AuthorityClass(authorityClass) {}

	DomainRuntimeOperationKind DomainRuntimeOperation::getKind() const
	{
		return Kind;
	}

	DomainRuntimeOperationSource DomainRuntimeOperation::getSource() const
	{
		return Source;
	}

	bool DomainRuntimeOperation::getRequiresCapabilityGrant() const
	{
		return RequiresCapabilityGrant;
	}

	DomainRuntimeOperationAuthorityClass DomainRuntimeOperation::getAuthorityClass() const
	{
		return AuthorityClass;
	}

	bool DomainRuntimeOperation::isProjectionOnly() const
	{
		return getAuthorityClass() == DomainRuntimeOperationAuthorityClass::ProjectionOnly;
	}

	bool DomainRuntimeOperation::isNoStateExecution() const
	{
		return getAuthorityClass() == DomainRuntimeOperationAuthorityClass::NoStateExecution;
	}

	std::shared_ptr<DomainRuntimeOperation> DomainRuntimeOperation::FromCompatibilityFrontend(DomainRuntimeOperationKind kind, bool requiresCapabilityGrant, bool isProjectionOnly)
	{
		return std::make_shared<DomainRuntimeOperation>(kind, DomainRuntimeOperationSource::CompatibilityFrontend, requiresCapabilityGrant, isProjectionOnly);
	}

	bool DomainRuntimeOperation::getCanMutateAuthoritativeState() const
	{
		return getAuthorityClass() == DomainRuntimeOperationAuthorityClass::AuthoritativeMutation && getSource() != DomainRuntimeOperationSource::MigrationReplay;
	}
}
