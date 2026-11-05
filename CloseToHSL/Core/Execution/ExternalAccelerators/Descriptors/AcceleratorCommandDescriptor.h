#pragma once

#include <memory>
#include <vector>
#include <stdexcept>
#include <string>
#include <codecvt>
#include <locale>
#include <cwctype>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth { class AcceleratorGuardDecision; }

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{

	enum class AcceleratorClassId : int
	{
		Matrix = 1,
		Tensor = 2,
		TopologyQueue = 3,
		Fft = 4,
		CryptoHash = 5,
		SparseGraph = 6
	};

	enum class AcceleratorDeviceId : int
	{
		ReferenceMatMul = 1,
		TensorMetadata = 2,
		TopologyQueueMetadata = 3,
		FftMetadata = 4,
		CryptoHashMetadata = 5,
		SparseGraphMetadata = 6
	};

	enum class AcceleratorOperationKind : int
	{
		MatMul = 1,
		TensorContract = 2,
		TopologyQueueContract = 3,
		FftContract = 4,
		CryptoHashContract = 5,
		SparseGraphContract = 6
	};

	enum class AcceleratorDatatype : int
	{
		Float32 = 1,
		Float64 = 2,
		Int32 = 3
	};

	enum class AcceleratorShapeKind : int
	{
		Matrix2D = 1,
		TensorND = 2,
		TopologyQueueMap = 3,
		Fft1D = 4,
		CryptoHashBlock = 5,
		SparseGraphCsr = 6
	};

	enum class AcceleratorPartialCompletionPolicy : int
	{
		AllOrNone = 1
	};

	enum class AcceleratorDescriptorFault
	{
		None = 0,
		DescriptorDecodeFault = 1,
		UnsupportedAbiVersion = 2,
		DescriptorSizeFault = 3,
		ReservedFieldFault = 4,
		UnsupportedAcceleratorClass = 5,
		UnsupportedAcceleratorId = 6,
		UnsupportedOperation = 7,
		UnsupportedDatatype = 8,
		UnsupportedShape = 9,
		UnsupportedPartialCompletionPolicy = 10,
		RangeOverflow = 11,
		AlignmentFault = 12,
		ZeroLengthFault = 13,
		DescriptorReferenceMismatch = 14,
		DescriptorIdentityHashMismatch = 15,
		NormalizedFootprintHashMismatch = 16,
		DescriptorCarrierDecodeFault = 17,
		OwnerDomainFault = 18,
		AliasAmbiguousFootprint = 19
	};

	inline const wchar_t* AcceleratorDescriptorFaultName(AcceleratorDescriptorFault fault)
	{
		switch (fault)
		{
		case AcceleratorDescriptorFault::None: return L"None";
		case AcceleratorDescriptorFault::DescriptorDecodeFault: return L"DescriptorDecodeFault";
		case AcceleratorDescriptorFault::UnsupportedAbiVersion: return L"UnsupportedAbiVersion";
		case AcceleratorDescriptorFault::DescriptorSizeFault: return L"DescriptorSizeFault";
		case AcceleratorDescriptorFault::ReservedFieldFault: return L"ReservedFieldFault";
		case AcceleratorDescriptorFault::UnsupportedAcceleratorClass: return L"UnsupportedAcceleratorClass";
		case AcceleratorDescriptorFault::UnsupportedAcceleratorId: return L"UnsupportedAcceleratorId";
		case AcceleratorDescriptorFault::UnsupportedOperation: return L"UnsupportedOperation";
		case AcceleratorDescriptorFault::UnsupportedDatatype: return L"UnsupportedDatatype";
		case AcceleratorDescriptorFault::UnsupportedShape: return L"UnsupportedShape";
		case AcceleratorDescriptorFault::UnsupportedPartialCompletionPolicy: return L"UnsupportedPartialCompletionPolicy";
		case AcceleratorDescriptorFault::RangeOverflow: return L"RangeOverflow";
		case AcceleratorDescriptorFault::AlignmentFault: return L"AlignmentFault";
		case AcceleratorDescriptorFault::ZeroLengthFault: return L"ZeroLengthFault";
		case AcceleratorDescriptorFault::DescriptorReferenceMismatch: return L"DescriptorReferenceMismatch";
		case AcceleratorDescriptorFault::DescriptorIdentityHashMismatch: return L"DescriptorIdentityHashMismatch";
		case AcceleratorDescriptorFault::NormalizedFootprintHashMismatch: return L"NormalizedFootprintHashMismatch";
		case AcceleratorDescriptorFault::DescriptorCarrierDecodeFault: return L"DescriptorCarrierDecodeFault";
		case AcceleratorDescriptorFault::OwnerDomainFault: return L"OwnerDomainFault";
		case AcceleratorDescriptorFault::AliasAmbiguousFootprint: return L"AliasAmbiguousFootprint";
		default: return L"Unknown";
		}
	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorDescriptorReference(ulong DescriptorAddress, uint DescriptorSize, ulong DescriptorIdentityHash);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorMemoryRange(ulong Address, ulong Length);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorAlignmentRequirement(ushort MinimumAlignmentBytes);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorScratchRequirement(ulong RequiredBytes, IReadOnlyList<AcceleratorMemoryRange> Ranges);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorDescriptorIdentity(ulong DescriptorIdentityHash, ulong NormalizedFootprintHash);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorOwnerBinding
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
//		public required ulong DomainTag
//		{
//			get;
//			set;
//		}
//	}

	struct AcceleratorDescriptorReference final { unsigned long long DescriptorAddress = 0; unsigned int DescriptorSize = 0; unsigned long long DescriptorIdentityHash = 0; bool operator==(const AcceleratorDescriptorReference&) const = default; };
	struct AcceleratorMemoryRange final { unsigned long long Address = 0, Length = 0; bool operator==(const AcceleratorMemoryRange&) const = default; };
	struct AcceleratorAlignmentRequirement final { unsigned short MinimumAlignmentBytes = 0; bool operator==(const AcceleratorAlignmentRequirement&) const = default; };
	struct AcceleratorScratchRequirement final { unsigned long long RequiredBytes = 0; std::vector<std::shared_ptr<AcceleratorMemoryRange>> Ranges; };
	struct AcceleratorDescriptorIdentity final { unsigned long long DescriptorIdentityHash = 0, NormalizedFootprintHash = 0; bool operator==(const AcceleratorDescriptorIdentity&) const = default; };
	struct AcceleratorOwnerBinding final {
		unsigned short OwnerVirtualThreadId = 0;
		unsigned int OwnerContextId = 0, OwnerCoreId = 0, OwnerPodId = 0;
		unsigned long long DomainTag = 0;
		bool operator==(const AcceleratorOwnerBinding&) const = default;
		bool Equals(const std::shared_ptr<AcceleratorOwnerBinding>& other) const { return other && *this == *other; }
	};
	struct AcceleratorNormalizedFootprint final {
		std::vector<std::shared_ptr<AcceleratorMemoryRange>> SourceRanges, DestinationRanges, ScratchRanges;
		unsigned long long Hash = 0;
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorNormalizedFootprint
//	{
//		public required IReadOnlyList<AcceleratorMemoryRange> SourceRanges
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<AcceleratorMemoryRange> DestinationRanges
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<AcceleratorMemoryRange> ScratchRanges
//		{
//			get;
//			set;
//		}
//
//		public required ulong Hash
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AcceleratorDescriptorHeader(uint Magic, ushort AbiVersion, ushort HeaderSize, uint DescriptorSize, AcceleratorClassId AcceleratorClass, AcceleratorDeviceId AcceleratorId, AcceleratorOperationKind Operation, AcceleratorDatatype Datatype, AcceleratorShapeKind Shape, ushort ShapeRank, ushort SourceRangeCount, ushort DestinationRangeCount, ushort ScratchRangeCount, AcceleratorPartialCompletionPolicy PartialCompletionPolicy, AcceleratorAlignmentRequirement Alignment, ulong ElementCount, uint CapabilityVersion, AcceleratorDescriptorIdentity Identity, AcceleratorOwnerBinding OwnerBinding, ulong ScratchRequiredBytes);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
	struct AcceleratorDescriptorHeader final {
		unsigned int Magic = 0;
		unsigned short AbiVersion = 0, HeaderSize = 0;
		unsigned int DescriptorSize = 0;
		AcceleratorClassId AcceleratorClass{};
		AcceleratorDeviceId AcceleratorId{};
		AcceleratorOperationKind Operation{};
		AcceleratorDatatype Datatype{};
		AcceleratorShapeKind Shape{};
		unsigned short ShapeRank = 0, SourceRangeCount = 0, DestinationRangeCount = 0, ScratchRangeCount = 0;
		AcceleratorPartialCompletionPolicy PartialCompletionPolicy{};
		AcceleratorAlignmentRequirement Alignment{};
		unsigned long long ElementCount = 0;
		unsigned int CapabilityVersion = 0;
		AcceleratorDescriptorIdentity Identity{};
		std::shared_ptr<AcceleratorOwnerBinding> OwnerBinding;
		unsigned long long ScratchRequiredBytes = 0;
	};
	class AcceleratorCommandDescriptor final {
	public:
		AcceleratorDescriptorReference DescriptorReference{};
		AcceleratorDescriptorHeader Header{};
		unsigned short AbiVersion = 0, HeaderSize = 0;
		unsigned int DescriptorSize = 0;
		AcceleratorClassId AcceleratorClass{};
		AcceleratorDeviceId AcceleratorId{};
		AcceleratorOperationKind Operation{};
		AcceleratorDatatype Datatype{};
		AcceleratorShapeKind Shape{};
		unsigned short ShapeRank = 0;
		unsigned long long ElementCount = 0;
		unsigned int CapabilityVersion = 0;
		AcceleratorAlignmentRequirement Alignment{};
		AcceleratorScratchRequirement ScratchRequirement{};
		AcceleratorPartialCompletionPolicy PartialCompletionPolicy{};
		std::shared_ptr<AcceleratorOwnerBinding> OwnerBinding;
		std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth::AcceleratorGuardDecision> OwnerGuardDecision;
		AcceleratorDescriptorIdentity Identity{};
		std::vector<std::shared_ptr<AcceleratorMemoryRange>> SourceRanges, DestinationRanges, ScratchRanges;
		AcceleratorNormalizedFootprint NormalizedFootprint{};
		bool Equals(const std::shared_ptr<AcceleratorCommandDescriptor>& other) const;
		std::shared_ptr<AcceleratorCommandDescriptor> Freeze() const {
			auto copy = std::make_shared<AcceleratorCommandDescriptor>(*this);
			auto clone = [](const auto& source) {
				std::vector<std::shared_ptr<AcceleratorMemoryRange>> result;
				result.reserve(source.size());
				for (const auto& range : source) result.push_back(range ? std::make_shared<AcceleratorMemoryRange>(*range) : nullptr);
				return result;
			};
			copy->SourceRanges = clone(SourceRanges); copy->DestinationRanges = clone(DestinationRanges); copy->ScratchRanges = clone(ScratchRanges);
			copy->ScratchRequirement.Ranges = clone(ScratchRequirement.Ranges);
			copy->NormalizedFootprint.SourceRanges = clone(NormalizedFootprint.SourceRanges);
			copy->NormalizedFootprint.DestinationRanges = clone(NormalizedFootprint.DestinationRanges);
			copy->NormalizedFootprint.ScratchRanges = clone(NormalizedFootprint.ScratchRanges);
			return copy;
		}
	};
//	public sealed record AcceleratorCommandDescriptor
//	{
//		public required AcceleratorDescriptorReference DescriptorReference
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorDescriptorHeader Header
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
//		public required uint DescriptorSize
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorClassId AcceleratorClass
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorDeviceId AcceleratorId
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorOperationKind Operation
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorDatatype Datatype
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorShapeKind Shape
//		{
//			get;
//			set;
//		}
//
//		public required ushort ShapeRank
//		{
//			get;
//			set;
//		}
//
//		public required ulong ElementCount
//		{
//			get;
//			set;
//		}
//
//		public required uint CapabilityVersion
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorAlignmentRequirement Alignment
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorScratchRequirement ScratchRequirement
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorPartialCompletionPolicy PartialCompletionPolicy
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorOwnerBinding OwnerBinding
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorGuardDecision OwnerGuardDecision
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorDescriptorIdentity Identity
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<AcceleratorMemoryRange> SourceRanges
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<AcceleratorMemoryRange> DestinationRanges
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<AcceleratorMemoryRange> ScratchRanges
//		{
//			get;
//			set;
//		}
//
//		public required AcceleratorNormalizedFootprint NormalizedFootprint
//		{
//			get;
//			set;
//		}
//
//		public AcceleratorCommandDescriptor Freeze()
//		{
//			IReadOnlyList<AcceleratorMemoryRange> sourceRanges = FreezeRanges(SourceRanges);
//			IReadOnlyList<AcceleratorMemoryRange> destinationRanges = FreezeRanges(DestinationRanges);
//			IReadOnlyList<AcceleratorMemoryRange> scratchRanges = FreezeRanges(ScratchRanges);
//			IReadOnlyList<AcceleratorMemoryRange> scratchRequirementRanges = FreezeRanges(ScratchRequirement.Ranges);
//			IReadOnlyList<AcceleratorMemoryRange> normalizedSourceRanges = FreezeRanges(NormalizedFootprint.SourceRanges);
//			IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationRanges = FreezeRanges(NormalizedFootprint.DestinationRanges);
//			IReadOnlyList<AcceleratorMemoryRange> normalizedScratchRanges = FreezeRanges(NormalizedFootprint.ScratchRanges);
//
//			return this with { SourceRanges = sourceRanges, DestinationRanges = destinationRanges, ScratchRanges = scratchRanges, ScratchRequirement = ScratchRequirement with { Ranges = scratchRequirementRanges }, NormalizedFootprint = NormalizedFootprint with { SourceRanges = normalizedSourceRanges, DestinationRanges = normalizedDestinationRanges, ScratchRanges = normalizedScratchRanges } };
//		}
//
//		private static IReadOnlyList<AcceleratorMemoryRange> FreezeRanges(IReadOnlyList<AcceleratorMemoryRange> ranges)
//		{
//			if (ranges == nullptr || ranges.Count == 0)
//			{
//				return Array.Empty<AcceleratorMemoryRange>();
//			}
//
//			AcceleratorMemoryRange[] copy = new AcceleratorMemoryRange[ranges.Count];
//			for (int index = 0; index < ranges.Count; index++)
//			{
//				copy[index] = ranges[index];
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
	class AcceleratorDescriptorValidationResult final {
	public:
		const AcceleratorDescriptorFault Fault;
		const std::shared_ptr<AcceleratorCommandDescriptor> Descriptor;
		const std::wstring Message;
		const bool IsValid, IsDescriptorAbiAccepted;
	private:
		AcceleratorDescriptorValidationResult(AcceleratorDescriptorFault fault, std::shared_ptr<AcceleratorCommandDescriptor> descriptor, std::wstring message)
			: Fault(fault), Descriptor(std::move(descriptor)), Message(std::move(message)), IsValid(fault == AcceleratorDescriptorFault::None && Descriptor != nullptr), IsDescriptorAbiAccepted(IsValid) {}
	public:
		static std::shared_ptr<AcceleratorDescriptorValidationResult> Valid(const std::shared_ptr<AcceleratorCommandDescriptor>& descriptor) {
			if (!descriptor) throw std::invalid_argument("descriptor");
			return std::shared_ptr<AcceleratorDescriptorValidationResult>(new AcceleratorDescriptorValidationResult(AcceleratorDescriptorFault::None, descriptor->Freeze(), L"L7-SDC descriptor accepted as guard-backed typed sideband; execution remains fail-closed."));
		}
		static std::shared_ptr<AcceleratorDescriptorValidationResult> Fail(AcceleratorDescriptorFault fault, const std::wstring& message) {
			if (fault == AcceleratorDescriptorFault::None) throw std::invalid_argument("Use Valid for successful descriptor validation.");
			bool hasText = false;
			for (wchar_t ch : message) if (!std::iswspace(ch)) { hasText = true; break; }
			if (!hasText) throw std::invalid_argument("Validation failure message is required.");
			return std::shared_ptr<AcceleratorDescriptorValidationResult>(new AcceleratorDescriptorValidationResult(fault, nullptr, message));
		}
		std::shared_ptr<AcceleratorCommandDescriptor> RequireDescriptor() const {
			if (IsDescriptorAbiAccepted && Descriptor) return Descriptor;
			const std::wstring message = std::wstring(L"L7-SDC descriptor is not valid: ") + AcceleratorDescriptorFaultName(Fault) + L". " + Message;
			throw std::logic_error(std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(message));
		}
	};
//	public sealed record AcceleratorDescriptorValidationResult
//	{
//		private AcceleratorDescriptorValidationResult(AcceleratorDescriptorFault fault, System.Nullable<AcceleratorCommandDescriptor> descriptor, string message)
//		{
//			Fault = fault;
//			Descriptor = descriptor;
//			Message = message;
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return Fault == AcceleratorDescriptorFault.None && Descriptor != nullptr;
//			}
//		}
//
//		public bool IsDescriptorAbiAccepted
//		{
//			get
//			{
//				return IsValid;
//			}
//		}
//
//		public AcceleratorDescriptorFault Fault
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorCommandDescriptor> Descriptor
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorDescriptorValidationResult Valid(AcceleratorCommandDescriptor descriptor)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			return new AcceleratorDescriptorValidationResult(AcceleratorDescriptorFault.None, descriptor.Freeze(), "L7-SDC descriptor accepted as guard-backed typed sideband; execution remains fail-closed.");
//		}
//
//		public static AcceleratorDescriptorValidationResult Fail(AcceleratorDescriptorFault fault, string message)
//		{
//			if (fault == AcceleratorDescriptorFault.None)
//			{
//				throw new ArgumentException("Use Valid for successful descriptor validation.", nameof(fault));
//			}
//
//			if (string.IsNullOrWhiteSpace(message))
//			{
//				throw new ArgumentException("Validation failure message is required.", nameof(message));
//			}
//
//			return new AcceleratorDescriptorValidationResult(fault, nullptr, message);
//		}
//
//		public AcceleratorCommandDescriptor RequireDescriptor()
//		{
//			if (IsDescriptorAbiAccepted && Descriptor != nullptr)
//			{
//				return Descriptor;
//			}
//
//			throw new InvalidOperationException(string.Format("L7-SDC descriptor is not valid: {0}. {1}", Fault, Message));
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorDescriptorStructuralReadResult
//	{
//		private AcceleratorDescriptorStructuralReadResult(AcceleratorDescriptorFault fault, AcceleratorDescriptorReference descriptorReference, ulong descriptorIdentityHash, uint descriptorSize, System.Nullable<AcceleratorOwnerBinding> ownerBinding, string message)
//		{
//			Fault = fault;
//			DescriptorReference = descriptorReference;
//			DescriptorIdentityHash = descriptorIdentityHash;
//			DescriptorSize = descriptorSize;
//			OwnerBinding = ownerBinding;
//			Message = message;
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return Fault == AcceleratorDescriptorFault.None && OwnerBinding != nullptr;
//			}
//		}
//
//		public AcceleratorDescriptorFault Fault
//		{
//			get;
//		}
//
//		public AcceleratorDescriptorReference DescriptorReference
//		{
//			get;
//		}
//
//		public ulong DescriptorIdentityHash
//		{
//			get;
//		}
//
//		public uint DescriptorSize
//		{
//			get;
//		}
//
//		public System.Nullable<AcceleratorOwnerBinding> OwnerBinding
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorDescriptorStructuralReadResult Valid(AcceleratorDescriptorReference descriptorReference, ulong descriptorIdentityHash, uint descriptorSize, AcceleratorOwnerBinding ownerBinding)
//		{
//			ArgumentNullException.ThrowIfNull(ownerBinding);
//			return new AcceleratorDescriptorStructuralReadResult(AcceleratorDescriptorFault.None, descriptorReference, descriptorIdentityHash, descriptorSize, ownerBinding, "L7-SDC structural owner/domain fields located; descriptor acceptance still requires guard-plane authority.");
//		}
//
//		public static AcceleratorDescriptorStructuralReadResult Fail(AcceleratorDescriptorFault fault, string message)
//		{
//			if (fault == AcceleratorDescriptorFault.None)
//			{
//				throw new ArgumentException("Use Valid for successful structural descriptor reads.", nameof(fault));
//			}
//
//			return new AcceleratorDescriptorStructuralReadResult(fault, default, descriptorIdentityHash: 0, descriptorSize: 0, ownerBinding: nullptr, message);
//		}
//
//		public AcceleratorOwnerBinding RequireOwnerBindingForGuard()
//		{
//			if (IsValid && OwnerBinding != nullptr)
//			{
//				return OwnerBinding;
//			}
//
//			throw new InvalidOperationException(string.Format("L7-SDC structural owner/domain read failed: {0}. {1}", Fault, Message));
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record AcceleratorCarrierValidationResult
//	{
//		private AcceleratorCarrierValidationResult(AcceleratorDescriptorFault fault, string message)
//		{
//			Fault = fault;
//			Message = message;
//		}
//
//		public bool IsValid
//		{
//			get
//			{
//				return Fault == AcceleratorDescriptorFault.None;
//			}
//		}
//
//		public AcceleratorDescriptorFault Fault
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public static AcceleratorCarrierValidationResult Valid()
//		{
//			return new(AcceleratorDescriptorFault.None, "Carrier accepted for typed sideband projection.");
//		}
//
//		public static AcceleratorCarrierValidationResult Fail(AcceleratorDescriptorFault fault, string message)
//		{
//			if (fault == AcceleratorDescriptorFault.None)
//			{
//				throw new ArgumentException("Use Valid for successful carrier validation.", nameof(fault));
//			}
//
//			return new AcceleratorCarrierValidationResult(fault, message);
//		}
//	}

	/// Result of validating the native VLIW carrier for an external accelerator command.
	class AcceleratorCarrierValidationResult final {
	private:
		AcceleratorCarrierValidationResult(AcceleratorDescriptorFault fault, std::wstring message)
			: Fault(fault), Message(std::move(message)), IsValid(fault == AcceleratorDescriptorFault::None) {}
	public:
		const AcceleratorDescriptorFault Fault;
		const std::wstring Message;
		const bool IsValid;
		static std::shared_ptr<AcceleratorCarrierValidationResult> Valid() {
			return std::shared_ptr<AcceleratorCarrierValidationResult>(new AcceleratorCarrierValidationResult(AcceleratorDescriptorFault::None, L"Carrier accepted for typed sideband projection."));
		}
		static std::shared_ptr<AcceleratorCarrierValidationResult> Fail(AcceleratorDescriptorFault fault, const std::wstring& message) {
			if (fault == AcceleratorDescriptorFault::None) throw std::invalid_argument("Use Valid for successful carrier validation.");
			return std::shared_ptr<AcceleratorCarrierValidationResult>(new AcceleratorCarrierValidationResult(fault, message));
		}
	};

}
