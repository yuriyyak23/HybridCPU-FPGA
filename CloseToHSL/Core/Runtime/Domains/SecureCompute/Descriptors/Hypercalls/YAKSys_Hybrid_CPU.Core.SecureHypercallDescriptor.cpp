#include "YAKSys_Hybrid_CPU.Core.SecureHypercallDescriptor.h"

namespace YAKSys_Hybrid_CPU::Core
{

	SecureHypercallDescriptor::SecureHypercallDescriptor() : SecureHypercallDescriptor(true, false, std::vector<unsigned long long>{}, std::make_shared<SecureGrantHandle>(), std::vector<std::shared_ptr<SecureHypercallArgumentDescriptor>>{}, true, true, true)
	{
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureHypercallDescriptor(bool neutralBackendOwnerRequired, bool allowBackendExecution, SecureGrantHandle requiredGrant, IReadOnlyList<SecureHypercallArgumentDescriptor> arguments, bool requireEvidenceApproval, bool requireCompletionFence, bool requireRetirePublicationRule) : this(neutralBackendOwnerRequired, allowBackendExecution, System.Array.Empty<ulong>(), requiredGrant, arguments, requireEvidenceApproval, requireCompletionFence, requireRetirePublicationRule)
	SecureHypercallDescriptor::SecureHypercallDescriptor(bool neutralBackendOwnerRequired, bool allowBackendExecution, std::shared_ptr<SecureGrantHandle> requiredGrant, const std::vector<std::shared_ptr<SecureHypercallArgumentDescriptor>> &arguments, bool requireEvidenceApproval, bool requireCompletionFence, bool requireRetirePublicationRule) : SecureHypercallDescriptor(neutralBackendOwnerRequired, allowBackendExecution, std::vector<unsigned long long>{}, requiredGrant, arguments, requireEvidenceApproval, requireCompletionFence, requireRetirePublicationRule)
	{
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public SecureHypercallDescriptor(bool neutralBackendOwnerRequired, bool allowBackendExecution, IReadOnlyList<ulong> allowedHypercallIds, SecureGrantHandle requiredGrant, IReadOnlyList<SecureHypercallArgumentDescriptor> arguments, bool requireEvidenceApproval, bool requireCompletionFence, bool requireRetirePublicationRule)
	SecureHypercallDescriptor::SecureHypercallDescriptor(bool neutralBackendOwnerRequired, bool allowBackendExecution, const std::vector<unsigned long long> &allowedHypercallIds, std::shared_ptr<SecureGrantHandle> requiredGrant, const std::vector<std::shared_ptr<SecureHypercallArgumentDescriptor>> &arguments, bool requireEvidenceApproval, bool requireCompletionFence, bool requireRetirePublicationRule)
		: NeutralBackendOwnerRequired(neutralBackendOwnerRequired), AllowBackendExecution(allowBackendExecution), AllowedHypercallIds(allowedHypercallIds), RequiredGrant(std::move(requiredGrant)), Arguments(arguments), RequireEvidenceApproval(requireEvidenceApproval), RequireCompletionFence(requireCompletionFence), RequireRetirePublicationRule(requireRetirePublicationRule)
	{
	}

	std::shared_ptr<SecureHypercallDescriptor> SecureHypercallDescriptor::getDisabled()
	{
		return Disabled;
	}

	bool SecureHypercallDescriptor::getNeutralBackendOwnerRequired() const
	{
		return NeutralBackendOwnerRequired;
	}

	bool SecureHypercallDescriptor::getAllowBackendExecution() const
	{
		return AllowBackendExecution;
	}

	const std::vector<unsigned long long> SecureHypercallDescriptor::getAllowedHypercallIds() const
	{
		return AllowedHypercallIds;
	}

	std::shared_ptr<SecureGrantHandle> SecureHypercallDescriptor::getRequiredGrant() const
	{
		return RequiredGrant;
	}

	const std::vector<std::shared_ptr<SecureHypercallArgumentDescriptor>> SecureHypercallDescriptor::getArguments() const
	{
		return Arguments;
	}

	bool SecureHypercallDescriptor::getRequireEvidenceApproval() const
	{
		return RequireEvidenceApproval;
	}

	bool SecureHypercallDescriptor::getRequireCompletionFence() const
	{
		return RequireCompletionFence;
	}

	bool SecureHypercallDescriptor::getRequireRetirePublicationRule() const
	{
		return RequireRetirePublicationRule;
	}

	bool SecureHypercallDescriptor::isDenialOnly() const
	{
		return !getAllowBackendExecution();
	}

	bool SecureHypercallDescriptor::getHasPolicy() const
	{
		return getAllowedHypercallIds().size() != 0;
	}

	bool SecureHypercallDescriptor::AllowsHypercallId(unsigned long long hypercallId)
	{
		for (auto allowed : getAllowedHypercallIds())
		{
			if (allowed == hypercallId)
			{
				return true;
			}
		}

		return false;
	}

	bool SecureHypercallDescriptor::HasDeniedRawPrivatePointer()
	{
		for (auto argument : getArguments())
		{
			if (argument->isDeniedRawPrivatePointer())
			{
				return true;
			}
		}

		return false;
	}
}
