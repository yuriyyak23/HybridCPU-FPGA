#include "TensorMetadataCapabilityProvider.h"
#include "AcceleratorShapeCapability.h"
#include "AcceleratorOperationCapability.h"
#include "AcceleratorResourceModel.h"
#include "../Descriptors/AcceleratorDescriptorTaxonomyCatalog.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> TensorMetadataCapabilityProvider::GetCapabilities()
	{
		std::shared_ptr<AcceleratorShapeCapability> shape = std::make_shared<AcceleratorShapeCapability>(L"tensor-nd", 1, 1'048'576, 1, 8);

		auto operation = std::make_shared<AcceleratorOperationCapability>(L"tensor-contract", std::vector<std::wstring> {L"f32", L"f64", L"int32"}, std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities::AcceleratorShapeCapability>> {shape});

		return std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> {std::make_shared<AcceleratorCapabilityDescriptor>(AcceleratorId, L"Tensor taxonomy metadata", CapabilityVersion, std::vector<std::shared_ptr<AcceleratorOperationCapability>>{operation}, std::make_shared<AcceleratorResourceModel>(0, 0, 0, 0, 0), AcceleratorCapabilityAdoptionMode::MetadataOnly, AcceleratorCompatibilityMode::MetadataOnly, AcceleratorDescriptorTaxonomyCatalog::getTensorMetadata()->getKey(), AcceleratorDescriptorTaxonomyStatus::MetadataOnly)};
	}
}
