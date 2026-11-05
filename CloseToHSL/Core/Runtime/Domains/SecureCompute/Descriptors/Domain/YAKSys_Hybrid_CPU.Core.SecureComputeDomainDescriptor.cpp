#include "YAKSys_Hybrid_CPU.Core.SecureComputeDomainDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureComputeDomainDescriptor::SecureComputeDomainDescriptor() : SecureComputeDomainDescriptor(0, SecureComputeSecurityLevel::Disabled, false, false, SecureHostInspectionPolicy::getDenyAll(), SecureEvidencePolicy::getFailClosed(), SecureMigrationDescriptor::getDisabled(), SecureIoDomainDescriptor::getDisabled(), SecureHypercallDescriptor::getDisabled(), SecureDebugPolicy::getDenied(), SecureCompatibilityProjectionPolicy::getDenyAll())
	{
	}

	SecureComputeDomainDescriptor::SecureComputeDomainDescriptor(unsigned long long domainTag, SecureComputeSecurityLevel securityLevel, bool measurementRequired, bool privateMemoryRequired, std::shared_ptr<SecureHostInspectionPolicy> hostInspectionPolicy, std::shared_ptr<SecureEvidencePolicy> evidenceVisibilityPolicy, std::shared_ptr<SecureMigrationDescriptor> migrationPolicy, std::shared_ptr<SecureIoDomainDescriptor> ioPolicy, std::shared_ptr<SecureHypercallDescriptor> hypercallPolicy, std::shared_ptr<SecureDebugPolicy> debugPolicy, std::shared_ptr<SecureCompatibilityProjectionPolicy> compatibilityProjectionPolicy)
		: DomainTag(domainTag),
		  SecurityLevel(NormalizeSecurityLevel(securityLevel)),
		  MeasurementRequired(measurementRequired),
		  PrivateMemoryRequired(privateMemoryRequired),
		  HostInspectionPolicy(std::move(hostInspectionPolicy)),
		  EvidenceVisibilityPolicy(std::move(evidenceVisibilityPolicy)),
		  MigrationPolicy(std::move(migrationPolicy)),
		  IoPolicy(std::move(ioPolicy)),
		  HypercallPolicy(std::move(hypercallPolicy)),
		  DebugPolicy(std::move(debugPolicy)),
		  CompatibilityProjectionPolicy(std::move(compatibilityProjectionPolicy))
	{
	}

	std::shared_ptr<SecureComputeDomainDescriptor> SecureComputeDomainDescriptor::getDisabled()
	{
		return Disabled;
	}

	unsigned long long SecureComputeDomainDescriptor::getDomainTag() const
	{
		return DomainTag;
	}

	SecureComputeSecurityLevel SecureComputeDomainDescriptor::getSecurityLevel() const
	{
		return SecurityLevel;
	}

	bool SecureComputeDomainDescriptor::getMeasurementRequired() const
	{
		return MeasurementRequired;
	}

	bool SecureComputeDomainDescriptor::getPrivateMemoryRequired() const
	{
		return PrivateMemoryRequired;
	}

	std::shared_ptr<SecureHostInspectionPolicy> SecureComputeDomainDescriptor::getHostInspectionPolicy() const
	{
		return HostInspectionPolicy;
	}

	std::shared_ptr<SecureEvidencePolicy> SecureComputeDomainDescriptor::getEvidenceVisibilityPolicy() const
	{
		return EvidenceVisibilityPolicy;
	}

	std::shared_ptr<SecureMigrationDescriptor> SecureComputeDomainDescriptor::getMigrationPolicy() const
	{
		return MigrationPolicy;
	}

	std::shared_ptr<SecureIoDomainDescriptor> SecureComputeDomainDescriptor::getIoPolicy() const
	{
		return IoPolicy;
	}

	std::shared_ptr<SecureHypercallDescriptor> SecureComputeDomainDescriptor::getHypercallPolicy() const
	{
		return HypercallPolicy;
	}

	std::shared_ptr<SecureDebugPolicy> SecureComputeDomainDescriptor::getDebugPolicy() const
	{
		return DebugPolicy;
	}

	std::shared_ptr<SecureCompatibilityProjectionPolicy> SecureComputeDomainDescriptor::getCompatibilityProjectionPolicy() const
	{
		return CompatibilityProjectionPolicy;
	}

	bool SecureComputeDomainDescriptor::isEnabled() const
	{
		return getSecurityLevel() != SecureComputeSecurityLevel::Disabled;
	}

	bool SecureComputeDomainDescriptor::isMaterialized() const
	{
		return getDomainTag() != 0;
	}

	bool SecureComputeDomainDescriptor::isActive() const
	{
		return isEnabled() && isMaterialized();
	}

	bool SecureComputeDomainDescriptor::isNoEffect() const
	{
		return !isActive();
	}

	bool SecureComputeDomainDescriptor::getRequiresSecureMemoryPolicy() const
	{
		return isActive() && getPrivateMemoryRequired();
	}

	bool SecureComputeDomainDescriptor::getRequiresMeasurement() const
	{
		return isActive() && getMeasurementRequired();
	}

	SecureComputeSecurityLevel SecureComputeDomainDescriptor::NormalizeSecurityLevel(SecureComputeSecurityLevel securityLevel)
	{
		return securityLevel == SecureComputeSecurityLevel::None ? SecureComputeSecurityLevel::Disabled : securityLevel;
	}
}
