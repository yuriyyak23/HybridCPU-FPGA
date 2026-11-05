#pragma once

#include "VmxOpcodeAliasSet.h"
#include <vector>
#include <memory>

namespace YAKSys_Hybrid_CPU::Core
{
	class VmxOpcodeAliasSet final : public std::enable_shared_from_this<VmxOpcodeAliasSet>
	{
	private:
		static inline const std::vector<std::shared_ptr<VmxOpcodeAlias>> FrozenAliasTable = {std::make_shared<VmxOpcodeAlias>(250, VmxOpcodeAliasKind::Core, L"VMXON"), std::make_shared<VmxOpcodeAlias>(251, VmxOpcodeAliasKind::Core, L"VMXOFF"), std::make_shared<VmxOpcodeAlias>(252, VmxOpcodeAliasKind::Core, L"VMLAUNCH"), std::make_shared<VmxOpcodeAlias>(253, VmxOpcodeAliasKind::Core, L"VMRESUME"), std::make_shared<VmxOpcodeAlias>(254, VmxOpcodeAliasKind::Core, L"VMREAD"), std::make_shared<VmxOpcodeAlias>(255, VmxOpcodeAliasKind::Core, L"VMWRITE"), std::make_shared<VmxOpcodeAlias>(256, VmxOpcodeAliasKind::Core, L"VMCLEAR"), std::make_shared<VmxOpcodeAlias>(257, VmxOpcodeAliasKind::Core, L"VMPTRLD"), std::make_shared<VmxOpcodeAlias>(258, VmxOpcodeAliasKind::PointerAndCall, L"VMPTRST"), std::make_shared<VmxOpcodeAlias>(259, VmxOpcodeAliasKind::PointerAndCall, L"VMCALL"), std::make_shared<VmxOpcodeAlias>(267, VmxOpcodeAliasKind::Extension, L"INVEPT"), std::make_shared<VmxOpcodeAlias>(268, VmxOpcodeAliasKind::Extension, L"INVVPID"), std::make_shared<VmxOpcodeAlias>(269, VmxOpcodeAliasKind::Extension, L"VMFUNC"), std::make_shared<VmxOpcodeAlias>(270, VmxOpcodeAliasKind::Extension, L"VMSAVEX"), std::make_shared<VmxOpcodeAlias>(271, VmxOpcodeAliasKind::Extension, L"VMRESTX")};

	public:
		static std::span<const std::shared_ptr<VmxOpcodeAlias>> getFrozenAliases();

		bool Contains(unsigned short opcode);

		bool TryGetAlias(unsigned short opcode, std::shared_ptr<VmxOpcodeAlias> &alias);
	};
}
