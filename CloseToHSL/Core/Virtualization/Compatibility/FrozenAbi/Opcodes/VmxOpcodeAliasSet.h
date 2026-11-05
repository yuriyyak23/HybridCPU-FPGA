#pragma once
#include <string>
#include <utility>
#include <span>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class VmxOpcodeAliasKind : unsigned char
	{
		Core = 0,
		PointerAndCall = 1,
		Extension = 2
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct VmxOpcodeAlias(ushort Opcode, VmxOpcodeAliasKind Kind, string Name);


struct VmxOpcodeAlias final
{
    unsigned short Opcode = 0;
    VmxOpcodeAliasKind Kind{};
    std::wstring Name;
    VmxOpcodeAlias(unsigned short opcode, VmxOpcodeAliasKind kind, std::wstring name)
        : Opcode(opcode), Kind(kind), Name(std::move(name)) {}
};
}
