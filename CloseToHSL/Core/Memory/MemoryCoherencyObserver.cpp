#include "MemoryCoherencyObserver.h"

#include "../Architecture/MemoryModel/Addressing/MemoryRangeOverlap.h"

using namespace YAKSys_Hybrid_CPU::Memory;

namespace YAKSys_Hybrid_CPU::Core::Memory
{

	MemoryCoherencyCacheParticipant::MemoryCoherencyCacheParticipant(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l1Data, std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l2Data)
	{
		_l1Data = l1Data;
		_l2Data = l2Data;
	}

	std::shared_ptr<DataCacheRangeInvalidationResult> MemoryCoherencyCacheParticipant::InvalidateRange(unsigned long long address, unsigned long long length, unsigned long long domainTag) const
	{
		return InvalidateArrays(_l1Data, _l2Data, address, length, domainTag);
	}

	std::shared_ptr<DataCacheRangeFlushResult> MemoryCoherencyCacheParticipant::FlushRange(unsigned long long address, unsigned long long length, unsigned long long domainTag) const
	{
		return FlushArrays(_l1Data, _l2Data, address, length, domainTag);
	}

	std::shared_ptr<DataCacheRangeInvalidationResult> MemoryCoherencyCacheParticipant::InvalidateArrays(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l1Data, std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l2Data, unsigned long long address, unsigned long long length, unsigned long long domainTag)
	{
		if (!MemoryRangeOverlap::IsNonEmptyWellFormed(address, length))
		{
			return std::make_shared<DataCacheRangeInvalidationResult>(false, 0, 0, 0);
		}

		int assistInvalidated = 0;
		int l1Invalidated = InvalidateArray(l1Data, address, length, domainTag, assistInvalidated);
		int l2Invalidated = InvalidateArray(l2Data, address, length, domainTag, assistInvalidated);

		return std::make_shared<DataCacheRangeInvalidationResult>(true, l1Invalidated, l2Invalidated, assistInvalidated);
	}

	std::shared_ptr<DataCacheRangeFlushResult> MemoryCoherencyCacheParticipant::FlushArrays(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l1Data, std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l2Data, unsigned long long address, unsigned long long length, unsigned long long domainTag)
	{
		if (!MemoryRangeOverlap::IsNonEmptyWellFormed(address, length))
		{
			return std::make_shared<DataCacheRangeFlushResult>(false, false, false, 0, 0, 0);
		}

		int cleanLines = 0;
		int l1Dirty = CountOverlappingDirtyLines(l1Data, address, length, domainTag, cleanLines);
		int l2Dirty = CountOverlappingDirtyLines(l2Data, address, length, domainTag, cleanLines);
		bool hasDirtyLines = l1Dirty != 0 || l2Dirty != 0;

		return std::make_shared<DataCacheRangeFlushResult>(true, !hasDirtyLines, hasDirtyLines, cleanLines, l1Dirty, l2Dirty);
	}

	int MemoryCoherencyCacheParticipant::InvalidateArray(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> cache, unsigned long long address, unsigned long long length, unsigned long long domainTag, int &assistInvalidated)
	{
		if (cache == nullptr)
		{
			return 0;
		}

		int invalidated = 0;
		for (int index = 0; index < cache->size(); index++)
		{
			Processor::CPU_Core::Cache_Data_Object line = (*cache)[index];
			if (!IsLiveLine(line) || !DomainMatches(line.DomainTag, domainTag) || !MemoryRangeOverlap::RangesOverlap(address, length, line.DataCache_MemoryAddress, line.DataCache_DataLenght))
			{
				continue;
			}

			if (line.AssistResident)
			{
				assistInvalidated++;
			}

			(*cache)[index] = Processor::CPU_Core::Cache_Data_Object{};
			invalidated++;
		}

		return invalidated;
	}

	int MemoryCoherencyCacheParticipant::CountOverlappingDirtyLines(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> cache, unsigned long long address, unsigned long long length, unsigned long long domainTag, int &cleanLines)
	{
		if (cache == nullptr)
		{
			return 0;
		}

		int dirtyLines = 0;
		for (int index = 0; index < cache->size(); index++)
		{
			Processor::CPU_Core::Cache_Data_Object line = (*cache)[index];
			if (!IsLiveLine(line) || !DomainMatches(line.DomainTag, domainTag) || !MemoryRangeOverlap::RangesOverlap(address, length, line.DataCache_MemoryAddress, line.DataCache_DataLenght))
			{
				continue;
			}

			if (line.DataCache_IsDirty)
			{
				dirtyLines++;
			}
			else
			{
				cleanLines++;
			}
		}

		return dirtyLines;
	}

