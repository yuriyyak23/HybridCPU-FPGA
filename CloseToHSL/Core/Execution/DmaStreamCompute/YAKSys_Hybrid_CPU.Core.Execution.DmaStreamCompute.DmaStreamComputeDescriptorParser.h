#pragma once

#include "DmaStreamComputeTelemetry.h"
#include "../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "DmaStreamComputeValidationResult.h"
#include "DmaStreamComputeDescriptor.h"
#include "DmaStreamComputeDescriptorParser.Dsc2.h"
#include <string>
#include <vector>
#include <limits>
#include <optional>
#include <format>
#include <memory>
#include <stdexcept>
#include <utility>
#include <span>

using namespace HybridCPU_ISE::Arch;


namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	class DmaStreamComputeStructuralReadResult final
	{
		DmaStreamComputeStructuralReadResult(DmaStreamComputeValidationFault fault,
			std::shared_ptr<DmaStreamComputeDescriptorReference> descriptorReference,
			unsigned long long descriptorIdentityHash, unsigned int totalSize,
			std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, std::wstring message)
			: Fault(fault), DescriptorReference(std::move(descriptorReference)),
			  DescriptorIdentityHash(descriptorIdentityHash), TotalSize(totalSize),
			  OwnerBinding(std::move(ownerBinding)), Message(std::move(message)) {}

	public:
		const DmaStreamComputeValidationFault Fault;
		const std::shared_ptr<DmaStreamComputeDescriptorReference> DescriptorReference;
		const unsigned long long DescriptorIdentityHash;
		const unsigned int TotalSize;
		const std::shared_ptr<DmaStreamComputeOwnerBinding> OwnerBinding;
		const std::wstring Message;

		bool getIsValid() const noexcept
		{
			return Fault == DmaStreamComputeValidationFault::None && OwnerBinding != nullptr;
		}
		bool getIsStructuralDescriptorReadAccepted() const noexcept { return getIsValid(); }

		static std::shared_ptr<DmaStreamComputeStructuralReadResult> Valid(
			std::shared_ptr<DmaStreamComputeDescriptorReference> descriptorReference,
			unsigned long long descriptorIdentityHash, unsigned int totalSize,
			std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding)
		{
			if (!ownerBinding) throw std::invalid_argument("ownerBinding");
			return std::shared_ptr<DmaStreamComputeStructuralReadResult>(new DmaStreamComputeStructuralReadResult(
				DmaStreamComputeValidationFault::None, std::move(descriptorReference), descriptorIdentityHash,
				totalSize, std::move(ownerBinding), L"Structural owner fields located; descriptor is not executable."));
		}

		static std::shared_ptr<DmaStreamComputeStructuralReadResult> Fail(
			DmaStreamComputeValidationFault fault, std::wstring message)
		{
			if (fault == DmaStreamComputeValidationFault::None)
				throw std::invalid_argument("Use Valid for successful structural descriptor reads.");
			return std::shared_ptr<DmaStreamComputeStructuralReadResult>(new DmaStreamComputeStructuralReadResult(
				fault, nullptr, 0, 0, nullptr, std::move(message)));
		}

		std::shared_ptr<DmaStreamComputeOwnerBinding> RequireOwnerBindingForGuard() const
		{
			if (getIsStructuralDescriptorReadAccepted()) return OwnerBinding;
			std::string narrowMessage;
			narrowMessage.reserve(Message.size());
			for (const wchar_t character : Message)
			{
				narrowMessage.push_back(character >= 0 && character <= 0x7f ? static_cast<char>(character) : '?');
			}
			throw std::runtime_error(std::format("DmaStreamCompute structural owner read failed: {}. {}",
				static_cast<int>(Fault), narrowMessage));
		}
	};

	class DmaStreamComputeDescriptorParser final
	{
	private:
		class Dsc2FootprintBuilder;
		struct Dsc2ExtensionHeader;
		struct Dsc2FootprintSummary;

	public:
		static constexpr unsigned int Magic = 0x31435344; // "DSC1" as a little-endian scalar.
		static constexpr unsigned short CurrentAbiVersion = 1;
		static constexpr int CurrentHeaderSize = 128;

		static bool getExecutionEnabled();

		static inline const std::wstring ExecutionDisabledProjectionMessage = L"DmaStreamComputeMicroOp execution is enabled only for the Phase 06 DSC1 contour; unsupported descriptor shapes and operations still fail closed without StreamEngine or DMAController fallback.";

	private:
		static constexpr int HeaderSize = CurrentHeaderSize;
		static constexpr int RangeEntrySize = 16;

		static constexpr int MagicOffset = 0;
		static constexpr int AbiVersionOffset = 4;
		static constexpr int HeaderSizeOffset = 6;
		static constexpr int TotalSizeOffset = 8;
		static constexpr int FlagsOffset = 12;
		static constexpr int Reserved0Offset = 16;
		static constexpr int DescriptorIdentityHashOffset = 24;
		static constexpr int CertificateInputHashOffset = 32;
		static constexpr int OperationOffset = 40;
		static constexpr int ElementTypeOffset = 42;
		static constexpr int ShapeOffset = 44;
		static constexpr int RangeEncodingOffset = 46;
		static constexpr int SourceRangeCountOffset = 48;
		static constexpr int DestinationRangeCountOffset = 50;
		static constexpr int MaskRangeCountOffset = 52;
		static constexpr int AccumulatorRangeCountOffset = 54;
		static constexpr int PartialCompletionPolicyOffset = 56;
		static constexpr int RoundingModeOffset = 58;
		static constexpr int OwnerVirtualThreadIdOffset = 60;
		static constexpr int Reserved1Offset = 62;
		static constexpr int OwnerContextIdOffset = 64;
		static constexpr int OwnerCoreIdOffset = 68;
		static constexpr int OwnerPodIdOffset = 72;
		static constexpr int DeviceIdOffset = 76;
		static constexpr int OwnerDomainTagOffset = 80;
		static constexpr int Reserved2Offset = 88;
		static constexpr int SourceRangeTableOffset = 96;
		static constexpr int DestinationRangeTableOffset = 100;
		static constexpr int MaskRangeTableOffset = 104;
		static constexpr int AccumulatorRangeTableOffset = 108;
		static constexpr int Reserved3Offset = 112;
		static constexpr int Reserved4Offset = 120;

	public:
		static std::shared_ptr<DmaStreamComputeStructuralReadResult> ReadStructuralOwnerBinding(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference = std::nullopt);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, System.Nullable<DmaStreamComputeDescriptorReference> descriptorReference = null, DmaStreamComputeTelemetryCounters? telemetry = null)
		static std::shared_ptr<DmaStreamComputeValidationResult> Parse(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference = std::nullopt, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, DmaStreamComputeOwnerGuardDecision ownerGuardDecision, System.Nullable<DmaStreamComputeDescriptorReference> descriptorReference = null, DmaStreamComputeTelemetryCounters? telemetry = null)
		static std::shared_ptr<DmaStreamComputeValidationResult> Parse(std::span<const unsigned char> descriptorBytes, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference = std::nullopt, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry = nullptr);

		static std::shared_ptr<DmaStreamComputeValidationResult> TryDecodeRawVliwCarrier(VLIW_Instruction const instruction, int slotIndex);

		static bool TryValidateNativeVliwCarrier(VLIW_Instruction const instruction, int slotIndex, bool hasDescriptorSideband, std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> &failure);

	private:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static DmaStreamComputeValidationResult Fail(DmaStreamComputeValidationFault fault, string message, DmaStreamComputeTelemetryCounters? telemetry)
		static std::shared_ptr<DmaStreamComputeValidationResult> Fail(DmaStreamComputeValidationFault fault, const std::wstring &message, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static DmaStreamComputeValidationResult Valid(DmaStreamComputeDescriptor descriptor, DmaStreamComputeTelemetryCounters? telemetry)
		static std::shared_ptr<DmaStreamComputeValidationResult> Valid(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry);

		static bool HasAnyReservedBitSet(std::span<const unsigned char> bytes);

		static bool TryDecodeOperation(unsigned short rawOperation, DmaStreamComputeOperationKind &operation);

		static bool TryDecodeElementType(unsigned short rawElementType, DmaStreamComputeElementType &elementType, int &elementSize);

		static bool TryDecodeShape(unsigned short rawShape, DmaStreamComputeOperationKind operation, DmaStreamComputeShapeKind &shape);

		static unsigned short GetExpectedSourceRangeCount(DmaStreamComputeOperationKind operation);

		static bool TryReadRanges(std::span<const unsigned char> bytes, unsigned int totalSize, unsigned int tableOffset, unsigned short rangeCount, int elementSize, std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges, std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> &failure);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<DmaStreamComputeMemoryRange> NormalizeMemoryRanges(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
		static const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> NormalizeMemoryRanges(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryValidateAliasPolicy(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges, out DmaStreamComputeAliasPolicy aliasPolicy, out System.Nullable<DmaStreamComputeValidationResult> failure)
		static bool TryValidateAliasPolicy(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &readRanges, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &writeRanges, DmaStreamComputeAliasPolicy &aliasPolicy, std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> &failure);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool HasAnyInternalOverlap(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
		static bool HasAnyInternalOverlap(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges);

		static bool RangesOverlap(std::shared_ptr<DmaStreamComputeMemoryRange> left, std::shared_ptr<DmaStreamComputeMemoryRange> right);

		static bool RangesEqual(std::shared_ptr<DmaStreamComputeMemoryRange> left, std::shared_ptr<DmaStreamComputeMemoryRange> right);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static ulong ComputeNormalizedFootprintHash(DmaStreamComputeOperationKind operation, DmaStreamComputeElementType elementType, DmaStreamComputeShapeKind shape, DmaStreamComputeAliasPolicy aliasPolicy, IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges)
		static unsigned long long ComputeNormalizedFootprintHash(DmaStreamComputeOperationKind operation, DmaStreamComputeElementType elementType, DmaStreamComputeShapeKind shape, DmaStreamComputeAliasPolicy aliasPolicy, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &readRanges, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &writeRanges);

		static unsigned short ReadUInt16(std::span<const unsigned char> bytes, int offset);

		static unsigned int ReadUInt32(std::span<const unsigned char> bytes, int offset);

		static unsigned long long ReadUInt64(std::span<const unsigned char> bytes, int offset);


	public:
		static constexpr unsigned int Dsc2Magic = 0x32435344; // "DSC2" as a little-endian scalar.
		static constexpr unsigned short Dsc2MajorVersion = 2;
		static constexpr unsigned short Dsc2MinorVersion = 0;
		static constexpr int Dsc2HeaderSize = 128;
		static constexpr int Dsc2ExtensionBlockHeaderSize = 32;

	private:
		static constexpr int Dsc2MagicOffset = 0;
		static constexpr int Dsc2MajorVersionOffset = 4;
		static constexpr int Dsc2MinorVersionOffset = 6;
		static constexpr int Dsc2HeaderSizeOffset = 8;
		static constexpr int Dsc2HeaderFlagsOffset = 10;
		static constexpr int Dsc2TotalSizeOffset = 12;
		static constexpr int Dsc2ExtensionTableOffsetOffset = 16;
		static constexpr int Dsc2ExtensionCountOffset = 20;
		static constexpr int Dsc2Reserved0Offset = 22;
		static constexpr int Dsc2ExtensionTableByteSizeOffset = 24;
		static constexpr int Dsc2DescriptorFlagsOffset = 28;
		static constexpr int Dsc2DescriptorIdentityHashOffset = 32;
		static constexpr int Dsc2CapabilitySetHashOffset = 40;
		static constexpr int Dsc2NormalizedFootprintHashOffset = 48;
		static constexpr int Dsc2OwnerVirtualThreadIdOffset = 56;
		static constexpr int Dsc2ParserStatusOffset = 58;
		static constexpr int Dsc2OwnerContextIdOffset = 60;
		static constexpr int Dsc2OwnerCoreIdOffset = 64;
		static constexpr int Dsc2OwnerPodIdOffset = 68;
		static constexpr int Dsc2DeviceIdOffset = 72;
		static constexpr int Dsc2AddressSpaceSummaryOffset = 76;
		static constexpr int Dsc2Reserved1Offset = 78;
		static constexpr int Dsc2OwnerDomainTagOffset = 80;
		static constexpr int Dsc2MappingEpochOffset = 88;
		static constexpr int Dsc2Reserved2Offset = 96;
		static constexpr int Dsc2Reserved3Offset = 104;
		static constexpr int Dsc2Reserved4Offset = 112;
		static constexpr int Dsc2Reserved5Offset = 120;

		static constexpr int Dsc2ExtensionTypeOffset = 0;
		static constexpr int Dsc2ExtensionVersionOffset = 2;
		static constexpr int Dsc2ExtensionFlagsOffset = 4;
		static constexpr int Dsc2ExtensionAlignmentOffset = 6;
		static constexpr int Dsc2ExtensionLengthOffset = 8;
		static constexpr int Dsc2ExtensionCapabilityIdOffset = 12;
		static constexpr int Dsc2ExtensionMinimumParserMajorOffset = 14;
		static constexpr int Dsc2ExtensionPayloadChecksumOffset = 16;
		static constexpr int Dsc2ExtensionReserved0Offset = 20;
		static constexpr int Dsc2ExtensionReserved1Offset = 24;

		static constexpr int StridedRangePayloadSize = 32;
		static constexpr int Tile2DPayloadSize = 40;
		static constexpr int AddressSpacePayloadSize = 32;
		static constexpr int FootprintSummaryPayloadSize = 48;
		static constexpr int ScatterGatherPayloadHeaderSize = 16;
		static constexpr int ScatterGatherSegmentEntrySize = 16;
		static constexpr int MaxExactRangeCount = 64;
		static constexpr int MaxScatterGatherSegmentCount = 128;
		static constexpr unsigned long long MaxConservativeFootprintSpanBytes = 1ULL << 32;

	public:
		static std::shared_ptr<DmaStreamComputeStructuralReadResult> ReadDsc2StructuralOwnerBinding(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference = std::nullopt);

		static std::shared_ptr<DmaStreamComputeDsc2ValidationResult> ParseDsc2ParserOnly(std::span<const unsigned char> descriptorBytes, std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> capabilitySet, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference = std::nullopt);

	private:
		static bool ValidateOwnerGuard(std::shared_ptr<DmaStreamComputeOwnerBinding> structuralOwnerBinding, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool HasAnyDsc2ReservedHeaderBitSet(std::span<const unsigned char> bytes);

		static bool TryDecodeDsc2AddressSpace(unsigned short raw, DmaStreamComputeDsc2AddressSpaceKind &addressSpace);

		static bool ValidateExtensionTable(unsigned int totalSize, unsigned int extensionTableOffset, unsigned short extensionCount, unsigned int extensionTableByteSize, std::wstring &message);

		static bool TryParseExtensionHeader(std::span<const unsigned char> bytes, int offset, int tableEnd, std::shared_ptr<Dsc2ExtensionHeader> &header, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool TryHandleExtension(std::span<const unsigned char> payload, std::shared_ptr<Dsc2ExtensionHeader> header, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, DmaStreamComputeDsc2AddressSpaceKind headerAddressSpace, std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> capabilitySet, std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder, std::optional<std::shared_ptr<Dsc2FootprintSummary>> &footprintSummary, bool &hasAddressSpaceExtension, unsigned long long &mappingEpoch, std::vector<std::shared_ptr<DmaStreamComputeDsc2ParsedExtension>> &parsedExtensions, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool ValidateKnownExtensionCapability(std::shared_ptr<Dsc2ExtensionHeader> header, std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> capabilitySet, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool TryParseAddressSpaceExtension(std::span<const unsigned char> payload, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, DmaStreamComputeDsc2AddressSpaceKind headerAddressSpace, unsigned long long &mappingEpoch, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool TryParseStridedRange(std::span<const unsigned char> payload, std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool TryParseTile2D(std::span<const unsigned char> payload, std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool TryParseScatterGather(std::span<const unsigned char> payload, std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool TryParseFootprintSummary(std::span<const unsigned char> payload, std::optional<std::shared_ptr<Dsc2FootprintSummary>> &summary, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool TryDecodeAccessKind(unsigned short raw, DmaStreamComputeDsc2AccessKind &accessKind);

		static bool TryValidateElementSize(unsigned short rawElementSize, unsigned long long &elementSize, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool ValidateRange(unsigned long long address, unsigned long long length, unsigned long long elementSize, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		static bool IsPowerOfTwoAlignment(unsigned short alignment);

		static std::shared_ptr<DmaStreamComputeDsc2ParsedExtension> CreateParsedExtension(std::shared_ptr<Dsc2ExtensionHeader> header, DmaStreamComputeDsc2FootprintOutcomeKind outcome);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private readonly record struct Dsc2ExtensionHeader(ushort RawType, ushort Version, DmaStreamComputeDsc2ExtensionFlags Flags, ushort Alignment, uint ByteLength, DmaStreamComputeDsc2CapabilityId CapabilityId, ushort MinimumParserMajor)
//		{
//			public DmaStreamComputeDsc2ExtensionType ExtensionType
//			{
//				get
//				{
//					return (DmaStreamComputeDsc2ExtensionType)RawType;
//				}
//			}
//		}

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private readonly record struct Dsc2FootprintSummary(DmaStreamComputeDsc2FootprintOutcomeKind OutcomeKind, DmaStreamComputeMemoryRange ReadRange, DmaStreamComputeMemoryRange WriteRange);
		struct Dsc2ExtensionHeader final
		{
			const unsigned short RawType;
			const unsigned short Version;
			const DmaStreamComputeDsc2ExtensionFlags Flags;
			const unsigned short Alignment;
			const unsigned int ByteLength;
			const DmaStreamComputeDsc2CapabilityId CapabilityId;
			const unsigned short MinimumParserMajor;
			const DmaStreamComputeDsc2ExtensionType ExtensionType;

			Dsc2ExtensionHeader(unsigned short rawType, unsigned short version,
				DmaStreamComputeDsc2ExtensionFlags flags, unsigned short alignment,
				unsigned int byteLength, DmaStreamComputeDsc2CapabilityId capabilityId,
				unsigned short minimumParserMajor) noexcept
				: RawType(rawType), Version(version), Flags(flags), Alignment(alignment),
				  ByteLength(byteLength), CapabilityId(capabilityId), MinimumParserMajor(minimumParserMajor),
				  ExtensionType(static_cast<DmaStreamComputeDsc2ExtensionType>(rawType)) {}
		};

		struct Dsc2FootprintSummary final
		{
			const DmaStreamComputeDsc2FootprintOutcomeKind OutcomeKind;
			const std::shared_ptr<DmaStreamComputeMemoryRange> ReadRange;
			const std::shared_ptr<DmaStreamComputeMemoryRange> WriteRange;

			Dsc2FootprintSummary(DmaStreamComputeDsc2FootprintOutcomeKind outcomeKind,
				std::shared_ptr<DmaStreamComputeMemoryRange> readRange,
				std::shared_ptr<DmaStreamComputeMemoryRange> writeRange)
				: OutcomeKind(outcomeKind), ReadRange(std::move(readRange)), WriteRange(std::move(writeRange))
			{
				if (!ReadRange || !WriteRange)
					throw std::invalid_argument("DSC2 footprint summary ranges cannot be null.");
			}
		};

	private:
		class Dsc2FootprintBuilder final : public std::enable_shared_from_this<Dsc2FootprintBuilder>
		{
		private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<DmaStreamComputeMemoryRange> _readRanges = new();
			std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> _readRanges = std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>>();
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly List<DmaStreamComputeMemoryRange> _writeRanges = new();
			std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> _writeRanges = std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>>();
			bool _hasConservativeRange = false;

		public:
			bool TryAddStridedRange(DmaStreamComputeDsc2AccessKind accessKind, unsigned long long baseAddress, unsigned int elementCount, unsigned long long elementSize, unsigned long long strideBytes, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

			bool TryAddTile2D(DmaStreamComputeDsc2AccessKind accessKind, unsigned long long baseAddress, unsigned int rows, unsigned int columns, unsigned long long elementSize, unsigned long long rowStrideBytes, unsigned long long columnStrideBytes, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool TryAddScatterGather(DmaStreamComputeDsc2AccessKind accessKind, IReadOnlyList<DmaStreamComputeMemoryRange> segments, out DmaStreamComputeDsc2FootprintOutcomeKind outcome, out System.Nullable<DmaStreamComputeDsc2ValidationResult> failure)
			bool TryAddScatterGather(DmaStreamComputeDsc2AccessKind accessKind, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &segments, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

			bool TryBuild(std::optional<std::shared_ptr<Dsc2FootprintSummary>> &summary, std::optional<std::shared_ptr<DmaStreamComputeDsc2FootprintModel>> &footprint, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

		private:
			void AddRange(DmaStreamComputeDsc2AccessKind accessKind, std::shared_ptr<DmaStreamComputeMemoryRange> range);
		};

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool ValidateFootprintSummary(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges, Dsc2FootprintSummary summary, out System.Nullable<DmaStreamComputeDsc2ValidationResult> failure)
		static bool ValidateFootprintSummary(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &readRanges, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &writeRanges, std::shared_ptr<Dsc2FootprintSummary> summary, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool SummaryCovers(IReadOnlyList<DmaStreamComputeMemoryRange> ranges, DmaStreamComputeMemoryRange summaryRange)
		static bool SummaryCovers(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges, std::shared_ptr<DmaStreamComputeMemoryRange> summaryRange);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool SummaryExactlyMatches(IReadOnlyList<DmaStreamComputeMemoryRange> ranges, DmaStreamComputeMemoryRange summaryRange)
		static bool SummaryExactlyMatches(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges, std::shared_ptr<DmaStreamComputeMemoryRange> summaryRange);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<DmaStreamComputeMemoryRange> NormalizeRanges(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
		static const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> NormalizeRanges(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static ulong ComputeDsc2FootprintHash(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges, DmaStreamComputeDsc2FootprintOutcomeKind outcomeKind)
		static unsigned long long ComputeDsc2FootprintHash(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &readRanges, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &writeRanges, DmaStreamComputeDsc2FootprintOutcomeKind outcomeKind);

		static bool TryMultiply(unsigned long long left, unsigned long long right, unsigned long long &product);

		static bool TryAdd(unsigned long long left, unsigned long long right, unsigned long long &sum);

	};
}
