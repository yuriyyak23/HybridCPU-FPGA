#pragma once

#include <string>
#include <memory>
#include <any>

namespace YAKSys_Hybrid_CPU::Core
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ExecutionDomainStateEpoch : IEquatable<ExecutionDomainStateEpoch>
	class ExecutionDomainStateEpoch
	{
	private:
		unsigned long long Value = 0;

	public:
		ExecutionDomainStateEpoch(unsigned long long value);

		unsigned long long getValue() const;
		bool isMaterialized() const;

		bool Equals(ExecutionDomainStateEpoch other) const ;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public override bool Equals(Object? obj)
		bool Equals(std::any obj) const;
		int GetHashCode() const;
		bool operator == (ExecutionDomainStateEpoch right);
		bool operator != (ExecutionDomainStateEpoch right);
		std::wstring ToString() const;

		ExecutionDomainStateEpoch() = default;
	};

	enum class ExecutionDomainSourceBindDecision : unsigned char
	{
		Bound = 0,
		MissingDescriptor = 1,
		DescriptorAuthorityDenied = 2,
		CompatibilityProjectionDenied = 3,
		MissingDomainIdentity = 4,
		MissingAddressSpaceIdentity = 5,
		IncompleteSourceState = 6
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct ExecutionDomainSourceBindResult(ExecutionDomainSourceBindDecision Decision, ExecutionDomainDescriptor? Descriptor, ExecutionDomainStateEpoch Epoch, string Reason)
//	internal readonly record struct ExecutionDomainSourceBindResult(ExecutionDomainSourceBindDecision Decision, ExecutionDomainDescriptor Descriptor, ExecutionDomainStateEpoch Epoch, string Reason)
//	{
//		internal bool IsBound
//		{
//			get
//			{
//				return Decision == ExecutionDomainSourceBindDecision.Bound && Descriptor != nullptr && Epoch.IsMaterialized;
//			}
//		}
//	}

	class ExecutionDomainDescriptor;
class ExecutionDomainSourceCaptureResult;
struct ExecutionDomainSourceBindResult final {
    ExecutionDomainSourceBindDecision Decision; std::shared_ptr<ExecutionDomainDescriptor> Descriptor; ExecutionDomainStateEpoch Epoch; std::wstring Reason;
    bool isBound() const noexcept { return Decision == ExecutionDomainSourceBindDecision::Bound && Descriptor != nullptr && Epoch.isMaterialized(); }
};
enum class ExecutionDomainSourceCaptureDecision : unsigned char
	{
		Captured = 0,
		SourceUnbound = 1,
		StaleOrForeignDescriptor = 2,
		DomainMismatch = 3,
		AddressSpaceMismatch = 4,
		SourceEpochUnmaterialized = 5,
		SourceEpochMismatch = 6,
		FieldDenied = 7,
		FieldUnmaterialized = 8
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
// ORIGINAL LINE: internal readonly record struct ExecutionDomainSourceCaptureResult(ExecutionDomainSourceCaptureDecision Decision, ExecutionDomainRuntime.SourceCapture? Capture, string Reason)
//	internal readonly record struct ExecutionDomainSourceCaptureResult(ExecutionDomainSourceCaptureDecision Decision, ExecutionDomainRuntime.SourceCapture Capture, string Reason)
//	{
//		internal bool IsCaptured
//		{
//			get
//			{
//				return Decision == ExecutionDomainSourceCaptureDecision.Captured && Capture != nullptr;
//			}
//		}
//	}


}
