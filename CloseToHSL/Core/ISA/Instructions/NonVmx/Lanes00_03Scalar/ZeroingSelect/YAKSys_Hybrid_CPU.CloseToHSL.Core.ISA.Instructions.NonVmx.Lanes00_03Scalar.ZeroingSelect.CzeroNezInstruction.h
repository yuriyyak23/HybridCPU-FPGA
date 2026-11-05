#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ZeroingSelect { class ZeroingSelectGoldenVector; }

// CZERO.NEZ production path:
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\ZeroingSelect\CzeroNezInstruction.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\ZeroingSelect\CzeroNezInstruction.LocalSemantics.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\Architecture\Common\CPU_Core.Enums.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\InstructionSupportStatus.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\IsaV4Surface.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\OpcodeInfo.Registry.Data.Scalar.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\Frontend\Decode\VliwDecoderV4Bridge\VliwDecoderV4.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Core\Diagnostics\InstructionRegistry.Helpers.Core.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Core\Diagnostics\InstructionRegistry.Initialize.Scalar.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Core\Pipeline\InternalOpBuilder.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\Pipeline\MicroOps\Internal\InternalOp.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\Execution\Scalar\ALU\ScalarAluOps.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\Execution\Dispatch\ExecutionDispatcherV4.Scalar.cs
// - \HybridCPU ISE\HybridCPU_ISE.Tests\tests\NonVmxPhase01ScalarSelectExecutableTests.cs
// CZERO.NEZ production metadata anchor: scalar zeroing select is opened as a
// binary XLEN=64 scalar ALU row. Its nonzero polarity evidence is separate from
// closed CZERO.EQZ; VMX remains a generic runtime client only.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ZeroingSelect
{
	class CzeroNezInstruction final : public std::enable_shared_from_this<CzeroNezInstruction>
	{
	public:
		class ZeroingSelectGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"CZERO.NEZ";
		static inline const std::wstring OperandShape = L"rd, rs1, rs2";
		static inline const std::wstring ParameterDescriptor = L"rd receives zero or rs1 according to the nonzero test of rs2";
		static inline const std::wstring MicroOpShape = L"ScalarBinaryAlu.ZeroIfNotEqualZero, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring Polarity = L"ConditionNotEqualZeroProducesZero";

		static constexpr int XLen = 64;

		static constexpr bool RequiresPolarityProof = false;
		static constexpr bool PolarityProofClosed = true;
		static constexpr bool SeparateFromClosedCzeroEqz = true;
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

		static unsigned long long Execute(unsigned long long value, unsigned long long condition);


		static constexpr bool LocalSemanticsAvailable = true;
		static constexpr bool LocalGoldenSeedAvailable = true;
		static inline const std::wstring LocalOpcodeGate = L"CZERO.NEZ opcode 333 is allocated in the Phase 01E scalar zeroing-select package.";
		static inline const std::wstring LocalDecoderGate = L"Canonical runtime shape is rd, rs1, rs2 with Immediate=0.";
		static inline const std::wstring LocalInstructionIrGate = L"Binary scalar IR projection carries rs1=value and rs2=condition; no hidden predicate state.";
		static inline const std::wstring LocalMaterializerGate = L"Registry publishes ScalarALUMicroOp for CZERO.NEZ with register-register operands.";
		static inline const std::wstring LocalMicroOpGate = L"InternalOpBuilder maps CZERO.NEZ to typed InternalOpKind.CzeroNez.";
		static inline const std::wstring LocalExecuteGate = L"Pure XLEN=64 nonzero-condition zeroing semantics are runtime Execute authority.";
		static inline const std::wstring LocalRetireGate = L"Retire writes rd and discards x0; no side effects are published.";
		static inline const std::wstring LocalReplayGate = L"Replay restores the destination architectural register through the scalar rollback token.";
		static inline const std::wstring LocalCompilerGate = L"Compiler helper emission and hidden lowering remain closed.";
		static inline const std::wstring LocalVmxGate = L"No VMX-specific frontend path; generic legality/runtime projection only.";
		static inline const std::wstring LocalPolarityGate = L"CZERO.NEZ polarity is closed as condition != 0 produces zero; closed CZERO.EQZ evidence is not reused.";

		static unsigned long long EvaluateXLen64(unsigned long long value, unsigned long long condition);

		static bool RetireWritesDestination(int destinationRegister);

		static std::vector<ZeroingSelectGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ZeroingSelectGoldenVector
		class ZeroingSelectGoldenVector
		{
		private:
			const unsigned long long Value;
			const unsigned long long Condition;
			const unsigned long long Result;

		public:
			ZeroingSelectGoldenVector(unsigned long long value, unsigned long long condition, unsigned long long result);

			unsigned long long getValue() const;

			unsigned long long getCondition() const;

			unsigned long long getResult() const;

			ZeroingSelectGoldenVector() = default;
		};
	};
}
