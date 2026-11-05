#include "ReadRangeMetadataHelper.h"

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata
{

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static bool TryNormalizeContiguousReadRanges(IReadOnlyList<(ulong Address, ulong Length)> readMemoryRanges, out IReadOnlyList<(ulong Address, ulong Length)> normalizedReadMemoryRanges)
	bool ReadRangeMetadataHelper::TryNormalizeContiguousReadRanges(const std::vector<std::pair<unsigned long long, unsigned long long>> &readMemoryRanges, const std::vector<std::pair<unsigned long long, unsigned long long>> &normalizedReadMemoryRanges)
	{
		normalizedReadMemoryRanges = readMemoryRanges ? readMemoryRanges : Array::Empty<(unsigned long long Address, unsigned long long Length)>();
		if (readMemoryRanges.empty() || readMemoryRanges.size() <= 1)
		{
			return false;
		}

		std::vector<std::pair<unsigned long long, unsigned long long>> sortedRanges = CollectOrderedNonEmptyRanges(readMemoryRanges);
		if (sortedRanges.size() <= 1)
		{
			normalizedReadMemoryRanges = sortedRanges.empty() ? Array::Empty<(unsigned long long Address, unsigned long long Length)>() : new (unsigned long long Address, unsigned long long Length)[] {sortedRanges[0]};
			return readMemoryRanges.size() != sortedRanges.size();
		}

		std::vector<std::pair<unsigned long long, unsigned long long>> mergedRanges(sortedRanges.size());
		unsigned long long currentStart = sortedRanges[0]->Address;
		unsigned long long currentEnd = SaturatingAdd(sortedRanges[0]->Address, sortedRanges[0]->Length);

		for (int index = 1; index < sortedRanges.size(); index++)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
			(ulong nextStart, ulong nextLength) = sortedRanges[index];
			unsigned long long nextEnd = SaturatingAdd(nextStart, nextLength);
			if (nextStart <= currentEnd)
			{
				if (nextEnd > currentEnd)
				{
					currentEnd = nextEnd;
				}

				continue;
			}

			mergedRanges.push_back((currentStart, currentEnd - currentStart));
			currentStart = nextStart;
			currentEnd = nextEnd;
		}

		mergedRanges.push_back((currentStart, currentEnd - currentStart));
		if (mergedRanges.size() == sortedRanges.size() && HasSameRangeSequence(sortedRanges, readMemoryRanges))
		{
			return false;
		}

		normalizedReadMemoryRanges = mergedRanges.ToArray();
		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static AssistCoalescingDescriptor BuildAssistCoalescingDescriptor(IReadOnlyList<(ulong Address, ulong Length)> readMemoryRanges, IReadOnlyList<(ulong Address, ulong Length)> normalizedReadMemoryRanges)
	std::shared_ptr<AssistCoalescingDescriptor> ReadRangeMetadataHelper::BuildAssistCoalescingDescriptor(const std::vector<std::pair<unsigned long long, unsigned long long>> &readMemoryRanges, const std::vector<std::pair<unsigned long long, unsigned long long>> &normalizedReadMemoryRanges)
	{
		if (readMemoryRanges.empty() || readMemoryRanges.size() <= 1)
		{
			return AssistCoalescingDescriptor::None;
		}

		std::vector<std::pair<unsigned long long, unsigned long long>> sortedRanges = CollectOrderedNonEmptyRanges(readMemoryRanges);
		if (sortedRanges.size() <= 1)
		{
			return AssistCoalescingDescriptor::None;
		}

		unsigned long long windowStart = sortedRanges[0]->Address;
		unsigned long long windowEnd = SaturatingAdd(sortedRanges[0]->Address, sortedRanges[0]->Length);
		unsigned long long largestGapBytes = 0;

		for (int index = 1; index < sortedRanges.size(); index++)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
			(ulong nextStart, ulong nextLength) = sortedRanges[index];
			unsigned long long nextEnd = SaturatingAdd(nextStart, nextLength);
			unsigned long long gapBytes = nextStart > windowEnd ? nextStart - windowEnd : 0;
			if (gapBytes > AssistNearContiguousGapBytes)
			{
				return AssistCoalescingDescriptor::None;
			}

			if (gapBytes > largestGapBytes)
			{
				largestGapBytes = gapBytes;
			}

			if (nextEnd > windowEnd)
			{
				windowEnd = nextEnd;
			}
		}

		int normalizedRangeCount = normalizedReadMemoryRanges.empty() || normalizedReadMemoryRanges.empty() ? sortedRanges.size() : normalizedReadMemoryRanges.size();
		return std::make_shared<AssistCoalescingDescriptor>(BaseAddress: windowStart, PrefetchLength: windowEnd - windowStart, SourceRangeCount: sortedRanges.size(), NormalizedRangeCount: normalizedRangeCount, LargestGapBytes: largestGapBytes);
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static void ValidateCoalescedRangeMetadata(IReadOnlyList<(ulong Address, ulong Length)> rawReadMemoryRanges, IReadOnlyList<(ulong Address, ulong Length)> normalizedReadMemoryRanges, AssistCoalescingDescriptor descriptor)
	void ReadRangeMetadataHelper::ValidateCoalescedRangeMetadata(const std::vector<std::pair<unsigned long long, unsigned long long>> &rawReadMemoryRanges, const std::vector<std::pair<unsigned long long, unsigned long long>> &normalizedReadMemoryRanges, std::shared_ptr<AssistCoalescingDescriptor> descriptor)
	{
		if (!descriptor->IsValid)
		{
			return;
		}

		unsigned long long descriptorEnd = SaturatingAdd(descriptor->BaseAddress, descriptor->PrefetchLength);
		ValidateRangesStayInsideWindow(rawReadMemoryRanges, descriptor->BaseAddress, descriptorEnd, L"rawReadMemoryRanges");
		ValidateRangesStayInsideWindow(normalizedReadMemoryRanges, descriptor->BaseAddress, descriptorEnd, L"normalizedReadMemoryRanges");
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool HasSameRangeSequence(IReadOnlyList<(ulong Address, ulong Length)> orderedRanges, IReadOnlyList<(ulong Address, ulong Length)> rawReadMemoryRanges)
	bool ReadRangeMetadataHelper::HasSameRangeSequence(const std::vector<std::pair<unsigned long long, unsigned long long>> &orderedRanges, const std::vector<std::pair<unsigned long long, unsigned long long>> &rawReadMemoryRanges)
	{
		if (orderedRanges.empty() || rawReadMemoryRanges.empty() || orderedRanges.size() != rawReadMemoryRanges.size())
		{
			return false;
		}

		for (int index = 0; index < orderedRanges.size(); index++)
		{
			if (orderedRanges[index] != rawReadMemoryRanges[index])
			{
				return false;
			}
		}

		return true;
	}

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ValidateRangesStayInsideWindow(IReadOnlyList<(ulong Address, ulong Length)> ranges, ulong windowStart, ulong windowEnd, string paramName)
	void ReadRangeMetadataHelper::ValidateRangesStayInsideWindow(const std::vector<std::pair<unsigned long long, unsigned long long>> &ranges, unsigned long long windowStart, unsigned long long windowEnd, const std::wstring &paramName)
	{
		if (ranges.empty())
		{
			return;
		}

		for (int index = 0; index < ranges.size(); index++)
		{
// C# TO C++ CONVERTER TASK: C++ has no equivalent to C# deconstruction declarations:
			(ulong address, ulong length) = ranges[index];
			if (length == 0)
			{
				continue;
			}

			unsigned long long end = SaturatingAdd(address, length);
			if (address < windowStart || end > windowEnd)
			{
				throw InvalidOperationException(std::format("Coalesced read-range metadata escaped its advisory window while validating {0:s}.", paramName));
			}
		}
	}

	unsigned long long ReadRangeMetadataHelper::SaturatingAdd(unsigned long long address, unsigned long long length)
	{
		unsigned long long remaining = std::numeric_limits<unsigned long long>::max() - address;
		return length > remaining ? std::numeric_limits<unsigned long long>::max() : address + length;
	}
}

