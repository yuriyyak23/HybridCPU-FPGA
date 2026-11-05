#include "CanonicalDecodedContracts.h"
#include "DecodedInstruction.h"
#include "../Pipeline/Metadata/BundleMetadata.h"
#include "../Pipeline/Metadata/MicroOpAdmissionMetadata.h"
#include "../Contracts/CompilerTransport/InstructionSlotMetadata.h"
#include "DeclarativeDecoderStages.h"
#include "CanonicalSecurityOperationIdentity.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileNumericPolicyAbi.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileLayoutPolicyAbi.h"
#include <utility>
#include <sstream>
#include <stdexcept>
#include <cwctype>
#include <cstdint>
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../../../NonRTL/Arch/SerializationClass.h"

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Arch::Generated;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using OpcodeValues = YAKSys_Hybrid_CPU::Processor::CPU_Core::IsaOpcodeValues;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
    CanonicalSecurityOperationKind CanonicalDecodedInstruction::getSecurityOperationKind() const
    {
        return IsOccupied && StaticBinding && StaticBinding->Opcode == Opcode
            ? CanonicalSecurityOperationIdentity::FromGeneratedBinding(StaticBinding)
            : CanonicalSecurityOperationKind::Unknown;
    }
    const std::shared_ptr<CanonicalDecodeContext> CanonicalDecodeContext::Unbound = [] {
        auto context = std::make_shared<CanonicalDecodeContext>();
        context->ManifestVersion = GeneratedIsaCatalog::ManifestVersion;
        context->ManifestHash = GeneratedIsaCatalog::ManifestSha256;
        context->ExtensionConfigurationFingerprint = L"unbound";
        context->DecoderEpoch = L"rf04-shadow";
        context->DecoderVersion = L"VliwDecoderV4-legacy";
        context->PrivilegeContext = L"unbound";
        context->DomainIdentity = L"unbound";
        context->AddressSpaceIdentity = L"unbound";
        context->VectorConfigurationFingerprint = L"unbound";
        return context;
    }();



// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static CanonicalPayloadSnapshot FromObject(string kind, Object? value)
	std::shared_ptr<CanonicalPayloadSnapshot> CanonicalPayloadSnapshot::FromObject(const std::wstring &kind, std::any value)
	{
		if (kind.empty() || std::all_of(kind.begin(), kind.end(), [](wchar_t ch) { return std::iswspace(ch) != 0; }))
			throw std::invalid_argument("kind must not be empty or whitespace");

		std::ostringstream json;
		auto writeBool = [&json](bool value) { json << (value ? "true" : "false"); };
		auto writeString = [&json](const std::wstring &value)
		{
			auto writeUnicodeEscape = [&json](std::uint32_t codeUnit)
			{
				static constexpr char digits[] = "0123456789ABCDEF";
				json << "\\u"
					 << digits[(codeUnit >> 12) & 0x0f]
					 << digits[(codeUnit >> 8) & 0x0f]
					 << digits[(codeUnit >> 4) & 0x0f]
					 << digits[codeUnit & 0x0f];
			};
			json << '"';
			for (wchar_t ch : value)
			{
				const auto codePoint = static_cast<std::uint32_t>(ch);
				switch (codePoint)
				{
				case L'"': json << "\\\""; break;
				case L'\\': json << "\\\\"; break;
				case L'\b': json << "\\b"; break;
				case L'\f': json << "\\f"; break;
				case L'\n': json << "\\n"; break;
				case L'\r': json << "\\r"; break;
				case L'\t': json << "\\t"; break;
				case L'<': json << "\\u003C"; break;
				case L'>': json << "\\u003E"; break;
				case L'&': json << "\\u0026"; break;
				case L'\'': json << "\\u0027"; break;
				default:
					if (codePoint < 0x20)
						writeUnicodeEscape(codePoint);
					else if (codePoint <= 0x7f)
						json << static_cast<char>(codePoint);
					else if (codePoint <= 0xffff)
						writeUnicodeEscape(codePoint);
					else if (codePoint <= 0x10ffff)
					{
						const std::uint32_t scalar = codePoint - 0x10000;
						writeUnicodeEscape(0xd800 + (scalar >> 10));
						writeUnicodeEscape(0xdc00 + (scalar & 0x3ff));
					}
					else
						throw std::invalid_argument("string contains an invalid Unicode scalar value");
				}
			}
			json << '"';
		};
		auto writeIntVector = [&json](const std::vector<int> &values)
		{
			json << '[';
			for (std::size_t i = 0; i < values.size(); ++i) { if (i) json << ','; json << values[i]; }
			json << ']';
		};
		auto writeRanges = [&json](const std::vector<std::pair<unsigned long long, unsigned long long>> &ranges)
		{
			json << '[';
			for (std::size_t i = 0; i < ranges.size(); ++i)
			{
				if (i) json << ',';
				// System.Text.Json's default options ignore public fields. ValueTuple
				// exposes Item1/Item2 as fields, so each tuple serializes as an empty object.
				json << "{}";
			}
			json << ']';
		};
		auto writeVectorPayload = [&json](const VectorInstructionPayload &v)
		{
			json << "{\"primaryPointer\":" << v.PrimaryPointer
				 << ",\"secondaryPointer\":" << v.SecondaryPointer
				 << ",\"streamLength\":" << v.StreamLength
				 << ",\"stride\":" << v.Stride
				 << ",\"rowStride\":" << v.RowStride
				 << ",\"indexed\":" << v.Indexed
				 << ",\"is2D\":" << v.Is2D
				 << ",\"tailAgnostic\":" << v.TailAgnostic
				 << ",\"maskAgnostic\":" << v.MaskAgnostic
				 << ",\"saturating\":" << v.Saturating
				 << ",\"predicateMask\":" << static_cast<unsigned int>(v.PredicateMask)
				 << ",\"dataType\":" << static_cast<unsigned int>(v.DataType)
				 << ",\"matrixTileNumericPolicy\":";
			if (v.MatrixTileNumericPolicy)
			{
				const auto &p = *v.MatrixTileNumericPolicy;
				json << '[' << p.AbiVersion << ',' << static_cast<unsigned int>(p.ProfileId) << ','
					 << static_cast<unsigned int>(p.ElementType) << ',' << static_cast<unsigned int>(p.AccumulatorType) << ','
					 << static_cast<unsigned int>(p.PublishFormat) << ',' << static_cast<unsigned int>(p.Signedness) << ','
					 << static_cast<unsigned int>(p.WideningRule) << ',' << static_cast<unsigned int>(p.MultiplyRule) << ','
					 << static_cast<unsigned int>(p.AddRule) << ',' << static_cast<unsigned int>(p.RoundingMode) << ','
					 << static_cast<unsigned int>(p.SaturationMode) << ',' << static_cast<unsigned int>(p.OverflowMode) << ','
					 << static_cast<unsigned int>(p.NaNPolicy) << ',' << static_cast<unsigned int>(p.InfinityPolicy) << ','
					 << static_cast<unsigned int>(p.DenormalPolicy) << ',' << static_cast<unsigned int>(p.ReproducibilityMode) << ','
					 << static_cast<unsigned int>(p.ExceptionPolicy) << ',' << p.Fingerprint << ']';
			}
			else json << "null";
			json << ",\"matrixTileLayoutPolicy\":";
			if (v.MatrixTileLayoutPolicy)
			{
				const auto &p = *v.MatrixTileLayoutPolicy;
				json << '[' << p.AbiVersion << ',' << static_cast<unsigned int>(p.ProfileId) << ','
					 << static_cast<unsigned int>(p.OperationKind) << ',' << static_cast<unsigned int>(p.SourceAddressing) << ','
					 << static_cast<unsigned int>(p.SecondaryAddressing) << ',' << static_cast<unsigned int>(p.DestinationAddressing) << ','
					 << static_cast<unsigned int>(p.KIterationOrder) << ',' << static_cast<unsigned int>(p.TransposePermutation) << ','
					 << static_cast<unsigned int>(p.TransposeAliasPolicy) << ',' << p.Fingerprint << ']';
			}
			else json << "null";
			json << '}';
		};
		if (!value.has_value())
		{
			json << "null";
		}
		else if (const auto *instruction = std::any_cast<std::shared_ptr<InstructionIR>>(&value))
		{
			if (!*instruction) json << "null";
			else
			{
				const auto &ir = **instruction;
				json << "{\"opcode\":" << (ir.CanonicalOpcode ? ir.CanonicalOpcode->Value : 0)
					 << ",\"class\":" << static_cast<unsigned int>(ir.Class)
					 << ",\"serializationClass\":" << static_cast<unsigned int>(ir.SerializationClass)
					 << ",\"rd\":" << static_cast<unsigned int>(ir.Rd)
					 << ",\"rs1\":" << static_cast<unsigned int>(ir.Rs1)
					 << ",\"rs2\":" << static_cast<unsigned int>(ir.Rs2)
					 << ",\"imm\":" << ir.Imm
					 << ",\"absolute\":" << ir.HasAbsoluteAddressing
					 << ",\"acquire\":" << ir.AcquireOrdering
					 << ",\"release\":" << ir.ReleaseOrdering
					 << ",\"dataType\":" << static_cast<unsigned int>(ir.DataType)
					 << ",\"csr\":" << (ir.CsrAddress ? static_cast<int>(*ir.CsrAddress) : -1);
				json << ",\"vector\":";
				if (ir.VectorPayload && *ir.VectorPayload) writeVectorPayload(**ir.VectorPayload);
				else json << "null";
				if (ir.DmaStreamComputeDescriptor)
					json << ",\"dmaDescriptor\":" << ir.DmaStreamComputeDescriptor->DescriptorIdentityHash;
				if (ir.AcceleratorCommandDescriptor)
					json << ",\"acceleratorDescriptor\":" << ir.AcceleratorCommandDescriptor->DescriptorReference.DescriptorIdentityHash;
				json << '}';
			}
		}
		else if (const auto *semantics = std::any_cast<std::shared_ptr<DeclarativeInstructionSemantics>>(&value))
		{
			if (!*semantics) json << "null";
			else
			{
				const auto &s = **semantics;
				json << "{\"opcode\":" << s.Opcode
					 << ",\"instructionClass\":" << static_cast<unsigned int>(s.InstructionClass)
					 << ",\"serializationClass\":" << static_cast<unsigned int>(s.SerializationClass)
					 << ",\"rd\":" << static_cast<unsigned int>(s.Rd)
					 << ",\"rs1\":" << static_cast<unsigned int>(s.Rs1)
					 << ",\"rs2\":" << static_cast<unsigned int>(s.Rs2)
					 << ",\"immediate\":" << s.Immediate << ",\"vector\":";
				if (s.VectorPayload && *s.VectorPayload) writeVectorPayload(**s.VectorPayload);
				else json << "null";
				json << '}';
			}
		}
		else if (const auto *bundle = std::any_cast<std::shared_ptr<BundleMetadata>>(&value))
		{
			if (!*bundle) json << "null";
			else
			{
				const auto &b = **bundle;
				json << "{\"SchemaVersion\":" << static_cast<unsigned int>(b.SchemaVersion) << ",\"SlotMetadata\":";
				if (!b.SlotMetadata)
				{
					json << "null";
				}
				else
				{
					json << '[';
					for (std::size_t i = 0; i < b.SlotMetadata->size(); ++i)
					{
						if (i) json << ',';
						const auto &slot = (*b.SlotMetadata)[i];
						if (!slot) { json << "null"; continue; }
						json << "{\"SchemaVersion\":" << static_cast<unsigned int>(slot->SchemaVersion)
							 << ",\"BranchHint\":" << static_cast<unsigned int>(slot->BranchHint)
							 << ",\"StealabilityPolicy\":" << static_cast<unsigned int>(slot->StealabilityPolicy)
							 << ",\"DonorVtHint\":" << static_cast<unsigned int>(slot->DonorVtHint)
							 << ",\"LocalityHint\":" << static_cast<unsigned int>(slot->LocalityHint)
							 << ",\"PreferredVt\":" << static_cast<unsigned int>(slot->PreferredVt)
							 << ",\"ThermalHint\":" << static_cast<unsigned int>(slot->ThermalHint)
							 << ",\"AdmissionMetadata\":";
						{
							// The C# property is a value-type record and therefore always
							// serializes as an object. A null native pointer represents its
							// neutral/default value at this boundary.
							const MicroOpAdmissionMetadata defaultAdmission{};
							const auto &admission = slot->AdmissionMetadata
								? *slot->AdmissionMetadata : defaultAdmission;
							json << "{\"IsStealable\":"; writeBool(admission.IsStealable);
							json << ",\"IsControlFlow\":"; writeBool(admission.IsControlFlow);
							json << ",\"IsMemoryOp\":"; writeBool(admission.IsMemoryOp);
							json << ",\"WritesRegister\":"; writeBool(admission.WritesRegister);
							json << ",\"HasSideEffects\":"; writeBool(admission.HasSideEffects);
							json << ",\"OwnerContextId\":" << admission.OwnerContextId
								 << ",\"DomainTag\":" << admission.DomainTag
								 // SlotPlacementMetadata is a C# struct with public fields;
								 // System.Text.Json excludes fields by default.
								 << ",\"Placement\":{}"
								 << ",\"ReadRegisters\":";
							writeIntVector(admission.ReadRegisters);
							json << ",\"WriteRegisters\":";
							writeIntVector(admission.WriteRegisters);
							json << ",\"ReadMemoryRanges\":";
							writeRanges(admission.ReadMemoryRanges);
							json << ",\"NormalizedReadMemoryRanges\":";
							writeRanges(admission.NormalizedReadMemoryRanges);
							json << ",\"WriteMemoryRanges\":";
							writeRanges(admission.WriteMemoryRanges);
							const auto &assist = admission.AssistCoalescingDescriptorValue;
							json << ",\"AssistCoalescingDescriptor\":{\"BaseAddress\":" << assist.BaseAddress
								 << ",\"PrefetchLength\":" << assist.PrefetchLength
								 << ",\"SourceRangeCount\":" << assist.SourceRangeCount
								 << ",\"NormalizedRangeCount\":" << assist.NormalizedRangeCount
								 << ",\"LargestGapBytes\":" << assist.LargestGapBytes
								 << ",\"IsValid\":"; writeBool(assist.getIsValid());
							json << ",\"IsNearContiguous\":"; writeBool(assist.getIsNearContiguous());
							json << '}'
								 << ",\"RegisterHazardMask\":" << admission.RegisterHazardMask
								 << ",\"StructuralSafetyMask\":{\"IsZero\":";
							writeBool(admission.StructuralSafetyMask.isZero());
							json << ",\"IsNonZero\":";
							writeBool(admission.StructuralSafetyMask.isNonZero());
							json << "},\"HasStructuralSafetyMask\":";
							writeBool(admission.StructuralSafetyMask.isNonZero());
							const unsigned long long registerBitsMaskLow32 = 0xFFFF'FFFFULL;
							SafetyMask128 sharedStructuralMask(
								admission.StructuralSafetyMask.Low & ~registerBitsMaskLow32,
								admission.StructuralSafetyMask.High);
							SafetyMask128 certificateMask(
								(admission.StructuralSafetyMask.Low & ~registerBitsMaskLow32) | admission.RegisterHazardMask,
								admission.StructuralSafetyMask.High);
							json << ",\"SharedStructuralMask\":{\"IsZero\":";
							writeBool(sharedStructuralMask.isZero());
							json << ",\"IsNonZero\":";
							writeBool(sharedStructuralMask.isNonZero());
							json << "},\"CertificateMask\":{\"IsZero\":";
							writeBool(certificateMask.isZero());
							json << ",\"IsNonZero\":";
							writeBool(certificateMask.isNonZero());
							json << "}}";
						}
						json << '}';
					}
					json << ']';
				}
				json << ",\"FspBoundary\":"; writeBool(b.FspBoundary);
				json << ",\"BundleThermalHint\":" << static_cast<unsigned int>(b.BundleThermalHint)
					 << ",\"IsReplayAnchor\":"; writeBool(b.IsReplayAnchor);
				json << ",\"DiagnosticsTag\":";
				if (b.DiagnosticsTag) writeString(*b.DiagnosticsTag); else json << "null";
				json << '}';
			}
		}
		else if (const auto *slotMetadata = std::any_cast<std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata>>(&value))
		{
			if (!*slotMetadata) json << "null";
			else
			{
				const auto &slot = **slotMetadata;
				json << "{\"vt\":" << static_cast<unsigned int>(slot.VirtualThreadId.getValue());
				if (slot.SlotMetadata)
					json << ",\"slot\":[" << static_cast<unsigned int>(slot.SlotMetadata->SchemaVersion) << ','
						 << static_cast<unsigned int>(slot.SlotMetadata->BranchHint) << ','
						 << static_cast<unsigned int>(slot.SlotMetadata->StealabilityPolicy) << ','
						 << static_cast<unsigned int>(slot.SlotMetadata->DonorVtHint) << ','
						 << static_cast<unsigned int>(slot.SlotMetadata->LocalityHint) << ','
						 << static_cast<unsigned int>(slot.SlotMetadata->PreferredVt) << ','
						 << static_cast<unsigned int>(slot.SlotMetadata->ThermalHint) << ']';
				if (slot.DmaStreamComputeDescriptor) json << ",\"dmaDescriptor\":" << slot.DmaStreamComputeDescriptor->DescriptorIdentityHash;
				if (slot.AcceleratorCommandDescriptor) json << ",\"acceleratorDescriptor\":" << slot.AcceleratorCommandDescriptor->DescriptorReference.DescriptorIdentityHash;
				json << '}';
			}
		}
		else
		{
			throw std::invalid_argument("Canonical payload type has no deterministic serializer");
		}
		const std::string bytes = json.str();
		const auto payloadBytes = std::span<const unsigned char>(reinterpret_cast<const unsigned char *>(bytes.data()), bytes.size());
		if (const auto *semantics = std::any_cast<std::shared_ptr<DeclarativeInstructionSemantics>>(&value))
			return FromBytes(kind, payloadBytes, std::any(*semantics));
		if (const auto *bundle = std::any_cast<std::shared_ptr<BundleMetadata>>(&value); bundle && *bundle)
			return FromBytes(kind, payloadBytes, std::any(*bundle));
		if (const auto *slot = std::any_cast<std::shared_ptr<HybridCPU_ISE::Arch::InstructionSlotMetadata>>(&value); slot && *slot)
			return FromBytes(kind, payloadBytes, std::any(*slot));
		return FromBytes(kind, payloadBytes);
	}





	CanonicalScalarLoadAddressPlan::CanonicalScalarLoadAddressPlan(std::shared_ptr<GeneratedStaticBinding> generatedBinding, unsigned char destinationRegisterId, unsigned char baseRegisterId, long long signedDisplacement, unsigned char accessSize)
        : GeneratedBinding(std::move(generatedBinding)), DestinationRegisterId(destinationRegisterId),
          BaseRegisterId(baseRegisterId), SignedDisplacement(signedDisplacement), AccessSize(accessSize)
    {
    }

	std::shared_ptr<GeneratedStaticBinding> CanonicalScalarLoadAddressPlan::getGeneratedBinding() const
	{
		return GeneratedBinding;
	}

	unsigned char CanonicalScalarLoadAddressPlan::getDestinationRegisterId() const
	{
		return DestinationRegisterId;
	}

	unsigned char CanonicalScalarLoadAddressPlan::getBaseRegisterId() const
	{
		return BaseRegisterId;
	}

	long long CanonicalScalarLoadAddressPlan::getSignedDisplacement() const
	{
		return SignedDisplacement;
	}

	unsigned char CanonicalScalarLoadAddressPlan::getAccessSize() const
	{
		return AccessSize;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryCreate(CanonicalDecodedInstruction canonical, out CanonicalScalarLoadAddressPlan? plan)
	bool CanonicalScalarLoadAddressPlan::TryCreate(std::shared_ptr<CanonicalDecodedInstruction> canonical, std::shared_ptr<CanonicalScalarLoadAddressPlan> &plan)
	{
		ArgumentNullException::ThrowIfNull(canonical);
		std::shared_ptr<GeneratedStaticBinding> binding = canonical->StaticBinding;
		unsigned char accessSize;
		if (!canonical->IsOccupied || canonical->InstructionClass != InstructionClass::Memory || canonical->SerializationClass != SerializationClass::Free || !binding || binding->Opcode != canonical->Opcode || !TryResolveAccessSize(canonical->Opcode, accessSize))
		{
			plan = nullptr;
			return false;
		}

		plan = std::shared_ptr<CanonicalScalarLoadAddressPlan>(new CanonicalScalarLoadAddressPlan(binding, canonical->Rd, canonical->Rs1, canonical->Immediate, accessSize));
		return true;
	}

	void CanonicalScalarLoadAddressPlan::EnsureMatches(std::shared_ptr<CanonicalDecodedInstruction> canonical)
	{
		std::shared_ptr<CanonicalScalarLoadAddressPlan> expected;
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: if (!TryCreate(canonical, out CanonicalScalarLoadAddressPlan? expected) || expected == null || !ReferenceEquals(GeneratedBinding, expected.GeneratedBinding) || DestinationRegisterId != expected.DestinationRegisterId || BaseRegisterId != expected.BaseRegisterId || SignedDisplacement != expected.SignedDisplacement || AccessSize != expected.AccessSize)
		if (!TryCreate(canonical, expected) || expected == nullptr || getGeneratedBinding() != expected->GeneratedBinding || getDestinationRegisterId() != expected->DestinationRegisterId || getBaseRegisterId() != expected->BaseRegisterId || getSignedDisplacement() != expected->SignedDisplacement || getAccessSize() != expected->AccessSize)
		{
			throw InvalidOperationException(L"Canonical scalar-load address plan does not match its frozen canonical slot and binding.");
		}
	}

	bool CanonicalScalarLoadAddressPlan::TryResolveAccessSize(unsigned int opcode, unsigned char &accessSize)
	{
		int temp_var14;
		switch (opcode)
		{
			case OpcodeValues::LB:
			case OpcodeValues::LBU:
				temp_var14 = 1;
				break;
			case OpcodeValues::LH:
			case OpcodeValues::LHU:
				temp_var14 = 2;
				break;
			case OpcodeValues::LW:
			case OpcodeValues::LWU:
				temp_var14 = 4;
				break;
			case OpcodeValues::LD:
				temp_var14 = 8;
				break;
			default:
				temp_var14 = 0;
				break;
		}
		accessSize = temp_var14;
		return accessSize != 0;
	}

	CanonicalBundle::CanonicalBundle(unsigned long long bundleAddress, unsigned long long bundleSerial, std::vector<std::shared_ptr<CanonicalDecodedInstruction>> slots, std::shared_ptr<CanonicalPayloadSnapshot> bundleSideband, SemanticInstructionKey semanticKey) : BundleAddress(bundleAddress), BundleSerial(bundleSerial), Slots(std::move(slots)), BundleSideband(std::move(bundleSideband)), SemanticKey(std::move(semanticKey)) {}

	unsigned long long CanonicalBundle::getBundleAddress() const
	{
		return BundleAddress;
	}

	unsigned long long CanonicalBundle::getBundleSerial() const
	{
		return BundleSerial;
	}

	std::vector<std::shared_ptr<CanonicalDecodedInstruction>> CanonicalBundle::getSlots() const
	{
		return Slots;
	}

	std::shared_ptr<CanonicalPayloadSnapshot> CanonicalBundle::getBundleSideband() const
	{
		return BundleSideband;
	}

	SemanticInstructionKey CanonicalBundle::getSemanticKey() const
	{
		return SemanticKey;
	}

	bool CanonicalBundle::isReplayEligible() const
	{
		return getSemanticKey().isReplayEligible();
	}

	std::shared_ptr<CanonicalDecodedInstruction> CanonicalBundle::GetSlot(int slotIndex)
	{
		if (slotIndex < 0 || static_cast<std::size_t>(slotIndex) >= Slots.size())
			throw std::out_of_range("slotIndex");
		return Slots[static_cast<std::size_t>(slotIndex)];
	}

	std::shared_ptr<CanonicalBundle> CanonicalBundle::Create(std::span<const VLIW_Instruction> rawSlots, const std::vector<std::shared_ptr<DecodedInstruction>> &decodedSlots, std::shared_ptr<BundleMetadata> bundleMetadata, unsigned long long bundleAddress, unsigned long long bundleSerial, std::shared_ptr<CanonicalDecodeContext> context)
	{
		if (!bundleMetadata) throw std::invalid_argument("bundleMetadata");
		if (decodedSlots.size() != BundleMetadata::BundleSlotCount)
			throw std::invalid_argument("Canonical bundles require exactly eight normalized decoded slots.");

		std::vector<unsigned char> rawBundle(BundleMetadata::BundleSlotCount * 32);
		std::vector<std::shared_ptr<CanonicalDecodedInstruction>> slots;
		slots.reserve(BundleMetadata::BundleSlotCount);
		for (int slotIndex = 0; slotIndex < BundleMetadata::BundleSlotCount; ++slotIndex)
		{
			VLIW_Instruction rawSlot = slotIndex < static_cast<int>(rawSlots.size()) ? rawSlots[slotIndex] : VLIW_Instruction();
			const std::span<unsigned char> rawSlotBytes(rawBundle.data() + slotIndex * 32, 32);
			if (!rawSlot.TryWriteBytes(rawSlotBytes))
				throw InvalidOperationException(L"Fixed-width VLIW raw-slot serialization unexpectedly failed.");

			const std::shared_ptr<DecodedInstruction> decoded = decodedSlots[slotIndex];
			if (!decoded) throw std::invalid_argument("Decoded slots cannot contain null.");
			if (decoded->SlotIndex != slotIndex)
				throw std::invalid_argument("Decoded slots must be normalized into physical slot order.");

			const std::shared_ptr<InstructionIR> instruction = decoded->Instruction;
			const bool occupied = static_cast<bool>(instruction);
			const unsigned int opcode = occupied && instruction->CanonicalOpcode ? instruction->CanonicalOpcode->Value : 0u;
			const auto instructionClass = occupied ? std::optional<YAKSys_Hybrid_CPU::Arch::InstructionClass>(instruction->Class) : std::nullopt;
			const auto serializationClass = occupied ? std::optional<YAKSys_Hybrid_CPU::Arch::SerializationClass>(instruction->SerializationClass) : std::nullopt;
			std::shared_ptr<CanonicalDecodedInstruction> canonicalSlot = std::make_shared<CanonicalDecodedInstruction>(
				slotIndex, occupied, opcode, instructionClass, serializationClass,
				occupied ? instruction->Rd : 0, occupied ? instruction->Rs1 : 0, occupied ? instruction->Rs2 : 0,
				occupied ? instruction->Imm : 0, occupied ? instruction->CsrAddress : std::nullopt,
				occupied && instruction->AcquireOrdering, occupied && instruction->ReleaseOrdering,
				CanonicalPayloadSnapshot::FromBytes(L"VLIW_Instruction", rawSlotBytes),
				CanonicalPayloadSnapshot::FromObject(L"InstructionIR", std::any(instruction)),
				CanonicalPayloadSnapshot::FromObject(L"InstructionSlotMetadata", std::any(decoded->SlotMetadata)));

			std::shared_ptr<GeneratedStaticBinding> binding;
			if (instruction && instruction->CanonicalOpcode && GeneratedStaticBinding::TryFromOpcode(opcode, binding))
			{
				canonicalSlot->StaticBinding = binding;
				std::shared_ptr<CanonicalScalarLoadAddressPlan> loadPlan;
				if (CanonicalScalarLoadAddressPlan::TryCreate(canonicalSlot, loadPlan))
					canonicalSlot->ScalarLoadAddressPlan = loadPlan;
			}
			slots.push_back(std::move(canonicalSlot));
		}

		auto bundleSideband = CanonicalPayloadSnapshot::FromObject(L"BundleMetadata", std::any(bundleMetadata));
		const std::wstring annotationHash = ComputeAnnotationsHash(bundleSideband, slots);
		const SemanticInstructionKey key = SemanticInstructionKey::Create(std::span<const unsigned char>(rawBundle.data(), rawBundle.size()), annotationHash, context ? context : CanonicalDecodeContext::Unbound);
		return std::shared_ptr<CanonicalBundle>(new CanonicalBundle(bundleAddress, bundleSerial, std::move(slots), bundleSideband, key));
	}

	std::shared_ptr<CanonicalBundle> CanonicalBundle::CreateFromCanonicalSlots(std::span<const VLIW_Instruction> rawSlots, const std::vector<std::shared_ptr<CanonicalDecodedInstruction>> &canonicalSlots, std::shared_ptr<BundleMetadata> bundleMetadata, unsigned long long bundleAddress, unsigned long long bundleSerial, std::shared_ptr<CanonicalDecodeContext> context)
	{
		if (!bundleMetadata) throw std::invalid_argument("bundleMetadata");
		if (rawSlots.size() != BundleMetadata::BundleSlotCount || canonicalSlots.size() != BundleMetadata::BundleSlotCount)
			throw std::invalid_argument("Canonical bundles require exactly eight raw and decoded slots.");

		std::vector<unsigned char> rawBundle(BundleMetadata::BundleSlotCount * 32);
		std::vector<std::shared_ptr<CanonicalDecodedInstruction>> slots;
		slots.reserve(BundleMetadata::BundleSlotCount);
		for (int slotIndex = 0; slotIndex < BundleMetadata::BundleSlotCount; ++slotIndex)
		{
			const std::span<unsigned char> rawSlotBytes(rawBundle.data() + slotIndex * 32, 32);
			VLIW_Instruction rawSlot = rawSlots[slotIndex];
			if (!rawSlot.TryWriteBytes(rawSlotBytes))
				throw InvalidOperationException(L"Fixed-width VLIW raw-slot serialization unexpectedly failed.");
			const auto &slot = canonicalSlots[slotIndex];
			if (!slot) throw std::invalid_argument("Canonical slots cannot contain null.");
			if (slot->SlotIndex != slotIndex)
				throw std::invalid_argument("Canonical slots must be normalized into physical slot order.");

			std::shared_ptr<CanonicalScalarLoadAddressPlan> derivedLoadPlan;
			if (CanonicalScalarLoadAddressPlan::TryCreate(slot, derivedLoadPlan))
			{
				if (!slot->ScalarLoadAddressPlan) slot->ScalarLoadAddressPlan = derivedLoadPlan;
				else slot->ScalarLoadAddressPlan->EnsureMatches(slot);
			}
			else if (slot->ScalarLoadAddressPlan)
				throw InvalidOperationException(L"A canonical non-load slot cannot carry a scalar-load address plan.");
			slots.push_back(slot);
		}

		auto bundleSideband = CanonicalPayloadSnapshot::FromObject(L"BundleMetadata", std::any(bundleMetadata));
		const std::wstring annotationHash = ComputeAnnotationsHash(bundleSideband, slots);
		const SemanticInstructionKey key = SemanticInstructionKey::Create(std::span<const unsigned char>(rawBundle.data(), rawBundle.size()), annotationHash, context ? context : CanonicalDecodeContext::Unbound);
		return std::shared_ptr<CanonicalBundle>(new CanonicalBundle(bundleAddress, bundleSerial, std::move(slots), bundleSideband, key));
	}
std::wstring CanonicalBundle::ComputeAnnotationsHash(std::shared_ptr<CanonicalPayloadSnapshot> bundleSideband, const std::vector<std::shared_ptr<CanonicalDecodedInstruction>> &slots)
	{
		std::shared_ptr<StringBuilder> text = std::make_shared<StringBuilder>(bundleSideband->getContentSha256());
		for (int slotIndex = 0; slotIndex < slots.size(); slotIndex++)
		{
			text->append(L'|');
			text->append(slots[slotIndex]->SlotSideband->getContentSha256());
		}

		const std::wstring value = text->toString();
		const std::vector<unsigned char> bytes(value.begin(), value.end());
		return CanonicalPayloadSnapshot::Hash(std::span<const unsigned char>(bytes.data(), bytes.size()));
	}
}
