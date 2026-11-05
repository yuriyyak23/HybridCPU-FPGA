#pragma once

#include <string>
#include <vector>
#include <limits>
#include <format>
#include <memory>
#include <utility>

namespace YAKSys_Hybrid_CPU::Core::Pipeline::Metadata
{
	struct AssistCoalescingDescriptor final {
		const unsigned long long BaseAddress, PrefetchLength; const int SourceRangeCount, NormalizedRangeCount; const unsigned long long LargestGapBytes;
		static const AssistCoalescingDescriptor None;
		AssistCoalescingDescriptor(unsigned long long baseAddress=0,unsigned long long prefetchLength=0,int sourceCount=0,int normalizedCount=0,unsigned long long largestGap=0):BaseAddress(baseAddress),PrefetchLength(prefetchLength),SourceRangeCount(sourceCount),NormalizedRangeCount(normalizedCount),LargestGapBytes(largestGap){}
		bool getIsValid() const{return SourceRangeCount>1&&PrefetchLength>0;}
		bool getIsNearContiguous() const{return getIsValid()&&LargestGapBytes>0;}
	};// C# TO C++ CONVERTER TASK: C# 'records' are not converted by C# to C++ Converter:
//	public readonly record struct AssistCoalescingDescriptor(ulong BaseAddress, ulong PrefetchLength, int SourceRangeCount, int NormalizedRangeCount, ulong LargestGapBytes)
//	{
//		public static AssistCoalescingDescriptor None
//		{
//			get;
//		} = new(BaseAddress: 0, PrefetchLength: 0, SourceRangeCount: 0, NormalizedRangeCount: 0, LargestGapBytes: 0);
//
//		public bool IsValid
//		{
//			get
//			{
//				return SourceRangeCount > 1 && PrefetchLength > 0;
//			}
//		}
//
//		public bool IsNearContiguous
//		{
//			get
//			{
//				return IsValid && LargestGapBytes > 0;
//			}
//		}
//	}

	inline const AssistCoalescingDescriptor AssistCoalescingDescriptor::None{0,0,0,0,0};
class ReadRangeMetadataHelper final
	{
	private:
		static constexpr unsigned long long AssistNearContiguousGapBytes = 32;

	public:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static bool TryNormalizeContiguousReadRanges(IReadOnlyList<(ulong Address, ulong Length)> readMemoryRanges, out IReadOnlyList<(ulong Address, ulong Length)> normalizedReadMemoryRanges)
		static bool TryNormalizeContiguousReadRanges(const std::vector<std::pair<unsigned long long, unsigned long long>> &readMemoryRanges, const std::vector<std::pair<unsigned long long, unsigned long long>> &normalizedReadMemoryRanges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static AssistCoalescingDescriptor BuildAssistCoalescingDescriptor(IReadOnlyList<(ulong Address, ulong Length)> readMemoryRanges, IReadOnlyList<(ulong Address, ulong Length)> normalizedReadMemoryRanges)
		static std::shared_ptr<AssistCoalescingDescriptor> BuildAssistCoalescingDescriptor(const std::vector<std::pair<unsigned long long, unsigned long long>> &readMemoryRanges, const std::vector<std::pair<unsigned long long, unsigned long long>> &normalizedReadMemoryRanges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: internal static void ValidateCoalescedRangeMetadata(IReadOnlyList<(ulong Address, ulong Length)> rawReadMemoryRanges, IReadOnlyList<(ulong Address, ulong Length)> normalizedReadMemoryRanges, AssistCoalescingDescriptor descriptor)
		static void ValidateCoalescedRangeMetadata(const std::vector<std::pair<unsigned long long, unsigned long long>> &rawReadMemoryRanges, const std::vector<std::pair<unsigned long long, unsigned long long>> &normalizedReadMemoryRanges, std::shared_ptr<AssistCoalescingDescriptor> descriptor);

// C# TO C++ CONVERTER TASK: Methods returning tuples are not converted by C# to C++ Converter:
//		private static List<(ulong Address, ulong Length)> CollectOrderedNonEmptyRanges(IReadOnlyList<(ulong Address, ulong Length)> ranges)
//		{
//			List<(ulong Address, ulong Length)> orderedRanges = new List<(ulong Address, ulong Length)>((ranges == nullptr ? nullptr : ranges.Count) ?? 0);
//			if (ranges == nullptr)
//			{
//				return orderedRanges;
//			}
//
//			for (int index = 0; index < ranges.Count; index++)
//			{
//				(ulong address, ulong length) = ranges[index];
//				if (length == 0)
//				{
//					continue;
//				}
//
//				ulong end = SaturatingAdd(address, length);
//				orderedRanges.Add((address, end - address));
//			}
//
//			orderedRanges.Sort(static (left, right) => left.Address.CompareTo(right.Address));
//			return orderedRanges;
//		}

	private:
// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static bool HasSameRangeSequence(IReadOnlyList<(ulong Address, ulong Length)> orderedRanges, IReadOnlyList<(ulong Address, ulong Length)> rawReadMemoryRanges)
		static bool HasSameRangeSequence(const std::vector<std::pair<unsigned long long, unsigned long long>> &orderedRanges, const std::vector<std::pair<unsigned long long, unsigned long long>> &rawReadMemoryRanges);

// C# TO C++ CONVERTER WARNING: C++ has no equivalent to a 'readonly' collection which allows modification of internal state:
// ORIGINAL LINE: private static void ValidateRangesStayInsideWindow(IReadOnlyList<(ulong Address, ulong Length)> ranges, ulong windowStart, ulong windowEnd, string paramName)
		static void ValidateRangesStayInsideWindow(const std::vector<std::pair<unsigned long long, unsigned long long>> &ranges, unsigned long long windowStart, unsigned long long windowEnd, const std::wstring &paramName);

		static unsigned long long SaturatingAdd(unsigned long long address, unsigned long long length);
	};
}


