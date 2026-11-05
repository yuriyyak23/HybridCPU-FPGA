#pragma once

#include "../Telemetry/AcceleratorTelemetry.h"
#include "../../../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "AcceleratorCommandDescriptor.h"
#include <string>
#include <vector>
#include <span>
#include <limits>
#include <optional>
#include <format>
#include <memory>
#include "../../../../../tangible_string_helper.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{

	class AcceleratorDescriptorStructuralReadResult;
	class AcceleratorCarrierValidationResult;
	class AcceleratorDescriptorStructuralReadResult final {
	public:
		const AcceleratorDescriptorFault Fault;
		const std::shared_ptr<AcceleratorDescriptorReference> DescriptorReference;
		const unsigned long long DescriptorIdentityHash;
		const unsigned int DescriptorSize;
		const std::shared_ptr<AcceleratorOwnerBinding> OwnerBinding;
		const std::wstring Message;
		const bool IsValid;

	private:
		AcceleratorDescriptorStructuralReadResult(AcceleratorDescriptorFault fault, std::shared_ptr<AcceleratorDescriptorReference> reference, unsigned long long identityHash, unsigned int descriptorSize, std::shared_ptr<AcceleratorOwnerBinding> owner, std::wstring message)
			: Fault(fault), DescriptorReference(std::move(reference)), DescriptorIdentityHash(identityHash), DescriptorSize(descriptorSize), OwnerBinding(std::move(owner)), Message(std::move(message)), IsValid(fault == AcceleratorDescriptorFault::None && OwnerBinding != nullptr) {}

	public:
		static std::shared_ptr<AcceleratorDescriptorStructuralReadResult> Valid(std::shared_ptr<AcceleratorDescriptorReference> reference, unsigned long long identityHash, unsigned int descriptorSize, std::shared_ptr<AcceleratorOwnerBinding> owner) {
			if (!reference || !owner) throw std::invalid_argument("reference and owner are required");
			return std::shared_ptr<AcceleratorDescriptorStructuralReadResult>(new AcceleratorDescriptorStructuralReadResult(AcceleratorDescriptorFault::None, std::move(reference), identityHash, descriptorSize, std::move(owner), L"L7-SDC structural owner/domain fields located; descriptor acceptance still requires guard-plane authority."));
		}
		static std::shared_ptr<AcceleratorDescriptorStructuralReadResult> Fail(AcceleratorDescriptorFault fault, const std::wstring& message) {
			if (fault == AcceleratorDescriptorFault::None) throw std::invalid_argument("failure fault is required");
			return std::shared_ptr<AcceleratorDescriptorStructuralReadResult>(new AcceleratorDescriptorStructuralReadResult(fault, nullptr, 0, 0, nullptr, message));
		}
		std::shared_ptr<AcceleratorOwnerBinding> RequireOwnerBindingForGuard() const {
			if (IsValid && OwnerBinding) return OwnerBinding;
			throw std::logic_error("L7-SDC structural owner/domain read failed.");
		}
	};
	class AcceleratorDescriptorParser final
	{
	public:
		static constexpr unsigned int Magic = 0x31434453; // "SDC1" as a little-endian scalar.
		static constexpr unsigned short CurrentAbiVersion = 1;
		static constexpr int CurrentHeaderSize = 128;

	private:
		static constexpr int HeaderSize = CurrentHeaderSize;
		static constexpr int RangeEntrySize = 16;
		static constexpr unsigned short MaxRangeCount = 16;

		static constexpr int MagicOffset = 0;
		static constexpr int AbiVersionOffset = 4;
		static constexpr int HeaderSizeOffset = 6;
		static constexpr int DescriptorSizeOffset = 8;
		static constexpr int FlagsOffset = 12;
		static constexpr int AcceleratorClassOffset = 16;
		static constexpr int AcceleratorIdOffset = 18;
		static constexpr int OperationOffset = 20;
		static constexpr int DatatypeOffset = 22;
		static constexpr int DescriptorIdentityHashOffset = 24;
		static constexpr int NormalizedFootprintHashOffset = 32;
		static constexpr int ShapeOffset = 40;
		static constexpr int ShapeRankOffset = 42;
		static constexpr int SourceRangeCountOffset = 44;
		static constexpr int DestinationRangeCountOffset = 46;
		static constexpr int ScratchRangeCountOffset = 48;
		static constexpr int PartialCompletionPolicyOffset = 50;
		static constexpr int AlignmentBytesOffset = 52;
		static constexpr int Reserved1Offset = 54;
		static constexpr int ElementCountOffset = 56;
		static constexpr int CapabilityVersionOffset = 64;
		static constexpr int OwnerVirtualThreadIdOffset = 68;
		static constexpr int Reserved2Offset = 70;
		static constexpr int OwnerContextIdOffset = 72;
		static constexpr int OwnerCoreIdOffset = 76;
		static constexpr int OwnerPodIdOffset = 80;
		static constexpr int Reserved3Offset = 84;
		static constexpr int DomainTagOffset = 88;
		static constexpr int SourceRangeTableOffset = 96;
		static constexpr int DestinationRangeTableOffset = 100;
		static constexpr int ScratchRangeTableOffset = 104;
		static constexpr int Reserved4Offset = 108;
		static constexpr int ScratchRequiredBytesOffset = 112;
		static constexpr int Reserved5Offset = 120;

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static AcceleratorDescriptorValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, System.Nullable<AcceleratorDescriptorReference> descriptorReference = null, AcceleratorTelemetry? telemetry = null)
		static std::shared_ptr<AcceleratorDescriptorValidationResult> Parse(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference = std::nullopt, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static AcceleratorDescriptorValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, AcceleratorGuardEvidence guardEvidence, System.Nullable<AcceleratorDescriptorReference> descriptorReference = null, AcceleratorTelemetry? telemetry = null)
		static std::shared_ptr<AcceleratorDescriptorValidationResult> Parse(std::span<const unsigned char> descriptorBytes, std::shared_ptr<AcceleratorGuardEvidence> guardEvidence, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference = std::nullopt, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static AcceleratorDescriptorValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, AcceleratorGuardDecision ownerGuardDecision, System.Nullable<AcceleratorDescriptorReference> descriptorReference = null, AcceleratorTelemetry? telemetry = null)
		static std::shared_ptr<AcceleratorDescriptorValidationResult> Parse(std::span<const unsigned char> descriptorBytes, std::shared_ptr<AcceleratorGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference = std::nullopt, std::shared_ptr<AcceleratorTelemetry> telemetry = nullptr);

	private:
		static std::shared_ptr<AcceleratorDescriptorValidationResult> ParseGuardedCore(std::span<const unsigned char> descriptorBytes, std::shared_ptr<AcceleratorGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static AcceleratorDescriptorValidationResult RecordParseResult(AcceleratorDescriptorValidationResult result, AcceleratorTelemetry? telemetry)
		static std::shared_ptr<AcceleratorDescriptorValidationResult> RecordParseResult(std::shared_ptr<AcceleratorDescriptorValidationResult> result, std::shared_ptr<AcceleratorTelemetry> telemetry);

	public:
		static std::shared_ptr<AcceleratorDescriptorStructuralReadResult> ReadStructuralOwnerBinding(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference = std::nullopt);

		static bool TryReadHeader(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference, std::shared_ptr<AcceleratorDescriptorHeader> &header, std::shared_ptr<AcceleratorDescriptorReference> &effectiveReference, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

		static bool ValidateReservedZero(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

		static bool TryValidateNativeVliwCarrier(VLIW_Instruction const instruction, unsigned short opcode, int slotIndex, bool hasDescriptorSideband, std::optional<std::shared_ptr<AcceleratorCarrierValidationResult>> &failure);

		static std::shared_ptr<AcceleratorCarrierValidationResult> ValidateNativeCarrier(VLIW_Instruction const instruction, unsigned short opcode, int slotIndex, bool hasDescriptorSideband);

		static unsigned long long ComputeDescriptorIdentityHash(std::span<const unsigned char> descriptorBytes);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static ulong ComputeNormalizedFootprintHash(AcceleratorClassId acceleratorClass, AcceleratorDeviceId acceleratorId, AcceleratorOperationKind operation, AcceleratorDatatype datatype, AcceleratorShapeKind shape, ushort shapeRank, ulong elementCount, AcceleratorPartialCompletionPolicy partialCompletionPolicy, AcceleratorAlignmentRequirement alignment, IReadOnlyList<AcceleratorMemoryRange> normalizedSourceRanges, IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationRanges, IReadOnlyList<AcceleratorMemoryRange> normalizedScratchRanges)
		static unsigned long long ComputeNormalizedFootprintHash(AcceleratorClassId acceleratorClass, AcceleratorDeviceId acceleratorId, AcceleratorOperationKind operation, AcceleratorDatatype datatype, AcceleratorShapeKind shape, unsigned short shapeRank, unsigned long long elementCount, AcceleratorPartialCompletionPolicy partialCompletionPolicy, std::shared_ptr<AcceleratorAlignmentRequirement> alignment, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedSourceRanges, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedDestinationRanges, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedScratchRanges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static IReadOnlyList<AcceleratorMemoryRange> NormalizeMemoryRanges(IReadOnlyList<AcceleratorMemoryRange> ranges)
		static const std::vector<std::shared_ptr<AcceleratorMemoryRange>> NormalizeMemoryRanges(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges);

	private:
		static bool ValidateCapabilityShape(AcceleratorClassId acceleratorClass, AcceleratorDeviceId acceleratorId, AcceleratorOperationKind operation, AcceleratorDatatype datatype, AcceleratorShapeKind shape, unsigned short shapeRank, unsigned long long elementCount, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

		static bool ValidateDescriptorSize(unsigned int descriptorSize, unsigned short sourceRangeCount, unsigned short destinationRangeCount, unsigned short scratchRangeCount, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

		static bool ValidateRangeCount(unsigned short rangeCount, const std::wstring &rangeKind, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

		static bool ValidateAlignment(unsigned short alignmentBytes, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

		static bool TryReadRanges(std::span<const unsigned char> bytes, unsigned int descriptorSize, unsigned int tableOffset, unsigned short rangeCount, unsigned short alignmentBytes, bool requireNonEmpty, const std::wstring &rangeKind, std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

		static bool ValidateRangeTableLayout(unsigned int descriptorSize, unsigned int sourceOffset, unsigned short sourceCount, unsigned int destinationOffset, unsigned short destinationCount, unsigned int scratchOffset, unsigned short scratchCount, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

		struct RangeTableSpan;
		static bool TryAddRangeTableSpan(std::vector<std::shared_ptr<RangeTableSpan>> &declaredTables, const std::wstring &rangeKind, unsigned int tableOffset, unsigned short rangeCount, unsigned int descriptorSize, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool ScratchRangesCoverRequirement(IReadOnlyList<AcceleratorMemoryRange> scratchRanges, ulong scratchRequiredBytes)
		static bool ScratchRangesCoverRequirement(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &scratchRanges, unsigned long long scratchRequiredBytes);

		static bool HasAnyReservedBitSet(std::span<const unsigned char> bytes);

		static unsigned short ReadUInt16(std::span<const unsigned char> bytes, int offset);

		static unsigned int ReadUInt32(std::span<const unsigned char> bytes, int offset);

		static unsigned long long ReadUInt64(std::span<const unsigned char> bytes, int offset);

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private readonly record struct RangeTableSpan(string Kind, ulong Start, ulong End);
	};

}
