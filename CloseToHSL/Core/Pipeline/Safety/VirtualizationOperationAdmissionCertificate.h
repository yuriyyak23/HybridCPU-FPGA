#pragma once
#include <any>
#include <memory>
#include <string>
namespace YAKSys_Hybrid_CPU::Core {
class VirtualizationE2IssueRequest;
		class VirtualizationOperationAdmissionCertificate final : public std::enable_shared_from_this<VirtualizationOperationAdmissionCertificate>
		{
		private:
			const unsigned long long IssuanceSequence;
			const unsigned long long AttemptId;
			const unsigned long long E1IssuerGeneration;
			const int VirtualThreadId;
			const int OwnerContextId;
			const unsigned long long DomainTag;
			const unsigned long long BundleIdentity;
			const unsigned long long ReplayEpoch;
			const std::wstring DecisionId;
			const std::wstring SpecDigest;
			const std::wstring AcceptanceDigest;
			const unsigned long long OwnerId;
			const unsigned int OwnerPolicyVersion;
			const unsigned int OwnerEpoch;
			const std::wstring OperationNamespace;
			const std::wstring OperationId;
			const unsigned short NumericLeaf;
			const std::wstring OwnerPolicyDigest;
			const std::wstring OperandDigest;
			const unsigned long long CapabilityGrantIdentity;
			const unsigned long long CapabilityGeneration;
			const unsigned long long RootAuthorityEpoch;
			const unsigned long long RestoreGeneration;
			const std::wstring CertificateDigest;

			const std::any _issuerSeal;

			VirtualizationOperationAdmissionCertificate(std::any issuerSeal, unsigned long long issuanceSequence, unsigned long long attemptId, unsigned long long e1IssuerGeneration, int virtualThreadId, int ownerContextId, unsigned long long domainTag, unsigned long long bundleIdentity, unsigned long long replayEpoch, const std::wstring &decisionId, const std::wstring &specDigest, const std::wstring &acceptanceDigest, unsigned long long ownerId, unsigned int ownerPolicyVersion, unsigned int ownerEpoch, const std::wstring &operationNamespace, const std::wstring &operationId, unsigned short numericLeaf, const std::wstring &ownerPolicyDigest, const std::wstring &operandDigest, unsigned long long capabilityGrantIdentity, unsigned long long capabilityGeneration, unsigned long long rootAuthorityEpoch, unsigned long long restoreGeneration, const std::wstring &certificateDigest);

		public:
			unsigned int getSchemaVersion() const;
			unsigned long long getIssuanceSequence() const;
			unsigned long long getAttemptId() const;
			unsigned long long getE1IssuerGeneration() const;
			int getVirtualThreadId() const;
			int getOwnerContextId() const;
			unsigned long long getDomainTag() const;
			unsigned long long getBundleIdentity() const;
			unsigned long long getReplayEpoch() const;
			std::wstring getDecisionId() const;
			std::wstring getSpecDigest() const;
			std::wstring getAcceptanceDigest() const;
			unsigned long long getOwnerId() const;
			unsigned int getOwnerPolicyVersion() const;
			unsigned int getOwnerEpoch() const;
			std::wstring getOperationNamespace() const;
			std::wstring getOperationId() const;
			unsigned short getNumericLeaf() const;
			std::wstring getOwnerPolicyDigest() const;
			std::wstring getOperandDigest() const;
			unsigned long long getCapabilityGrantIdentity() const;
			unsigned long long getCapabilityGeneration() const;
			unsigned long long getRootAuthorityEpoch() const;
			unsigned long long getRestoreGeneration() const;
			std::wstring getCertificateDigest() const;
			bool getHasAddressSpaceIdentity() const;
			bool getHasEvidenceIdentity() const;
			bool getBackendExecutionAuthorized() const;
			bool getCompletionPublicationAuthorized() const;
			bool getRetirePublicationAuthorized() const;

			bool WasIssuedBy(std::any seal);

			static std::shared_ptr<VirtualizationOperationAdmissionCertificate> Create(std::any issuerSeal, unsigned long long issuanceSequence, std::shared_ptr<VirtualizationE2IssueRequest> request);
		};
}
