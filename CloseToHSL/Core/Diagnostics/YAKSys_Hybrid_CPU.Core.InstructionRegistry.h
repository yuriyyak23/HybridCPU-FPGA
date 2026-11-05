#pragma once

#include "../Architecture/Common/YAKSys_Hybrid_CPU.Processor.h"
#include "InstructionRegistry.Types.h"
#include "../../../NonRTL/Arch/InvalidOpcodeException.h"
#include "../Pipeline/MicroOps/Control/MicroOp.Control.h"
#include "../Pipeline/MicroOps/System/MicroOp.System.h"
#include "../Virtualization/Compatibility/Frontend/Retire/VmxRetireModel.h"
#include "../Architecture/Registers/Architectural/ArchRegisterTripletEncoding.h"
#include "../../../NonRTL/Arch/OpcodeInfo.Types.h"
#include "../ISA/Instructions/NonVmx/Lanes00_03Vector/MatrixTile/MatrixTileIrProjectionAndMaterializer.h"
#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../Pipeline/MicroOps/Lane7Accelerator/SystemDeviceCommandMicroOp.h"
#include "../Pipeline/MicroOps/Types/MicroOp.h"
#include "../Pipeline/Certificates/SafetyMask128.h"
#include "../Pipeline/Scheduling/SlotPlacementMetadata.h"
#include "../../../NonRTL/Arch/SerializationClass.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <limits>
#include <functional>
#include <optional>
#include <format>
#include <concepts>
#include "tangible_exception_helper.h"
#include <memory>
#include <atomic>
#include "../../../tangible_string_helper.h"

using namespace YAKSys_Hybrid_CPU::Arch;


using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;
using namespace YAKSys_Hybrid_CPU::Core::Registers;


using namespace YAKSys_Hybrid_CPU::Core::Legality;

using namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile;
// C# TO C++ CONVERTER TASK: The C# 'using static' statement  cannot be converted to C++:
//using static YAKSys_Hybrid_CPU.Processor.CPU_Core;








using namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata;

namespace YAKSys_Hybrid_CPU::Core
{
	/// <summary>
	/// Centralized registry for instruction-to-MicroOp mapping.
	///
	/// Purpose: Eliminate large switch-case statements in decoder by using registration pattern.
	/// Benefits:
	/// - Easier to add new instructions (register once, use everywhere)
	/// - Clear separation between instruction metadata and execution logic
	/// - Supports runtime extension (can register new instructions dynamically)
	/// - Facilitates testing (can mock/override specific instructions)
	///
	/// Usage:
	///   1. At startup: InstructionRegistry.Initialize()
	///   2. To decode: MicroOp uop = InstructionRegistry.CreateMicroOp(opcode, context)
	///   3. To add a new instruction: InstructionRegistry.RegisterSemanticFactory(opcode, factory)
	///      plus InstructionRegistry.RegisterOpAttributes(opcode, descriptor)
	/// </summary>
	class InstructionRegistry final
	{
		/// <summary>
		/// Clear all registered factories for testing.
		/// </summary>
	public:
		static void Clear();

		/// <summary>
		/// Get the count of registered instructions.
		/// </summary>
		static int GetRegisteredCount();

		static const std::vector<unsigned int> GetRegisteredOpcodes();

		/// <summary>
		/// Register a retained custom-accelerator descriptor surface.
		/// The current runtime tracks the opcode family for diagnostics and negative
		/// controls only; it still fails closed for canonical decode, direct factory
		/// publication, and execution until a truthful accelerator carrier contract exists.
		/// </summary>
		/// <param name="accelerator">Custom accelerator descriptor/test fixture.</param>
		static void RegisterAccelerator(std::shared_ptr<ICustomAccelerator> accelerator);

		static bool IsCustomAcceleratorOpcode(unsigned int opCode);

		static std::shared_ptr<InvalidOpcodeException> CreateUnsupportedCustomAcceleratorException(unsigned int opcode, const std::wstring &opcodeIdentifier, int slotIndex);

		static std::shared_ptr<InvalidOpcodeException> CreateUnsupportedCustomAcceleratorException(unsigned int opcode);

		/// <summary>
		/// Get a registered descriptor/test fixture by name.
		/// The returned object is not execution authority.
		/// </summary>
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public static ICustomAccelerator? GetAccelerator(string name)
		static std::shared_ptr<ICustomAccelerator> GetAccelerator(const std::wstring &name);

