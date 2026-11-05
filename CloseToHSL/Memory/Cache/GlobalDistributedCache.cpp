#include "GlobalDistributedCache.h"

namespace HybridCPU_ISE::CloseToHSL::Memory::Cache
{

	long long GlobalDistributedCache::getTotalLookups() const
	{
		return TotalLookups;
	}

	void GlobalDistributedCache::setTotalLookups(long long value)
	{
		TotalLookups = value;
	}

	long long GlobalDistributedCache::getHits() const
	{
		return Hits;
	}

	void GlobalDistributedCache::setHits(long long value)
	{
		Hits = value;
	}

	long long GlobalDistributedCache::getMisses() const
	{
		return Misses;
	}

	void GlobalDistributedCache::setMisses(long long value)
	{
		Misses = value;
	}

	GlobalDistributedCache::GlobalDistributedCache() : _lines(std::vector<L3Line>(TOTAL_LINES))
	{
		for (int i = 0; i < TOTAL_LINES; i++)
		{
			L3Line temp_var = L3Line();
			temp_var.Tag = 0;
			temp_var.Valid = false;
			temp_var.DomainTag = 0;
			temp_var.LastAccessCycle = 0;
			_lines[i] = temp_var;
		}
	}

	inline int GlobalDistributedCache::GetHomeSlice(unsigned long long address)
	{
		return static_cast<int>((address >> OFFSET_BITS) % NUM_SLICES);
	}

	inline bool GlobalDistributedCache::Lookup(unsigned long long address)
	{
		setTotalLookups(getTotalLookups() + 1);
		int lineIndex = GetLineIndex(address);
		unsigned long long tag = GetTag(address);

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref L3Line line = ref _lines[lineIndex];
		L3Line &line = _lines[lineIndex];
		if (line.Valid && line.Tag == tag)
		{
			setHits(getHits() + 1);
			line.LastAccessCycle = _currentCycle;
			return true;
		}

		setMisses(getMisses() + 1);
		return false;
	}

	inline void GlobalDistributedCache::Fill(unsigned long long address, unsigned long long domainTag)
	{
		int lineIndex = GetLineIndex(address);
		unsigned long long tag = GetTag(address);

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref L3Line line = ref _lines[lineIndex];
		L3Line &line = _lines[lineIndex];
		line.Tag = tag;
		line.Valid = true;
		line.DomainTag = domainTag;
		line.LastAccessCycle = _currentCycle;
	}

	inline void GlobalDistributedCache::Invalidate(unsigned long long address)
	{
		int lineIndex = GetLineIndex(address);
		unsigned long long tag = GetTag(address);

// C# TO C++ CONVERTER TASK: 'ref locals' are not converted by C# to C++ Converter:
// ORIGINAL LINE: ref L3Line line = ref _lines[lineIndex];
		L3Line &line = _lines[lineIndex];
		if (line.Valid && line.Tag == tag)
		{
			line.Valid = false;
		}
	}

	void GlobalDistributedCache::AdvanceCycle()
	{
		_currentCycle++;
	}

	void GlobalDistributedCache::Reset()
	{
		for (int i = 0; i < TOTAL_LINES; i++)
		{
			_lines[i].Valid = false;
			_lines[i].Tag = 0;
			_lines[i].DomainTag = 0;
		}

		setTotalLookups(0);
		setHits(0);
		setMisses(0);
	}

	inline int GlobalDistributedCache::GetLineIndex(unsigned long long address)
	{
		return static_cast<int>((address >> OFFSET_BITS) % TOTAL_LINES);
	}

	inline unsigned long long GlobalDistributedCache::GetTag(unsigned long long address)
	{
		return address >> (OFFSET_BITS + 12); // 12 = log2(TOTAL_LINES)
	}
}
