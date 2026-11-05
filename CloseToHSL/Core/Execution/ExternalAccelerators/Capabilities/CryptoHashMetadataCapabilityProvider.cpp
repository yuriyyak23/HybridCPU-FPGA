#include "CryptoHashMetadataCapabilityProvider.h"
#include "AcceleratorShapeCapability.h"
#include "AcceleratorOperationCapability.h"
#include "AcceleratorResourceModel.h"
#include "../Descriptors/AcceleratorDescriptorTaxonomyCatalog.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> CryptoHashMetadataCapabilityProvider::GetCapabilities()
	{
		std::shared_ptr<AcceleratorShapeCapability> shape = std::make_shared<AcceleratorShapeCapability>(L"crypto-hash-block", 1, 1'048'576, 1, 1);

		auto operation = std::make_shared<AcceleratorOperationCapability>(L"crypto-hash-contract", std::vector<std::wstring> {L"metadata"}, std::vector<std::shared_ptr<YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities::AcceleratorShapeCapability>> {shape});

		return std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> {std::make_shared<AcceleratorCapabilityDescriptor>(AcceleratorId, L"Crypto/Hash taxonomy metadata", CapabilityVersion, std::vector<std::shared_ptr<AcceleratorOperationCapability>>{operation}, std::make_shared<AcceleratorResourceModel>(0, 0, 0, 0, 0), AcceleratorCapabilityAdoptionMode::MetadataOnly, AcceleratorCompatibilityMode::MetadataOnly, AcceleratorDescriptorTaxonomyCatalog::getCryptoHashMetadata()->getKey(), AcceleratorDescriptorTaxonomyStatus::MetadataOnly)};
	}
}
