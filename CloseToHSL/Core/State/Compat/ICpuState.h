#pragma once

#include "../ICpuState.h"

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Compat
{
	/// <summary>
	/// Legacy compatibility surface retained only for compat/test callers that still
	/// exercise the old non-VT shims. Production execution paths should depend on
	/// <see cref="ICanonicalCpuState"/> instead.
	/// </summary>
	class ICpuState : public ICanonicalCpuState
	{
	public:
// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use ReadRegister(byte vtId, int regId) for VT-scoped access.")] ulong ReadIntRegister(ushort regID);
		virtual unsigned long long ReadIntRegister(unsigned short regID) = 0;

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use WriteRegister(byte vtId, int regId, ulong value) for VT-scoped access.")] void WriteIntRegister(ushort regID, ulong value);
		virtual void WriteIntRegister(unsigned short regID, unsigned long long value) = 0;

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use ReadPc(byte vtId) for VT-scoped access.")] ulong GetInstructionPointer();
		virtual unsigned long long GetInstructionPointer() = 0;

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Obsolete("Use WritePc(byte vtId, ulong pc) for VT-scoped access.")] void SetInstructionPointer(ulong ip);
		virtual void SetInstructionPointer(unsigned long long ip) = 0;
	};
}
