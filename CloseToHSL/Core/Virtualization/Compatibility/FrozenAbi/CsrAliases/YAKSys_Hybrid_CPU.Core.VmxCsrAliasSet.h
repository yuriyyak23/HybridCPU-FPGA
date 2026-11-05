#pragma once

#include "../../../../../../Arch/CsrAddresses.h"
#include "VmxCsrAliasSet.h"
#include <vector>
#include <memory>

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxCsrAliasSet final : public std::enable_shared_from_this<VmxCsrAliasSet>
	{
	private:
		static inline const std::vector<std::shared_ptr<VmxCsrAlias>> FrozenAliasTable = {std::make_shared<VmxCsrAlias>(CsrAddresses::VmxEnable, VmxCsrAliasKind::Enable, L"VmxEnable", false), std::make_shared<VmxCsrAlias>(CsrAddresses::VmxCaps, VmxCsrAliasKind::CapabilityProjection, L"VmxCaps", true), std::make_shared<VmxCsrAlias>(CsrAddresses::VmxControl, VmxCsrAliasKind::Control, L"VmxControl", false), std::make_shared<VmxCsrAlias>(CsrAddresses::VmxExitReason, VmxCsrAliasKind::CompletionProjection, L"VmxExitReason", false), std::make_shared<VmxCsrAlias>(CsrAddresses::VmxExitQual, VmxCsrAliasKind::CompletionProjection, L"VmxExitQual", false)};

	public:
		static std::span<const std::shared_ptr<VmxCsrAlias>> getFrozenAliases();

		bool Contains(unsigned short address);

		bool TryGetAlias(unsigned short address, std::shared_ptr<VmxCsrAlias> &alias);

		bool IsReadOnlyProjection(unsigned short address);
	};
}
