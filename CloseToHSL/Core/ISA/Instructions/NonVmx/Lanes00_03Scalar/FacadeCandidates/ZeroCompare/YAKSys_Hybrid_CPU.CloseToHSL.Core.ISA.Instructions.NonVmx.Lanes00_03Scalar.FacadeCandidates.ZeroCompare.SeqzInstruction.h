#pragma once

#include <string>
#include <vector>
#include <memory>

// C# TO C++ CONVERTER NOTE: Forward class declarations:
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::FacadeCandidates::ZeroCompare { class ZeroCompareFacadeGoldenVector; }

// SEQZ facade decision path:
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\FacadeCandidates\ZeroCompare\SeqzInstruction.cs
// - \HybridCPU ISE\HybridCPU_ISE\CloseToHSL\Core\ISA\Instructions\NonVmx\Lanes00_03Scalar\FacadeCandidates\ZeroCompare\SeqzInstruction.LocalNoEmissionContract.cs
// - \HybridCPU ISE\HybridCPU_ISE\NonRTL\Arch\InstructionSupportStatus.cs
// - \HybridCPU ISE\HybridCPU_ISE.Tests\CompilerTests\CompilerNoEmissionBoundaryTests.cs
// SEQZ is closed as facade-only/no-emission for this Phase 01F decision. No
// hardware opcode, decoder row, compiler helper, or hidden lowering is opened.
// VMX observes only the generic runtime model.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::FacadeCandidates::ZeroCompare
{
	class SeqzInstruction final : public std::enable_shared_from_this<SeqzInstruction>
	{
	public:
		class ZeroCompareFacadeGoldenVector;

	public:
		static inline const std::wstring Mnemonic = L"SEQZ";
		static inline const std::wstring OperandShape = L"rd, rs1";
		static inline const std::wstring ParameterDescriptor = L"rd receives rs1 == 0 as 0/1 if a hardware opcode is approved";
		static inline const std::wstring MicroOpShape = L"ScalarUnaryAluMicroOp candidate; no MicroOp is published in this template";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"FacadeOnlyNoEmissionClosed";
		static constexpr bool RequiresHardwareOpcodeDecision = false;
		static constexpr bool FacadeDecisionClosed = true;
		static constexpr bool SelectedHardwareOpcode = false;
		static constexpr bool SelectedFacadeOnly = true;
		static constexpr bool HiddenLoweringAllowed = false;
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


		static constexpr bool LocalNoEmissionContractAvailable = true;
		static constexpr bool LocalHardwareCandidateGoldenSeedAvailable = true;
		static inline const std::wstring LocalOpcodeGate = L"Phase 01F selects facade-only/no-emission; no SEQZ opcode is allocated.";
		static inline const std::wstring LocalDecoderGate = L"No SEQZ decoder row is published; hardware form would need a future full scalar path.";
		static inline const std::wstring LocalInstructionIrGate = L"No SEQZ IR projection is published; facade policy is not runtime evidence.";
		static inline const std::wstring LocalMaterializerGate = L"No SEQZ materializer row is published.";
		static inline const std::wstring LocalMicroOpGate = L"No SEQZ runtime MicroOp is published.";
		static inline const std::wstring LocalExecuteGate = L"Canonical 0/1 hardware-candidate result is available as EvaluateHardwareCandidateXLen64; not runtime Execute authority.";
		static inline const std::wstring LocalRetireGate = L"Future hardware retire would write rd and discard x0; no retire engine path is modified here.";
		static inline const std::wstring LocalReplayGate = L"Future hardware replay would restore destination architectural register; no token path is modified here.";
		static inline const std::wstring LocalCompilerGate = L"Compiler helper emission and hidden lowering remain closed.";
		static inline const std::wstring LocalVmxGate = L"No VMX-specific frontend path; generic legality/projection only if runtime evidence later closes.";
		static inline const std::wstring LocalFacadeEvidenceGate = L"Facade-only/no-emission decision is closed; hidden lowering remains forbidden.";

		static unsigned long long EvaluateHardwareCandidateXLen64(unsigned long long value);

		static bool WouldRetireWriteDestinationIfHardwareSelected(int destinationRegister);

		static std::vector<ZeroCompareFacadeGoldenVector> GetLocalHardwareCandidateGoldenVectors();

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to the C# readonly struct:
// ORIGINAL LINE: public readonly struct ZeroCompareFacadeGoldenVector
		class ZeroCompareFacadeGoldenVector
		{
		private:
			const unsigned long long Value;
			const unsigned long long Result;

		public:
			ZeroCompareFacadeGoldenVector(unsigned long long value, unsigned long long result);

			unsigned long long getValue() const;

			unsigned long long getResult() const;

			ZeroCompareFacadeGoldenVector() = default;
		};
	};
}
