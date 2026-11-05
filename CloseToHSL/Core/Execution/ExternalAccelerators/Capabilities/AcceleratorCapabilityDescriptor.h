#pragma once

#include "AcceleratorOperationCapability.h"
#include "AcceleratorResourceModel.h"
#include "../Descriptors/AcceleratorDescriptorTaxonomyCatalog.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>
#include <memory>
#include "../../../../../tangible_string_helper.h"

using namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors;

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Capabilities
{

	enum class AcceleratorCapabilityAdoptionMode
	{
		MetadataOnly = 0
	};

	enum class AcceleratorCompatibilityMode
	{
		MetadataOnly = 0
	};

	/// <summary>
	/// External accelerator capability metadata. This descriptor is not decode,
	/// submit, execution, or commit authority.
	/// </summary>
	class AcceleratorCapabilityDescriptor final : public std::enable_shared_from_this<AcceleratorCapabilityDescriptor>
	{
	private:
		const std::wstring AcceleratorId;
		const std::wstring DisplayName;
		const unsigned int CapabilityVersion;
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private readonly IReadOnlyList<AcceleratorOperationCapability> Operations;
		const std::vector<std::shared_ptr<AcceleratorOperationCapability>> Operations;
		const std::shared_ptr<AcceleratorResourceModel> ResourceModel;
		const AcceleratorCapabilityAdoptionMode AdoptionMode;
		const AcceleratorCompatibilityMode CompatibilityMode;
		std::shared_ptr<AcceleratorDescriptorTaxonomyKey> TaxonomyKey;
		AcceleratorDescriptorTaxonomyStatus TaxonomyStatus;
		std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> TaxonomyEntry;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: public AcceleratorCapabilityDescriptor(string acceleratorId, string displayName, uint capabilityVersion, IReadOnlyList<AcceleratorOperationCapability> operations, AcceleratorResourceModel resourceModel, AcceleratorCapabilityAdoptionMode adoptionMode = AcceleratorCapabilityAdoptionMode.MetadataOnly, AcceleratorCompatibilityMode compatibilityMode = AcceleratorCompatibilityMode.MetadataOnly, System.Nullable<AcceleratorDescriptorTaxonomyKey> taxonomyKey = null, System.Nullable<AcceleratorDescriptorTaxonomyStatus> taxonomyStatus = null)
		AcceleratorCapabilityDescriptor(const std::wstring &acceleratorId, const std::wstring &displayName, unsigned int capabilityVersion, const std::vector<std::shared_ptr<AcceleratorOperationCapability>> &operations, std::shared_ptr<AcceleratorResourceModel> resourceModel, AcceleratorCapabilityAdoptionMode adoptionMode = AcceleratorCapabilityAdoptionMode::MetadataOnly, AcceleratorCompatibilityMode compatibilityMode = AcceleratorCompatibilityMode::MetadataOnly, std::optional<std::shared_ptr<AcceleratorDescriptorTaxonomyKey>> taxonomyKey = std::nullopt, std::optional<AcceleratorDescriptorTaxonomyStatus> taxonomyStatus = std::nullopt);

		std::wstring getAcceleratorId() const;

		std::wstring getDisplayName() const;

		unsigned int getCapabilityVersion() const;

		const std::vector<std::shared_ptr<AcceleratorOperationCapability>> getOperations() const;

		std::shared_ptr<AcceleratorResourceModel> getResourceModel() const;

		AcceleratorCapabilityAdoptionMode getAdoptionMode() const;

		AcceleratorCompatibilityMode getCompatibilityMode() const;

		std::shared_ptr<AcceleratorDescriptorTaxonomyKey> getTaxonomyKey() const;

		AcceleratorDescriptorTaxonomyStatus getTaxonomyStatus() const;

		std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> getTaxonomyEntry() const;

		bool getGrantsDescriptorAcceptanceAuthority() const;

		bool getGrantsTokenAuthority() const;

		bool getGrantsExecutionAuthority() const;

		bool getGrantsCommitAuthority() const;

		bool getGrantsCompilerEmissionAuthority() const;

		bool getGrantsTopologyQueryAuthority() const;

		bool getGrantsQueueOpenAuthority() const;

		bool getGrantsQueueBindAuthority() const;

		bool getGrantsQueueLifecycleAuthority() const;

// C# TO C++ CONVERTER WARNING: Nullable reference types have no equivalent in C++:
// ORIGINAL LINE: public bool TryGetOperation(string operationKind, out AcceleratorOperationCapability? operation)
		bool TryGetOperation(const std::wstring &operationKind, std::shared_ptr<AcceleratorOperationCapability> &operation);
	};

}