		/// <summary>
		/// Get all registered descriptor/test fixtures.
		/// Registry membership is not canonical decode or execution authority.
		/// </summary>
		static const std::unordered_map<std::wstring, std::shared_ptr<ICustomAccelerator>> GetAllAccelerators();


		// Reserved high-word bits used only as a structural backstop when legacy
		// producer metadata would otherwise emit a zero SafetyMask. These bits keep
		// mask-era compatibility code honest without reintroducing policy into the
		// bundle encoding or widening the mask format.
	private:
		static constexpr unsigned long long StructuralFallbackSerializedOpBitHigh = 1ULL << 61;
		static constexpr unsigned long long StructuralFallbackDmaStreamLaneBitHigh = 1ULL << 62;
		static constexpr unsigned long long StructuralFallbackLane7SingletonBitHigh = 1ULL << 63;

		static inline std::unordered_map<unsigned int, MicroOpFactory> _factories = std::unordered_map<unsigned int, MicroOpFactory>();
		static inline std::unordered_map<unsigned int, std::shared_ptr<MicroOpDescriptor>> _descriptors = std::unordered_map<unsigned int, std::shared_ptr<MicroOpDescriptor>>();
		static inline std::unordered_map<std::wstring, std::shared_ptr<ICustomAccelerator>> _customAccelerators = std::unordered_map<std::wstring, std::shared_ptr<ICustomAccelerator>>();
		static inline std::unordered_set<unsigned int> _customAcceleratorOpcodes = std::unordered_set<unsigned int>();
		static inline bool _initialized = false;


		static unsigned long long SignExtendScalarImmediate(unsigned short rawImmediate);

		static void RejectScalarImmediateRegisterFormAlias(DecoderContext const ctx);

		static void RejectScalarUnaryRegisterFormAlias(DecoderContext const ctx);

		static void RejectScalarAddressGenerationRegisterFormAlias(DecoderContext const ctx);

		static unsigned short GetScalarAddressGenerationImmediate(DecoderContext const ctx);

		static void RejectScalarCarryLessRegisterFormAlias(DecoderContext const ctx);

		static void RejectScalarRotateRegisterFormAlias(DecoderContext const ctx);

		static unsigned short GetScalarRotateImmediate(DecoderContext const ctx);

		static void RejectScalarBooleanInvertRegisterFormAlias(DecoderContext const ctx);

		static void RejectScalarBitfieldRegisterFormAlias(DecoderContext const ctx);

		static unsigned short GetScalarBitfieldImmediate(DecoderContext const ctx);

		static void RejectScalarMinMaxRegisterFormAlias(DecoderContext const ctx);

		static void RejectScalarZeroingSelectRegisterFormAlias(DecoderContext const ctx);

		static unsigned short GetRequiredDecoderImmediate(DecoderContext const ctx, const std::wstring &contourName);

		static unsigned char GetRequiredDecoderDataType(DecoderContext const ctx, const std::wstring &contourName);

		static unsigned long long GetRequiredDecoderMemoryAddress(DecoderContext const ctx, const std::wstring &contourName);

		static bool TryDecodeCanonicalArchRegister(unsigned short rawRegId, unsigned char &registerId);

		static bool TryDecodeCanonicalOrUnpackedNoArchRegister(unsigned short rawRegId, unsigned char &registerId);

		static bool TryResolveCanonicalBranchPackedRegisters(DecoderContext const ctx, unsigned char &rd, unsigned char &rs1, unsigned char &rs2);

		static void ApplyCanonicalDirectBranchProjection(std::shared_ptr<BranchMicroOp> branchMicroOp, DecoderContext const ctx);

		static unsigned short ResolveOpcode(std::shared_ptr<InstructionIR> instruction);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static bool TryCreatePublishedControlFlowMicroOp(in InstructionIR instruction, out BranchMicroOp? branchMicroOp)
		static bool TryCreatePublishedControlFlowMicroOp(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<BranchMicroOp> &branchMicroOp);

		static bool TryResolvePublishedSystemEventKind(std::shared_ptr<InstructionIR> instruction, SystemEventKind &systemEventKind);

		static bool TryResolvePublishedSystemReadRegisters(std::shared_ptr<InstructionIR> instruction, std::vector<int> &readRegisters);

		static bool TryResolvePublishedStreamControlRetireContour(std::shared_ptr<InstructionIR> instruction, bool &requiresSerializingBoundaryFollowThrough);

