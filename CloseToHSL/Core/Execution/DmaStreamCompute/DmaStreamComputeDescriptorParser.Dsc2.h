#pragma once

#include "DmaStreamComputeDescriptor.h"
#include "DmaStreamComputeValidationResult.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <optional>
#include <format>
#include <stdexcept>
#include <string>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	enum class DmaStreamComputeDsc2ParserStatus : int
	{
		ParserOnly = 1,
		ExecutionDisabled = 2,
		ExecutionEligibleAfterFutureGates = 3
	};

	enum class DmaStreamComputeDsc2ExecutionState : unsigned char
	{
		ParserOnlyExecutionDisabled = 0
	};

	enum class DmaStreamComputeDsc2AddressSpaceKind : int
	{
		Physical = 0,
		IommuTranslated = 1,
		ReservedFuture = 2
	};

	enum class DmaStreamComputeDsc2AccessKind : int
	{
		Read = 1,
		Write = 2,
		ReadWrite = 3
	};

	enum class DmaStreamComputeDsc2CapabilityId : int
	{
		None = 0,
		CapabilityProfile = 1,
		AddressSpace = 2,
		StridedRange = 3,
		Tile2D = 4,
		ScatterGather = 5,
		FootprintSummary = 6,
		PartialCompletionPolicy = 7,
		NonSemanticMetadata = 8
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum DmaStreamComputeDsc2CapabilityStage : ushort
	enum class DmaStreamComputeDsc2CapabilityStage : unsigned short
	{
		None = 0,
		ParserKnown = 1 << 0,
		Validation = 1 << 1,
		FootprintNormalization = 1 << 2,
		BackendAddressSpace = 1 << 3,
		Execution = 1 << 4,
		CompilerLowering = 1 << 5
	};

	constexpr DmaStreamComputeDsc2CapabilityStage operator|(
		DmaStreamComputeDsc2CapabilityStage left,
		DmaStreamComputeDsc2CapabilityStage right) noexcept
	{
		return static_cast<DmaStreamComputeDsc2CapabilityStage>(
			static_cast<unsigned short>(left) | static_cast<unsigned short>(right));
	}

	constexpr DmaStreamComputeDsc2CapabilityStage operator&(
		DmaStreamComputeDsc2CapabilityStage left,
		DmaStreamComputeDsc2CapabilityStage right) noexcept
	{
		return static_cast<DmaStreamComputeDsc2CapabilityStage>(
			static_cast<unsigned short>(left) & static_cast<unsigned short>(right));
	}

	constexpr DmaStreamComputeDsc2CapabilityStage& operator|=(
		DmaStreamComputeDsc2CapabilityStage& left,
		DmaStreamComputeDsc2CapabilityStage right) noexcept
	{
		left = left | right;
		return left;
	}

	enum class DmaStreamComputeDsc2ExtensionType : int
	{
		CapabilityProfile = 1,
		AddressSpace = 2,
		StridedRange = 3,
		Tile2D = 4,
		ScatterGather = 5,
		FootprintSummary = 6,
		PartialCompletionPolicy = 7,
		VendorNonSemanticMetadata = 0x7FFF
	};

// C# TO C++ CONVERTER NOTE: The following .NET attribute has no direct equivalent in C++:
// ORIGINAL LINE: [Flags] public enum DmaStreamComputeDsc2ExtensionFlags : ushort
	enum class DmaStreamComputeDsc2ExtensionFlags : unsigned short
	{
		None = 0,
		Required = 1 << 0,
		Semantic = 1 << 1,
		NonSemantic = 1 << 2
	};

	constexpr DmaStreamComputeDsc2ExtensionFlags operator&(
		DmaStreamComputeDsc2ExtensionFlags left,
		DmaStreamComputeDsc2ExtensionFlags right) noexcept
	{
		return static_cast<DmaStreamComputeDsc2ExtensionFlags>(
			static_cast<unsigned short>(left) & static_cast<unsigned short>(right));
	}

	enum class DmaStreamComputeDsc2FootprintOutcomeKind : unsigned char
	{
		None = 0,
		Exact = 1,
		Conservative = 2,
		Rejected = 3
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DmaStreamComputeDsc2CapabilityGrant
//	{
//		public DmaStreamComputeDsc2CapabilityGrant(DmaStreamComputeDsc2CapabilityId capabilityId, DmaStreamComputeDsc2CapabilityStage stages)
//		{
//			CapabilityId = capabilityId;
//			Stages = stages;
//		}
//
//		public DmaStreamComputeDsc2CapabilityId CapabilityId
//		{
//			get;
//		}
//
//		public DmaStreamComputeDsc2CapabilityStage Stages
//		{
//			get;
//		}
//	}

	struct DmaStreamComputeDsc2CapabilityGrant final
	{
		const DmaStreamComputeDsc2CapabilityId CapabilityId;
		const DmaStreamComputeDsc2CapabilityStage Stages;

		DmaStreamComputeDsc2CapabilityGrant(
			DmaStreamComputeDsc2CapabilityId capabilityId,
			DmaStreamComputeDsc2CapabilityStage stages) noexcept
			: CapabilityId(capabilityId), Stages(stages) {}

		bool operator==(const DmaStreamComputeDsc2CapabilityGrant& other) const noexcept
		{
			return CapabilityId == other.CapabilityId && Stages == other.Stages;
		}
	};

	class DmaStreamComputeDsc2CapabilitySet final : public std::enable_shared_from_this<DmaStreamComputeDsc2CapabilitySet>
	{
	private:
		static const std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> Empty;
		static const std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> ParserOnlyFoundation;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly Dictionary<DmaStreamComputeDsc2CapabilityId, DmaStreamComputeDsc2CapabilityStage> _grants;
		std::unordered_map<DmaStreamComputeDsc2CapabilityId, DmaStreamComputeDsc2CapabilityStage> _grants;
		std::vector<DmaStreamComputeDsc2CapabilityId> _grantOrder;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private DmaStreamComputeDsc2CapabilitySet(IReadOnlyList<DmaStreamComputeDsc2CapabilityGrant> grants)
		DmaStreamComputeDsc2CapabilitySet(const std::vector<DmaStreamComputeDsc2CapabilityGrant> &grants);

	public:
		static std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> getEmpty();

		static std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> getParserOnlyFoundation();

		std::vector<DmaStreamComputeDsc2CapabilityGrant> getGrants() const;

		bool getGrantsExecution() const;

		bool getGrantsCompilerLowering() const;

		static std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> Create(std::vector<DmaStreamComputeDsc2CapabilityGrant> grants);

		bool Has(DmaStreamComputeDsc2CapabilityId capabilityId, DmaStreamComputeDsc2CapabilityStage requiredStages);

	private:
		bool HasAnyStage(DmaStreamComputeDsc2CapabilityStage stage) const;
	};

	struct DmaStreamComputeDsc2ParsedExtension final
	{
		const DmaStreamComputeDsc2ExtensionType ExtensionType;
		const unsigned short Version;
		const DmaStreamComputeDsc2ExtensionFlags Flags;
		const unsigned int ByteLength;
		const DmaStreamComputeDsc2CapabilityId CapabilityId;
		const DmaStreamComputeDsc2FootprintOutcomeKind FootprintOutcome;

		DmaStreamComputeDsc2ParsedExtension(DmaStreamComputeDsc2ExtensionType extensionType,
			unsigned short version, DmaStreamComputeDsc2ExtensionFlags flags, unsigned int byteLength,
			DmaStreamComputeDsc2CapabilityId capabilityId, DmaStreamComputeDsc2FootprintOutcomeKind footprintOutcome) noexcept
			: ExtensionType(extensionType), Version(version), Flags(flags), ByteLength(byteLength),
			  CapabilityId(capabilityId), FootprintOutcome(footprintOutcome) {}
	};

	class DmaStreamComputeDsc2FootprintModel final
	{
	private:
		static std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> FreezeRanges(
			const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>>& ranges)
		{
			std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> copy;
			copy.reserve(ranges.size());
			for (const auto& range : ranges)
			{
				if (!range)
					throw std::invalid_argument("DSC2 footprint ranges cannot contain null entries.");
				copy.push_back(std::make_shared<DmaStreamComputeMemoryRange>(*range));
			}
			return copy;
		}

	public:
		const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> ReadRanges;
		const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> WriteRanges;
		const DmaStreamComputeDsc2FootprintOutcomeKind OutcomeKind;
		const unsigned long long NormalizedFootprintHash;

		DmaStreamComputeDsc2FootprintModel(
			const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>>& readRanges,
			const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>>& writeRanges,
			DmaStreamComputeDsc2FootprintOutcomeKind outcomeKind,
			unsigned long long normalizedFootprintHash)
			: ReadRanges(FreezeRanges(readRanges)), WriteRanges(FreezeRanges(writeRanges)),
			  OutcomeKind(outcomeKind), NormalizedFootprintHash(normalizedFootprintHash) {}

		bool getIsExact() const noexcept { return OutcomeKind == DmaStreamComputeDsc2FootprintOutcomeKind::Exact; }
		bool getIsConservative() const noexcept { return OutcomeKind == DmaStreamComputeDsc2FootprintOutcomeKind::Conservative; }
	};

	struct DmaStreamComputeDsc2Descriptor final
	{
		const DmaStreamComputeDescriptorReference DescriptorReference;
		const unsigned short MajorVersion;
		const unsigned short MinorVersion;
		const unsigned short HeaderSize;
		const unsigned int TotalSize;
		const unsigned long long DescriptorIdentityHash;
		const unsigned long long CapabilitySetHash;
		const std::shared_ptr<DmaStreamComputeOwnerBinding> OwnerBinding;
		const std::shared_ptr<DmaStreamComputeOwnerGuardDecision> OwnerGuardDecision;
		const DmaStreamComputeDsc2ParserStatus ParserStatus;
		const DmaStreamComputeDsc2ExecutionState ExecutionState;
		const DmaStreamComputeDsc2AddressSpaceKind AddressSpace;
		const unsigned long long MappingEpoch;
		const std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> CapabilitySet;
		const std::vector<std::shared_ptr<DmaStreamComputeDsc2ParsedExtension>> Extensions;
		const std::shared_ptr<DmaStreamComputeDsc2FootprintModel> NormalizedFootprint;

		bool getIsParserOnly() const noexcept { return true; }
		bool getExecutionEnabled() const noexcept { return false; }
		bool getCanIssueToken() const noexcept { return false; }
		bool getCanPublishMemory() const noexcept { return false; }
		bool getCanProductionLower() const noexcept { return false; }
	};

	class DmaStreamComputeDsc2ValidationResult final
	{
	private:
		DmaStreamComputeDsc2ValidationResult(DmaStreamComputeValidationFault fault,
			std::optional<std::shared_ptr<DmaStreamComputeDsc2Descriptor>> descriptor,
			std::wstring message)
			: Fault(fault), Descriptor(std::move(descriptor)),
			  Message(message.empty() ? std::format(L"DmaStreamCompute DSC2 parser-only validation result: {}.", static_cast<int>(fault)) : std::move(message)) {}

	public:
		const DmaStreamComputeValidationFault Fault;
		const std::optional<std::shared_ptr<DmaStreamComputeDsc2Descriptor>> Descriptor;
		const std::wstring Message;

		bool getIsParserAccepted() const noexcept
		{
			return Fault == DmaStreamComputeValidationFault::None && Descriptor.has_value() && Descriptor.value() != nullptr;
		}
		bool getExecutionEnabled() const noexcept { return false; }
		bool getCanIssueToken() const noexcept { return false; }
		bool getCanPublishMemory() const noexcept { return false; }
		bool getCanProductionLower() const noexcept { return false; }

		static std::shared_ptr<DmaStreamComputeDsc2ValidationResult> Accepted(
			std::shared_ptr<DmaStreamComputeDsc2Descriptor> descriptor)
		{
			if (!descriptor)
				throw std::invalid_argument("descriptor");
			return std::shared_ptr<DmaStreamComputeDsc2ValidationResult>(new DmaStreamComputeDsc2ValidationResult(
				DmaStreamComputeValidationFault::None, std::move(descriptor),
				L"DSC2 descriptor accepted for parser-only validation; execution, token issue, memory publication, and compiler lowering remain disabled."));
		}

		static std::shared_ptr<DmaStreamComputeDsc2ValidationResult> Fail(
			DmaStreamComputeValidationFault fault, std::wstring message)
		{
			if (fault == DmaStreamComputeValidationFault::None)
				throw std::invalid_argument("Use Accepted for successful DSC2 parser-only validation.");
			return std::shared_ptr<DmaStreamComputeDsc2ValidationResult>(new DmaStreamComputeDsc2ValidationResult(
				fault, std::nullopt, std::move(message)));
		}

		std::shared_ptr<DmaStreamComputeDsc2Descriptor> RequireParserOnlyDescriptor() const
		{
			if (getIsParserAccepted())
				return Descriptor.value();
			std::string narrowMessage;
			narrowMessage.reserve(Message.size());
			for (const wchar_t character : Message)
			{
				narrowMessage.push_back(character >= 0 && character <= 0x7f ? static_cast<char>(character) : '?');
			}
			throw std::runtime_error(std::format("DmaStreamCompute DSC2 descriptor is not parser-accepted: {}. {}",
				static_cast<int>(Fault), narrowMessage));
		}
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeDsc2ParsedExtension
//	{
//		public required DmaStreamComputeDsc2ExtensionType ExtensionType
//		{
//			get;
//			set;
//		}
//
//		public required ushort Version
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeDsc2ExtensionFlags Flags
//		{
//			get;
//			set;
//		}
//
//		public required uint ByteLength
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeDsc2CapabilityId CapabilityId
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeDsc2FootprintOutcomeKind FootprintOutcome
//		{
//			get;
//			set;
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeDsc2FootprintModel
//	{
//		public DmaStreamComputeDsc2FootprintModel(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges, DmaStreamComputeDsc2FootprintOutcomeKind outcomeKind, ulong normalizedFootprintHash)
//		{
//			ReadRanges = FreezeRanges(readRanges);
//			WriteRanges = FreezeRanges(writeRanges);
//			OutcomeKind = outcomeKind;
//			NormalizedFootprintHash = normalizedFootprintHash;
//		}
//
//		public IReadOnlyList<DmaStreamComputeMemoryRange> ReadRanges
//		{
//			get;
//		}
//
//		public IReadOnlyList<DmaStreamComputeMemoryRange> WriteRanges
//		{
//			get;
//		}
//
//		public DmaStreamComputeDsc2FootprintOutcomeKind OutcomeKind
//		{
//			get;
//		}
//
//		public ulong NormalizedFootprintHash
//		{
//			get;
//		}
//
//		public bool IsExact
//		{
//			get
//			{
//				return OutcomeKind == DmaStreamComputeDsc2FootprintOutcomeKind.Exact;
//			}
//		}
//
//		public bool IsConservative
//		{
//			get
//			{
//				return OutcomeKind == DmaStreamComputeDsc2FootprintOutcomeKind.Conservative;
//			}
//		}
//
//		private static IReadOnlyList<DmaStreamComputeMemoryRange> FreezeRanges(System.Nullable<IReadOnlyList<DmaStreamComputeMemoryRange>> ranges)
//		{
//			if (ranges == nullptr || ranges.Count == 0)
//			{
//				return Array.Empty<DmaStreamComputeMemoryRange>();
//			}
//
//			DmaStreamComputeMemoryRange[] copy = new DmaStreamComputeMemoryRange[ranges.Count];
//			for (int index = 0; index < ranges.Count; index++)
//			{
//				copy[index] = ranges[index];
//			}
//
//			return Array.AsReadOnly(copy);
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeDsc2Descriptor
//	{
//		public required DmaStreamComputeDescriptorReference DescriptorReference
//		{
//			get;
//			set;
//		}
//
//		public required ushort MajorVersion
//		{
//			get;
//			set;
//		}
//
//		public required ushort MinorVersion
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
//		public required ulong CapabilitySetHash
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
//		public required DmaStreamComputeDsc2ParserStatus ParserStatus
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeDsc2ExecutionState ExecutionState
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeDsc2AddressSpaceKind AddressSpace
//		{
//			get;
//			set;
//		}
//
//		public required ulong MappingEpoch
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeDsc2CapabilitySet CapabilitySet
//		{
//			get;
//			set;
//		}
//
//		public required IReadOnlyList<DmaStreamComputeDsc2ParsedExtension> Extensions
//		{
//			get;
//			set;
//		}
//
//		public required DmaStreamComputeDsc2FootprintModel NormalizedFootprint
//		{
//			get;
//			set;
//		}
//
//		public bool IsParserOnly
//		{
//			get
//			{
//				return true;
//			}
//		}
//
//		public bool ExecutionEnabled
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanIssueToken
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanPublishMemory
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanProductionLower
//		{
//			get
//			{
//				return false;
//			}
//		}
//	}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DmaStreamComputeDsc2ValidationResult
//	{
//		private DmaStreamComputeDsc2ValidationResult(DmaStreamComputeValidationFault fault, System.Nullable<DmaStreamComputeDsc2Descriptor> descriptor, string message)
//		{
//			Fault = fault;
//			Descriptor = descriptor;
//			Message = string.IsNullOrWhiteSpace(message) ? string.Format("DmaStreamCompute DSC2 parser-only validation result: {0}.", fault) : message;
//		}
//
//		public bool IsParserAccepted
//		{
//			get
//			{
//				return Fault == DmaStreamComputeValidationFault.None && Descriptor != nullptr;
//			}
//		}
//
//		public DmaStreamComputeValidationFault Fault
//		{
//			get;
//		}
//
//		public System.Nullable<DmaStreamComputeDsc2Descriptor> Descriptor
//		{
//			get;
//		}
//
//		public string Message
//		{
//			get;
//		}
//
//		public bool ExecutionEnabled
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanIssueToken
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanPublishMemory
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public bool CanProductionLower
//		{
//			get
//			{
//				return false;
//			}
//		}
//
//		public static DmaStreamComputeDsc2ValidationResult Accepted(DmaStreamComputeDsc2Descriptor descriptor)
//		{
//			ArgumentNullException.ThrowIfNull(descriptor);
//			return new DmaStreamComputeDsc2ValidationResult(DmaStreamComputeValidationFault.None, descriptor, "DSC2 descriptor accepted for parser-only validation; execution, token issue, memory publication, and compiler lowering remain disabled.");
//		}
//
//		public static DmaStreamComputeDsc2ValidationResult Fail(DmaStreamComputeValidationFault fault, string message)
//		{
//			if (fault == DmaStreamComputeValidationFault.None)
//			{
//				throw new ArgumentException("Use Accepted for successful DSC2 parser-only validation.", nameof(fault));
//			}
//
//			return new DmaStreamComputeDsc2ValidationResult(fault, nullptr, message);
//		}
//
//		public DmaStreamComputeDsc2Descriptor RequireParserOnlyDescriptor()
//		{
//			if (IsParserAccepted && Descriptor != nullptr)
//			{
//				return Descriptor;
//			}
//
//			throw new InvalidOperationException(string.Format("DmaStreamCompute DSC2 descriptor is not parser-accepted: {0}. {1}", Fault, Message));
//		}
//	}

}
