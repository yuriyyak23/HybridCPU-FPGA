#include "YAKSys_Hybrid_CPU.Core.SecureDomainAdmissionService.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureDomainAdmissionService::SecureDomainAdmissionService() : SecureDomainAdmissionService(std::make_shared<SecureDomainAdmissionPolicy>())
	{
	}

	SecureDomainAdmissionService::SecureDomainAdmissionService(std::shared_ptr<SecureDomainAdmissionPolicy> policy) : Policy(policy) {}

	std::shared_ptr<SecureDomainAdmissionPolicy> SecureDomainAdmissionService::getPolicy() const
	{
		return Policy;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureDomainAdmissionResult Admit(SecureComputeDomainDescriptor? descriptor, SecureDomainOperationClass operationClass, System.Nullable<DomainMeasurementDescriptor> measurement, System.Nullable<SecureMemoryDomainDescriptor> memory)
	std::shared_ptr<SecureDomainAdmissionResult> SecureDomainAdmissionService::Admit(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, SecureDomainOperationClass operationClass, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory)
	{
		return getPolicy()->Admit(descriptor, operationClass, measurement, memory);
	}
}
