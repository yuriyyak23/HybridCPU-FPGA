#pragma once
#include "../../Architecture/Registers/Architectural/RegisterIdentity.h"
#include "../../Execution/DmaStreamCompute/DmaStreamComputeDescriptor.h"
#include "../../Pipeline/Metadata/SlotMetadata.h"
#include <memory>
#include <optional>
namespace YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute { class DmaStreamComputeDescriptor; }
namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors { class AcceleratorCommandDescriptor; }
namespace YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile { struct MatrixTileNumericPolicy; struct MatrixTileLayoutPolicy; }
namespace HybridCPU_ISE::Arch
{
 class InstructionSlotMetadata final
 {
 public:
  YAKSys_Hybrid_CPU::Core::Registers::VtId VirtualThreadId;
  std::shared_ptr<YAKSys_Hybrid_CPU::Core::SlotMetadata> SlotMetadata = YAKSys_Hybrid_CPU::Core::SlotMetadata::getDefault();
  std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute::DmaStreamComputeDescriptor> DmaStreamComputeDescriptor;
  std::optional<YAKSys_Hybrid_CPU::Core::Execution::DmaStreamCompute::DmaStreamComputeDescriptorReference> GetDmaStreamComputeDescriptorReference() const
  {
   if (!DmaStreamComputeDescriptor)
    return std::nullopt;
   return DmaStreamComputeDescriptor->DescriptorReference;
  }
  std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors::AcceleratorCommandDescriptor> AcceleratorCommandDescriptor;
  std::shared_ptr<YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile::MatrixTileNumericPolicy> MatrixTileNumericPolicy;
  std::shared_ptr<YAKSys_Hybrid_CPU::CloseToHSL::Core::ISA::Instructions::NonVmx::Lanes00_03Vector::MatrixTile::MatrixTileLayoutPolicy> MatrixTileLayoutPolicy;
  static const std::shared_ptr<InstructionSlotMetadata> Default;
  static std::shared_ptr<InstructionSlotMetadata> getDefault();
  InstructionSlotMetadata() = default;
  InstructionSlotMetadata(YAKSys_Hybrid_CPU::Core::Registers::VtId vt, std::shared_ptr<YAKSys_Hybrid_CPU::Core::SlotMetadata> metadata) : VirtualThreadId(vt), SlotMetadata(std::move(metadata)) {}
 };
}
