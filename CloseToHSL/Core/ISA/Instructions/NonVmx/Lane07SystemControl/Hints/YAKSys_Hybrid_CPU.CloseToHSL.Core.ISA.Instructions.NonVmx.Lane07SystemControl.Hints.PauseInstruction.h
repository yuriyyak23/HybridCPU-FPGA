#pragma once

#include <string>
#include <memory>

// PAUSE metadata anchor: scheduling hint remains deferred/no-execution until
// hint encoding, progress semantics, replay, and no-state-publication evidence close.
// It must not promise architectural progress or expose a VMX-specific integration path.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane07SystemControl::Hints
{
	class PauseInstruction final : public std::enable_shared_from_this<PauseInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"PAUSE";
		static inline const std::wstring OperandShape = L"no operands or approved hint immediate";
		static inline const std::wstring ParameterDescriptor = L"No architectural operands; optional hint immediate only after ABI decision";
		static inline const std::wstring MicroOpShape = L"Lane7SchedulingHint.Pause, no architectural writeback, no progress guarantee";
		static inline const std::wstring ExecutionLaneBinding = L"Lane07SystemControl";
		static inline const std::wstring EvidenceBoundary = L"Lane7HintNoExecutionGuarantee";

		static constexpr bool IsSchedulingHint = true;
		static constexpr bool RequiresHintEncodingAbi = true;
		static constexpr bool RequiresNoArchitecturalStateLeakage = true;
		static constexpr bool NoArchitecturalProgressGuarantee = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresRetireOwnedPublication = false;
		static constexpr bool RequiresRetireRegisterWriteback = false;
		static constexpr bool RequiresReplayRollbackEvidence = true;
		static constexpr bool NoVmxFrontendIntegrationRequired = true;
		static constexpr bool RequiresImmediateVmxProjection = false;
		static constexpr bool RequiresFutureVirtualizationBoundaryPolicy = false;
		static constexpr bool HasOpcodeAllocation = false;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
