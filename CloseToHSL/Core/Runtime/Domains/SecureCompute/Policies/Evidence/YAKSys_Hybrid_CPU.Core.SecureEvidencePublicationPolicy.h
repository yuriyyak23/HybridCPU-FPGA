#pragma once

#include "YAKSys_Hybrid_CPU.Core.SecureEvidencePolicy.h"
#include "../../../Descriptors/EvidencePolicy/YAKSys_Hybrid_CPU.Core.EvidencePolicyDescriptor.h"
#include "SecureEvidencePolicy.h"
#include "../Compatibility/YAKSys_Hybrid_CPU.Core.SecureCompatibilityProjectionPolicy.h"
#include "../../Publication/YAKSys_Hybrid_CPU.Core.SecureCompletionPublicationFence.h"
#include "../../../../../Virtualization/SecureCompute/Sideband/EvidenceTransport/YAKSys_Hybrid_CPU.Core.SecureComputeEvidenceSidebandEnvelope.h"
#include "../../Descriptors/Measurement/YAKSys_Hybrid_CPU.Core.DomainMeasurementDescriptor.h"
#include "SecureEvidencePublicationPolicy.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureEvidencePublicationPolicy final : public std::enable_shared_from_this<SecureEvidencePublicationPolicy>
	{
	private:
		static inline const std::shared_ptr<SecureEvidencePublicationPolicy> Default = std::make_shared<SecureEvidencePublicationPolicy>();

	public:
		static std::shared_ptr<SecureEvidencePublicationPolicy> getDefault();

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitGuestVisibleEvidence(SecureEvidencePolicy? secureEvidencePolicy, System.Nullable<EvidencePolicyDescriptor> neutralEvidencePolicy, SecureEvidenceVisibilityClass evidenceClass)
		std::shared_ptr<SecureEvidencePublicationResult> AdmitGuestVisibleEvidence(std::shared_ptr<SecureEvidencePolicy> secureEvidencePolicy, std::shared_ptr<EvidencePolicyDescriptor> neutralEvidencePolicy, SecureEvidenceVisibilityClass evidenceClass);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitCompatibilityAliasEvidence(SecureEvidencePolicy? secureEvidencePolicy, System.Nullable<EvidencePolicyDescriptor> neutralEvidencePolicy, System.Nullable<SecureCompatibilityProjectionPolicy> compatibilityPolicy, ulong aliasBit)
		std::shared_ptr<SecureEvidencePublicationResult> AdmitCompatibilityAliasEvidence(std::shared_ptr<SecureEvidencePolicy> secureEvidencePolicy, std::shared_ptr<EvidencePolicyDescriptor> neutralEvidencePolicy, std::shared_ptr<SecureCompatibilityProjectionPolicy> compatibilityPolicy, unsigned long long aliasBit);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitCompletionPublication(SecureCompletionPublicationFence? publicationFence)
		std::shared_ptr<SecureEvidencePublicationResult> AdmitCompletionPublication(std::shared_ptr<SecureCompletionPublicationFence> publicationFence);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitRetirePublication(SecureCompletionPublicationFence? publicationFence)
		std::shared_ptr<SecureEvidencePublicationResult> AdmitRetirePublication(std::shared_ptr<SecureCompletionPublicationFence> publicationFence);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitSidebandEnvelope(SecureComputeEvidenceSidebandEnvelope envelope, SecureEvidencePolicy? secureEvidencePolicy, System.Nullable<EvidencePolicyDescriptor> neutralEvidencePolicy, System.Nullable<SecureCompatibilityProjectionPolicy> compatibilityPolicy = null, ulong aliasBit = 0)
		std::shared_ptr<SecureEvidencePublicationResult> AdmitSidebandEnvelope(std::shared_ptr<SecureComputeEvidenceSidebandEnvelope> envelope, std::shared_ptr<SecureEvidencePolicy> secureEvidencePolicy, std::shared_ptr<EvidencePolicyDescriptor> neutralEvidencePolicy, std::shared_ptr<SecureCompatibilityProjectionPolicy> compatibilityPolicy = nullptr, unsigned long long aliasBit = 0);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public SecureEvidencePublicationResult AdmitReplayOrRestoreEvidence(DomainMeasurementDescriptor? measurement, SecureRevocationEpoch expectedEpoch)
		std::shared_ptr<SecureEvidencePublicationResult> AdmitReplayOrRestoreEvidence(std::shared_ptr<DomainMeasurementDescriptor> measurement, std::shared_ptr<SecureRevocationEpoch> expectedEpoch);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static SecureEvidencePublicationResult ValidateEvidencePolicyPrerequisites(SecureEvidencePolicy? secureEvidencePolicy, System.Nullable<EvidencePolicyDescriptor> neutralEvidencePolicy, SecureEvidenceVisibilityClass evidenceClass)
		static std::shared_ptr<SecureEvidencePublicationResult> ValidateEvidencePolicyPrerequisites(std::shared_ptr<SecureEvidencePolicy> secureEvidencePolicy, std::shared_ptr<EvidencePolicyDescriptor> neutralEvidencePolicy, SecureEvidenceVisibilityClass evidenceClass);

		static std::shared_ptr<SecureEvidencePublicationResult> Deny(SecureEvidencePublicationDecision decision, const std::wstring &reason);
	};
}
