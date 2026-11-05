#pragma once

#include <string>
#include <memory>

// SUBC is a multi-precision negative-decision gate: no opcode/materializer/helper
// authority until borrow-out publication ABI is retire-owned. VMX stays generic.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::MultiPrecision
{
	class SubcInstruction final : public std::enable_shared_from_this<SubcInstruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"SUBC";
		static inline const std::wstring OperandShape = L"explicit borrow-out ABI TBD";
		static inline const std::wstring ParameterDescriptor = L"rd, rs1, rs2, explicit borrow-out/result sideband";
		static inline const std::wstring MicroOpShape = L"ScalarMultiPrecisionMicroOp candidate; no implicit flags and no MicroOp is published in this template";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"MultiPrecisionCarryAbiDeferredNoEmission";
		static inline const std::wstring AbiDecision = L"NoAllocationUntilExplicitCarryBorrowPublicationAbi";
		static inline const std::wstring CarryBorrowPublicationPolicy = L"No implicit architectural flags; borrow-out must publish through an explicit retire-owned register or sideband ABI.";
		static inline const std::wstring BorrowOutputPolicy = L"Borrow-out must be replayable and rollback-safe before execution opens.";
		static constexpr bool RequiresBorrowOutAbi = true;
		static constexpr bool RequiresCarryBorrowPublicationAbi = true;
		static constexpr bool RequiresExplicitBorrowOutputTransportAbi = true;
		static constexpr bool NoImplicitFlags = true;
		static constexpr bool RejectHiddenArchitecturalFlags = true;
		static constexpr bool NoHiddenMultiOpEmission = true;
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
	};
}
