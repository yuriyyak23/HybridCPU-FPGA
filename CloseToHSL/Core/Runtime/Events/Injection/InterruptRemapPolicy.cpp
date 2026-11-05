#include "InterruptRemapPolicy.h"

namespace YAKSys_Hybrid_CPU::Core
{

	int InterruptRemapTable::getCount() const
	{
		return _entries.size();
	}

	unsigned long long InterruptRemapTable::getPolicyEpoch() const
	{
		return PolicyEpoch;
	}

	void InterruptRemapTable::setPolicyEpoch(unsigned long long value)
	{
		PolicyEpoch = value;
	}

	void InterruptRemapTable::Configure(std::shared_ptr<InterruptRemapEntry> entry)
	{
		for (int index = 0; index < _entries.size(); index++)
		{
			if (*_entries[index]->Key == *entry->Key)
			{
				_entries[index] = entry;
				AdvancePolicyEpoch();
				return;
			}
		}

		_entries.push_back(entry);
		AdvancePolicyEpoch();
	}

	bool InterruptRemapTable::Remove(std::shared_ptr<InterruptRemapKey> key)
	{
		for (int index = 0; index < _entries.size(); index++)
		{
			if (*_entries[index]->Key == *key)
			{
				_entries.erase(_entries.begin() + index);
				AdvancePolicyEpoch();
				return true;
			}
		}

		return false;
	}

	void InterruptRemapTable::Clear()
	{
		if (_entries.empty())
		{
			return;
		}

		_entries.clear();
		AdvancePolicyEpoch();
	}

	bool InterruptRemapTable::TryRoute(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<EventInjectionDescriptor> &routed, bool &remapped, bool &coalescePosted, InterruptPostDisposition &disposition)
	{
		routed = descriptor;
		remapped = false;
		coalescePosted = true;
		disposition = InterruptPostDisposition::None;

		if (!descriptor || !descriptor->IsValid())
		{
			disposition = InterruptPostDisposition::DroppedInvalid;
			return false;
		}

		std::shared_ptr<InterruptRemapEntry> entry;
		if (!TryFindEntry(descriptor, entry))
		{
			return true;
		}

		remapped = true;
		if (entry->IsDrop())
		{
			routed = nullptr;
			coalescePosted = false;
			disposition = InterruptPostDisposition::DroppedByRemap;
			return false;
		}

		routed = entry->Apply(descriptor);
		coalescePosted = entry->CoalescePosted;
		return true;
	}

	bool InterruptRemapTable::TryFindEntry(std::shared_ptr<EventInjectionDescriptor> descriptor, std::shared_ptr<InterruptRemapEntry> &entry)
	{
		for (int index = 0; index < _entries.size(); index++)
		{
			std::shared_ptr<InterruptRemapEntry> candidate = _entries[index];
			if (candidate->Key->SourceExecutionDomainTag != InterruptRemapKey::AnyExecutionDomainTag && candidate->Matches(descriptor))
			{
				entry = candidate;
				return true;
			}
		}

		for (int index = 0; index < _entries.size(); index++)
		{
			std::shared_ptr<InterruptRemapEntry> candidate = _entries[index];
			if (candidate->Key->SourceExecutionDomainTag == InterruptRemapKey::AnyExecutionDomainTag && candidate->Matches(descriptor))
			{
				entry = candidate;
				return true;
			}
		}

		entry = nullptr;
		return false;
	}

	void InterruptRemapTable::AdvancePolicyEpoch()
	{
//		unchecked
			setPolicyEpoch(getPolicyEpoch() + 1);
// C# TO C++ CONVERTER TASK: End of the original C# 'unchecked' block.
	}
}
