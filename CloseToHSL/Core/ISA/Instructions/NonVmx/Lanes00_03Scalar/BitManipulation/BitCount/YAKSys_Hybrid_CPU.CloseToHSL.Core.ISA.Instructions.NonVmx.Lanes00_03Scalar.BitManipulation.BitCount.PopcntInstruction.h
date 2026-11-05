#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitCount { class PopulationCountGoldenVector; }

// POPCNT alias decision path:
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\BitManipulation\BitCount\PopcntInstruction.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\BitManipulation\BitCount\PopcntInstruction.LocalSemantics.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\InstructionSupportStatus.cs
// - \HybridCPU ISE\HybridCPU_ISE.Tests\CompilerTests\CompilerNoEmissionBoundaryTests.cs
// - \HybridCPU ISE\HybridCPU_ISE.Tests\CompilerTests\CompilerFacadeAbiPhase01Tests.cs
// POPCNT is not a separate runtime opcode. CPOP is the canonical hardware row;
// POPCNT remains reserved/no-emission until a future parser-only alias policy is
// explicitly approved. VMX has no direct integration point for this alias.

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::BitCount
{
	class PopcntInstruction final : public std::enable_shared_from_this<PopcntInstruction>
	{
	public:
		class PopulationCountGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"POPCNT";
		static inline const std::wstring OperandShape = L"rd, rs1";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, SourceRegister, XLEN=64 population-count sideband";
		static inline const std::wstring MicroOpShape = L"ScalarUnaryAlu.PopCount, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"FacadeAliasNoEmissionClosed";
		static inline const std::wstring AliasPolicy = L"CPOP is the canonical hardware mnemonic; POPCNT remains no-emission alias/reserved.";

		static constexpr int XLen = 64;

		static constexpr bool RequiresCanonicalMnemonicDecision = false;
		static constexpr bool CanonicalMnemonicDecisionClosed = true;
		static constexpr bool SelectedAsRuntimeMnemonic = false;
		static constexpr bool SelectedAsNoEmissionAlias = true;
		static constexpr bool RequiresAliasNoEmissionPolicy = false;
		static constexpr bool AliasNoEmissionPolicyClosed = true;
		static constexpr bool RequiresPopcountSemantics = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresRetireRegisterWriteback = true;
		static constexpr bool RequiresReplayRollbackEvidence = true;
		static constexpr bool NoVmxFrontendIntegrationRequired = true;
		static constexpr bool RequiresVmxProjection = false;
		static constexpr bool HasOpcodeAllocation = false;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;


		static constexpr bool LocalSemanticsAvailable = true;
		static constexpr bool LocalGoldenSeedAvailable = true;
		static inline const std::wstring LocalOpcodeGate = L"POPCNT is not allocated as a runtime opcode; CPOP opcode 334 is canonical.";
		static inline const std::wstring LocalDecoderGate = L"No POPCNT decoder row is published; parser-only alias acceptance would require a future explicit policy.";
		static inline const std::wstring LocalInstructionIrGate = L"No POPCNT IR projection is published; facade alias policy is not runtime evidence.";
		static inline const std::wstring LocalMaterializerGate = L"No POPCNT registry row is published; CPOP owns the scalar popcount materializer.";
		static inline const std::wstring LocalMicroOpGate = L"No POPCNT runtime MicroOp is published.";
		static inline const std::wstring LocalExecuteGate = L"Pure XLEN=64 popcount semantics remain local alias documentation, not runtime Execute authority.";
		static inline const std::wstring LocalRetireGate = L"POPCNT publishes no retire path; CPOP owns canonical scalar writeback.";
		static inline const std::wstring LocalReplayGate = L"POPCNT publishes no replay path; CPOP owns canonical rollback evidence.";
		static inline const std::wstring LocalCompilerGate = L"Compiler helper emission and hidden POPCNT lowering remain closed.";
		static inline const std::wstring LocalVmxGate = L"No VMX-specific frontend path; generic projection only if a future alias policy closes.";

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
