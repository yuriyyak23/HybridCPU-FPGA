#pragma once
#include "Lane7StateBlock.h"

#include "../../../System/Vmcs/V2/VmcsV2HostEvidenceKind.h"
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Tokens;

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;


namespace YAKSys_Hybrid_CPU::Core
{
	class Lane7Checkpoint final : public std::enable_shared_from_this<Lane7Checkpoint>
	{
	private:
		const unsigned short ExecutionDomainTag;
		const unsigned short AddressSpaceTag;
		const bool VirtualizationEnabled;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<Lane7VirtualHandle> VirtualHandles;
		const std::vector<std::shared_ptr<Lane7VirtualHandle>> VirtualHandles;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<Lane7VirtualToken> VirtualTokens;
		const std::vector<std::shared_ptr<Lane7VirtualToken>> VirtualTokens;
		const unsigned long long OwnershipEpoch;
		const unsigned long long TokenEpoch;
		const unsigned long long CompletionEpoch;

	public:
		bool ContainsNativeTokenHandle(std::shared_ptr<AcceleratorTokenHandle> hostHandle);


// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal Lane7Checkpoint(ushort executionDomainTag, ushort addressSpaceTag, bool virtualizationEnabled, IReadOnlyList<Lane7VirtualHandle> handles, IReadOnlyList<Lane7VirtualToken> tokens, ulong ownershipEpoch, ulong tokenEpoch, ulong completionEpoch)
		Lane7Checkpoint(unsigned short executionDomainTag, unsigned short addressSpaceTag, bool virtualizationEnabled, const std::vector<std::shared_ptr<Lane7VirtualHandle>> &handles, const std::vector<std::shared_ptr<Lane7VirtualToken>> &tokens, unsigned long long ownershipEpoch, unsigned long long tokenEpoch, unsigned long long completionEpoch);

		unsigned short getExecutionDomainTag() const;

		unsigned short getAddressSpaceTag() const;

		bool getVirtualizationEnabled() const;

		const std::vector<std::shared_ptr<Lane7VirtualHandle>> getVirtualHandles() const;

		const std::vector<std::shared_ptr<Lane7VirtualToken>> getVirtualTokens() const;

		unsigned long long getOwnershipEpoch() const;

		unsigned long long getTokenEpoch() const;

		unsigned long long getCompletionEpoch() const;



		bool ContainsHostEvidence(Vmcs::V2::VmcsV2HostEvidenceKind evidence);

	};
}
