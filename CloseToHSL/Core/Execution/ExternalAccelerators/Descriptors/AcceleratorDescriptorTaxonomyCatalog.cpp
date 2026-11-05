#include "AcceleratorDescriptorTaxonomyCatalog.h"

namespace YAKSys_Hybrid_CPU::Core::Execution::ExternalAccelerators::Descriptors
{

	AcceleratorDescriptorTaxonomyEntry::AcceleratorDescriptorTaxonomyEntry(std::shared_ptr<AcceleratorDescriptorTaxonomyKey> key, const std::wstring &name, AcceleratorDescriptorTaxonomyStatus status, const std::wstring &boundary)
		: Key(std::move(key)), Name(name), Status(status), Boundary(boundary)
	{
		if (StringHelper::isEmptyOrWhiteSpace(name))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Taxonomy entry name is required.", nameof(name));
			throw std::invalid_argument("Taxonomy entry name is required.");
		}

		if (StringHelper::isEmptyOrWhiteSpace(boundary))
		{
// C# TO C++ CONVERTER TASK: This exception's constructor requires only one argument:
// ORIGINAL LINE: throw new ArgumentException("Taxonomy boundary is required.", nameof(boundary));
			throw std::invalid_argument("Taxonomy boundary is required.");
		}

	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyKey> AcceleratorDescriptorTaxonomyEntry::getKey() const
	{
		return Key;
	}

	std::wstring AcceleratorDescriptorTaxonomyEntry::getName() const
	{
		return Name;
	}

	AcceleratorDescriptorTaxonomyStatus AcceleratorDescriptorTaxonomyEntry::getStatus() const
	{
		return Status;
	}

	std::wstring AcceleratorDescriptorTaxonomyEntry::getBoundary() const
	{
		return Boundary;
	}

	bool AcceleratorDescriptorTaxonomyEntry::isCurrentRuntimeContour() const
	{
		return getStatus() == AcceleratorDescriptorTaxonomyStatus::CurrentRuntimeContour;
	}

	bool AcceleratorDescriptorTaxonomyEntry::isMetadataOnly() const
	{
		return getStatus() == AcceleratorDescriptorTaxonomyStatus::MetadataOnly;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsDescriptorAcceptanceAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsCapabilityAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsTokenAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsExecutionAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsCommitAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsCompilerEmissionAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsTopologyQueryAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsQueueOpenAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsQueueBindAuthority() const
	{
		return false;
	}

	bool AcceleratorDescriptorTaxonomyEntry::getGrantsQueueLifecycleAuthority() const
	{
		return false;
	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> AcceleratorDescriptorTaxonomyCatalog::getCurrentMatMul()
	{
		return _entries[0];
	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> AcceleratorDescriptorTaxonomyCatalog::getTensorMetadata()
	{
		return _entries[1];
	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> AcceleratorDescriptorTaxonomyCatalog::getTopologyQueueMetadata()
	{
		return _entries[2];
	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> AcceleratorDescriptorTaxonomyCatalog::getFftMetadata()
	{
		return _entries[3];
	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> AcceleratorDescriptorTaxonomyCatalog::getCryptoHashMetadata()
	{
		return _entries[4];
	}

	std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> AcceleratorDescriptorTaxonomyCatalog::getSparseGraphMetadata()
	{
		return _entries[5];
	}

	const std::vector<std::shared_ptr<AcceleratorDescriptorTaxonomyEntry>> AcceleratorDescriptorTaxonomyCatalog::getEntries()
	{
		return _entries;
	}

	bool AcceleratorDescriptorTaxonomyCatalog::TryGetEntry(std::shared_ptr<AcceleratorDescriptorTaxonomyKey> key, std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> &entry)
	{
		for (int index = 0; index < _entries.size(); index++)
		{
			std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> candidate = _entries[index];
			if (key && *candidate->getKey() == *key)
			{
				entry = candidate;
				return true;
			}
		}

	// C# TO C++ CONVERTER TASK: There is no C++ equivalent to the C# 'null-forgiving operator':
	// ORIGINAL LINE: entry = null!;
		entry = nullptr;
		return false;
	}

	bool AcceleratorDescriptorTaxonomyCatalog::TryGetEntry(AcceleratorClassId acceleratorClass, AcceleratorDeviceId acceleratorId, AcceleratorOperationKind operation, AcceleratorShapeKind shape, std::shared_ptr<AcceleratorDescriptorTaxonomyEntry> &entry)
	{
		return TryGetEntry(std::make_shared<AcceleratorDescriptorTaxonomyKey>(acceleratorClass, acceleratorId, operation, shape), entry);
	}
}
