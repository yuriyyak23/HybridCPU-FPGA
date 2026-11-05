#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BooleanInvert { class BooleanInvertGoldenVector; }

// Production path for ORN:
// \HybridCPU ISE\HybridCPU_ISE\Core\Common\CPU_Core.Enums.cs
// \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\OpcodeInfo.Registry.Data.Scalar.cs
// \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\InstructionSupportStatus.cs
// \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\IsaV4Surface.cs
// \HybridCPU ISE\HybridCPU_ISE\Core\Decoder\VliwDecoderV4.cs
// \HybridCPU ISE\HybridCPU_ISE\NonRTL\Core\Diagnostics\InstructionRegistry.Initialize.Scalar.cs
// \HybridCPU ISE\HybridCPU_ISE\NonRTL\Core\Pipeline\InternalOpBuilder.cs
// \HybridCPU ISE\HybridCPU_ISE\Core\ALU\ScalarAluOps.cs
// \HybridCPU ISE\HybridCPU_ISE\Core\Execution\ExecutionDispatcherV4.Scalar.cs
// \HybridCPU ISE\HybridCPU_ISE.Tests\tests\NonVmxPhase01BooleanInvertExecutableTests.cs
// ORN production metadata anchor: bitwise-invert boolean ALU is opened as a
// single scalar register-register opcode with no compiler helper authority.
// VMX does not receive a special frontend path for this ordinary scalar row.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BooleanInvert
{
	class OrnInstruction final : public std::enable_shared_from_this<OrnInstruction>
	{
	public:
		class BooleanInvertGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"ORN";
		static inline const std::wstring OperandShape = L"rd, rs1, rs2";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, SourceRegister1, SourceRegister2, XLEN=64 boolean-invert sideband";
		static inline const std::wstring MicroOpShape = L"ScalarBinaryAlu.OrNot, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring Operation = L"rs1 | ~rs2";

		static constexpr int XLen = 64;

		static constexpr bool RequiresFacadeOrHardwareDecision = false;
		static constexpr bool RequiresBitwiseInvertSecondOperandAbi = false;
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

		static unsigned long long Execute(unsigned long long left, unsigned long long right);


		static constexpr bool LocalSemanticsAvailable = true;
		static constexpr bool LocalGoldenSeedAvailable = true;
		static inline const std::wstring LocalOpcodeGate = L"Closed by production ORN opcode allocation.";
		static inline const std::wstring LocalDecoderGate = L"Closed canonical shape is rd, rs1, rs2 with Immediate=0.";
		static inline const std::wstring LocalInstructionIrGate = L"Closed binary scalar IR projection: Rd, Rs1, Rs2, Imm=0.";
		static inline const std::wstring LocalMaterializerGate = L"Closed through ScalarALUMicroOp materialization.";
		static inline const std::wstring LocalMicroOpGate = L"Closed through scalar binary ALU or-not runtime MicroOp publication.";
		static inline const std::wstring LocalExecuteGate = L"Pure XLEN=64 rs1 | ~rs2 semantics are runtime Execute authority for ORN.";
		static inline const std::wstring LocalRetireGate = L"Closed: retire writes rd and discards x0.";
		static inline const std::wstring LocalReplayGate = L"Closed: rollback token restores destination architectural register.";
		static inline const std::wstring LocalCompilerGate = L"Compiler helper emission and hidden multi-op lowering remain closed.";
		static inline const std::wstring LocalVmxGate = L"No VMX-specific frontend path; generic legality/projection only.";

		static unsigned long long EvaluateXLen64(unsigned long long left, unsigned long long right);

		static bool RetireWritesDestination(int destinationRegister);

		static std::vector<BooleanInvertGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct BooleanInvertGoldenVector
		class BooleanInvertGoldenVector
		{
		private:
			const unsigned long long Left;
			const unsigned long long Right;
			const unsigned long long Result;

		public:
			BooleanInvertGoldenVector(unsigned long long left, unsigned long long right, unsigned long long result);

			unsigned long long getLeft() const;

			unsigned long long getRight() const;

			unsigned long long getResult() const;

			BooleanInvertGoldenVector() = default;
		};
	};
}
