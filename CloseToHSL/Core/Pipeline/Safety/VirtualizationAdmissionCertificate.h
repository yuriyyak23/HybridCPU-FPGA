#pragma once
#include <any>
#include <memory>
namespace YAKSys_Hybrid_CPU::Core {
enum class VmxOperationKind : unsigned char;
		class VirtualizationAdmissionCertificate final : public std::enable_shared_from_this<VirtualizationAdmissionCertificate>
		{
		private:
			const unsigned long long IssuerGeneration;
			const unsigned long long AttemptId;
			const unsigned short Opcode;
			const VmxOperationKind Operation;
			const int VirtualThreadId;
			const int OwnerContextId;
			const unsigned long long DomainTag;
			const int SourceSlotId;
			const int WorkingSlotId;
			const unsigned long long BundleIdentity;
			const unsigned long long ReplayEpoch;
			const unsigned long long CarrierIdentityDigest;

			const std::any _issuerSeal;

		public:
			VirtualizationAdmissionCertificate(std::any issuerSeal, unsigned long long issuerGeneration, unsigned long long attemptId, unsigned short opcode, VmxOperationKind operation, int virtualThreadId, int ownerContextId, unsigned long long domainTag, int sourceSlotId, int workingSlotId, unsigned long long bundleIdentity, unsigned long long replayEpoch, unsigned long long carrierIdentityDigest);

			unsigned long long getIssuerGeneration() const;
			unsigned int getSchemaVersion() const;
			unsigned long long getAttemptId() const;
			unsigned short getOpcode() const;
			VmxOperationKind getOperation() const;
			int getVirtualThreadId() const;
			int getOwnerContextId() const;
			unsigned long long getDomainTag() const;
			int getSourceSlotId() const;
			int getWorkingSlotId() const;
			unsigned long long getBundleIdentity() const;
			unsigned long long getReplayEpoch() const;
			unsigned long long getAttemptEpoch() const;
			unsigned long long getReplayGeneration() const;
			unsigned long long getCarrierIdentityDigest() const;

			// E1 binds explicit absence for identities that require later neutral owners.
			bool getHasAcceptedNumericLeaf() const;
			bool getHasMaterializedAddressSpaceIdentity() const;
			unsigned long long getAddressSpaceTag() const;
			bool getHasMaterializedDescriptorIdentity() const;
			unsigned long long getDescriptorIdentityDigest() const;
			unsigned long long getDescriptorEpoch() const;
			bool getHasCapabilityGrantIdentity() const;
			unsigned long long getCapabilityGrantIdentity() const;
			unsigned long long getCapabilityRevocationEpoch() const;
			bool getHasEvidencePolicyIdentity() const;
			unsigned long long getEvidencePolicyDigest() const;
			unsigned long long getEvidenceEpoch() const;
			bool getHasRestoreGeneration() const;
			unsigned long long getRestoreGeneration() const;
			bool getBackendExecutionAuthorized() const;
			bool getCompletionPublicationAuthorized() const;
			bool getRetirePublicationAuthorized() const;

			bool WasIssuedBy(std::any issuerSeal);
		};
}
