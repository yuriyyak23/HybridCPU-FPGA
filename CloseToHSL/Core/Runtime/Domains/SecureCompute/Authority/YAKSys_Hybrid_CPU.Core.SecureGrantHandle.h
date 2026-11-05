#pragma once

#include "SecureRevocationEpoch.h"
#include "SecureGrantHandle.h"

namespace YAKSys_Hybrid_CPU::Core
{
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly partial record struct SecureGrantHandle(SecureGrantHandleKind Kind, ulong LocalId, ulong ProvenanceHash, ulong Epoch)
//	{
//		public static SecureGrantHandle None
//		{
//			get;
//		} = new(SecureGrantHandleKind.None, 0, 0, 0);
//
//		public bool HasScalarShape
//		{
//			get
//			{
//				return Kind != SecureGrantHandleKind.None && LocalId != 0 && Epoch != 0;
//			}
//		}
//
//		public bool HasProvenance
//		{
//			get
//			{
//				return ProvenanceHash != 0;
//			}
//		}
//
//		public bool IsMaterialized
//		{
//			get
//			{
//				return HasScalarShape && HasProvenance;
//			}
//		}
//
//		public bool MatchesEpoch(SecureRevocationEpoch epoch)
//		{
//			return IsMaterialized && epoch.IsCurrent(Epoch);
//		}
//
//		public static bool TryMaterializeFromGuestScalar(ulong guestVisibleScalar, out SecureGrantHandle handle)
//		{
//			_ = guestVisibleScalar;
//			handle = None;
//			return false;
//		}
//	}
}
