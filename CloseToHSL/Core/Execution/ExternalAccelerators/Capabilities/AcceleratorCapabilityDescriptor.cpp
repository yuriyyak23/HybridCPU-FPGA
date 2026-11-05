#include "AcceleratorCapabilityDescriptor.h"
#include <Windows.h>

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorCapabilityDescriptor(string acceleratorId, string displayName, uint capabilityVersion, IReadOnlyList<AcceleratorOperationCapability> operations, AcceleratorResourceModel resourceModel, AcceleratorCapabilityAdoptionMode adoptionMode = AcceleratorCapabilityAdoptionMode.MetadataOnly, AcceleratorCompatibilityMode compatibilityMode = AcceleratorCompatibilityMode.MetadataOnly, System.Nullable<AcceleratorDescriptorTaxonomyKey> taxonomyKey = null, System.Nullable<AcceleratorDescriptorTaxonomyStatus> taxonomyStatus = null)
	AcceleratorCapabilityDescriptor::AcceleratorCapabilityDescriptor(const std::wstring &acceleratorId, const std::wstring &displayName, unsigned int capabilityVersion, const std::vector<std::shared_ptr<AcceleratorOperationCapability>> &operations, std::shared_ptr<AcceleratorResourceModel> resourceModel, AcceleratorCapabilityAdoptionMode adoptionMode, AcceleratorCompatibilityMode compatibilityMode, std::optional<std::shared_ptr<AcceleratorDescriptorTaxonomyKey>> taxonomyKey, std::optional<AcceleratorDescriptorTaxonomyStatus> taxonomyStatus)
		: AcceleratorId(acceleratorId), DisplayName(displayName), CapabilityVersion(capabilityVersion), Operations(operations), ResourceModel(resourceModel), AdoptionMode(adoptionMode), CompatibilityMode(compatibilityMode) 
	{
		if (StringHelper::isEmptyOrWhiteSpace(acceleratorId))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Accelerator id is required.", nameof(acceleratorId));
			throw std::invalid_argument("Accelerator id is required.");
		}

		if (StringHelper::isEmptyOrWhiteSpace(displayName))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Display name is required.", nameof(displayName));
			throw std::invalid_argument("Display name is required.");
		}

		
		if (!resourceModel) throw std::invalid_argument("resourceModel");

		std::shared_ptr<AcceleratorDescriptorTaxonomyKey> effectiveTaxonomyKey = taxonomyKey.value_or(AcceleratorDescriptorTaxonomyCatalog::getCurrentMatMul()->getKey());
		std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> taxonomyEntry;
		if (!AcceleratorDescriptorTaxonomyCatalog::TryGetEntry(effectiveTaxonomyKey, taxonomyEntry))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Accelerator capability descriptor references an unknown Lane7 descriptor taxonomy entry.", nameof(taxonomyKey));
			throw std::invalid_argument("Accelerator capability descriptor references an unknown Lane7 descriptor taxonomy entry.");
		}

		AcceleratorDescriptorTaxonomyStatus effectiveTaxonomyStatus = taxonomyStatus.value_or(taxonomyEntry->getStatus());
		if (effectiveTaxonomyStatus != taxonomyEntry->getStatus())
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Accelerator capability descriptor taxonomy status does not match the catalog entry.", nameof(taxonomyStatus));
			throw std::invalid_argument("Accelerator capability descriptor taxonomy status does not match the catalog entry.");
		}

		
		
		
		
		
		
		
		TaxonomyKey = effectiveTaxonomyKey;
		TaxonomyStatus = effectiveTaxonomyStatus;
		TaxonomyEntry = taxonomyEntry;
	}

	std::wstring AcceleratorCapabilityDescriptor::getAcceleratorId() const
	{
		return AcceleratorId;
	}

	std::wstring AcceleratorCapabilityDescriptor::getDisplayName() const
	{
		return DisplayName;
	}

	unsigned int AcceleratorCapabilityDescriptor::getCapabilityVersion() const
	{
		return CapabilityVersion;
	}

	const std::vector<std::shared_ptr<AcceleratorOperationCapability>> AcceleratorCapabilityDescriptor::getOperations() const
	{
		return Operations;
	}

	std::shared_ptr<AcceleratorResourceModel> AcceleratorCapabilityDescriptor::getResourceModel() const
	{
		return ResourceModel;
	}

	AcceleratorCapabilityAdoptionMode AcceleratorCapabilityDescriptor::getAdoptionMode() const
	{
		return AdoptionMode;
	}

	AcceleratorCompatibilityMode AcceleratorCapabilityDescriptor::getCompatibilityMode() const
	{
		return CompatibilityMode;
	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyKey> AcceleratorCapabilityDescriptor::getTaxonomyKey() const
	{
		return TaxonomyKey;
	}

	AcceleratorDescriptorTaxonomyStatus AcceleratorCapabilityDescriptor::getTaxonomyStatus() const
	{
		return TaxonomyStatus;
	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> AcceleratorCapabilityDescriptor::getTaxonomyEntry() const
	{
		return TaxonomyEntry;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsDescriptorAcceptanceAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsTokenAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsExecutionAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsCommitAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsCompilerEmissionAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsTopologyQueryAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsQueueOpenAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsQueueBindAuthority() const
	{
		return false;
	}

	bool AcceleratorCapabilityDescriptor::getGrantsQueueLifecycleAuthority() const
	{
		return false;
	}

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool TryGetOperation(string operationKind, out AcceleratorOperationCapability? operation)
	bool AcceleratorCapabilityDescriptor::TryGetOperation(const std::wstring &operationKind, std::shared_ptr<AcceleratorOperationCapability> &operation)
	{
		operation = nullptr;
		if (StringHelper::isEmptyOrWhiteSpace(operationKind))
		{
			return false;
		}

		for (const auto& candidate : Operations)
		{
			if (candidate && CompareStringOrdinal(candidate->getOperationKind().c_str(), -1, operationKind.c_str(), -1, TRUE) == CSTR_EQUAL)
			{
				operation = candidate;
				break;
			}
		}

		return operation != nullptr;
	}
}
