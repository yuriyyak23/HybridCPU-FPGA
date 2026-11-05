#pragma once

#include "../../Descriptors/Memory/YAKSys_Hybrid_CPU.Core.SecureMemoryDomainDescriptor.h"
#include "../../Descriptors/Measurement/YAKSys_Hybrid_CPU.Core.DomainMeasurementDescriptor.h"
#include "../../Descriptors/Domain/YAKSys_Hybrid_CPU.Core.SecureComputeDomainDescriptor.h"
#include "../../Publication/YAKSys_Hybrid_CPU.Core.SecureCompletionPublicationFence.h"
#include "../../../Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "SecureMeasurementAdmissionPolicy.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureMeasurementAdmissionPolicy final : public std::enable_shared_from_this<SecureMeasurementAdmissionPolicy>
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMeasurementAdmissionResult AdmitMeasurement(SecureComputeDomainDescriptor descriptor, DomainMeasurementDescriptor? measurement, System.Nullable<SecureMemoryDomainDescriptor> memory)
		std::shared_ptr<SecureMeasurementAdmissionResult> AdmitMeasurement(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureMeasurementAdmissionResult AdmitAttestationPublication(SecureComputeDomainDescriptor descriptor, DomainMeasurementDescriptor? measurement, System.Nullable<SecureMemoryDomainDescriptor> memory, System.Nullable<EvidencePolicyDescriptor> baseEvidencePolicy, System.Nullable<SecureCompletionPublicationFence> publicationFence)
		std::shared_ptr<SecureMeasurementAdmissionResult> AdmitAttestationPublication(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory, std::shared_ptr<EvidencePolicyDescriptor> baseEvidencePolicy, std::shared_ptr<SecureCompletionPublicationFence> publicationFence);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureMeasurementAdmissionResult AdmitMeasurement(SecureComputeDomainDescriptor descriptor, DomainMeasurementDescriptor? measurement, System.Nullable<SecureMemoryDomainDescriptor> memory, bool requireMeasurement)
		static std::shared_ptr<SecureMeasurementAdmissionResult> AdmitMeasurement(std::shared_ptr<SecureComputeDomainDescriptor> descriptor, std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureMemoryDomainDescriptor> memory, bool requireMeasurement);

		static std::shared_ptr<SecureMeasurementAdmissionResult> Deny(SecureMeasurementAdmissionDecision decision, const std::wstring &reason);
	};
}
