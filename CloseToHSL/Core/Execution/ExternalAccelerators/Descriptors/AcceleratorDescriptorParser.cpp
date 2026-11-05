#include "AcceleratorDescriptorParser.h"
#include "AcceleratorDescriptorTaxonomyCatalog.h"
#include "../Auth/AcceleratorOwnerDomainGuard.h"
#include "../../../Pipeline/Safety/SafetyVerifier.Types.h"
#include "../../../../../NonRTL/Arch/YAKSys_Hybrid_CPU.Arch.OpcodeRegistry.h"
#include "../../../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include <algorithm>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Telemetry;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{
	struct AcceleratorDescriptorParser::RangeTableSpan final
	{
		std::wstring Kind;
		unsigned long long Start = 0;
		unsigned long long End = 0;

		RangeTableSpan(std::wstring kind, unsigned long long start, unsigned long long end)
			: Kind(std::move(kind)), Start(start), End(end) {}
	};

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static AcceleratorDescriptorValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, System.Nullable<AcceleratorDescriptorReference> descriptorReference = null, AcceleratorTelemetry? telemetry = null)
	std::shared_ptr<AcceleratorDescriptorValidationResult> AcceleratorDescriptorParser::Parse(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference, std::shared_ptr<AcceleratorTelemetry> telemetry)
	{
		std::shared_ptr<AcceleratorDescriptorStructuralReadResult> structuralRead = ReadStructuralOwnerBinding(descriptorBytes, descriptorReference);
		if (!structuralRead->IsValid)
		{
			return RecordParseResult(AcceleratorDescriptorValidationResult::Fail(structuralRead->Fault, structuralRead->Message), telemetry);
		}

		return RecordParseResult(AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::OwnerDomainFault, L"L7-SDC descriptor cannot be accepted without an explicit owner/domain guard decision."), telemetry);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static AcceleratorDescriptorValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, AcceleratorGuardEvidence guardEvidence, System.Nullable<AcceleratorDescriptorReference> descriptorReference = null, AcceleratorTelemetry? telemetry = null)
	std::shared_ptr<AcceleratorDescriptorValidationResult> AcceleratorDescriptorParser::Parse(std::span<const unsigned char> descriptorBytes, std::shared_ptr<AcceleratorGuardEvidence> guardEvidence, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference, std::shared_ptr<AcceleratorTelemetry> telemetry)
	{
		std::shared_ptr<AcceleratorDescriptorStructuralReadResult> structuralRead = ReadStructuralOwnerBinding(descriptorBytes, descriptorReference);
		if (!structuralRead->IsValid)
		{
			return RecordParseResult(AcceleratorDescriptorValidationResult::Fail(structuralRead->Fault, structuralRead->Message), telemetry);
		}

		std::optional<std::shared_ptr<AcceleratorGuardEvidence>> evidence;
		if (guardEvidence) evidence = guardEvidence;
		std::shared_ptr<AcceleratorGuardDecision> ownerGuardDecision = AcceleratorOwnerDomainGuard::getDefault()->EnsureBeforeDescriptorAcceptance(structuralRead->RequireOwnerBindingForGuard(), evidence);
		return Parse(descriptorBytes, ownerGuardDecision, descriptorReference, telemetry);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static AcceleratorDescriptorValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, AcceleratorGuardDecision ownerGuardDecision, System.Nullable<AcceleratorDescriptorReference> descriptorReference = null, AcceleratorTelemetry? telemetry = null)
	std::shared_ptr<AcceleratorDescriptorValidationResult> AcceleratorDescriptorParser::Parse(std::span<const unsigned char> descriptorBytes, std::shared_ptr<AcceleratorGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference, std::shared_ptr<AcceleratorTelemetry> telemetry)
	{
		if (!ownerGuardDecision)
		{
			return RecordParseResult(AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::OwnerDomainFault, L"L7-SDC descriptor requires an explicit owner/domain guard decision."), telemetry);
		}
		std::shared_ptr<AcceleratorDescriptorValidationResult> result = ParseGuardedCore(descriptorBytes, ownerGuardDecision, descriptorReference);
		if (!ownerGuardDecision->IsAllowed)
		{
			if (telemetry != nullptr)
			{
				std::optional<std::shared_ptr<AcceleratorGuardDecision>> decision = ownerGuardDecision;
				telemetry->RecordGuardReject(decision, result->Message);
			}
		}

		return RecordParseResult(result, telemetry);
	}

	std::shared_ptr<AcceleratorDescriptorValidationResult> AcceleratorDescriptorParser::ParseGuardedCore(std::span<const unsigned char> descriptorBytes, std::shared_ptr<AcceleratorGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference)
	{
		std::shared_ptr<AcceleratorDescriptorStructuralReadResult> structuralRead = ReadStructuralOwnerBinding(descriptorBytes, descriptorReference);
		if (!structuralRead->IsValid)
		{
			return AcceleratorDescriptorValidationResult::Fail(structuralRead->Fault, structuralRead->Message);
		}
		if (!ownerGuardDecision)
		{
			return AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::OwnerDomainFault, L"L7-SDC descriptor requires an explicit owner/domain guard decision.");
		}

		std::shared_ptr<AcceleratorOwnerBinding> structuralOwnerBinding = structuralRead->RequireOwnerBindingForGuard();
		if (!ownerGuardDecision->IsAllowed)
		{
			return AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::OwnerDomainFault, StringHelper::isEmptyOrWhiteSpace(ownerGuardDecision->Message) ? L"L7-SDC descriptor owner/domain guard rejected before descriptor acceptance." : ownerGuardDecision->Message);
		}

		if (ownerGuardDecision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || ownerGuardDecision->LegalityDecision.getAttemptedReplayCertificateReuse() || (!ownerGuardDecision->Evidence || ownerGuardDecision->Evidence->Source != AcceleratorGuardEvidenceSource::GuardPlane))
		{
			return AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::OwnerDomainFault, L"L7-SDC owner guard decision must come from the guard plane before replay/certificate or evidence-plane reuse.");
		}

		if (ownerGuardDecision->DescriptorOwnerBinding == nullptr || !ownerGuardDecision->DescriptorOwnerBinding->Equals(structuralOwnerBinding))
		{
			return AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::OwnerDomainFault, L"L7-SDC owner guard decision does not match structurally-read descriptor owner fields.");
		}

		std::shared_ptr<AcceleratorDescriptorHeader> header;
		std::shared_ptr<AcceleratorDescriptorReference> effectiveReference;
		std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> headerFailure;
		if (!TryReadHeader(descriptorBytes, descriptorReference, header, effectiveReference, headerFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return headerFailure!;
			return headerFailure && headerFailure.value()
				? headerFailure.value()
				: AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC header read failed without a diagnostic.");
		}

		if (HasAnyReservedBitSet(descriptorBytes))
		{
			return AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::ReservedFieldFault, L"L7-SDC v1 descriptor flags and reserved fields must be zero.");
		}

		std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> capabilityFailure;
		if (!ValidateCapabilityShape(header->AcceleratorClass, header->AcceleratorId, header->Operation, header->Datatype, header->Shape, header->ShapeRank, header->ElementCount, capabilityFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return capabilityFailure!;
			return capabilityFailure && capabilityFailure.value()
				? capabilityFailure.value()
				: AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::UnsupportedShape, L"L7-SDC capability validation failed without a diagnostic.");
		}

		unsigned int sourceRangeTableOffset = ReadUInt32(descriptorBytes, SourceRangeTableOffset);
		unsigned int destinationRangeTableOffset = ReadUInt32(descriptorBytes, DestinationRangeTableOffset);
		unsigned int scratchRangeTableOffset = ReadUInt32(descriptorBytes, ScratchRangeTableOffset);

		std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> tableLayoutFailure;
		if (!ValidateRangeTableLayout(header->DescriptorSize, sourceRangeTableOffset, header->SourceRangeCount, destinationRangeTableOffset, header->DestinationRangeCount, scratchRangeTableOffset, header->ScratchRangeCount, tableLayoutFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return tableLayoutFailure!;
			return tableLayoutFailure && tableLayoutFailure.value()
				? tableLayoutFailure.value()
				: AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC range table layout validation failed without a diagnostic.");
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> sourceRanges;
		std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> sourceFailure;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryReadRanges(descriptorBytes, header.DescriptorSize, sourceRangeTableOffset, header.SourceRangeCount, header.Alignment.MinimumAlignmentBytes, requireNonEmpty: true, "source", out IReadOnlyList<AcceleratorMemoryRange> sourceRanges, out System.Nullable<AcceleratorDescriptorValidationResult> sourceFailure))
		if (!TryReadRanges(descriptorBytes, header->DescriptorSize, sourceRangeTableOffset, header->SourceRangeCount, header->Alignment.MinimumAlignmentBytes, true, L"source", sourceRanges, sourceFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return sourceFailure!;
			return sourceFailure && sourceFailure.value()
				? sourceFailure.value()
				: AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC source range parsing failed without a diagnostic.");
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> destinationRanges;
		std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> destinationFailure;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryReadRanges(descriptorBytes, header.DescriptorSize, destinationRangeTableOffset, header.DestinationRangeCount, header.Alignment.MinimumAlignmentBytes, requireNonEmpty: true, "destination", out IReadOnlyList<AcceleratorMemoryRange> destinationRanges, out System.Nullable<AcceleratorDescriptorValidationResult> destinationFailure))
		if (!TryReadRanges(descriptorBytes, header->DescriptorSize, destinationRangeTableOffset, header->DestinationRangeCount, header->Alignment.MinimumAlignmentBytes, true, L"destination", destinationRanges, destinationFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return destinationFailure!;
			return destinationFailure && destinationFailure.value()
				? destinationFailure.value()
				: AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC destination range parsing failed without a diagnostic.");
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> scratchRanges;
		std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> scratchFailure;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryReadRanges(descriptorBytes, header.DescriptorSize, scratchRangeTableOffset, header.ScratchRangeCount, header.Alignment.MinimumAlignmentBytes, requireNonEmpty: header.ScratchRequiredBytes != 0, "scratch", out IReadOnlyList<AcceleratorMemoryRange> scratchRanges, out System.Nullable<AcceleratorDescriptorValidationResult> scratchFailure))
		if (!TryReadRanges(descriptorBytes, header->DescriptorSize, scratchRangeTableOffset, header->ScratchRangeCount, header->Alignment.MinimumAlignmentBytes, header->ScratchRequiredBytes != 0, L"scratch", scratchRanges, scratchFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return scratchFailure!;
			return scratchFailure && scratchFailure.value()
				? scratchFailure.value()
				: AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC scratch range parsing failed without a diagnostic.");
		}

		if (header->ScratchRequiredBytes != 0 && !ScratchRangesCoverRequirement(scratchRanges, header->ScratchRequiredBytes))
		{
			return AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC scratch ranges do not cover the descriptor scratch requirement.");
		}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<AcceleratorMemoryRange> normalizedSourceRanges = NormalizeMemoryRanges(sourceRanges);
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> normalizedSourceRanges = NormalizeMemoryRanges(sourceRanges);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationRanges = NormalizeMemoryRanges(destinationRanges);
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> normalizedDestinationRanges = NormalizeMemoryRanges(destinationRanges);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<AcceleratorMemoryRange> normalizedScratchRanges = NormalizeMemoryRanges(scratchRanges);
		const std::vector<std::shared_ptr<AcceleratorMemoryRange>> normalizedScratchRanges = NormalizeMemoryRanges(scratchRanges);

		unsigned long long computedFootprintHash = ComputeNormalizedFootprintHash(header->AcceleratorClass, header->AcceleratorId, header->Operation, header->Datatype, header->Shape, header->ShapeRank, header->ElementCount, header->PartialCompletionPolicy, std::make_shared<AcceleratorAlignmentRequirement>(header->Alignment), normalizedSourceRanges, normalizedDestinationRanges, normalizedScratchRanges);

		if (computedFootprintHash != header->Identity.NormalizedFootprintHash)
		{
			return AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::NormalizedFootprintHashMismatch, L"L7-SDC normalized footprint hash does not match the validated descriptor footprint.");
		}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: ulong computedIdentityHash = ComputeDescriptorIdentityHash(descriptorBytes.Slice(0, checked((int)header.DescriptorSize)));
		unsigned long long computedIdentityHash = ComputeDescriptorIdentityHash(descriptorBytes.first(header->DescriptorSize));
		if (computedIdentityHash != header->Identity.DescriptorIdentityHash)
		{
			return AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorIdentityHashMismatch, L"L7-SDC descriptor identity hash does not match the typed descriptor payload.");
		}

		std::shared_ptr<AcceleratorNormalizedFootprint> footprint = std::make_shared<AcceleratorNormalizedFootprint>(AcceleratorNormalizedFootprint{normalizedSourceRanges, normalizedDestinationRanges, normalizedScratchRanges, computedFootprintHash});

		std::shared_ptr<AcceleratorCommandDescriptor> descriptor = std::make_shared<AcceleratorCommandDescriptor>();
		descriptor->DescriptorReference = *effectiveReference;
		descriptor->Header = *header;
		descriptor->AbiVersion = header->AbiVersion;
		descriptor->HeaderSize = header->HeaderSize;
		descriptor->DescriptorSize = header->DescriptorSize;
		descriptor->AcceleratorClass = header->AcceleratorClass;
		descriptor->AcceleratorId = header->AcceleratorId;
		descriptor->Operation = header->Operation;
		descriptor->Datatype = header->Datatype;
		descriptor->Shape = header->Shape;
		descriptor->ShapeRank = header->ShapeRank;
		descriptor->ElementCount = header->ElementCount;
		descriptor->CapabilityVersion = header->CapabilityVersion;
		descriptor->Alignment = header->Alignment;
		descriptor->ScratchRequirement = AcceleratorScratchRequirement{header->ScratchRequiredBytes, scratchRanges};
		descriptor->PartialCompletionPolicy = header->PartialCompletionPolicy;
		descriptor->OwnerBinding = header->OwnerBinding;
		descriptor->OwnerGuardDecision = ownerGuardDecision;
		descriptor->Identity = header->Identity;
		descriptor->SourceRanges = sourceRanges;
		descriptor->DestinationRanges = destinationRanges;
		descriptor->ScratchRanges = scratchRanges;
		descriptor->NormalizedFootprint = *footprint;

		return AcceleratorDescriptorValidationResult::Valid(descriptor);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static AcceleratorDescriptorValidationResult RecordParseResult(AcceleratorDescriptorValidationResult result, AcceleratorTelemetry? telemetry)
	std::shared_ptr<AcceleratorDescriptorValidationResult> AcceleratorDescriptorParser::RecordParseResult(std::shared_ptr<AcceleratorDescriptorValidationResult> result, std::shared_ptr<AcceleratorTelemetry> telemetry)
	{
		if (telemetry != nullptr)
		{
			telemetry->RecordDescriptorParse(result->IsDescriptorAbiAccepted, result->Fault, result->Message);
		}
		return result;
	}

	std::shared_ptr<AcceleratorDescriptorStructuralReadResult> AcceleratorDescriptorParser::ReadStructuralOwnerBinding(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference)
	{
		if (descriptorBytes.size() < HeaderSize)
		{
			return AcceleratorDescriptorStructuralReadResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC descriptor is shorter than the v1 fixed header.");
		}

		unsigned int magic = ReadUInt32(descriptorBytes, MagicOffset);
		if (magic != Magic)
		{
			return AcceleratorDescriptorStructuralReadResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC descriptor magic does not match v1.");
		}

		unsigned short abiVersion = ReadUInt16(descriptorBytes, AbiVersionOffset);
		if (abiVersion != CurrentAbiVersion)
		{
			return AcceleratorDescriptorStructuralReadResult::Fail(AcceleratorDescriptorFault::UnsupportedAbiVersion, std::format(L"Unsupported L7-SDC descriptor ABI version {0}.", abiVersion));
		}

		unsigned short headerSize = ReadUInt16(descriptorBytes, HeaderSizeOffset);
		if (headerSize != HeaderSize)
		{
			return AcceleratorDescriptorStructuralReadResult::Fail(AcceleratorDescriptorFault::DescriptorSizeFault, std::format(L"L7-SDC descriptor header size {0} does not match v1 header size {1}.", headerSize, HeaderSize));
		}

		unsigned int descriptorSize = ReadUInt32(descriptorBytes, DescriptorSizeOffset);
		if (descriptorSize < HeaderSize || descriptorSize > descriptorBytes.size() || descriptorSize > static_cast<unsigned int>(std::numeric_limits<int>::max()))
		{
			return AcceleratorDescriptorStructuralReadResult::Fail(AcceleratorDescriptorFault::DescriptorSizeFault, L"L7-SDC descriptor size is outside the provided descriptor buffer.");
		}

		unsigned long long descriptorIdentityHash = ReadUInt64(descriptorBytes, DescriptorIdentityHashOffset);
		std::shared_ptr<AcceleratorDescriptorReference> effectiveReference = descriptorReference && *descriptorReference
			? *descriptorReference
			: std::make_shared<AcceleratorDescriptorReference>(AcceleratorDescriptorReference{0, descriptorSize, descriptorIdentityHash});

		if (effectiveReference->DescriptorSize != 0 && effectiveReference->DescriptorSize < descriptorSize)
		{
			return AcceleratorDescriptorStructuralReadResult::Fail(AcceleratorDescriptorFault::DescriptorReferenceMismatch, L"L7-SDC descriptor reference sideband does not cover the typed descriptor payload.");
		}

		if (effectiveReference->DescriptorIdentityHash != 0 && effectiveReference->DescriptorIdentityHash != descriptorIdentityHash)
		{
			return AcceleratorDescriptorStructuralReadResult::Fail(AcceleratorDescriptorFault::DescriptorReferenceMismatch, L"L7-SDC descriptor reference identity hash does not match the typed descriptor payload.");
		}

		std::shared_ptr<AcceleratorOwnerBinding> ownerBinding = std::make_shared<AcceleratorOwnerBinding>(AcceleratorOwnerBinding{ReadUInt16(descriptorBytes, OwnerVirtualThreadIdOffset), ReadUInt32(descriptorBytes, OwnerContextIdOffset), ReadUInt32(descriptorBytes, OwnerCoreIdOffset), ReadUInt32(descriptorBytes, OwnerPodIdOffset), ReadUInt64(descriptorBytes, DomainTagOffset)});

		return AcceleratorDescriptorStructuralReadResult::Valid(effectiveReference, descriptorIdentityHash, descriptorSize, ownerBinding);
	}

	bool AcceleratorDescriptorParser::TryReadHeader(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<AcceleratorDescriptorReference>> descriptorReference, std::shared_ptr<AcceleratorDescriptorHeader> &header, std::shared_ptr<AcceleratorDescriptorReference> &effectiveReference, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		header = nullptr;
		effectiveReference = nullptr;
		failure = std::nullopt;

		if (descriptorBytes.size() < HeaderSize)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC descriptor is shorter than the v1 fixed header.");
			return false;
		}

		unsigned int magic = ReadUInt32(descriptorBytes, MagicOffset);
		if (magic != Magic)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC descriptor magic does not match v1.");
			return false;
		}

		unsigned short abiVersion = ReadUInt16(descriptorBytes, AbiVersionOffset);
		if (abiVersion != CurrentAbiVersion)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::UnsupportedAbiVersion, std::format(L"Unsupported L7-SDC descriptor ABI version {0}.", abiVersion));
			return false;
		}

		unsigned short headerSize = ReadUInt16(descriptorBytes, HeaderSizeOffset);
		if (headerSize != HeaderSize)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorSizeFault, std::format(L"L7-SDC descriptor header size {0} does not match v1 header size {1}.", headerSize, HeaderSize));
			return false;
		}

		unsigned int descriptorSize = ReadUInt32(descriptorBytes, DescriptorSizeOffset);
		if (descriptorSize < HeaderSize || descriptorSize > descriptorBytes.size() || descriptorSize > static_cast<unsigned int>(std::numeric_limits<int>::max()))
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorSizeFault, L"L7-SDC descriptor size is outside the provided descriptor buffer.");
			return false;
		}

		unsigned long long descriptorIdentityHash = ReadUInt64(descriptorBytes, DescriptorIdentityHashOffset);
		unsigned long long normalizedFootprintHash = ReadUInt64(descriptorBytes, NormalizedFootprintHashOffset);

		effectiveReference = descriptorReference && *descriptorReference
			? *descriptorReference
			: std::make_shared<AcceleratorDescriptorReference>(AcceleratorDescriptorReference{0, descriptorSize, descriptorIdentityHash});

		if (effectiveReference->DescriptorSize != 0 && effectiveReference->DescriptorSize < descriptorSize)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorReferenceMismatch, L"L7-SDC descriptor reference sideband does not cover the typed descriptor payload.");
			return false;
		}

		if (effectiveReference->DescriptorIdentityHash != 0 && effectiveReference->DescriptorIdentityHash != descriptorIdentityHash)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorReferenceMismatch, L"L7-SDC descriptor reference identity hash does not match the typed descriptor payload.");
			return false;
		}

		AcceleratorClassId acceleratorClass = static_cast<AcceleratorClassId>(ReadUInt16(descriptorBytes, AcceleratorClassOffset));
		AcceleratorDeviceId acceleratorId = static_cast<AcceleratorDeviceId>(ReadUInt16(descriptorBytes, AcceleratorIdOffset));
		AcceleratorOperationKind operation = static_cast<AcceleratorOperationKind>(ReadUInt16(descriptorBytes, OperationOffset));
		AcceleratorDatatype datatype = static_cast<AcceleratorDatatype>(ReadUInt16(descriptorBytes, DatatypeOffset));
		AcceleratorShapeKind shape = static_cast<AcceleratorShapeKind>(ReadUInt16(descriptorBytes, ShapeOffset));
		unsigned short shapeRank = ReadUInt16(descriptorBytes, ShapeRankOffset);
		unsigned short sourceRangeCount = ReadUInt16(descriptorBytes, SourceRangeCountOffset);
		unsigned short destinationRangeCount = ReadUInt16(descriptorBytes, DestinationRangeCountOffset);
		unsigned short scratchRangeCount = ReadUInt16(descriptorBytes, ScratchRangeCountOffset);
		AcceleratorPartialCompletionPolicy partialCompletionPolicy = static_cast<AcceleratorPartialCompletionPolicy>(ReadUInt16(descriptorBytes, PartialCompletionPolicyOffset));
		unsigned short alignmentBytes = ReadUInt16(descriptorBytes, AlignmentBytesOffset);
		unsigned long long elementCount = ReadUInt64(descriptorBytes, ElementCountOffset);
		unsigned int capabilityVersion = ReadUInt32(descriptorBytes, CapabilityVersionOffset);
		unsigned long long scratchRequiredBytes = ReadUInt64(descriptorBytes, ScratchRequiredBytesOffset);

		if (!ValidateDescriptorSize(descriptorSize, sourceRangeCount, destinationRangeCount, scratchRangeCount, failure))
		{
			return false;
		}

		if (!ValidateRangeCount(sourceRangeCount, L"source", failure) || !ValidateRangeCount(destinationRangeCount, L"destination", failure) || !ValidateRangeCount(scratchRangeCount, L"scratch", failure))
		{
			return false;
		}

		if (!ValidateAlignment(alignmentBytes, failure))
		{
			return false;
		}

		if (partialCompletionPolicy != AcceleratorPartialCompletionPolicy::AllOrNone)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::UnsupportedPartialCompletionPolicy, L"L7-SDC v1 accepts only all-or-none partial completion policy.");
			return false;
		}

		std::shared_ptr<AcceleratorOwnerBinding> ownerBinding = std::make_shared<AcceleratorOwnerBinding>(AcceleratorOwnerBinding{ReadUInt16(descriptorBytes, OwnerVirtualThreadIdOffset), ReadUInt32(descriptorBytes, OwnerContextIdOffset), ReadUInt32(descriptorBytes, OwnerCoreIdOffset), ReadUInt32(descriptorBytes, OwnerPodIdOffset), ReadUInt64(descriptorBytes, DomainTagOffset)});

		header = std::make_shared<AcceleratorDescriptorHeader>(AcceleratorDescriptorHeader{magic, abiVersion, headerSize, descriptorSize, acceleratorClass, acceleratorId, operation, datatype, shape, shapeRank, sourceRangeCount, destinationRangeCount, scratchRangeCount, partialCompletionPolicy, AcceleratorAlignmentRequirement{alignmentBytes}, elementCount, capabilityVersion, AcceleratorDescriptorIdentity{descriptorIdentityHash, normalizedFootprintHash}, ownerBinding, scratchRequiredBytes});
		return true;
	}

	bool AcceleratorDescriptorParser::ValidateReservedZero(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		failure = std::nullopt;
		if (descriptorBytes.size() < HeaderSize)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, L"L7-SDC descriptor is shorter than the v1 fixed header.");
			return false;
		}

		if (HasAnyReservedBitSet(descriptorBytes))
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::ReservedFieldFault, L"L7-SDC v1 descriptor flags and reserved fields must be zero.");
			return false;
		}

		return true;
	}

	bool AcceleratorDescriptorParser::TryValidateNativeVliwCarrier(VLIW_Instruction const instruction, unsigned short opcode, int slotIndex, bool hasDescriptorSideband, std::optional<std::shared_ptr<AcceleratorCarrierValidationResult>> &failure)
	{
		if (!OpcodeRegistry::IsSystemDeviceCommandOpcode(opcode))
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: opcode '{1}' is not a canonical native L7-SDC system-device command.", slotIndex, OpcodeRegistry::GetMnemonicOrHex(opcode)));
			return false;
		}

		if (slotIndex != 7)
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: L7-SDC system-device command carriers are hard-pinned to physical lane7 (lane 7).", slotIndex));
			return false;
		}

		if ((instruction.getWord3() & VLIW_Instruction::RetiredPolicyGapMask) != 0)
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: word3[50] is a retired policy gap and cannot carry L7-SDC ABI data.", slotIndex));
			return false;
		}

		if (instruction.getReserved() != 0)
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: word0[47:40] is reserved and cannot carry L7-SDC ABI data.", slotIndex));
			return false;
		}

		if (instruction.getVirtualThreadId() != 0)
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: word3[49:48] VirtualThreadId is a transport hint, not L7-SDC owner/domain authority.", slotIndex));
			return false;
		}

		if (instruction.getImmediate() != 0 || instruction.getPredicateMask() != 0 || instruction.getAcquire() || instruction.getRelease() || instruction.getSaturating() || instruction.getMaskAgnostic() || instruction.getTailAgnostic() || instruction.getIndexed() || instruction.getIs2D() || instruction.getReduction())
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: L7-SDC current command ABI accepts only a clean packed-register carrier.", slotIndex));
			return false;
		}

		unsigned char rd;
		unsigned char rs1;
		unsigned char rs2;
		if (!VLIW_Instruction::TryUnpackArchRegs(instruction.getWord1(), rd, rs1, rs2))
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: word1 must be the packed architectural register tuple, not a raw pointer authority.", slotIndex));
			return false;
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((rs2 != 0 && rs2 != VLIW_Instruction::NoArchReg))
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: L7-SDC current command ABI reserves rs2; use x0/NoArchReg instead of x{1}.", slotIndex, static_cast<unsigned int>(rs2)));
			return false;
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if (((opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_QUERY_CAPS || opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT)) && (rs1 != 0 && rs1 != VLIW_Instruction::NoArchReg))
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: {1} uses rd, x0, x0 in the current L7-SDC ABI; rs1 cannot carry authority.", slotIndex, OpcodeRegistry::GetMnemonicOrHex(opcode)));
			return false;
		}

		if (instruction.getSrc2Pointer() != 0)
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, std::format(L"Slot {0}: word2 raw pointer fields are not accepted as L7-SDC descriptor or owner authority.", slotIndex));
			return false;
		}

		if (opcode == Processor::CPU_Core::IsaOpcodeValues::ACCEL_SUBMIT && !hasDescriptorSideband)
		{
			failure = AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, L"ACCEL_SUBMIT requires typed AcceleratorCommandDescriptor sideband.");
			return false;
		}

		failure = std::nullopt;
		return true;
	}

	std::shared_ptr<AcceleratorCarrierValidationResult> AcceleratorDescriptorParser::ValidateNativeCarrier(VLIW_Instruction const instruction, unsigned short opcode, int slotIndex, bool hasDescriptorSideband)
	{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return TryValidateNativeVliwCarrier( in instruction, opcode, slotIndex, hasDescriptorSideband, out AcceleratorCarrierValidationResult? failure) ? AcceleratorCarrierValidationResult.Valid() : failure!;
		std::optional<std::shared_ptr<AcceleratorCarrierValidationResult>> failure;
		if (TryValidateNativeVliwCarrier(instruction, opcode, slotIndex, hasDescriptorSideband, failure)) return AcceleratorCarrierValidationResult::Valid();
		if (failure && *failure) return *failure;
		return AcceleratorCarrierValidationResult::Fail(AcceleratorDescriptorFault::DescriptorCarrierDecodeFault, L"L7-SDC carrier validation failed without a diagnostic.");
	}

	unsigned long long AcceleratorDescriptorParser::ComputeDescriptorIdentityHash(std::span<const unsigned char> descriptorBytes)
	{
		constexpr unsigned long long offsetBasis = 14695981039346656037ULL;
		constexpr unsigned long long prime = 1099511628211ULL;

		unsigned long long hash = offsetBasis;
		for (std::size_t index = 0; index < descriptorBytes.size(); index++)
		{
			unsigned char value = index >= DescriptorIdentityHashOffset && index < DescriptorIdentityHashOffset + sizeof(unsigned long long) ? static_cast<unsigned char>(0) : descriptorBytes[index];
//			unchecked
				hash ^= value;
				hash *= prime;
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
		}

		return hash == 0 ? offsetBasis : hash;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static ulong ComputeNormalizedFootprintHash(AcceleratorClassId acceleratorClass, AcceleratorDeviceId acceleratorId, AcceleratorOperationKind operation, AcceleratorDatatype datatype, AcceleratorShapeKind shape, ushort shapeRank, ulong elementCount, AcceleratorPartialCompletionPolicy partialCompletionPolicy, AcceleratorAlignmentRequirement alignment, IReadOnlyList<AcceleratorMemoryRange> normalizedSourceRanges, IReadOnlyList<AcceleratorMemoryRange> normalizedDestinationRanges, IReadOnlyList<AcceleratorMemoryRange> normalizedScratchRanges)
	unsigned long long AcceleratorDescriptorParser::ComputeNormalizedFootprintHash(AcceleratorClassId acceleratorClass, AcceleratorDeviceId acceleratorId, AcceleratorOperationKind operation, AcceleratorDatatype datatype, AcceleratorShapeKind shape, unsigned short shapeRank, unsigned long long elementCount, AcceleratorPartialCompletionPolicy partialCompletionPolicy, std::shared_ptr<AcceleratorAlignmentRequirement> alignment, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedSourceRanges, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedDestinationRanges, const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &normalizedScratchRanges)
	{
		constexpr unsigned long long offsetBasis = 14695981039346656037ULL;
		constexpr unsigned long long prime = 1099511628211ULL;

		unsigned long long hash = offsetBasis;
		auto add = [&](unsigned long long value) { hash ^= value; hash *= prime; };
		auto addRanges = [&](const std::vector<std::shared_ptr<AcceleratorMemoryRange>>& ranges) {
			add(static_cast<unsigned long long>(ranges.size()));
			for (const auto& range : ranges) {
				if (!range) throw std::invalid_argument("normalized range must not be null");
				add(range->Address);
				add(range->Length);
			}
		};
		if (!alignment) throw std::invalid_argument("alignment");
		add(static_cast<unsigned short>(acceleratorClass));
		add(static_cast<unsigned short>(acceleratorId));
		add(static_cast<unsigned short>(operation));
		add(static_cast<unsigned short>(datatype));
		add(static_cast<unsigned short>(shape));
		add(shapeRank);
		add(elementCount);
		add(static_cast<unsigned short>(partialCompletionPolicy));
		add(alignment->MinimumAlignmentBytes);
		addRanges(normalizedSourceRanges);
		addRanges(normalizedDestinationRanges);
		addRanges(normalizedScratchRanges);
		return hash == 0 ? offsetBasis : hash;

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		void Add(ulong value)
	//			{
	//				unchecked
	//				{
	//					hash ^= value;
	//					hash *= prime;
	//				}
	//			}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
// ORIGINAL LINE: void AddRanges(IReadOnlyList<AcceleratorMemoryRange>? ranges)
//		void AddRanges(IReadOnlyList<AcceleratorMemoryRange> ranges)
	//			{
	//				Add((ulong)((ranges == nullptr ? nullptr : ranges.Count) ?? 0));
	//				if (ranges == nullptr)
	//				{
	//					return;
	//				}
	//
	//				for (int index = 0; index < ranges.Count; index++)
	//				{
	//					Add(ranges[index].Address);
	//					Add(ranges[index].Length);
	//				}
	//			}
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public static IReadOnlyList<AcceleratorMemoryRange> NormalizeMemoryRanges(IReadOnlyList<AcceleratorMemoryRange> ranges)
	const std::vector<std::shared_ptr<AcceleratorMemoryRange>> AcceleratorDescriptorParser::NormalizeMemoryRanges(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges)
	{
		if (ranges.empty())
		{
			return {};
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> sorted = ranges;
		if (std::any_of(sorted.begin(), sorted.end(), [](const auto& range) { return !range; }))
		{
			throw std::invalid_argument("range must not be null");
		}

		std::sort(sorted.begin(), sorted.end(), [] (const auto& left, const auto& right)
		{
				return left->Address != right->Address ? left->Address < right->Address : left->Length < right->Length;
		});

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> merged;
		merged.reserve(sorted.size());
		unsigned long long currentStart = sorted[0]->Address;
		unsigned long long currentEnd = sorted[0]->Address + sorted[0]->Length;
		for (std::size_t index = 1; index < sorted.size(); index++)
		{
			std::shared_ptr<AcceleratorMemoryRange> next = sorted[index];
			unsigned long long nextEnd = next->Address + next->Length;
			if (next->Address <= currentEnd)
			{
				if (nextEnd > currentEnd)
				{
					currentEnd = nextEnd;
				}

				continue;
			}

			merged.push_back(std::make_shared<AcceleratorMemoryRange>(AcceleratorMemoryRange{currentStart, currentEnd - currentStart}));
			currentStart = next->Address;
			currentEnd = nextEnd;
		}

		merged.push_back(std::make_shared<AcceleratorMemoryRange>(AcceleratorMemoryRange{currentStart, currentEnd - currentStart}));
		return merged;
	}

	bool AcceleratorDescriptorParser::ValidateCapabilityShape(AcceleratorClassId acceleratorClass, AcceleratorDeviceId acceleratorId, AcceleratorOperationKind operation, AcceleratorDatatype datatype, AcceleratorShapeKind shape, unsigned short shapeRank, unsigned long long elementCount, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		failure = std::nullopt;

		if (acceleratorClass != AcceleratorClassId::Matrix)
		{
			std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> taxonomyEntry;
			std::wstring message = AcceleratorDescriptorTaxonomyCatalog::TryGetEntry(acceleratorClass, acceleratorId, operation, shape, taxonomyEntry) && taxonomyEntry->isMetadataOnly() ? L"L7-SDC descriptor taxonomy entry is known as metadata-only; parser acceptance, token issue, execution, commit, and compiler emission remain disabled." : L"L7-SDC descriptor accelerator class is unsupported in v1.";
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::UnsupportedAcceleratorClass, message);
			return false;
		}

		if (acceleratorId != AcceleratorDeviceId::ReferenceMatMul)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::UnsupportedAcceleratorId, L"L7-SDC descriptor accelerator id is unsupported in v1.");
			return false;
		}

		if (operation != AcceleratorOperationKind::MatMul)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::UnsupportedOperation, L"L7-SDC descriptor operation is unsupported in v1.");
			return false;
		}

// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((datatype != AcceleratorDatatype::Float32 && datatype != AcceleratorDatatype::Float64 && datatype != AcceleratorDatatype::Int32))
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::UnsupportedDatatype, L"L7-SDC descriptor datatype is unsupported in v1.");
			return false;
		}

		if (shape != AcceleratorShapeKind::Matrix2D || shapeRank != 2 || elementCount == 0)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::UnsupportedShape, L"L7-SDC v1 accepts only non-empty rank-2 matrix descriptors.");
			return false;
		}

		return true;
	}

	bool AcceleratorDescriptorParser::ValidateDescriptorSize(unsigned int descriptorSize, unsigned short sourceRangeCount, unsigned short destinationRangeCount, unsigned short scratchRangeCount, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		failure = std::nullopt;

		unsigned long long rangeBytes = (static_cast<unsigned long long>(sourceRangeCount) + destinationRangeCount + scratchRangeCount) * RangeEntrySize;
		unsigned long long minimumSize = HeaderSize + rangeBytes;
		if (minimumSize > descriptorSize)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorSizeFault, L"L7-SDC descriptor size does not cover the declared inline range tables.");
			return false;
		}

		return true;
	}

	bool AcceleratorDescriptorParser::ValidateRangeCount(unsigned short rangeCount, const std::wstring &rangeKind, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		failure = std::nullopt;
		if (rangeCount > MaxRangeCount)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, std::format(L"L7-SDC {0} range count exceeds the v1 parser limit.", rangeKind));
			return false;
		}

		return true;
	}

	bool AcceleratorDescriptorParser::ValidateAlignment(unsigned short alignmentBytes, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		failure = std::nullopt;
		if (alignmentBytes == 0 || (alignmentBytes & (alignmentBytes - 1)) != 0)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::AlignmentFault, L"L7-SDC alignment requirement must be a non-zero power of two.");
			return false;
		}

		return true;
	}

	bool AcceleratorDescriptorParser::TryReadRanges(std::span<const unsigned char> bytes, unsigned int descriptorSize, unsigned int tableOffset, unsigned short rangeCount, unsigned short alignmentBytes, bool requireNonEmpty, const std::wstring &rangeKind, std::vector<std::shared_ptr<AcceleratorMemoryRange>> &ranges, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		ranges.clear();
		failure = std::nullopt;

		if (rangeCount == 0)
		{
			if (requireNonEmpty)
			{
				failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, std::format(L"L7-SDC descriptor requires at least one {0} range.", rangeKind));
				return false;
			}

			if (tableOffset != 0)
			{
				failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::ReservedFieldFault, std::format(L"L7-SDC {0} range table offset must be zero when the count is zero.", rangeKind));
				return false;
			}

			return true;
		}

		if ((tableOffset & 0x7) != 0)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::AlignmentFault, std::format(L"L7-SDC {0} range table offset must be 8-byte aligned.", rangeKind));
			return false;
		}

		unsigned long long tableBytes = static_cast<unsigned long long>(rangeCount) * RangeEntrySize;
		unsigned long long tableEnd = static_cast<unsigned long long>(tableOffset) + tableBytes;
		if (tableEnd < tableOffset || tableEnd > descriptorSize)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::RangeOverflow, std::format(L"L7-SDC {0} range table overflows the validated descriptor size.", rangeKind));
			return false;
		}

		std::vector<std::shared_ptr<AcceleratorMemoryRange>> decodedRanges;
		decodedRanges.reserve(rangeCount);
		for (int index = 0; index < rangeCount; index++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int entryOffset = checked((int)tableOffset + (index * RangeEntrySize));
			int entryOffset = static_cast<int>(tableOffset) + (index * RangeEntrySize);
			unsigned long long address = ReadUInt64(bytes, entryOffset);
			unsigned long long length = ReadUInt64(bytes, entryOffset + sizeof(unsigned long long));

			if (length == 0)
			{
				failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::ZeroLengthFault, std::format(L"L7-SDC {0} memory ranges must have non-zero length.", rangeKind));
				return false;
			}

			if (address > std::numeric_limits<unsigned long long>::max() - length)
			{
				failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::RangeOverflow, std::format(L"L7-SDC {0} memory range address + length overflows UInt64.", rangeKind));
				return false;
			}

			if ((address % alignmentBytes) != 0 || (length % alignmentBytes) != 0)
			{
				failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::AlignmentFault, std::format(L"L7-SDC {0} memory range does not satisfy the descriptor alignment requirement.", rangeKind));
				return false;
			}

			decodedRanges.push_back(std::make_shared<AcceleratorMemoryRange>(AcceleratorMemoryRange{address, length}));
		}

		ranges = decodedRanges;
		return true;
	}

	bool AcceleratorDescriptorParser::ValidateRangeTableLayout(unsigned int descriptorSize, unsigned int sourceOffset, unsigned short sourceCount, unsigned int destinationOffset, unsigned short destinationCount, unsigned int scratchOffset, unsigned short scratchCount, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		failure = std::nullopt;

		std::vector<std::shared_ptr<RangeTableSpan>> declaredTables;
		declaredTables.reserve(3);
		if (!TryAddRangeTableSpan(declaredTables, L"source", sourceOffset, sourceCount, descriptorSize, failure) || !TryAddRangeTableSpan(declaredTables, L"destination", destinationOffset, destinationCount, descriptorSize, failure) || !TryAddRangeTableSpan(declaredTables, L"scratch", scratchOffset, scratchCount, descriptorSize, failure))
		{
			return false;
		}

		for (int leftIndex = 0; leftIndex < declaredTables.size(); leftIndex++)
		{
			std::shared_ptr<RangeTableSpan> left = declaredTables[leftIndex];
			for (int rightIndex = leftIndex + 1; rightIndex < declaredTables.size(); rightIndex++)
			{
				std::shared_ptr<RangeTableSpan> right = declaredTables[rightIndex];
				if (left->Start < right->End && right->Start < left->End)
				{
					failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, std::format(L"L7-SDC {0} and {1} range tables overlap.", left->Kind, right->Kind));
					return false;
				}
			}
		}

		return true;
	}

	bool AcceleratorDescriptorParser::TryAddRangeTableSpan(std::vector<std::shared_ptr<RangeTableSpan>> &declaredTables, const std::wstring &rangeKind, unsigned int tableOffset, unsigned short rangeCount, unsigned int descriptorSize, std::optional<std::shared_ptr<AcceleratorDescriptorValidationResult>> &failure)
	{
		failure = std::nullopt;
		if (rangeCount == 0)
		{
			return true;
		}

		if (tableOffset < HeaderSize)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::DescriptorDecodeFault, std::format(L"L7-SDC {0} range table must start after the fixed descriptor header.", rangeKind));
			return false;
		}

		unsigned long long tableBytes = static_cast<unsigned long long>(rangeCount) * RangeEntrySize;
		unsigned long long tableEnd = static_cast<unsigned long long>(tableOffset) + tableBytes;
		if (tableEnd < tableOffset || tableEnd > descriptorSize)
		{
			failure = AcceleratorDescriptorValidationResult::Fail(AcceleratorDescriptorFault::RangeOverflow, std::format(L"L7-SDC {0} range table overflows the validated descriptor size.", rangeKind));
			return false;
		}

		declaredTables.push_back(std::make_shared<RangeTableSpan>(rangeKind, tableOffset, tableEnd));
		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool ScratchRangesCoverRequirement(IReadOnlyList<AcceleratorMemoryRange> scratchRanges, ulong scratchRequiredBytes)
	bool AcceleratorDescriptorParser::ScratchRangesCoverRequirement(const std::vector<std::shared_ptr<AcceleratorMemoryRange>> &scratchRanges, unsigned long long scratchRequiredBytes)
	{
		unsigned long long total = 0;
		for (int index = 0; index < scratchRanges.size(); index++)
		{
			unsigned long long next = total + scratchRanges[index]->Length;
			if (next < total)
			{
				return false;
			}

			total = next;
		}

		return total >= scratchRequiredBytes;
	}

	bool AcceleratorDescriptorParser::HasAnyReservedBitSet(std::span<const unsigned char> bytes)
	{
		return ReadUInt32(bytes, FlagsOffset) != 0 || ReadUInt16(bytes, Reserved1Offset) != 0 || ReadUInt16(bytes, Reserved2Offset) != 0 || ReadUInt32(bytes, Reserved3Offset) != 0 || ReadUInt32(bytes, Reserved4Offset) != 0 || ReadUInt64(bytes, Reserved5Offset) != 0;
	}

	unsigned short AcceleratorDescriptorParser::ReadUInt16(std::span<const unsigned char> bytes, int offset)
	{
		return static_cast<unsigned short>(bytes[offset]) | static_cast<unsigned short>(static_cast<unsigned short>(bytes[offset + 1]) << 8);
	}

	unsigned int AcceleratorDescriptorParser::ReadUInt32(std::span<const unsigned char> bytes, int offset)
	{
		return static_cast<unsigned int>(bytes[offset]) |
			(static_cast<unsigned int>(bytes[offset + 1]) << 8) |
			(static_cast<unsigned int>(bytes[offset + 2]) << 16) |
			(static_cast<unsigned int>(bytes[offset + 3]) << 24);
	}

	unsigned long long AcceleratorDescriptorParser::ReadUInt64(std::span<const unsigned char> bytes, int offset)
	{
		return static_cast<unsigned long long>(ReadUInt32(bytes, offset)) |
			(static_cast<unsigned long long>(ReadUInt32(bytes, offset + 4)) << 32);
	}
}