		static bool TryResolvePublishedAtomicAccessSize(std::shared_ptr<InstructionIR> instruction, unsigned char &accessSize);

		static bool TryResolvePublishedVmxOperationKind(std::shared_ptr<InstructionIR> instruction, VmxOperationKind &operationKind);

	private:
		static bool TryResolvePublishedSystemEventKindFromOpcode(unsigned short opcode, SystemEventKind &systemEventKind);

		static bool TryResolvePublishedSystemReadRegistersFromOpcode(unsigned short opcode, unsigned char rs1, unsigned char rs2, std::vector<int> &readRegisters);

		static bool TryResolvePublishedStreamControlRetireContourFromOpcode(unsigned short opcode, bool &requiresSerializingBoundaryFollowThrough);

		static bool TryResolvePublishedAtomicAccessSizeFromOpcode(unsigned short opcode, unsigned char &accessSize);

		static bool TryResolvePublishedVmxOperationKindFromOpcode(unsigned short opcode, VmxOperationKind &operationKind);

		static const std::vector<int> BuildArchitecturalReadRegisterList(unsigned char first, unsigned char second = ArchRegisterTripletEncoding::NoArchReg);

		static bool HasArchitecturalRegister(unsigned char registerId);

		static unsigned short ToLegacyDecoderField(unsigned char registerId);

		static void ApplyCanonicalPublishedBranchProjection(std::shared_ptr<BranchMicroOp> branchMicroOp, unsigned short opcode, unsigned char rd, unsigned char rs1, unsigned char rs2);

		static OpcodeInfo RequirePublishedOpcodeInfo(unsigned int opCode);

		// Keep the implementation's mutable-reference entry point, and provide a
		// value-taking adapter so nullable arguments (including std::nullopt and
		// bool literals) can be passed like the C# optional values they represent.
		static std::shared_ptr<MicroOpDescriptor> CreatePublishedSystemLikeDescriptor(unsigned int opCode, std::optional<bool> &writesRegister, std::optional<bool> &isMemoryOp, int memFootprintClass);
		static std::shared_ptr<MicroOpDescriptor> CreatePublishedSystemLikeDescriptor(unsigned int opCode, std::optional<bool> writesRegister = {}, std::optional<bool> isMemoryOp = {}, int memFootprintClass = 0)
		{
			using Implementation = std::shared_ptr<MicroOpDescriptor>(*)(unsigned int, std::optional<bool> &, std::optional<bool> &, int);
			return (static_cast<Implementation>(&InstructionRegistry::CreatePublishedSystemLikeDescriptor))(opCode, writesRegister, isMemoryOp, memFootprintClass);
		}

		static std::shared_ptr<MicroOpDescriptor> CreatePublishedScalarDescriptor(unsigned int opCode);

		static void RegisterPublishedDescriptorOnly(unsigned int opCode, int memFootprintClass, std::optional<bool> &writesRegister, std::optional<bool> &isMemoryOp);
		static void RegisterPublishedDescriptorOnly(unsigned int opCode, int memFootprintClass, std::optional<bool> writesRegister = {}, std::optional<bool> isMemoryOp = {})
		{
			using Implementation = void(*)(unsigned int, int, std::optional<bool> &, std::optional<bool> &);
			(static_cast<Implementation>(&InstructionRegistry::RegisterPublishedDescriptorOnly))(opCode, memFootprintClass, writesRegister, isMemoryOp);
		}

		static void RegisterScalarRegisterOp(unsigned int opCode);

		static void RegisterScalarAddressGenerationRegisterOp(unsigned int opCode);

		static void RegisterScalarAddressGenerationImmediateOp(unsigned int opCode);

		static void RegisterScalarCarryLessRegisterOp(unsigned int opCode);

		static void RegisterScalarRotateRegisterOp(unsigned int opCode);

		static void RegisterScalarRotateImmediateOp(unsigned int opCode);

		static void RegisterScalarBooleanInvertRegisterOp(unsigned int opCode);

		static void RegisterScalarBitfieldRegisterOp(unsigned int opCode);

		static void RegisterScalarBitfieldImmediateOp(unsigned int opCode);

		static void RegisterScalarMinMaxRegisterOp(unsigned int opCode);

		static void RegisterScalarZeroingSelectRegisterOp(unsigned int opCode);

		static void RegisterScalarUnaryRegisterOp(unsigned int opCode);

		static void RegisterScalarImmediateOp(unsigned int opCode);

