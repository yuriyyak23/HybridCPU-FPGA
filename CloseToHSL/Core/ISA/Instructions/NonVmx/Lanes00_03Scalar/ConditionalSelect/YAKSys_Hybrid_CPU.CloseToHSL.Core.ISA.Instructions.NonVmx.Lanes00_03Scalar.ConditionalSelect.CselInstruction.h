#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ConditionalSelect { class ScalarSelectGoldenVector; }

// CSEL Phase 01E carrier-gate decision path, closed negative:
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\ConditionalSelect\CselInstruction.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\ConditionalSelect\CselInstruction.LocalSemantics.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\InstructionSupportStatus.cs
// - \HybridCPU ISE\HybridCPU_ISE.Tests\tests\NonVmxPhase01ScalarSelectExecutableTests.cs
// - \HybridCPU ISE\HybridCPU_ISE.Tests\tests\NonVmxIteration04BDeferredTemplateSurfaceTests.cs
// Future hardware CSEL requires a new scalar-select production package with an
// approved four-register carrier/sideband ABI plus decoder/encoder,
// InstructionIR/projection, registry/materializer, InternalOp, execute,
// retire, replay, golden, and no-emission evidence.
// CSEL remains metadata-only because current packed scalar IR carries only rd/rs1/rs2
// and Phase 01E/01 gate closure does not approve a four-register source carrier
// for rs_cond. VMX is not a direct integration point for this row.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::ConditionalSelect
{
	class CselInstruction final : public std::enable_shared_from_this<CselInstruction>
	{
	public:
		class ScalarSelectGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"CSEL";
		static inline const std::wstring OperandShape = L"rd, rs_true, rs_false, rs_cond";
		static inline const std::wstring ParameterDescriptor = L"rd selects rs_true or rs_false from explicit condition register sideband";
		static inline const std::wstring MicroOpShape = L"ScalarSelectMicroOp candidate; no MicroOp is published in this template";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"ScalarSelectAbiDeferredNoEmission";
		static inline const std::wstring CarrierGateDecision = L"Phase01ECarrierGateClosedNoApprovedCarrier";
		static constexpr bool RequiresFourRegisterCarrierAbi = true;
		static constexpr bool FourSourceCarrierDecisionClosed = true;
		static constexpr bool ExternalCarrierGateClosed = true;
		static constexpr bool ApprovedFourSourceCarrier = false;
		static constexpr bool ExternalCarrierApprovedInPhase01 = false;
		static constexpr bool CurrentPackedScalarIrSupportsCarrier = false;
		static constexpr bool RequiresExternalCarrierAbi = true;
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
		static inline const std::wstring LocalCarrierGate = L"Phase 01E closes the external CSEL carrier gate negatively; no four-register carrier is approved in Phase 01.";
		static inline const std::wstring LocalOpcodeGate = L"Phase 01E carrier gate is closed negative; no CSEL opcode is allocated here.";
		static inline const std::wstring LocalDecoderGate = L"Canonical local shape is rd, rs_true, rs_false, rs_cond only in a future production package with an approved four-source carrier ABI.";
		static inline const std::wstring LocalInstructionIrGate = L"Current packed scalar IR carries only rd/rs1/rs2; no four-register CSEL IR row is published here.";
		static inline const std::wstring LocalMaterializerGate = L"ScalarSelect materializer contract only; Phase 01 publishes no registry row.";
		static inline const std::wstring LocalMicroOpGate = L"Scalar conditional select contract only; no runtime MicroOp is published here.";
		static inline const std::wstring LocalExecuteGate = L"Pure XLEN=64 condition-nonzero select semantics available as EvaluateXLen64; not runtime Execute authority.";
		static inline const std::wstring LocalRetireGate = L"Future retire writes rd and discards x0; no retire engine path is modified here.";
		static inline const std::wstring LocalReplayGate = L"Future replay restores destination architectural register; no token path is modified here.";
		static inline const std::wstring LocalCompilerGate = L"Compiler helper emission and hidden lowering remain closed.";
		static inline const std::wstring LocalVmxGate = L"No VMX-specific frontend path; generic legality/projection only if runtime evidence later closes.";
		static inline const std::wstring LocalPredicateStateGate = L"No hidden predicate register state is introduced by this local contract.";

		static unsigned long long EvaluateXLen64(unsigned long long trueValue, unsigned long long falseValue, unsigned long long condition);

		static bool RetireWritesDestination(int destinationRegister);

		static std::vector<ScalarSelectGoldenVector> GetLocalGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ScalarSelectGoldenVector
		class ScalarSelectGoldenVector
		{
		private:
			const unsigned long long TrueValue;
			const unsigned long long FalseValue;
			const unsigned long long Condition;
			const unsigned long long Result;

		public:
			ScalarSelectGoldenVector(unsigned long long trueValue, unsigned long long falseValue, unsigned long long condition, unsigned long long result);

			unsigned long long getTrueValue() const;

			unsigned long long getFalseValue() const;

			unsigned long long getCondition() const;

			unsigned long long getResult() const;

			ScalarSelectGoldenVector() = default;
		};
	};
}
