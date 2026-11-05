#pragma once

#include "../Memory/YAKSys_Hybrid_CPU.Core.SecureMemoryAdmissionPolicy.h"
#include "../../Authority/YAKSys_Hybrid_CPU.Core.SecureGrantAuthorityPolicy.h"
#include "../../Publication/YAKSys_Hybrid_CPU.Core.SecureCompletionPublicationFence.h"
#include "../../Descriptors/Memory/YAKSys_Hybrid_CPU.Core.SecureMemoryDomainDescriptor.h"
#include "../../Descriptors/Io/YAKSys_Hybrid_CPU.Core.SecureIoDomainDescriptor.h"
#include "../../Descriptors/Hypercalls/YAKSys_Hybrid_CPU.Core.SecureHypercallDescriptor.h"
#include "../../../Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "../../../../Capabilities/Descriptors/YAKSys_Hybrid_CPU.Core.CapabilityDescriptorSet.h"
#include "SecureIoHypercallAdmissionPolicy.h"
#include "../Memory/SecureMemoryAdmissionPolicy.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureIoHypercallAdmissionPolicy final : public std::enable_shared_from_this<SecureIoHypercallAdmissionPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureIoHypercallAdmissionPolicy> Default = std::make_shared<SecureIoHypercallAdmissionPolicy>();

	public:
		static std::shared_ptr<SecureIoHypercallAdmissionPolicy> getDefault();

	private:
		const std::shared_ptr<SecureMemoryAdmissionPolicy> _memoryAdmission = std::make_shared<SecureMemoryAdmissionPolicy>();
		const std::shared_ptr<SecureGrantAuthorityPolicy> _grantAuthority = std::make_shared<SecureGrantAuthorityPolicy>();

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureIoHypercallAdmissionResult AdmitIoDma(SecureIoDomainDescriptor? ioPolicy, System.Nullable<SecureMemoryDomainDescriptor> memory, SecureMemoryAccessRequest access, System.Nullable<SecureCompletionPublicationFence> publicationFence, bool requireRetirePublication)
		std::shared_ptr<SecureIoHypercallAdmissionResult> AdmitIoDma(std::shared_ptr<SecureIoDomainDescriptor> ioPolicy, std::shared_ptr<SecureMemoryDomainDescriptor> memory, std::shared_ptr<SecureMemoryAccessRequest> access, std::shared_ptr<SecureCompletionPublicationFence> publicationFence, bool requireRetirePublication);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureIoHypercallAdmissionResult AdmitHypercall(SecureHypercallDescriptor? hypercallPolicy, ulong hypercallId, System.Nullable<CapabilityDescriptorSet> capabilities, System.Nullable<EvidencePolicyDescriptor> evidencePolicy, System.Nullable<SecureIoDomainDescriptor> ioPolicy, SecureRevocationEpoch policyEpoch, System.Nullable<SecureCompletionPublicationFence> publicationFence, bool neutralBackendOwnerMaterialized, bool domainValidated, ulong validatedDomainTag = 0)
		std::shared_ptr<SecureIoHypercallAdmissionResult> AdmitHypercall(std::shared_ptr<SecureHypercallDescriptor> hypercallPolicy, unsigned long long hypercallId, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<EvidencePolicyDescriptor> evidencePolicy, std::shared_ptr<SecureIoDomainDescriptor> ioPolicy, std::shared_ptr<SecureRevocationEpoch> policyEpoch, std::shared_ptr<SecureCompletionPublicationFence> publicationFence, bool neutralBackendOwnerMaterialized, bool domainValidated, unsigned long long validatedDomainTag = 0);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureIoHypercallAdmissionResult AdmitArgument(SecureHypercallArgumentDescriptor argument, SecureIoDomainDescriptor? ioPolicy, SecureRevocationEpoch policyEpoch, ulong validatedDomainTag)
		std::shared_ptr<SecureIoHypercallAdmissionResult> AdmitArgument(std::shared_ptr<SecureHypercallArgumentDescriptor> argument, std::shared_ptr<SecureIoDomainDescriptor> ioPolicy, std::shared_ptr<SecureRevocationEpoch> policyEpoch, unsigned long long validatedDomainTag);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private SecureIoHypercallAdmissionResult AdmitTypedGrant(SecureGrantHandle requiredGrant, CapabilityDescriptorSet? capabilities, SecureRevocationEpoch policyEpoch)
		std::shared_ptr<SecureIoHypercallAdmissionResult> AdmitTypedGrant(std::shared_ptr<SecureGrantHandle> requiredGrant, std::shared_ptr<CapabilityDescriptorSet> capabilities, std::shared_ptr<SecureRevocationEpoch> policyEpoch);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureIoHypercallAdmissionResult AdmitCompletionAndRetire(SecureCompletionPublicationFence? publicationFence, bool requireCompletionPublication, bool requireRetirePublication)
		static std::shared_ptr<SecureIoHypercallAdmissionResult> AdmitCompletionAndRetire(std::shared_ptr<SecureCompletionPublicationFence> publicationFence, bool requireCompletionPublication, bool requireRetirePublication);

		static SecureIoHypercallAdmissionDecision MapMemoryDecision(SecureMemoryAdmissionDecision decision);

		static std::shared_ptr<SecureIoHypercallAdmissionResult> Deny(SecureIoHypercallAdmissionDecision decision, const std::wstring &reason);
	};
}
