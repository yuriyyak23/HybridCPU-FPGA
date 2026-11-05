#pragma once



namespace YAKSys_Hybrid_CPU::Core
{

	enum class CpuSecondStageSourceBindDecision : unsigned char
	{
		Bound = 0,
		AdmissionDenied = 1,
		SourceBindDenied = 2,
		MissingSecondStageRoot = 3,
		ScopeMismatch = 4
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct CpuSecondStageSourceBindResult(CpuSecondStageSourceBindDecision Decision, MemoryDomainRuntime.CpuSecondStageTranslationBinding? Binding, string Reason)
//	internal readonly record struct CpuSecondStageSourceBindResult(CpuSecondStageSourceBindDecision Decision, MemoryDomainRuntime.CpuSecondStageTranslationBinding Binding, string Reason)
//	{
//		internal bool IsBound
//		{
//			get
//			{
//				return Decision == CpuSecondStageSourceBindDecision.Bound && Binding != nullptr;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct CpuSecondStageTranslationSnapshot(MemoryDomainDescriptor SourceOwner, ulong SecondStageRoot, ulong DomainIdentity, ulong AddressSpaceIdentity, ulong MemoryDomainOwnerEpoch, ulong AddressSpaceGeneration, ulong BindingGeneration, AddressSpaceDescriptor AddressSpace)
//	{
//		internal bool IsValid
//		{
//			get
//			{
//				return SecondStageRoot != 0 && DomainIdentity != 0 && AddressSpaceIdentity != 0 && MemoryDomainOwnerEpoch != 0 && AddressSpaceGeneration != 0 && BindingGeneration != 0 && AddressSpace.IsRuntimeAuthoritative;
//			}
//		}
//	}


}
