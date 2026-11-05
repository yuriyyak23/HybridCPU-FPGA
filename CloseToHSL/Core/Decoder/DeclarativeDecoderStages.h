#pragma once

#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "../Pipeline/MicroOps/IR/InstructionIR.h"
#include "../../../NonRTL/Arch/InstructionClass.h"
#include "../../../NonRTL/Arch/SerializationClass.h"
#include "../Contracts/CompilerTransport/VliwBundleAnnotations.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <any>
#include <optional>
#include <format>
#include "tangible_exception_helper.h"
#include <memory>
#include <functional>
#include <span>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::Core::Decoder { class IEncodingConstraint; struct RawSlot; struct DecodedOperandFields; }

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Arch::Generated;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{

	/// <summary>
	/// RF-05 raw frontend value contracts. These types perform no semantic
	/// reconstruction and deliberately contain no mutable decoder context.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct RawSlot(int SlotIndex, VLIW_Instruction Instruction, ushort Opcode, ulong Word0, ulong Word1, ulong Word2, ulong Word3);

		struct RawSlot final { const int SlotIndex; const VLIW_Instruction Instruction; const unsigned short Opcode; const unsigned long long Word0,Word1,Word2,Word3; RawSlot(int slotIndex,VLIW_Instruction instruction,unsigned short opcode,unsigned long long word0,unsigned long long word1,unsigned long long word2,unsigned long long word3):SlotIndex(slotIndex),Instruction(instruction),Opcode(opcode),Word0(word0),Word1(word1),Word2(word2),Word3(word3){} };
/// <summary>Fixed-width raw bundle passed between the first two decoder stages.</summary>
	class RawBundle final : public std::enable_shared_from_this<RawBundle>
	{
	private:
		const std::vector<std::shared_ptr<RawSlot>> Slots;

	public:
		RawBundle(std::vector<std::shared_ptr<RawSlot>> slots);

		std::vector<std::shared_ptr<RawSlot>> getSlots() const;
	};

	/// <summary>Raw bundle reader: copies the eight frontend slots without legality decisions.</summary>
	class RawBundleReader final
	{
	public:
		static std::shared_ptr<RawBundle> Read(std::span<const VLIW_Instruction> bundle);
	};

	/// <summary>Raw-slot reader: field extraction only.</summary>
	class RawSlotReader final
	{
	public:
		static std::shared_ptr<RawSlot> Read(VLIW_Instruction const instruction, int slotIndex);
	};

	/// <summary>Generated static descriptor lookup; dynamic legality remains with RF-05 validators.</summary>
	class OpcodeDescriptorLookup final
	{
		// These optional contours deliberately have no generated executable row.
		// Keep their raw identities explicit so that a known fail-closed contour is
		// never reported as an unknown encoding during the RF-05 shadow period.
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyDictionary<ushort, string> UnsupportedOptionalContours = new Dictionary<ushort, string> {{ 45, "XSQRT" }, { 52, "NOT" }, { 55, "XFMAC" }};
		static inline const std::unordered_map<unsigned short, std::wstring> UnsupportedOptionalContours = std::unordered_map<unsigned short, std::wstring>
		{
			{45, L"XSQRT"},
			{52, L"NOT"},
			{55, L"XFMAC"}
		};

	public:
		static bool TryLookup(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> &descriptor, std::optional<std::shared_ptr<DecodeFailure>> &failure);

	private:
		static std::vector<unsigned char> RawSlotBytes(std::shared_ptr<RawSlot> slot);
	};

	/// <summary>
	/// Frozen canonical operand fields obtained directly from the raw slot ABI.
	/// This stage has no sideband, dynamic-legality, or materialization knowledge.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct DecodedOperandFields(byte Rd, byte Rs1, byte Rs2, long Immediate);

		struct DecodedOperandFields final { const unsigned char Rd,Rs1,Rs2; const long long Immediate; DecodedOperandFields(unsigned char rd=0,unsigned char rs1=0,unsigned char rs2=0,long long immediate=0):Rd(rd),Rs1(rs1),Rs2(rs2),Immediate(immediate){} };
/// <summary>
	/// Declarative operand decoder. The descriptor selects the static operand ABI;
	/// expected malformed packed-register encodings become typed failures.
	/// </summary>
	class OperandDecoder final
	{
	public:
		static bool TryDecode(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> &operands, std::optional<std::shared_ptr<DecodeFailure>> &failure);

	private:
		static bool TryValidateRequiredRf06RegisterOperands(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, unsigned char rd, unsigned char rs1, unsigned char rs2, std::optional<std::shared_ptr<DecodeFailure>> &failure);

		static std::vector<unsigned char> RawSlotBytes(std::shared_ptr<RawSlot> slot);
	};

	/// <summary>
	/// RF-05 named static encoding-legality registry.  These rules intentionally
	/// describe only raw, opcode-local ABI constraints: they do not own sideband,
	/// privilege, resource, or other runtime-dependent legality.
	/// </summary>
	class EncodingConstraintValidator final
	{
	private:
		class IEncodingConstraint;
		struct RawEncodingRule;
		class StaticConstructor final
		{
		public:
			StaticConstructor();
		};
		static const std::vector<std::shared_ptr<IEncodingConstraint>> Constraints;
		static const std::vector<std::shared_ptr<RawEncodingRule>> RawFormConstraints;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<string> RegisteredConstraintIds = Array.ConvertAll(Constraints, static constraint => constraint.Id);


	public:
		static const std::vector<std::wstring> getRegisteredConstraintIds();

		static const std::vector<std::wstring> getRegisteredRawFormConstraintIds();

		/// <summary>
		/// Validates raw encoding fields that are independent of decoded operands.
		/// These named checks retain typed failure semantics while the legacy body is
		/// still the public reject authority.
		/// </summary>
		static bool TryValidateRawForm(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::optional<std::shared_ptr<DecodeFailure>> &failure);

		static bool TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure);

	private:
		class IEncodingConstraint
		{
		public:
			virtual std::wstring getId() const = 0;

			virtual bool AppliesTo(unsigned short opcode) = 0;

			virtual bool TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure) = 0;
		};

		using RawEncodingRuleValidator = std::function<std::optional<std::shared_ptr<DecodeFailure>>(std::shared_ptr<RawSlot>, std::shared_ptr<GeneratedIsaDescriptor>)>;
		struct RawEncodingRule final { const std::wstring Id; const RawEncodingRuleValidator Validate; RawEncodingRule(const std::wstring& id, RawEncodingRuleValidator validate) : Id(id), Validate(std::move(validate)) {} };

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//		private sealed record RawEncodingRule(string Id, RawEncodingRuleValidator Validate);

	private:
		static std::optional<std::shared_ptr<DecodeFailure>> ValidateReservedWord0(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor);

		static std::optional<std::shared_ptr<DecodeFailure>> ValidateRetiredPolicyGap(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor);

		static std::optional<std::shared_ptr<DecodeFailure>> ValidateOpcodeFlags(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor);

		static std::optional<std::shared_ptr<DecodeFailure>> ValidateFencePayload(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor);

	private:
		class RegisterConstraint final : public IEncodingConstraint
		{
		private:
			const std::wstring Id;

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly HashSet<ushort> _opcodes;
			std::unordered_set<unsigned short> _opcodes;
			const bool _requireRs1Zero;
			const bool _requireRs2Zero;
			const bool _requireImmediateZero;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly System.Nullable<ushort> _maximumImmediate;
			std::optional<unsigned short> _maximumImmediate;

			RegisterConstraint(const std::wstring &id, bool requireRs1Zero, bool requireRs2Zero, bool requireImmediateZero, std::optional<unsigned short> maximumImmediate, const std::vector<unsigned short> &opcodes);

		public:
			std::wstring getId() const override;

			static std::shared_ptr<RegisterConstraint> RequireRs2Zero(const std::wstring &id, const std::vector<unsigned short> &opcodes);

			static std::shared_ptr<RegisterConstraint> RequireRs2AndImmediateZero(const std::wstring &id, const std::vector<unsigned short> &opcodes);

			static std::shared_ptr<RegisterConstraint> RequireImmediateZero(const std::wstring &id, const std::vector<unsigned short> &opcodes);

			static std::shared_ptr<RegisterConstraint> RequireRs2ZeroAndImmediateAtMost(const std::wstring &id, unsigned short maximumImmediate, const std::vector<unsigned short> &opcodes);

			static std::shared_ptr<RegisterConstraint> RequireRs1Rs2AndImmediateZero(const std::wstring &id, const std::vector<unsigned short> &opcodes);

			bool AppliesTo(unsigned short opcode) override;

			bool TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure) override;
		};

	private:
		class ControlFlowTargetTransportConstraint final : public IEncodingConstraint
		{
		public:
			std::wstring getId() const override;

			bool AppliesTo(unsigned short opcode) override;

			bool TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure) override;
		};

		/// <summary>
		/// The queue-control DSC opcodes are register ABI variants, not descriptor
		/// carriers. Their fixed source-zero and clean-carrier fields are static
		/// encoding legality, while queue state remains runtime authority.
		/// </summary>
	private:
		class DscQueueControlConstraint final : public IEncodingConstraint
		{
		public:
			std::wstring getId() const override;

			bool AppliesTo(unsigned short opcode) override;

			bool TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<DecodeFailure>> &failure) override;
		};

	private:
		static inline StaticConstructor staticConstructor{};

	private:
		static std::shared_ptr<DecodeFailure> CreateFailure(std::shared_ptr<RawSlot> slot, const std::wstring &mnemonic, const std::wstring &field, const std::wstring &requirement);

		static std::vector<unsigned char> RawSlotBytes(std::shared_ptr<RawSlot> slot);
	};

	/// <summary>
	/// RF-05 extension-payload projection.  It consumes only the frozen raw slot
	/// and frozen compiler transport sideband; it neither grants execution nor
	/// performs matrix/materialization work.
	/// </summary>
	class ExtensionPayloadDecoder final
	{
	public:
		static bool TryDecode(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::optional<std::shared_ptr<VectorInstructionPayload>> &vectorPayload, std::optional<std::shared_ptr<DecodeFailure>> &failure);
	};

	/// <summary>
	/// RF-05 static sideband-placement validator.  It owns only carrier-to-opcode
	/// association and fixed lane placement. Descriptor parsing, owner/domain
	/// guards and other state-dependent checks deliberately remain outside it.
	/// </summary>
	class SidebandValidator final
	{
	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static readonly IReadOnlyList<string> RegisteredStaticRuleIds = ["dma-descriptor-opcode-association", "accelerator-descriptor-opcode-association", "dma-fixed-lane", "dma-required-descriptor", "dma-native-carrier-abi", "dsc-queue-control-fixed-lane", "accelerator-submit-required-descriptor", "accelerator-native-carrier-abi",];
		static inline const std::vector<std::wstring> RegisteredStaticRuleIds = {L"dma-descriptor-opcode-association", L"accelerator-descriptor-opcode-association", L"dma-fixed-lane", L"dma-required-descriptor", L"dma-native-carrier-abi", L"dsc-queue-control-fixed-lane", L"accelerator-submit-required-descriptor", L"accelerator-native-carrier-abi"};

		/// <summary>
		/// Stable identities for the static descriptor-carrier rules owned by this
		/// validator. They are diagnostic/test identities, not a configuration
		/// surface and not an authority for runtime descriptor admission.
		/// </summary>
	public:
		static const std::vector<std::wstring> getRegisteredStaticRuleIds();

		static bool TryValidate(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::optional<std::shared_ptr<DecodeFailure>> &failure);

	private:
		static std::shared_ptr<DecodeFailure> CreateFailure(std::shared_ptr<RawSlot> slot, const std::wstring &mnemonic, const std::wstring &field, const std::wstring &message);

		static std::vector<unsigned char> RawSlotBytes(std::shared_ptr<RawSlot> slot);
	};

	/// <summary>
	/// Immutable, pre-materialization semantic payload emitted by the RF-05
	/// decoder stages. It is intentionally not an <see cref="InstructionIR"/>:
	/// the legacy IR remains a compatibility projection until the cutover gate.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DeclarativeInstructionSemantics(ushort Opcode, InstructionClass InstructionClass, SerializationClass SerializationClass, byte Rd, byte Rs1, byte Rs2, long Immediate, System.Nullable<VectorInstructionPayload> VectorPayload);

	struct DeclarativeInstructionSemantics final
	{
		const unsigned short Opcode;
		const YAKSys_Hybrid_CPU::Arch::InstructionClass InstructionClass;
		const YAKSys_Hybrid_CPU::Arch::SerializationClass SerializationClass;
		const unsigned char Rd, Rs1, Rs2;
		const long long Immediate;
		const std::optional<std::shared_ptr<VectorInstructionPayload>> VectorPayload;

		DeclarativeInstructionSemantics(unsigned short opcode, YAKSys_Hybrid_CPU::Arch::InstructionClass instructionClass,
			YAKSys_Hybrid_CPU::Arch::SerializationClass serializationClass, unsigned char rd, unsigned char rs1,
			unsigned char rs2, long long immediate,
			std::optional<std::shared_ptr<VectorInstructionPayload>> vectorPayload)
			: Opcode(opcode), InstructionClass(instructionClass), SerializationClass(serializationClass),
			  Rd(rd), Rs1(rs1), Rs2(rs2), Immediate(immediate), VectorPayload(std::move(vectorPayload)) {}
	};
	/// <summary>
	/// Final RF-05 stage for one occupied slot. It creates an immutable canonical
	/// contract from prior stage outputs and never reconstructs a mutable legacy
	/// <see cref="InstructionIR"/>.
	/// </summary>
	class CanonicalInstructionIrBuilder final
	{
	public:
		static bool TryBuild(std::shared_ptr<RawSlot> slot, std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<VectorInstructionPayload>> &vectorPayload, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::shared_ptr<CanonicalDecodedInstruction> &canonicalInstruction, std::optional<std::shared_ptr<DecodeFailure>> &failure);

	private:
		static std::vector<unsigned char> RawSlotBytes(std::shared_ptr<RawSlot> slot);

		static std::optional<unsigned short> ResolveCsrAddress(InstructionClass instructionClass, unsigned short opcode, long long immediate);
	};

	/// <summary>
	/// Frozen output of the complete RF-05 occupied-slot pipeline. The public
	/// facade consumes this only in shadow mode until the differential gate closes.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DeclarativeDecodedSlot(GeneratedIsaDescriptor Descriptor, DecodedOperandFields Operands, System.Nullable<VectorInstructionPayload> VectorPayload, CanonicalDecodedInstruction CanonicalInstruction);
	struct DeclarativeDecodedSlot final
	{
		std::shared_ptr<GeneratedIsaDescriptor> Descriptor;
		std::shared_ptr<DecodedOperandFields> Operands;
		std::optional<std::shared_ptr<VectorInstructionPayload>> VectorPayload;
		std::shared_ptr<CanonicalDecodedInstruction> CanonicalInstruction;
		DeclarativeDecodedSlot(std::shared_ptr<GeneratedIsaDescriptor> descriptor, std::shared_ptr<DecodedOperandFields> operands, std::optional<std::shared_ptr<VectorInstructionPayload>> vectorPayload, std::shared_ptr<CanonicalDecodedInstruction> canonicalInstruction)
			: Descriptor(std::move(descriptor)), Operands(std::move(operands)), VectorPayload(std::move(vectorPayload)), CanonicalInstruction(std::move(canonicalInstruction)) {}
	};

	/// <summary>
	/// Frozen result of the RF-05 bundle pipeline. Empty physical slots remain
	/// explicit canonical slots and do not carry a synthetic semantic instruction.
	/// </summary>
// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public sealed record DeclarativeDecodedBundle(ImmutableArray<System.Nullable<DeclarativeDecodedSlot>> Slots, CanonicalBundle CanonicalBundle);
	struct DeclarativeDecodedBundle final
	{
		std::vector<std::optional<std::shared_ptr<DeclarativeDecodedSlot>>> Slots;
		std::shared_ptr<CanonicalBundle> CanonicalBundleValue;
		DeclarativeDecodedBundle(std::vector<std::optional<std::shared_ptr<DeclarativeDecodedSlot>>> slots, std::shared_ptr<CanonicalBundle> canonicalBundle)
			: Slots(std::move(slots)), CanonicalBundleValue(std::move(canonicalBundle)) {}
	};

	/// <summary>
	/// One declarative, non-materializing decode path for a single occupied slot.
	/// Expected illegal input is returned as <see cref="DecodeFailure"/>; only a
	/// programming invariant may escape this boundary as an exception.
	/// </summary>
	class DeclarativeDecoderPipeline final
	{
	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryDecodeBundle(std::span<const VLIW_Instruction> bundle, VliwBundleAnnotations? bundleAnnotations, ulong bundleAddress, ulong bundleSerial, out System.Nullable<DeclarativeDecodedBundle> decodedBundle, out System.Nullable<DecodeFailure> failure)
		static bool TryDecodeBundle(std::span<const VLIW_Instruction> bundle, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, unsigned long long bundleAddress, unsigned long long bundleSerial, std::optional<std::shared_ptr<DeclarativeDecodedBundle>> &decodedBundle, std::optional<std::shared_ptr<DecodeFailure>> &failure);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static bool TryDecodeBundle(std::span<const VLIW_Instruction> bundle, VliwBundleAnnotations? bundleAnnotations, ulong bundleAddress, ulong bundleSerial, System.Nullable<CanonicalDecodeContext> decodeContext, out System.Nullable<DeclarativeDecodedBundle> decodedBundle, out System.Nullable<DecodeFailure> failure)
		static bool TryDecodeBundle(std::span<const VLIW_Instruction> bundle, std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, unsigned long long bundleAddress, unsigned long long bundleSerial, std::optional<std::shared_ptr<CanonicalDecodeContext>> &decodeContext, std::optional<std::shared_ptr<DeclarativeDecodedBundle>> &decodedBundle, std::optional<std::shared_ptr<DecodeFailure>> &failure);

		static bool TryDecodeOccupiedSlot(std::shared_ptr<RawSlot> slot, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::optional<std::shared_ptr<DeclarativeDecodedSlot>> &decodedSlot, std::optional<std::shared_ptr<DecodeFailure>> &failure);

	private:
		static std::vector<unsigned char> RawSlotBytes(std::shared_ptr<RawSlot> slot);

		static bool TryDecodeEmptySlot(std::shared_ptr<RawSlot> slot, std::shared_ptr<InstructionSlotMetadata> slotMetadata, std::shared_ptr<CanonicalDecodedInstruction> &canonicalInstruction, std::optional<std::shared_ptr<DecodeFailure>> &failure);

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: private static InstructionSlotMetadata ResolveSlotMetadata(VliwBundleAnnotations? bundleAnnotations, int slotIndex)
		static std::shared_ptr<InstructionSlotMetadata> ResolveSlotMetadata(std::shared_ptr<VliwBundleAnnotations> bundleAnnotations, int slotIndex);
	};

}






