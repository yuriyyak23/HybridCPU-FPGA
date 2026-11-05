#pragma once
#include "DmaStreamComputeOwnerGuardContext.h"
#include "DmaStreamComputeOwnerBinding.h"
#include "../../Pipeline/Safety/SafetyVerifier.Types.h"
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <utility>

using namespace YAKSys_Hybrid_CPU::Core;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	enum class DmaStreamComputeOperationKind : int
	{
		Copy = 1,
		Add = 2,
		Mul = 3,
		Fma = 4,
		Reduce = 5
	};

	enum class DmaStreamComputeElementType : int
	{
		UInt8 = 1,
		UInt16 = 2,
		UInt32 = 3,
		UInt64 = 4,
		Float32 = 5,
		Float64 = 6
	};

	enum class DmaStreamComputeShapeKind : int
	{
		Contiguous1D = 1,
		FixedReduce = 2
	};

	enum class DmaStreamComputeRangeEncoding : int
	{
		InlineContiguous = 1
	};

	enum class DmaStreamComputePartialCompletionPolicy : int
	{
		AllOrNone = 1
	};

	enum class DmaStreamComputeAliasPolicy : int
	{
		Disjoint = 0,
		ExactInPlaceSnapshot = 1
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeDescriptorReference
//	{
//		public DmaStreamComputeDescriptorReference(ulong descriptorAddress, uint descriptorSize, ulong descriptorIdentityHash)
//		{
//			DescriptorAddress = descriptorAddress;
//			DescriptorSize = descriptorSize;
//			DescriptorIdentityHash = descriptorIdentityHash;
//		}
//
//		public ulong DescriptorAddress
//		{
//			get;
//		}
//
//		public uint DescriptorSize
//		{
//			get;
//		}
//
//		public ulong DescriptorIdentityHash
//		{
//			get;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeMemoryRange(ulong Address, ulong Length);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeOwnerBinding
//	{
//		public required ushort OwnerVirtualThreadId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerContextId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerCoreId
//		{
//			get;
//			set;
//		}
//
//		public required uint OwnerPodId
//		{
//			get;
//			set;
//		}
//
//		public required ulong OwnerDomainTag
//		{
//			get;
//			set;
//		}
//
//		public required uint DeviceId
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeOwnerGuardContext
//	{
//		public DmaStreamComputeOwnerGuardContext(ushort ownerVirtualThreadId, uint ownerContextId, uint ownerCoreId, uint ownerPodId, ulong ownerDomainTag, ulong activeDomainCertificate, uint deviceId = DmaStreamComputeDescriptor.CanonicalLane6DeviceId)
//		{
//			OwnerVirtualThreadId = ownerVirtualThreadId;
//			OwnerContextId = ownerContextId;
//			OwnerCoreId = ownerCoreId;
//			OwnerPodId = ownerPodId;
//			OwnerDomainTag = ownerDomainTag;
//			ActiveDomainCertificate = activeDomainCertificate;
//			DeviceId = deviceId;
//		}
//
//		public ushort OwnerVirtualThreadId
//		{
//			get;
//			set;
//		}
//
//		public uint OwnerContextId
//		{
//			get;
//			set;
//		}
//
//		public uint OwnerCoreId
//		{
//			get;
//			set;
//		}
//
//		public uint OwnerPodId
//		{
//			get;
//			set;
//		}
//
//		public ulong OwnerDomainTag
//		{
//			get;
//			set;
//		}
//
//		public ulong ActiveDomainCertificate
//		{
//			get;
//			set;
//		}
//
//		public uint DeviceId
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeOwnerGuardDecision
//	{
//		private DmaStreamComputeOwnerGuardDecision(DmaStreamComputeOwnerBinding descriptorOwnerBinding, DmaStreamComputeOwnerGuardContext runtimeOwnerContext, LegalityDecision legalityDecision, string message)
//		{
//			DescriptorOwnerBinding = descriptorOwnerBinding;
//			RuntimeOwnerContext = runtimeOwnerContext;
//			LegalityDecision = legalityDecision;
//			Message = message;
//		}
//
//		public System.Nullable<DmaStreamComputeOwnerBinding> DescriptorOwnerBinding
//		{
//			get;
//		}
//
//		public DmaStreamComputeOwnerGuardContext RuntimeOwnerContext
//		{
//			get;
//		}
//
//		public LegalityDecision LegalityDecision
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool IsAllowed
//		{
//			get
//			{
//				return LegalityDecision.IsAllowed;
//			}
//		}
//
//		internal static DmaStreamComputeOwnerGuardDecision Allow(DmaStreamComputeOwnerBinding descriptorOwnerBinding, DmaStreamComputeOwnerGuardContext runtimeOwnerContext, string message)
//		{
//			ArgumentNullException.ThrowIfNull(descriptorOwnerBinding);
//			return new DmaStreamComputeOwnerGuardDecision(descriptorOwnerBinding, runtimeOwnerContext, LegalityDecision.Allow(LegalityAuthoritySource.GuardPlane, attemptedReplayCertificateReuse: false), message);
//		}
//
//		internal static DmaStreamComputeOwnerGuardDecision Reject(DmaStreamComputeOwnerBinding descriptorOwnerBinding, DmaStreamComputeOwnerGuardContext runtimeOwnerContext, RejectKind rejectKind, string message)
//		{
//			ArgumentNullException.ThrowIfNull(descriptorOwnerBinding);
//			return new DmaStreamComputeOwnerGuardDecision(descriptorOwnerBinding, runtimeOwnerContext, LegalityDecision.Reject(rejectKind, CertificateRejectDetail.None, LegalityAuthoritySource.GuardPlane, attemptedReplayCertificateReuse: false), message);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeStructuralReadResult
//	{
//		private DmaStreamComputeStructuralReadResult(DmaStreamComputeValidationFault fault, DmaStreamComputeDescriptorReference descriptorReference, ulong descriptorIdentityHash, uint totalSize, System.Nullable<DmaStreamComputeOwnerBinding> ownerBinding, string message)
//		{
//			Fault = fault;
//			DescriptorReference = descriptorReference;
//			DescriptorIdentityHash = descriptorIdentityHash;
//			TotalSize = totalSize;
//			OwnerBinding = ownerBinding;
//			Message = message;
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return Fault == DmaStreamComputeValidationFault.None && OwnerBinding != nullptr;
//			}
//		}
//
//		public bool IsStructuralDescriptorReadAccepted
//		{
//			get
//			{
//				return IsValid;
//			}
//		}
//
//		public DmaStreamComputeValidationFault Fault
//		{
//			get;
//		}
//
//		public DmaStreamComputeDescriptorReference DescriptorReference
//		{
//			get;
//		}
//
//		public ulong DescriptorIdentityHash
//		{
//			get;
//		}
//
//		public uint TotalSize
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeOwnerBinding> OwnerBinding
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static DmaStreamComputeStructuralReadResult Valid(DmaStreamComputeDescriptorReference descriptorReference, ulong descriptorIdentityHash, uint totalSize, DmaStreamComputeOwnerBinding ownerBinding)
//		{
//			ArgumentNullException.ThrowIfNull(ownerBinding);
//			return new DmaStreamComputeStructuralReadResult(DmaStreamComputeValidationFault.None, descriptorReference, descriptorIdentityHash, totalSize, ownerBinding, "Structural owner fields located; descriptor is not executable.");
//		}
//
//		public static DmaStreamComputeStructuralReadResult Fail(DmaStreamComputeValidationFault fault, string message)
//		{
//			if (fault == DmaStreamComputeValidationFault.None)
//			{
//				throw new ArgumentException("Use Valid for successful structural descriptor reads.", nameof(fault));
//			}
//
//			return new DmaStreamComputeStructuralReadResult(fault, default, descriptorIdentityHash: 0, totalSize: 0, ownerBinding: nullptr, message);
//		}
//
//		public DmaStreamComputeOwnerBinding RequireOwnerBindingForGuard()
//		{
//			if (IsStructuralDescriptorReadAccepted && OwnerBinding != nullptr)
//			{
//				return OwnerBinding;
//			}
//
//			throw new InvalidOperationException(string.Format("DmaStreamCompute structural owner read failed: {0}. {1}", Fault, Message));
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeDescriptor
//	{
//		public const uint CanonicalLane6DeviceId = 6;
//
//		public required DmaStreamComputeDescriptorReference DescriptorReference
//		{
//			get;
//			set;
//		}
//
//		public required ushort AbiVersion
//		{
//			get;
//			set;
//		}
//
//		public required ushort HeaderSize
//		{
//			get;
//			set;
//		}
//
//		public required uint TotalSize
//		{
//			get;
//			set;
//		}
//
//		public required ulong DescriptorIdentityHash
//		{
//			get;
//			set;
//		}
//
//		public required ulong CertificateInputHash
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeOperationKind Operation
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeElementType ElementType
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeShapeKind Shape
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeRangeEncoding RangeEncoding
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputePartialCompletionPolicy PartialCompletionPolicy
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeOwnerBinding OwnerBinding
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeOwnerGuardDecision OwnerGuardDecision
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<DmaStreamComputeMemoryRange> ReadMemoryRanges
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<DmaStreamComputeMemoryRange> NormalizedReadMemoryRanges
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<DmaStreamComputeMemoryRange> WriteMemoryRanges
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<DmaStreamComputeMemoryRange> NormalizedWriteMemoryRanges
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeAliasPolicy AliasPolicy
//		{
//			get;
//			set;
//		}
//
//		public required ulong NormalizedFootprintHash
//		{
//			get;
//			set;
//		}
//
//		public System.Nullable<VmxDmaDescriptorValidationEvidence> VmxValidationEvidence
//		{
//			get;
//			set;
//		}
//	}

	struct DmaStreamComputeDescriptorReference final {
		unsigned long long DescriptorAddress = 0;
		unsigned int DescriptorSize = 0;
		unsigned long long DescriptorIdentityHash = 0;
		bool operator==(const DmaStreamComputeDescriptorReference&) const = default;
	};
	class DmaStreamComputeMemoryRange final {
	public:
		unsigned long long Address = 0, Length = 0;
		bool operator==(const DmaStreamComputeMemoryRange&) const = default;
	};
	class DmaStreamComputeOwnerGuardDecision final {
	public:
		std::shared_ptr<DmaStreamComputeOwnerBinding> DescriptorOwnerBinding;
		DmaStreamComputeOwnerGuardContext RuntimeOwnerContext;
		YAKSys_Hybrid_CPU::Core::LegalityDecision LegalityDecision;
		std::wstring Message;
		bool IsAllowed;
		DmaStreamComputeOwnerGuardDecision(std::shared_ptr<DmaStreamComputeOwnerBinding> binding, DmaStreamComputeOwnerGuardContext context, YAKSys_Hybrid_CPU::Core::LegalityDecision decision, std::wstring message)
			: DescriptorOwnerBinding(std::move(binding)), RuntimeOwnerContext(context), LegalityDecision(decision), Message(std::move(message)), IsAllowed(LegalityDecision.isAllowed()) {}
		static std::shared_ptr<DmaStreamComputeOwnerGuardDecision> Allow(std::shared_ptr<DmaStreamComputeOwnerBinding> binding, DmaStreamComputeOwnerGuardContext context, const std::wstring& message) {
			if (!binding) throw std::invalid_argument("descriptorOwnerBinding");
			return std::make_shared<DmaStreamComputeOwnerGuardDecision>(std::move(binding), context, YAKSys_Hybrid_CPU::Core::LegalityDecision::Allow(LegalityAuthoritySource::GuardPlane, false), message);
		}
		static std::shared_ptr<DmaStreamComputeOwnerGuardDecision> Reject(std::shared_ptr<DmaStreamComputeOwnerBinding> binding, DmaStreamComputeOwnerGuardContext context, RejectKind reject, const std::wstring& message) {
			if (!binding) throw std::invalid_argument("descriptorOwnerBinding");
			return std::make_shared<DmaStreamComputeOwnerGuardDecision>(std::move(binding), context, YAKSys_Hybrid_CPU::Core::LegalityDecision::Reject(reject, CertificateRejectDetail::None, LegalityAuthoritySource::GuardPlane, false), message);
		}
	};
	class VmxDmaDescriptorValidationEvidence;
	class DmaStreamComputeDescriptor final {
	public:
		static constexpr unsigned int CanonicalLane6DeviceId = 6;
		DmaStreamComputeDescriptorReference DescriptorReference{};
		unsigned short AbiVersion = 0, HeaderSize = 0;
		unsigned int TotalSize = 0;
		unsigned long long DescriptorIdentityHash = 0, CertificateInputHash = 0;
		DmaStreamComputeOperationKind Operation{};
		DmaStreamComputeElementType ElementType{};
		DmaStreamComputeShapeKind Shape{};
		DmaStreamComputeRangeEncoding RangeEncoding{};
		DmaStreamComputePartialCompletionPolicy PartialCompletionPolicy{};
		std::shared_ptr<DmaStreamComputeOwnerBinding> OwnerBinding;
		std::shared_ptr<DmaStreamComputeOwnerGuardDecision> OwnerGuardDecision;
		std::vector<DmaStreamComputeMemoryRange> ReadMemoryRanges, NormalizedReadMemoryRanges, WriteMemoryRanges, NormalizedWriteMemoryRanges;
		DmaStreamComputeAliasPolicy AliasPolicy{};
		unsigned long long NormalizedFootprintHash = 0;
		std::shared_ptr<VmxDmaDescriptorValidationEvidence> VmxValidationEvidence;
	};
}
