#pragma once
#include "../../Authority/SecureGrantHandle.h"

namespace YAKSys_Hybrid_CPU::Core
{

	enum class SecureHypercallArgumentClass : unsigned char
	{
		Immediate = 0,
		ExplicitSharedBuffer = 1,
		OpaqueHandle = 2,
		RawPrivatePointerDenied = 3
	};

struct SecureHypercallArgumentDescriptor final {
    unsigned char Index = 0;
    SecureHypercallArgumentClass ArgumentClass = SecureHypercallArgumentClass::Immediate;
    unsigned long long SharedBufferId = 0;
    SecureGrantHandle Grant;
    bool requiresSharedBuffer() const noexcept { return ArgumentClass == SecureHypercallArgumentClass::ExplicitSharedBuffer; }
    bool isDeniedRawPrivatePointer() const noexcept { return ArgumentClass == SecureHypercallArgumentClass::RawPrivatePointerDenied; }
    bool operator==(const SecureHypercallArgumentDescriptor&) const = default;
};
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct SecureHypercallArgumentDescriptor(byte Index, SecureHypercallArgumentClass ArgumentClass, ulong SharedBufferId, SecureGrantHandle Grant)
//	{
//		public bool RequiresSharedBuffer
//		{
//			get
//			{
//				return ArgumentClass == SecureHypercallArgumentClass.ExplicitSharedBuffer;
//			}
//		}
//
//		public bool IsDeniedRawPrivatePointer
//		{
//			get
//			{
//				return ArgumentClass == SecureHypercallArgumentClass.RawPrivatePointerDenied;
//			}
//		}
//	}


}
