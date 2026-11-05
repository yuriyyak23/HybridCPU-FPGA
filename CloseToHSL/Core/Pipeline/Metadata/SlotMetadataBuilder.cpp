#include "SlotMetadataBuilder.h"

using namespace YAKSys_Hybrid_CPU::Core::Pipeline::MicroOps;

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata
{
	std::shared_ptr<SlotMetadata> SlotMetadataBuilder::Build(std::shared_ptr<InstructionIR> instruction, std::optional<std::shared_ptr<CompilerAnnotation>> &annotation)
	{
		if (!annotation || !annotation.value())
		{
			return std::make_shared<SlotMetadata>();
		}

		const CompilerAnnotation &annotationValue = *annotation.value();
		std::shared_ptr<SlotMetadata> metadata = std::make_shared<SlotMetadata>();
		metadata->BranchHint = annotationValue.BranchHint;
		metadata->StealabilityPolicy = annotationValue.StealabilityPolicy;
		metadata->DonorVtHint = annotationValue.DonorVtHint;
		metadata->LocalityHint = annotationValue.LocalityHint;
		metadata->PreferredVt = annotationValue.PreferredVt;
		metadata->ThermalHint = annotationValue.ThermalHint;
		return metadata;
	}
}
