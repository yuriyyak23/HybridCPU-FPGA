#pragma once
#include "../../../Pipeline/Metadata/BundleWidth.h"
#include <stdexcept>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class NeutralArchitecturalCompletionClass : unsigned char
	{
		None = 0,
		TrapEntry = 1,
		SystemEvent = 2,
		TranslationFault = 3,
		SecureNoEffect = 4,
		SecureRegisterLocal = 5
	};

	enum class NeutralFaultAddressSemantic : unsigned char
	{
		None = 0,
		VirtualAddress = 1,
		GuestPhysicalAddress = 2,
		HostPhysicalAddress = 3
	};

	enum class NeutralFaultAuxiliarySemantic : unsigned char
	{
		None = 0,
		TranslationFault = 1,
		SecondStageTranslationViolation = 2,
		AccessPolicy = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralScalarFact(bool IsPresent, ulong Value)
//	{
//		internal static NeutralScalarFact Absent
//		{
//			get
//			{
//				return default;
//			}
//		}
//
//		internal static NeutralScalarFact Present(ulong value)
//		{
//			return new(true, value);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralAddressFact(bool IsPresent, ulong Value, NeutralFaultAddressSemantic Semantic)
//	{
//		internal static NeutralAddressFact Absent
//		{
//			get
//			{
//				return default;
//			}
//		}
//
//		internal static NeutralAddressFact Present(ulong value, NeutralFaultAddressSemantic semantic)
//		{
//			if (semantic == NeutralFaultAddressSemantic.None)
//				throw new ArgumentOutOfRangeException(nameof(semantic));
//			return new(true, value, semantic);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralAuxiliaryFact(bool IsPresent, ulong Value, NeutralFaultAuxiliarySemantic Semantic)
//	{
//		internal static NeutralAuxiliaryFact Absent
//		{
//			get
//			{
//				return default;
//			}
//		}
//
//		internal static NeutralAuxiliaryFact Present(ulong value, NeutralFaultAuxiliarySemantic semantic)
//		{
//			if (semantic == NeutralFaultAuxiliarySemantic.None)
//				throw new ArgumentOutOfRangeException(nameof(semantic));
//			return new(true, value, semantic);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralArchitecturalCompletionFacts(NeutralArchitecturalCompletionClass CompletionClass, NeutralScalarFact Reason, NeutralScalarFact Qualification, NeutralAddressFact FaultAddress, NeutralAuxiliaryFact FaultAuxiliary);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct NeutralTranslationProvenance(bool IsPresent, ulong MemoryOperationId, ulong TranslationOwnerEpoch, ulong MemoryDomainOwnerEpoch, ulong AddressSpaceGeneration, ulong AddressSpaceIdentity)
//	{
//		internal static NeutralTranslationProvenance Present(ulong memoryOperationId, ulong translationOwnerEpoch, ulong memoryDomainOwnerEpoch = 0, ulong addressSpaceGeneration = 0, ulong addressSpaceIdentity = 0)
//		{
//			return new(true, memoryOperationId, translationOwnerEpoch, memoryDomainOwnerEpoch, addressSpaceGeneration, addressSpaceIdentity);
//		}
//
//		internal bool HasStageOneIdentity
//		{
//			get
//			{
//				return IsPresent && MemoryOperationId != 0 && TranslationOwnerEpoch != 0;
//			}
//		}
//
//		internal bool HasSecondStageIdentity
//		{
//			get
//			{
//				return HasStageOneIdentity && MemoryDomainOwnerEpoch != 0 && AddressSpaceGeneration != 0 && AddressSpaceIdentity != 0;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal readonly record struct ArchitecturalCompletionCandidate(ulong DomainId, int ContextId, int VirtualThreadId, ulong AttemptId, ulong EventId, NeutralArchitecturalCompletionFacts Facts, NeutralTranslationProvenance TranslationProvenance = default);

	class ArchitecturalCompletionEventIdentity final
	{
	public:
		static unsigned long long Create(unsigned long long bundleSequence, int slotIndex);
	};

struct NeutralScalarFact final { bool IsPresent = false; unsigned long long Value = 0; static NeutralScalarFact Present(unsigned long long value) noexcept { return {true, value}; } };
struct NeutralAddressFact final {
    bool IsPresent = false; unsigned long long Value = 0; NeutralFaultAddressSemantic Semantic = NeutralFaultAddressSemantic::None;
    static NeutralAddressFact Present(unsigned long long value, NeutralFaultAddressSemantic semantic) { if (semantic == NeutralFaultAddressSemantic::None) throw std::invalid_argument("semantic"); return {true, value, semantic}; }
};
struct NeutralAuxiliaryFact final {
    bool IsPresent = false; unsigned long long Value = 0; NeutralFaultAuxiliarySemantic Semantic = NeutralFaultAuxiliarySemantic::None;
    static NeutralAuxiliaryFact Present(unsigned long long value, NeutralFaultAuxiliarySemantic semantic) { if (semantic == NeutralFaultAuxiliarySemantic::None) throw std::invalid_argument("semantic"); return {true, value, semantic}; }
};
struct NeutralArchitecturalCompletionFacts final {
    NeutralArchitecturalCompletionClass CompletionClass = NeutralArchitecturalCompletionClass::None;
    NeutralScalarFact Reason, Qualification; NeutralAddressFact FaultAddress; NeutralAuxiliaryFact FaultAuxiliary;
};
struct NeutralTranslationProvenance final {
    bool IsPresent = false; unsigned long long MemoryOperationId = 0, TranslationOwnerEpoch = 0, MemoryDomainOwnerEpoch = 0, AddressSpaceGeneration = 0, AddressSpaceIdentity = 0;
    static NeutralTranslationProvenance Present(unsigned long long operation, unsigned long long translationEpoch, unsigned long long ownerEpoch = 0, unsigned long long generation = 0, unsigned long long addressIdentity = 0) noexcept { return {true, operation, translationEpoch, ownerEpoch, generation, addressIdentity}; }
    bool hasStageOneIdentity() const noexcept { return IsPresent && MemoryOperationId && TranslationOwnerEpoch; }
    bool hasSecondStageIdentity() const noexcept { return hasStageOneIdentity() && MemoryDomainOwnerEpoch && AddressSpaceGeneration && AddressSpaceIdentity; }
};
struct ArchitecturalCompletionCandidate final {
    unsigned long long DomainId = 0; int ContextId = 0, VirtualThreadId = 0; unsigned long long AttemptId = 0, EventId = 0;
    NeutralArchitecturalCompletionFacts Facts; NeutralTranslationProvenance TranslationProvenance;
};
}