		static void RegisterTypedLoadOp(unsigned int opCode);

		static void RegisterTypedStoreOp(unsigned int opCode);

		static void RegisterBranchOp(unsigned int opCode, bool isConditional);

		static void RegisterAtomicOp(unsigned int opCode);

		/// <summary>
		/// Register a VMX plane instruction that produces a <see cref="VmxMicroOp"/>.
		/// VMX instructions are always fully serialised, cannot be stolen, and require
		/// Machine-mode privilege. Compatibility publication is routed through typed fail-closed effects
		/// by the pipeline dispatch stage.
		/// </summary>
		static void RegisterVmxOp(unsigned int opCode);

		static bool IsVmxRegisterWritebackOpcode(unsigned short opcode);




		static unsigned short NormalizeCanonicalOrSuppressedCsrDestinationRegister(unsigned int opCode, unsigned short rawRegId);

		static unsigned short NormalizeCanonicalOrSuppressedCsrSourceRegister(unsigned int opCode, unsigned short rawRegId, const std::wstring &operandName);

		static unsigned long long NormalizeCanonicalCsrImmediateField(unsigned int opCode, unsigned short rawImmediateField, const std::wstring &operandName);

	public:
// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: internal static bool TryCreatePublishedCsrMicroOp(in InstructionIR instruction, out CSRMicroOp? csrMicroOp)
		static bool TryCreatePublishedCsrMicroOp(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<CSRMicroOp> &csrMicroOp);

	private:
		static std::shared_ptr<CsrReadCounterMicroOp> CreatePublishedCounterReadCsrMicroOp(unsigned short opcode, std::shared_ptr<InstructionIR> instruction, unsigned short csrAddress);

		template<typename T> requires (std::derived_from<T, CSRMicroOp> && std::default_initializable<T>)
		static std::shared_ptr<T> CreatePublishedRegisterSourceCsrMicroOp(unsigned short opcode, std::shared_ptr<InstructionIR> instruction)
		{
// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: T csrMicroOp = new T { OpCode = opcode, CSRAddress = unchecked((ushort)instruction.Imm), DestRegID = NormalizeCanonicalOrSuppressedCsrDestinationRegister(opcode, ToLegacyDecoderField(instruction.Rd)), SrcRegID = NormalizeCanonicalOrSuppressedCsrSourceRegister(opcode, ToLegacyDecoderField(instruction.Rs1), "rs1")};
			std::shared_ptr<T> csrMicroOp = std::make_shared<T>();
			csrMicroOp->setOpCode(opcode);
			csrMicroOp->setCSRAddress(static_cast<unsigned short>(instruction->Imm));
			csrMicroOp->setDestRegID(NormalizeCanonicalOrSuppressedCsrDestinationRegister(opcode, ToLegacyDecoderField(instruction->Rd)));
			csrMicroOp->setSrcRegID(NormalizeCanonicalOrSuppressedCsrSourceRegister(opcode, ToLegacyDecoderField(instruction->Rs1), L"rs1"));

			csrMicroOp->InitializeMetadata();
			return csrMicroOp;
		}

		template<typename T> requires (std::derived_from<T, CSRMicroOp> && std::default_initializable<T>)
		static std::optional<std::shared_ptr<T>> CreatePublishedImmediateSourceCsrMicroOp(unsigned short opcode, std::shared_ptr<InstructionIR> instruction)
		{
			if (instruction->Rs1 > 0x1F)
			{
				return std::nullopt;
			}

// C# TO C++ CONVERTER TASK: There is no C++ equivalent to 'unchecked' in this context:
// ORIGINAL LINE: T csrMicroOp = new T { OpCode = opcode, CSRAddress = unchecked((ushort)instruction.Imm), DestRegID = NormalizeCanonicalOrSuppressedCsrDestinationRegister(opcode, ToLegacyDecoderField(instruction.Rd)), WriteValue = NormalizeCanonicalCsrImmediateField(opcode, instruction.Rs1, "zimm")};
			std::shared_ptr<T> csrMicroOp = std::make_shared<T>();
			csrMicroOp->setOpCode(opcode);
			csrMicroOp->setCSRAddress(static_cast<unsigned short>(instruction->Imm));
			csrMicroOp->setDestRegID(NormalizeCanonicalOrSuppressedCsrDestinationRegister(opcode, ToLegacyDecoderField(instruction->Rd)));
			csrMicroOp->setWriteValue(NormalizeCanonicalCsrImmediateField(opcode, instruction->Rs1, L"zimm"));

			csrMicroOp->InitializeMetadata();
			return csrMicroOp;
		}

