#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::Rotates { class RotateImmediateGoldenVector; }

// Production paths: CPU_Core.Enums opcode/IsaOpcodeValues; InstructionSupportStatus/IsaV4Surface;
// OpcodeInfo.Registry.Data.Scalar; InstructionEncoder.EncodeScalarRotateImmediate;
// VliwDecoderV4 imm6 + Word1=(rd,rs1,x0) ABI; InstructionIR/DecodedBundleTransportProjector;
// InstructionRegistry scalar rotate-immediate factory/materializer; InternalOpBuilder/InternalOpKind;
// ScalarALUMicroOp + ScalarAluOps + ExecutionDispatcherV4; retire x0/writeback and replay/rollback;
// Phase 02 golden/conformance tests; compiler no-emission/helper guardrails; generic VMX boundary only.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::Rotates
{
	class RoriInstruction final : public std::enable_shared_from_this<RoriInstruction>
	{
	public:
		class RotateImmediateGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"RORI";
		static inline const std::wstring OperandShape = L"rd, rs1, imm6";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, SourceRegister, Immediate6, XLEN=64 rotate count sideband";
		static inline const std::wstring MicroOpShape = L"ScalarImmediateAlu.RotateRight, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring CompatibilityBoundary = L"Register-register ROR remains separate; rotate-immediate uses canonical imm6 ABI.";

		static constexpr int XLen = 64;
		static constexpr int ImmediateBits = 6;
		static constexpr int ImmediateMask = 0x3F;

		static constexpr bool RequiresRotateImmediateEncodingAbi = false;
		static constexpr bool RequiresImmediateRangeRejectionTests = false;
		static constexpr bool SeparateFromClosedRegisterRotate = true;
		static constexpr bool NoRegisterRotateEvidenceReuse = true;
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

		static std::vector<RotateImmediateGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct RotateImmediateGoldenVector
		class RotateImmediateGoldenVector
		{
		private:
			const unsigned long long Source;
			const unsigned short Immediate6;
			const unsigned long long Expected;

		public:
			RotateImmediateGoldenVector(unsigned long long source, unsigned short immediate6, unsigned long long expected);

			unsigned long long getSource() const;

			unsigned short getImmediate6() const;

			unsigned long long getExpected() const;

			RotateImmediateGoldenVector() = default;
		};
	};
}
