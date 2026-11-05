#pragma once

#include "../../Descriptors/Memory/YAKSys_Hybrid_CPU.Core.SecureMemoryDomainDescriptor.h"
#include "../../Descriptors/Measurement/YAKSys_Hybrid_CPU.Core.DomainMeasurementDescriptor.h"
#include "../../Descriptors/Domain/YAKSys_Hybrid_CPU.Core.SecureComputeDomainDescriptor.h"
#include "SecureDomainAdmissionPolicy.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureDomainAdmissionPolicy final : public std::enable_shared_from_this<SecureDomainAdmissionPolicy>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureDomainAdmissionResult Admit(SecureComputeDomainDescriptor? descriptor, SecureDomainOperationClass operationClass, System.Nullable<DomainMeasurementDescriptor> measurement, System.Nullable<SecureMemoryDomainDescriptor> memory)
		std::shared_ptr<SecureDomainAdmissionResult> Admit(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, SecureDomainOperationClass operationClass, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory);
	};
}
