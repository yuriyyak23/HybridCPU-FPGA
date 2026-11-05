#pragma once

#include <string>
#include <vector>
#include <limits>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration { class AddressGenerationGoldenVector; }

// Production paths: CPU_Core.Enums opcode/IsaOpcodeValues; InstructionSupportStatus/IsaV4Surface;
// OpcodeInfo.Registry.Data.Scalar; canonical scalar register ABI Word1=(rd,rs1,rs2), Immediate=0;
// VliwDecoderV4/InstructionIR/DecodedBundleTransportProjector; InstructionRegistry scalar address-generation
// factory/materializer; InternalOpBuilder/InternalOpKind; ScalarALUMicroOp + ScalarAluOps +
// ExecutionDispatcherV4; retire x0/writeback and replay/rollback; Phase 03 golden/conformance tests;
// compiler no-emission/helper guardrails; generic VMX boundary only.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::AddressGeneration
{
	class Sh3addInstruction final : public std::enable_shared_from_this<Sh3addInstruction>
	{
	public:
		class AddressGenerationGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"SH3ADD";
		static inline const std::wstring OperandShape = L"rd, rs1, rs2";
		static inline const std::wstring ParameterDescriptor = L"rd receives rs2 + (rs1 << 3) under XLEN=64 scalar ALU rules";
		static inline const std::wstring MicroOpShape = L"ScalarBinaryAlu.AddressGenerationShiftLeftThreeAdd, retire-owned register writeback, no LSU bypass";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring Operation = L"(rs1 << 3) + rs2";

		static constexpr int XLen = 64;

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

		static unsigned long long Execute(unsigned long long source, unsigned long long addend);

		static unsigned long long EvaluateXLen64(unsigned long long source, unsigned long long addend);

		static bool RetireWritesDestination(unsigned short destinationRegister);

		static std::vector<AddressGenerationGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct AddressGenerationGoldenVector
		class AddressGenerationGoldenVector
		{
		private:
			const unsigned long long Source;
			const unsigned long long Addend;
			const unsigned long long Expected;

		public:
			AddressGenerationGoldenVector(unsigned long long source, unsigned long long addend, unsigned long long expected);

			unsigned long long getSource() const;

			unsigned long long getAddend() const;

			unsigned long long getExpected() const;

			AddressGenerationGoldenVector() = default;
		};
	};
}
