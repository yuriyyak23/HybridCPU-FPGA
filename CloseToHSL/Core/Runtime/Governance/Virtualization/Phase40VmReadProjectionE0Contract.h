#pragma once

#include "../../../Virtualization/Compatibility/FrozenAbi/VmcsFieldAliases/VmcsField.h"
#include <memory>
#include <string>
#include <vector>

namespace YAKSys_Hybrid_CPU::Core
{
	struct VmReadProjectionE0FindingV2 final
	{
		unsigned char Number;
		std::wstring Name;
		std::wstring ExactContract;
	};

	/// <summary>
	/// Audited E0 input for the GuestCr0/GuestCr4 D2 decision. This is immutable
	/// governance evidence only and has no projection, capability or admission API.
	/// </summary>
	class Phase40VmReadProjectionE0Contract final
	{
	private:
		static const std::vector<unsigned short> &ExactFieldIds();
		static const std::vector<std::shared_ptr<VmReadProjectionE0FindingV2>> &Findings();

	public:
		static std::vector<unsigned short> getExactFieldIds();

		static std::vector<std::shared_ptr<VmReadProjectionE0FindingV2>> getFindings();

		static bool getRuntimeAuthorityGranted();
		static bool getProjectionValueAvailable();
		static bool getCapabilityGranted();
		static bool getBackendExecutionAuthorized();
		static bool getMutationAuthorized();
		static bool getCompletionPublicationAuthorized();
		static bool getRetirePublicationAuthorized();
	};

}
