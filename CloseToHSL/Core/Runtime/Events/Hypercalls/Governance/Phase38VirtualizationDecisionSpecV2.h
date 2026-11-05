#pragma once

#include "../../../Governance/Virtualization/VirtualizationDecisionCanonicalEncoderV2.h"
#include "../../../Governance/Virtualization/VirtualizationDecisionContractsV2.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// The byte-exact Phase 38 D2 specification artifact. This is immutable policy
	/// input only: it is not an owner snapshot, capability, admission certificate,
	/// executor, completion record or retire authorization.
	/// </summary>
	class Phase38VirtualizationDecisionSpecV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> Create();

	public:
		static std::shared_ptr<VirtualizationDecisionSpecV2> getInstance();

	public:
		static std::vector<unsigned char> getCanonicalBytes();

		static bool getRuntimeAuthorityGranted();
		static bool getBackendExecutionAuthorized();
		static bool getCompletionPublicationAuthorized();
		static bool getRetirePublicationAuthorized();

	private:
		static std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> CreateOwnerMap();

		static std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Entry(const std::wstring &field, const std::wstring &owner);
	};

}
