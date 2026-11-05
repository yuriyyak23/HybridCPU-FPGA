#include "MatMulCapabilityProvider.h"
#include "AcceleratorShapeCapability.h"
#include "../Descriptors/MatMulDescriptor.h"
#include "AcceleratorOperationCapability.h"
#include "MatMulResourceModel.h"
#include "../Descriptors/AcceleratorDescriptorTaxonomyCatalog.h"
#include "../Descriptors/AcceleratorCommandDescriptor.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	const std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> MatMulCapabilityProvider::GetCapabilities()
	{
		std::shared_ptr<AcceleratorShapeCapability> shape = std::make_shared<AcceleratorShapeCapability>(L"matrix-2d", 1, MatMulDescriptorValidator::MaxOutputElements, 2, 2);

		auto operation = std::make_shared<AcceleratorOperationCapability>(L"matmul", std::vector<std::wstring>{L"f32", L"f64", L"int32"}, std::vector<std::shared_ptr<AcceleratorShapeCapability>>{shape});

		return std::vector<std::shared_ptr<AcceleratorCapabilityDescriptor>> {std::make_shared<AcceleratorCapabilityDescriptor>(AcceleratorId, L"MatMul fixture metadata", CapabilityVersion, std::vector<std::shared_ptr<AcceleratorOperationCapability>>{operation}, MatMulResourceModel::getDefault()->ToCapabilityResourceModel(), AcceleratorCapabilityAdoptionMode::MetadataOnly, AcceleratorCompatibilityMode::MetadataOnly, AcceleratorDescriptorTaxonomyCatalog::getCurrentMatMul()->getKey(), AcceleratorDescriptorTaxonomyStatus::CurrentRuntimeContour)};
	}

	bool MatMulCapabilityProvider::Matches(std::shared_ptr<AcceleratorCommandDescriptor> descriptor)
	{
		return descriptor->AcceleratorClass == AcceleratorClassId::Matrix && descriptor->AcceleratorId == AcceleratorDeviceId::ReferenceMatMul && descriptor->Operation == AcceleratorOperationKind::MatMul && descriptor->CapabilityVersion == CapabilityVersion;
	}
}
