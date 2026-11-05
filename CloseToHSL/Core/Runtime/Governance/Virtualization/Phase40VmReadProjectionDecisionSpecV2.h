#pragma once

#include "VirtualizationDecisionCanonicalEncoderV2.h"
#include <string>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{

	/// <summary>
	/// Byte-exact governance specification for the GuestCr0/GuestCr4 read-only
	/// semantic group. It does not register, authorize or execute a VMREAD.
	/// </summary>
	class Phase40VmReadProjectionDecisionSpecV2 final
	{
	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> GetInstance();
		static std::vector<unsigned char> GetCanonicalBytes();

	public:
		static std::shared_ptr<VirtualizationDecisionSpecV2> getInstance();

		static std::vector<unsigned char> getCanonicalBytes();

		static bool getRuntimeAuthorityGranted();
		static bool getProjectionValueAvailable();
		static bool getCapabilityGranted();
		static bool getBackendExecutionAuthorized();
		static bool getMutationAuthorized();
		static bool getCompletionPublicationAuthorized();
		static bool getRetirePublicationAuthorized();

	private:
		static std::shared_ptr<VirtualizationDecisionSpecV2> Create();

		static std::vector<std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2>> CreateOwnerMap();

		static std::shared_ptr<VirtualizationDecisionOwnerMapEntryV2> Entry(const std::wstring &field, const std::wstring &valueSource);
	};

}
