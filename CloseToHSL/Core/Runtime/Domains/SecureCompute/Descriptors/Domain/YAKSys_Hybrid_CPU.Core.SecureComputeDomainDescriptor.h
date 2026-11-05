#pragma once

#include "SecureComputeDomainDescriptor.h"
#include "../../Policies/HostInspection/YAKSys_Hybrid_CPU.Core.SecureHostInspectionPolicy.h"
#include "../../Policies/Evidence/YAKSys_Hybrid_CPU.Core.SecureEvidencePolicy.h"
#include "../Migration/YAKSys_Hybrid_CPU.Core.SecureMigrationDescriptor.h"
#include "../Io/YAKSys_Hybrid_CPU.Core.SecureIoDomainDescriptor.h"
#include "../Hypercalls/YAKSys_Hybrid_CPU.Core.SecureHypercallDescriptor.h"
#include "../../Policies/Debug/YAKSys_Hybrid_CPU.Core.SecureDebugPolicy.h"
#include "../../Policies/Compatibility/YAKSys_Hybrid_CPU.Core.SecureCompatibilityProjectionPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureComputeDomainDescriptor final : public std::enable_shared_from_this<SecureComputeDomainDescriptor>
	{
	private:
		static inline const std::shared_ptr<SecureComputeDomainDescriptor> Disabled = std::make_shared<SecureComputeDomainDescriptor>();
		const unsigned long long DomainTag;
		const SecureComputeSecurityLevel SecurityLevel;
		const bool MeasurementRequired;
		const bool PrivateMemoryRequired;
		const std::shared_ptr<SecureHostInspectionPolicy> HostInspectionPolicy;
		const std::shared_ptr<SecureEvidencePolicy> EvidenceVisibilityPolicy;
		const std::shared_ptr<SecureMigrationDescriptor> MigrationPolicy;
		const std::shared_ptr<SecureIoDomainDescriptor> IoPolicy;
		const std::shared_ptr<SecureHypercallDescriptor> HypercallPolicy;
		const std::shared_ptr<SecureDebugPolicy> DebugPolicy;
		const std::shared_ptr<SecureCompatibilityProjectionPolicy> CompatibilityProjectionPolicy;

	public:
		SecureComputeDomainDescriptor();

		SecureComputeDomainDescriptor(unsigned long long domainTag, SecureComputeSecurityLevel securityLevel, bool measurementRequired, bool privateMemoryRequired, std::shared_ptr<SecureHostInspectionPolicy> hostInspectionPolicy, std::shared_ptr<SecureEvidencePolicy> evidenceVisibilityPolicy, std::shared_ptr<SecureMigrationDescriptor> migrationPolicy, std::shared_ptr<SecureIoDomainDescriptor> ioPolicy, std::shared_ptr<SecureHypercallDescriptor> hypercallPolicy, std::shared_ptr<SecureDebugPolicy> debugPolicy, std::shared_ptr<SecureCompatibilityProjectionPolicy> compatibilityProjectionPolicy);

		static std::shared_ptr<SecureComputeDomainDescriptor> getDisabled();

		unsigned long long getDomainTag() const;

		SecureComputeSecurityLevel getSecurityLevel() const;

		bool getMeasurementRequired() const;

		bool getPrivateMemoryRequired() const;

		std::shared_ptr<SecureHostInspectionPolicy> getHostInspectionPolicy() const;

		std::shared_ptr<SecureEvidencePolicy> getEvidenceVisibilityPolicy() const;

		std::shared_ptr<SecureMigrationDescriptor> getMigrationPolicy() const;

		std::shared_ptr<SecureIoDomainDescriptor> getIoPolicy() const;

		std::shared_ptr<SecureHypercallDescriptor> getHypercallPolicy() const;

		std::shared_ptr<SecureDebugPolicy> getDebugPolicy() const;

		std::shared_ptr<SecureCompatibilityProjectionPolicy> getCompatibilityProjectionPolicy() const;

		bool isEnabled() const;

		bool isMaterialized() const;

		bool isActive() const;

		bool isNoEffect() const;

		bool getRequiresSecureMemoryPolicy() const;

		bool getRequiresMeasurement() const;

		static SecureComputeSecurityLevel NormalizeSecurityLevel(SecureComputeSecurityLevel securityLevel);
	};
}
