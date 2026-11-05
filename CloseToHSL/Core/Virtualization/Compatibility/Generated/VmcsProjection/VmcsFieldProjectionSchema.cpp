#include "VmcsFieldProjectionSchema.h"

namespace YAKSys_Hybrid_CPU::Core
{

	std::span<const std::shared_ptr<VmcsFieldProjectionSchemaEntry>> VmcsFieldProjectionSchema::getEntries()
	{
		return EntryTable;
	}

	std::shared_ptr<VmcsFieldProjectionSchemaArtifact> VmcsFieldProjectionSchema::getCanonicalArtifact()
	{
		return CanonicalArtifact;
	}

	bool VmcsFieldProjectionSchema::TryGet(VmcsField field, std::shared_ptr<VmcsFieldProjectionSchemaEntry> &entry)
	{
		for (auto candidate : getEntries())
		{
			if (candidate->Field == field)
			{
				entry = candidate;
				return true;
			}
		}

		entry = nullptr;
		return false;
	}

	bool VmcsFieldProjectionSchema::CanRead(std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry)
	{
		return entry->AccessPolicy == VmcsFieldProjectionAccessPolicy::ReadOnly || entry->AccessPolicy == VmcsFieldProjectionAccessPolicy::ReadWrite;
	}

	bool VmcsFieldProjectionSchema::CanWrite(std::shared_ptr<VmcsFieldProjectionSchemaEntry> entry)
	{
		return false;
	}
}
