#pragma once

#include "../../../NonRTL/Arch/Compat/HybridCPU_ISE.Arch.VLIW_Instruction.h"
#include "../../../NonRTL/Arch/InvalidOpcodeException.h"
#include "../Execution/DmaStreamCompute/DmaStreamComputeDescriptor.h"
#include "../Execution/ExternalAccelerators/Descriptors/AcceleratorCommandDescriptor.h"
#include "../Execution/ExternalAccelerators/Auth/AcceleratorOwnerDomainGuard.h"
#include "../Contracts/CompilerTransport/InstructionSlotMetadata.h"
#include <string>
#include <optional>
#include <format>
#include <memory>
#include <utility>

using namespace HybridCPU_ISE::Arch;
using namespace YAKSys_Hybrid_CPU::Arch;
using namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Auth;
using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Decoder
{
	struct RuntimeSidebandProjection final
	{
		std::optional<std::shared_ptr<DmaStreamComputeDescriptor>> DmaDescriptor;
		std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> AcceleratorDescriptor;
		RuntimeSidebandProjection(std::optional<std::shared_ptr<DmaStreamComputeDescriptor>> dmaDescriptor = std::nullopt, std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> acceleratorDescriptor = std::nullopt)
			: DmaDescriptor(std::move(dmaDescriptor)), AcceleratorDescriptor(std::move(acceleratorDescriptor)) {}
	};

	/// <summary>
	/// Runtime-only continuation of the decoder boundary. It deliberately runs
	/// after static canonical decoding: descriptor guard evidence, reference
	/// coherence and admission placement are state-dependent authority and are
	/// therefore not <see cref="DecodeFailure"/> outcomes.
	/// </summary>
	class DecoderRuntimeSidebandValidator final
	{
	public:
		static std::shared_ptr<RuntimeSidebandProjection> ValidateAndResolve(VLIW_Instruction const instruction, int slotIndex, std::shared_ptr<InstructionSlotMetadata> slotMetadata);

	private:
		static std::optional<std::shared_ptr<DmaStreamComputeDescriptor>> ValidateDmaDescriptor(VLIW_Instruction const instruction, unsigned short opcode, const std::wstring &opcodeName, int slotIndex, std::shared_ptr<InstructionSlotMetadata> slotMetadata);

		static std::optional<std::shared_ptr<AcceleratorCommandDescriptor>> ValidateAcceleratorDescriptor(VLIW_Instruction const instruction, unsigned short opcode, const std::wstring &opcodeName, int slotIndex, std::shared_ptr<InstructionSlotMetadata> slotMetadata);

		static std::shared_ptr<InvalidOpcodeException> Failure(const std::wstring &message, const std::wstring &opcodeName, int slotIndex);
	};

// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	internal sealed record RuntimeSidebandProjection(System.Nullable<DmaStreamComputeDescriptor> DmaStreamComputeDescriptor, System.Nullable<AcceleratorCommandDescriptor> AcceleratorCommandDescriptor);

}
