#include "TLB.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace HybridCPU_ISE::CloseToHSL::Memory::MMU
{

	unsigned long long TLB::getHits() const
	{
		return Hits;
	}

	void TLB::setHits(unsigned long long value)
	{
		Hits = value;
	}

	unsigned long long TLB::getMisses() const
	{
		return Misses;
	}

	void TLB::setMisses(unsigned long long value)
	{
		Misses = value;
	}

	bool TLB::TryTranslate(unsigned long long virtualAddress, int domainId, unsigned long long &physicalAddress, unsigned char &permissions)
	{
		unsigned long long vpn = virtualAddress >> 12;
		unsigned long long offset = virtualAddress & 0xFFF;

		for (int i = 0; i < TLB_ENTRIES; i++)
		{
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref TlbEntry entry = ref _entries[i];
			const TlbEntry& entry = _entries[i];
			if (entry.Valid && !entry.IsNested && entry.VPN == vpn && entry.DomainId == domainId)
			{
				physicalAddress = (entry.PPN << 12) | offset;
				permissions = entry.Permissions;
				setHits(getHits() + 1);
				UpdateLru(i);
				return true;
			}
		}

		physicalAddress = 0;
		permissions = 0;
		setMisses(getMisses() + 1);
		return false;
	}

	void TLB::Insert(unsigned long long virtualAddress, unsigned long long physicalAddress, unsigned char permissions, int domainId)
	{
		unsigned long long vpn = virtualAddress >> 12;
		unsigned long long ppn = physicalAddress >> 12;

		int victimIdx = 0;
		unsigned char maxAge = 0;

		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (!_entries[i].Valid)
			{
				victimIdx = i;
				break;
			}

			if (_entries[i].LruAge > maxAge)
			{
				maxAge = _entries[i].LruAge;
				victimIdx = i;
			}
		}

		TlbEntry temp_var = TlbEntry();
		temp_var.VPN = vpn;
		temp_var.PPN = ppn;
		temp_var.Permissions = permissions;
		temp_var.MemoryType = 0;
		temp_var.Valid = true;
		temp_var.DomainId = domainId;
		temp_var.IsNested = false;
		temp_var.NestedTag = nullptr;
		temp_var.NestedGuestPPN = 0;
		temp_var.LruAge = 0;
		_entries[victimIdx] = temp_var;
	}

	bool TLB::TryTranslateNested(unsigned long long guestVirtualAddress, std::shared_ptr<AddressSpaceId> addressSpace, unsigned long long &hostPhysicalAddress, unsigned long long &guestPhysicalAddress, unsigned char &permissions, unsigned char &memoryType, std::shared_ptr<NestedTlbTag> &tag)
	{
		unsigned long long vpn = guestVirtualAddress >> 12;
		unsigned long long offset = guestVirtualAddress & 0xFFF;

		for (int i = 0; i < TLB_ENTRIES; i++)
		{
// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref TlbEntry entry = ref _entries[i];
			const TlbEntry& entry = _entries[i];
			if (entry.Valid && entry.IsNested && entry.VPN == vpn && entry.NestedTag->MatchesLookup(guestVirtualAddress, addressSpace))
			{
				hostPhysicalAddress = (entry.PPN << 12) | offset;
				guestPhysicalAddress = (entry.NestedGuestPPN << 12) | offset;
				permissions = entry.Permissions;
				memoryType = entry.MemoryType;
				tag = entry.NestedTag;
				setHits(getHits() + 1);
				UpdateLru(i);
				return true;
			}
		}

		hostPhysicalAddress = 0;
		guestPhysicalAddress = 0;
		permissions = 0;
		memoryType = 0;
		tag = nullptr;
		setMisses(getMisses() + 1);
		return false;
	}

	void TLB::InsertNested(unsigned long long guestVirtualAddress, unsigned long long guestPhysicalAddress, unsigned long long hostPhysicalAddress, unsigned char permissions, unsigned char memoryType, std::shared_ptr<AddressSpaceId> addressSpace)
	{
		unsigned long long vpn = guestVirtualAddress >> 12;
		unsigned long long hppn = hostPhysicalAddress >> 12;
		unsigned long long gppn = guestPhysicalAddress >> 12;
		std::shared_ptr<NestedTlbTag> tag = NestedTlbTag::Create(guestVirtualAddress, addressSpace, permissions, memoryType);

		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (_entries[i].Valid && _entries[i].IsNested && _entries[i].NestedTag->MatchesLookup(guestVirtualAddress, addressSpace))
			{
				_entries[i].PPN = hppn;
				_entries[i].NestedGuestPPN = gppn;
				_entries[i].Permissions = permissions;
				_entries[i].MemoryType = memoryType;
				_entries[i].NestedTag = tag;
				_entries[i].LruAge = 0;
				return;
			}
		}

		int victimIdx = FindVictimIndex();
		TlbEntry temp_var = TlbEntry();
		temp_var.VPN = vpn;
		temp_var.PPN = hppn;
		temp_var.Permissions = permissions;
		temp_var.MemoryType = memoryType;
		temp_var.Valid = true;
		temp_var.DomainId = addressSpace->DomainTag;
		temp_var.IsNested = true;
		temp_var.NestedTag = tag;
		temp_var.NestedGuestPPN = gppn;
		temp_var.LruAge = 0;
		_entries[victimIdx] = temp_var;
	}

	void TLB::FlushDomain(int domainId)
	{
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (!_entries[i].IsNested && _entries[i].DomainId == domainId)
			{
				_entries[i].Valid = false;
			}
		}
	}

	int TLB::FlushNestedAll()
	{
		int invalidated = 0;
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (_entries[i].Valid && _entries[i].IsNested)
			{
				_entries[i].Valid = false;
				invalidated++;
			}
		}

		return invalidated;
	}

	int TLB::FlushNestedBySecondStageRoot(unsigned long long secondStageRootIdentity)
	{
		int invalidated = 0;
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (_entries[i].Valid && _entries[i].IsNested && _entries[i].NestedTag->SecondStageRootIdentity == secondStageRootIdentity)
			{
				_entries[i].Valid = false;
				invalidated++;
			}
		}

		return invalidated;
	}

	int TLB::FlushNestedByAddressSpaceTag(unsigned short addressSpaceTag)
	{
		int invalidated = 0;
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (_entries[i].Valid && _entries[i].IsNested && _entries[i].NestedTag->AddressSpace->AddressSpaceTag == addressSpaceTag)
			{
				_entries[i].Valid = false;
				invalidated++;
			}
		}

		return invalidated;
	}

	int TLB::FlushNestedByDomainTag(unsigned short domainTag)
	{
		int invalidated = 0;
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (_entries[i].Valid && _entries[i].IsNested && _entries[i].NestedTag->AddressSpace->DomainTag == domainTag)
			{
				_entries[i].Valid = false;
				invalidated++;
			}
		}

		return invalidated;
	}

	int TLB::FlushNestedSingleAddress(std::shared_ptr<AddressSpaceId> addressSpace, unsigned long long guestVirtualAddress)
	{
		unsigned long long vpn = guestVirtualAddress >> 12;
		int invalidated = 0;
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (_entries[i].Valid && _entries[i].IsNested && _entries[i].VPN == vpn && _entries[i].NestedTag->MatchesLookup(guestVirtualAddress, addressSpace))
			{
				_entries[i].Valid = false;
				invalidated++;
			}
		}

		return invalidated;
	}

	int TLB::CountNestedEntries() const
	{
		int count = 0;
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (_entries[i].Valid && _entries[i].IsNested)
			{
				count++;
			}
		}

		return count;
	}

	void TLB::FlushAll()
	{
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			_entries[i].Valid = false;
		}

		setHits(0);
		setMisses(0);
	}

	void TLB::UpdateLru(int hitIndex)
	{
		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (_entries[i].Valid && i != hitIndex)
			{
				_entries[i].LruAge++;
			}
		}

		_entries[hitIndex].LruAge = 0;
	}

	int TLB::FindVictimIndex()
	{
		int victimIdx = 0;
		unsigned char maxAge = 0;

		for (int i = 0; i < TLB_ENTRIES; i++)
		{
			if (!_entries[i].Valid)
			{
				return i;
			}

			if (_entries[i].LruAge > maxAge)
			{
				maxAge = _entries[i].LruAge;
				victimIdx = i;
			}
		}

		return victimIdx;
	}
}