		static std::shared_ptr<CsrReadWriteMicroOp> CreatePublishedVectorExceptionControlCsrMicroOp(unsigned short opcode, std::shared_ptr<InstructionIR> instruction, unsigned short csrAddress);

		static void RegisterCounterReadCsrOp(unsigned int opCode, unsigned short csrAddress);

		static void ValidateCounterReadCsrDecoderContext(DecoderContext const ctx, unsigned short csrAddress);

		static void RegisterCsrClearOp(unsigned int opCode);

		static void RegisterVectorExceptionControlCsrOp(unsigned int opCode, unsigned short csrAddress);


		static void RegisterMatrixTileInstructions();

	public:
		static bool IsMatrixTileMaterializerRegistered(unsigned int opCode);

		static bool TryCreateMatrixTileRuntimeObject(std::shared_ptr<InstructionIR> instruction, std::shared_ptr<MatrixTileMaterializedInstruction> &materializedInstruction, MatrixTileIrProjectionFaultKind &faultKind);

		static bool IsMatrixTileMicroOpRegistered(unsigned int opCode);

	private:
		static void RegisterMatrixTileMicroOp(unsigned int opCode);

		static std::shared_ptr<MatrixTileMaterializedInstruction> CreateRequiredMatrixTileRuntimeObject(DecoderContext const ctx);

		static std::shared_ptr<VectorInstructionPayload> GetRequiredProjectedMatrixTilePayload(DecoderContext const ctx, InstructionsEnum opcode);



			static unsigned short NormalizeRequiredLegacyMemoryRegister(unsigned int opCode, unsigned short rawRegId, const std::wstring &operandName, const std::wstring &contourName);

			static std::wstring DescribeNonRepresentableVectorAddressingContour(bool indexed, bool is2D);

			static std::wstring DescribeNonRepresentableVectorAddressingContour(VLIW_Instruction const instruction);

			static bool TryResolveUnsupportedVectorAddressingContour(DecoderContext const ctx, std::wstring &addressingContour);

			static VLIW_Instruction GetRequiredProjectedVectorInstruction(DecoderContext const ctx);

			static void RegisterPublishedVectorBinaryOp(unsigned int opCode);

			static void RegisterVectorTransferOp(unsigned int opCode, unsigned char latency);

			static void RegisterPublishedVectorGatherOp(unsigned int opCode);

			static void RegisterPublishedVectorScatterOp(unsigned int opCode);

			static void RegisterPublishedVectorUnaryOp(unsigned int opCode);

			static void RegisterPublishedVectorFmaOp(unsigned int opCode);

			static void RegisterPublishedVectorReductionOp(unsigned int opCode);

			static void RegisterPublishedVectorComparisonOp(unsigned int opCode);

			static void RegisterPublishedVectorMaskOp(unsigned int opCode);

			static void RegisterPublishedVectorMaskPopCountOp(unsigned int opCode);

			static void RegisterPublishedVectorZeroExtendOp(unsigned int opCode);

			static void RegisterPublishedVectorScanSumOp(unsigned int opCode);

			static void RegisterPublishedVectorPermutationOp(unsigned int opCode);

			static void RegisterPublishedVectorPredicativeMovementOp(unsigned int opCode);

			static void RegisterPublishedVectorSlideOp(unsigned int opCode);

			static void RegisterPublishedVectorPerm2Op(unsigned int opCode);

			static void RegisterPublishedVectorTransposeOp(unsigned int opCode);

			static void RegisterPublishedVectorSlideOneUpOp(unsigned int opCode);

			static void RegisterPublishedVectorSlideOneDownOp(unsigned int opCode);

			static void RegisterPublishedVectorDotProductOp(unsigned int opCode);

			static void RegisterPublishedVectorDotProductWideOp(unsigned int opCode);

			static void RegisterVectorConfigOp(unsigned int opCode);

			static void RegisterRetainedMoveOp(unsigned int opCode);

			static void RegisterRetainedMoveNumOp(unsigned int opCode);

			static void RegisterRetainedAbsoluteLoadOp(unsigned int opCode);

			static void RegisterRetainedAbsoluteStoreOp(unsigned int opCode);


		static void RegisterBaseInstructionSet();

		static void RegisterSystemDeviceCommandOp(unsigned int opCode, std::function<std::shared_ptr<SystemDeviceCommandMicroOp>(DecoderContext)> factory);

