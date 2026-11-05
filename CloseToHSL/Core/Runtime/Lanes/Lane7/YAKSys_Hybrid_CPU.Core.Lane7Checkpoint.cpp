#include "YAKSys_Hybrid_CPU.Core.Lane7Checkpoint.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core
{

	bool Lane7Checkpoint::ContainsNativeTokenHandle(std::shared_ptr<AcceleratorTokenHandle> hostHandle)
	{
		return false;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal Lane7Checkpoint(ushort executionDomainTag, ushort addressSpaceTag, bool virtualizationEnabled, IReadOnlyList<Lane7VirtualHandle> handles, IReadOnlyList<Lane7VirtualToken> tokens, ulong ownershipEpoch, ulong tokenEpoch, ulong completionEpoch)
	Lane7Checkpoint::Lane7Checkpoint(unsigned short executionDomainTag, unsigned short addressSpaceTag, bool virtualizationEnabled, const std::vector<std::shared_ptr<Lane7VirtualHandle>> &handles, const std::vector<std::shared_ptr<Lane7VirtualToken>> &tokens, unsigned long long ownershipEpoch, unsigned long long tokenEpoch, unsigned long long completionEpoch) : ExecutionDomainTag(executionDomainTag), AddressSpaceTag(addressSpaceTag), VirtualizationEnabled(virtualizationEnabled), VirtualHandles(handles), VirtualTokens(tokens), OwnershipEpoch(ownershipEpoch), TokenEpoch(tokenEpoch), CompletionEpoch(completionEpoch)
	{
	}

	unsigned short Lane7Checkpoint::getExecutionDomainTag() const
	{
		return ExecutionDomainTag;
	}

	unsigned short Lane7Checkpoint::getAddressSpaceTag() const
	{
		return AddressSpaceTag;
	}

	bool Lane7Checkpoint::getVirtualizationEnabled() const
	{
		return VirtualizationEnabled;
	}

	const std::vector<std::shared_ptr<Lane7VirtualHandle>> Lane7Checkpoint::getVirtualHandles() const
	{
		return VirtualHandles;
	}

	const std::vector<std::shared_ptr<Lane7VirtualToken>> Lane7Checkpoint::getVirtualTokens() const
	{
		return VirtualTokens;
	}

	unsigned long long Lane7Checkpoint::getOwnershipEpoch() const
	{
		return OwnershipEpoch;
	}

	unsigned long long Lane7Checkpoint::getTokenEpoch() const
	{
		return TokenEpoch;
	}

	unsigned long long Lane7Checkpoint::getCompletionEpoch() const
	{
		return CompletionEpoch;
	}

	bool Lane7Checkpoint::ContainsHostEvidence(Vmcs::V2::VmcsV2HostEvidenceKind evidence)
	{
		return false;
	}
}
