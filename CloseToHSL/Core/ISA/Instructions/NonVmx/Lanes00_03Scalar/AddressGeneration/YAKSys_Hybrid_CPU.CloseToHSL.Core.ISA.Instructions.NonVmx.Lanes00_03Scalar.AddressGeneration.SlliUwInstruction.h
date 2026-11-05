#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration { class AddressGenerationImmediateGoldenVector; }

// Production paths: CPU_Core.Enums opcode/IsaOpcodeValues; InstructionSupportStatus/IsaV4Surface;
// OpcodeInfo.Registry.Data.Scalar; InstructionEncoder.EncodeScalarAddressGenerationImmediate;
// VliwDecoderV4 imm6 + Word1=(rd,rs1,x0) ABI; explicit .UW source-width ABI zero-extends
// low 32 bits of rs1 before shift; InstructionIR/DecodedBundleTransportProjector; InstructionRegistry
// scalar address-generation immediate factory/materializer; InternalOpBuilder/InternalOpKind;
// ScalarALUMicroOp + ScalarAluOps + ExecutionDispatcherV4; retire x0/writeback and replay/rollback;
// Phase 03 golden/conformance tests; compiler no-emission/helper guardrails; generic VMX boundary only.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration
{
	class SlliUwInstruction final : public std::enable_shared_from_this<SlliUwInstruction>
	{
	public:
		class AddressGenerationImmediateGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"SLLI.UW";
		static inline const std::wstring OperandShape = L"rd, rs1, imm6";
		static inline const std::wstring ParameterDescriptor = L"rd receives zero-extend32(rs1) << imm6 under XLEN=64 scalar ALU rules";
		static inline const std::wstring MicroOpShape = L"ScalarImmediateAlu.AddressGenerationShiftLeftUnsignedWordImmediate, retire-owned register writeback, no LSU bypass";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring SourceWidthPolicy = L"Zero-extend low 32 bits of rs1 before shifting by unsigned imm6.";
		static inline const std::wstring ImmediatePolicy = L"Immediate shift amount is an unsigned imm6 payload; decoder/encoder reject values outside [0, 63].";
		static inline const std::wstring Operation = L"zero_extend_64(low32(rs1)) << imm6";

		static constexpr int XLen = 64;
		static constexpr int SourceWidth = 32;
		static constexpr int ImmediateBits = 6;
		static constexpr int ImmediateMask = 0x3F;

		static constexpr bool RequiresUwSourceWidthAbi = false;
		static constexpr bool RequiresImmediateEncodingAbi = false;
		static constexpr bool RequiresImmediateRangeRejectionTests = false;
		static constexpr bool NoLsuBypassAuthority = true;
		static constexpr bool NoHiddenMultiOpEmission = true;
		static constexpr bool RequiresDecoderEncoderAbi = false;
		static constexpr bool RequiresInstructionIrProjection = false;
		static constexpr bool RequiresRegistryMaterializer = false;
		static constexpr bool RequiresRetireRegisterWriteback = false;
		static constexpr bool RequiresReplayRollbackEvidence = false;
		static constexpr bool NoVmxFrontendIntegrationRequired = true;
		static constexpr bool RequiresVmxProjection = false;
		static constexpr bool HasOpcodeAllocation = true;
		static constexpr bool IsExecutable = true;
		static constexpr bool CompilerHelperAllowed = false;

		static unsigned short getOpcode();

		static bool getWritesScalarRegister();

		static bool getHasSideEffects();

		static unsigned long long Execute(unsigned long long source, unsigned long long immediate6);

		static unsigned long long EvaluateXLen64(unsigned long long source, unsigned long long immediate6);

		static bool RetireWritesDestination(unsigned short destinationRegister);

		static std::vector<AddressGenerationImmediateGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AddressGenerationImmediateGoldenVector
		class AddressGenerationImmediateGoldenVector
		{
		private:
			const unsigned long long Source;
			const unsigned short Immediate6;
			const unsigned long long Expected;

		public:
			AddressGenerationImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected);

			unsigned long long getSource() const;

			unsigned short getImmediate6() const;

			unsigned long long getExpected() const;

			AddressGenerationImmediateGoldenVector() = default;
		};
	};
}
