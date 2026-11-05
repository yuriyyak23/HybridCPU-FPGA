#pragma once

#include "DomainMeasurementDescriptor.h"
#include "../../Authority/YAKSys_Hybrid_CPU.Core.SecureMeasurementHandle.h"
#include "../../Authority/YAKSys_Hybrid_CPU.Core.SecureGrantHandle.h"
#include "../../Policies/Evidence/SecureEvidencePolicy.h"
#include "../Migration/YAKSys_Hybrid_CPU.Core.SecureMigrationDescriptor.h"
#include "../Domain/YAKSys_Hybrid_CPU.Core.SecureComputeDomainDescriptor.h"
#include "../Memory/YAKSys_Hybrid_CPU.Core.SecureMemoryDomainDescriptor.h"
#include "../Io/YAKSys_Hybrid_CPU.Core.SecureIoDomainDescriptor.h"
#include "../Hypercalls/YAKSys_Hybrid_CPU.Core.SecureHypercallDescriptor.h"
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class DomainMeasurementDescriptor final : public std::enable_shared_from_this<DomainMeasurementDescriptor>
	{
	private:
		static inline const std::shared_ptr<DomainMeasurementDescriptor> Missing = std::make_shared<DomainMeasurementDescriptor>();
		const std::shared_ptr<SecureMeasurementHandle> Handle;
		const SecureMeasurementState State;
		const SecureMeasurementDebugClass DebugClass;
		const unsigned long long PolicyDigest;
		const unsigned long long MemoryDigest;
		const unsigned long long RuntimeDigest;
		const SecureEvidenceVisibilityClass AttestationEvidenceClass;
		const unsigned long long CreatorDomainTag;
		const unsigned long long ParentMeasurementId;
		const unsigned long long PolicySourceHash;

	public:
		DomainMeasurementDescriptor();

		DomainMeasurementDescriptor(std::shared_ptr<SecureMeasurementHandle> handle, SecureMeasurementState state, SecureMeasurementDebugClass debugClass, unsigned long long policyDigest, unsigned long long memoryDigest, unsigned long long runtimeDigest);

		DomainMeasurementDescriptor(std::shared_ptr<SecureMeasurementHandle> handle, SecureMeasurementState state, SecureMeasurementDebugClass debugClass, unsigned long long policyDigest, unsigned long long memoryDigest, unsigned long long runtimeDigest, SecureEvidenceVisibilityClass attestationEvidenceClass, unsigned long long creatorDomainTag, unsigned long long parentMeasurementId, unsigned long long policySourceHash);

		static std::shared_ptr<DomainMeasurementDescriptor> getMissing();

		std::shared_ptr<SecureMeasurementHandle> getHandle() const;

		SecureMeasurementState getState() const;

		SecureMeasurementDebugClass getDebugClass() const;

		unsigned long long getPolicyDigest() const;

		unsigned long long getMemoryDigest() const;

		unsigned long long getRuntimeDigest() const;

		SecureEvidenceVisibilityClass getAttestationEvidenceClass() const;

		unsigned long long getCreatorDomainTag() const;

		unsigned long long getParentMeasurementId() const;

		unsigned long long getPolicySourceHash() const;

		unsigned long long getMeasurementEpoch() const;

		bool isMaterialized() const;

		bool isAttestationEvidenceOnly() const;

		bool isStale() const;

		bool IsCurrentFor(std::shared_ptr<SecureRevocationEpoch> policyEpoch);

		bool BindsDomain(unsigned long long domainTag);

		bool BindsPolicyDigest(unsigned long long expectedPolicyDigest);

		bool BindsMemoryDigest(unsigned long long expectedMemoryDigest);

		bool MustRevalidateOnRestore(std::shared_ptr<SecureMigrationDescriptor> migrationPolicy);

		static unsigned long long ComputePolicyDigest(std::shared_ptr<SecureComputeDomainDescriptor> descriptor);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ulong ComputeMemoryDigest(SecureMemoryDomainDescriptor? memory)
		static unsigned long long ComputeMemoryDigest(std::shared_ptr<SecureMemoryDomainDescriptor> memory);

	private:
		static unsigned long long Mix(unsigned long long digest, unsigned long long value);

		static unsigned long long MixIoPolicy(unsigned long long digest, std::shared_ptr<SecureIoDomainDescriptor> policy);

		static unsigned long long MixHypercallPolicy(unsigned long long digest, std::shared_ptr<SecureHypercallDescriptor> policy);

		static unsigned long long MixGrantHandle(unsigned long long digest, SecureGrantHandle grant);

		static unsigned long long MixGrantHandle(unsigned long long digest, std::shared_ptr<SecureGrantHandle> grant);

		static unsigned long long NonZero(unsigned long long value);
	};
}
