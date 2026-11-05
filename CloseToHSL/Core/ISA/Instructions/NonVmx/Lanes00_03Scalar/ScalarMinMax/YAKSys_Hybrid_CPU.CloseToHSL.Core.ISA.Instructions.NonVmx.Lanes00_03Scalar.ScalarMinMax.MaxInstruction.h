#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ScalarMinMax { class ScalarMinMaxGoldenVector; }

// MAX production path:
// - \HybridCPU ISE\HybridCPU_ISE\Core\Common\CPU_Core.Enums.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\OpcodeInfo.Registry.Data.Scalar.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\InstructionSupportStatus.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\IsaV4Surface.cs
// - \HybridCPU ISE\HybridCPU_ISE\Core\Decoder\VliwDecoderV4.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Core\Diagnostics\InstructionRegistry.Initialize.Scalar.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Core\Pipeline\InternalOpBuilder.cs
// - \HybridCPU ISE\HybridCPU_ISE\Core\ALU\ScalarAluOps.cs
// - \HybridCPU ISE\HybridCPU_ISE\Core\Execution\ExecutionDispatcherV4.Scalar.cs
// - \HybridCPU ISE\HybridCPU_ISE.Tests\tests\NonVmxPhase01ScalarMinMaxExecutableTests.cs
// MAX production metadata anchor: scalar signed max is runtime-owned by the scalar ALU evidence chain.
// It is distinct from vector, AMO, and Lane6 descriptor min/max surfaces; no VMX-specific path is added.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ScalarMinMax
{
	class MaxInstruction final : public std::enable_shared_from_this<MaxInstruction>
	{
	public:
		class ScalarMinMaxGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"MAX";
		static inline const std::wstring OperandShape = L"rd, rs1, rs2";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, SourceRegister1, SourceRegister2, XLEN=64 signed comparison sideband";
		static inline const std::wstring MicroOpShape = L"ScalarBinaryAlu.MaxSigned, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring Signedness = L"Signed";

		static constexpr int XLen = 64;

		static constexpr bool RequiresSignednessAbi = false;
		static constexpr bool UsesSignedComparison = true;
		static constexpr bool SeparateFromVectorMinMax = true;
		static constexpr bool SeparateFromAmoMinMax = true;
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
		static inline const std::wstring LocalOpcodeGate = L"Opcode 328 is allocated in the scalar extension block.";
		static inline const std::wstring LocalDecoderGate = L"Canonical scalar decoder accepts rd, rs1, rs2 and rejects non-zero immediate aliases.";
		static inline const std::wstring LocalInstructionIrGate = L"Binary scalar IR projection is Rd, Rs1, Rs2, Imm=0, no vector payload.";
		static inline const std::wstring LocalMaterializerGate = L"Registry publishes ScalarALUMicroOp for MAX with register-register operands.";
		static inline const std::wstring LocalMicroOpGate = L"InternalOpBuilder maps MAX to typed InternalOpKind.Max.";
		static inline const std::wstring LocalExecuteGate = L"Runtime Execute and ScalarAluOps share XLEN=64 signed max semantics.";
		static inline const std::wstring LocalRetireGate = L"Retire writes rd and discards x0 through the scalar ALU retire path.";
		static inline const std::wstring LocalReplayGate = L"Replay rollback restores the destination architectural register after writeback.";
		static inline const std::wstring LocalCompilerGate = L"Compiler helper emission remains closed; no MAX helper authority is opened.";
		static inline const std::wstring LocalVmxGate = L"No VMX-specific frontend path; generic scalar legality only.";
		static inline const std::wstring LocalEvidenceSeparationGate = L"Scalar evidence is separate from vector, AMO, and Lane6 descriptor min/max evidence.";

		static unsigned long long EvaluateXLen64(unsigned long long left, unsigned long long right);

		static bool RetireWritesDestination(int destinationRegister);

		static std::vector<ScalarMinMaxGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ScalarMinMaxGoldenVector
		class ScalarMinMaxGoldenVector
		{
		private:
			const unsigned long long Left;
			const unsigned long long Right;
			const unsigned long long Result;

		public:
			ScalarMinMaxGoldenVector(unsigned long long left, unsigned long long right, unsigned long long result);

			unsigned long long getLeft() const;

			unsigned long long getRight() const;

			unsigned long long getResult() const;

			ScalarMinMaxGoldenVector() = default;
		};
	};
}
