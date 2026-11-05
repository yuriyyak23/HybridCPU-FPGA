#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::ByteBitReverse { class ByteBitReverseGoldenVector; }

// REV8 production path:
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\BitManipulation\ByteBitReverse\Rev8Instruction.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\BitManipulation\ByteBitReverse\Rev8Instruction.LocalSemantics.cs
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
// - \HybridCPU ISE\HybridCPU_ISE.Tests\tests\NonVmxPhase01ByteBitReverseExecutableTests.cs
// REV8 production metadata anchor: scalar byte-order reverse is opened as a
// unary XLEN=64 scalar ALU row. VMX integration remains generic because this
// row has no virtualization boundary; compiler helper emission remains closed.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::BitManipulation::ByteBitReverse
{
	class Rev8Instruction final : public std::enable_shared_from_this<Rev8Instruction>
	{
	public:
		class ByteBitReverseGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"REV8";
		static inline const std::wstring OperandShape = L"rd, rs1";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, SourceRegister, XLEN=64 byte-order sideband";
		static inline const std::wstring MicroOpShape = L"ScalarUnaryAlu.ReverseBytes, retire-owned register writeback, no flags";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ExecutableScalarAlu";
		static inline const std::wstring Operation = L"Reverse byte order within XLEN=64.";

		static constexpr int XLen = 64;

		static constexpr bool RequiresByteOrderAbi = false;
		static constexpr bool RequiresBitOrderingAbi = false;
		static constexpr bool SeparateFromVectorBrev8 = true;
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

		static unsigned long long Execute(unsigned long long value);


		static constexpr bool LocalSemanticsAvailable = true;
		static constexpr bool LocalGoldenSeedAvailable = true;
		static inline const std::wstring LocalOpcodeGate = L"Opcode 331 is allocated in the scalar extension block.";
		static inline const std::wstring LocalDecoderGate = L"Canonical scalar shape is rd, rs1 with rs2=x0 and Immediate=0.";
		static inline const std::wstring LocalInstructionIrGate = L"Unary scalar IR projection: Rd, Rs1, XLEN=64 byte-order sideband.";
		static inline const std::wstring LocalMaterializerGate = L"ScalarUnaryAlu.ReverseBytes is published through the scalar unary register materializer.";
		static inline const std::wstring LocalMicroOpGate = L"Scalar byte-order reverse publishes a runtime ScalarALUMicroOp.";
		static inline const std::wstring LocalExecuteGate = L"Runtime Execute and ScalarAluOps share XLEN=64 byte-order reverse semantics.";
		static inline const std::wstring LocalRetireGate = L"Retire writes rd and discards x0.";
		static inline const std::wstring LocalReplayGate = L"Replay/rollback restores destination architectural register.";
		static inline const std::wstring LocalCompilerGate = L"Compiler helper emission remains closed until scalar runtime evidence closes.";
		static inline const std::wstring LocalVmxGate = L"No VMX-specific frontend path; generic legality/projection only if runtime evidence later closes.";
		static inline const std::wstring LocalEvidenceSeparationGate = L"Scalar REV8 evidence is separate from vector byte/bit reverse evidence.";

		static unsigned long long EvaluateXLen64(unsigned long long value);

		static bool RetireWritesDestination(int destinationRegister);

		static std::vector<ByteBitReverseGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ByteBitReverseGoldenVector
		class ByteBitReverseGoldenVector
		{
		private:
			const unsigned long long Value;
			const unsigned long long Result;

		public:
			ByteBitReverseGoldenVector(unsigned long long value, unsigned long long result);

			unsigned long long getValue() const;

			unsigned long long getResult() const;

			ByteBitReverseGoldenVector() = default;
		};
	};
}
