#pragma once
#include <string>
#include <utility>
#include <span>
#include "../../../../../../Arch/CsrAddresses.h"

using namespace YAKSys_Hybrid_CPU::Arch;

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxCsrAliasKind : unsigned char
	{
		Enable = 0,
		CapabilityProjection = 1,
		Control = 2,
		CompletionProjection = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxCsrAlias(ushort Address, VmxCsrAliasKind Kind, string Name, bool IsReadOnlyProjection);


struct VmxCsrAlias final
{
    unsigned short Address = 0;
    VmxCsrAliasKind Kind{};
    std::wstring Name;
    bool IsReadOnlyProjection = false;
    VmxCsrAlias(unsigned short address, VmxCsrAliasKind kind, std::wstring name, bool readOnly)
        : Address(address), Kind(kind), Name(std::move(name)), IsReadOnlyProjection(readOnly) {}
};
}
