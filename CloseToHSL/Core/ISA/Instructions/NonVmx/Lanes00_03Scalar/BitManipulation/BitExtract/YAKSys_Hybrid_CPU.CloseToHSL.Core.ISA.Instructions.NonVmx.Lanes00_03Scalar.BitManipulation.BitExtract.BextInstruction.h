#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitExtract { class BitfieldGoldenVector; }

// Production paths: CPU_Core.Enums opcode/IsaOpcodeValues; InstructionSupportStatus/IsaV4Surface;
// OpcodeInfo.Registry.Data.Scalar; InstructionEncoder.EncodeScalar register-register ABI with Immediate=0;
// VliwDecoderV4 canonical rd,rs1,rs2 rejection of immediate aliases; InstructionIR/DecodedBundleTransportProjector;
// InstructionRegistry scalar bitfield factory/materializer; InternalOpBuilder/InternalOpKind;
// ScalarALUMicroOp + ScalarAluOps + ExecutionDispatcherV4; retire x0/writeback and replay/rollback;
// Phase 02 golden/conformance tests; compiler no-emission/helper guardrails; generic VMX boundary only.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitExtract
{
	class BextInstruction final : public std::enable_shared_from_this<BextInstruction>
	{
	public:
		class BitfieldGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"BEXT";
		static inline const std::wstring OperandShape = L"rd, rs1, rs2";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, SourceRegister, IndexRegister, XLEN=64 bit-extract sideband";
		static inline const std::wstring MicroOpShape = L"ScalarBinaryAlu.BitExtractRegisterIndex, retire-owned canonical 0/1 writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring IndexPolicy = L"Register index is masked to XLEN=64 width with rs2 & 0x3F; result is canonical 0 or 1.";
		static inline const std::wstring Operation = L"(rs1 >> (rs2 & 0x3F)) & 1";

		static constexpr int XLen = 64;
		static constexpr int IndexMask = 0x3F;

		static constexpr bool RequiresIndexMaskingAbi = false;
		static constexpr bool RequiresBitfieldResultAbi = false;
		static constexpr bool RequiresCanonicalBooleanResultAbi = false;
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

		static unsigned long long Execute(unsigned long long source, unsigned long long index);

		static unsigned long long EvaluateXLen64(unsigned long long source, unsigned long long index);

		static bool RetireWritesDestination(unsigned short destinationRegister);

		static std::vector<BitfieldGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct BitfieldGoldenVector
		class BitfieldGoldenVector
		{
		private:
			const unsigned long long Source;
			const unsigned long long Index;
			const unsigned long long Expected;

		public:
			BitfieldGoldenVector(unsigned long long source, unsigned long long index, unsigned long long expected);

			unsigned long long getSource() const;

			unsigned long long getIndex() const;

			unsigned long long getExpected() const;

			BitfieldGoldenVector() = default;
		};
	};
}
