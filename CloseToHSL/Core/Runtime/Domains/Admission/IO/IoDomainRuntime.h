#pragma once

namespace YAKSys_Hybrid_CPU::Core
{

	enum class IoDomainRuntimeDecision : unsigned char
	{
		Allowed = 0,
		MissingDescriptor = 1,
		DescriptorAuthorityDenied = 2,
		DmaAuthorityDenied = 3,
		IommuAuthorityDenied = 4,
		MissingVirtualizationBlock = 5,
		MissingDmaWindow = 6,
		CompatibilityProjectionDenied = 7
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: public readonly record struct IoDomainRuntimeRequest(IoDomainDescriptor? Descriptor, bool RequiresDmaAuthority, bool RequiresIommuAuthority, bool RequiresVirtualizationBlock, bool RequiresDmaWindow, bool RequiresCompatibilityProjection);
//	public readonly record struct IoDomainRuntimeRequest(IoDomainDescriptor Descriptor, bool RequiresDmaAuthority, bool RequiresIommuAuthority, bool RequiresVirtualizationBlock, bool RequiresDmaWindow, bool RequiresCompatibilityProjection);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct IoDomainRuntimeResult(IoDomainRuntimeDecision Decision, string Reason)
//	{
//		public bool IsAllowed
//		{
//			get
//			{
//				return Decision == IoDomainRuntimeDecision.Allowed;
//			}
//		}
//
//		public static IoDomainRuntimeResult Allowed
//		{
//			get;
//		} = new(IoDomainRuntimeDecision.Allowed, "I/O domain runtime admission allowed.");
//
//		public static IoDomainRuntimeResult Denied(IoDomainRuntimeDecision decision, string reason)
//		{
//			return new(decision, reason);
//		}
//	}


}
