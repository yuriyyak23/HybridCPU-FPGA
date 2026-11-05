#include "YAKSys_Hybrid_CPU.Core.Execution.DmaStreamCompute.DmaStreamComputeDescriptorParser.h"
#include "../../Pipeline/Safety/SafetyVerifier.Types.h"
#include "../../State/ExecutionState.h"
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

using namespace HybridCPU_ISE::Arch;

namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute
{
	namespace
	{
		std::wstring Dsc2ExtensionTypeName(DmaStreamComputeDsc2ExtensionType type)
		{
		switch (type)
		{
		case DmaStreamComputeDsc2ExtensionType::CapabilityProfile: return L"CapabilityProfile";
		case DmaStreamComputeDsc2ExtensionType::AddressSpace: return L"AddressSpace";
		case DmaStreamComputeDsc2ExtensionType::StridedRange: return L"StridedRange";
		case DmaStreamComputeDsc2ExtensionType::Tile2D: return L"Tile2D";
		case DmaStreamComputeDsc2ExtensionType::ScatterGather: return L"ScatterGather";
		case DmaStreamComputeDsc2ExtensionType::FootprintSummary: return L"FootprintSummary";
		case DmaStreamComputeDsc2ExtensionType::PartialCompletionPolicy: return L"PartialCompletionPolicy";
		case DmaStreamComputeDsc2ExtensionType::VendorNonSemanticMetadata: return L"VendorNonSemanticMetadata";
		default: return std::format(L"{}", static_cast<unsigned short>(type));
		}
		}

		std::wstring Dsc2CapabilityStagesName(DmaStreamComputeDsc2CapabilityStage stages)
		{
			const auto bits = static_cast<unsigned short>(stages);
			std::wstring text;
			const auto append = [&text](std::wstring_view name)
			{
				if (!text.empty()) text += L", ";
				text.append(name);
			};
			if ((bits & static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::ParserKnown)) != 0) append(L"ParserKnown");
			if ((bits & static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::Validation)) != 0) append(L"Validation");
			if ((bits & static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::FootprintNormalization)) != 0) append(L"FootprintNormalization");
			if ((bits & static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::BackendAddressSpace)) != 0) append(L"BackendAddressSpace");
			if ((bits & static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::Execution)) != 0) append(L"Execution");
			if ((bits & static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::CompilerLowering)) != 0) append(L"CompilerLowering");
			const auto knownMask = static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::ParserKnown) |
				static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::Validation) |
				static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::FootprintNormalization) |
				static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::BackendAddressSpace) |
				static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::Execution) |
				static_cast<unsigned short>(DmaStreamComputeDsc2CapabilityStage::CompilerLowering);
			const auto unknownBits = static_cast<unsigned short>(bits & static_cast<unsigned short>(~knownMask));
			if (unknownBits != 0) append(std::format(L"0x{:X}", unknownBits));
			if (text.empty()) return L"None";
			return text;
		}
	}

	bool DmaStreamComputeDescriptorParser::getExecutionEnabled()
	{
		return true;
	}

	std::shared_ptr<DmaStreamComputeStructuralReadResult> DmaStreamComputeDescriptorParser::ReadStructuralOwnerBinding(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference)
	{
		if (descriptorBytes.size() < HeaderSize)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"Descriptor is shorter than the v1 fixed header.");
		}

		if (ReadUInt32(descriptorBytes, MagicOffset) != Magic)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"Descriptor magic does not match DmaStreamCompute v1.");
		}

		unsigned short abiVersion = ReadUInt16(descriptorBytes, AbiVersionOffset);
		if (abiVersion != CurrentAbiVersion)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::UnsupportedAbiVersion, std::format(L"Unsupported DmaStreamCompute descriptor ABI version {}.", abiVersion));
		}

		unsigned short headerSize = ReadUInt16(descriptorBytes, HeaderSizeOffset);
		if (headerSize != HeaderSize)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, std::format(L"Descriptor header size {} does not match v1 header size {}.", headerSize, HeaderSize));
		}

		unsigned int totalSize = ReadUInt32(descriptorBytes, TotalSizeOffset);
		if (totalSize < HeaderSize || totalSize > descriptorBytes.size())
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"Descriptor total size is outside the provided descriptor buffer.");
		}

		unsigned long long descriptorIdentityHash = ReadUInt64(descriptorBytes, DescriptorIdentityHashOffset);
		std::shared_ptr<DmaStreamComputeDescriptorReference> effectiveReference = descriptorReference.has_value() && descriptorReference.value()
			? descriptorReference.value()
			: std::make_shared<DmaStreamComputeDescriptorReference>(DmaStreamComputeDescriptorReference{0, totalSize, descriptorIdentityHash});

		if (effectiveReference->DescriptorSize != 0 && effectiveReference->DescriptorSize < totalSize)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorReferenceLost, L"Descriptor sideband reference does not cover the validated descriptor payload.");
		}

		if (effectiveReference->DescriptorIdentityHash != 0 && effectiveReference->DescriptorIdentityHash != descriptorIdentityHash)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorReferenceLost, L"Descriptor sideband identity hash does not match the validated payload.");
		}

		auto ownerBinding = std::make_shared<DmaStreamComputeOwnerBinding>(DmaStreamComputeOwnerBinding{ReadUInt16(descriptorBytes, OwnerVirtualThreadIdOffset), ReadUInt32(descriptorBytes, OwnerContextIdOffset), ReadUInt32(descriptorBytes, OwnerCoreIdOffset), ReadUInt32(descriptorBytes, OwnerPodIdOffset), ReadUInt64(descriptorBytes, OwnerDomainTagOffset), ReadUInt32(descriptorBytes, DeviceIdOffset)});

		return DmaStreamComputeStructuralReadResult::Valid(effectiveReference, descriptorIdentityHash, totalSize, ownerBinding);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, System.Nullable<DmaStreamComputeDescriptorReference> descriptorReference = null, DmaStreamComputeTelemetryCounters? telemetry = null)
	std::shared_ptr<DmaStreamComputeValidationResult> DmaStreamComputeDescriptorParser::Parse(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		if (telemetry != nullptr)
		{
			telemetry->RecordDescriptorParseAttempt();
		}
		std::shared_ptr<DmaStreamComputeStructuralReadResult> structuralRead = ReadStructuralOwnerBinding(descriptorBytes, descriptorReference);
		if (!structuralRead->getIsStructuralDescriptorReadAccepted())
		{
			return Fail(structuralRead->Fault, structuralRead->Message, telemetry);
		}

		return Fail(DmaStreamComputeValidationFault::OwnerDomainFault, L"DmaStreamCompute descriptor cannot be accepted without an explicit owner/domain guard decision.", telemetry);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static DmaStreamComputeValidationResult Parse(ReadOnlySpan<byte> descriptorBytes, DmaStreamComputeOwnerGuardDecision ownerGuardDecision, System.Nullable<DmaStreamComputeDescriptorReference> descriptorReference = null, DmaStreamComputeTelemetryCounters? telemetry = null)
	std::shared_ptr<DmaStreamComputeValidationResult> DmaStreamComputeDescriptorParser::Parse(std::span<const unsigned char> descriptorBytes, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		if (telemetry != nullptr)
		{
			telemetry->RecordDescriptorParseAttempt();
		}
		std::shared_ptr<DmaStreamComputeStructuralReadResult> structuralRead = ReadStructuralOwnerBinding(descriptorBytes, descriptorReference);
		if (!structuralRead->getIsStructuralDescriptorReadAccepted())
		{
			return Fail(structuralRead->Fault, structuralRead->Message, telemetry);
		}

		std::shared_ptr<DmaStreamComputeOwnerBinding> structuralOwnerBinding = structuralRead->RequireOwnerBindingForGuard();
		if (!ownerGuardDecision->IsAllowed)
		{
			if (telemetry != nullptr)
			{
				telemetry->RecordOwnerGuardRejected(structuralOwnerBinding, ownerGuardDecision);
			}
			return DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::OwnerDomainFault, ownerGuardDecision->Message);
		}

		if (ownerGuardDecision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || ownerGuardDecision->LegalityDecision.getAttemptedReplayCertificateReuse())
		{
			return Fail(DmaStreamComputeValidationFault::OwnerDomainFault, L"DmaStreamCompute owner guard decision must come from the guard plane before replay or certificate reuse.", telemetry);
		}

		if (ownerGuardDecision->DescriptorOwnerBinding == nullptr || !(*ownerGuardDecision->DescriptorOwnerBinding == *structuralOwnerBinding))
		{
			return Fail(DmaStreamComputeValidationFault::OwnerDomainFault, L"DmaStreamCompute owner guard decision does not match structurally-read descriptor owner fields.", telemetry);
		}

		unsigned short abiVersion = ReadUInt16(descriptorBytes, AbiVersionOffset);
		unsigned short headerSize = ReadUInt16(descriptorBytes, HeaderSizeOffset);
		unsigned int totalSize = structuralRead->TotalSize;

		if (HasAnyReservedBitSet(descriptorBytes))
		{
			return Fail(DmaStreamComputeValidationFault::ReservedFieldFault, L"DmaStreamCompute v1 descriptor reserved fields and flags must be zero.", telemetry);
		}

		DmaStreamComputeOperationKind operation;
		if (!TryDecodeOperation(ReadUInt16(descriptorBytes, OperationOffset), operation))
		{
			return Fail(DmaStreamComputeValidationFault::UnsupportedOperation, L"DmaStreamCompute descriptor operation is unsupported in v1.", telemetry);
		}

		DmaStreamComputeElementType elementType;
		int elementSize;
		if (!TryDecodeElementType(ReadUInt16(descriptorBytes, ElementTypeOffset), elementType, elementSize))
		{
			return Fail(DmaStreamComputeValidationFault::UnsupportedElementType, L"DmaStreamCompute descriptor element type is unsupported in v1.", telemetry);
		}

		DmaStreamComputeShapeKind shape;
		if (!TryDecodeShape(ReadUInt16(descriptorBytes, ShapeOffset), operation, shape))
		{
			return Fail(DmaStreamComputeValidationFault::UnsupportedShape, L"DmaStreamCompute descriptor shape is unsupported or not legal for the operation.", telemetry);
		}

		if (ReadUInt16(descriptorBytes, RangeEncodingOffset) != static_cast<unsigned short>(DmaStreamComputeRangeEncoding::InlineContiguous))
		{
			return Fail(DmaStreamComputeValidationFault::UnsupportedShape, L"Only inline contiguous range tables are accepted by the v1 parser.", telemetry);
		}

		if (ReadUInt16(descriptorBytes, PartialCompletionPolicyOffset) != static_cast<unsigned short>(DmaStreamComputePartialCompletionPolicy::AllOrNone))
		{
			return Fail(DmaStreamComputeValidationFault::ReservedFieldFault, L"DmaStreamCompute v1 requires all-or-none partial completion policy.", telemetry);
		}

		unsigned short sourceRangeCount = ReadUInt16(descriptorBytes, SourceRangeCountOffset);
		unsigned short destinationRangeCount = ReadUInt16(descriptorBytes, DestinationRangeCountOffset);
		unsigned short expectedSourceRangeCount = GetExpectedSourceRangeCount(operation);
		if (sourceRangeCount != expectedSourceRangeCount || destinationRangeCount == 0)
		{
			return Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"Descriptor range counts do not match the v1 operation contract.", telemetry);
		}

		unsigned int sourceRangeOffset = ReadUInt32(descriptorBytes, SourceRangeTableOffset);
		unsigned int destinationRangeOffset = ReadUInt32(descriptorBytes, DestinationRangeTableOffset);
		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> readRanges;
		std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> readFailure;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryReadRanges(descriptorBytes, totalSize, sourceRangeOffset, sourceRangeCount, elementSize, out IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, out System.Nullable<DmaStreamComputeValidationResult> readFailure))
		if (!TryReadRanges(descriptorBytes, totalSize, sourceRangeOffset, sourceRangeCount, elementSize, readRanges, readFailure))
		{
			auto failure = readFailure.has_value() ? readFailure.value() : nullptr;
			if (telemetry != nullptr)
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: telemetry.RecordDescriptorRejected(readFailure!.Fault);
				if (failure) telemetry->RecordDescriptorRejected(failure->Fault);
			}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return readFailure!;
			return failure ? failure : DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"DmaStreamCompute range parsing failed without a failure record.");
		}

		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> writeRanges;
		std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> writeFailure;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: if (!TryReadRanges(descriptorBytes, totalSize, destinationRangeOffset, destinationRangeCount, elementSize, out IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges, out System.Nullable<DmaStreamComputeValidationResult> writeFailure))
		if (!TryReadRanges(descriptorBytes, totalSize, destinationRangeOffset, destinationRangeCount, elementSize, writeRanges, writeFailure))
		{
			auto failure = writeFailure.has_value() ? writeFailure.value() : nullptr;
			if (telemetry != nullptr)
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: telemetry.RecordDescriptorRejected(writeFailure!.Fault);
				if (failure) telemetry->RecordDescriptorRejected(failure->Fault);
			}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return writeFailure!;
			return failure ? failure : DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"DmaStreamCompute range parsing failed without a failure record.");
		}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<DmaStreamComputeMemoryRange> normalizedReadRanges = NormalizeMemoryRanges(readRanges);
		const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> normalizedReadRanges = NormalizeMemoryRanges(readRanges);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<DmaStreamComputeMemoryRange> normalizedWriteRanges = NormalizeMemoryRanges(writeRanges);
		const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> normalizedWriteRanges = NormalizeMemoryRanges(writeRanges);
		DmaStreamComputeAliasPolicy aliasPolicy;
		std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> aliasFailure;
		if (!TryValidateAliasPolicy(readRanges, writeRanges, aliasPolicy, aliasFailure))
		{
			auto failure = aliasFailure.has_value() ? aliasFailure.value() : nullptr;
			if (telemetry != nullptr)
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: telemetry.RecordDescriptorRejected(aliasFailure!.Fault);
				if (failure) telemetry->RecordDescriptorRejected(failure->Fault);
			}
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return aliasFailure!;
			return failure ? failure : DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::AliasOverlapFault, L"DmaStreamCompute alias validation failed without a failure record.");
		}

		auto toValues = [](const auto& ranges) {
			std::vector<DmaStreamComputeMemoryRange> result; result.reserve(ranges.size());
			for (const auto& range : ranges) result.push_back(*range);
			return result;
		};
		auto descriptor = std::make_shared<DmaStreamComputeDescriptor>(DmaStreamComputeDescriptor{
			.DescriptorReference = *structuralRead->DescriptorReference,
			.AbiVersion = abiVersion, .HeaderSize = headerSize, .TotalSize = totalSize,
			.DescriptorIdentityHash = structuralRead->DescriptorIdentityHash,
			.CertificateInputHash = ReadUInt64(descriptorBytes, CertificateInputHashOffset),
			.Operation = operation, .ElementType = elementType, .Shape = shape,
			.RangeEncoding = DmaStreamComputeRangeEncoding::InlineContiguous,
			.PartialCompletionPolicy = DmaStreamComputePartialCompletionPolicy::AllOrNone,
			.OwnerBinding = structuralOwnerBinding, .OwnerGuardDecision = ownerGuardDecision,
			.ReadMemoryRanges = toValues(readRanges), .NormalizedReadMemoryRanges = toValues(normalizedReadRanges),
			.WriteMemoryRanges = toValues(writeRanges), .NormalizedWriteMemoryRanges = toValues(normalizedWriteRanges),
			.AliasPolicy = aliasPolicy,
			.NormalizedFootprintHash = ComputeNormalizedFootprintHash(operation, elementType, shape, aliasPolicy, normalizedReadRanges, normalizedWriteRanges)});

		return Valid(descriptor, telemetry);
	}

	std::shared_ptr<DmaStreamComputeValidationResult> DmaStreamComputeDescriptorParser::TryDecodeRawVliwCarrier(VLIW_Instruction const instruction, int slotIndex)
	{
		std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> nativeFailure;
		if (TryValidateNativeVliwCarrier(instruction, slotIndex, false, nativeFailure))
		{
			return DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::DescriptorCarrierDecodeFault, L"DmaStreamCompute raw VLIW carrier unexpectedly validated without a typed descriptor sideband.");
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return nativeFailure!;
		return nativeFailure.has_value() ? nativeFailure.value() : DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::DescriptorCarrierDecodeFault, L"DmaStreamCompute carrier validation failed without a failure record.");
	}

	bool DmaStreamComputeDescriptorParser::TryValidateNativeVliwCarrier(VLIW_Instruction const instruction, int slotIndex, bool hasDescriptorSideband, std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> &failure)
	{
		if ((instruction.getWord3() & VLIW_Instruction::RetiredPolicyGapMask) != 0)
		{
			failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::DescriptorCarrierDecodeFault, std::format(L"Slot {}: word3[50] is a retired policy gap and cannot carry a descriptor ABI bit.", slotIndex));
			return false;
		}

		if (instruction.getReserved() != 0)
		{
			failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::DescriptorCarrierDecodeFault, std::format(L"Slot {}: word0[47:40] is reserved and cannot carry an unversioned descriptor ABI.", slotIndex));
			return false;
		}

		if (instruction.getVirtualThreadId() != 0)
		{
			failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::DescriptorCarrierDecodeFault, std::format(L"Slot {}: word3[49:48] VirtualThreadId is a transport hint, not descriptor owner authority.", slotIndex));
			return false;
		}

		if (!hasDescriptorSideband)
		{
			failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::DescriptorCarrierDecodeFault, L"DmaStreamCompute descriptor references are accepted only through typed decoded sideband.");
			return false;
		}

		failure = std::nullopt;
		return true;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static DmaStreamComputeValidationResult Fail(DmaStreamComputeValidationFault fault, string message, DmaStreamComputeTelemetryCounters? telemetry)
	std::shared_ptr<DmaStreamComputeValidationResult> DmaStreamComputeDescriptorParser::Fail(DmaStreamComputeValidationFault fault, const std::wstring &message, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		if (telemetry != nullptr)
		{
			telemetry->RecordDescriptorRejected(fault);
		}
		return DmaStreamComputeValidationResult::Fail(fault, message);
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static DmaStreamComputeValidationResult Valid(DmaStreamComputeDescriptor descriptor, DmaStreamComputeTelemetryCounters? telemetry)
	std::shared_ptr<DmaStreamComputeValidationResult> DmaStreamComputeDescriptorParser::Valid(std::shared_ptr<DmaStreamComputeDescriptor> descriptor, std::shared_ptr<DmaStreamComputeTelemetryCounters> telemetry)
	{
		if (telemetry != nullptr)
		{
			telemetry->RecordDescriptorAccepted(descriptor);
		}
		return DmaStreamComputeValidationResult::Valid(descriptor);
	}

	bool DmaStreamComputeDescriptorParser::HasAnyReservedBitSet(std::span<const unsigned char> bytes)
	{
		return ReadUInt32(bytes, FlagsOffset) != 0 || ReadUInt64(bytes, Reserved0Offset) != 0 || ReadUInt16(bytes, MaskRangeCountOffset) != 0 || ReadUInt16(bytes, AccumulatorRangeCountOffset) != 0 || ReadUInt16(bytes, RoundingModeOffset) != 0 || ReadUInt16(bytes, Reserved1Offset) != 0 || ReadUInt64(bytes, Reserved2Offset) != 0 || ReadUInt32(bytes, MaskRangeTableOffset) != 0 || ReadUInt32(bytes, AccumulatorRangeTableOffset) != 0 || ReadUInt64(bytes, Reserved3Offset) != 0 || ReadUInt64(bytes, Reserved4Offset) != 0;
	}

	bool DmaStreamComputeDescriptorParser::TryDecodeOperation(unsigned short rawOperation, DmaStreamComputeOperationKind &operation)
	{
		operation = static_cast<DmaStreamComputeOperationKind>(rawOperation);
		return (operation == DmaStreamComputeOperationKind::Copy || operation == DmaStreamComputeOperationKind::Add || operation == DmaStreamComputeOperationKind::Mul || operation == DmaStreamComputeOperationKind::Fma || operation == DmaStreamComputeOperationKind::Reduce);
	}

	bool DmaStreamComputeDescriptorParser::TryDecodeElementType(unsigned short rawElementType, DmaStreamComputeElementType &elementType, int &elementSize)
	{
		elementType = static_cast<DmaStreamComputeElementType>(rawElementType);
		int temp_var38;
		switch (elementType)
		{
			case DmaStreamComputeElementType::UInt8:
				temp_var38 = 1;
				break;
			case DmaStreamComputeElementType::UInt16:
				temp_var38 = 2;
				break;
			case DmaStreamComputeElementType::UInt32:
				temp_var38 = 4;
				break;
			case DmaStreamComputeElementType::UInt64:
				temp_var38 = 8;
				break;
			case DmaStreamComputeElementType::Float32:
				temp_var38 = 4;
				break;
			case DmaStreamComputeElementType::Float64:
				temp_var38 = 8;
				break;
			default:
				temp_var38 = 0;
				break;
		}
		elementSize = temp_var38;

		return elementSize != 0;
	}

	bool DmaStreamComputeDescriptorParser::TryDecodeShape(unsigned short rawShape, DmaStreamComputeOperationKind operation, DmaStreamComputeShapeKind &shape)
	{
		shape = static_cast<DmaStreamComputeShapeKind>(rawShape);
		return shape == DmaStreamComputeShapeKind::Contiguous1D || (operation == DmaStreamComputeOperationKind::Reduce && shape == DmaStreamComputeShapeKind::FixedReduce);
	}

	unsigned short DmaStreamComputeDescriptorParser::GetExpectedSourceRangeCount(DmaStreamComputeOperationKind operation)
	{
		int temp_var39;
		switch (operation)
		{
			case DmaStreamComputeOperationKind::Copy:
				temp_var39 = 1;
				break;
			case DmaStreamComputeOperationKind::Add:
				temp_var39 = 2;
				break;
			case DmaStreamComputeOperationKind::Mul:
				temp_var39 = 2;
				break;
			case DmaStreamComputeOperationKind::Fma:
				temp_var39 = 3;
				break;
			case DmaStreamComputeOperationKind::Reduce:
				temp_var39 = 1;
				break;
			default:
				temp_var39 = 0;
				break;
		}
		return temp_var39;
	}

	bool DmaStreamComputeDescriptorParser::TryReadRanges(std::span<const unsigned char> bytes, unsigned int totalSize, unsigned int tableOffset, unsigned short rangeCount, int elementSize, std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges, std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> &failure)
	{
		ranges.clear();
		failure = std::nullopt;

		if (rangeCount == 0)
		{
			return true;
		}

		if ((tableOffset & 0x7) != 0)
		{
			failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::AlignmentFault, L"Descriptor range table offset must be 8-byte aligned.");
			return false;
		}

		unsigned long long tableBytes = static_cast<unsigned long long>(rangeCount) * RangeEntrySize;
		unsigned long long tableEnd = static_cast<unsigned long long>(tableOffset) + tableBytes;
		if (tableEnd < tableOffset || tableEnd > totalSize)
		{
			failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"Descriptor range table overflows the validated descriptor size.");
			return false;
		}

		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> normalized(rangeCount);
		for (int i = 0; i < rangeCount; i++)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int entryOffset = checked((int)tableOffset + (i * RangeEntrySize));
			int entryOffset = static_cast<int>(tableOffset) + (i * RangeEntrySize);
			unsigned long long address = ReadUInt64(bytes, entryOffset);
			unsigned long long length = ReadUInt64(bytes, entryOffset + sizeof(unsigned long long));

			if (length == 0)
			{
				failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::ZeroLengthFault, L"DmaStreamCompute memory ranges must have non-zero length.");
				return false;
			}

			if (address > std::numeric_limits<unsigned long long>::max() - length)
			{
				failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DmaStreamCompute memory range address + length overflows UInt64.");
				return false;
			}

			unsigned long long alignment = static_cast<unsigned long long>(elementSize);
			if ((address % alignment) != 0 || (length % alignment) != 0)
			{
				failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::AlignmentFault, L"DmaStreamCompute memory range is not aligned to the element size.");
				return false;
			}

			normalized[i] = std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{address, length});
		}

		ranges = normalized;
		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<DmaStreamComputeMemoryRange> NormalizeMemoryRanges(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
	const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> DmaStreamComputeDescriptorParser::NormalizeMemoryRanges(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges)
	{
		if (ranges.empty())
		{
			return {};
		}

		if (ranges.size() == 1)
		{
			return ranges;
		}

		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> sorted(ranges.begin(), ranges.end());
		std::sort(sorted.begin(), sorted.end(), [](const auto& left, const auto& right)
		{
				if (!left || !right) return static_cast<bool>(left) > static_cast<bool>(right);
				return left->Address != right->Address ? left->Address < right->Address : left->Length < right->Length;
		});

		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> merged;
		merged.reserve(sorted.size());
		unsigned long long currentStart = sorted[0]->Address;
		unsigned long long currentEnd = sorted[0]->Address + sorted[0]->Length;
		for (int i = 1; i < sorted.size(); i++)
		{
			std::shared_ptr<DmaStreamComputeMemoryRange> next = sorted[i];
			unsigned long long nextEnd = next->Address + next->Length;
			if (next->Address <= currentEnd)
			{
				if (nextEnd > currentEnd)
				{
					currentEnd = nextEnd;
				}

				continue;
			}

			merged.push_back(std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{currentStart, currentEnd - currentStart}));
			currentStart = next->Address;
			currentEnd = nextEnd;
		}

		merged.push_back(std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{currentStart, currentEnd - currentStart}));
		return merged;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool TryValidateAliasPolicy(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges, out DmaStreamComputeAliasPolicy aliasPolicy, out System.Nullable<DmaStreamComputeValidationResult> failure)
	bool DmaStreamComputeDescriptorParser::TryValidateAliasPolicy(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &readRanges, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &writeRanges, DmaStreamComputeAliasPolicy &aliasPolicy, std::optional<std::shared_ptr<DmaStreamComputeValidationResult>> &failure)
	{
		aliasPolicy = DmaStreamComputeAliasPolicy::Disjoint;
		failure = std::nullopt;

		if (HasAnyInternalOverlap(writeRanges))
		{
			failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::AliasOverlapFault, L"DmaStreamCompute destination ranges overlap; v1 requires explicit non-overlapping writes.");
			return false;
		}

		for (int writeIndex = 0; writeIndex < writeRanges.size(); writeIndex++)
		{
			std::shared_ptr<DmaStreamComputeMemoryRange> writeRange = writeRanges[writeIndex];
			for (int readIndex = 0; readIndex < readRanges.size(); readIndex++)
			{
				std::shared_ptr<DmaStreamComputeMemoryRange> readRange = readRanges[readIndex];
				if (!RangesOverlap(readRange, writeRange))
				{
					continue;
				}

				if (RangesEqual(readRange, writeRange))
				{
					aliasPolicy = DmaStreamComputeAliasPolicy::ExactInPlaceSnapshot;
					continue;
				}

				failure = DmaStreamComputeValidationResult::Fail(DmaStreamComputeValidationFault::AliasOverlapFault, L"DmaStreamCompute source/destination overlap is only legal for exact in-place snapshot ranges.");
				return false;
			}
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool HasAnyInternalOverlap(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
	bool DmaStreamComputeDescriptorParser::HasAnyInternalOverlap(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges)
	{
		if (ranges.empty() || ranges.size() <= 1)
		{
			return false;
		}

		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> sorted(ranges.begin(), ranges.end());
		std::sort(sorted.begin(), sorted.end(), [](const auto& left, const auto& right)
		{
				if (!left || !right) return static_cast<bool>(left) > static_cast<bool>(right);
				return left->Address != right->Address ? left->Address < right->Address : left->Length < right->Length;
		});

		unsigned long long previousEnd = sorted[0]->Address + sorted[0]->Length;
		for (int i = 1; i < sorted.size(); i++)
		{
			if (sorted[i]->Address < previousEnd)
			{
				return true;
			}

			unsigned long long currentEnd = sorted[i]->Address + sorted[i]->Length;
			if (currentEnd > previousEnd)
			{
				previousEnd = currentEnd;
			}
		}

		return false;
	}

	bool DmaStreamComputeDescriptorParser::RangesOverlap(std::shared_ptr<DmaStreamComputeMemoryRange> left, std::shared_ptr<DmaStreamComputeMemoryRange> right)
	{
		unsigned long long leftEnd = left->Address + left->Length;
		unsigned long long rightEnd = right->Address + right->Length;
		return left->Address < rightEnd && right->Address < leftEnd;
	}

	bool DmaStreamComputeDescriptorParser::RangesEqual(std::shared_ptr<DmaStreamComputeMemoryRange> left, std::shared_ptr<DmaStreamComputeMemoryRange> right)
	{
		return left->Address == right->Address && left->Length == right->Length;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static ulong ComputeNormalizedFootprintHash(DmaStreamComputeOperationKind operation, DmaStreamComputeElementType elementType, DmaStreamComputeShapeKind shape, DmaStreamComputeAliasPolicy aliasPolicy, IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges)
	unsigned long long DmaStreamComputeDescriptorParser::ComputeNormalizedFootprintHash(DmaStreamComputeOperationKind operation, DmaStreamComputeElementType elementType, DmaStreamComputeShapeKind shape, DmaStreamComputeAliasPolicy aliasPolicy, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &readRanges, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &writeRanges)
	{
		constexpr unsigned long long offsetBasis = 14695981039346656037ULL;
		constexpr unsigned long long prime = 1099511628211ULL;

		unsigned long long hash = offsetBasis;
		auto Add = [&](unsigned long long value) { hash ^= value; hash *= prime; };
		auto AddRanges = [&](const auto& ranges) { Add(ranges.size()); for (const auto& range : ranges) { Add(range->Address); Add(range->Length); } };
		Add(static_cast<unsigned short>(operation));
		Add(static_cast<unsigned short>(elementType));
		Add(static_cast<unsigned short>(shape));
		Add(static_cast<unsigned short>(aliasPolicy));
		AddRanges(readRanges);
		AddRanges(writeRanges);
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

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		void AddRanges(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
	//			{
	//				Add((ulong)ranges.Count);
	//				for (int i = 0; i < ranges.Count; i++)
	//				{
	//					Add(ranges[i].Address);
	//					Add(ranges[i].size());
	//				}
	//			}
	}

	unsigned short DmaStreamComputeDescriptorParser::ReadUInt16(std::span<const unsigned char> bytes, int offset)
	{
		return static_cast<unsigned short>(bytes[static_cast<std::size_t>(offset)] |
			(static_cast<unsigned short>(bytes[static_cast<std::size_t>(offset + 1)]) << 8));
	}

	unsigned int DmaStreamComputeDescriptorParser::ReadUInt32(std::span<const unsigned char> bytes, int offset)
	{
		return static_cast<unsigned int>(bytes[static_cast<std::size_t>(offset)]) |
			(static_cast<unsigned int>(bytes[static_cast<std::size_t>(offset + 1)]) << 8) |
			(static_cast<unsigned int>(bytes[static_cast<std::size_t>(offset + 2)]) << 16) |
			(static_cast<unsigned int>(bytes[static_cast<std::size_t>(offset + 3)]) << 24);
	}

	unsigned long long DmaStreamComputeDescriptorParser::ReadUInt64(std::span<const unsigned char> bytes, int offset)
	{
		unsigned long long value = 0;
		for (int byteIndex = 0; byteIndex < 8; ++byteIndex)
		{
			value |= static_cast<unsigned long long>(bytes[static_cast<std::size_t>(offset + byteIndex)]) << (byteIndex * 8);
		}
		return value;
	}

	std::shared_ptr<DmaStreamComputeStructuralReadResult> DmaStreamComputeDescriptorParser::ReadDsc2StructuralOwnerBinding(std::span<const unsigned char> descriptorBytes, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference)
	{
		if (descriptorBytes.size() < Dsc2HeaderSize)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"DSC2 descriptor is shorter than the fixed parser-only header.");
		}

		if (ReadUInt32(descriptorBytes, Dsc2MagicOffset) != Dsc2Magic)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"Descriptor magic does not match DmaStreamCompute DSC2.");
		}

		unsigned short majorVersion = ReadUInt16(descriptorBytes, Dsc2MajorVersionOffset);
		if (majorVersion != Dsc2MajorVersion)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::UnsupportedAbiVersion, std::format(L"Unsupported DmaStreamCompute DSC2 major version {}.", majorVersion));
		}

		unsigned short headerSize = ReadUInt16(descriptorBytes, Dsc2HeaderSizeOffset);
		if (headerSize != Dsc2HeaderSize)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, std::format(L"DSC2 descriptor header size {} does not match parser-only header size {}.", headerSize, Dsc2HeaderSize));
		}

		unsigned int totalSize = ReadUInt32(descriptorBytes, Dsc2TotalSizeOffset);
		if (totalSize < Dsc2HeaderSize || totalSize > descriptorBytes.size())
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorDecodeFault, L"DSC2 descriptor total size is outside the provided descriptor buffer.");
		}

		unsigned long long descriptorIdentityHash = ReadUInt64(descriptorBytes, Dsc2DescriptorIdentityHashOffset);
		std::shared_ptr<DmaStreamComputeDescriptorReference> effectiveReference = descriptorReference.has_value() && descriptorReference.value()
			? descriptorReference.value()
			: std::make_shared<DmaStreamComputeDescriptorReference>(DmaStreamComputeDescriptorReference{0, totalSize, descriptorIdentityHash});

		if (effectiveReference->DescriptorSize != 0 && effectiveReference->DescriptorSize < totalSize)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorReferenceLost, L"DSC2 descriptor sideband reference does not cover the validated descriptor payload.");
		}

		if (effectiveReference->DescriptorIdentityHash != 0 && effectiveReference->DescriptorIdentityHash != descriptorIdentityHash)
		{
			return DmaStreamComputeStructuralReadResult::Fail(DmaStreamComputeValidationFault::DescriptorReferenceLost, L"DSC2 descriptor sideband identity hash does not match the validated payload.");
		}

		auto ownerBinding = std::make_shared<DmaStreamComputeOwnerBinding>(DmaStreamComputeOwnerBinding{ReadUInt16(descriptorBytes, Dsc2OwnerVirtualThreadIdOffset), ReadUInt32(descriptorBytes, Dsc2OwnerContextIdOffset), ReadUInt32(descriptorBytes, Dsc2OwnerCoreIdOffset), ReadUInt32(descriptorBytes, Dsc2OwnerPodIdOffset), ReadUInt64(descriptorBytes, Dsc2OwnerDomainTagOffset), ReadUInt32(descriptorBytes, Dsc2DeviceIdOffset)});

		return DmaStreamComputeStructuralReadResult::Valid(effectiveReference, descriptorIdentityHash, totalSize, ownerBinding);
	}

	std::shared_ptr<DmaStreamComputeDsc2ValidationResult> DmaStreamComputeDescriptorParser::ParseDsc2ParserOnly(std::span<const unsigned char> descriptorBytes, std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> capabilitySet, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<DmaStreamComputeDescriptorReference>> descriptorReference)
	{
		ArgumentNullException::ThrowIfNull(capabilitySet);

		std::shared_ptr<DmaStreamComputeStructuralReadResult> structuralRead = ReadDsc2StructuralOwnerBinding(descriptorBytes, descriptorReference);
		if (!structuralRead->getIsStructuralDescriptorReadAccepted())
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(structuralRead->Fault, structuralRead->Message);
		}

		std::shared_ptr<DmaStreamComputeOwnerBinding> structuralOwnerBinding = structuralRead->RequireOwnerBindingForGuard();
		std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> ownerFailure;
		if (!ValidateOwnerGuard(structuralOwnerBinding, ownerGuardDecision, ownerFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return ownerFailure!;
			return ownerFailure.has_value() ? ownerFailure.value() : DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::OwnerDomainFault, L"DSC2 owner guard failed without a failure record.");
		}

		if (ReadUInt16(descriptorBytes, Dsc2MinorVersionOffset) != Dsc2MinorVersion)
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedAbiVersion, L"DSC2 parser-only support accepts only ABI minor version 0.");
		}

		if (HasAnyDsc2ReservedHeaderBitSet(descriptorBytes))
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::ReservedFieldFault, L"DSC2 parser-only header reserved fields and descriptor flags must be zero.");
		}

		unsigned short parserStatusRaw = ReadUInt16(descriptorBytes, Dsc2ParserStatusOffset);
		DmaStreamComputeDsc2ParserStatus parserStatus = static_cast<DmaStreamComputeDsc2ParserStatus>(parserStatusRaw);
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((parserStatus != DmaStreamComputeDsc2ParserStatus::ParserOnly && parserStatus != DmaStreamComputeDsc2ParserStatus::ExecutionDisabled))
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::ExecutionDisabled, L"DSC2 Phase07 accepts only parser-only or execution-disabled descriptors.");
		}

		DmaStreamComputeDsc2AddressSpaceKind addressSpace;
		if (!TryDecodeDsc2AddressSpace(ReadUInt16(descriptorBytes, Dsc2AddressSpaceSummaryOffset), addressSpace))
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AddressSpaceFault, L"DSC2 address-space summary is unsupported or reserved.");
		}

		unsigned int extensionTableOffset = ReadUInt32(descriptorBytes, Dsc2ExtensionTableOffsetOffset);
		unsigned short extensionCount = ReadUInt16(descriptorBytes, Dsc2ExtensionCountOffset);
		unsigned int extensionTableByteSize = ReadUInt32(descriptorBytes, Dsc2ExtensionTableByteSizeOffset);
		std::wstring extensionTableFailure;
		if (!ValidateExtensionTable(structuralRead->TotalSize, extensionTableOffset, extensionCount, extensionTableByteSize, extensionTableFailure))
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, extensionTableFailure);
		}

		std::vector<std::shared_ptr<DmaStreamComputeDsc2ParsedExtension>> parsedExtensions;
		parsedExtensions.reserve(extensionCount);
		std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder = std::make_shared<Dsc2FootprintBuilder>();
		std::optional<std::shared_ptr<Dsc2FootprintSummary>> footprintSummary;
		bool hasAddressSpaceExtension = false;
		unsigned long long mappingEpoch = ReadUInt64(descriptorBytes, Dsc2MappingEpochOffset);

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int cursor = checked((int)extensionTableOffset);
		int cursor = static_cast<int>(extensionTableOffset);
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: int tableEnd = checked(cursor + (int)extensionTableByteSize);
		int tableEnd = cursor + static_cast<int>(extensionTableByteSize);
		for (int extensionIndex = 0; extensionIndex < extensionCount; extensionIndex++)
		{
			std::shared_ptr<Dsc2ExtensionHeader> header;
			std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> headerFailure;
			if (!TryParseExtensionHeader(descriptorBytes, cursor, tableEnd, header, headerFailure))
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return headerFailure!;
				return headerFailure.has_value() ? headerFailure.value() : DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 extension header parsing failed without a failure record.");
			}

			std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> extensionFailure;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: if (!TryHandleExtension(descriptorBytes.subspan(cursor + Dsc2ExtensionBlockHeaderSize, checked((int)header.ByteLength - Dsc2ExtensionBlockHeaderSize)), header, structuralOwnerBinding, addressSpace, capabilitySet, footprintBuilder, ref footprintSummary, ref hasAddressSpaceExtension, ref mappingEpoch, parsedExtensions, out System.Nullable<DmaStreamComputeDsc2ValidationResult> extensionFailure))
			if (!TryHandleExtension(descriptorBytes.subspan(cursor + Dsc2ExtensionBlockHeaderSize, static_cast<int>(header->ByteLength) - Dsc2ExtensionBlockHeaderSize), header, structuralOwnerBinding, addressSpace, capabilitySet, footprintBuilder, footprintSummary, hasAddressSpaceExtension, mappingEpoch, parsedExtensions, extensionFailure))
			{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return extensionFailure!;
				return extensionFailure.has_value() ? extensionFailure.value() : DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 extension validation failed without a failure record.");
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'checked' in this context:
// ORIGINAL LINE: cursor += checked((int)header.ByteLength);
			cursor += static_cast<int>(header->ByteLength);
		}

		if (cursor != tableEnd)
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 extension table byte size does not match the parsed extension block stream.");
		}

		if (addressSpace == DmaStreamComputeDsc2AddressSpaceKind::IommuTranslated && !hasAddressSpaceExtension)
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AddressSpaceFault, L"DSC2 IOMMU-translated address-space selection requires an explicit AddressSpace extension.");
		}

		std::optional<std::shared_ptr<DmaStreamComputeDsc2FootprintModel>> footprint;
		std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> footprintFailure;
		if (!footprintBuilder->TryBuild(footprintSummary, footprint, footprintFailure))
		{
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: return footprintFailure!;
			return footprintFailure.has_value() ? footprintFailure.value() : DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnderApproximatedFootprintFault, L"DSC2 footprint construction failed without a failure record.");
		}

		unsigned long long headerFootprintHash = ReadUInt64(descriptorBytes, Dsc2NormalizedFootprintHashOffset);
	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: if (headerFootprintHash != 0 && headerFootprintHash != footprint!.NormalizedFootprintHash)
		if (!footprint.has_value() || !footprint.value())
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnderApproximatedFootprintFault, L"DSC2 footprint construction returned no footprint.");
		if (headerFootprintHash != 0 && headerFootprintHash != footprint.value()->NormalizedFootprintHash)
		{
			return DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnderApproximatedFootprintFault, L"DSC2 header normalized footprint hash does not match deterministic parser-only normalization.");
		}

	auto descriptor = std::make_shared<DmaStreamComputeDsc2Descriptor>(DmaStreamComputeDsc2Descriptor{
			.DescriptorReference = *structuralRead->DescriptorReference,
			.MajorVersion = Dsc2MajorVersion, .MinorVersion = Dsc2MinorVersion,
			.HeaderSize = Dsc2HeaderSize, .TotalSize = structuralRead->TotalSize,
			.DescriptorIdentityHash = structuralRead->DescriptorIdentityHash,
			.CapabilitySetHash = ReadUInt64(descriptorBytes, Dsc2CapabilitySetHashOffset),
			.OwnerBinding = structuralOwnerBinding, .OwnerGuardDecision = ownerGuardDecision,
			.ParserStatus = parserStatus,
			.ExecutionState = DmaStreamComputeDsc2ExecutionState::ParserOnlyExecutionDisabled,
			.AddressSpace = addressSpace, .MappingEpoch = mappingEpoch,
			.CapabilitySet = capabilitySet, .Extensions = parsedExtensions,
			.NormalizedFootprint = footprint.value()});

		return DmaStreamComputeDsc2ValidationResult::Accepted(descriptor);
	}

	bool DmaStreamComputeDescriptorParser::ValidateOwnerGuard(std::shared_ptr<DmaStreamComputeOwnerBinding> structuralOwnerBinding, std::shared_ptr<DmaStreamComputeOwnerGuardDecision> ownerGuardDecision, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		failure = std::nullopt;
		if (!ownerGuardDecision->IsAllowed)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::OwnerDomainFault, ownerGuardDecision->Message);
			return false;
		}

		if (ownerGuardDecision->LegalityDecision.getAuthoritySource() != LegalityAuthoritySource::GuardPlane || ownerGuardDecision->LegalityDecision.getAttemptedReplayCertificateReuse())
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::OwnerDomainFault, L"DSC2 owner guard decision must come from the guard plane before replay or certificate reuse.");
			return false;
		}

		if (ownerGuardDecision->DescriptorOwnerBinding == nullptr || !(*ownerGuardDecision->DescriptorOwnerBinding == *structuralOwnerBinding))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::OwnerDomainFault, L"DSC2 owner guard decision does not match structurally-read descriptor owner fields.");
			return false;
		}

		return true;
	}

	bool DmaStreamComputeDescriptorParser::HasAnyDsc2ReservedHeaderBitSet(std::span<const unsigned char> bytes)
	{
		return ReadUInt16(bytes, Dsc2HeaderFlagsOffset) != 0 || ReadUInt16(bytes, Dsc2Reserved0Offset) != 0 || ReadUInt32(bytes, Dsc2DescriptorFlagsOffset) != 0 || ReadUInt16(bytes, Dsc2Reserved1Offset) != 0 || ReadUInt64(bytes, Dsc2Reserved2Offset) != 0 || ReadUInt64(bytes, Dsc2Reserved3Offset) != 0 || ReadUInt64(bytes, Dsc2Reserved4Offset) != 0 || ReadUInt64(bytes, Dsc2Reserved5Offset) != 0;
	}

	bool DmaStreamComputeDescriptorParser::TryDecodeDsc2AddressSpace(unsigned short raw, DmaStreamComputeDsc2AddressSpaceKind &addressSpace)
	{
		addressSpace = static_cast<DmaStreamComputeDsc2AddressSpaceKind>(raw);
		return (addressSpace == DmaStreamComputeDsc2AddressSpaceKind::Physical || addressSpace == DmaStreamComputeDsc2AddressSpaceKind::IommuTranslated);
	}

	bool DmaStreamComputeDescriptorParser::ValidateExtensionTable(unsigned int totalSize, unsigned int extensionTableOffset, unsigned short extensionCount, unsigned int extensionTableByteSize, std::wstring &message)
	{
		message = L"";
		if (extensionCount == 0)
		{
			message = L"DSC2 parser-only descriptors require at least one extension block.";
			return false;
		}

		if ((extensionTableOffset & 0x7) != 0 || extensionTableOffset < Dsc2HeaderSize)
		{
			message = L"DSC2 extension table offset must be 8-byte aligned and outside the fixed header.";
			return false;
		}

		if ((extensionTableByteSize & 0x7) != 0 || extensionTableByteSize < static_cast<unsigned int>(extensionCount) * Dsc2ExtensionBlockHeaderSize)
		{
			message = L"DSC2 extension table byte size is not a valid aligned block table.";
			return false;
		}

		unsigned long long tableEnd = static_cast<unsigned long long>(extensionTableOffset) + extensionTableByteSize;
		if (tableEnd < extensionTableOffset || tableEnd > totalSize)
		{
			message = L"DSC2 extension table overflows the validated descriptor size.";
			return false;
		}

		return true;
	}

	bool DmaStreamComputeDescriptorParser::TryParseExtensionHeader(std::span<const unsigned char> bytes, int offset, int tableEnd, std::shared_ptr<Dsc2ExtensionHeader> &header, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		header = nullptr;
		failure = std::nullopt;

		if (offset < 0 || offset > tableEnd - Dsc2ExtensionBlockHeaderSize)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 extension block header is outside the extension table.");
			return false;
		}

		unsigned short rawType = ReadUInt16(bytes, offset + Dsc2ExtensionTypeOffset);
		unsigned short version = ReadUInt16(bytes, offset + Dsc2ExtensionVersionOffset);
		DmaStreamComputeDsc2ExtensionFlags flags = static_cast<DmaStreamComputeDsc2ExtensionFlags>(ReadUInt16(bytes, offset + Dsc2ExtensionFlagsOffset));
		unsigned short alignment = ReadUInt16(bytes, offset + Dsc2ExtensionAlignmentOffset);
		unsigned int byteLength = ReadUInt32(bytes, offset + Dsc2ExtensionLengthOffset);
		DmaStreamComputeDsc2CapabilityId capabilityId = static_cast<DmaStreamComputeDsc2CapabilityId>(ReadUInt16(bytes, offset + Dsc2ExtensionCapabilityIdOffset));
		unsigned short minimumParserMajor = ReadUInt16(bytes, offset + Dsc2ExtensionMinimumParserMajorOffset);
		unsigned int payloadChecksum = ReadUInt32(bytes, offset + Dsc2ExtensionPayloadChecksumOffset);
		unsigned long long reserved0 = ReadUInt64(bytes, offset + Dsc2ExtensionReserved0Offset);
		unsigned long long reserved1 = ReadUInt64(bytes, offset + Dsc2ExtensionReserved1Offset);

		if (!IsPowerOfTwoAlignment(alignment) || alignment > 16 || (offset % alignment) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 extension block alignment is malformed or not satisfied.");
			return false;
		}

		if (byteLength < Dsc2ExtensionBlockHeaderSize || (byteLength & 0x7) != 0 || static_cast<unsigned long long>(offset) + byteLength > static_cast<unsigned long long>(tableEnd))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 extension block length is malformed or outside the extension table.");
			return false;
		}

		if (minimumParserMajor > Dsc2MajorVersion)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedCapability, L"DSC2 extension requires a newer parser major version.");
			return false;
		}

		if (payloadChecksum != 0 || reserved0 != 0 || reserved1 != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 Phase07 extension blocks use deterministic validation rules; checksum and reserved fields must be zero.");
			return false;
		}

		header = std::make_shared<Dsc2ExtensionHeader>(rawType, version, flags, alignment, byteLength, capabilityId, minimumParserMajor);
		return true;
	}

	bool DmaStreamComputeDescriptorParser::TryHandleExtension(std::span<const unsigned char> payload, std::shared_ptr<Dsc2ExtensionHeader> header, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, DmaStreamComputeDsc2AddressSpaceKind headerAddressSpace, std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> capabilitySet, std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder, std::optional<std::shared_ptr<Dsc2FootprintSummary>> &footprintSummary, bool &hasAddressSpaceExtension, unsigned long long &mappingEpoch, std::vector<std::shared_ptr<DmaStreamComputeDsc2ParsedExtension>> &parsedExtensions, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		failure = std::nullopt;
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'typeof' operator:
		const bool known = (header->RawType >= static_cast<unsigned short>(DmaStreamComputeDsc2ExtensionType::CapabilityProfile) &&
			header->RawType <= static_cast<unsigned short>(DmaStreamComputeDsc2ExtensionType::PartialCompletionPolicy)) ||
			header->RawType == static_cast<unsigned short>(DmaStreamComputeDsc2ExtensionType::VendorNonSemanticMetadata);
		if (!known)
		{
			if ((header->Flags & DmaStreamComputeDsc2ExtensionFlags::Required) != DmaStreamComputeDsc2ExtensionFlags::None || (header->Flags & DmaStreamComputeDsc2ExtensionFlags::Semantic) != DmaStreamComputeDsc2ExtensionFlags::None || (header->Flags & DmaStreamComputeDsc2ExtensionFlags::NonSemantic) == DmaStreamComputeDsc2ExtensionFlags::None)
			{
				failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedCapability, L"Unknown DSC2 required or semantic extension rejects in parser-only mode.");
				return false;
			}

			parsedExtensions.push_back(CreateParsedExtension(header, DmaStreamComputeDsc2FootprintOutcomeKind::None));
			return true;
		}

		if (header->ExtensionType == DmaStreamComputeDsc2ExtensionType::PartialCompletionPolicy)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedCapability, L"DSC2 partial-success or partial-completion policy extension is reserved for a future ADR and remains disabled in parser-only mode.");
			return false;
		}

		if (!ValidateKnownExtensionCapability(header, capabilitySet, failure))
		{
			return false;
		}

		if (header->Version != 1)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedCapability, L"DSC2 Phase07 known extensions accept only extension version 1.");
			return false;
		}

		DmaStreamComputeDsc2FootprintOutcomeKind outcome = DmaStreamComputeDsc2FootprintOutcomeKind::None;
		switch (header->ExtensionType)
		{
			case DmaStreamComputeDsc2ExtensionType::CapabilityProfile:
				if (payload.size() != 0)
				{
					failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 CapabilityProfile extension is header-only in Phase07.");
					return false;
				}

				break;

			case DmaStreamComputeDsc2ExtensionType::AddressSpace:
				if (!TryParseAddressSpaceExtension(payload, ownerBinding, headerAddressSpace, mappingEpoch, failure))
				{
					return false;
				}

				hasAddressSpaceExtension = true;
				break;

			case DmaStreamComputeDsc2ExtensionType::StridedRange:
				if (!TryParseStridedRange(payload, footprintBuilder, outcome, failure))
				{
					return false;
				}

				break;

			case DmaStreamComputeDsc2ExtensionType::Tile2D:
				if (!TryParseTile2D(payload, footprintBuilder, outcome, failure))
				{
					return false;
				}

				break;

			case DmaStreamComputeDsc2ExtensionType::ScatterGather:
				if (!TryParseScatterGather(payload, footprintBuilder, outcome, failure))
				{
					return false;
				}

				break;

			case DmaStreamComputeDsc2ExtensionType::FootprintSummary:
				if (!TryParseFootprintSummary(payload, footprintSummary, failure))
				{
					return false;
				}

				break;

			case DmaStreamComputeDsc2ExtensionType::VendorNonSemanticMetadata:
				if ((header->Flags & DmaStreamComputeDsc2ExtensionFlags::NonSemantic) == DmaStreamComputeDsc2ExtensionFlags::None || (header->Flags & DmaStreamComputeDsc2ExtensionFlags::Semantic) != DmaStreamComputeDsc2ExtensionFlags::None)
				{
					failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedCapability, L"DSC2 non-semantic metadata extension must be explicitly non-semantic.");
					return false;
				}

				break;

			default:
				failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedCapability, L"Unhandled DSC2 extension type.");
				return false;
		}

		parsedExtensions.push_back(CreateParsedExtension(header, outcome));
		return true;
	}

	bool DmaStreamComputeDescriptorParser::ValidateKnownExtensionCapability(std::shared_ptr<Dsc2ExtensionHeader> header, std::shared_ptr<DmaStreamComputeDsc2CapabilitySet> capabilitySet, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		failure = std::nullopt;
	// C# TO C++ CONVERTER TASK: The following 'switch expression' was not converted by C# to C++ Converter:
	//			DmaStreamComputeDsc2CapabilityId expectedCapability = header.ExtensionType switch
	//			{
	//				DmaStreamComputeDsc2ExtensionType.CapabilityProfile => DmaStreamComputeDsc2CapabilityId.CapabilityProfile,
	//				DmaStreamComputeDsc2ExtensionType.AddressSpace => DmaStreamComputeDsc2CapabilityId.AddressSpace,
	//				DmaStreamComputeDsc2ExtensionType.StridedRange => DmaStreamComputeDsc2CapabilityId.StridedRange,
	//				DmaStreamComputeDsc2ExtensionType.Tile2D => DmaStreamComputeDsc2CapabilityId.Tile2D,
	//				DmaStreamComputeDsc2ExtensionType.ScatterGather => DmaStreamComputeDsc2CapabilityId.ScatterGather,
	//				DmaStreamComputeDsc2ExtensionType.FootprintSummary => DmaStreamComputeDsc2CapabilityId.FootprintSummary,
	//				DmaStreamComputeDsc2ExtensionType.PartialCompletionPolicy => DmaStreamComputeDsc2CapabilityId.PartialCompletionPolicy,
	//				DmaStreamComputeDsc2ExtensionType.VendorNonSemanticMetadata => DmaStreamComputeDsc2CapabilityId.NonSemanticMetadata,
	//				_ => DmaStreamComputeDsc2CapabilityId.None
	//			};
		DmaStreamComputeDsc2CapabilityId expectedCapability = DmaStreamComputeDsc2CapabilityId::None;
		switch (header->ExtensionType)
		{
		case DmaStreamComputeDsc2ExtensionType::CapabilityProfile: expectedCapability = DmaStreamComputeDsc2CapabilityId::CapabilityProfile; break;
		case DmaStreamComputeDsc2ExtensionType::AddressSpace: expectedCapability = DmaStreamComputeDsc2CapabilityId::AddressSpace; break;
		case DmaStreamComputeDsc2ExtensionType::StridedRange: expectedCapability = DmaStreamComputeDsc2CapabilityId::StridedRange; break;
		case DmaStreamComputeDsc2ExtensionType::Tile2D: expectedCapability = DmaStreamComputeDsc2CapabilityId::Tile2D; break;
		case DmaStreamComputeDsc2ExtensionType::ScatterGather: expectedCapability = DmaStreamComputeDsc2CapabilityId::ScatterGather; break;
		case DmaStreamComputeDsc2ExtensionType::FootprintSummary: expectedCapability = DmaStreamComputeDsc2CapabilityId::FootprintSummary; break;
		case DmaStreamComputeDsc2ExtensionType::PartialCompletionPolicy: expectedCapability = DmaStreamComputeDsc2CapabilityId::PartialCompletionPolicy; break;
		case DmaStreamComputeDsc2ExtensionType::VendorNonSemanticMetadata: expectedCapability = DmaStreamComputeDsc2CapabilityId::NonSemanticMetadata; break;
		default: break;
		}

		if (header->CapabilityId != expectedCapability)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedCapability, L"DSC2 extension capability id does not match the extension family.");
			return false;
		}

		DmaStreamComputeDsc2CapabilityStage requiredStages = DmaStreamComputeDsc2CapabilityStage::ParserKnown | DmaStreamComputeDsc2CapabilityStage::Validation;
		switch (expectedCapability)
		{
		case DmaStreamComputeDsc2CapabilityId::AddressSpace:
			requiredStages |= DmaStreamComputeDsc2CapabilityStage::BackendAddressSpace;
			break;
		case DmaStreamComputeDsc2CapabilityId::StridedRange:
		case DmaStreamComputeDsc2CapabilityId::Tile2D:
		case DmaStreamComputeDsc2CapabilityId::ScatterGather:
		case DmaStreamComputeDsc2CapabilityId::FootprintSummary:
			requiredStages |= DmaStreamComputeDsc2CapabilityStage::FootprintNormalization;
			break;
		case DmaStreamComputeDsc2CapabilityId::NonSemanticMetadata:
			requiredStages = DmaStreamComputeDsc2CapabilityStage::ParserKnown;
			break;
		default:
			break;
		}

		if (!capabilitySet->Has(expectedCapability, requiredStages))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnsupportedCapability,
				std::format(L"DSC2 extension {} requires absent capability stage(s): {}.", Dsc2ExtensionTypeName(header->ExtensionType), Dsc2CapabilityStagesName(requiredStages)));
			return false;
		}

		return true;
	}

	bool DmaStreamComputeDescriptorParser::TryParseAddressSpaceExtension(std::span<const unsigned char> payload, std::shared_ptr<DmaStreamComputeOwnerBinding> ownerBinding, DmaStreamComputeDsc2AddressSpaceKind headerAddressSpace, unsigned long long &mappingEpoch, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		mappingEpoch = 0;
		failure = std::nullopt;
		if (payload.size() != AddressSpacePayloadSize)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 AddressSpace extension payload length is malformed.");
			return false;
		}

		DmaStreamComputeDsc2AddressSpaceKind extensionAddressSpace;
		if (!TryDecodeDsc2AddressSpace(ReadUInt16(payload, 0), extensionAddressSpace) || extensionAddressSpace != headerAddressSpace)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AddressSpaceFault, L"DSC2 AddressSpace extension does not match the descriptor address-space summary.");
			return false;
		}

		if (ReadUInt16(payload, 2) != 0 || ReadUInt32(payload, 24) != 0 || ReadUInt32(payload, 28) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 AddressSpace extension reserved fields must be zero.");
			return false;
		}

		unsigned int deviceId = ReadUInt32(payload, 4);
		unsigned long long domainTag = ReadUInt64(payload, 8);
		mappingEpoch = ReadUInt64(payload, 16);

		if (deviceId != ownerBinding->DeviceId)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AddressSpaceFault, L"DSC2 AddressSpace extension device binding does not match descriptor owner binding.");
			return false;
		}

		if (domainTag != ownerBinding->OwnerDomainTag)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AddressSpaceFault, L"DSC2 AddressSpace extension domain binding does not match descriptor owner binding.");
			return false;
		}

		if (extensionAddressSpace == DmaStreamComputeDsc2AddressSpaceKind::IommuTranslated && mappingEpoch == 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AddressSpaceFault, L"DSC2 IOMMU-translated AddressSpace extension requires a non-zero mapping epoch.");
			return false;
		}

		return true;
	}

	bool DmaStreamComputeDescriptorParser::TryParseStridedRange(std::span<const unsigned char> payload, std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::None;
		failure = std::nullopt;
		if (payload.size() != StridedRangePayloadSize)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 StridedRange extension payload length is malformed.");
			return false;
		}

		DmaStreamComputeDsc2AccessKind accessKind;
		if (!TryDecodeAccessKind(ReadUInt16(payload, 0), accessKind))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 footprint extension access kind is unsupported.");
			return false;
		}

		unsigned long long elementSize;
		if (!TryValidateElementSize(ReadUInt16(payload, 2), elementSize, failure))
		{
			return false;
		}

		unsigned int elementCount = ReadUInt32(payload, 4);
		unsigned long long baseAddress = ReadUInt64(payload, 8);
		unsigned long long strideBytes = ReadUInt64(payload, 16);
		unsigned long long reserved = ReadUInt64(payload, 24);
		if (reserved != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 StridedRange reserved fields must be zero.");
			return false;
		}

		return footprintBuilder->TryAddStridedRange(accessKind, baseAddress, elementCount, elementSize, strideBytes, outcome, failure);
	}

	bool DmaStreamComputeDescriptorParser::TryParseTile2D(std::span<const unsigned char> payload, std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::None;
		failure = std::nullopt;
		if (payload.size() != Tile2DPayloadSize)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 Tile2D extension payload length is malformed.");
			return false;
		}

		DmaStreamComputeDsc2AccessKind accessKind;
		if (!TryDecodeAccessKind(ReadUInt16(payload, 0), accessKind))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 footprint extension access kind is unsupported.");
			return false;
		}

		unsigned long long elementSize;
		if (!TryValidateElementSize(ReadUInt16(payload, 2), elementSize, failure))
		{
			return false;
		}

		unsigned int rows = ReadUInt32(payload, 4);
		unsigned int columns = ReadUInt32(payload, 8);
		if (ReadUInt32(payload, 12) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 Tile2D reserved fields must be zero.");
			return false;
		}

		return footprintBuilder->TryAddTile2D(accessKind, ReadUInt64(payload, 16), rows, columns, elementSize, ReadUInt64(payload, 24), ReadUInt64(payload, 32), outcome, failure);
	}

	bool DmaStreamComputeDescriptorParser::TryParseScatterGather(std::span<const unsigned char> payload, std::shared_ptr<Dsc2FootprintBuilder> footprintBuilder, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::None;
		failure = std::nullopt;
		if (payload.size() < ScatterGatherPayloadHeaderSize)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 ScatterGather extension payload header is malformed.");
			return false;
		}

		DmaStreamComputeDsc2AccessKind accessKind;
		if (!TryDecodeAccessKind(ReadUInt16(payload, 0), accessKind))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 footprint extension access kind is unsupported.");
			return false;
		}

		unsigned long long elementSize;
		if (!TryValidateElementSize(ReadUInt16(payload, 2), elementSize, failure))
		{
			return false;
		}

		unsigned int segmentCount = ReadUInt32(payload, 4);
		unsigned int segmentTableBytes = ReadUInt32(payload, 8);
		if (ReadUInt32(payload, 12) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 ScatterGather reserved fields must be zero.");
			return false;
		}

		if (segmentCount == 0 || segmentCount > MaxScatterGatherSegmentCount || segmentTableBytes != segmentCount * ScatterGatherSegmentEntrySize || payload.size() != ScatterGatherPayloadHeaderSize + segmentTableBytes)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::FootprintNormalizationFault, L"DSC2 ScatterGather segment table count or byte size is outside Phase07 parser-only bounds.");
			return false;
		}

		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> segments(segmentCount);
		int segmentOffset = ScatterGatherPayloadHeaderSize;
		for (int index = 0; index < segmentCount; index++)
		{
			unsigned long long address = ReadUInt64(payload, segmentOffset);
			unsigned long long length = ReadUInt64(payload, segmentOffset + sizeof(unsigned long long));
			if (!ValidateRange(address, length, elementSize, failure))
			{
				return false;
			}

			segments[index] = std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{address, length});
			segmentOffset += ScatterGatherSegmentEntrySize;
		}

		return footprintBuilder->TryAddScatterGather(accessKind, segments, outcome, failure);
	}

	bool DmaStreamComputeDescriptorParser::TryParseFootprintSummary(std::span<const unsigned char> payload, std::optional<std::shared_ptr<Dsc2FootprintSummary>> &summary, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		summary = std::nullopt;
		failure = std::nullopt;
		if (payload.size() != FootprintSummaryPayloadSize)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 FootprintSummary extension payload length is malformed.");
			return false;
		}

		DmaStreamComputeDsc2FootprintOutcomeKind outcome = static_cast<DmaStreamComputeDsc2FootprintOutcomeKind>(ReadUInt16(payload, 0));
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		if ((outcome != DmaStreamComputeDsc2FootprintOutcomeKind::Exact && outcome != DmaStreamComputeDsc2FootprintOutcomeKind::Conservative) || ReadUInt16(payload, 2) != 0 || ReadUInt32(payload, 4) != 0 || ReadUInt64(payload, 40) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::MalformedExtension, L"DSC2 FootprintSummary outcome or reserved fields are malformed.");
			return false;
		}

		summary = std::make_shared<Dsc2FootprintSummary>(outcome,
			std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{ReadUInt64(payload, 8), ReadUInt64(payload, 16)}),
			std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{ReadUInt64(payload, 24), ReadUInt64(payload, 32)}));
		return true;
	}

	bool DmaStreamComputeDescriptorParser::TryDecodeAccessKind(unsigned short raw, DmaStreamComputeDsc2AccessKind &accessKind)
	{
		accessKind = static_cast<DmaStreamComputeDsc2AccessKind>(raw);
		return (accessKind == DmaStreamComputeDsc2AccessKind::Read || accessKind == DmaStreamComputeDsc2AccessKind::Write || accessKind == DmaStreamComputeDsc2AccessKind::ReadWrite);
	}

	bool DmaStreamComputeDescriptorParser::TryValidateElementSize(unsigned short rawElementSize, unsigned long long &elementSize, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		elementSize = rawElementSize;
		failure = std::nullopt;
		if (elementSize == 0 || (elementSize & (elementSize - 1)) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AlignmentFault, L"DSC2 footprint element size must be a non-zero power of two.");
			return false;
		}

		return true;
	}

	bool DmaStreamComputeDescriptorParser::ValidateRange(unsigned long long address, unsigned long long length, unsigned long long elementSize, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		failure = std::nullopt;
		if (length == 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::ZeroLengthFault, L"DSC2 footprint ranges must have non-zero length.");
			return false;
		}

		if (address > std::numeric_limits<unsigned long long>::max() - length)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DSC2 footprint range address + length overflows UInt64.");
			return false;
		}

		if ((address % elementSize) != 0 || (length % elementSize) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AlignmentFault, L"DSC2 footprint range is not aligned to the element size.");
			return false;
		}

		return true;
	}

	bool DmaStreamComputeDescriptorParser::IsPowerOfTwoAlignment(unsigned short alignment)
	{
		return alignment != 0 && (alignment & (alignment - 1)) == 0;
	}

	std::shared_ptr<DmaStreamComputeDsc2ParsedExtension> DmaStreamComputeDescriptorParser::CreateParsedExtension(std::shared_ptr<Dsc2ExtensionHeader> header, DmaStreamComputeDsc2FootprintOutcomeKind outcome)
	{
		return std::make_shared<DmaStreamComputeDsc2ParsedExtension>(
			header->ExtensionType, header->Version, header->Flags, header->ByteLength,
			header->CapabilityId, outcome);
	}

	bool DmaStreamComputeDescriptorParser::Dsc2FootprintBuilder::TryAddStridedRange(DmaStreamComputeDsc2AccessKind accessKind, unsigned long long baseAddress, unsigned int elementCount, unsigned long long elementSize, unsigned long long strideBytes, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::None;
		failure = std::nullopt;
		if (elementCount == 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::ZeroLengthFault, L"DSC2 StridedRange element count must be non-zero.");
			return false;
		}

		if ((baseAddress % elementSize) != 0 || (strideBytes % elementSize) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AlignmentFault, L"DSC2 StridedRange base and stride must be aligned to the element size.");
			return false;
		}

		unsigned long long lastIndex = elementCount - 1ULL;
		unsigned long long lastDelta;
		unsigned long long lastAddress;
		unsigned long long lastEnd;
		if (!TryMultiply(lastIndex, strideBytes, lastDelta) || !TryAdd(baseAddress, lastDelta, lastAddress) || !TryAdd(lastAddress, elementSize, lastEnd))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DSC2 StridedRange address calculation overflows UInt64.");
			return false;
		}

		if (strideBytes == elementSize)
		{
			unsigned long long length = static_cast<unsigned long long>(elementCount) * elementSize;
			if (!ValidateRange(baseAddress, length, elementSize, failure))
			{
				return false;
			}

			AddRange(accessKind, std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{baseAddress, length}));
			outcome = DmaStreamComputeDsc2FootprintOutcomeKind::Exact;
			return true;
		}

		if (elementCount <= MaxExactRangeCount)
		{
			for (unsigned int index = 0; index < elementCount; index++)
			{
				unsigned long long delta;
				unsigned long long address;
				if (!TryMultiply(index, strideBytes, delta) || !TryAdd(baseAddress, delta, address))
				{
					failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DSC2 StridedRange exact normalization overflows UInt64.");
					return false;
				}

				AddRange(accessKind, std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{address, elementSize}));
			}

			outcome = DmaStreamComputeDsc2FootprintOutcomeKind::Exact;
			return true;
		}

		unsigned long long span = lastEnd - baseAddress;
		if (span > MaxConservativeFootprintSpanBytes)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::FootprintNormalizationFault, L"DSC2 StridedRange conservative footprint exceeds Phase07 bounds.");
			return false;
		}

		AddRange(accessKind, std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{baseAddress, span}));
		_hasConservativeRange = true;
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::Conservative;
		return true;
	}

	bool DmaStreamComputeDescriptorParser::Dsc2FootprintBuilder::TryAddTile2D(DmaStreamComputeDsc2AccessKind accessKind, unsigned long long baseAddress, unsigned int rows, unsigned int columns, unsigned long long elementSize, unsigned long long rowStrideBytes, unsigned long long columnStrideBytes, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::None;
		failure = std::nullopt;
		if (rows == 0 || columns == 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::ZeroLengthFault, L"DSC2 Tile2D row and column counts must be non-zero.");
			return false;
		}

		if ((baseAddress % elementSize) != 0 || (rowStrideBytes % elementSize) != 0 || (columnStrideBytes % elementSize) != 0)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::AlignmentFault, L"DSC2 Tile2D base, row stride, and column stride must be aligned to the element size.");
			return false;
		}

		unsigned long long contiguousRowBytes;
		if (!TryMultiply(columns, elementSize, contiguousRowBytes))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DSC2 Tile2D contiguous row byte calculation overflows UInt64.");
			return false;
		}

		if (columnStrideBytes == elementSize && rowStrideBytes == contiguousRowBytes)
		{
			unsigned long long length;
			if (!TryMultiply(rows, contiguousRowBytes, length) || !ValidateRange(baseAddress, length, elementSize, failure))
			{
				failure = failure ? failure : DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DSC2 Tile2D contiguous footprint overflows UInt64.");
				return false;
			}

			AddRange(accessKind, std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{baseAddress, length}));
			outcome = DmaStreamComputeDsc2FootprintOutcomeKind::Exact;
			return true;
		}

		unsigned long long lastRowDelta;
		unsigned long long lastColumnDelta;
		unsigned long long lastDelta;
		unsigned long long lastAddress;
		unsigned long long maxEnd;
		if (!TryMultiply(rows - 1ULL, rowStrideBytes, lastRowDelta) || !TryMultiply(columns - 1ULL, columnStrideBytes, lastColumnDelta) || !TryAdd(lastRowDelta, lastColumnDelta, lastDelta) || !TryAdd(baseAddress, lastDelta, lastAddress) || !TryAdd(lastAddress, elementSize, maxEnd))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DSC2 Tile2D address calculation overflows UInt64.");
			return false;
		}

		unsigned long long elementTotal = static_cast<unsigned long long>(rows) * columns;
		if (elementTotal <= MaxExactRangeCount)
		{
			for (unsigned int row = 0; row < rows; row++)
			{
				unsigned long long rowDelta;
				if (!TryMultiply(row, rowStrideBytes, rowDelta))
				{
					failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DSC2 Tile2D exact row calculation overflows UInt64.");
					return false;
				}

				for (unsigned int column = 0; column < columns; column++)
				{
					unsigned long long columnDelta;
					unsigned long long rowAddress;
					unsigned long long address;
					if (!TryMultiply(column, columnStrideBytes, columnDelta) || !TryAdd(baseAddress, rowDelta, rowAddress) || !TryAdd(rowAddress, columnDelta, address))
					{
						failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::RangeOverflow, L"DSC2 Tile2D exact element calculation overflows UInt64.");
						return false;
					}

					AddRange(accessKind, std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{address, elementSize}));
				}
			}

			outcome = DmaStreamComputeDsc2FootprintOutcomeKind::Exact;
			return true;
		}

		unsigned long long span = maxEnd - baseAddress;
		if (span > MaxConservativeFootprintSpanBytes)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::FootprintNormalizationFault, L"DSC2 Tile2D conservative footprint exceeds Phase07 bounds.");
			return false;
		}

		AddRange(accessKind, std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{baseAddress, span}));
		_hasConservativeRange = true;
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::Conservative;
		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public bool TryAddScatterGather(DmaStreamComputeDsc2AccessKind accessKind, IReadOnlyList<DmaStreamComputeMemoryRange> segments, out DmaStreamComputeDsc2FootprintOutcomeKind outcome, out System.Nullable<DmaStreamComputeDsc2ValidationResult> failure)
	bool DmaStreamComputeDescriptorParser::Dsc2FootprintBuilder::TryAddScatterGather(DmaStreamComputeDsc2AccessKind accessKind, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &segments, DmaStreamComputeDsc2FootprintOutcomeKind &outcome, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::None;
		failure = std::nullopt;
		if (segments.size() <= MaxExactRangeCount)
		{
			for (int index = 0; index < segments.size(); index++)
			{
				AddRange(accessKind, segments[index]);
			}

			outcome = DmaStreamComputeDsc2FootprintOutcomeKind::Exact;
			return true;
		}

		unsigned long long minStart = std::numeric_limits<unsigned long long>::max();
		unsigned long long maxEnd = 0;
		for (int index = 0; index < segments.size(); index++)
		{
			std::shared_ptr<DmaStreamComputeMemoryRange> range = segments[index];
			unsigned long long end = range->Address + range->Length;
			if (range->Address < minStart)
			{
				minStart = range->Address;
			}

			if (end > maxEnd)
			{
				maxEnd = end;
			}
		}

		unsigned long long span = maxEnd - minStart;
		if (span > MaxConservativeFootprintSpanBytes)
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::FootprintNormalizationFault, L"DSC2 ScatterGather conservative footprint exceeds Phase07 bounds.");
			return false;
		}

		AddRange(accessKind, std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{minStart, span}));
		_hasConservativeRange = true;
		outcome = DmaStreamComputeDsc2FootprintOutcomeKind::Conservative;
		return true;
	}

	bool DmaStreamComputeDescriptorParser::Dsc2FootprintBuilder::TryBuild(std::optional<std::shared_ptr<Dsc2FootprintSummary>> &summary, std::optional<std::shared_ptr<DmaStreamComputeDsc2FootprintModel>> &footprint, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		footprint = std::nullopt;
		failure = std::nullopt;
		if (_readRanges.empty() && _writeRanges.empty())
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::FootprintNormalizationFault, L"DSC2 parser-only descriptor did not produce a normalized read or write footprint.");
			return false;
		}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<DmaStreamComputeMemoryRange> normalizedReadRanges = NormalizeRanges(_readRanges);
		const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> normalizedReadRanges = NormalizeRanges(_readRanges);
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: IReadOnlyList<DmaStreamComputeMemoryRange> normalizedWriteRanges = NormalizeRanges(_writeRanges);
		const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> normalizedWriteRanges = NormalizeRanges(_writeRanges);
		DmaStreamComputeDsc2FootprintOutcomeKind outcome = _hasConservativeRange ? DmaStreamComputeDsc2FootprintOutcomeKind::Conservative : DmaStreamComputeDsc2FootprintOutcomeKind::Exact;

		if (summary && !ValidateFootprintSummary(normalizedReadRanges, normalizedWriteRanges, summary.value(), failure))
		{
			return false;
		}

		footprint = std::make_shared<DmaStreamComputeDsc2FootprintModel>(normalizedReadRanges, normalizedWriteRanges, outcome, ComputeDsc2FootprintHash(normalizedReadRanges, normalizedWriteRanges, outcome));
		return true;
	}

	void DmaStreamComputeDescriptorParser::Dsc2FootprintBuilder::AddRange(DmaStreamComputeDsc2AccessKind accessKind, std::shared_ptr<DmaStreamComputeMemoryRange> range)
	{
		if ((accessKind == DmaStreamComputeDsc2AccessKind::Read || accessKind == DmaStreamComputeDsc2AccessKind::ReadWrite))
		{
			_readRanges.push_back(range);
		}

		if ((accessKind == DmaStreamComputeDsc2AccessKind::Write || accessKind == DmaStreamComputeDsc2AccessKind::ReadWrite))
		{
			_writeRanges.push_back(range);
		}
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool ValidateFootprintSummary(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges, Dsc2FootprintSummary summary, out System.Nullable<DmaStreamComputeDsc2ValidationResult> failure)
	bool DmaStreamComputeDescriptorParser::ValidateFootprintSummary(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &readRanges, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &writeRanges, std::shared_ptr<Dsc2FootprintSummary> summary, std::optional<std::shared_ptr<DmaStreamComputeDsc2ValidationResult>> &failure)
	{
		failure = std::nullopt;
		if (!SummaryCovers(readRanges, summary->ReadRange) || !SummaryCovers(writeRanges, summary->WriteRange))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnderApproximatedFootprintFault, L"DSC2 FootprintSummary under-approximates parser-normalized memory ranges.");
			return false;
		}

		if (summary->OutcomeKind == DmaStreamComputeDsc2FootprintOutcomeKind::Exact && (!SummaryExactlyMatches(readRanges, summary->ReadRange) || !SummaryExactlyMatches(writeRanges, summary->WriteRange)))
		{
			failure = DmaStreamComputeDsc2ValidationResult::Fail(DmaStreamComputeValidationFault::UnderApproximatedFootprintFault, L"DSC2 exact FootprintSummary must match parser-normalized ranges exactly.");
			return false;
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool SummaryCovers(IReadOnlyList<DmaStreamComputeMemoryRange> ranges, DmaStreamComputeMemoryRange summaryRange)
	bool DmaStreamComputeDescriptorParser::SummaryCovers(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges, std::shared_ptr<DmaStreamComputeMemoryRange> summaryRange)
	{
		if (ranges.empty())
		{
			return summaryRange->Length == 0;
		}

		if (summaryRange->Length == 0 || summaryRange->Address > std::numeric_limits<unsigned long long>::max() - summaryRange->Length)
		{
			return false;
		}

		unsigned long long summaryEnd = summaryRange->Address + summaryRange->Length;
		for (int index = 0; index < ranges.size(); index++)
		{
			std::shared_ptr<DmaStreamComputeMemoryRange> range = ranges[index];
			unsigned long long rangeEnd = range->Address + range->Length;
			if (range->Address < summaryRange->Address || rangeEnd > summaryEnd)
			{
				return false;
			}
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool SummaryExactlyMatches(IReadOnlyList<DmaStreamComputeMemoryRange> ranges, DmaStreamComputeMemoryRange summaryRange)
	bool DmaStreamComputeDescriptorParser::SummaryExactlyMatches(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges, std::shared_ptr<DmaStreamComputeMemoryRange> summaryRange)
	{
		if (ranges.empty())
		{
			return summaryRange->Length == 0;
		}

		return ranges.size() == 1 && ranges[0]->Address == summaryRange->Address && ranges[0]->Length == summaryRange->Length;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static IReadOnlyList<DmaStreamComputeMemoryRange> NormalizeRanges(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
	const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> DmaStreamComputeDescriptorParser::NormalizeRanges(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &ranges)
	{
		if (ranges.empty())
		{
			return {};
		}

		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> sorted(ranges.begin(), ranges.end());
		if (std::any_of(sorted.begin(), sorted.end(), [](const auto& range) { return !range; }))
			throw std::invalid_argument("DSC2 ranges cannot contain null entries.");
		std::sort(sorted.begin(), sorted.end(), [](const auto& left, const auto& right)
		{
			return left->Address != right->Address ? left->Address < right->Address : left->Length < right->Length;
		});

		std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> merged;
		merged.reserve(sorted.size());
		unsigned long long currentStart = sorted[0]->Address;
		unsigned long long currentEnd = sorted[0]->Address + sorted[0]->Length;
		for (int index = 1; index < sorted.size(); index++)
		{
			std::shared_ptr<DmaStreamComputeMemoryRange> next = sorted[index];
			unsigned long long nextEnd = next->Address + next->Length;
			if (next->Address <= currentEnd)
			{
				if (nextEnd > currentEnd)
				{
					currentEnd = nextEnd;
				}

				continue;
			}

			merged.push_back(std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{currentStart, currentEnd - currentStart}));
			currentStart = next->Address;
			currentEnd = nextEnd;
		}

		merged.push_back(std::make_shared<DmaStreamComputeMemoryRange>(DmaStreamComputeMemoryRange{currentStart, currentEnd - currentStart}));
		return merged;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static ulong ComputeDsc2FootprintHash(IReadOnlyList<DmaStreamComputeMemoryRange> readRanges, IReadOnlyList<DmaStreamComputeMemoryRange> writeRanges, DmaStreamComputeDsc2FootprintOutcomeKind outcomeKind)
	unsigned long long DmaStreamComputeDescriptorParser::ComputeDsc2FootprintHash(const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &readRanges, const std::vector<std::shared_ptr<DmaStreamComputeMemoryRange>> &writeRanges, DmaStreamComputeDsc2FootprintOutcomeKind outcomeKind)
	{
		constexpr unsigned long long offsetBasis = 14695981039346656037ULL;
		constexpr unsigned long long prime = 1099511628211ULL;

		unsigned long long hash = offsetBasis;
		auto Add = [&](unsigned long long value) { hash ^= value; hash *= prime; };
		auto AddRanges = [&](const auto& ranges) { Add(ranges.size()); for (const auto& range : ranges) { Add(range->Address); Add(range->Length); } };
		Add(static_cast<unsigned short>(outcomeKind));
		AddRanges(readRanges);
		AddRanges(writeRanges);
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

// C# TO C++ CONVERTER TASK: Local functions are not converted by C# to C++ Converter:
//		void AddRanges(IReadOnlyList<DmaStreamComputeMemoryRange> ranges)
	//			{
	//				Add((ulong)ranges.Count);
	//				for (int index = 0; index < ranges.Count; index++)
	//				{
	//					Add(ranges[index].Address);
	//					Add(ranges[index].size());
	//				}
	//			}
	}

	bool DmaStreamComputeDescriptorParser::TryMultiply(unsigned long long left, unsigned long long right, unsigned long long &product)
	{
		if (left != 0 && right > std::numeric_limits<unsigned long long>::max() / left)
		{
			product = 0;
			return false;
		}

		product = left * right;
		return true;
	}

	bool DmaStreamComputeDescriptorParser::TryAdd(unsigned long long left, unsigned long long right, unsigned long long &sum)
	{
		if (left > std::numeric_limits<unsigned long long>::max() - right)
		{
			sum = 0;
			return false;
		}

		sum = left + right;
		return true;
	}
}
