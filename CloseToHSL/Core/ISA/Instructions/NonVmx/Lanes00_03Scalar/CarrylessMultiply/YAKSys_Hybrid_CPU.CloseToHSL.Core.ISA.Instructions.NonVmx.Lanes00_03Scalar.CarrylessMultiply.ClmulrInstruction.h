#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::CarrylessMultiply { class CarryLessGoldenVector; }

// Production paths: CPU_Core.Enums opcode/IsaOpcodeValues; InstructionSupportStatus/IsaV4Surface;
// OpcodeInfo.Registry.Data.Scalar; InstructionEncoder.EncodeScalar register-register ABI with Immediate=0;
// VliwDecoderV4 canonical rd,rs1,rs2 rejection of immediate aliases; InstructionIR/DecodedBundleTransportProjector;
// InstructionRegistry scalar carry-less factory/materializer; InternalOpBuilder/InternalOpKind;
// ScalarALUMicroOp + ScalarAluOps + ExecutionDispatcherV4; retire x0/writeback and replay/rollback;
// Phase 04 golden/conformance tests; compiler no-emission/helper guardrails; generic VMX boundary only.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::CarrylessMultiply
{
	class ClmulrInstruction final : public std::enable_shared_from_this<ClmulrInstruction>
	{
	public:
		class CarryLessGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"CLMULR";
		static inline const std::wstring OperandShape = L"rd, rs1, rs2";
		static inline const std::wstring ParameterDescriptor = L"rd receives carry-less product bits [126:63] for XLEN=64";
		static inline const std::wstring MicroOpShape = L"ScalarCarryLessMultiply.ReversedHighWindow, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring BitOrderPolicy = L"Bit index 0 is the least-significant bit; reversed/high window starts at product bit XLEN-1.";
		static inline const std::wstring Operation = L"carryless_product(rs1, rs2) >> 63";

		static constexpr int XLen = 64;
		static constexpr int ProductWindowStartBit = 63;

		static constexpr bool RequiresBitOrderAbi = false;
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

		static unsigned long long Execute(unsigned long long multiplicand, unsigned long long multiplier);

		static unsigned long long EvaluateXLen64(unsigned long long multiplicand, unsigned long long multiplier);

		static bool RetireWritesDestination(unsigned short destinationRegister);

		static std::vector<CarryLessGoldenVector> GetLocalGoldenVectors();

	private:
		static unsigned long long CarryLessMultiplyWindow64(unsigned long long multiplicand, unsigned long long multiplier, int startBit);

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct CarryLessGoldenVector
		class CarryLessGoldenVector
		{
		private:
			const unsigned long long Multiplicand;
			const unsigned long long Multiplier;
			const unsigned long long Expected;

		public:
			CarryLessGoldenVector(unsigned long long multiplicand, unsigned long long multiplier, unsigned long long expected);

			unsigned long long getMultiplicand() const;

			unsigned long long getMultiplier() const;

			unsigned long long getExpected() const;

			CarryLessGoldenVector() = default;
		};
	};
}
