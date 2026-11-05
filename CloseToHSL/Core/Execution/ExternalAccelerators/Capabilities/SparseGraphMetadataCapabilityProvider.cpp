#include "SparseGraphMetadataCapabilityProvider.h"
#include "AcceleratorShapeCapability.h"
#include "AcceleratorOperationCapability.h"
#include "AcceleratorResourceModel.h"
#include "../Descriptors/AcceleratorDescriptorTaxonomyCatalog.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> SparseGraphMetadataCapabilityProvider::GetCapabilities()
	{
		std::shared_ptr<AcceleratorShapeCapability> shape = std::make_shared<AcceleratorShapeCapability>(L"sparse-graph-csr", 1, 1'048'576, 1, 2);

		auto operation = std::make_shared<AcceleratorOperationCapability>(L"sparse-graph-contract", std::vector<std::wstring> {L"metadata"}, std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities::AcceleratorShapeCapability>> {shape});

		return std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> {std::make_shared<AcceleratorCapabilityDescriptor>(AcceleratorId, L"Sparse/Graph taxonomy metadata", CapabilityVersion, std::vector<std::shared_ptr<AcceleratorOperationCapability>>{operation}, std::make_shared<AcceleratorResourceModel>(0, 0, 0, 0, 0), AcceleratorCapabilityAdoptionMode::MetadataOnly, AcceleratorCompatibilityMode::MetadataOnly, AcceleratorDescriptorTaxonomyCatalog::getSparseGraphMetadata()->getKey(), AcceleratorDescriptorTaxonomyStatus::MetadataOnly)};
	}
}