		static bool IsZeroOrNoRegister(unsigned short registerId);


	public:
		static void Initialize();



	private:
		static void RegisterScalarArithmeticInstructions();


		static void RegisterVectorInstructions();


		/// <summary>
		/// Phase-1 registration: store only the <see cref="MicroOpFactory"/> for
		/// <paramref name="opCode"/>. Call <see cref="RegisterOpAttributes"/> separately
		/// (or not at all - a default <see cref="MicroOpDescriptor"/> is used until then).
		///
		/// Separating factory registration from attribute registration allows deferred or
		/// table-driven descriptor assignment without modifying factory lambdas.
		///
		/// Blueprint section 4: split registration into semantic-factory and descriptor phases.
		/// </summary>
	public:
		static void RegisterSemanticFactory(unsigned int opCode, MicroOpFactory factory);

		/// <summary>
		/// Phase-2 registration: store (or replace) the <see cref="MicroOpDescriptor"/>
		/// for <paramref name="opCode"/>. The descriptor is immutable after this call.
		/// May be called before or after <see cref="RegisterSemanticFactory"/>.
		///
		/// Blueprint section 4: descriptors are treated as immutable after creation.
		/// </summary>
		static void RegisterOpAttributes(unsigned int opCode, std::shared_ptr<MicroOpDescriptor> descriptor);

		/// <summary>
		/// Get the descriptor for a registered opcode.
		/// </summary>
		/// <param name="opCode">Instruction opcode</param>
		/// <returns>Descriptor if registered, null otherwise</returns>
		static std::shared_ptr<MicroOpDescriptor> GetDescriptor(unsigned int opCode);

		/// <summary>
		/// Create a MicroOp from a decoder context.
		/// </summary>
		static std::shared_ptr<MicroOp> CreateMicroOp(unsigned int opCode, DecoderContext context);

		/// <summary>
		/// Check if an opcode is registered.
		/// </summary>
		static bool IsRegistered(unsigned int opCode);


	private:
		static inline std::atomic<long long> _resourceMaskStructuralFallbackCount{ 0 };
		static inline std::atomic<long long> _placementStructuralFallbackCount{ 0 };

	public:
		static unsigned long long getResourceMaskStructuralFallbackCount();

		static unsigned long long getPlacementStructuralFallbackCount();

		static void ResetSafetyFallbackTelemetryForTesting();

		static SafetyMask128 BuildExplicitStructuralSafetyMask(std::shared_ptr<MicroOp> microOp);

		static bool RequiresExplicitStructuralSafetyMask(std::shared_ptr<MicroOp> microOp);

		static std::shared_ptr<InvalidOperationException> CreateMissingExplicitStructuralSafetyMaskException(std::shared_ptr<MicroOp> microOp, const std::wstring &surface);

	private:
		static std::shared_ptr<SysEventMicroOp> CreateSystemEventMicroOp(unsigned int opcode, std::function<std::shared_ptr<SysEventMicroOp>()> factory);

		static std::shared_ptr<SysEventMicroOp> CreateFenceSystemEventMicroOp(DecoderContext context, std::function<std::shared_ptr<SysEventMicroOp>()> factory);

		static void ValidateCanonicalFenceDecoderContext(DecoderContext context);

		/// <summary>
		/// Blueprint Phase 7: compute a <see cref="SafetyMask128"/> for a MicroOp.
		/// Uses <see cref="DefaultSafetyMask128Generator"/> so that safety authority
		/// stays on the 128-bit surface with per-VT isolation bits in the high word.
		/// </summary>
	public:
		static SafetyMask128 ComputeSafetyMask(unsigned int opCode, std::shared_ptr<MicroOp> microOp, int memoryDomainId);

		/// <summary>
		/// Blueprint Phase 7: 128-bit safety mask generator.
		/// Encodes the baseline register, memory-domain, and LSU-class hazards
		/// directly on the 128-bit authority surface.
		/// </summary>
		static SafetyMask128 DefaultSafetyMask128Generator(SafetyMaskContext context);

	private:
		static SafetyMask128 ApplyStructuralFallbackSafetyMask(SafetyMask128 computedMask, std::shared_ptr<MicroOp> microOp);

		static SafetyMask128 BuildPlacementBackedStructuralSafetyMask(SlotPlacementMetadata placement, SerializationClass serializationClass, bool isControlFlow, bool hasSideEffects);
	};
}
