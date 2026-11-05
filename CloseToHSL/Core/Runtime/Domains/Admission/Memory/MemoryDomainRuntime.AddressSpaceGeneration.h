#pragma once
#include <memory>
#include <string>

namespace YAKSys_Hybrid_CPU::Core
{

	enum class MemoryDomainSourceBindDecision : unsigned char
	{
		Bound = 0,
		MissingDescriptor = 1,
		InvalidTranslationControl = 2,
		TranslationDisabled = 3,
		MissingDomainIdentity = 4,
		MissingAddressSpaceIdentity = 5
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct MemoryDomainSourceBindResult(MemoryDomainSourceBindDecision Decision, MemoryDomainDescriptor? Descriptor, ulong AddressSpaceGeneration, string Reason)
//	internal readonly record struct MemoryDomainSourceBindResult(MemoryDomainSourceBindDecision Decision, MemoryDomainDescriptor Descriptor, ulong AddressSpaceGeneration, string Reason)
//	{
//		internal bool IsBound
//		{
//			get
//			{
//				return Decision == MemoryDomainSourceBindDecision.Bound && Descriptor != nullptr && AddressSpaceGeneration != 0;
//			}
//		}
//	}

	class MemoryDomainDescriptor;
	struct MemoryDomainSourceCaptureResult;
struct MemoryDomainSourceBindResult final {
    MemoryDomainSourceBindDecision Decision;
    std::shared_ptr<MemoryDomainDescriptor> Descriptor;
    unsigned long long AddressSpaceGeneration;
    std::wstring Reason;
    bool isBound() const noexcept { return Decision == MemoryDomainSourceBindDecision::Bound && Descriptor != nullptr && AddressSpaceGeneration != 0; }
};
enum class MemoryDomainSourceCaptureDecision : unsigned char
	{
		Captured = 0,
		SourceUnbound = 1,
		StaleOrForeignDescriptor = 2,
		DomainMismatch = 3,
		AddressSpaceMismatch = 4,
		GenerationUnmaterialized = 5,
		GenerationMismatch = 6,
		FieldDenied = 7,
		SecondStageDenied = 8,
		TaggingDenied = 9,
		TargetCountDenied = 10
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct MemoryDomainSourceCaptureResult(MemoryDomainSourceCaptureDecision Decision, MemoryDomainRuntime.SourceCapture? Capture, string Reason)
//	internal readonly record struct MemoryDomainSourceCaptureResult(MemoryDomainSourceCaptureDecision Decision, MemoryDomainRuntime.SourceCapture Capture, string Reason)
//	{
//		internal bool IsCaptured
//		{
//			get
//			{
//				return Decision == MemoryDomainSourceCaptureDecision.Captured && Capture != nullptr;
//			}
//		}
//	}


}
