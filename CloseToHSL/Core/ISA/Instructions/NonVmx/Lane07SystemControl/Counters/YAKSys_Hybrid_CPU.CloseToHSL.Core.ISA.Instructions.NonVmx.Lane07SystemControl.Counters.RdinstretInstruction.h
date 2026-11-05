#pragma once

#include <string>
#include <memory>

// RDINSTRET metadata anchor: retired-instruction counter read remains deferred
// until retire accounting, replay rollback, decoder ABI, and publication evidence close.
// VMX must see any future execution only through generic counter/projection policy.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lane07SystemControl::Counters
{
	class RdinstretInstruction final : public std::enable_shared_from_this<RdinstretInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"RDINSTRET";
		static inline const std::wstring OperandShape = L"rd";
		static inline const std::wstring ParameterDescriptor = L"DestRegister, retire-counter accounting domain sideband";
		static inline const std::wstring MicroOpShape = L"Lane7SystemCounter.ReadInstret, retire-owned register writeback, no memory side effects";
		static inline const std::wstring ExecutionLaneBinding = L"Lane07SystemControl";
		static inline const std::wstring EvidenceBoundary = L"Lane7CounterReplayDeferred";

		static constexpr bool IsSystemCounter = true;
		static constexpr bool RequiresCounterSourceAbi = true;
		static constexpr bool RequiresRetireAccountingModel = true;
		static constexpr bool RequiresReplayStableCounterModel = true;
		static constexpr bool RequiresPrivilegePolicy = true;
		static constexpr bool RequiresDecoderEncoderAbi = true;
		static constexpr bool RequiresInstructionIrProjection = true;
		static constexpr bool RequiresRegistryMaterializer = true;
		static constexpr bool RequiresRetireOwnedPublication = true;
		static constexpr bool RequiresRetireRegisterWriteback = true;
		static constexpr bool RequiresReplayRollbackEvidence = true;
		static constexpr bool SeparateFromClosedRdcycle = true;
		static constexpr bool NoVmxFrontendIntegrationRequired = true;
		static constexpr bool RequiresImmediateVmxProjection = false;
		static constexpr bool RequiresFutureVirtualizationBoundaryPolicy = true;
		static constexpr bool HasOpcodeAllocation = false;
		static constexpr bool IsExecutable = false;
		static constexpr bool CompilerHelperAllowed = false;
	};
}