	bool MemoryCoherencyCacheParticipant::IsLiveLine(Processor::CPU_Core::Cache_Data_Object line)
	{
// C# TO C++ CONVERTER TASK: The following 'is' operator pattern is not converted by C# to C++ Converter:
		return line.DataCache_DataLenght != 0 && !line.DataCache_StoredValue.empty();
	}

	bool MemoryCoherencyCacheParticipant::DomainMatches(unsigned long long lineDomainTag, unsigned long long notificationDomainTag)
	{
		return notificationDomainTag == 0 || lineDomainTag == 0 || lineDomainTag == notificationDomainTag;
	}

	std::shared_ptr<MemoryCoherencyObserver> MemoryCoherencyObserver::RegisterDataCache(std::shared_ptr<Processor::CPU_Core> core)
	{
		if (!core) throw std::invalid_argument("core");
		auto [l1, l2] = core->GetDataCacheArrays();
		_cacheParticipants.push_back(MemoryCoherencyCacheParticipant(l1, l2));
		return shared_from_this();
	}

	std::shared_ptr<MemoryCoherencyObserver> MemoryCoherencyObserver::RegisterDataCache(std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l1Data, std::shared_ptr<std::vector<Processor::CPU_Core::Cache_Data_Object>> l2Data)
	{
		_cacheParticipants.push_back(MemoryCoherencyCacheParticipant(l1Data, l2Data));
		return shared_from_this();
	}

	std::shared_ptr<MemoryCoherencyObserver> MemoryCoherencyObserver::RegisterStreamRegisterFile(std::shared_ptr<StreamRegisterFile> streamRegisterFile)
	{
		if (!streamRegisterFile) throw std::invalid_argument("streamRegisterFile");
		_streamRegisterFiles.push_back(streamRegisterFile);
		return shared_from_this();
	}

	std::shared_ptr<MemoryCoherencyObserverResult> MemoryCoherencyObserver::NotifyWrite(std::shared_ptr<MemoryCoherencyWriteNotification> notification)
	{
		if (!notification->isWellFormed())
		{
			return std::make_shared<MemoryCoherencyObserverResult>(false, 0, 0, 0);
		}

		int dataInvalidated = 0;
		int assistInvalidated = 0;
		for (int index = 0; index < _cacheParticipants.size(); index++)
		{
			std::shared_ptr<DataCacheRangeInvalidationResult> result = _cacheParticipants[index].InvalidateRange(notification->Address, notification->Length, notification->DomainTag);
			dataInvalidated += result->getTotalLinesInvalidated();
			assistInvalidated += result->AssistResidentLinesInvalidated;
		}

		int srfInvalidated = 0;
		for (int index = 0; index < _streamRegisterFiles.size(); index++)
		{
			srfInvalidated += _streamRegisterFiles[index]->InvalidateOverlappingRangeAndCount(notification->Address, notification->Length);
		}

		return std::make_shared<MemoryCoherencyObserverResult>(true, dataInvalidated, assistInvalidated, srfInvalidated);
	}

	std::shared_ptr<DataCacheRangeFlushResult> MemoryCoherencyObserver::FlushBeforeExternalRead(unsigned long long address, unsigned long long length, unsigned long long domainTag)
	{
		if (!MemoryRangeOverlap::IsNonEmptyWellFormed(address, length))
		{
			return std::make_shared<DataCacheRangeFlushResult>(false, false, false, 0, 0, 0);
		}

		DataCacheRangeFlushResult aggregate = DataCacheRangeFlushResult::EmptyNoOpProof;
		for (int index = 0; index < _cacheParticipants.size(); index++)
		{
			aggregate = aggregate.Merge(*_cacheParticipants[index].FlushRange(address, length, domainTag));
		}

		return std::make_shared<DataCacheRangeFlushResult>(aggregate);
	}
}
