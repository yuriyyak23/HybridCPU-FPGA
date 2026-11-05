#pragma once

#include <string>
#include <vector>

namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::CompatibilityConformance
{

	class Phase15CompatibilityConformanceSweepContract final
	{
	private:
		static inline const std::vector<std::wstring> ExecutableEvidenceChain{ L"CAT", L"OP", L"DEC", L"IR", L"MAT", L"OBJ", L"UOP", L"EXE", L"RET", L"RPL", L"TST", L"GLD", L"NOE" };
		static inline const std::vector<std::wstring> DeferredEvidenceGuards{ L"NoOpcodeAllocation", L"NoDecoderEncoderAbi", L"NoInstructionIrProjection", L"NoRegistryMaterializer", L"NoTypedMicroOpPublication", L"NoExecutionPath", L"NoRetireSideEffectPublication", L"NoCompilerHelper", L"NoVmxSpecificPath" };
		static inline const std::vector<std::wstring> SweepBoundaries{ L"VectorLegalityMatrix", L"Lane6DescriptorAuthority", L"Lane7PrivilegeAdmission", L"RetireOwnedPublication", L"ReplayRollbackConformance", L"GoldenArtifacts", L"CompilerNoEmission", L"HostEvidenceNonLeak" };

	public:
		static inline const std::wstring Phase = L"Phase15";
		static inline const std::wstring SweepName = L"NonVmxCompatibilityConformanceSweep";
		static inline const std::wstring ClosureDecision = L"CompatibilityConformanceAuditOnly";
		static inline const std::wstring VmxBoundary = L"GenericRuntimeOnly";

		static constexpr bool AddsInstructionSemantics = false;
		static constexpr bool AllocatesOpcode = false;
		static constexpr bool OpensDecoderEncoderAbi = false;
		static constexpr bool OpensInstructionIrProjection = false;
		static constexpr bool OpensRegistryMaterializer = false;
		static constexpr bool PublishesTypedMicroOp = false;
		static constexpr bool OpensExecutionPath = false;
		static constexpr bool OpensRetireSideEffects = false;
		static constexpr bool OpensCompilerHelper = false;
		static constexpr bool OpensVmxSpecificPath = false;

		static std::vector<std::wstring> getExecutableEvidenceChain();

		static std::vector<std::wstring> getDeferredEvidenceGuards();

		static std::vector<std::wstring> getSweepBoundaries();
	};

}
