#pragma once
#include "SecureHypercallDescriptor.h"

#include <vector>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core
{
	class SecureHypercallDescriptor final : public std::enable_shared_from_this<SecureHypercallDescriptor>
	{
	private:
		static inline const std::shared_ptr<SecureHypercallDescriptor> Disabled = std::make_shared<SecureHypercallDescriptor>();
		const bool NeutralBackendOwnerRequired;
		const bool AllowBackendExecution;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<ulong> AllowedHypercallIds;
		const std::vector<unsigned long long> AllowedHypercallIds;
		const std::shared_ptr<SecureGrantHandle> RequiredGrant;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<SecureHypercallArgumentDescriptor> Arguments;
		const std::vector<std::shared_ptr<SecureHypercallArgumentDescriptor>> Arguments;
		const bool RequireEvidenceApproval;
		const bool RequireCompletionFence;
		const bool RequireRetirePublicationRule;

	public:
		SecureHypercallDescriptor();

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureHypercallDescriptor(bool neutralBackendOwnerRequired, bool allowBackendExecution, SecureGrantHandle requiredGrant, IReadOnlyList<SecureHypercallArgumentDescriptor> arguments, bool requireEvidenceApproval, bool requireCompletionFence, bool requireRetirePublicationRule) : this(neutralBackendOwnerRequired, allowBackendExecution, System.Array.Empty<ulong>(), requiredGrant, arguments, requireEvidenceApproval, requireCompletionFence, requireRetirePublicationRule)
		SecureHypercallDescriptor(bool neutralBackendOwnerRequired, bool allowBackendExecution, std::shared_ptr<SecureGrantHandle> requiredGrant, const std::vector<std::shared_ptr<SecureHypercallArgumentDescriptor>> &arguments, bool requireEvidenceApproval, bool requireCompletionFence, bool requireRetirePublicationRule);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureHypercallDescriptor(bool neutralBackendOwnerRequired, bool allowBackendExecution, IReadOnlyList<ulong> allowedHypercallIds, SecureGrantHandle requiredGrant, IReadOnlyList<SecureHypercallArgumentDescriptor> arguments, bool requireEvidenceApproval, bool requireCompletionFence, bool requireRetirePublicationRule)
		SecureHypercallDescriptor(bool neutralBackendOwnerRequired, bool allowBackendExecution, const std::vector<unsigned long long> &allowedHypercallIds, std::shared_ptr<SecureGrantHandle> requiredGrant, const std::vector<std::shared_ptr<SecureHypercallArgumentDescriptor>> &arguments, bool requireEvidenceApproval, bool requireCompletionFence, bool requireRetirePublicationRule);

		static std::shared_ptr<SecureHypercallDescriptor> getDisabled();

		bool getNeutralBackendOwnerRequired() const;

		bool getAllowBackendExecution() const;

		const std::vector<unsigned long long> getAllowedHypercallIds() const;

		std::shared_ptr<SecureGrantHandle> getRequiredGrant() const;

		const std::vector<std::shared_ptr<SecureHypercallArgumentDescriptor>> getArguments() const;

		bool getRequireEvidenceApproval() const;

		bool getRequireCompletionFence() const;

		bool getRequireRetirePublicationRule() const;

		bool isDenialOnly() const;

		bool getHasPolicy() const;

		bool AllowsHypercallId(unsigned long long hypercallId);

		bool HasDeniedRawPrivatePointer();
	};
}
