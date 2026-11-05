#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitSetClearInvert { class BitfieldImmediateGoldenVector; }

// Production paths: CPU_Core.Enums opcode/IsaOpcodeValues; InstructionSupportStatus/IsaV4Surface;
// OpcodeInfo.Registry.Data.Scalar; InstructionEncoder.EncodeScalarBitfieldImmediate;
// VliwDecoderV4 imm6 + Word1=(rd,rs1,x0) ABI; InstructionIR/DecodedBundleTransportProjector;
// InstructionRegistry scalar bitfield-immediate factory/materializer; InternalOpBuilder/InternalOpKind;
// ScalarALUMicroOp + ScalarAluOps + ExecutionDispatcherV4; retire x0/writeback and replay/rollback;
// Phase 02 golden/conformance tests; compiler no-emission/helper guardrails; generic VMX boundary only.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitSetClearInvert
{
	class BclriInstruction final : public std::enable_shared_from_this<BclriInstruction>
	{
	public:
		class BitfieldImmediateGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"BCLRI";
		static inline const std::wstring OperandShape = L"rd, rs1, imm6";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, SourceRegister, Immediate6, XLEN=64 bitfield sideband";
		static inline const std::wstring MicroOpShape = L"ScalarImmediateAlu.BitClearImmediateIndex, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring IndexPolicy = L"Immediate index is an unsigned imm6 payload; decoder/encoder reject values outside [0, 63].";
		static inline const std::wstring Operation = L"rs1 & ~(1UL << imm6)";

		static constexpr int XLen = 64;
		static constexpr int ImmediateBits = 6;
		static constexpr int ImmediateMask = 0x3F;

		static constexpr bool RequiresImmediateEncodingAbi = false;
		static constexpr bool RequiresImmediateRangeRejectionTests = false;
		static constexpr bool RequiresIndexMaskingAbi = false;
		static constexpr bool RequiresBitfieldResultAbi = false;
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

		static std::vector<BitfieldImmediateGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct BitfieldImmediateGoldenVector
		class BitfieldImmediateGoldenVector
		{
		private:
			const unsigned long long Source;
			const unsigned short Immediate6;
			const unsigned long long Expected;

		public:
			BitfieldImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected);

			unsigned long long getSource() const;

			unsigned short getImmediate6() const;

			unsigned long long getExpected() const;

			BitfieldImmediateGoldenVector() = default;
		};
	};
}
