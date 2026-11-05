#pragma once

#include <string>
#include <memory>

// CRC32 is a scalar CRC negative-decision gate: no opcode/materializer/helper authority
// until polynomial/reflection/seed/final-xor/endian ABI is explicit. VMX stays generic.
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Scalar::CRC
{
	class Crc32Instruction final : public std::enable_shared_from_this<Crc32Instruction>
	{
	public:
		static inline const std::wstring Mnemonic = L"CRC32";
		static inline const std::wstring OperandShape = L"rd, rs_seed, rs_data";
		static inline const std::wstring ParameterDescriptor = L"rd receives a 32-bit CRC result once polynomial/reflection ABI is frozen";
		static inline const std::wstring MicroOpShape = L"ScalarCrcMicroOp candidate; no MicroOp is published in this template";
		static inline const std::wstring ExecutionLaneBinding = L"Lanes00_03Scalar";
		static inline const std::wstring EvidenceBoundary = L"CrcPolynomialAbiDeferredNoEmission";
		static inline const std::wstring AbiDecision = L"NoAllocationUntilPolynomialReflectionSeedFinalXorEndianAbi";
		static inline const std::wstring PolynomialPolicy = L"Unspecified: CRC32 does not imply IEEE, Castagnoli, Koopman, or any other polynomial.";
		static inline const std::wstring ReflectionPolicy = L"Unspecified: input/output bit reflection must be explicit before execution.";
		static inline const std::wstring SeedFinalXorPolicy = L"Unspecified: seed initialization and final XOR must be explicit before execution.";
		static inline const std::wstring EndianPolicy = L"Unspecified: byte/word ingestion order must be explicit before execution.";
		static constexpr bool RequiresPolynomialAbi = true;
		static constexpr bool RequiresReflectionAbi = true;
		static constexpr bool RequiresSeedFinalXorAbi = true;
		static constexpr bool RequiresEndianPolicyAbi = true;
		static constexpr bool RejectImplicitPolynomialSelection = true;
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
