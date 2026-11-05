#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitCount { class PopulationCountGoldenVector; }

// CPOP production path:
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\BitManipulation\BitCount\CpopInstruction.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\BitManipulation\BitCount\CpopInstruction.LocalSemantics.cs
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
// - \HybridCPU ISE\HybridCPU_ISE.Tests\tests\NonVmxPhase01BitCountExecutableTests.cs
// CPOP is the canonical XLEN=64 scalar population-count runtime mnemonic.
// POPCNT remains a no-emission alias boundary. VMX observes this row only through
// generic legality/projection.

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitCount
{
	class CpopInstruction final : public std::enable_shared_from_this<CpopInstruction>
	{
	public:
		class PopulationCountGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"CPOP";
		static inline const std::wstring OperandShape = L"rd, rs1";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, SourceRegister, XLEN=64 population-count sideband";
		static inline const std::wstring MicroOpShape = L"ScalarUnaryAlu.PopCount, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring AliasPolicy = L"CPOP is the canonical hardware mnemonic; POPCNT remains no-emission alias/reserved.";

		static constexpr int XLen = 64;

		static constexpr bool RequiresCanonicalMnemonicDecision = false;
		static constexpr bool CanonicalMnemonicDecisionClosed = true;
		static constexpr bool PopcntAliasNoEmissionPolicyClosed = true;
		static constexpr bool RequiresAliasNoEmissionPolicy = false;
		static constexpr bool RequiresPopcountSemantics = false;
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

		static unsigned long long Execute(unsigned long long source);


		static constexpr bool LocalSemanticsAvailable = true;
		static constexpr bool LocalGoldenSeedAvailable = true;
		static inline const std::wstring LocalOpcodeGate = L"CPOP opcode 334 is allocated as the canonical Phase 01A population-count runtime row.";
		static inline const std::wstring LocalDecoderGate = L"Canonical runtime shape is rd, rs1, rs2=x0 with Immediate=0.";
		static inline const std::wstring LocalInstructionIrGate = L"Unary scalar IR projection carries Rd and Rs1; no POPCNT alias IR row is published.";
		static inline const std::wstring LocalMaterializerGate = L"Registry publishes ScalarALUMicroOp for CPOP with register-unary operands.";
		static inline const std::wstring LocalMicroOpGate = L"InternalOpBuilder maps CPOP to typed InternalOpKind.Cpop.";
		static inline const std::wstring LocalExecuteGate = L"Pure XLEN=64 popcount semantics are runtime Execute authority.";
		static inline const std::wstring LocalRetireGate = L"Retire writes rd and discards x0; no side effects are published.";
		static inline const std::wstring LocalReplayGate = L"Replay restores the destination architectural register through the scalar rollback token.";
		static inline const std::wstring LocalCompilerGate = L"Compiler helper emission remains closed; POPCNT stays a no-emission alias boundary.";
		static inline const std::wstring LocalVmxGate = L"No VMX-specific frontend path; generic legality/runtime projection only.";

		static unsigned long long EvaluateXLen64(unsigned long long source);

		static bool RetireWritesDestination(int destinationRegister);

		static std::vector<PopulationCountGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct PopulationCountGoldenVector
		class PopulationCountGoldenVector
		{
		private:
			const unsigned long long Source;
			const unsigned long long Result;

		public:
			PopulationCountGoldenVector(unsigned long long source, unsigned long long result);

			unsigned long long getSource() const;

			unsigned long long getResult() const;

			PopulationCountGoldenVector() = default;
		};
	};
}
